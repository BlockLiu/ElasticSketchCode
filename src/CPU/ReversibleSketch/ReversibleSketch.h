#ifndef STREAMMEASUREMENTSYSTEM_REVERSIBLESKETCH_H 
#define STREAMMEASUREMENTSYSTEM_REVERSIBLESKETCH_H

#include <sstream>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <random>
#include "../common/k-ary.h"
#include <set>

#include "../common/os_mangler.h"
#define H 6

using namespace std;

class ReversibleSketch {
public:
	uint8_t keylen;
	uint32_t mem_in_bytes;
	//int H;
    class KeyMangler
    {
        uint8_t a[4], b[4];

        uint64_t gcd(uint64_t a, uint64_t b) {
            if (a < b) {
                a ^= b;
                b ^= a;
                a ^= b;
            }
            while (b != 0) {
                uint64_t temp = a;
                a = b;
                b = temp % b;
            }
            return a;
        }

        uint64_t Euclid(uint64_t m, uint64_t b){
            uint64_t A[4];
            A[1] = 1;
            A[2] = 0;
            A[3] = m;
            uint64_t B[4];
            B[1] = 0;
            B[2] = 1;
            B[3] = b;

            while(true){
                if (0 == B[3]) {
                    return gcd(m, b);
                } else if (1 == B[3]) {
                    return B[2];
                }

                int quotient = A[3] / B[3];

                uint64_t T[4];
                T[1] = A[1] - quotient * B[1];
                T[2] = A[2] - quotient * B[2];
                T[3] = A[3] - quotient * B[3];

                A[1] = B[1];
                A[2] = B[2];
                A[3] = B[3];
                B[1] = T[1];
                B[2] = T[2];
                B[3] = T[3];
            }
            return 0;
        }

        uint32_t mul32[2][4];
        uint8_t mul8[2][4];

        void convert(const uint8_t * a, uint8_t *b, int direction)
        {
            //BUG
			//if (keylen == 4) {
                uint32_t k = *(uint32_t *)a;
                k *= mul32[direction][0];
                *(uint32_t *)b = k;
            /*}
            else if (keylen == 13) {
                *(uint32_t *)b = *(uint32_t *)a * mul32[direction][0];
                *(uint32_t *)(b + 4) = *(uint32_t *)(a + 4) * mul32[direction][1];
                *(uint32_t *)(b + 8) = *(uint32_t *)(a + 8) * mul32[direction][2];
                *(uint8_t *)(b + 12) = *(uint8_t *)(a + 12) * mul8[direction][0];
            }*/
        }

    public:
        Mangler *mangler = NULL;

        KeyMangler()
        {
			mul32[0][0] = 10000007u;
            mul32[0][1] = 1000007u;
            mul32[0][2] = 100007u;
            mul32[0][3] = 10007u;
            mul8[0][0] = 131u;
            mul8[0][1] = 79u;
            mul8[0][2] = 61u;
            mul8[0][3] = 201u;
            for (int i = 0; i < 4; ++i) {
                mul32[1][i] = uint32_t(Euclid(1ull << 32, mul32[0][i]));
                mul8[1][i] = uint8_t(Euclid(1ull << 8, mul8[0][i]));
                // cout << "Verified: " << (mul8[0][i] * mul8[1][i]) % (1u << 8) << endl;
                // cout << "Verified: " << mul32[0][i] * mul32[1][i] << endl;
            }
            random_device rd;
            uint64_t _seed1 = (0x00000000FFFFFFFF & (uint64_t)rd()) << 32;
            _seed1 += rd() | 1;
            uint64_t _seed2 = (0x00000000FFFFFFFF & (uint64_t)rd()) << 32;
            _seed2 += rd() | 1;

            _seed1 = hash_ns::mangleSeed1;
            _seed2 = hash_ns::mangleSeed2;

            mangler = new Mangler(_seed1, _seed2);    // 2 seeds needed
        }

        void mangling(const uint8_t *key, uint8_t *new_key)
        {
            // convert(key, new_key, 0);
            *(uint32_t *)new_key = mangler->MangleCharTable(*(const uint32_t *)key);
        }

