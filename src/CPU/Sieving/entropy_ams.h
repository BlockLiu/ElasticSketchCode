/**
 * Implementation of Algorithm 1 in paper
 * Lall, A., Sekar, V., Ogihara, M., Xu, J., & Zhang, H. (2006).
 * Data Streaming Algorithms for Estimating Entropy of Network Traffic.
 * SIGMETRICS Perform. Eval. Rev., 34(1), 145–156.
 * https://doi.org/10.1145/1140103.1140295
 */

#ifndef STREAMMEASUREMENTSYSTEM_ENTROPHY_AMS_H
#define STREAMMEASUREMENTSYSTEM_ENTROPHY_AMS_H

#include "../common/BOBHash32.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <unordered_set>

namespace algorithms {
namespace entropy {

struct Item {
        Item *chaining_ptr = nullptr;
        uint8_t *key = nullptr;
        uint32_t counter = 0;
        Item *prev_ptr = nullptr;
};

template <int key_len, uint32_t g, uint32_t z> class EntropyAMS {
        Item items[g * z];
        Item *hash_table[g * z];
        Item *available_stack[g * z];
        uint32_t available_top;

        uint32_t rand_pos[g * z];
        uint32_t now_pos = 0;
        uint32_t now_item_cnt = 0;

        static constexpr uint32_t element_num = 2400000;

        BOBHash32 *hash_fun;

        void preprocessing() {
                // random z * g position
                std::random_device rd;
                std::uniform_int_distribution<uint32_t> dist(0, element_num);
                std::unordered_set<uint32_t> used;
                for (int i = 0; i < z * g; ++i) {
                        do {
                                rand_pos[i] = dist(rd) + 1;
                        } while (used.find(rand_pos[i]) != used.end());
                        used.insert(rand_pos[i]);
                }
                std::sort(rand_pos, rand_pos + z * g);
        }

      public:
        const string name = "Entropy AMS";

        EntropyAMS() {
                memset(items, 0, sizeof(items));
                memset(hash_table, 0, sizeof(hash_table));

                for (int i = 0; i < g * z; ++i) {
                        available_stack[i] = items + i;
                }
                int64_t seed =
                    std::chrono::system_clock::now().time_since_epoch().count();
                shuffle(available_stack, available_stack + g * z,
                        std::default_random_engine(seed));
                available_top = g * z;

                hash_fun = new BOBHash32(107);

                now_item_cnt = 0;
                preprocessing();
        }

        void insert(uint8_t *key) {
                uint32_t hash_pos =
                    hash_fun->run((const char *)key, key_len) % (z * g);
                Item *hash_ptr = hash_table[hash_pos];

                now_item_cnt++;

                // if hit pos, create new entry
                while (now_item_cnt == rand_pos[now_pos]) {
                        //                cout << now_pos << " " <<
                        //                rand_pos[now_pos] << " " <<
                        //                rand_pos[now_pos + 1] << endl;
                        now_pos++;
                        Item *p = available_stack[--available_top];

                        p->key = new uint8_t[key_len];
                        memcpy(p->key, key, key_len);

                        p->chaining_ptr = hash_ptr;
                        hash_table[hash_pos] = p;

                        while (hash_ptr) {
                                if (!memcmp(hash_ptr->key, key, key_len)) {
                                        //                        hash_ptr->prev_ptr
                                        //                        = p;
                                        p->prev_ptr = hash_ptr;
                                        //                        hash_ptr = p;
                                        break;
                                }
                                hash_ptr = hash_ptr->chaining_ptr;
                        }
                        hash_ptr = hash_table[hash_pos];
                        //                p->counter = 1;
                }

                // if hit item, add counter
                while (hash_ptr) {
                        if (!memcmp(hash_ptr->key, key, key_len)) {
                                hash_ptr->counter += 1;
                                return;
                        }
                        hash_ptr = hash_ptr->chaining_ptr;
                }

                // else, do nothing
                return;
        }

        double get_entropy() {
                // add counters to their prevs
                for (int i = 0; i < g * z; ++i) {
                        //            Item * p = items + i;
                        //            Item * temp = p->prev_ptr;
                        //            while (temp) {
                        //                p->counter += temp-> counter;
                        //                temp = temp->prev_ptr;
                        //            }
                        Item *p = hash_table[i];
                        while (p) {
                                Item *temp = p->prev_ptr;
                                if (temp) {
                                        temp->counter += p->counter;
                                }
                                p = p->chaining_ptr;
                        }
                }

                vector<double> xs;
                for (int i = 0; i < g; ++i) {
                        double temp = 0;
                        for (int j = 0; j < z; ++j) {
                                int index = i * z + j;
                                uint32_t c = items[index].counter;
                                //                if (c == 0) cout << "errr" <<
                                //                endl;
                                temp += now_item_cnt *
                                        (c <= 1 ? 0 : (c * log2(c) -
                                                       (c - 1) * log2(c - 1)));
                        }
                        xs.emplace_back(temp / z);
                }
                sort(xs.begin(), xs.end());

                int mid = g / 2;
                double s;
                if (g % 2 == 0) {
                        s = (xs[mid] + xs[mid - 1]) / 2;
                } else {
                        s = xs[mid];
                }

                //        return s;
                return log2(now_item_cnt) - s / now_item_cnt;
        }

        ~EntropyAMS() { delete hash_fun; }
};
}
}

#endif // STREAMMEASUREMENTSYSTEM_ENTROPHY_AMS_H
