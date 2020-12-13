#ifndef _ELASTIC_SKETCH_H_
#define _ELASTIC_SKETCH_H_

#include "../algorithms.h"
#include "HeavyPart.h"
#include "LightPart.h"
#include "HeavyPart.cpp"
#include "LightPart.cpp"


template<int bucket_num, int tot_memory_in_bytes>
class ElasticSketch: public Algorithm
{
    static constexpr int heavy_mem = bucket_num * COUNTER_PER_BUCKET * 8;
    static constexpr int light_mem = tot_memory_in_bytes - heavy_mem;

    HeavyPart<bucket_num> heavy_part;
    LightPart<light_mem> light_part;
public:
    ElasticSketch(): Algorithm("ElasticSketch") {}
    ~ElasticSketch();
    
    int insert(uint8_t *key, int keylen);
    int query(uint8_t *key, int keylen);
    void clear();

    void get_flowsize(vector<string> &flowIDs, unordered_map<string,int> &freq);
private:
    void quick_insert(uint8_t *key, int f = 1);

    int query(uint8_t *key);
    int query_compressed_part(uint8_t *key, uint8_t *compress_part, int compress_counter_num);

    int get_compress_width(int ratio);
    void compress(int ratio, uint8_t *dst);

    int get_bucket_num();
    double get_bandwidth(int compress_ratio) ;

    void get_heavy_hitters(int threshold, vector<pair<string, int>> & results);
    int get_cardinality();
    double get_entropy();
    void get_distribution(vector<double> &dist);
public:
    void *operator new(size_t sz);
    void operator delete(void *p);
};

#endif // _ELASTIC_SKETCH_H_
