#include "task_flowSize.h"
#include <unordered_set>
using std::unordered_set;

TaskFlowSize::TaskFlowSize(): Task("FlowSize"){}
TaskFlowSize::~TaskFlowSize(){}

void TaskFlowSize::run(Algorithm *algo, DataManager *dataManager, DataType dataType)
{
    assert(algo && dataManager);
    /* get data */
    int itemNum = dataManager->getDataLen();
    int keylen = dataManager->getDataSize(dataType);
    uint8_t *data = (uint8_t*)malloc(itemNum * keylen);
    dataManager->getData(data, dataType, itemNum);

    /* get real flow size */
    Freq real_freq;
    this->getRealFreq(real_freq, data, keylen, itemNum);
    vector<string> flowIDs;
    this->getKeys(flowIDs, data, keylen, itemNum);
    free(data);

    /* get estimated flow size */
    Freq est_freq;
    algo->get_flowsize(flowIDs, est_freq);

    /* get performance */
    double ARE = this->computeARE(real_freq, est_freq);
    printf("(Task:%s, Algo:%s): itemNum=%d, keylen=%d, ARE=%.6lf\n",
        this->name.c_str(), algo->name.c_str(), itemNum, keylen, ARE);
}

void TaskFlowSize::getRealFreq(Freq &freq, uint8_t *data, int keylen, int itemNum)
{
    freq.clear();
    for(int i = 0, offset = 0; i < itemNum; i++, offset += keylen){
        string str((const char*)(&data[offset]), keylen);
        freq[str]++;
    }
}
void TaskFlowSize::getKeys(vector<string> &flowIDs, uint8_t *data, int keylen, int itemNum)
{
    unordered_set<string> keys;
    for(int i = 0, offset = 0; i < itemNum; i++, offset += keylen){
        string str((const char*)(&data[offset]), keylen);
        keys.insert(str);
    }
    flowIDs.clear();
    for(auto it : keys)
        flowIDs.push_back(it);
    printf("TaskFlowSize::getKeys have %lu keys\n", flowIDs.size());
}
double TaskFlowSize::computeARE(Freq &real_freq, Freq &est_freq)
{
    double ARE = 0;
    for(auto it : real_freq)
    {
        int est_val = 0;
        if(est_freq.count(it.first))
            est_val = est_freq[it.first];
        ARE += std::abs(it.second - est_val) * 1.0 / it.second;
    }
    ARE /= real_freq.size();
    return ARE;
}
