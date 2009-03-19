/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Debug.cpp
Responsibility: Darrell Zook
Last reviewed: Not yet.

Description:
	This file provides the standard debug error functions.
----------------------------------------------------------------------------------------------*/

#ifdef _WIN32
#include <windows.h>
#endif
//#include "common.h"
//#include "Main.h"
#include "GrDebug.h"
#ifdef _MSC_VER
#pragma hdrstop
#endif
#undef THIS_FILE
DEFINE_THIS_FILE

//#include "ModuleEntry.h"
#include <stdio.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif


#if 0
//#ifdef DEBUG

extern HMODULE Win32DllGetModuleHandle();

void WINAPI AssertProcLocal(const char * pszExp, const char * pszFile, int nLine,
	bool fCritical)
{
	AssertProc(pszExp, pszFile, nLine, fCritical, Win32DllGetModuleHandle());
}

void WINAPI WarnProcLocal(const char * pszExp, const char * pszFile, int nLine,
	bool fCritical)
{
	WarnProc(pszExp, pszFile, nLine, fCritical, Win32DllGetModuleHandle());
}

void WINAPI WarnHrProc(HRESULT hr, const char * pszFile, int nLine, bool fCritical)
{
	char szBuffer[MAX_PATH + 25];
	sprintf(szBuffer, "HRESULT[0x%08x]--", hr);
	int cch = lstrlen(szBuffer);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 0, szBuffer + cch,
		sizeof(szBuffer) - cch, NULL);
	szBuffer[max(lstrlen(szBuffer) - 2, 0)] = 0;
	WarnProcLocal(szBuffer, pszFile, nLine, fCritical);
}


OLECHAR * DebugWatch::WatchNV() 
{ 
	// JT: we need this, otherwise if dealing with a deleted object or null pointer
	// we may try to call a virtual function where there is no valid VTABLE pointer.
	// In the debugger we can't be sure of not dereferencing a bad pointer.
	if (!::_CrtIsValidPointer(this, isizeof(this), TRUE ))
		return L"A very bad object pointer";
	// We could also be referencing memory that is trashed (e.g., the object has
	// been deleted, or deleted and replaced by something else).
	if (!dynamic_cast<DebugWatch *>(this))
		return L"A bad object pointer";
	return Watch(); 
}

void DebugWatch::Output (char *fmt, ...) // LIMITED to 10 pointer-sized arguments
{
	struct args { void * pv[10]; };
	_CrtDbgReport (_CRT_WARN, NULL, NULL, __FILE__, fmt, *(args*)(&fmt+1));
}

#endif // DEBUG
