/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2008 by SIL International
 copyright (c) 2009 by Jonathan Kew

 Written by Jonathan Kew

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the copyright holders
shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written
authorization from the copyright holders.
\****************************************************************************/

#ifndef __MATHTABLE_H__
#define __MATHTABLE_H__

#include "layout/OpenTypeTables.h"

typedef struct {
	le_int16 value;
	le_uint16 deviceTable;
} MathValueRecord;

typedef struct {
	le_uint32 version;
	le_uint16 mathConstants;
	le_uint16 mathGlyphInfo;
	le_uint16 mathVariants;
} MathTableHeader;

typedef struct {
	le_uint16 scriptPercentScaleDown;
	le_uint16 scriptScriptPercentScaleDown;
	le_uint16 delimitedSubFormulaMinHeight;
	le_uint16 displayOperatorMinHeight;
	MathValueRecord mathLeading;
	MathValueRecord axisHeight;
	MathValueRecord accentBaseHeight;
	MathValueRecord flattenedAccentBaseHeight;
	MathValueRecord subscriptShiftDown;
	MathValueRecord subscriptTopMax;
	MathValueRecord subscriptBaselineDropMin;
	MathValueRecord superscriptShiftUp;
	MathValueRecord superscriptShiftUpCramped;
	MathValueRecord superscriptBottomMin;
	MathValueRecord superscriptBaselineDropMax;
	MathValueRecord subSuperscriptGapMin;
	MathValueRecord superscriptBottomMaxWithSubscript;
	MathValueRecord spaceAfterScript;
	MathValueRecord upperLimitGapMin;
	MathValueRecord upperLimitBaselineRiseMin;
	MathValueRecord lowerLimitGapMin;
	MathValueRecord lowerLimitBaselineDropMin;
	MathValueRecord stackTopShiftUp;
	MathValueRecord stackTopDisplayStyleShiftUp;
	MathValueRecord stackBottomShiftDown;
	MathValueRecord stackBottomDisplayStyleShiftDown;
	MathValueRecord stackGapMin;
	MathValueRecord stackDisplayStyleGapMin;
	MathValueRecord stretchStackTopShiftUp;
	MathValueRecord stretchStackBottomShiftDown;
	MathValueRecord stretchStackGapAboveMin;
	MathValueRecord stretchStackGapBelowMin;
	MathValueRecord fractionNumeratorShiftUp;
	MathValueRecord fractionNumeratorDisplayStyleShiftUp;
	MathValueRecord fractionDenominatorShiftDown;
	MathValueRecord fractionDenominatorDisplayStyleShiftDown;
	MathValueRecord fractionNumeratorGapMin;
	MathValueRecord fractionNumDiisplayStyleGapMin;
	MathValueRecord fractionRuleThickness;
	MathValueRecord fractionDenominatorGapMin;
	MathValueRecord fractionDenomDisplayStyleGapMin;
	MathValueRecord skewedFractionHorizontalGap;
	MathValueRecord skewedFractionVerticalGap;
	MathValueRecord overbarVerticalGap;
	MathValueRecord overbarRuleThickness;
	MathValueRecord overbarExtraAscender;
	MathValueRecord underbarVerticalGap;
	MathValueRecord underbarRuleThickness;
	MathValueRecord underbarExtraDescender;
	MathValueRecord radicalVerticalGap;
	MathValueRecord radicalDisplayStyleVerticalGap;
	MathValueRecord radicalRuleThickness;
	MathValueRecord radicalExtraAscender;
	MathValueRecord radicalKernBeforeDegree;
	MathValueRecord radicalKernAfterDegree;
	le_uint16 radicalDegreeBottomRaisePercent;
} MathConstants;

