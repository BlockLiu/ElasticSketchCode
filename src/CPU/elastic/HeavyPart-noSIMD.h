#ifndef _HEAVYPART_H_
#define _HEAVYPART_H_

#include "param.h"




template<int bucket_num>
class HeavyPart
{
public:
	alignas(64) Bucket buckets[bucket_num];

	HeavyPart()
	{
		clear();
	}
	~HeavyPart(){}

	void clear()
	{
		memset(buckets, 0, sizeof(Bucket) * bucket_num);
	}

/* insertion */
	int insert(uint8_t *key, uint8_t *swap_key, uint32_t &swap_val, uint32_t f = 1)
	{
		uint32_t fp;
		int pos = CalculateFP(key, fp);

		/* find if there has matched bucket */
		int matched = -1, empty = -1, min_counter = 0;
		uint32_t min_counter_val = GetCounterVal(buckets[pos].val[0]);
		for(int i = 0; i < COUNTER_PER_BUCKET - 1; i++)
		{
			if(buckets[pos].key[i] == fp){
				matched = i;
				break;
			}
			if(buckets[pos].key[i] == 0 && empty == -1)
				empty = i;
			if(min_counter_val > GetCounterVal(buckets[pos].val[i])){
				min_counter = i;
				min_counter_val = GetCounterVal(buckets[pos].val[i]);
			}
		}

		/* if matched */
		if(matched != -1){
			buckets[pos].val[matched] += f;
			return 0;
		}

		/* if there has empty bucket */
		if(empty != -1){
			buckets[pos].key[empty] = fp;
			buckets[pos].val[empty] = f;
			return 0;
		}

		/* update guard val and comparison */
		uint32_t guard_val = buckets[pos].val[MAX_VALID_COUNTER];
		guard_val = UPDATE_GUARD_VAL(guard_val);

		if(!JUDGE_IF_SWAP(GetCounterVal(min_counter_val), guard_val))
		{
			buckets[pos].val[MAX_VALID_COUNTER] = guard_val;
			return 2;
		}

	
		*((uint32_t*)swap_key) = buckets[pos].key[min_counter];
		swap_val = buckets[pos].val[min_counter];


		buckets[pos].val[MAX_VALID_COUNTER] = 0;


		buckets[pos].key[min_counter] = fp;
		buckets[pos].val[min_counter] = 0x80000001;

		return 1;
	}





/* query */
	uint32_t query(uint8_t *key)
	{
		uint32_t fp;
		int pos = CalculateFP(key, fp);

		for(int i = 0; i < MAX_VALID_COUNTER; ++i)
			if(buckets[pos].key[i] == fp)
				return buckets[pos].val[i];

		return 0;
	}


/* interface */
	int get_memory_usage()
	{
		return bucket_num * sizeof(Bucket);
	}
	int get_bucket_num()
	{
		return bucket_num;
	}

private:
	int CalculateFP(uint8_t *key, uint32_t &fp)
	{
		fp = *((uint32_t*)key);
		return CalculateBucketPos(fp) % bucket_num;
	}
};

#endif












