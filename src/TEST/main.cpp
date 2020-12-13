#include <stdio.h>
#include "controller/controller.h"

#define HEAVY_MEM (150 * 1024)
#define BUCKET_NUM (HEAVY_MEM / 64)
#define TOT_MEM_IN_BYTES (600 * 1024)
typedef ElasticSketch<BUCKET_NUM,TOT_MEM_IN_BYTES> Elastic;

int main()
{
    char dataFile[] = "130000.dat";
    Controller controller;
    int maxItemNum = 3000000;

    Elastic *elastic = new Elastic();
    controller.performTask(dataFile, DATA_TYPE_SRC_IP, maxItemNum, (Algorithm*)elastic, TASK_TYPE_FLOW_SIZE);
    controller.performTask(dataFile, DATA_TYPE_SRC_IP, maxItemNum, (Algorithm*)elastic, TASK_TYPE_THROUGHPUT, false);
    controller.performTask(dataFile, DATA_TYPE_SRC_IP, maxItemNum, (Algorithm*)elastic, TASK_TYPE_FLOW_SIZE, false);
    delete elastic;

    int tot_mem_in_bytes = 600 * 1024;
    CMSketch *cm = new CMSketch(tot_mem_in_bytes, 3);
    controller.performTask(dataFile, DATA_TYPE_SRC_IP, maxItemNum, (Algorithm*)cm, TASK_TYPE_FLOW_SIZE);
    controller.performTask(dataFile, DATA_TYPE_SRC_IP, maxItemNum, (Algorithm*)cm, TASK_TYPE_THROUGHPUT, false);
    controller.performTask(dataFile, DATA_TYPE_SRC_IP, maxItemNum, (Algorithm*)cm, TASK_TYPE_FLOW_SIZE, false);
    delete cm;
}
