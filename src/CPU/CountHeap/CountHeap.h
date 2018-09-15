#ifndef STREAMCLASSIFIER_TOPK_COUNT_HEAP_H
#define STREAMCLASSIFIER_TOPK_COUNT_HEAP_H

#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include "../common/BOBHash32.h"

using std::min;
using std::swap;

#define SQR(X) (X) * (X)

template<uint8_t key_len, int capacity, int d = 3>
struct CountHeap {
public:
    typedef pair <string, int> KV;
    typedef pair <int, string> VK;
    VK heap[capacity];
    int heap_element_num;
    int mem_in_bytes;
    int w;
    int * cm_sketch[d];
    BOBHash32 * hash[d];
    BOBHash32 * hash_polar[d];
    unordered_map<string, uint32_t> ht;

    double get_f2()
    {
        double res[d];
        for (int i = 0; i < d; ++i) {
            double est = 0;
            for (int j = 0; j < w; ++j) {
                est += SQR(double(cm_sketch[i][j]));
            }
            res[i] = est;
        }

        sort(res, res + d);
        if (d % 2) {
            return res[d / 2];
        } else {
            return (res[d / 2] + res[d / 2 - 1]) / 2;
        }
    }

    // heap
    void heap_adjust_down(int i) {
        while (i < heap_element_num / 2) {
            int l_child = 2 * i + 1;
            int r_child = 2 * i + 2;
            int larger_one = i;
            if (l_child < heap_element_num && heap[l_child] < heap[larger_one]) {
                larger_one = l_child;
            }
            if (r_child < heap_element_num && heap[r_child] < heap[larger_one]) {
                larger_one = r_child;
            }
            if (larger_one != i) {
                swap(heap[i], heap[larger_one]);
                swap(ht[heap[i].second], ht[heap[larger_one].second]);
                heap_adjust_down(larger_one);
            } else {
                break;
            }
        }
    }

    void heap_adjust_up(int i) {
        while (i > 1) {
            int parent = (i - 1) / 2;
            if (heap[parent] <= heap[i]) {
                break;
            }
            swap(heap[i], heap[parent]);
            swap(ht[heap[i].second], ht[heap[parent].second]);
            i = parent;
        }
    }

//public:
    string name;

    CountHeap(int mem_in_bytes_) : mem_in_bytes(mem_in_bytes_), heap_element_num(0) {
//        memset(heap, 0, sizeof(heap));
		w = mem_in_bytes / 4 / d;
        for (int i = 0; i < capacity; ++i) {
            heap[i].first = 0;
        }
        memset(cm_sketch, 0, sizeof(cm_sketch));
        srand(time(0));
        for (int i = 0; i < d; i++) {
            hash[i] = new BOBHash32(uint32_t(rand() % MAX_PRIME32));
            hash_polar[i] = new BOBHash32(uint32_t(rand() % MAX_PRIME32));
            cm_sketch[i] = new int[w];
            memset(cm_sketch[i], 0, sizeof(int)*w);
        }

        stringstream name_buf;
        name_buf << "CountHeap@" << mem_in_bytes;
        name = name_buf.str();
    }

    void insert(uint8_t * key) {
        int ans[d];

        for (int i = 0; i < d; ++i) {
            int idx = hash[i]->run((char *)key, key_len) % w;
            int polar = hash_polar[i]->run((char *)key, key_len) % 2;

            cm_sketch[i][idx] += polar ? 1 : -1;

            int val = cm_sketch[i][idx];

            ans[i] = polar ? val : -val;
        }

        sort(ans, ans + d);

        int tmin;
        if (d % 2 == 0) {
            tmin = (ans[d / 2] + ans[d / 2 - 1]) / 2;
        } else {
            tmin = ans[d / 2];
        }
        tmin = (tmin <= 1) ? 1 : tmin;

        string str_key = string((const char *)key, key_len);
        if (ht.find(str_key) != ht.end()) {
            heap[ht[str_key]].first++;
            heap_adjust_down(ht[str_key]);
        } else if (heap_element_num < capacity) {
            heap[heap_element_num].second = str_key;
            heap[heap_element_num].first = tmin;
            ht[str_key] = heap_element_num++;
            heap_adjust_up(heap_element_num - 1);
        } else if (tmin > heap[0].first) {
            VK & kv = heap[0];
            ht.erase(kv.second);
            kv.second = str_key;
            kv.first = tmin;
            ht[str_key] = 0;
            heap_adjust_down(0);
        }
    }

//    void get_top_k(uint16_t k, uint32_t * result) {
//        VK * a = new VK[capacity];
//        memcpy(a, heap, sizeof(heap));
//        sort(a, a + capacity);
//        int i;
//        for (i = 0; i < k && i < capacity; ++i) {
//            result[i] = a[capacity - 1 - i].second;
//        }
//        for (; i < k; ++i) {
//            result[i] = 0;
//        }
//    }

    void get_top_k_with_frequency(uint16_t k, vector<KV> & result) {
        VK * a = new VK[capacity];
        for (int i = 0; i < capacity; ++i) {
            a[i] = heap[i];
        }
        sort(a, a + capacity);
        int i;
        for (i = 0; i < k && i < capacity; ++i) {
            result[i].first = a[capacity - 1 - i].second;
            result[i].second = a[capacity - 1 - i].first;
        }
        for (; i < k; ++i) {
//            result[i].first ;
            result[i].second = 0;
        }
    }

    void get_l2_heavy_hitters(double alpha, vector<KV> & result)
    {
        get_top_k_with_frequency(capacity, result);
        double f2 = get_f2();
        for (int i = 0; i < capacity; ++i) {
            if (SQR(double(result[i].second)) < alpha * f2) {
                result.resize(i);
                return;
            }
        }
    }

    void get_heavy_hitters(uint32_t threshold, std::vector<pair<string, uint32_t> >& ret)
    {
        ret.clear();
        for (int i = 0; i < capacity; ++i) {
            if (heap[i].first >= threshold) {
                ret.emplace_back(make_pair(heap[i].second, heap[i].first));
            }
        }
    }

    ~CountHeap() {
        for (int i = 0; i < d; ++i) {
            delete hash[i];
            delete hash_polar[i];
            delete cm_sketch[i];
        }
        return;
    }
};

#endif //STREAMCLASSIFIER_COUNT_HEAP_H