typedef enum {
	unknown = -1,
	scriptPercentScaleDown = 0,
	scriptScriptPercentScaleDown,
	delimitedSubFormulaMinHeight,
	displayOperatorMinHeight,
	mathLeading,
	firstMathValueRecord = mathLeading,
	axisHeight,
	accentBaseHeight,
	flattenedAccentBaseHeight,
	subscriptShiftDown,
	subscriptTopMax,
	subscriptBaselineDropMin,
	superscriptShiftUp,
	superscriptShiftUpCramped,
	superscriptBottomMin,
	superscriptBaselineDropMax,
	subSuperscriptGapMin,
	superscriptBottomMaxWithSubscript,
	spaceAfterScript,
	upperLimitGapMin,
	upperLimitBaselineRiseMin,
	lowerLimitGapMin,
	lowerLimitBaselineDropMin,
	stackTopShiftUp,
	stackTopDisplayStyleShiftUp,
	stackBottomShiftDown,
	stackBottomDisplayStyleShiftDown,
	stackGapMin,
	stackDisplayStyleGapMin,
	stretchStackTopShiftUp,
	stretchStackBottomShiftDown,
	stretchStackGapAboveMin,
	stretchStackGapBelowMin,
	fractionNumeratorShiftUp,
	fractionNumeratorDisplayStyleShiftUp,
	fractionDenominatorShiftDown,
	fractionDenominatorDisplayStyleShiftDown,
	fractionNumeratorGapMin,
	fractionNumDisplayStyleGapMin,
	fractionRuleThickness,
	fractionDenominatorGapMin,
	fractionDenomDisplayStyleGapMin,
	skewedFractionHorizontalGap,
	skewedFractionVerticalGap,
	overbarVerticalGap,
	overbarRuleThickness,
	overbarExtraAscender,
	underbarVerticalGap,
	underbarRuleThickness,
	underbarExtraDescender,
	radicalVerticalGap,
	radicalDisplayStyleVerticalGap,
	radicalRuleThickness,
	radicalExtraAscender,
	radicalKernBeforeDegree,
	radicalKernAfterDegree,
	lastMathValueRecord = radicalKernAfterDegree,
	radicalDegreeBottomRaisePercent,
	lastMathConstant = radicalDegreeBottomRaisePercent
} mathConstantIndex;

typedef struct {
	le_uint16 minConnectorOverlap;
	Offset vertGlyphCoverage;
	Offset horizGlyphCoverage;
	le_uint16 vertGlyphCount;
	le_uint16 horizGlyphCount;
	Offset vertGlyphConstruction[ANY_NUMBER];
	Offset horizGlyphConstruction[ANY_NUMBER];
} MathVariants;

typedef struct {
	TTGlyphID variantGlyph;
	le_uint16 advanceMeasurement;
} MathGlyphVariantRecord;

typedef struct {
	Offset glyphAssembly;
	le_uint16 variantCount;
	MathGlyphVariantRecord mathGlyphVariantRecord[ANY_NUMBER];
} MathGlyphConstruction;

typedef struct {
	TTGlyphID glyph;
	le_uint16 startConnectorLength;
	le_uint16 endConnectorLength;
	le_uint16 fullAdvance;
	le_uint16 partFlags;
} GlyphPartRecord;
#define fExtender	0x0001

typedef struct {
	MathValueRecord italicsCorrection;
	le_uint16 partCount;
	GlyphPartRecord partRecords[ANY_NUMBER];
} GlyphAssembly;

typedef struct {
	le_uint16	mathItalicsCorrectionInfo;
	le_uint16	mathTopAccentAttachment;
	le_uint16	extendedShapeCoverage;
	le_uint16	mathKernInfo;
} MathGlyphInfo;

typedef struct {
	le_uint16	coverage;
	le_uint16	italicsCorrectionCount;
	MathValueRecord	italicsCorrection[ANY_NUMBER];
} MathItalicsCorrectionInfo;

typedef struct {
	le_uint16	coverage;
	le_uint16	topAccentAttachmentCount;
	MathValueRecord	topAccentAttachment[ANY_NUMBER];
} MathTopAccentAttachment;

#endif
