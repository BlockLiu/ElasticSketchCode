#ifndef STREAMMEASUREMENTSYSTEM_UNIVMON_H
#define STREAMMEASUREMENTSYSTEM_UNIVMON_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "../CountHeap/CountHeap.h"
#include <ctime>
#include <cmath>
#include <sstream>

template<uint8_t key_len, uint64_t mem_in_bytes, uint8_t level = 14>
class UnivMon
{
public:
    static constexpr uint16_t k = 1000;
    typedef CountHeap<key_len, k, 5> L2HitterDetector;
    L2HitterDetector * sketches[level];
    BOBHash32 * polar_hash[level];
    int element_num = 0;

    double g_sum(double (*g)(double))
    {
        std::vector<pair<std::string, int>> result(k);
        double Y[level] = {0};

        for (int i = level - 1; i >= 0; i--) {
            sketches[i]->get_top_k_with_frequency(k, result);
//            sketches[i]->get_l2_heavy_hitters(0.001, result);
            Y[i] = (i == level - 1) ? 0 : 2 * Y[i + 1];
            for (auto & kv: result) {
                if (kv.second == 0) {
                    continue;
                }
                int polar = (i == level - 1) ? 1 : (polar_hash[i + 1]->run(kv.first.c_str(), key_len)) % 2;
                int coe = (i == level - 1) ? 1 : (1 - 2 * polar);
                Y[i] += coe * g(double(kv.second));

//                if (i == level - 1) {
//                    cout << kv.second << " " << g(double(kv.second)) << endl;
//                }
            }
//            cout << sketches[i]->ht.size() << endl;
        }

        return Y[0];
    }
//public:
    string name;

    UnivMon()
    {
        stringstream name_buffer;
        name_buffer << "UnivMon@" << mem_in_bytes;
        name = name_buffer.str();
        //srand(time(0));
        double total = (1u << level) - 1;
        for (int i = 0; i < level; ++i) {
//            int mem = int(mem_in_bytes * (1 << (level - 1 - i)) / total);
            int mem_for_sk = int(mem_in_bytes) - level * (key_len + 4) * k;
            int mem = int(mem_for_sk / level);
            sketches[i] = new L2HitterDetector(mem);
            auto idx = uint32_t(rand() % MAX_PRIME32);
//            cout << idx << endl;
            polar_hash[i] = new BOBHash32(idx);
        }
    }

    void insert(uint8_t * key)
    {
        int polar;
        element_num++;
        sketches[0]->insert(key);
        for (int i = 1; i < level; ++i) {
            polar = ((polar_hash[i]->run((const char *)key, key_len))) % 2;
//            cout << polar << endl;
            if (polar) {
                sketches[i]->insert(key);
            } else {
                break;
            }
        }
    }

    double get_cardinality()
    {
        return g_sum([](double x) { return 1.0; });
    }

    double get_entropy()
    {
        double sum = g_sum([](double x) { return x == 0 ? 0 : x * std::log2(x); });
        return std::log2(element_num) - sum / element_num;
    }

    void get_heavy_hitters(uint32_t threshold, std::vector<pair<uint32_t, int> >& ret)
    {
        unordered_map<std::string, uint32_t> results;
        vector<std::pair<std::string, int>> vec_top_k(k);
        for (int i = level - 1; i >= 0; --i) {
            sketches[i]->get_top_k_with_frequency(k, vec_top_k);
            for (auto kv: vec_top_k) {
                if (results.find(kv.first) == results.end()) {
                    results[kv.first] = kv.second;
                }
            }
        }

        ret.clear();
        for (auto & kv: results) {
            if (kv.second >= threshold) {
//                results.erase(kv.first);
                ret.emplace_back(make_pair(*(uint32_t *)(kv.first.c_str()), kv.second));
            }
        }
//        std::copy(results.begin(), results.end(), ret.begin());
//        for (auto & kv: ret) {
//            cout << kv.second << endl;
//        }
    }

    ~UnivMon()
    {
        for (int i = 0; i < level; ++i) {
            delete sketches[i];
            delete polar_hash[i];
        }
    }
};

#endif //STREAMMEASUREMENTSYSTEM_UNIVMON_H
