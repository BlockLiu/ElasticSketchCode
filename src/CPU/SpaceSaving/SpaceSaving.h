#ifndef STREAMCLASSIFIER_SPACESAVING_H
#define STREAMCLASSIFIER_SPACESAVING_H

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "SpaceSavingUtils.h"
#include <unordered_map>
#include <vector>
#include <sstream>

using namespace std;

template<int keylen> struct KeyNode;

template<int keylen>
struct ValueNode
{
    ValueNode<keylen> * prev = NULL;
    ValueNode<keylen> * next = NULL;
    KeyNode<keylen> * first = NULL;
    uint32_t val = 0;
};

template<int keylen>
struct KeyNode
{
    ValueNode<keylen> * parent = NULL;
    KeyNode<keylen> * prev = NULL;
    KeyNode<keylen> * next = NULL;
    uint8_t key[keylen];
};


template<int key_len>
class SpaceSaving
{
//    int now_element;
	uint32_t mem_in_bytes;
    static constexpr int bytes_per_item = 36 + key_len * 2;
    int capacity = mem_in_bytes / bytes_per_item;
//    uint16_t tail_idx;
    typedef KeyNode<key_len> SSKeyNode;
    typedef ValueNode<key_len> SSValNode;
    SSKeyNode *key_nodes;
    SSValNode *val_nodes;


//    SSKeyNode * key_node_pool[capacity];
    SSValNode ** val_node_pool;
//    int key_node_empty_cnt;
    int val_node_empty_cnt;

    SSValNode * tail_node;

    unordered_map<string, SSKeyNode *> hash_table;

    void append_new_key(uint8_t * key) {
        // exact first key in tail node
        SSKeyNode * victim = tail_node->first;
        uint8_t * old_key = victim->key;
        hash_table.erase(string((const char *)old_key, key_len));
        hash_table[string((const char *)key, key_len)] = victim;
        memcpy(victim->key, key, key_len);
        add_counter(victim);
    }

    void add_counter(SSKeyNode * my) {
        //
        if (my->next == my) {
            my->parent->val++;
            SSValNode * brother = my->parent->prev;
//            if (brother == my->parent)
//                std::cout << "wtf" << endl;
            if (brother && brother->val == my->parent->val) {
                val_node_pool[val_node_empty_cnt++] = my->parent; // release my parent
//                cout << "release" << endl;
                SSValNode * old_parent = my->parent;
                if (old_parent->next)
                    old_parent->next->prev = old_parent->prev;
                if (old_parent == tail_node)
                    tail_node = old_parent->prev;
                old_parent->prev->next = old_parent->next;
                my->parent = brother;
                my->next = brother->first;
                my->prev = brother->first->prev;
                my->prev->next = my;
                my->next->prev = my;
                brother->first = my;
            } else {
                return;
            }
        } else {
            // detach from old parent
            my->next->prev = my->prev;
            my->prev->next = my->next;
            if (my->parent->first == my) {
                my->parent->first = my->next;
            }

            SSValNode * brother = my->parent->prev;
//            if (my->parent == brother)
//                cout << "asdf" << endl;
            if (brother && brother->val == my->parent->val + 1) {
                my->parent = brother;
                my->next = brother->first;
                my->prev = brother->first->prev;
                my->prev->next = my;
                my->next->prev = my;
                brother->first = my;
            } else {
                // create a new parent
                SSValNode * new_parent = val_node_pool[--val_node_empty_cnt];
                SSValNode * old_parent = my->parent;
//                if (my->parent == new_parent) {
//                    cout << "fds" << endl;
//                    exit(-1);
//                }
                new_parent->next = old_parent;
                new_parent->prev = brother;
//                if (brother == new_parent) {
//                    cout << "fds1" << endl;
//                    exit(-1);
//                }
                new_parent->val = old_parent->val + 1;
                new_parent->first = my;
                if (brother)
                    brother->next = new_parent;
                old_parent->prev = new_parent;
                my->parent = new_parent;
                my->next = my;
                my->prev = my;
            }
        }
    }
public:
    string name;

    SpaceSaving(uint32_t mem_in_bytes_): mem_in_bytes(mem_in_bytes_)
	{
        capacity = mem_in_bytes / bytes_per_item;
        key_nodes = new SSKeyNode[capacity];
        val_nodes = new SSValNode[capacity];
        val_node_pool = new SSValNode*[capacity];
		memset(key_nodes, 0, sizeof(SSKeyNode)*capacity);
        memset(val_nodes, 0, sizeof(SSValNode)*capacity);
//        now_element = 0;
//        nodes[0].val = -1;
//        nodes[0].parent = nodes;
        val_node_empty_cnt = 0;
        for (int i = 1; i < capacity; ++i) {
            val_node_pool[val_node_empty_cnt++] = val_nodes + i;
        }

        tail_node = val_nodes;
        val_nodes[0].val = 0;
        val_nodes->first = key_nodes;
        for (int i = 0; i < capacity; ++i) {
            key_nodes[i].next = key_nodes + (i + 1 + capacity) % capacity;
            key_nodes[i].prev = key_nodes + (i - 1 + capacity) % capacity;
            key_nodes[i].parent = tail_node;
        }

        hash_table.reserve(10 * capacity);

        stringstream name_buffer;
        name_buffer << "SS@" << mem_in_bytes;
        name = name_buffer.str();
    }

    void insert(uint8_t * key)
    {
//        Node *& my_node = hash_table[key];
        auto itr = hash_table.find(string((const char *)key, key_len));
        if (itr == hash_table.end()) {
            // key not found
            append_new_key(key);
        } else {
            // key found
            add_counter(itr->second);
        }
    }

    void get_top_k(uint16_t k, vector<pair<string, uint32_t>> & result)
    {
        SSValNode * p = tail_node;

        while (p->prev)
            p = p->prev;

        int i = 0;
        for (; p; p = p->next) {
            SSKeyNode * v = p->first;
            do {
                result[i].first = string((const char *)v->key, key_len);
                result[i++].second = p->val;
                v = v->next;
                if (i == k) {
                    return;
                }
            } while(v != p->first);
        }

        for (; i < k; ++i) {
            result[i].second = 0;
        }

        return;
    }

    void get_heavy_hitters(uint32_t threshold, std::vector<pair<string, uint32_t> >& ret)
    {
        int max_k = capacity;
        ret.resize(max_k);
        get_top_k(max_k, ret);
        for (int i = 0; i < max_k; ++i) {
            if (ret[i].second < threshold) {
                ret.resize(i);
                return;
            }
        }
    }
};

#endif //STREAMCLASSIFIER_SPACESAVING_H
