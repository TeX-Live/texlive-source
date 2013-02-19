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

/*
 *   file name:  XeTeXFontInst.cpp
 *
 *   created on: 2005-10-22
 *   created by: Jonathan Kew
 *	
 *	originally based on PortableFontInstance.cpp from ICU
 */

#include "XeTeXFontInst.h"
#include "XeTeXLayoutInterface.h"
#include "XeTeXswap.h"

#include "XeTeX_ext.h"

#include "sfnt.h"

#include <string.h>


XeTeXFontInst::XeTeXFontInst(float pointSize, int &status)
    : fPointSize(pointSize)
    , fUnitsPerEM(0)
    , fAscent(0)
    , fDescent(0)
    , fItalicAngle(0)
    , fMetricsTable(NULL)
    , fNumLongMetrics(0)
    , fNumGlyphs(0)
    , fNumGlyphsInited(false)
    , fVertical(false)
    , fFilename(NULL)
    , hbFont(NULL)
{
	// the concrete subclass is responsible to call initialize()
}

XeTeXFontInst::~XeTeXFontInst()
{
	if (fMetricsTable != NULL)
		deleteTable(fMetricsTable);
}

void XeTeXFontInst::initialize(int &status)
{
    const HEADTable *headTable = NULL;
    const HHEATable *dirHeadTable = NULL;
    const POSTTable *postTable = NULL;

    // dispose of any cached metrics table
    if (fMetricsTable != NULL) {
	    deleteTable(fMetricsTable);
    	fMetricsTable = NULL;
    }
    
    // read unitsPerEm from 'head' table
    headTable = (const HEADTable *) readFontTable(kHEAD);

    if (headTable == NULL) {
        status = 1;
        goto error_exit;
    }

    fUnitsPerEM = SWAP(headTable->unitsPerEm);
    deleteTable(headTable);

	// we use the fact that 'hhea' and 'vhea' have the same format!
    dirHeadTable = (const HHEATable *) readFontTable(fVertical ? kVHEA : kHHEA);

    if (dirHeadTable == NULL) {
        status = 1;
        goto error_exit;
    }

    fAscent  = unitsToPoints((float)(int16_t)SWAP(dirHeadTable->ascent));
    fDescent = unitsToPoints((float)(int16_t)SWAP(dirHeadTable->descent));

    fNumLongMetrics = SWAP(dirHeadTable->numOfLongHorMetrics);

    deleteTable(dirHeadTable);

    postTable = (const POSTTable *) readFontTable(kPOST);

    if (postTable != NULL) {
		fItalicAngle = Fix2D(SWAP(uint32_t(postTable->italicAngle)));
		deleteTable(postTable);
    }

    return;

error_exit:
    return;
}

void XeTeXFontInst::setLayoutDirVertical(bool vertical)
{
	fVertical = vertical;
	int	status = 0;
	initialize(status);
}

void XeTeXFontInst::deleteTable(const void *table) const
{
    free((void *) table);
}

const void *XeTeXFontInst::getFontTable(OTTag tableTag) const
{
    return FontTableCache::find(tableTag);
}

const void *XeTeXFontInst::getFontTable(OTTag tableTag, uint32_t *length) const
{
    return FontTableCache::find(tableTag, length);
}

const void *XeTeXFontInst::readFontTable(OTTag tableTag) const
{
    uint32_t len;

    return readTable(tableTag, &len);
}

const void *XeTeXFontInst::readFontTable(OTTag tableTag, uint32_t& len) const
{
    return readTable(tableTag, &len);
}

uint16_t XeTeXFontInst::getNumGlyphs() const
{
    if (!fNumGlyphsInited) {
        const MAXPTable *maxpTable = (MAXPTable *) readFontTable(kMAXP);

        if (maxpTable != NULL) {
			XeTeXFontInst *realThis = (XeTeXFontInst *) this;
            realThis->fNumGlyphs = SWAP(maxpTable->numGlyphs);
            deleteTable(maxpTable);
			realThis->fNumGlyphsInited = true;
		}
    }

	return fNumGlyphs;
}

void XeTeXFontInst::getGlyphAdvance(GlyphID glyph, realpoint &advance) const
{
    if (fMetricsTable == NULL) {
        XeTeXFontInst *realThis = (XeTeXFontInst *) this;
        // we use the fact that 'hmtx' and 'vmtx' have the same format
        realThis->fMetricsTable = (const HMTXTable *) readFontTable(fVertical ? kVMTX : kHMTX);
    }

    uint16_t index = glyph;

    if (glyph >= getNumGlyphs() || fMetricsTable == NULL) {
        advance.x = advance.y = 0;
        return;
    }

    if (glyph >= fNumLongMetrics) {
        index = fNumLongMetrics - 1;
    }

    advance.x = unitsToPoints(SWAP(fMetricsTable->hMetrics[index].advanceWidth));
    advance.y = 0;
}

float
XeTeXFontInst::getGlyphWidth(GlyphID gid)
{
	realpoint	advance;
	getGlyphAdvance(gid, advance);
	return advance.x;
}

void
XeTeXFontInst::getGlyphHeightDepth(GlyphID gid, float* ht, float* dp)
{
	GlyphBBox	bbox;
	getGlyphBounds(gid, &bbox);
	
	if (ht)
		*ht = bbox.yMax;
	if (dp)
		*dp = -bbox.yMin;
}

void
XeTeXFontInst::getGlyphSidebearings(GlyphID gid, float* lsb, float* rsb)
{
	realpoint	adv;
	getGlyphAdvance(gid, adv);

	GlyphBBox	bbox;
	getGlyphBounds(gid, &bbox);

	if (lsb)
		*lsb = bbox.xMin;
	if (rsb)
		*rsb = adv.x - bbox.xMax;
}

float
XeTeXFontInst::getGlyphItalCorr(GlyphID gid)
{
	float	rval = 0.0;

	realpoint	adv;
	getGlyphAdvance(gid, adv);

	GlyphBBox	bbox;
	getGlyphBounds(gid, &bbox);
	
	if (bbox.xMax > adv.x)
		rval = bbox.xMax - adv.x;
	
	return rval;
}

GlyphID
XeTeXFontInst::mapGlyphToIndex(const char* glyphName) const
	/* default implementation, may be overridden (e.g. by Freetype-based XeTeXFontInst_ */
{
    uint32_t	len;
    const char *p = (const char*)readFontTable(kPOST, len);
    if (p != NULL)
		return findGlyphInPostTable(p, len, glyphName);
	else
		return 0;
}

const char*
XeTeXFontInst::getGlyphName(GlyphID gid, int& nameLen)
{
    uint32_t	len;
    const char *p = (const char*)readFontTable(kPOST, len);
    if (p != NULL)
		return getGlyphNamePtr(p, len, gid, &nameLen);
	else
		return NULL;
}
