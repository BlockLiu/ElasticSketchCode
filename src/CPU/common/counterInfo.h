#ifndef _COUNTERINFO_H
#define _COUNTERINFO_H
#include "common.h"

using namespace std;

typedef struct CounterInfo {
  int sketch_id;
  int numRows;
  int countersPerRow;
  int counterSize;
  int updateType;
  int nextOffset;
} tCounterInfo;

#endif
