/* -*-  Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/* GaloisField.h	-- Yin Zhang
 *
 * My implementation of GF(2^32)
 *
 * $Header: /home/zhichun/Research/CVS/RevSketch/galoisField.h,v 1.1.1.1 2005/01/13 17:22:28 zhichun Exp $
 */

#ifndef galoisfield_h
#define galoisfield_h

//#include "util.h"
#include "common.h"


class GaloisField {
public:
	// constructor
	GaloisField(int arg_n)
		{
		n = arg_n;
		assert(n <= 32 && n >= 1);
		prim = primTable[n];
		}

	// destructor
	~GaloisField()
		{
		}

	// compute a+b
	uint64 Add(uint64 a, uint64 b) const
		{
		return a ^ b;
		}

	// compute a-b
	uint64 Sub(uint64 a, uint64 b) const
		{
		return a ^ b;
		}

	// compute a*b
	uint64 Mul(uint64 a, uint64 b) const
		{
		uint64 bitn = 1ULL << n;
		uint64 p = a;
		uint64 r = 0;
	
		while (b)
			{
			if (b & 1) r ^= p;
			b >>= 1;
			p <<= 1;
			if (p & bitn) p ^= prim;
			}
		return r;
		}

	// compute a/b
	uint64 Div(uint64 a, uint64 b) const
		{
		uint64 q, r;
		LongDiv(a, b, q, r);
		return q;
		}

	// compute a%b
	uint64 Mod(uint64 a, uint64 b) const
		{
		uint64 q, r;
		LongDiv(a, b, q, r);
		return r;
		}


	// find b s.t. Mul(b,a) = 1
	uint64 Inv(uint64 a) const
		{
		uint64 g, u, v;

		// a*u + prim*v = 1 => u = 1/a
		ExtendedEuclid(a, prim, g, u, v);
		return u;
		}

	// compute q=a/b, r=a-b*q
	void LongDiv(uint64 a, uint64 b, uint64 &q, uint64 &r) const;

	// Extended Euclid's Algorithm: find u, v, s.t. a*u + b*v = g
	void ExtendedEuclid(uint64 a, uint64 b,
			    uint64 &g, uint64 &u, uint64 &v) const;

	void Test(int N1 = 1024);
	
private:

	int n;       // GF(2^n)
	uint64 prim; // primitive polynomial

	/*---------------------------------------------------------------------

	  Below are the table of primitive polynomials for
	  GF(2^n), n = 0, ..., 32
	  
	  1
	  x + 1
	  x2 + x + 1
	  x3 + x + 1
	  x4 + x + 1
	  x5 + x2 + 1
	  x6 + x + 1
	  x7 + x3 + 1
	  x8 + x4 + x3 + x2 + 1
	  x9 + x4 + 1
	  x10 + x3 + 1
	  x11 + x2 + 1
	  x12 + x6 + x4 + x + 1
	  x13 + x4 + x3 + x + 1
	  x14 + x10 + x6 + x + 1
	  x15 + x + 1
	  x16 + x12 + x3 + x + 1
	  x17 + x3 + 1
	  x18 + x7 + 1
	  x19 + x5 + x2 + x + 1
	  x20 + x3 + 1
	  x21 + x2 + 1
	  x22 + x + 1
	  x23 + x5 + 1
	  x24 + x7 + x2 + x + 1
	  x25 + x3 + 1
	  x26 + x6 + x2 + x + 1
	  x27 + x5 + x2 + x + 1
	  x28 + x3 + 1
	  x29 + x + 1
	  x30 + x6 + x4 + x + 1
	  x31 + x3 + 1
	  x32 + x7 + x6 + x2 + 1
	*/
	static uint64 primTable[33];
		
};

extern GaloisField *gf32;

#endif // galoisfield_h
