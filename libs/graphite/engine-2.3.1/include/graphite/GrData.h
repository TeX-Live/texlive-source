/*--------------------------------------------------------------------*//*:Ignore this sentence.
Copyright (C) 1999, 2001 SIL International. All rights reserved.

Distributable under the terms of either the Common Public License or the
GNU Lesser General Public License, as specified in the LICENSING.txt file.

File: GrData.h
Responsibility: Sharon Correll
Last reviewed: Not yet.

Description:
	Data structures needed by the Graphite engine, particularly for argument passing.
	NOT to be used within FieldWorks.

Special Note:
        This file needs to be used by C files. Please do not use C++ style comments; use
        ONLY C style comments in this file to make the C compiler happy.
----------------------------------------------------------------------------------------------*/
#ifdef _MSC_VER
#pragma once
#endif
#ifndef GRDATA_INCLUDED
#define GRDATA_INCLUDED

#include "GrCommon.h"

/*
//:End Ignore
 */

#include "GrStructs.h"

namespace gr
{

struct Point
{
	float x;
	float y;

	Point()
	{
		x = y = 0;
	}

#if defined(_WIN32)
	Point(POINT & p)
	{
		x = (float)p.x;
		y = (float)p.y;
	}
#endif
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

#if defined(_WIN32)
	Rect(RECT & r)
	{
		top = (float)r.top;
		bottom = (float)r.bottom;
		left = (float)r.left;
		right = (float)r.right;
	};
#endif
};

enum {
	kttvOff = 0,
	kttvForceOn = 1,
	kttvInvert = 2
};

//typedef struct tagLgParaRenderProps
//{
//    int dummy;
//} LgParaRenderProps;

typedef enum tagSegEnd
{	kestNoMore		= 0,
	kestMoreLines	= kestNoMore + 1,
	kestHardBreak	= kestMoreLines + 1,
	kestBadBreak	= kestHardBreak + 1,
	kestOkayBreak	= kestBadBreak + 1,
	kestWsBreak		= kestOkayBreak + 1,
	kestMoreWhtsp	= kestWsBreak + 1,
	kestNothingFit	= kestMoreWhtsp + 1
} SegEnd;

typedef enum tagLgIPDrawMode // TODO: remove
{	kdmNormal	= 0,
	kdmSplitPrimary	= kdmNormal + 1,
	kdmSplitSecondary	= kdmSplitPrimary + 1
} LgIPDrawMode;

typedef enum tagLgIpValidResult
{	kipvrOK	= 0,
	kipvrBad	= kipvrOK + 1,
	kipvrUnknown	= kipvrBad + 1
} LgIpValidResult;

typedef enum tagLineBrk
{	klbNoBreak		=  0,
	klbWsBreak		= 10,
	klbWordBreak	= 15,
	klbHyphenBreak	= 20,
	klbLetterBreak	= 30,
	klbClipBreak	= 40
} LineBrk;

typedef enum tagTrWsHandling
{	ktwshAll	= 0,
	ktwshNoWs	= ktwshAll + 1,
	ktwshOnlyWs	= ktwshNoWs + 1
} TrWsHandling;

typedef enum tagUtfType
{
	kutf8		= 0,
	kutf16		= kutf8 + 1,
	kutf32		= kutf16 + 1
} UtfType;

enum tagFlushMode
{
	kflushAuto		= 0,
	kflushManual	= kflushAuto + 1
};

typedef enum ScriptDirCode
{	kfsdcNone			= 0,
	kfsdcHorizLtr		= 1,
	kfsdcHorizRtl		= 2,
	kfsdcVertFromLeft	= 4,
	kfsdcVertFromRight	= 8
} ScriptDirCode;

typedef enum tagFwTextColor
{	kclrWhite	= 0xffffff,
	kclrBlack	= 0,
	kclrRed		= 0x0000ff,
	kclrGreen	= 0x00ff00,
	kclrBlue	= 0xff0000,
/*
//	kclrYellow	= 0x00ffff,
//	kclrMagenta	= 0xff00ff,
//	kclrCyan	= 0xffff00,
 */
	kclrReserved1	= 0x80000000,
	kclrReserved2	= 0x80000001,
	kclrTransparent	= 0xc0000000
} FwTextColor;


typedef enum FwSuperscriptVal
{	kssvOff	= 0,
	kssvSuper	= 1,
	kssvSub	= 2
} FwSuperscriptVal;


// Glyph attributes for justification
typedef enum tagJustGlyphAttr
{
	kjgatStretch = 1,
	kjgatShrink,
	kjgatWeight,
	kjgatStep,
	kjgatChunk,
	kjgatWidth,
	kjgatBreak,
	// pseudo-attributes for handling steps:
	kjgatStretchInSteps,
	kjgatWidthInSteps,
	// also metrics:
	kjgatAdvWidth,
	kjgatAdvHeight,
	kjgatBbLeft,
	kjgatBbRight,
	kjgatBbTop,
	kjgatBbBottom	
} JustGlyphAttr;	// Hungarian: jgat

class GrSlotState;
typedef union {
    int nValue;
	short smallint[2];	// min = smallint[0], max = smallint[1]
    GrSlotState * pslot;
} u_intslot;

/*****
#undef ATTACH_GUID_TO_CLASS
#if defined(__cplusplus)
#define ATTACH_GUID_TO_CLASS(type, guid, cls) \
	type __declspec(uuid(#guid)) cls;
#else // !defined(__cplusplus)
#define ATTACH_GUID_TO_CLASS(type, guid, cls)
#endif // !defined(__cplusplus)

#ifndef DEFINE_COM_PTR
#define DEFINE_COM_PTR(cls)
#endif

#undef GENERIC_DECLARE_SMART_INTERFACE_PTR
#define GENERIC_DECLARE_SMART_INTERFACE_PTR(cls, iid) \
	ATTACH_GUID_TO_CLASS(interface, iid, cls); \
	DEFINE_COM_PTR(cls);
*****/

} // namespace gr

#if defined(GR_NO_NAMESPACE)
using namespace gr;
#endif

#endif  /* !GRDATA_INCLUDED */
