/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: debug.h
Responsibility: 
Last reviewed: 

Description:
	Assert and debug definitions.
----------------------------------------------------------------------------------------------*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <cassert>

namespace gr
{

#ifdef _DEBUG
	#undef DEBUG
	#define DEBUG 1
	#undef NDEBUG
	#define DEFINE_THIS_FILE static char THIS_FILE[] = __FILE__;
	#undef THIS_FILE
	#define THIS_FILE __FILE__
#else  //!_DEBUG
	#undef DEBUG
	#define DEFINE_THIS_FILE
	
	//#define Debug(foo)
#endif //!_DEBUG

#if 0
//#ifdef DEBUG

//#include <windows.h>

	typedef void (WINAPI * Pfn_Assert)(const char * pszExp, const char * pszFile, int nLine,
		HMODULE hmod);
	typedef Pfn_Assert Pfn_Warn;

	/*__declspec(dllimport)*/ void WINAPI WarnProc(const char * pszExp, const char * pszFile,
		int nLine, bool fCritical, HMODULE hmod);
	/*__declspec(dllimport)*/ void WINAPI AssertProc(const char * pszExp, const char * pszFile,
		int nLine, bool fCritical, HMODULE hmod);

	__declspec(dllimport) Pfn_Assert WINAPI SetAssertProc(Pfn_Assert pfnAssert);
	__declspec(dllimport) Pfn_Warn WINAPI SetWarnProc(Pfn_Warn pfnWarn);
	__declspec(dllimport) int WINAPI HideWarnings(bool f);
	__declspec(dllimport) int WINAPI HideAsserts(bool f);
	__declspec(dllimport) int WINAPI HideErrors(bool f);

	void WINAPI WarnHrProc(HRESULT hr, const char * pszFile, int nLine, bool fCritical);
	void WINAPI WarnProcLocal(const char * pszExp, const char * pszFile, int nLine,
		bool fCritical);
	void WINAPI AssertProcLocal(const char * pszExp, const char * pszFile, int nLine,
		bool fCritical);

	#define Assert(exp) ((exp) || (AssertProcLocal(#exp, THIS_FILE, __LINE__, false), 0))
	#define AssertC(exp) ((exp) || (AssertProcLocal(#exp, THIS_FILE, __LINE__, true), 0))
	#define AssertMsg(exp, msg) ((exp) || (AssertProcLocal(msg, THIS_FILE, __LINE__, false), 0))
	#define AssertMsgC(exp, msg) ((exp) || (AssertProcLocal(msg, THIS_FILE, __LINE__, true), 0))

	#undef Warn
	#define Warn(psz) (void)(WarnProcLocal(psz, THIS_FILE, __LINE__, false))
	#define WarnC(psz) (void)(WarnProcLocal(psz, THIS_FILE, __LINE__, true))

	#undef WarnIf
	#define WarnIf(exp) (void)(!(exp) || (WarnProcLocal("(" #exp ") is true", THIS_FILE, \
		__LINE__, false), 0))
	#define WarnIfC(exp) (void)(!(exp) || (WarnProcLocal("(" #exp ") is true", THIS_FILE, \
		__LINE__, true), 0))

	#undef WarnUnless
	#define WarnUnless(exp) (void)((exp) || (WarnProcLocal("(" #exp ") is false", \
		THIS_FILE, __LINE__, false), 0))
	#define WarnUnlessC(exp) (void)((exp) || (WarnProcLocal("(" #exp ") is false", \
		THIS_FILE, __LINE__, true), 0))

	#undef WarnHr
	#define WarnHr(hr) ((SUCCEEDED(hr) || (WarnHrProc(hr, THIS_FILE, __LINE__, false), 0)), \
		(hr))
	#define WarnHrC(hr) ((SUCCEEDED(hr) || (WarnHrProc(hr, THIS_FILE, __LINE__, true), 0)), \
		(hr))

	class IgnoreWarnings
	{
	public:
		IgnoreWarnings()
			{ HideWarnings(true); }
		~IgnoreWarnings()
			{ HideWarnings(false); }
	};

	class IgnoreAsserts
	{
	public:
		IgnoreAsserts()
			{ HideAsserts(true); }
		~IgnoreAsserts()
			{ HideAsserts(false); }
	};

	class IgnoreErrors
	{
	private:
		IgnoreWarnings iw;
		IgnoreAsserts ia;
	};

	#define Debug(exp) exp
	#define DoAssert(exp) Assert(exp)
	#define DoAssertC(exp) AssertC(exp)
	#define DoAssertHr(hr) Assert(SUCCEEDED(hr))
	#define DoAssertHrC(hr) AssertC(SUCCEEDED(hr))