        void demangling(const uint8_t *key, uint8_t *original_key)
        {
            // convert(key, original_key, 1);
            *(uint32_t *)original_key = mangler->ReverseCharTable(*(const uint32_t *)key);
        }
    };


    static constexpr int r = 1;

    int w;
    int mem_by_rev;
    int mem_k_ary;

    int mapped_sl;
    int map_sh[H][4];
    uint8_t map_xor[H][4];

    int* counters[H];
    KeyMangler km;
    
    KAry normal_sk;

    uint8_t compute_small_hash(uint8_t original_key, int i, int j)
    {
        if (keylen == 4) {
            return uint8_t(((original_key * uint8_t(map_sh[i][j])) >> 5) % ((1u << mapped_sl)));
        } else if (keylen == 13) {
            return uint8_t((((original_key * uint8_t(map_sh[i][j])) >> 6) ^ map_xor[i][j]) % ((1u << mapped_sl)));
        }
    }

    void rev_insert(const uint8_t * key, int f = 1)
    {
        for (int i = 0; i < H; ++i) {
            int pos = 0;
            for (int j = 0; j < keylen; ++j) {
                uint8_t hash_val = compute_small_hash(key[j], i, j);
                pos = (pos << mapped_sl) + hash_val;
            }
            counters[i][pos] += f;
        }
    }

    void modular_potentials(
        const vector<int> heavy_buckets[H], int w, set<uint8_t> & Iw, set<int> Bw[256][H])
    {
        set<int> L[H][256];
        int hits[256];
        memset(hits, 0, sizeof(hits));

        for (int i = 0; i < H; ++i) {
            for (auto & t: heavy_buckets[i]) {
                uint8_t min_pos = (t >> ((keylen - w - 1) * mapped_sl)) % ((1u << mapped_sl));
//                int cnt = 0;
                for (int x = 0; x < 256; ++x) {
                    uint8_t hash_val = compute_small_hash(uint8_t(x), i, w);
//                    uint8_t hash_val = ((uint8_t(x) * uint8_t(map_sh[i][w])) >> 3) % ((1u << mapped_sl));
//                    if (hash_val == 0) cnt++;
                    if (min_pos == hash_val) {
                        if (L[i][x].empty()) {
                            hits[x]++;
                        }
                        L[i][x].insert(t);
                    }
                }
//                cout << "asd: " << cnt << endl;
            }
        }

        for (int i = 0; i < 256; ++i) {
            if (hits[i] >= H - r) {
                Iw.insert(uint8_t(i));
                for (int j = 0; j < H; ++j) {
                    Bw[i][j] = L[j][i];
                }
            }
        }
    }

public:
    string name;

    ReversibleSketch(uint8_t keylen_, uint32_t mem_in_bytes_, int H_ = 6): keylen(keylen_), mem_in_bytes(mem_in_bytes_)//, H(H_)
    {		
		random_device rd;
        for (int i = 0; i < H; ++i) {
            for (int j = 0; j < keylen; ++j) {
                map_sh[i][j] = (rd() % 256) / 2 * 2 + 1;
                map_xor[i][j] = (rd() % 256);
            }
        }

        stringstream name_buf;
        name_buf << "Rev@" << mem_in_bytes;
        name = name_buf.str();

        w = keylen == 4 ? (1 << 12) : (1 << 13);
    	mem_by_rev = w * H * 4;
    	mem_k_ary = mem_in_bytes - mem_by_rev;
    	normal_sk = KAry(keylen, mem_k_ary, H);

    	mapped_sl = keylen == 4 ? 3 : 1;

		for(int i = 0; i < H ; i ++)
		{
			counters[i] = new int [w];
        	memset(counters[i], 0, sizeof(int)*w);
        }
    }

    void copy_configurations(ReversibleSketch & another)
    {
        memcpy(map_sh, another.map_sh, sizeof(another.map_sh));
        memcpy(map_xor, another.map_xor, sizeof(another.map_xor));
        normal_sk.copy_configurations(another.normal_sk);
    }

