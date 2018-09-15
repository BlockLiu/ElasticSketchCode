/* -*-  Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
// $Header: /home/zhichun/Research/CVS/RevSketch/mangler.cpp,v 1.2 2005/01/30 00:59:42 ashish Exp $
//
// Implementation of 2-wise independent hash functions
//

#include "os_mangler.h"

void Mangler::InitCharTable(GaloisField *gf)
	{
		uint64 i, a, b;

	//a = os_rand32bit();
		a = seed1;
	b = gf->Inv(a);
	
	for (i = 0; i < 256; i++)
		{
		char_mgltbl3[i] = uint32(gf->Mul(i<<24,a));
		char_mgltbl2[i] = uint32(gf->Mul(i<<16,a));
		char_mgltbl1[i] = uint32(gf->Mul(i<<8,a));
		char_mgltbl0[i] = uint32(gf->Mul(i,a));
		char_revtbl3[i] = uint32(gf->Mul(i<<24,b));
		char_revtbl2[i] = uint32(gf->Mul(i<<16,b));
		char_revtbl1[i] = uint32(gf->Mul(i<<8,b));
		char_revtbl0[i] = uint32(gf->Mul(i,b));		
		// cout << i << endl;
		}
	}

void Mangler::InitShortTable(GaloisField *gf)
	{
		uint64 i, a, b;

 	//a = os_rand32bit();
		a = seed2;
	b = gf->Inv(a);
	
	for (i = 0; i < 65536; i++)
		{
		short_mgltbl1[i] = uint32(gf->Mul(i<<16,a));
		short_mgltbl0[i] = uint32(gf->Mul(i,a));
		short_revtbl1[i] = uint32(gf->Mul(i<<16,b));
		short_revtbl0[i] = uint32(gf->Mul(i,b));
		}
	}

void Mangler::TestCorrectness(uint32 N1)
	{
	uint32 i, j;
	for (i = 0; i < N1; i++)
		{
		j = MangleCharTable(i);
		fprintf(stderr, "Mangle(%08x) = %08x\tReverse(%08x) = %08x\tCharTable\n",
			i, j, j, ReverseCharTable(j));
		j = MangleShortTable(i);
		fprintf(stderr, "Mangle(%08x) = %08x\tReverse(%08x) = %08x\tShortTable\n",
			i, j, j, ReverseShortTable(j));
		}
	}
	
void Mangler::TestSpeed(uint32 N1)
	{
	double start_time, end_time;
	double t0, t1;
	uint32 x;
	uint32 i;

	start_time = os_current_time();
	for (i = 0, x = 0; i < N1; i++)
		x ^= MangleCharTable(i);
	end_time = os_current_time();
	t0 = end_time - start_time;
	
	start_time = os_current_time();
	for (i = 0, x = 0; i < N1; i++)
		x ^= MangleShortTable(i);
	end_time = os_current_time();
	t1 = end_time - start_time;

	fprintf(stderr, "Mangler::TestSpeed(%d) charTime=%.6f shortTime=%.6f x=%u\n", N1, t0, t1, x);
	}
