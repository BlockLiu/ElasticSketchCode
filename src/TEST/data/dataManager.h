#ifndef _DATA_MANAGER_H_
#define _DATA_MANAGER_H_

#include <stdio.h>
#include <stdint.h>
#include <vector>
using std::vector;

#pragma pack (1)
typedef struct 
{
    int srcIP;
    int dstIP;
    short srcPort;
    short dstPort;
    char protocol;
}FIVE_TUPLE;
#pragma pack ()

typedef enum{
    DATA_TYPE_FIVE_TUPLE = 0,
    DATA_TYPE_SRC_IP,
    DATA_TYPE_SRC_AND_DST_IP
}DataType;

class DataManager
{
    vector<FIVE_TUPLE> data;
public:
    void loadData(const char *dataFile, int maxItemNum);
    int getData(void *outData, DataType type, int maxItemNum);
    int getDataSize(DataType type);
    int getDataLen();
private:
    int get_fiveTuples(FIVE_TUPLE *tuples, int maxItemNum);
    int get_srcIPs(int *srcIPs, int maxItemNum);
    int get_srcAndDstIPs(uint64_t *IPs, int maxItemNum);
};

#endif //_DATA_MANAGER_H_
