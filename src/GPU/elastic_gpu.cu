#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned char uint8;

using namespace std;

#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

#define MIN(a, b) ((a)<(b)?(a):(b))

#define OFFSET(a, b, c) ((a)*LEN1*3+(b)*3+(c)) 

inline
cudaError_t checkCuda(cudaError_t result)
{
#if defined(DEBUG) || defined(_DEBUG)
    if (result != cudaSuccess) {
      fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
      assert(result == cudaSuccess);
    }
#endif
    return result;
}

__host__ __device__
uint32 hash1(uint32 key) {
    return (key * 2654435761u) >> 15;
}

__host__ __device__
uint32 hash2(uint32 key) {
        //register ub4 a,b,c,len;
    uint32 a,b,c;
    uint32 len = 4;
    char* str = (char*)&key;
    //  uint32_t initval = 0;
    /* Set up the internal state */
    //len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = 8311;//prime32[1000];         /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12)
    {
        a += (str[0] +((uint32)str[1]<<8) +((uint32)str[2]<<16) +((uint32)str[3]<<24));
        b += (str[4] +((uint32)str[5]<<8) +((uint32)str[6]<<16) +((uint32)str[7]<<24));
        c += (str[8] +((uint32)str[9]<<8) +((uint32)str[10]<<16)+((uint32)str[11]<<24));
        mix(a,b,c);
        str += 12; len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += len;
    switch(len)              /* all the case statements fall through */
    {
        case 11: c+=((uint32)str[10]<<24);
        // fall through
        case 10: c+=((uint32)str[9]<<16);
        // fall through
        case 9 : c+=((uint32)str[8]<<8);
        /* the first byte of c is reserved for the length */
        // fall through
        case 8 : b+=((uint32)str[7]<<24);
        // fall through
        case 7 : b+=((uint32)str[6]<<16);
        // fall through
        case 6 : b+=((uint32)str[5]<<8);
        // fall through
        case 5 : b+=str[4];
        // fall through
        case 4 : a+=((uint32)str[3]<<24);
        // fall through
        case 3 : a+=((uint32)str[2]<<16);
        // fall through
        case 2 : a+=((uint32)str[1]<<8);
        // fall through
        case 1 : a+=str[0];
        /* case 0: nothing left to add */
    }
    mix(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}

const int LEN1 = 3200;
const int LEN2 = 460800;
const int BLOCK_SIZE = 32;

__device__ uint32 kernel_heavy[4 * LEN1 * 3];
__device__ uint32 kernel_light[LEN2];
__device__ uint32 mutex[4 * LEN1];

uint32 heavy[4 * LEN1 * 3];
uint32 light[LEN2];

uint32* read_keys(char* filename, uint32* len) {
    FILE* fp = fopen(filename, "rb");
    fseek(fp, 0L, SEEK_END);
    uint32 size = ftell(fp);
    rewind(fp);
    if (size % 13) {
        printf("wrong file size: %d\n", size);
        fclose(fp);
        return NULL;
    }
    *len = size / 13;
    uint32* keys;
    checkCuda( cudaMallocHost((void**)&keys, sizeof(uint32) * (*len)) );
    for (int i = 0; i < *len; ++i) {
        char tmp[13];
        fread(tmp, 13, sizeof(char), fp);
        *(keys+i) = *(uint32*)tmp;
    }
    fclose(fp);
    return keys;
}

__global__ void kernel_init() {
    for (int i = 0; i < 4 * LEN1 * 3; ++i)
        kernel_heavy[i] = 0;

    for (int i = 0; i < LEN2; ++i)
        kernel_light[i] = 0;

    for (int i = 0; i < LEN1 * 4; ++i)
        mutex[i] = 0;
}

__device__ inline bool insert_bucket(uint32 offset, uint32* key, uint32* val) {


    uint32* bucket = kernel_heavy + offset;

    bool res;

    bool next = true;
    int cnt = 10;
    while(next && cnt--)
    {

        int v = atomicCAS(mutex + offset / 3, 0, 1);
        if (v == 0)
        {

            if (bucket[0] == 0) {
                bucket[0] = *val;
                bucket[1] = *key;
                res = false;
            }
            else if (bucket[1] == *key) {
                bucket[0] = ((bucket[0] + *val) | (((bucket[0] | *val) >> 31) << 31));
                res = false;
            } else {
                bucket[2] += ((*val<<1)>>1);
                if (bucket[2] < (bucket[0] << 3)) {
                    res = true;
                } else {
                    bucket[2] = 0;
                    uint32 t_key = bucket[1];
                    uint32 t_val = bucket[0];
                    bucket[1] = *key;
                    bucket[0] = ((1u<<31) | *val);
                    *key = t_key;
                    *val = t_val;
                    res = true;
                }
            }

            atomicExch(mutex + offset / 3, 0);
            next = false;
        }
    }
    return res;
}

__device__ bool insert_heavy(uint32* key, uint32* val) {
    uint32 hash_val = hash1(*key);
    for (int i = 0; i < 4; ++i) {
        uint32 pos = hash_val % (LEN1 - i);
        if (insert_bucket(OFFSET(i, pos, 0), key, val) == false)
            return false;
    }
    return true;
}

__device__ inline void insert_light(uint32 key, uint32 val) {
    int pos = hash2(key) % LEN2;
    atomicAdd(kernel_light + pos, val);
}

__global__ void kernel_insert(uint32* keys, uint32 len) {
    const uint32 id = threadIdx.x + blockIdx.x * blockDim.x;
    if (id >= len) return;
    uint32 key = keys[id];
    uint32 val = 1;
    if (insert_heavy(&key, &val))
        insert_light(key, val);
}

double expr_result[10];
int expr_pos = 0;

void insert(uint32* keys, uint32 len) {
    uint32* kernel_keys;
    /**/checkCuda( cudaMalloc((void**)&kernel_keys, sizeof(uint32) * len) );
    //checkCuda( cudaMalloc((void**)&kernel_keys, sizeof(uint32) * BATCH_SIZE) );

    cudaEvent_t startEvent, stopEvent;
    checkCuda( cudaEventCreate(&startEvent) );
    checkCuda( cudaEventCreate(&stopEvent) );

    int batch_num = (len + BATCH_SIZE - 1) / BATCH_SIZE;
    cudaStream_t stream[batch_num];
    for (int i = 0; i < batch_num; ++i)
        checkCuda( cudaStreamCreate(&stream[i]) );


    checkCuda( cudaEventRecord(startEvent, 0) );

    for (uint32 i = 0, left = len; i < len; i += BATCH_SIZE, left -= BATCH_SIZE) {
        uint32 size = MIN(left, BATCH_SIZE);
        /**/checkCuda( cudaMemcpyAsync(kernel_keys + i, keys + i, sizeof(uint32) * size, cudaMemcpyHostToDevice, stream[i % BATCH_SIZE]) );
        /**/kernel_insert<<<(size + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE, 0, stream[i % BATCH_SIZE]>>>(kernel_keys + i, size);
        //checkCuda( cudaMemcpy(kernel_keys, keys + i, sizeof(uint32) * size, cudaMemcpyHostToDevice) );
        //kernel_insert<<<(size + BLOCK_SIZE - 1) / BLOCK_SIZE, BLOCK_SIZE>>>(kernel_keys, size);
    }

    checkCuda( cudaEventRecord(stopEvent, 0) );
    checkCuda( cudaEventSynchronize(stopEvent) );
    float ms;
    checkCuda( cudaEventElapsedTime(&ms, startEvent, stopEvent) );
    //printf("%f\n", len / ms / 1000);
    /**/expr_result[expr_pos++] = len / ms / 1000;
    //expr_result[expr_pos++] = ms / batch_num * 1000;
    cudaFree(kernel_keys);
}

uint32* true_result(uint32* keys, uint32* len) {
    unordered_map<uint32, uint32> s;
    for (int i = 0; i < *len; ++i) ++s[keys[i]];
    *len = s.size();
    uint32* res = (uint32*)malloc(sizeof(uint32) * (*len));
    int pos = 0;
    for (auto p: s) {
        keys[pos] = p.first;
        res[pos] = p.second;
        ++pos;
    }
    return res;
}

inline bool query_bucket(uint32* bucket, uint32 key, uint32* res) {
    if (bucket[1] != key) return false;
    *res += ((bucket[0] << 1) >> 1);
    return (bucket[0] >> 31) == 1;
}

bool query_heavy(uint32* res, uint32 key) {
    uint32 hash_val = hash1(key);
    *res = 0;
    bool should_query_light = false;
    for (int i = 0; i < 4; ++i) {
        uint32 pos = hash_val % (LEN1 - i);
        bool t = query_bucket(heavy + OFFSET(i, pos, 0), key, res);
        should_query_light = (should_query_light || t);
    }
    return should_query_light;
}

void query_light(uint32* res, uint32 key) {
    uint32 pos = hash2(key) % LEN2;
    *res += (light[pos] & 0xff);
}

uint32* query(uint32* keys, uint32 len) {
    uint32* res = (uint32*)malloc(sizeof(uint32) * len);
    checkCuda( cudaMemcpyFromSymbol(heavy, kernel_heavy, 4 * LEN1 * 3 * sizeof(uint32)) );
    checkCuda( cudaMemcpyFromSymbol(light, kernel_light, LEN2 * sizeof(uint32)) );
    for (int i = 0; i < len; ++i)
        if (query_heavy(res + i, keys[i]) || res[i] == 0)
            query_light(res + i, keys[i]);
    return res;
}

void compare(uint32* real, uint32* my, uint32 len) {
    int accuracy = 0;
    double are = 0;
    for (int i = 0; i < len; ++i) {
        accuracy += (real[i] == my[i]);
        are += ((real[i] > my[i]) ? (real[i] - my[i]) : (my[i] - real[i])) / (double)real[i];
    }
    //printf("%lf %lf\n", accuracy / (double)len, are / len);
}

void experiment_filename(char* filename) {
    uint32 len;

    uint32* keys = read_keys(filename, &len);
    kernel_init<<<1, 1>>>();
    insert(keys, len);
    uint32* true_res = true_result(keys, &len);
    uint32* my_res = query(keys, len);
    compare(true_res, my_res, len);

    free(my_res);
    cudaFreeHost(keys);
    free(true_res);
}

int main() {
    char filename[100];
    printf("%d ", BATCH_SIZE);
    for (int i = 0; i < 10; ++i) {
        sprintf(filename, "../../data/%d.dat", i);
        experiment_filename(filename);
    }
    sort(expr_result, expr_result + 10);
    printf("%lf %lf %lf\n", 
        (expr_result[0] + expr_result[1]) / 2,
        (expr_result[4] + expr_result[5]) / 2,
        (expr_result[8] + expr_result[9]) / 2
    );
    return 0;
}