/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrResult.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Support for the GrResult and error handling in the Graphite engine. Also argument
	checking. Much of this is set up to parallel standard COM behavior.

Special Note:
        This file needs to be used by C files. Please do not use C++ style comments; use
        ONLY C style comments in this file to make the C compiler happy.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRRESULT_INCLUDED
#define GRRESULT_INCLUDED

/*
//:End Ignore
 */


namespace gr
{



/*:>********************************************************************************************/ 
/*:>	Result flags                                                                           */
/*:>********************************************************************************************/

/* These are return values for the main interface methods to indicate various kinds of          */
/* error conditions. To facilitate COM compatibiliy, they match the standard COM HRESULT codes. */
enum GrResult
{
	kresOk = 0,                     // S_OK
	kresFalse = 1,                  // S_FALSE
	kresFail = 0x80004005L,         // E_FAIL
	kresOutOfMemory = 0x8007000EL,  // E_OUTOFMEMORY
	kresInvalidArg = 0x80000002L,   // E_INVALIDARG
	kresReadFault = 0x80000001L,    // STG_E_READFAULT
	kresUnexpected = 0x80000003L,   // E_UNEXPECTED
	kresNotImpl = 0x80000004L,      // E_NOTIMPL
	kresPointer = 0x80004003L//,       // E_POINTER
};

#define ResultFailed(res) (res != kresOk)  /* && res != kresFalse */

#define ResultSucceeded(res) (res == kresOk)  /* || res == kresFalse */

enum FontErrorCode
{
	kferrOkay,
	kferrUninitialized,
	kferrUnknown,
	kferrFindHeadTable,
	kferrReadDesignUnits,
	kferrFindCmapTable,
	kferrLoadCmapSubtable,
	kferrCheckCmapSubtable,
	kferrFindNameTable,
	kferrLoadSilfTable,
	kferrLoadFeatTable,
	kferrLoadGlatTable,
	kferrLoadGlocTable,
	kferrReadSilfTable,
	kferrReadGlocGlatTable,
	kferrReadFeatTable,
	kferrBadVersion,
	kferrSileTableMismatch,
	kferrReadSillTable
};

/*:>********************************************************************************************/ 
/*:>	Error handling                                                                         */
/*:>********************************************************************************************/
/* These are set up as #defines so we can easily adjust them once we know how errors should    */
/* be handled.                                                                                 */

#define ReturnResult(res) return res

#ifdef GR_FW
#define THROW(res) ThrowInternalError(res)
#else
#define THROW(res) throw res
#endif /* GR_FW */


// #if !defined(NDEBUG)

#if (0)   // can't get E_POINTER to work so no debugging this way for now

} // namespace gr

#include <iostream>

namespace gr
{

#define WARN(warning)    (std::cerr << __FILE__ << ':' << __LINE__ << ": WARN: " << #warning, warning)

/*:>********************************************************************************************/ 
/*:>	Argument checking                                                                      */
/*:>********************************************************************************************/

#define ChkGrArgPtr(p) \
{ \
	AssertPtrN(p); \
	if (!p) \
		THROW(E_POINTER); \
} \

#define ChkGrOutPtr(p) \
{ \
	AssertPtrN(p); \
	if (!p) \
		THROW(E_POINTER); \
} \

#define ChkGrArgPtrN(p) \
	AssertPtrN(p); \

#define ChkGrArrayArg(prgv, cv) \
{ \
	AssertArray(prgv, cv); \
	if (cv && !prgv) \
		THROW(E_POINTER); \
} \

#define ChkGrBstrArg(bstr) \
	AssertBstr(bstr); \

#define ChkGrBstrArgN(bstr) \
	AssertBstrN(bstr); \

#else // debugging is off

#define WARN(warning)    (warning)

/*:>********************************************************************************************/ 
/*:>	Argument checking                                                                      */
/*:>********************************************************************************************/

#define ChkGrArgPtr(p)            (static_cast<void>(0))

#define ChkGrOutPtr(p)            (static_cast<void>(0))

#define ChkGrArgPtrN(p)           (static_cast<void>(0))

#define ChkGrArrayArg(prgv, cv)   (static_cast<void>(0))

#define ChkGrBstrArg(bstr)        (static_cast<void>(0))  

#define ChkGrBstrArgN(bstr)       (static_cast<void>(0))

#endif // !defined(NDEBUG)

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif  /* !GRRESULT_INCLUDED */
