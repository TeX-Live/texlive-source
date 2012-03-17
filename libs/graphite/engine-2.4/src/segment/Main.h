/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: Main.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Main.header file for the Graphite engine.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRAPHITE_H
#define GRAPHITE_H 1

//:End Ignore

#define NO_EXCEPTIONS 1

// It's okay to use functions that were declared deprecated by VS 2005:
#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable: 4996) // warning: function was declared deprecated
#pragma warning(disable: 4702) // unreachable code
#pragma warning(push)
#pragma warning(disable: 4548) // expression has no effect

#endif

#include "GrCommon.h"

//:>********************************************************************************************
//:>	Interfaces.
//:>********************************************************************************************
#include "GrData.h"

//:>********************************************************************************************
//:>	Implementations.
//:>********************************************************************************************

#ifndef _WIN32
#include "GrMstypes.h"
#ifndef HAVE_FABSF
float fabsf(float x);
#endif
#endif
#include "GrDebug.h"

// For reading the font (FieldWorks's approach) and transduction logging:
#include <fstream>
#include <iostream>

#include <vector>
////#include <algorithm>
#include <string>
#include <cstring>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

// gAssert should be used for any kind of assertions that can be caused by a corrupted font,
// particularly those that won't be caught when loading the tables.
#define gAssert(x) Assert(x)
// When testing the error mechanism (because the process of bringing up the assertion dialog
// interferes with FW repainting the invalid window:
////#define gAssert(x) if (!(x)) Warn("corrupted font?")

// Internal headers.
#include "FileInput.h"

// Public headers.
#include "GrResult.h"
//////#include "IGrGraphics.h"
#include "ITextSource.h"
#include "IGrJustifier.h"
#include "IGrEngine.h"
#include "GrConstants.h"
#include "GrFeature.h"

// External helper classes.
#include "GrExt.h"


// Forward declarations.
namespace gr
{
    class GrTableManager;
	class Segment;
	class GrEngine;
}

// Define after GrExt.h to avoid conflict with FW Rect class.
namespace gr
{
/*
struct Point
{
	float x;
	float y;

	Point()
	{
		x = y = 0;
	}

	Point(POINT & p)
	{
		x = (float)p.x;
		y = (float)p.y;
	}
};


struct Rect
{
	float top;
	float bottom;
	float left;
	float right;

	Rect()
	{
		top = bottom = left = right = 0;
	};

	Rect(RECT & r)
	{
		top = (float)r.top;
		bottom = (float)r.bottom;
		left = (float)r.left;
		right = (float)r.right;
	};
};
*/
}; // namespace gr

#include "GrFeatureValues.h"
#include "GrSlotState.h"

#include "SegmentAux.h"

// Internal headers
#include "GrCharStream.h"
#include "GrGlyphTable.h"
#include "GrClassTable.h"
#include "GrPseudoMap.h"
#include "GrSlotStream.h"
#include "GrFSM.h"
#include "GrPass.h"
#include "GrTableManager.h"
#include "FontCache.h"


// Public headers
#include "Font.h"
#include "GraphiteProcess.h"
#include "GrEngine.h"
#include "FontFace.h"
#include "Segment.h"
#include "SegmentPainter.h"

//#ifdef _WIN32
//#include <hash_map>
//#include "WinFont.h"
//#include "WinSegmentPainter.h"
//#endif

// Internal headers
#include "TtfUtil.h"
//#include "GrUtil.h"

#include "GrWrappers.h"

//#include "IGrDebug.h"
//#include "GrSegmentDebug.h"
//#include "GrEngineDebug.h"

/////<<<<<<< .mine
// clashes with non gr stuff
// using namespace gr;
///////>>>>>>> .r98

#endif // GRAPHITE_H

