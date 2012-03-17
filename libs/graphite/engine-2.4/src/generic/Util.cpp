
#include "GrCommon.h"
#include "GrDebug.h"


namespace gr
{

/*----------------------------------------------------------------------------------------------
	Swap two blocks of size cb starting at pv1 and pv2. Doesn't handle overlapping blocks.
----------------------------------------------------------------------------------------------*/
void SwapBytes(void * pv1, void * pv2, int cb)
{
	Assert((byte *)pv1 + cb <= (byte *)pv2 || (byte *)pv2 + cb <= (byte *)pv1);
	AssertPtrSize(pv1, cb);
	AssertPtrSize(pv2, cb);

#ifdef NO_ASM

	byte *pb1 = (byte *)pv1;
	byte *pb2 = (byte *)pv2;
	byte b;

	while (--cb >= 0)
	{
		b = *pb1;
		*pb1++ = *pb2;
		*pb2++ = b;
	}

#else // !NO_ASM

	__asm
		{
		// edi -> memory to swap, first pointer
		// esi -> memory to swap, second pointer

		mov		edi,pv1
		mov		esi,pv2

		mov		ecx,cb
		shr		ecx,2
		jz		LBytes

LIntLoop:
		mov		eax,[edi]
		mov		ebx,[esi]
		mov		[edi],ebx
		mov		[esi],eax

		add		edi,4
		add		esi,4
		dec		ecx
		jnz		LIntLoop;

LBytes:
		mov		ecx,cb
		and		ecx,3
		jz		LDone

LByteLoop:
		mov		al,[edi]
		mov		bl,[esi]
		mov		[edi],bl
		mov		[esi],al
		inc		edi
		inc		esi
		dec		ecx
		jnz		LByteLoop

LDone:
		}

#endif //!NO_ASM
}

/*----------------------------------------------------------------------------------------------
	Fills a block of memory with the given short value.
----------------------------------------------------------------------------------------------*/
void FillShorts(void * pv, short sn, int csn)
{
	AssertPtrSize(pv, csn * static_cast<int>(sizeof(short)));

#ifdef NO_ASM

	short * psn = (short *)pv;
	short * psnLim = psn + csn;

	while (psn < psnLim)
		*psn++ = sn;

#else // !NO_ASM

	__asm
		{
		// Setup the registers for using REP STOS instruction to set memory.
		// NOTE: Alignment does not effect the speed of STOS.
		//
		// edi -> memory to set
		// eax = value to store in destination
		// direction flag is clear for auto-increment

		mov		edi,pv
		mov		ax,sn
		mov		ecx,csn

		mov		edx,ecx
		and		edx,1
		jz		LInts

		// set 1 short
		stosw

LInts:
		shr		ecx,1
		jz		LDone

		mov		ebx,eax
		shl		eax,16
		mov		ax,bx

		rep		stosd
LDone:
		}

#endif //!NO_ASM
}

/***********************************************************************************************
	Integer utilities.
***********************************************************************************************/

/*
 * table of powers of 2, and largest prime smaller than each power of 2
 *   n    2**n       prime      diff
 *  --- ----------  ----------  ----
 *   2:          4           3  ( -1)
 *   3:          8           7  ( -1)
 *   4:         16          13  ( -3)
 *   5:         32          31  ( -1)
 *   6:         64          61  ( -3)
 *   7:        128         127  ( -1)
 *   8:        256         251  ( -5)
 *   9:        512         509  ( -3)
 *  10:       1024        1021  ( -3)
 *  11:       2048        2039  ( -9)
 *  12:       4096        4093  ( -3)
 *  13:       8192        8191  ( -1)
 *  14:      16384       16381  ( -3)
 *  15:      32768       32749  (-19)
 *  16:      65536       65521  (-15)
 *  17:     131072      131071  ( -1)
 *  18:     262144      262139  ( -5)
 *  19:     524288      524287  ( -1)
 *  20:    1048576     1048573  ( -3)
 *  21:    2097152     2097143  ( -9)
 *  22:    4194304     4194301  ( -3)
 *  23:    8388608     8388593  (-15)
 *  24:   16777216    16777213  ( -3)
 *  25:   33554432    33554393  (-39)
 *  26:   67108864    67108859  ( -5)
 *  27:  134217728   134217689  (-39)
 *  28:  268435456   268435399  (-57)
 *  29:  536870912   536870909  ( -3)
 *  30: 1073741824  1073741789  (-35)
 *  31: 2147483648  2147483647  ( -1)
 *  32: 4294967296  4294967291	( -5)
 */
const static unsigned int g_rguPrimes[] = {
	3, 7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093,	8191, 16381, 32749, 65521, 131071,
	262139, 524287, 1048573, 2097143, 4194301, 8388593, 16777213, 33554393, 67108859,
	134217689, 268435399, 536870909, 1073741789, 2147483647, 4294967291U
};


/*----------------------------------------------------------------------------------------------
	Returns the prime in g_rguPrimes that is closest to u.
----------------------------------------------------------------------------------------------*/
unsigned int GetPrimeNear(unsigned int u)
{
	int cu = sizeof(g_rguPrimes) / sizeof(unsigned int);
	int iuMin;
	int iuLim;
	int iu;

	for (iuMin = 0, iuLim = cu; iuMin < iuLim; )
	{
		iu = (iuMin + iuLim) / 2;
		if (u > g_rguPrimes[iu])
			iuMin = iu + 1;
		else
			iuLim = iu;
	}
	Assert(iuMin == cu || (iuMin < cu && u <= g_rguPrimes[iuMin]));
	Assert(iuMin == 0 || (iuMin > 0 && u > g_rguPrimes[iuMin - 1]));

	if (!iuMin)
		return g_rguPrimes[0];
	if (iuMin == cu)
		return g_rguPrimes[cu - 1];
	if (g_rguPrimes[iuMin] - u < u - g_rguPrimes[iuMin - 1])
		return g_rguPrimes[iuMin];
	return g_rguPrimes[iuMin - 1];
}


/*----------------------------------------------------------------------------------------------
	Returns the prime in g_rguPrimes that is larger than u or is the largest in the list.
----------------------------------------------------------------------------------------------*/
unsigned int GetLargerPrime(unsigned int u)
{
	int cu = sizeof(g_rguPrimes) / sizeof(unsigned int);
	int iuMin;
	int iuLim;
	int iu;

	for (iuMin = 0, iuLim = cu; iuMin < iuLim; )
	{
		iu = (iuMin + iuLim) / 2;
		if (u >= g_rguPrimes[iu])
			iuMin = iu + 1;
		else
			iuLim = iu;
	}
	Assert(iuMin == cu || (iuMin < cu && u < g_rguPrimes[iuMin]));
	Assert(iuMin == 0 || (iuMin > 0 && u >= g_rguPrimes[iuMin - 1]));

	if (iuMin == cu)
		return g_rguPrimes[cu - 1];
	return g_rguPrimes[iuMin];
}


/*----------------------------------------------------------------------------------------------
	Returns the prime in g_rguPrimes that is smaller than u or is the smallest in the list.
----------------------------------------------------------------------------------------------*/
unsigned int GetSmallerPrime(unsigned int u)
{
	int cu = sizeof(g_rguPrimes) / sizeof(unsigned int);
	int iuMin;
	int iuLim;
	int iu;

	for (iuMin = 0, iuLim = cu; iuMin < iuLim; )
	{
		iu = (iuMin + iuLim) / 2;
		if (u > g_rguPrimes[iu])
			iuMin = iu + 1;
		else
			iuLim = iu;
	}
	Assert(iuMin == cu || (iuMin < cu && u <= g_rguPrimes[iuMin]));
	Assert(iuMin == 0 || (iuMin > 0 && u > g_rguPrimes[iuMin - 1]));

	if (!iuMin)
		return g_rguPrimes[0];
	return g_rguPrimes[iuMin - 1];
}

}

