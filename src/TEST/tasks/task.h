#ifndef _TASK_H_
#define _TASK_H_

#include "../algorithms/algorithms.h"
#include "../data/dataManager.h"
#include <string>
using std::string;

class Task
{
public:
    string name;
    Task(string name): name(name) {}
    virtual ~Task() = 0;
    virtual void run(Algorithm *algo, DataManager *dataManager, DataType dataType) = 0;
};

typedef enum{
    TASK_TYPE_FLOW_SIZE = 0,
    TASK_TYPE_THROUGHPUT
}TaskType;

#endif //_TASK_H_