#ifndef _TASK_FLOW_SIZE_H_
#define _TASK_FLOW_SIZE_H_

#include <assert.h>
#include <unordered_map>
#include "task.h"
using std::unordered_map;

class TaskFlowSize: public Task
{
    typedef unordered_map<string,int> Freq;
public:
    TaskFlowSize();
    ~TaskFlowSize();

    void run(Algorithm *algo, DataManager *dataManager, DataType dataType);
private:
    void getRealFreq(Freq &freq, uint8_t *data, int keylen, int itemNum);
    void getKeys(vector<string> &flowIDs, uint8_t *data, int keylen, int itemNum);
    double computeARE(Freq &real_freq, Freq &est_freq);
};

#endif //_TASK_FLOW_SIZE_H_