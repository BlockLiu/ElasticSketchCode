#ifndef _LINEAR_COUNTING_4_H_
#define _LINEAR_COUNTING_4_H_

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../common/BOBHash32.h"

template <int key_len, int tot_memory_in_bytes> class LinearCounting {
        static constexpr int map_num = tot_memory_in_bytes;

        uint8_t bit_map[map_num];
        BOBHash32 *hash_gen;

      public:
        LinearCounting() {
                memset(bit_map, 0, map_num);
                auto idx = uint32_t(rand() % MAX_PRIME32);
                hash_gen = new BOBHash32(idx);
        }
        ~LinearCounting() {}
        void clear() { memset(bit_map, 0, map_num); }

        void insert(uint8_t *key) {
                int pos =
                    hash_gen->run((const char *)key, key_len) % (map_num * 8);

                int pos_1 = pos / 8, pos_2 = pos % 8;
                bit_map[pos_1] |= (1 << pos_2);
        }

      public:
        string name = "FM";

        int get_cardinality() {
		int m = map_num * 8;
                double z = 0;

                for (int i = 0; i < map_num; i++)
                        z += __builtin_popcount(bit_map[i]);
                return m * log(m / (m - z));
        }
};

#endif
