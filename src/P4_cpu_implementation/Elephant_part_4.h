#ifndef _ELEPHANT_PART_4_H_
#define _ELEPHANT_PART_4_H_

#include "param.h"

template <int bucket_num> class ElephantPart_4 {
      public:
        alignas(64) Bucket buckets[bucket_num];
        int sentry[bucket_num][CEIL_PER_BUCKET];

        BOBHash32 *hash_gen[CEIL_PER_BUCKET];

        ElephantPart_4() {
                memset(buckets, 0, sizeof(Bucket) * bucket_num);
                memset(sentry, 0, sizeof(int) * bucket_num * CEIL_PER_BUCKET);

                for (int i = 0; i < CEIL_PER_BUCKET; i++) {
                        std::random_device rd;
                        hash_gen[i] = new BOBHash32(rd() % MAX_PRIME32);
                }
        }
        ~ElephantPart_4() {}

        void clear() {
                memset(buckets, 0, sizeof(Bucket) * bucket_num);
                memset(sentry, 0, sizeof(int) * bucket_num * CEIL_PER_BUCKET);
        }

        int insert(uint8_t *key, uint8_t *swap_key, uint32_t &swap_val,
                   uint32_t f = 1) {
                uint32_t full_fp[CEIL_PER_BUCKET];
                int full_pos[CEIL_PER_BUCKET];

                int Initial_Ele = 1;


                for (int i = 0; i < CEIL_PER_BUCKET; i++) {
                        uint32_t fp;
                        int pos = CalculateFP(key, fp, i);


                        // sentry[pos][i] = UPDATE_GUARD_VAL(sentry[pos][i], f);
                        sentry[pos][i] = UPDATE_GUARD_VAL(sentry[pos][i], 1);


                        if (buckets[pos].ceils[i].key == fp) {
                                // buckets[pos].ceils[i].val += f;
                                buckets[pos].ceils[i].val += 1;
                                return 0;
                        }

                        uint32_t min_counter_val = GetCounterVal(buckets[pos].ceils[i].val);


                        if (min_counter_val == 0)
                        {
                                buckets[pos].ceils[i].key = fp;
                                // buckets[pos].ceils[i].val += f;
                                buckets[pos].ceils[i].val += 1;
                                return 0;
                        }

                        if (!JUDGE_IF_SWAP(GetCounterVal(min_counter_val), sentry[pos][i])) {
                                if (i == CEIL_PER_BUCKET - 1) {
                                        if (Initial_Ele == 1)
                                                return 2;
                                        else
                                                return 1;
                                } else
                                        continue;
                        }


                        *((uint32_t *)swap_key) = buckets[pos].ceils[i].key;
                        swap_val = buckets[pos].ceils[i].val;

                        buckets[pos].ceils[i].key = fp;
                        // buckets[pos].ceils[i].val += f;
                        buckets[pos].ceils[i].val += 1;

                        if (i == CEIL_PER_BUCKET - 1)
                                return 1;
                        else {
                                *(uint32_t *)key = *(uint32_t *)swap_key;
                                // f = swap_val;
                                f = 1;

                                Initial_Ele = 0;
                                continue;
                        }
                }
        }

        uint32_t query(uint8_t *key) {
                int sum = 0, HIGHEST_BIT = 0;

                for (int i = 0; i < CEIL_PER_BUCKET; i++) {
                        uint32_t fp;
                        int pos = CalculateFP(key, fp, i);

                        if (buckets[pos].ceils[i].key == fp) {
                                sum += GetCounterVal(buckets[pos].ceils[i].val);
                        }
                }

                return sum;
        }

      private:
        int CalculateFP(uint8_t *key, uint32_t &fp, int i) {
                fp = *((uint32_t *)key);
                /*
                return hash_gen[i]->run((char *)&fp, 4) % bucket_num;
                */
                return CalculateBucketPos(fp) % (bucket_num - i);
        }

      public:
        int get_memory_usage() { return bucket_num * sizeof(Bucket); }
        int get_bucket_num() { return bucket_num; }
};

#endif
