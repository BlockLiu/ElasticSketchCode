/* -*-  Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
// $Header: /home/zhichun/Research/CVS/RevSketch/mangler.h,v 1.2 2005/01/30 00:59:42 ashish Exp $
//
// Implementation of 2-wise independent hash functions
//

#ifndef mangler_h
#define mangler_h

#include "common.h"
#include "os_galoisField.h"

class Mangler {
public:
	uint64 seed1, seed2;

	Mangler(uint64 a1, uint64 a2)
		{
			// gf32 is initialized in GaloisField.c
			seed1 = a1;
			seed2 = a2;

			InitCharTable(gf32);
			InitShortTable(gf32);
		}
	
	~Mangler()
		{
		}

	// mangle using char-table
	uint32 MangleCharTable(uint32 k)
		{
		int32views x;

		x.as_int32 = k;
		return
			char_mgltbl0[x.as_int8s[0]] ^
			char_mgltbl1[x.as_int8s[1]] ^
			char_mgltbl2[x.as_int8s[2]] ^
			char_mgltbl3[x.as_int8s[3]];
		}

	// mangle using short-table
	uint32 MangleShortTable(uint32 k)
		{
		int32views x;

		x.as_int32 = k;
		return
			short_mgltbl0[x.as_int16s[0]] ^
			short_mgltbl1[x.as_int16s[1]];
		}

	// reverse using char-table
	uint32 ReverseCharTable(uint32 k)
		{
		int32views x;

		x.as_int32 = k;
		return
			char_revtbl0[x.as_int8s[0]] ^
			char_revtbl1[x.as_int8s[1]] ^
			char_revtbl2[x.as_int8s[2]] ^
			char_revtbl3[x.as_int8s[3]];
		}

	// reverse using short-table
	uint32 ReverseShortTable(uint32 k)
		{
		int32views x;

		x.as_int32 = k;
		return
			short_revtbl0[x.as_int16s[0]] ^
			short_revtbl1[x.as_int16s[1]];
		}

	void TestCorrectness(uint32 N1);
	void TestSpeed(uint32 N1);

protected:

	void InitCharTable(GaloisField *gf);
	void InitShortTable(GaloisField *gf);
	
private:
	
	// data structures for short-table
	uint32 short_mgltbl0[65536];
	uint32 short_mgltbl1[65536];
	
	uint32 short_revtbl0[65536];
	uint32 short_revtbl1[65536];
	
	// data structures for char-table
	uint32 char_mgltbl0[256];
	uint32 char_mgltbl1[256];
	uint32 char_mgltbl2[256];
	uint32 char_mgltbl3[256];
	uint32 char_revtbl0[256];
	uint32 char_revtbl1[256];
	uint32 char_revtbl2[256];
	uint32 char_revtbl3[256];
};

#endif // mangler_h
