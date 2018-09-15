#ifndef _COUNT_SKETCH_H_VERSION_2
#define _COUNT_SKETCH_H_VERSION_2

#include <cstring>
#include <algorithm>
#include <cstdint>
#include <vector>

#include "../common/BOBHash32.h"

using namespace std;

template<uint32_t keylen, int d>
class CountSketch
{
private:
	int memory_in_bytes = 0;

	int w = 0;
	int* counters[d] = {NULL};

	BOBHash32* hash[d] = {NULL};
	BOBHash32* sign_hash[d] = {NULL};
public:

	CountSketch(){}
	CountSketch(int memory_in_bytes)
	{	initial(memory_in_bytes);	}
	~CountSketch(){clear();}

	void initial(int memory_in_bytes)
	{
		this->memory_in_bytes = memory_in_bytes;

		w = memory_in_bytes / 4 / d;

		srand(time(0));
		for (int i = 0; i < d; ++i)
		{
			counters[i] = new int[w];
			hash[i] = new BOBHash32(rand() % MAX_PRIME32);
			sign_hash[i] = new BOBHash32(rand() % MAX_PRIME32);

			memset(counters[i], 0, sizeof(int) * w);
		}
		
	}
	void clear()
	{
		for (int i = 0; i < d; ++i)
		{
			delete[] counters[i];
			delete hash[i];
			delete sign_hash[i];
		}
	}

	void print_basic_info()
    {
        printf("Count sketch\n");
        printf("\tCounters: %d\n", w);
        printf("\tMemory: %.6lfMB\n", w * d * 4.0 / 1024 / 1024);
    }

    void insert(uint8_t *key, int f = 1)
    {
    	for (int i = 0; i < d; ++i)
    	{
    		int index = (hash[i]->run((const char*)key, keylen)) % w;
    		int sign = (sign_hash[i]->run((const char*)key, keylen)) % 2;

    		counters[i][index] += sign ? f : -f;
    	}
    }

    int query(uint8_t *key)
    {
    	int result[d] = {0};
    	for (int i = 0; i < d; ++i)
    	{
			int index = (hash[i]->run((const char*)key, keylen)) % w;
    		int sign = (sign_hash[i]->run((const char*)key, keylen)) % 2;

    		result[i] = sign ? counters[i][index] : -counters[i][index];
    	}

    	sort(result, result + d);

    	int mid = d / 2;
    	int ret;
    	if(d % 2 == 0)
    		ret = (result[mid] + result[mid - 1]) / 2;
    	else
    		ret = result[mid];

    	return ret;
    }
};






#endif