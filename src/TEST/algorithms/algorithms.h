#ifndef _ALGORITHMS_H_
#define _ALGORITHMS_H_

#include <string>
#include <vector>
#include <unordered_map>
using std::string;
using std::vector;
using std::unordered_map;

class Algorithm
{
public:
    string name;
    int insertTimes;
    Algorithm(string _name): name(_name), insertTimes(0) {}
    virtual ~Algorithm() = 0;

    virtual int insert(uint8_t *key, int keylen) = 0;
    virtual int query(uint8_t *key, int keylen) = 0;
    virtual void clear() = 0;

    virtual void get_flowsize(vector<string> &flowIDs, unordered_map<string,int> &freq) = 0;
};

// Algorithm::~Algorithm(){}

typedef enum{
    ALGO_TYPE_ELASTIC_SKETCH = 0,
    ALGO_TYPE_CM_SKETCH
}AlgoType;


#endif //_ALGORITHMS_H_
