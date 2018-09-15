#ifndef STREAMCLASSIFIER_CM_HEAP_H
#define STREAMCLASSIFIER_CM_HEAP_H

#include <unordered_map>
#include <algorithm>
#include <sstream>
#include "../common/BOBHash32.h"
#include "../common/cuckoo_hashing.h"

using std::min;
using std::swap;

template<uint8_t key_len, int capacity, int d = 3>
struct CMHeap {
    typedef pair <uint8_t[key_len], int> KV;
//    typedef pair <int, string> VK;
    KV heap[capacity];
    int heap_element_num;
    int mem_in_bytes;
    int w;
    int * cm_sketch[d];
    BOBHash32 * hash[d];
//    BOBHash32 * hash_polar[d];
//    unordered_map<string, uint32_t> ht;
    cuckoo::CuckooHashing<key_len, int(capacity * 2)> ht;

    // heap
    void heap_adjust_down(int i) {
        while (i < heap_element_num / 2) {
            int l_child = 2 * i + 1;
            int r_child = 2 * i + 2;
            int larger_one = i;
            if (l_child < heap_element_num && heap[l_child].second < heap[larger_one].second) {
                larger_one = l_child;
            }
            if (r_child < heap_element_num && heap[r_child].second < heap[larger_one].second) {
                larger_one = r_child;
            }
            if (larger_one != i) {
                swap(heap[i], heap[larger_one]);
//                swap(ht[string((const char *)heap[i].first, key_len)], ht[string((const char *)heap[larger_one].first, key_len)]);
                swap(ht[heap[i].first], ht[heap[larger_one].first]);
                heap_adjust_down(larger_one);
            } else {
                break;
            }
        }
    }

    void heap_adjust_up(int i) {
        while (i > 1) {
            int parent = (i - 1) / 2;
            if (heap[parent].second <= heap[i].second) {
                break;
            }
            swap(heap[i], heap[parent]);
//            swap(ht[string((const char *)heap[i].first, key_len)], ht[string((const char *)heap[parent].first, key_len)]);
            swap(ht[heap[i].first], ht[heap[parent].first]);
            i = parent;
        }
    }

public:
    string name;
    CMHeap(int mem_in_bytes_) : mem_in_bytes(mem_in_bytes_), heap_element_num(0) {
        w = mem_in_bytes / 4 / d;
        for(int i = 0; i < d; i ++)
        {
        	cm_sketch[i] = new int[w];
        	memset(cm_sketch[i], 0, sizeof(int)*w);
        }
		memset(heap, 0, sizeof(heap));
        for (int i = 0; i < capacity; ++i) {
            heap[i].second = 0;
        }
        
        random_device rd;
        for (int i = 0; i < d; i++) {
            hash[i] = new BOBHash32(uint32_t(rd() % MAX_PRIME32));
        }

        stringstream name_buf;
        name_buf << "CMHeap@" << mem_in_bytes;
        name = name_buf.str();
    }

    void insert(uint8_t * key) {
        int tmin = 1 << 30, ans = tmin;

        for (int i = 0; i < d; ++i) {
            int idx = hash[i]->run((char *)key, key_len) % w;
            cm_sketch[i][idx]++;
            int val = cm_sketch[i][idx];

            ans = std::min(val, ans);
        }

        string str_key = string((const char *)key, key_len);
//        if (ht.find(str_key) != ht.end())
        if (ht.find(key))
        {
//            heap[ht[str_key]].second++;
//            heap_adjust_down(ht[str_key]);

            heap[ht[key]].second++;
            heap_adjust_down(ht[key]);
        } else if (heap_element_num < capacity) {
//            heap[heap_element_num].first = str_key;
            memcpy(heap[heap_element_num].first, key, key_len);
            heap[heap_element_num].second = ans;
//            ht[str_key] = heap_element_num++;
            ht[key] = heap_element_num++;
            heap_adjust_up(heap_element_num - 1);
        } else if (tmin > heap[0].second) {
            KV & kv = heap[0];
//            ht.erase(string((const char *)kv.first, key_len));
            ht.erase(kv.first);
//            kv.first = str_key;
            memcpy(kv.first, key, key_len);
            kv.second = ans;
//            ht[str_key] = 0;
            ht[key] = 0;
            heap_adjust_down(0);
        }
    }

    int query(uint8_t * key) {
        int tmin = 1 << 30, ans = tmin;
        for(int i = 0; i < d; ++i){
            int idx = hash[i]->run((char*)key, key_len) % w;
            int val = cm_sketch[i][idx];
            ans = std::min(val, ans);
        }
        return ans;
    }

    void get_heavy_hitters(uint32_t threshold, std::vector<pair<string, uint32_t> >& ret)
    {
        ret.clear();
        for (int i = 0; i < capacity; ++i) {
            if (heap[i].second >= threshold) {
                ret.emplace_back(make_pair(string((const char *)heap[i].first, key_len), heap[i].second));
            }
        }
    }

    ~CMHeap() {
        for (int i = 0; i < d; ++i) {
            delete hash[i];
            delete cm_sketch[i];
        }
        return;
    }
};

#endif //STREAMCLASSIFIER_COUNT_HEAP_H
