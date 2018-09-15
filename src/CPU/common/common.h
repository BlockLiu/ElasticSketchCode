#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>
//#include <winsock2.h> 
//#include <windows.h>


#include "packet.h"
#include "counterInfo.h"
#include "hashInfo.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

#include <vector>
#include <map>
#include <queue>
#include <fstream>
#include <numeric>
#include <cmath>


// for OS_CounterRevAnalyze
#include <list>
#include <iostream>

//#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib, "wsock32.lib")

typedef unsigned long long  uint64;
typedef unsigned int        uint32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;

const uint64 MAXUINT64 = (uint64) (-1);
const uint32 MAXUINT32 = (uint32) (-1);
const uint16 MAXUINT16 = (uint16) (-1);
const uint8  MAXUINT8  = (uint8)  (-1);
const double E = (double) (2.71828182846);

const int FIELD_SRCIP = 1;
const int FIELD_DSTIP = 2;
const int FIELD_SRCIP_DSTIP = 12;

const int UPDATETYPE_SET = 1;
const int UPDATETYPE_INCREMENT = 2;
const int UPDATETYPE_NEXT = 3;

const int HASHTYPE_DIETZTHORUP32 = 1;
const int HASHTYPE_REVERSIBLE8TO3 = 2;

const uint32 REVERSIBLE_NUMROWS = 5;
const uint32 REVERSIBLE_COUNTERSPERROW = 4096;
const uint32 REVERSIBLE_ITERSIZE = 128;//64;
const uint32 REVERSIBLE_NUMDIVS = 4;
const uint32 REVERSIBLE_NUMKEYS = 256;
const uint32 REVERSIBLE_R = 2;
const uint32 REVERSIBLE_BINSPERDIV = 8;

typedef union {
  uint64 as_int64; 
  uint32 as_int32s[2];
  uint16 as_int16s[4];
  uint8  as_int8s[8];
} int64views;

typedef union {
  uint32 as_int32; 
  uint16 as_int16s[2];
  uint8  as_int8s[4];
} int32views;

namespace hash_ns{                                                                            
  const uint64 A[] = {59032440799460394,\
		      1380096083914250750,\
		      9216393848249138261,\
		      1829347879307711444,\
		      9218705108064111365};

  // later change to regular uint64 A[] and use randint64()
  const uint64 B[] = {832108633134565846,\
		      9207888196126356626,\
		      1106582827276932161,\
		      7850759173320174309,\
		      8297516128533878091};

  const uint64 mangleSeed1 = 38409845058049, mangleSeed2 = 9475047563245;
};



uint32 os_dietz_thorup32(uint32 x, uint32 bins, uint64 a, uint64 b);
uint8 os_dietz8to3(uint8 x, uint8 a);
uint32 reversible4096(uint32 value, uint32 bins, uint64 a);
uint32 os_dietz64to32(uint64 x, uint64 a);
// ----------- from Reversible Sketch Code at http://www.zhichunli.org/software/download.php?file=RevSketch-1.0.tar.gz 

double os_current_time();
// Generates a 32-bit random string
// Note that rand() is not random enough -- it has a period of 2^32.
uint32 os_rand32bit();

void os_ipint2string(unsigned long int a, char  *b);
#endif
