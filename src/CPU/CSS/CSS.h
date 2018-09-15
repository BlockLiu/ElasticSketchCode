#ifndef _CSS_H
#define _CSS_H
 
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdint.h>
#include <map>
#include "../common/BOBHash32.h"
using namespace std;


template<int key_length, int max_K>
class CSS
{
private:
    int K;		// need calculation

    uint32_t CounterArr[max_K];
    int Reverse_CA_2_IDidx[max_K];		// reverse index from CounterArray to ID index

    struct Key_CAidx_pair{
        uint8_t key[key_length];
        int CounterArrayIndex;
    } ID_index[max_K];


    map<uint32_t, int> Value_index;		// mapping from value to counter array index;

    int next_idx[max_K];

    int Insert_cnt = 0;



    BOBHash32 *bobHash;

    int total_cnt = 0;

public:
    ~CSS(){	delete bobHash;	}
    CSS(int memory)
    {
        bobHash = new BOBHash32(1013);
        total_cnt = 0;
        //K = memory/20;
        K = max_K;

        Value_index.clear();
        memset(CounterArr, 0, sizeof(CounterArr));
        //memset(Reverse_CA_2_IDidx, 0, sizeof(Reverse_CA_2_IDidx));
        memset(ID_index, 0, sizeof(ID_index));
        for(int i = 0; i < K; ++i){
            ID_index[i].CounterArrayIndex = -1;
            next_idx[i] = -1;
            Reverse_CA_2_IDidx[i] = -1;
        }
    }

    int FindVacantIDidx()
    {
        int ans = -1;
        for(int i = 0; i < K; ++i)
            if(next_idx[i] == -1 && ID_index[i].CounterArrayIndex == -1){
                ans = i;
                break;
            }
        if(ans == -1)
        {
            printf(">>> error 5\n");
            exit(5);
        }
        return ans;
    }

    void SwapCSS(int cur_idx, int largest_idx)
    {
        int pos = Reverse_CA_2_IDidx[cur_idx], pos1, pos2;
        while(ID_index[pos].CounterArrayIndex != cur_idx){
            pos = next_idx[pos];
            if(pos == next_idx[pos])
                break;
        }
        if(ID_index[pos].CounterArrayIndex != cur_idx)
        {
            printf(">>> error 1\n");
            exit(1);
        }
        pos1 = pos;
        ID_index[pos].CounterArrayIndex = largest_idx;

        pos = Reverse_CA_2_IDidx[largest_idx];
        while(ID_index[pos].CounterArrayIndex != largest_idx){
            pos = next_idx[pos];
            if(pos == next_idx[pos])
                break;
        }
        if(ID_index[pos].CounterArrayIndex != largest_idx)
        {
            printf(">>> id = %d, error 2\n", largest_idx);
            exit(2);
        }
        pos2 = pos;

        ID_index[pos1].CounterArrayIndex = largest_idx;
        ID_index[pos2].CounterArrayIndex = cur_idx;

        int tmpR = Reverse_CA_2_IDidx[cur_idx];
        Reverse_CA_2_IDidx[cur_idx] = Reverse_CA_2_IDidx[largest_idx];
        Reverse_CA_2_IDidx[largest_idx] = tmpR;
    }


    int find(uint8_t *key)		// return an index of counter array
    {
        int pos = bobHash->run((char*)key, key_length) % K;

        while(pos != -1 && ID_index[pos].CounterArrayIndex != -1
              && string((char*)ID_index[pos].key) != string((const char*)key, key_length)){
            pos = next_idx[pos];
            if(pos == next_idx[pos])
                break;
        }
        if(string((char*)ID_index[pos].key) != string((const char*)key, key_length))
        {
            return -1;
        }
        return ID_index[pos].CounterArrayIndex;
    }

    int query(uint8_t *key)
    {
        int pos = find(key);
        if(pos != -1)
            return (int)CounterArr[pos];
        else return -1;
    }