#else
	#define Assert(exp) assert(exp)
	#define AssertC(exp)
	#define AssertMsg(exp, msg)
	#define AssertMsgC(exp, msg)
	#define Warn(psz)
	#define WarnC(psz)
	#define WarnIf(exp)
	#define WarnIfC(exp)
	#define WarnUnless(exp)
	#define WarnUnlessC(exp)
	#define WarnHr(hr) (hr)
	#define WarnHrC(hr) (hr)
        #define Debug(exp)
	#define DoAssert(exp) (exp)
	#define DoAssertC(exp) (exp)
	#define DoAssertHr(hr) (hr)
	#define DoAssertHrC(hr) (hr)
#endif //DEBUG

#define AssertPtr(pv) Assert(ValidReadPtr(pv))
#define AssertPtrN(pv) Assert(!(pv) || ValidReadPtr(pv))
#define AssertArray(pv, cv) Assert((cv) >= 0 && ValidReadPtrSize((pv), sizeof(*(pv)) * (cv)))
#define AssertArrayN(pv, cv) Assert((cv) >= 0 && (!(pv) || ValidReadPtrSize((pv), sizeof(*(pv)) * (cv))))
#define AssertPtrSize(pv, cb) Assert((cb) >= 0 && ValidReadPtrSize((pv), cb))

#define AssertPfn(pfn) Assert(!IsBadCodePtr((FARPROC)(pfn)))
#define AssertPfnN(pfn) Assert(!(pfn) || !IsBadCodePtr((FARPROC)(pfn)))

#define AssertPsz(psz) Assert(ValidPsz(psz))
#define AssertPszN(psz) Assert(!(psz) || ValidPsz(psz))

#define AssertBstr(bstr) Assert(ValidBstr(bstr))
#define AssertBstrN(bstr) Assert(!(bstr) || ValidBstr(bstr))

#define AssertObj(pv) Assert(ValidReadPtr(pv) && (pv)->AssertValid())
#define AssertObjN(pv) Assert(!(pv) || ValidReadPtr(pv) && (pv)->AssertValid())

/*----------------------------------------------------------------------------------------------
	This class implements custom output to the debugger.  The results of Watch() can be 
	viewed on a line in the Watch window.  Output() writes to the Debugger Output window.
	Calls to Output() should be included in the Watch() function as needed.  Watch()
	is executed every time the debugger displays this class in the Watch window.
	To use it:

	1) Put the following line in:
			\program files\Microsoft Visual Studio\Common\MSDev98\bin\AutoExp.dat
		in the [AutoExpand] section:
			DebugWatch=<WatchNV(),su>

	2) In the class from which you want custom debug output, add the equivalent of:
		#ifdef DEBUG
		class Dbw1 : public DebugWatch { virtual OLECHAR * Watch(); ... };
		Dbw1 m_dbw1;
		#endif //DEBUG

	3) Implement the Watch() function to display the data you want to see, bracketed by
	   #ifdef DEBUG / #endif.  Note that Output() is LIMITED to 10 pointer-sized arguments.  
	   If you must have more, then call _CrtDbgReport() directly.  Note also that 
	   _CrtDbgReport() is not implemented for wide characters, so neither is Output().  
----------------------------------------------------------------------------------------------*/
#if 0
//#ifdef DEBUG
class DebugWatch 
{
public:
	int m_nSerial; // increment and display this for positive affirmation of refresh
	DebugWatch() { m_nSerial = 0; }
	OLECHAR * WatchNV(); // debugger won't find it if it is virtual or inline.
	virtual OLECHAR * Watch() { return L"No DebugWatch string implemented"; }
	void Output (char *fmt, ...); // LIMITED to 10 pointer-sized arguments
};
#endif //DEBUG

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif
