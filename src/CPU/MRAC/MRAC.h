#ifndef STREAMMEASUREMENTSYSTEM_MRAC_H
#define STREAMMEASUREMENTSYSTEM_MRAC_H

#include "../common/BOBHash32.h"
#include "../common/EMFSD.h"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;

template <uint32_t key_length, uint32_t memory_in_bytes> class MRAC {
        static constexpr uint32_t w = memory_in_bytes / 4;
        uint32_t counters[w];
        BOBHash32 *bob_hash;

        double est_cardinality = 0;

        EMFSD *em_fsd_algo = NULL;

      public:
        string name;

        MRAC() {
                srand(time(0));
                memset(counters, 0, sizeof(counters));
                bob_hash = new BOBHash32(rand() % MAX_PRIME32);

                std::stringstream name_buffer;
                name_buffer << "MRAC@" << memory_in_bytes;
                name = name_buffer.str();
        }

        void insert(uint8_t *item) {
                uint32_t pos =
                    bob_hash->run((const char *)item, key_length) % w;
                counters[pos] += 1;
        }

        void collect_fsd() {
                em_fsd_algo = new EMFSD();
                em_fsd_algo->set_counters(w, counters);
        }

        void next_epoch() { em_fsd_algo->next_epoch(); }

        void get_distribution(vector<double> &dist_est) {
                dist_est = em_fsd_algo->ns;
        }

        double get_cardinality() {
                //        if (est_cardinality == 0) {
                //            calc_distribution();
                //        }
                return em_fsd_algo->n_sum;
        }
};

#endif // STREAMMEASUREMENTSYSTEM_MRAC_H
