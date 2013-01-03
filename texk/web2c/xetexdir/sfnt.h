/****************************************************************************\
 Part of the XeTeX typesetting system
 Copyright (c) 1994-2008 by SIL International
 Copyright (c) 2009 by Jonathan Kew

 SIL Author(s): Jonathan Kew

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

/* from ICU sample code, extended by JK for XeTeX */

/***************************************************************************
*
*   Copyright (C) 1998-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
************************************************************************/

#ifndef __SFNT_H
#define __SFNT_H

#ifndef ANY_NUMBER
#define ANY_NUMBER 1
#endif

typedef int32_t fixed;

typedef struct
{
	uint32_t	bc;
	uint32_t	ad;
} BigDate;

struct HEADTable
{
    fixed		version;
    fixed		fontRevision;
    uint32_t	checksumAdjustment;
    uint32_t	magicNumber;
    uint16_t	flags;
    uint16_t	unitsPerEm;
    BigDate		created;
    BigDate		modified;
    int16_t		xMin;
    int16_t		yMin;
    int16_t		xMax;
    int16_t		yMax;
	uint16_t	macStyle;
    uint16_t	lowestRecPPEM;
    int16_t		fontDirectionHint;
    int16_t		indexToLocFormat;
    int16_t		glyphDataFormat;
};

struct MAXPTable
{
    fixed		version;
    uint16_t	numGlyphs;
    uint16_t	maxPoints;
    uint16_t	maxContours;
    uint16_t	maxComponentPoints;
    uint16_t	maxComponentContours;
    uint16_t	maxZones;
    uint16_t	maxTwilightPoints;
    uint16_t	maxStorage;
    uint16_t	maxFunctionDefs;
    uint16_t	maxInstructionDefs;
    uint16_t	maxStackElements;
    uint16_t	maxSizeOfInstructions;
    uint16_t	maxComponentElements;
    uint16_t	maxComponentDepth;
};

struct HHEATable
{
    fixed		version;
    int16_t		ascent;
    int16_t		descent;
    int16_t		lineGap;
    uint16_t	advanceWidthMax;
    int16_t		minLeftSideBearing;
    int16_t		minRightSideBearing;
    int16_t		xMaxExtent;
    int16_t		caretSlopeRise;
    int16_t		caretSlopeRun;
    int16_t		caretOffset;
    int16_t		reserved1;
    int16_t		reserved2;
    int16_t		reserved3;
    int16_t		reserved4;
    int16_t		metricDataFormat;
    uint16_t	numOfLongHorMetrics;
};

typedef struct
{
	uint16_t	advanceWidth;
	int16_t		leftSideBearing;
} LongHorMetric;

struct HMTXTable
{
    LongHorMetric hMetrics[ANY_NUMBER];        // ANY_NUMBER = numOfLongHorMetrics from hhea table
//  int16_t        leftSideBearing[ANY_NUMBER]; // ANY_NUMBER = numGlyphs - numOfLongHorMetrics
};

struct POSTTable
{
	fixed		version;
	fixed		italicAngle;
	int16_t		underlinePosition;
	uint16_t	underlineThickness;
	uint32_t	isFixedPitch;
	uint32_t	minMemType42;
	uint32_t	maxMemType42;
	uint32_t	minMemType1;
	uint32_t	maxMemType1;
};

struct OS2TableHeader {
	uint16_t	version;
	int16_t		xAvgCharWidth;
	uint16_t	usWeightClass;
	uint16_t	usWidthClass;
	int16_t		fsType;
	int16_t		ySubscriptXSize;
	int16_t		ySubscriptYSize;
	int16_t		ySubscriptXOffset;
	int16_t		ySubscriptYOffset;
	int16_t		ySuperscriptXSize;
	int16_t		ySuperscriptYSize;
	int16_t		ySuperscriptXOffset;
	int16_t		ySuperscriptYOffset;
	int16_t		yStrikeoutSize;
	int16_t		yStrikeoutPosition;
	int16_t		sFamilyClass;
	uint8_t		panose[10];
	uint8_t		ulCharRange[16];	// spec'd as 4 longs, but do this to keep structure packed
	int8_t		achVendID[4];
	uint16_t	fsSelection;
	uint16_t	fsFirstCharIndex;
	uint16_t	fsLastCharIndex;
};

#endif

