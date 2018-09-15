/* -*-  Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/* GaloisField.cc	-- Yin Zhang Wed Jun 23 2004
 *
 * 
 */

#include "os_galoisField.h"

GaloisField *gf32 = new GaloisField(32);

uint64 GaloisField::primTable[33] =
	{
	1,
	(1<<1) + 1,
	(1ULL<<2) + (1ULL<<1) + 1,
	(1ULL<<3) + (1ULL<<1) + 1,
	(1ULL<<4) + (1ULL<<1) + 1,
	(1ULL<<5) + (1ULL<<2) + 1,
	(1ULL<<6) + (1ULL<<1) + 1,
	(1ULL<<7) + (1ULL<<3) + 1,
	(1ULL<<8) + (1ULL<<4) + (1ULL<<3) + (1ULL<<2) + 1,
	(1ULL<<9) + (1ULL<<4) + 1,
	(1ULL<<10) + (1ULL<<3) + 1,
	(1ULL<<11) + (1ULL<<2) + 1,
	(1ULL<<12) + (1ULL<<6) + (1ULL<<4) + (1ULL<<1) + 1,
	(1ULL<<13) + (1ULL<<4) + (1ULL<<3) + (1ULL<<1) + 1,
	(1ULL<<14) + (1ULL<<10) + (1ULL<<6) + (1ULL<<1) + 1,
	(1ULL<<15) + (1ULL<<1) + 1,
	(1ULL<<16) + (1ULL<<12) + (1ULL<<3) + (1ULL<<1) + 1,
	(1ULL<<17) + (1ULL<<3) + 1,
	(1ULL<<18) + (1ULL<<7) + 1,
	(1ULL<<19) + (1ULL<<5) + (1ULL<<2) + (1ULL<<1) + 1,
	(1ULL<<20) + (1ULL<<3) + 1,
	(1ULL<<21) + (1ULL<<2) + 1,
	(1ULL<<22) + (1ULL<<1) + 1,
	(1ULL<<23) + (1ULL<<5) + 1,
	(1ULL<<24) + (1ULL<<7) + (1ULL<<2) + (1ULL<<1) + 1,
	(1ULL<<25) + (1ULL<<3) + 1,
	(1ULL<<26) + (1ULL<<6) + (1ULL<<2) + (1ULL<<1) + 1,
	(1ULL<<27) + (1ULL<<5) + (1ULL<<2) + (1ULL<<1) + 1,
	(1ULL<<28) + (1ULL<<3) + 1,
	(1ULL<<29) + (1ULL<<1) + 1,
	(1ULL<<30) + (1ULL<<6) + (1ULL<<4) + (1ULL<<1) + 1,
	(1ULL<<31) + (1ULL<<3) + 1,
	(1ULL<<32) + (1ULL<<7) + (1ULL<<6) + (1ULL<<2) + 1,
	};

void GaloisField::LongDiv(uint64 a, uint64 b, uint64 &q, uint64 &r) const
	{
	int i, alen, blen;

	assert(b != 0);

	for(alen = 0; (1ULL<<alen) <= a; alen++);
	alen--;

	for(blen = 0; (1ULL<<blen) <= b; blen++);
	blen--;

	q = 0; r = a;
	for (i = alen-blen; i >= 0; i--)
		{
		q <<= 1;
		if ( r & (1ULL<<(blen+i)) )
			{
			r ^= (b<<i);
			q += 1;
			}
		}
	}

void GaloisField::ExtendedEuclid(uint64 a, uint64 b,
				   uint64 &g, uint64 &u, uint64 &v) const
	{
	uint64 g0, g1, u0, u1, v0, v1, q, r, tmp;

	g0 = a; g1 = b;
	u0 = 1; u1 = 0;
	v0 = 0; v1 = 1;
	while (g1 != 0)
		{
		LongDiv(g0, g1, q, r);
		g0 = g1; g1 = r;
		tmp = u0; u0 = u1; u1 = Sub(tmp, Mul(u1, q));
		tmp = v0; v0 = v1; v1 = Sub(tmp, Mul(v1, q));
		}

	g = g0 ; u = u0 ; v = v0;
	}

void GaloisField::Test(int N1)
	{
	int i;
	uint64 j;
	
	for (i = 1; i < n && i < N1; i++)
		{
		j = Inv(uint64(i));
		fprintf(stderr, "GaloisField(2^%d)::Test(N): Inv(%d)=%llu Mul(%d,%llu)=%llu\n",
			n, i, j, i, j, Mul(i,j));
		}
	}
