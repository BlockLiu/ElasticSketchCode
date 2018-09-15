#include "common.h"

using namespace std;

// http://en.wikipedia.org/wiki/Universal_hashing is great!
uint32 os_dietz_thorup32(uint32 x, uint32 bins, uint64 a, uint64 b){
  // ((ax mod 2**62) + b) mod 2 ** 62, hi 32, pad 0 on left
  // = (ax + b) mod 2 ** 62, hi 32, pad 0 on the left

  return ((uint32) ((a*x+b) >> 32)) % bins;
  // mod 64 then top 32 bits 
  // (this impl. okay for little endian)
  // wiki says just ax .. is 2-universal
  // why is it strongly universal/ 2-wise indep.?
}

// 2-universal hashing, assumes bins power of 2, very fast
uint8 os_dietz8to3(uint8 x, uint8 a) {
  return (((a*x)  >> 5) & 7);
}
// mod 8 then top 3 bits


uint32 reversible4096(uint32 value, uint32 bins, uint64 hasha) {
  uint32 j, index, tmp; 



  uint8 a, x;  
  int64views vv, vha;
  //cout << "hashing value " << value << "\n";
  //cout << "hashing value " << value << "\n";
  vv.as_int64 = value;
  vha.as_int64 = hasha;
  index = 0;
  
  for (j = 0; j < 4; j++) {
      x = vv.as_int8s[j];
      a = vha.as_int8s[j];
      /*cout << "x: " << (uint32) (x & 0xFF)\
	<< ", a: " << (uint32) (a & 0xFF)\
	<< ", b: " << (uint32) (b & 0xFF) << "\n";*/
      tmp = os_dietz8to3(x, a) & 0xFF;
      //cout << "tmp: " << tmp << "\n";
      tmp = tmp << 3*j;
      index += tmp;
    }
  return index;

}


uint32 os_dietz64to32(uint64 x, uint64 a) {
  if (!(a%2)) a--;
  //cout << "a*x (lower 64?): " << a*x << "\n";
  //cout << "a*x & MAXUINT32 (lower32): " << (a*x & (MAXUINT32)) << "\n";
  //cout << "a*x >> 32 (high32, maybe padded with 1s on right): " << (a*x >> 32) << "\n";
  //cout << "a*x >> (uint32) (32  & MAXUINT32 (high32)): " << (uint32) ((a*x >> 32) & (MAXUINT32)) << "\n";
  // all work as expected .. but to be safe & MAXUINT32
  return (((a*x) >> 32) & MAXUINT32);
}


// ----------- from Reversible Sketch Code at http://www.zhichunli.org/software/download.php?file=RevSketch-1.0.tar.gz 

double os_current_time()
{
  struct timeval tv;
  assert ( gettimeofday(&tv, 0) >= 0 );


  return double(tv.tv_sec) + double(tv.tv_usec) / 1e6;
}

// Generates a 32-bit random string
// Note that rand() is not random enough -- it has a period of 2^32.
uint32 os_rand32bit()
{
  uint32 base = 0;
  uint32 i;

  for (i=1; i<=2; i++)
    base = (base<<16) | rand();
  return(base);
}

/*
void os_ipint2string(unsigned long int a, char  *b)
{
  struct in_addr tmp;
  tmp.s_addr = a;
  strcpy(b,inet_ntoa(tmp));
}
*/
