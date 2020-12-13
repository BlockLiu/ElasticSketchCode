#ifndef _BOBHASH32_H
#define _BOBHASH32_H
#include <cstdio>
#include <random>
#include <vector>
#include <unordered_set>
using namespace std;

#define MAX_PRIME32 1229
#define MAX_BIG_PRIME32 50

class BOBHash32
{
public:
	BOBHash32();
	~BOBHash32();
	BOBHash32(uint32_t prime32Num);
	void initialize(uint32_t prime32Num);
	uint32_t run(const char * str, uint32_t len);	// produce a hash number
	static uint32_t get_random_prime_index()
	{
		random_device rd;
		return rd() % MAX_PRIME32;
	}

    static vector<uint32_t> get_random_prime_index_list(int n)
    {
        random_device rd;
        unordered_set<int> st;
        while (st.size() < n) {
            st.insert(rd() % MAX_PRIME32);
        }
        return vector<uint32_t>(st.begin(), st.end());
    }
private:
	uint32_t prime32Num;
};
#endif //_BOBHASH32_H
