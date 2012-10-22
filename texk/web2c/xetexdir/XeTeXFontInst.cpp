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

/*
 *   file name:  XeTeXFontInst.cpp
 *
 *   created on: 2005-10-22
 *   created by: Jonathan Kew
 *	
 *	originally based on PortableFontInstance.cpp from ICU
 */


#include "layout/LETypes.h"
#include "layout/LEFontInstance.h"
#include "layout/LESwaps.h"

#include "XeTeXFontInst.h"
#include "XeTeXLayoutInterface.h"

#ifdef XETEX_MAC
#include <Carbon/Carbon.h>
#endif

#include "XeTeX_ext.h"

#include "sfnt.h"

#include <string.h>


XeTeXFontInst::XeTeXFontInst(float pointSize, LEErrorCode &status)
    : fPointSize(pointSize)
    , fUnitsPerEM(0)
    , fAscent(0)
    , fDescent(0)
    , fLeading(0)
    , fXHeight(0)
    , fItalicAngle(0)
    , fCMAPMapper(NULL)
    , fMetricsTable(NULL)
    , fNumLongMetrics(0)
    , fNumGlyphs(0)
    , fNumGlyphsInited(false)
    , fVertical(false)
    , fFilename(NULL)
    , fFirstCharCode(-1)
    , fLastCharCode(-1)
{
	// the concrete subclass is responsible to call initialize()
}

XeTeXFontInst::~XeTeXFontInst()
{
	if (fMetricsTable != NULL)
		deleteTable(fMetricsTable);

	if (fCMAPMapper != NULL)
		delete fCMAPMapper;
}

void XeTeXFontInst::initialize(LEErrorCode &status)
{
    const LETag headTag = LE_HEAD_TABLE_TAG;
    const LETag hheaTag = LE_HHEA_TABLE_TAG;
    const LETag vheaTag = LE_VHEA_TABLE_TAG;
    const LETag postTag = LE_POST_TABLE_TAG;
    const HEADTable *headTable = NULL;
    const HHEATable *dirHeadTable = NULL;
    const POSTTable *postTable = NULL;

    // dispose of any cached metrics table
    if (fMetricsTable != NULL) {
	    deleteTable(fMetricsTable);
    	fMetricsTable = NULL;
    }
    
    // read unitsPerEm from 'head' table
    headTable = (const HEADTable *) readFontTable(headTag);

    if (headTable == NULL) {
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    }

    fUnitsPerEM = SWAPW(headTable->unitsPerEm);
    deleteTable(headTable);

	// we use the fact that 'hhea' and 'vhea' have the same format!
    dirHeadTable = (const HHEATable *) readFontTable(fVertical ? vheaTag : hheaTag);

    if (dirHeadTable == NULL) {
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    }

    fAscent  = yUnitsToPoints((float)(le_int16)SWAPW(dirHeadTable->ascent));
    fDescent = yUnitsToPoints((float)(le_int16)SWAPW(dirHeadTable->descent));
    fLeading = yUnitsToPoints((float)(le_int16)SWAPW(dirHeadTable->lineGap));

    fNumLongMetrics = SWAPW(dirHeadTable->numOfLongHorMetrics);

    deleteTable(dirHeadTable);

    fCMAPMapper = findUnicodeMapper();

    if (fCMAPMapper == NULL) {
        status = LE_MISSING_FONT_TABLE_ERROR;
        goto error_exit;
    }

    postTable = (const POSTTable *) readFontTable(postTag);

    if (postTable != NULL) {
		fItalicAngle = Fix2X(SWAPL(postTable->italicAngle));
		deleteTable(postTable);
    }

    return;

error_exit:
    return;
}

void XeTeXFontInst::setLayoutDirVertical(bool vertical)
{
	fVertical = vertical;
	LEErrorCode	status = LE_NO_ERROR;
	initialize(status);
}

void XeTeXFontInst::deleteTable(const void *table) const
{
    LE_DELETE_ARRAY(table);
}

const void *XeTeXFontInst::getFontTable(LETag tableTag) const
{
    return FontTableCache::find(tableTag);
}

const void *XeTeXFontInst::getFontTable(LETag tableTag, le_uint32 *length) const
{
    return FontTableCache::find(tableTag, length);
}

const void *XeTeXFontInst::readFontTable(LETag tableTag) const
{
    le_uint32 len;

    return readTable(tableTag, &len);
}

const void *XeTeXFontInst::readFontTable(LETag tableTag, le_uint32& len) const
{
    return readTable(tableTag, &len);
}

CMAPMapper *XeTeXFontInst::findUnicodeMapper()
{
    LETag cmapTag = LE_CMAP_TABLE_TAG;
    const CMAPTable *cmap = (CMAPTable *) readFontTable(cmapTag);

    if (cmap == NULL) {
        return NULL;
    }

    return CMAPMapper::createUnicodeMapper(cmap);
}


