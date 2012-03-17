/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 2004 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Main.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
    Header files to include in the Graphite regression test program.
-------------------------------------------------------------------------------*//*:End Ignore*/

#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRCOMPILER_H
#define GRCOMPILER_H 1

#define NO_EXCEPTIONS 1

// For some reason, WINVER defaults to zero and is not set to something reasonable soon enough:
#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0500
#endif
#endif

#ifdef _WIN32
// To allow call to IsDebuggerPresent:
#define _WIN32_WINNT WINVER
#endif

//:>********************************************************************************************
//:>	Include files
//:>********************************************************************************************
#include "stdafx.h"
////#include "resource.h"
// #include <hash_map>
#include <fstream>
#include <iostream>
#include <vector>
////#include <algorithm>
#include <string>
#include <cstring>

#ifdef _WIN32
#include "windows.h"
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#endif // _WIN32
#include <assert.h>

////using std::max;
////using std::min;

#include "GrCommon.h"
#include "GrPlatform.h"

////////#include "LgCharPropsStub.h"

#include "GrConstants.h"
///#include "TtfUtil.h"
///#include "Tt.h"

#include "GrClient.h"
#include "ITextSource.h"
#include "SimpleTextSrc.h"
#include "IGrEngine.h"
#include "IGrJustifier.h"
#include "GrJustifier.h"
#include "SegmentAux.h"
#include "Font.h"
// #include "WinFont.h"
#include "FileFont.h"
#include "Segment.h"
#include "SegmentPainter.h"
// #include "WinSegmentPainter.h"

#include "TestCase.h"
#include "RtTextSrc.h"


//:>********************************************************************************************
//:>	Functions
//:>********************************************************************************************
void RunTests(int numberOfTests, TestCase * ptcaseList);
int RunOneTestCase(TestCase * ptcase, Segment * psegPrev, Segment ** ppsegRet, RtTextSrc ** pptsrcRet);
void OutputError(TestCase * ptcase, std::string strErr, int i = -1);
void OutputErrorWithValues(TestCase * ptcase, std::string strErr, int i,
	int valueFound, int valueExpected);
void OutputErrorAux(TestCase * ptcase, std::string strErr, int i,
	bool showValues, int valueFound, int valueExpected);
bool WriteToLog(std::string str, int i = -1);
bool WriteToLog(std::string str, int i,
	bool showValues, int valueFound, int valueExpected);
bool WriteToLog(int n);

#endif //!WRCOMPILER_H

