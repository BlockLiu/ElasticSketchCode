#ifndef _LIGHT_PART_H_
#define _LIGHT_PART_H_

#include "EMFSD.h"
#include "param.h"

template<int init_mem_in_bytes>
class LightPart
{
	static constexpr int counter_num = init_mem_in_bytes;
	BOBHash32 *bobhash = NULL;

	uint8_t counters[counter_num];
	int mice_dist[256];
	EMFSD *em_fsd_algo = NULL;
public:
	LightPart();
	~LightPart();

	void clear();

	void insert(uint8_t *key, int f = 1);
	void swap_insert(uint8_t *key, int f);
	int query(uint8_t *key) ;

    void compress(int ratio, uint8_t *dst);
    int query_compressed_part(uint8_t *key, uint8_t *compress_part, int compress_counter_num);
    int get_compress_width(int ratio);
    int get_compress_memory(int ratio);
    int get_memory_usage();

   	int get_cardinality();
    void get_entropy(int &tot, double &entr);
    void get_distribution(vector<double> &dist);
};

#endif // _LIGHT_PART_H_
