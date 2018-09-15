#ifndef _CU_SKETCH_H_VERSION_2
#define _CU_SKETCH_H_VERSION_2

#include <sstream>
#include <cstring>
#include <algorithm>
#include <string>
#include "../common/BOBHash32.h"

using namespace std;

template<int key_len, int d>
class CUSketch
{
private:
	int memory_in_bytes = 0;

	int w = 0;
	int* counters[d] = {NULL};
	BOBHash32* hash[d] = {NULL};

public:
	string name;

	CUSketch(){}
	CUSketch(int memory_in_bytes)
	{	initial(memory_in_bytes);	}
	~CUSketch(){clear();}

	void initial(int memory_in_bytes)
	{
		this->memory_in_bytes = memory_in_bytes;
		w = memory_in_bytes / 4 / d;

        for(int i = 0; i < d; ++i){
            counters[i] = new int[w];
            memset(counters[i], 0, 4 * w);

            hash[i] = new BOBHash32(i + 750);
        }

		stringstream name_buffer;
        name_buffer << "CU@" << memory_in_bytes << "@" << d;
        name = name_buffer.str();
	}
	void clear()
	{
        for(int i = 0; i < d; ++i)
            delete[] counters[i];

		for(int i = 0; i < d; ++i)
			delete hash[i];
	}

	void print_basic_info()
    {
        printf("CU sketch\n");
        printf("\tCounters: %d\n", w);
        printf("\tMemory: %.6lfMB\n", w * 4.0 / 1024 / 1024);
    }

    void insert(uint8_t *key, int f = 1)
    {
        int index[d] = {0};
        int value[d] = {0};
    	int min_val = 1 << 30;

    	for (int i = 0; i < d; ++i)
    	{
    		index[i] = (hash[i]->run((const char*)key, key_len)) % w;
    		value[i] = counters[i][index[i]];
    		min_val = min(min_val, value[i]);
    	}

    	int temp = min_val + f;
    	for (int i = 0; i < d; ++i)
    		counters[i][index[i]] = max(counters[i][index[i]], temp);
    }
    int query(uint8_t *key)
    {
    	int ret = 1 << 30;
    	for (int i = 0; i < d; ++i)
    	{
    		int idx = (hash[i]->run((const char*)key, key_len)) % w;
    		ret = min(ret, counters[i][idx]);
    	}
    	return ret;
    }
};

#endif