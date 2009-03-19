/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrStructs.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Copyright (C) 1999 by SIL International. All rights reserved.

Description:
    Cross-platform structure definitions.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRSTRUCTS_INCLUDED
#define GRSTRUCTS_INCLUDED
#include "GrPlatform.h"

namespace gr
{

//:End Ignore

typedef struct tagGrCharProps
{
    unsigned long clrFore;
    unsigned long clrBack;
    int dympOffset;
    int ws;
    int ows;
    byte fWsRtl;
    int nDirDepth;
    byte ssv;
    byte ttvBold;
    byte ttvItalic;
    int dympHeight;
    wchar_t szFaceName[ 32 ];
    wchar_t szFontVar[ 64 ];
} GrCharProps;

// Used to pass feature information among the Graphite engine and the application.
typedef struct tagFeatureSetting
{
	int id;
	int value;
} FeatureSetting;

/*****
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
****/

} // namespace gr


#endif // !STRUCTS_INCLUDED

