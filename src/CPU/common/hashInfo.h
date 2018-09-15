#ifndef _HASHINFO_H
#define _HASHINFO_H
#include "common.h"

using namespace std;

typedef struct HashInfo {
  int field;
  int numHashValues;
  unsigned long long range;
  int rev; // 1 if reversible, 0 otherwise
} tHashInfo;

#endif