    void insert(uint8_t *key)
    {
        Insert_cnt++;
        int idx_in_CounterArr = find(key);
        if(idx_in_CounterArr >= 0 && idx_in_CounterArr < K)
        {
            if(total_cnt == K){
                uint32_t cur_val = CounterArr[idx_in_CounterArr];
                int largest_idx = Value_index[cur_val];
                printf("cur_val = %d, largest index = %d\n", cur_val, largest_idx);
                if(largest_idx != idx_in_CounterArr)	// if it is not the largest one in counter array
                {
                    SwapCSS(idx_in_CounterArr, largest_idx);
                    Value_index[cur_val] = largest_idx - 1;
                }
                else if(largest_idx > 0 && CounterArr[largest_idx - 1] == cur_val)	// it is the largest one
                    Value_index[cur_val] = largest_idx - 1;			// but still other counters equal to it
                else
                    Value_index.erase(cur_val);	// no other counters equal to it, we can erase it

                if(Value_index.find(cur_val + 1) == Value_index.end())
                    Value_index[cur_val + 1] = largest_idx;
                //Value_index[cur_val + 1] = std::max(Value_index[cur_val + 1], largest_idx);
                //Reverse_CA_2_IDidx[largest_idx] = bobHash->run((char*)key, key_length) % K;
                CounterArr[largest_idx] = cur_val + 1;
            }
            else{
                CounterArr[idx_in_CounterArr] += 1;
            }
        }
        else if(total_cnt == K)
        {
            uint32_t zero_val = CounterArr[0];
            int largest_idx = Value_index[zero_val];

            // first delete the node in ID_index for largest_idx
            int pos = Reverse_CA_2_IDidx[largest_idx];
            int pos_largest = pos, pos_last = pos, pos_last_second = -1;
            while(ID_index[pos_largest].CounterArrayIndex != largest_idx)
                pos_largest = next_idx[pos_largest];
            while(pos_last != next_idx[pos_last]){
                pos_last_second = pos_last;
                pos_last = next_idx[pos_last];
            }

            if(pos_last_second == -1 && pos_last == pos_largest){
                next_idx[pos] = -1;
                ID_index[pos].CounterArrayIndex = -1;
                memset(ID_index[pos].key, 0, key_length);
            }else{
                memcpy(ID_index[pos_largest].key, ID_index[pos_last].key, key_length);
                ID_index[pos_largest].CounterArrayIndex = ID_index[pos_last].CounterArrayIndex;
                memset(ID_index[pos_last].key, 0, key_length);
                ID_index[pos_last].CounterArrayIndex = -1;
                next_idx[pos_last] = -1;
                next_idx[pos_last_second] = pos_last_second;
            }

            Reverse_CA_2_IDidx[largest_idx] = bobHash->run((char*)key, key_length) % K;
            CounterArr[largest_idx] += 1;
            pos = Reverse_CA_2_IDidx[largest_idx];
            if(next_idx[pos] == -1){
                next_idx[pos] = pos;
                ID_index[pos].CounterArrayIndex = largest_idx;
                memcpy(ID_index[pos].key, key, key_length);
            }else{
                int vacantIndex = FindVacantIDidx();
                while(pos != next_idx[pos])
                    pos = next_idx[pos];
                next_idx[pos] = vacantIndex;
                next_idx[vacantIndex] = vacantIndex;
                ID_index[vacantIndex].CounterArrayIndex = largest_idx;
                memcpy(ID_index[vacantIndex].key, key, key_length);
            }
            if(largest_idx == 0)
                Value_index.erase(zero_val);
            else Value_index[zero_val] -= 1;
            Value_index[zero_val + 1] = std::max(Value_index[zero_val + 1], largest_idx);

        }
        else{
            CounterArr[total_cnt] = 1;
            Reverse_CA_2_IDidx[total_cnt] = bobHash->run((char*)key, key_length) % K;
            int pos = Reverse_CA_2_IDidx[total_cnt];
            if(next_idx[pos] == -1){
                next_idx[pos] = pos;
                ID_index[pos].CounterArrayIndex = total_cnt;
                memcpy(ID_index[pos].key, key, key_length);
            }else{
                int vacantIndex = FindVacantIDidx();
                while(pos != next_idx[pos])
                    pos = next_idx[pos];
                next_idx[pos] = vacantIndex;
                next_idx[vacantIndex] = vacantIndex;
                ID_index[vacantIndex].CounterArrayIndex = total_cnt;
                memcpy(ID_index[vacantIndex].key, key, key_length);
            }
            total_cnt++;

            if(total_cnt == K){
                printf("full\n");
                for(int i = 0; i < K - 1; ++i)
                    for(int j = i + 1; j < K; ++j)
                        if(CounterArr[i] > CounterArr[j])
                        {
                            SwapCSS(i, j);
                            uint32_t tmpVal = CounterArr[i];
                            CounterArr[i] = CounterArr[j];
                            CounterArr[j] = tmpVal;
                        }
                for(int i = 0; i < K; ++i)
                {
                    uint32_t tmpVal = CounterArr[i];
                    if(Value_index.find(tmpVal) == Value_index.end())
                        Value_index.insert(make_pair(tmpVal, i));
                    Value_index[tmpVal] = std::max(Value_index[tmpVal], i);
                }
                int tmp_cnt = 0;
                for(int i = 0; i < total_cnt; ++i)	tmp_cnt += (int)CounterArr[i];
                // 	printf("%d\n", CounterArr[i]);
                for(map<uint32_t, int>::iterator it = Value_index.begin(); it != Value_index.end(); ++it)
                    printf("%u\t%d\n", it->first, it->second);
                printf("total_cnt = %d, tmp_cnt = %d\n", total_cnt, tmp_cnt);
                printf("haha\n");
            }
        }
    }


    void get_top_k(uint32_t k, std::vector<pair<string, uint32_t> >& result)
    {
        for(int i = 0; i < K; ++i)
        {
            printf("%d: %d\n", i + 1, CounterArr[i]);
        }
        printf("Above table is counter array\n");
        printf("CounterArr[0] = %d\n", (int)CounterArr[0]);
//        exit(0);

        //for(int i = K - 1; i >= K - k; i -= 1)
        for(int i = K - k; i < K; ++i)
        {
            int pos = Reverse_CA_2_IDidx[i];
            while(ID_index[pos].CounterArrayIndex != i){
                pos = next_idx[pos];
                if(pos == -1){
                    printf(">>> %d: error 7\n", i);
                    exit(7);
                }
            }

            result[K - 1 - i].second = CounterArr[i];
            result[K - 1 - i].first = string((char*)ID_index[pos].key, key_length);
        }

    }
};




#endif
