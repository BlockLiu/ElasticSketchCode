#include "task_throughput.h"
#include <chrono>
#include <iostream>
using namespace::std::chrono;
using std::cout;
using std::endl;

TaskThroughput::TaskThroughput(): Task("Throughput"), test_cycle(100){}

TaskThroughput::~TaskThroughput(){}

void TaskThroughput::run(Algorithm *algo, DataManager *dataManager, DataType dataType)
{
    assert(algo && dataManager);
    /* get data */
    int itemNum = dataManager->getDataLen();
    int keylen = dataManager->getDataSize(dataType);
    uint8_t *data = (uint8_t*)malloc(itemNum * keylen);
    dataManager->getData(data, dataType, itemNum);

    int insertTimes = 0;
    auto t1 = steady_clock::now();
    for(int cycle = 0; cycle < test_cycle; ++cycle){
        algo->clear();
        for(int i = 0, offset = 0; i < itemNum; ++i, offset += keylen)
            insertTimes |= algo->insert(&data[offset], keylen);
    }
    auto t2 = steady_clock::now();
    auto t3 = duration_cast<microseconds>(t2 - t1).count();
    cout << "insertTimes = " << insertTimes << endl;
    
    cout << algo->name << "-duration: " << 1.0 * t3 / test_cycle << " microseconds per cycle" << endl;
    cout << algo->name << "-throughput: " << itemNum /(1.0 * t3 / test_cycle) << " MIPs" << endl;
}

