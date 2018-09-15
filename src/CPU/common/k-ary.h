#ifndef STREAMMEASUREMENTSYSTEM_K_ARY_H
#define STREAMMEASUREMENTSYSTEM_K_ARY_H

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>
#include "BOBHash32.h"


using namespace std;

class KAry
{
public:
    int w;
    int ** counters;
    BOBHash32 ** hashs;
    uint32_t * seed_idxs;
    int sum = 0;
    int keylen;
    int mem_in_bytes;
    int d;

// public:
    KAry(){;}
    KAry(uint8_t keylen_, uint32_t mem_in_bytes_, int d_ = 3): keylen(keylen_), mem_in_bytes(mem_in_bytes_), d(d_)
    {
        vector<uint32_t> idxs = BOBHash32::get_random_prime_index_list(d);
        sum = 0;
        hashs = new BOBHash32 *[d];
    	seed_idxs = new uint32_t[d];
        for (int i = 0; i < d; ++i) {
            seed_idxs[i] = idxs[i];
            hashs[i] = new BOBHash32(idxs[i]);
        }
        w = mem_in_bytes / d / 4;
    	counters = new int *[d];
    	for( int i = 0; i < d; i ++ )
    	{
    		counters[i] = new int[w];
    		memset(counters[i], 0, sizeof(int)*w);
		}
    }

    void copy_configurations(KAry & another)
    {
        for (int i = 0; i < d; ++i) {
            seed_idxs[i] = another.seed_idxs[i];
            delete hashs[i];
            hashs[i] = new BOBHash32(seed_idxs[i]);
        }
    }

    void insert(const uint8_t * key, int f = 1)
    {
        sum += f;
        for (int i = 0; i < d; ++i) {
            uint32_t pos = hashs[i]->run((const char *)key, keylen) % w;
            //cout << i << " " << pos << " " << counters[i] << " " << sizeof(int)*w << endl;
            counters[i][pos] += f;
        }
    }

    double query(const uint8_t * key)
    {
       vector<double> vs(d);
       for (int i = 0; i < d; ++i) {
           uint32_t pos = hashs[i]->run((const char *)key, keylen) % w;
           int val = counters[i][pos];
           vs[i] = (val - sum / double(w)) / (1.0 - 1.0 / w);
       }
       sort(vs.begin(), vs.end());

       if (d % 2 == 0) {
           return (vs[d / 2] + vs[d / 2 - 1]) / 2.0;
       } else {
           return vs[d / 2];
       }

        // using a CM method
        int ret = 1 << 30;
        for (int i = 0; i < d; ++i) {
            int32_t pos = hashs[i]->run((const char *)key, keylen) % w;
            int val = counters[i][pos];
            ret = std::min(val, ret);
        }
        return ret;
    }

    ~KAry()
    {
		for (int i = 0; i < d; ++i) {
            delete hashs[i];
        }
        for( int i = 0; i < d; i ++ )
    	{
    		delete counters[i];
		}
		delete hashs;
		delete counters;
		delete seed_idxs;
    }
    
    KAry &operator =(const KAry &another){
    	d = another.d;
    	hashs = new BOBHash32 *[d];
    	seed_idxs = new uint32_t[d];
        for (int i = 0; i < d; ++i) {
            seed_idxs[i] = another.seed_idxs[i]; 
            hashs[i] = new BOBHash32(seed_idxs[i]);
        }
        w = another.w;
    	counters = new int *[d];
    	for (int i = 0; i < d; ++i){
    		counters[i] = new int[w];
    		memset(counters[i], 0, sizeof(int)*w);		 //bug
		}
		mem_in_bytes = another.mem_in_bytes;
		keylen = another.keylen; 
	}
};

#endif //STREAMMEASUREMENTSYSTEM_K_ARY_H
