#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "../data/dataManager.h"

#include "../tasks/task.h"
#include "../tasks/task_flowSize.h"
#include "../tasks/task_throughput.h"

#include "../algorithms/algorithms.h"
#include "../algorithms/elasticSketch/ElasticSketch.h"
#include "../algorithms/elasticSketch/ElasticSketch.cpp"
#include "../algorithms/cmSketch/cmsketch.h"

class Controller
{
public:
    Controller(){}
    ~Controller(){}

    void performTask(char *dataFile, DataType dataType, int maxItemNum, Algorithm* algo, TaskType taskType, bool needInsert=true)
    {
        assert(dataFile && algo);
        // algo->clear();
        DataManager dataManager;
        dataManager.loadData(dataFile, maxItemNum);

        if(needInsert)
            this->insert(algo, &dataManager, dataType);

        Task *task = this->createTask(taskType);
        printf("********* algo:%s ********* task:%s *********\n", algo->name.c_str(), task->name.c_str());

        task->run(algo, &dataManager, dataType);
        delete task;

        printf("\n");
    } 
private:
    Task* createTask(TaskType type)
    {
        Task* task = nullptr;
        switch(type)
        {
            case TASK_TYPE_FLOW_SIZE:
                task = (Task*)new TaskFlowSize();
                break;
            case TASK_TYPE_THROUGHPUT:
                task = (Task*)new TaskThroughput();
                break;
            default:
                printf("Controller::createTask wrong type of task\n");
        }
        return task;
    }

    void insert(Algorithm *algo, DataManager *dataManager, DataType dataType)
    {
        assert(algo && dataManager);
        /* get data */
        int itemNum = dataManager->getDataLen();
        int keylen = dataManager->getDataSize(dataType);
        uint8_t *data = (uint8_t*)malloc(itemNum * keylen);
        dataManager->getData(data, dataType, itemNum);

        for(int i = 0, offset = 0; i < itemNum; ++i, offset += keylen)
            algo->insert(&data[offset], keylen);
        free(data);
        // printf("Controller:insert insertion ends\n");
    }
};

#endif //_CONTROLLER_H_