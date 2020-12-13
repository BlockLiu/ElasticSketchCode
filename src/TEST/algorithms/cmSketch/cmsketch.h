#ifndef _CM_SKETCH_H_
#define _CM_SKETCH_H_

#include "../BOBHash32.h"
#include "../algorithms.h"

class CMSketch: public Algorithm
{
    uint32_t *counters;
    int width;
    int d;
    BOBHash32 *hash;
public:
    CMSketch(int tot_mem_in_bytes, int d);
    ~CMSketch();

    int insert(uint8_t *key, int keylen);
    int query(uint8_t *key, int keylen);
    void clear();
    void get_flowsize(vector<string> &flowIDs, unordered_map<string,int> &freq);
};


#endif //_CM_SKETCH_H_