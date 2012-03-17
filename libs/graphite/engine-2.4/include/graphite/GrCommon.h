/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001, 2003 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrCommon.h
Responsibility: Sharon Correll
Last reviewed:

	Common Graphite header file.
----------------------------------------------------------------------------------------------*/

#ifndef GRCOMMON_INCLUDED
#define GRCOMMON_INCLUDED

// Standard Headers.

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <climits>
#include <new>

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
///#include <stdexcept> -- possibly needed for std::string Xran and Xlen functions??

// Uncomment this to allow multiple versions of gr to coexist
// in the same program e.g. pangographite with gtk uses namespace gr
// mozilla graphite defines this to gr2moz incase the 2 libraries are 
// incompatible.
// Provided the client includes GrClient.h first this #define is
// picked up by all files.

//#define gr gr2

// Project headers
#include "GrPlatform.h"

#ifndef GrAssert
#include <cassert>
#define GrAssert(exp) assert(exp)
#endif

using std::max;
using std::min;

namespace gr
{


/***********************************************************************************************
	Tests for valid strings and pointers.
***********************************************************************************************/
inline bool ValidPsz(const data16 *pszw)
{
	return pszw != 0 && !GrIsBadStringPtrW(pszw, 0x10000000);
}

inline bool ValidPsz(const char *pszs)
{
	return pszs != 0 && !GrIsBadStringPtrA(pszs, 0x10000000);
}

template<typename T> inline bool ValidReadPtr(T *pt)
{
	return pt != 0 && !GrIsBadReadPtr(pt, sizeof(T));
}

template<typename T> inline bool ValidWritePtr(T *pt)
{
	return pt != 0 && !GrIsBadWritePtr(pt, sizeof(T));
}

inline bool ValidReadPtrSize(const void *pv, size_t cb)
{
	if (cb == 0)	return true;

	return pv != 0 && !GrIsBadReadPtr(pv, cb);
}

inline bool ValidWritePtrSize(void *pv, size_t cb)
{
	if (cb == 0)	return true;

	return pv != 0 && !GrIsBadWritePtr(pv, cb);
}


} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif // !GRCOMMON_INCLUDED

