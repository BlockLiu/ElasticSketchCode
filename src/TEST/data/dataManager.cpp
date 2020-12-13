#include "dataManager.h"
#include <algorithm>
#include <string.h>

void DataManager::loadData(const char *dataFile, int maxItemNum)
{
    data.clear();
    FILE *fin = fopen(dataFile, "rb");
    if(fin == nullptr){
        printf("DataManager::loadData failed to open file %s\n", dataFile);
        exit(EXIT_FAILURE);
    }

    FIVE_TUPLE tmp_five_tuple;
    uint8_t buffer[21];
    int itemCnt = 0;
    while(fread(buffer, 1, 21, fin) == 21 && itemCnt++ < maxItemNum){
        memcpy(&tmp_five_tuple, buffer, sizeof(FIVE_TUPLE));
        data.push_back(tmp_five_tuple);
    }
    
    fclose(fin);
    // printf("DataManager::loadData successfully read in %s, %ld packets\n", dataFile, data.size());
}

int DataManager::getData(void *outData, DataType type, int maxItemNum)
{
    int itemNum = std::min(maxItemNum, (int)data.size());
    switch(type)
    {
        case DATA_TYPE_FIVE_TUPLE:
            return this->get_fiveTuples((FIVE_TUPLE*)outData, itemNum);
        case DATA_TYPE_SRC_IP:
            return this->get_srcIPs((int*)outData, itemNum);
        case DATA_TYPE_SRC_AND_DST_IP:
            return this->get_srcAndDstIPs((uint64_t*)outData, itemNum);
        default:
            printf("DataManager::getData wrong type of data\n");
    }
    return EXIT_FAILURE;
}

int DataManager::getDataSize(DataType type)
{
    switch (type)
    {
        case DATA_TYPE_FIVE_TUPLE:
            return sizeof(FIVE_TUPLE);
        case DATA_TYPE_SRC_IP:
            return sizeof(int);
        case DATA_TYPE_SRC_AND_DST_IP:
            return sizeof(uint64_t);
        default:
            printf("DataManager::getDataSize wrong type of data\n");
    }
    return EXIT_FAILURE;
}

int DataManager::getDataLen()
{
    return data.size();
}

int DataManager::get_fiveTuples(FIVE_TUPLE *tuples, int maxItemNum)
{
    int itemNum = std::min(maxItemNum, (int)data.size());
    for(int i = 0; i < itemNum; ++i)
        memcpy(&tuples[i], &data[i], sizeof(FIVE_TUPLE));
    return itemNum;
}

int DataManager::get_srcIPs(int *srcIPs, int maxItemNum)
{
    int itemNum = std::min(maxItemNum, (int)data.size());
    for(int i = 0; i < itemNum; ++i)
        memcpy(&srcIPs[i], &data[i].srcIP, sizeof(int));
    return itemNum;
}

int DataManager::get_srcAndDstIPs(uint64_t *IPs, int maxItemNum)
{
    int itemNum = std::min(maxItemNum, (int)data.size());
    for(int i = 0; i < itemNum; ++i)
        memcpy(&IPs[i], (uint64_t*)(&data[i]), sizeof(uint64_t));
    return itemNum;
}