le_uint16 XeTeXFontInst::getNumGlyphs() const
{
    if (!fNumGlyphsInited) {
        LETag maxpTag = LE_MAXP_TABLE_TAG;
        const MAXPTable *maxpTable = (MAXPTable *) readFontTable(maxpTag);

        if (maxpTable != NULL) {
			XeTeXFontInst *realThis = (XeTeXFontInst *) this;
            realThis->fNumGlyphs = SWAPW(maxpTable->numGlyphs);
            deleteTable(maxpTable);
			realThis->fNumGlyphsInited = true;
		}
    }

	return fNumGlyphs;
}

void XeTeXFontInst::getGlyphAdvance(LEGlyphID glyph, LEPoint &advance) const
{
    TTGlyphID ttGlyph = (TTGlyphID) LE_GET_GLYPH(glyph);

    if (fMetricsTable == NULL) {
    	// we use the fact that 'hmtx' and 'vmtx' have the same format
        LETag metricsTag = fVertical ? LE_VMTX_TABLE_TAG : LE_HMTX_TABLE_TAG;
        XeTeXFontInst *realThis = (XeTeXFontInst *) this;
        realThis->fMetricsTable = (const HMTXTable *) readFontTable(metricsTag);
    }

    le_uint16 index = ttGlyph;

    if (ttGlyph >= getNumGlyphs() || fMetricsTable == NULL) {
        advance.fX = advance.fY = 0;
        return;
    }

    if (ttGlyph >= fNumLongMetrics) {
        index = fNumLongMetrics - 1;
    }

    advance.fX = xUnitsToPoints(SWAPW(fMetricsTable->hMetrics[index].advanceWidth));
    advance.fY = 0;
}

le_bool XeTeXFontInst::getGlyphPoint(LEGlyphID glyph, le_int32 pointNumber, LEPoint &point) const
{
    return FALSE;
}

float
XeTeXFontInst::getGlyphWidth(LEGlyphID gid)
{
	LEPoint	advance;
	getGlyphAdvance(gid, advance);
	return advance.fX;
}

void
XeTeXFontInst::getGlyphHeightDepth(LEGlyphID gid, float* ht, float* dp)
{
	GlyphBBox	bbox;
	getGlyphBounds(gid, &bbox);
	
	if (ht)
		*ht = bbox.yMax;
	if (dp)
		*dp = -bbox.yMin;
}

void
XeTeXFontInst::getGlyphSidebearings(LEGlyphID gid, float* lsb, float* rsb)
{
	LEPoint	adv;
	getGlyphAdvance(gid, adv);

	GlyphBBox	bbox;
	getGlyphBounds(gid, &bbox);

	if (lsb)
		*lsb = bbox.xMin;
	if (rsb)
		*rsb = adv.fX - bbox.xMax;
}

float
XeTeXFontInst::getGlyphItalCorr(LEGlyphID gid)
{
	float	rval = 0.0;

	LEPoint	adv;
	getGlyphAdvance(gid, adv);

	GlyphBBox	bbox;
	getGlyphBounds(gid, &bbox);
	
	if (bbox.xMax > adv.fX)
		rval = bbox.xMax - adv.fX;
	
	return rval;
}

LEGlyphID
XeTeXFontInst::mapGlyphToIndex(const char* glyphName) const
	/* default implementation, may be overridden (e.g. by Freetype-based XeTeXFontInst_ */
{
    le_uint32	len;
    const char *p = (const char*)readFontTable(LE_POST_TABLE_TAG, len);
    if (p != NULL)
		return findGlyphInPostTable(p, len, glyphName);
	else
		return 0;
}

const char*
XeTeXFontInst::getGlyphName(LEGlyphID gid, int& nameLen)
{
    le_uint32	len;
    const char *p = (const char*)readFontTable(LE_POST_TABLE_TAG, len);
    if (p != NULL)
		return getGlyphNamePtr(p, len, gid, &nameLen);
	else
		return NULL;
}

LEUnicode32
XeTeXFontInst::getFirstCharCode()
{
	if (fFirstCharCode == -1) {
		int ch = 0;
		while (mapCharToGlyph(ch) == 0 && ch < 0x10ffff)
			++ch;
		fFirstCharCode = ch;
	}
	return fFirstCharCode;
}

LEUnicode32
XeTeXFontInst::getLastCharCode()
{
	if (fLastCharCode == -1) {
		int ch = 0x10ffff;
		while (mapCharToGlyph(ch) == 0 && ch > 0)
			--ch;
		fLastCharCode = ch;
	}
	return fLastCharCode;
}
