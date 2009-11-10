/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001, 2003 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrCommon.h
Responsibility: Sharon Correll
Last reviewed:

	Common generic header file.

	This header file checks for the following compile time switches:

		USING_MFC
----------------------------------------------------------------------------------------------*/

#ifndef GRCOMMON_INCLUDED
#define GRCOMMON_INCLUDED


// Standard Headers.

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <climits>
//#include <exception>
#include <new>

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

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
	Simple types.
***********************************************************************************************/
typedef char	schar;

// TODO ShonK: Make generic use these where appropriate.
#ifdef UNICODE
typedef utf16 	achar;
#else // !UNICODE
typedef schar	achar;
#endif // UNICODE

typedef achar 	*Psz;
//typedef const achar * Pcsz;



// This is to make a signed isizeof operator, otherwise we get tons of warnings about
// signed / unsigned mismatches.
#define isizeof(T) (sizeof(T))

#define SizeOfArray(rgt) (isizeof(rgt) / isizeof(rgt[0]))


/***********************************************************************************************
	Tests for valid strings and pointers.
***********************************************************************************************/
inline bool ValidPsz(const data16 *pszw)
{
	// TODO ShonK: Determine if IsBadStringPtrW is implemented on Win9x.
	return pszw != 0 && !GrIsBadStringPtrW(pszw, 0x10000000);
}

inline bool ValidPsz(const schar *pszs)
{
	return pszs != 0 && !GrIsBadStringPtrA(pszs, 0x10000000);
}

template<typename T> inline bool ValidReadPtr(T *pt)
{
	return pt != 0 && !GrIsBadReadPtr(pt, isizeof(T));
}

template<typename T> inline bool ValidWritePtr(T *pt)
{
	return pt != 0 && !GrIsBadWritePtr(pt, isizeof(T));
}

inline bool ValidReadPtrSize(const void *pv, int cb)
{
	if (cb < 0)	return false;
	if (cb == 0)	return true;

	return pv != 0 && !GrIsBadReadPtr(pv, cb);
}

inline bool ValidWritePtrSize(void *pv, int cb)
{
//	if (!bstr || ::IsBadReadPtr((byte *)bstr - isizeof(int), isizeof(int) + isizeof(OLECHAR)))
//		return false;
//	int cb = ((int *)bstr)[-1];
//	if (::IsBadReadPtr((byte *)bstr - isizeof(int), isizeof(int) + isizeof(OLECHAR) + cb))
//		return false;
	if (cb < 0)	return false;
	if (cb == 0)	return true;

	return pv != 0 && !GrIsBadWritePtr(pv, cb);
}


} // namespace gr


// TODO Remove these as soon as they are no longer needed by GrCompiler:
//#include "UtilMem.h"
//#include "UtilRect.h"
//#include "UtilString.h"
//#include "UtilVector.h"

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif // !GRCOMMON_INCLUDED

