#ifndef _TASK_THROUGHPUT_H_
#define _TASK_THROUGHPUT_H_

#include <assert.h>
#include "task.h"

class TaskThroughput: public Task
{
    int test_cycle;
public:
    TaskThroughput();
    ~TaskThroughput();

    void run(Algorithm *algo, DataManager *dataManager, DataType DataType);
};

#endif //_TASK_THROUGHPUT_H_