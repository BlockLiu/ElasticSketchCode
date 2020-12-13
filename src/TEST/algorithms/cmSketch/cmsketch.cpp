#include "cmsketch.h"
#include <string.h>

CMSketch::CMSketch(int tot_mem_in_bytes, int d): Algorithm("CMSketch")
{
    this->d = d;
    this->width = tot_mem_in_bytes / d / sizeof(uint32_t);
    counters = new uint32_t[width * d];
    memset(counters, 0, sizeof(uint32_t) * width * d);
    hash = new BOBHash32[d];
    for(int i = 0; i < d; ++i)
        hash[i].initialize(100 + i);
}

CMSketch::~CMSketch()
{
    delete[] hash;
    delete[] counters;
}

void CMSketch::clear()
{
    insertTimes = 0;
    memset(counters, 0, sizeof(uint32_t) * width * d);
}

int CMSketch::insert(uint8_t *key, int keylen)
{
    for(int i = 0; i < d; ++i)
    {
        int pos = i * width + (hash[i].run((const char*)key, keylen) % width);
        counters[pos]++;
    }
    return ++insertTimes;
}

int CMSketch::query(uint8_t *key, int keylen)
{
    int ret = 0x7FFFFFFF;
    for(int i = 0; i < d; ++i)
    {
        int pos = i * width + (hash[i].run((const char*)key, keylen) % width);
        ret = ret > counters[pos] ? counters[pos] : ret;
    }
    return ret;
}

void CMSketch::get_flowsize(vector<string> &flowIDs, unordered_map<string,int> &freq)
{
    freq.clear();
    uint8_t key[100] = {0};
    for(auto id : flowIDs){
        memcpy(key, id.c_str(), id.size());
        freq[id] = this->query(key, id.size());
    }
}