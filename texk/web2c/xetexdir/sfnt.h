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

/* from ICU sample code, extended by JK for XeTeX */

/***************************************************************************
*
*   Copyright (C) 1998-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
************************************************************************/

#ifndef __SFNT_H
#define __SFNT_H

#include "layout/LETypes.h"


#ifndef ANY_NUMBER
#define ANY_NUMBER 1
#endif

struct DirectoryEntry
{
    le_uint32   tag;
    le_uint32   checksum;
    le_uint32   offset;
    le_uint32   length;
};

struct SFNTDirectory
{
    le_uint32       scalerType;
    le_uint16       numTables;
    le_uint16       searchRange;
    le_uint16       entrySelector;
    le_uint16       rangeShift;
    DirectoryEntry  tableDirectory[ANY_NUMBER];
};


struct CMAPEncodingSubtableHeader
{
    le_uint16   platformID;
    le_uint16   platformSpecificID;
    le_uint32   encodingOffset;
};

struct CMAPTable
{
    le_uint16   version;
    le_uint16   numberSubtables;
    CMAPEncodingSubtableHeader encodingSubtableHeaders[ANY_NUMBER];
};

struct CMAPEncodingSubtable
{
    le_uint16   format;
    le_uint16   length;
    le_uint16   language;
};

struct CMAPFormat0Encoding : CMAPEncodingSubtable
{
    le_uint8    glyphIndexArray[256];
};

struct CMAPFormat2Subheader
{
    le_uint16   firstCode;
    le_uint16   entryCount;
    le_int16    idDelta;
    le_uint16   idRangeOffset;
};

struct CMAPFormat2Encoding : CMAPEncodingSubtable
{
    le_uint16  subHeadKeys[256];
    CMAPFormat2Subheader subheaders[ANY_NUMBER];
};

struct CMAPFormat4Encoding : CMAPEncodingSubtable
{
    le_uint16   segCountX2;
    le_uint16   searchRange;
    le_uint16   entrySelector;
    le_uint16   rangeShift;
    le_uint16   endCodes[ANY_NUMBER];
//  le_uint16   reservedPad;
//  le_uint16   startCodes[ANY_NUMBER];
//  le_uint16   idDelta[ANY_NUMBER];
//  le_uint16   idRangeOffset[ANY_NUMBER];
//  le_uint16   glyphIndexArray[ANY_NUMBER];
};

struct CMAPFormat6Encoding : CMAPEncodingSubtable
{
    le_uint16   firstCode;
    le_uint16   entryCount;
    le_uint16   glyphIndexArray[ANY_NUMBER];
};

struct CMAPEncodingSubtable32
{
    le_uint16   format;
    le_uint16   reserved;
    le_uint32   length;
    le_uint32   language;
};

struct CMAPGroup
{
    le_uint32   startCharCode;
    le_uint32   endCharCode;
    le_uint32   startGlyphCode;
};

struct CMAPFormat8Encoding : CMAPEncodingSubtable32
{
    le_uint32   is32[65536/32];
    le_uint32   nGroups;
    CMAPGroup   groups[ANY_NUMBER];
};

struct CMAPFormat10Encoding : CMAPEncodingSubtable32
{
    le_uint32   startCharCode;
    le_uint32   numCharCodes;
    le_uint16   glyphs[ANY_NUMBER];
};

struct CMAPFormat12Encoding : CMAPEncodingSubtable32
{
    le_uint32   nGroups;
    CMAPGroup   groups[ANY_NUMBER];
};

typedef le_int32 fixed;

struct BigDate
{
    le_uint32   bc;
    le_uint32   ad;
};

struct HEADTable
{
    fixed       version;
    fixed       fontRevision;
    le_uint32   checksumAdjustment;
    le_uint32   magicNumber;
    le_uint16   flags;
    le_uint16   unitsPerEm;
    BigDate     created;
    BigDate     modified;
    le_int16    xMin;
    le_int16    yMin;
    le_int16    xMax;
    le_int16    yMax;
	le_uint16	macStyle;
    le_uint16   lowestRecPPEM;
    le_int16    fontDirectionHint;
    le_int16    indexToLocFormat;
    le_int16    glyphDataFormat;
};

struct MAXPTable
{
    fixed       version;
    le_uint16   numGlyphs;
    le_uint16   maxPoints;
    le_uint16   maxContours;
    le_uint16   maxComponentPoints;
    le_uint16   maxComponentContours;
    le_uint16   maxZones;
    le_uint16   maxTwilightPoints;
    le_uint16   maxStorage;
    le_uint16   maxFunctionDefs;
    le_uint16   maxInstructionDefs;
    le_uint16   maxStackElements;
    le_uint16   maxSizeOfInstructions;
    le_uint16   maxComponentElements;
    le_uint16   maxComponentDepth;
};

struct HHEATable
{
    fixed       version;
    le_int16    ascent;
    le_int16    descent;
    le_int16    lineGap;
    le_uint16   advanceWidthMax;
    le_int16    minLeftSideBearing;
    le_int16    minRightSideBearing;
    le_int16    xMaxExtent;
    le_int16    caretSlopeRise;
    le_int16    caretSlopeRun;
    le_int16    caretOffset;
    le_int16    reserved1;
    le_int16    reserved2;
    le_int16    reserved3;
    le_int16    reserved4;
    le_int16    metricDataFormat;
    le_uint16   numOfLongHorMetrics;
};

struct LongHorMetric
{
    le_uint16   advanceWidth;
    le_int16    leftSideBearing;
};

struct HMTXTable
{
    LongHorMetric hMetrics[ANY_NUMBER];        // ANY_NUMBER = numOfLongHorMetrics from hhea table
//  le_int16        leftSideBearing[ANY_NUMBER]; // ANY_NUMBER = numGlyphs - numOfLongHorMetrics
};

struct POSTTable
{
	fixed		version;
	fixed		italicAngle;
	le_int16	underlinePosition;
	le_uint16	underlineThickness;
	le_uint32	isFixedPitch;
	le_uint32	minMemType42;
	le_uint32	maxMemType42;
	le_uint32	minMemType1;
	le_uint32	maxMemType1;
};

struct OS2TableHeader {
	le_uint16	version;
	le_int16	xAvgCharWidth;
	le_uint16	usWeightClass;
	le_uint16	usWidthClass;
	le_int16	fsType;
	le_int16	ySubscriptXSize;
	le_int16	ySubscriptYSize;
	le_int16	ySubscriptXOffset;
	le_int16	ySubscriptYOffset;
	le_int16	ySuperscriptXSize;
	le_int16	ySuperscriptYSize;
	le_int16	ySuperscriptXOffset;
	le_int16	ySuperscriptYOffset;
	le_int16	yStrikeoutSize;
	le_int16	yStrikeoutPosition;
	le_int16	sFamilyClass;
	le_uint8	panose[10];
	le_uint8	ulCharRange[16];	// spec'd as 4 longs, but do this to keep structure packed
	le_int8	achVendID[4];
	le_uint16	fsSelection;
	le_uint16	fsFirstCharIndex;
	le_uint16	fsLastCharIndex;
};

#endif

