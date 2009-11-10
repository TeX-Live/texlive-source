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
*   Copyright (C) 1998-2003, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
************************************************************************/

#include "layout/LETypes.h"
#include "layout/LESwaps.h"

#include "sfnt.h"
#include "cmaps.h"

#define SWAPU16(code) ((LEUnicode16) SWAPW(code))
#define SWAPU32(code) ((LEUnicode32) SWAPL(code))

// read a 32-bit value that might only be 16-bit-aligned in memory
#define READ_LONG(code) (le_uint32)((SWAPW(*(le_uint16*)&code) << 16) + SWAPW(*(((le_uint16*)&code) + 1)))

//
// Finds the high bit by binary searching
// through the bits in value.
//
le_int8 highBit(le_uint32 value)
{
    le_uint8 bit = 0;

    if (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    if (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    if (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    if (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    if (value >= 1 << 1) {
        value >>= 1;
        bit += 1;
    }

    return bit;
}

CMAPMapper *CMAPMapper::createUnicodeMapper(const CMAPTable *cmap)
{
    le_uint16 i;
    le_uint16 nSubtables = SWAPW(cmap->numberSubtables);
    const CMAPEncodingSubtable *subtable = NULL;
    le_uint32 offset1 = 0, offset10 = 0;

    for (i = 0; i < nSubtables; i += 1) {
        const CMAPEncodingSubtableHeader *esh = &cmap->encodingSubtableHeaders[i];

        switch (SWAPW(esh->platformID)) {
		case 3:	// Microsoft encodings
			switch (SWAPW(esh->platformSpecificID)) {
			case 1:	// Unicode (really UCS-2?)
				offset1 = READ_LONG(esh->encodingOffset);
				break;

			case 10:	// UCS-4
				offset10 = READ_LONG(esh->encodingOffset);
				break;
			}
			break;
	
		case 0: // Apple Unicode
			offset10 = READ_LONG(esh->encodingOffset);	// we ignore the unicode version
			break;		
        }
    }


    if (offset10 != 0) {
        subtable = (const CMAPEncodingSubtable *) ((const char *) cmap + offset10);
    } else if (offset1 != 0) {
        subtable = (const CMAPEncodingSubtable *) ((const char *) cmap + offset1);
    } else {
        return NULL;
    }

    switch (SWAPW(subtable->format)) {
    case 4:
        return new CMAPFormat4Mapper(cmap, (const CMAPFormat4Encoding *) subtable);

    case 12:
    {
        const CMAPFormat12Encoding *encoding = (const CMAPFormat12Encoding *) subtable;

        return new CMAPGroupMapper(cmap, encoding->groups, READ_LONG(encoding->nGroups));
    }

    default:
        break;
    }

    return NULL;
}

CMAPFormat4Mapper::CMAPFormat4Mapper(const CMAPTable *cmap, const CMAPFormat4Encoding *header)
    : CMAPMapper(cmap)
{
    le_uint16 segCount = SWAPW(header->segCountX2) / 2;

    fEntrySelector = SWAPW(header->entrySelector);
    fRangeShift = SWAPW(header->rangeShift) / 2;
    fEndCodes = &header->endCodes[0];
    fStartCodes = &header->endCodes[segCount + 1]; // + 1 for reservedPad...
    fIdDelta = &fStartCodes[segCount];
    fIdRangeOffset = &fIdDelta[segCount];
}

LEGlyphID CMAPFormat4Mapper::unicodeToGlyph(LEUnicode32 unicode32) const
{
    if (unicode32 >= 0x10000) {
        return 0;
    }

    LEUnicode16 unicode = (LEUnicode16) unicode32;
    le_uint16 index = 0;
    le_uint16 probe = 1 << fEntrySelector;
    TTGlyphID result = 0;

    if (SWAPU16(fStartCodes[fRangeShift]) <= unicode) {
        index = fRangeShift;
    }

    while (probe > (1 << 0)) {
        probe >>= 1;

        if (SWAPU16(fStartCodes[index + probe]) <= unicode) {
            index += probe;
        }
    }

    if (unicode >= SWAPU16(fStartCodes[index]) && unicode <= SWAPU16(fEndCodes[index])) {
        if (fIdRangeOffset[index] == 0) {
            result = (TTGlyphID) unicode;
        } else {
            le_uint16 offset = unicode - SWAPU16(fStartCodes[index]);
            le_uint16 rangeOffset = SWAPW(fIdRangeOffset[index]);
            le_uint16 *glyphIndexTable = (le_uint16 *) ((char *) &fIdRangeOffset[index] + rangeOffset);

            result = SWAPW(glyphIndexTable[offset]);
        }

        result += SWAPW(fIdDelta[index]);
    } else {
        result = 0;
    }

    return LE_SET_GLYPH(0, result);
}

CMAPFormat4Mapper::~CMAPFormat4Mapper()
{
    // parent destructor does it all
}

CMAPGroupMapper::CMAPGroupMapper(const CMAPTable *cmap, const CMAPGroup *groups, le_uint32 nGroups)
    : CMAPMapper(cmap), fGroups(groups)
{
    le_uint8 bit = highBit(nGroups);
    fPower = 1 << bit;
    fRangeOffset = nGroups - fPower;
}

LEGlyphID CMAPGroupMapper::unicodeToGlyph(LEUnicode32 unicode32) const
{
    le_int32 probe = fPower;
    le_int32 range = 0;

    if (READ_LONG(fGroups[fRangeOffset].startCharCode) <= unicode32) {
        range = fRangeOffset;
    }

    while (probe > (1 << 0)) {
        probe >>= 1;

        if (READ_LONG(fGroups[range + probe].startCharCode) <= unicode32) {
            range += probe;
        }
    }

    if (READ_LONG(fGroups[range].startCharCode) <= unicode32 && READ_LONG(fGroups[range].endCharCode) >= unicode32) {
        return (LEGlyphID) (READ_LONG(fGroups[range].startGlyphCode) + unicode32 - READ_LONG(fGroups[range].startCharCode));
    }

    return 0;
}

CMAPGroupMapper::~CMAPGroupMapper()
{
    // parent destructor does it all
}