    void insert(const uint8_t * key, int f = 1)
    {
        uint8_t new_key[4];
        km.mangling(key, new_key);
//        uint8_t de_key[keylen];
//        km.demangling(new_key, de_key);
//        if (memcmp(key, de_key, keylen) != 0)
//            cout << "err" << endl;
        normal_sk.insert(key, f); // insert original key to standard k-ary
        rev_insert(new_key, f); // insert mangling key to reversible k-ary
    }

    void get_heavy_hitters(int threshold, std::vector<pair<string, uint32_t> >& ret)
    {
        // collect heavy buckets
        vector<int> heavy_buckets[H];
        int max_t = 0;
        cout << normal_sk.sum << endl;
        for (int i = 0; i < H; ++i) {
            int tcnt = 0;
            for (int j = 0; j < w; ++j) {
                int val = counters[i][j];
                val = (val - normal_sk.sum / double(w)) / (1 - 1.0 / w);
                if (abs(val) >= threshold) {
                	//cout << abs(val) << endl;
                    heavy_buckets[i].emplace_back(j);
                    tcnt++;
                }
            }
            max_t = std::max(max_t, tcnt);
            cout << "Find " << tcnt << " counters" << endl;
        }

        // collect Iw and Bw
        set<uint8_t> I[keylen];
        set<int> B[keylen][256][H];
        for (int i = 0; i < keylen; ++i) {
            modular_potentials(heavy_buckets, i, I[i], B[i]);
        }

        set<pair<vector<uint8_t>, vector<set<int>>>> A_old, A_new;
        for (auto & x: I[0]) {
            for (auto & y: I[1]) {
                set<int> res[H];
                int non_empty_count = 0;
                for (int i = 0; i < H; ++i) {
                    vector<int> temp(max_t);
                    auto ptr = set_intersection(B[0][x][i].begin(), B[0][x][i].end(),
                                    B[1][y][i].begin(), B[1][y][i].end(), temp.begin());
                    if (ptr - temp.begin() > 0) {
                        non_empty_count += 1;
                        res[i] = set<int>(temp.begin(), temp.end());
                    }
                }
                if (non_empty_count >= H - r) {
                    A_new.insert(make_pair(vector<uint8_t>({x, y}), vector<set<int>>(res, res + H)));
                }
            }
        }


        for (int z = 2; z < keylen; ++z) {
            A_old = A_new;
            A_new.clear();
            for (auto & kv: A_old) {
                for (auto & y: I[z]) {
                    set<int> res[H];
                    int non_empty_count = 0;
                    for (int i = 0; i < H; ++i) {
                        vector<int> temp(max_t);
                        auto ptr = set_intersection(kv.second[i].begin(), kv.second[i].end(),
                                         B[z][y][i].begin(), B[z][y][i].end(), temp.begin());
                        if (ptr - temp.begin() > 0) {
                            non_empty_count += 1;
                            res[i] = set<int>(temp.begin(), temp.end());
                        }
                    }
                    if (non_empty_count >= H - r) {
                        vector<uint8_t> new_vec = kv.first;
                        new_vec.push_back(y);
                        A_new.insert(make_pair(new_vec, vector<set<int>>(res, res + H)));
                    }
                }
            }

            cout << "z = " << z << "done." << endl;
            cout << "before: " << A_old.size() << endl;
            cout << "end: " << A_new.size() << endl << endl;
        }

        // A_new contains the candidate set
        ret.clear();
        for (auto & kv: A_new) {
            uint8_t n_key[keylen], dm_key[keylen];
            for (int i = 0; i < keylen; ++i) {
                n_key[i] = kv.first[i];
            }
            km.demangling(n_key, dm_key);
            double verified_val = normal_sk.query(dm_key);
            if (std::abs(verified_val) >= threshold) {
            // if (1) {
                ret.emplace_back(make_pair(string((const char *)dm_key, keylen), std::abs(verified_val)));
            }
        }
    }
};

#endif //STREAMMEASUREMENTSYSTEM_REVERSIBLESKETCH_H
