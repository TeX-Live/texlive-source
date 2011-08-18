/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2008 by SIL International
 copyright (c) 2009, 2011 by Jonathan Kew

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
 *   file name:  XeTeXFontInst.h
 *
 *   created on: 2005-10-22
 *   created by: Jonathan Kew
 *	
 *	originally based on PortableFontInstance.h from ICU
 */


#ifndef __XeTeXFontInst_H
#define __XeTeXFontInst_H

#include <stdio.h>

#include "layout/LETypes.h"
#include "layout/LEFontInstance.h"

#include "FontTableCache.h"

#include "sfnt.h"
#include "cmaps.h"

#include "XeTeXFontMgr.h"
#include "XeTeX_ext.h"

extern "C" {
	void *xmalloc(size_t);	// from kpathsea
};

// Abstract superclass that XeTeXOTLayoutEngine uses;
// create specific subclasses for each supported platform

class XeTeXFontInst : public LEFontInstance, protected FontTableCache
{
friend class XeTeXGrFont;

protected:
    float    fPointSize;

    le_int32 fUnitsPerEM;
    float fAscent;
    float fDescent;
    float fLeading;

    float fDeviceScaleX;
    float fDeviceScaleY;

	float fXHeight;
	float fItalicAngle;

    CMAPMapper *fCMAPMapper;

    const HMTXTable *fMetricsTable;
    le_uint16 fNumLongMetrics;
    le_uint16 fNumGlyphs;
	bool fNumGlyphsInited;
	
	bool fVertical; // false = horizontal, true = vertical

	char *fFilename; // actually holds [filename:index], as used in xetex

	int fFirstCharCode;
	int fLastCharCode;

    virtual const void *readTable(LETag tag, le_uint32 *length) const = 0;
    void deleteTable(const void *table) const;
    void getMetrics();

    CMAPMapper *findUnicodeMapper();

    const void *readFontTable(LETag tableTag) const;
    const void *readFontTable(LETag tableTag, le_uint32& len) const;

public:
    XeTeXFontInst(float pointSize, LEErrorCode &status);

    virtual ~XeTeXFontInst();

	virtual void initialize(LEErrorCode &status);

    virtual const void *getFontTable(LETag tableTag) const;
    virtual const void *getFontTable(LETag tableTag, le_uint32* length) const;

	virtual const char *getFilename() const
	{
		return fFilename;
	}

	virtual void setLayoutDirVertical(bool vertical);

	virtual bool getLayoutDirVertical() const
	{
		return fVertical;
	};

    virtual le_int32 getUnitsPerEM() const
    {
        return fUnitsPerEM;
    };

    virtual le_int32 getAscent() const
    {
        return (le_int32)fAscent;
    }

    virtual le_int32 getDescent() const
    {
        return (le_int32)fDescent;
    }

    virtual le_int32 getLeading() const
    {
        return (le_int32)fLeading;
    }

    virtual float getExactAscent() const
    {
        return fAscent;
    }

    virtual float getExactDescent() const
    {
        return fDescent;
    }

    virtual float getExactLeading() const
    {
        return fLeading;
    }

    virtual LEGlyphID mapCharToGlyph(LEUnicode32 ch) const
    {
        return fCMAPMapper->unicodeToGlyph(ch);
    }
    
    virtual LEGlyphID mapGlyphToIndex(const char* glyphName) const;

	virtual le_uint16 getNumGlyphs() const;

    virtual void getGlyphAdvance(LEGlyphID glyph, LEPoint &advance) const;

    virtual le_bool getGlyphPoint(LEGlyphID glyph, le_int32 pointNumber, LEPoint &point) const;

	virtual void getGlyphBounds(LEGlyphID glyph, GlyphBBox *bbox) = 0; /* must be implemented by subclass */

	float getGlyphWidth(LEGlyphID glyph);	
	void getGlyphHeightDepth(LEGlyphID glyph, float *ht, float* dp);	
	void getGlyphSidebearings(LEGlyphID glyph, float* lsb, float* rsb);
	float getGlyphItalCorr(LEGlyphID glyph);

	virtual const char* getGlyphName(LEGlyphID gid, int& nameLen);
	
	virtual LEUnicode32 getFirstCharCode();
	virtual LEUnicode32 getLastCharCode();

    float getXPixelsPerEm() const
    {
        return fPointSize;
    };

    float getYPixelsPerEm() const
    {
        return fPointSize;
    };

    float getScaleFactorX() const
    {
        return 1.0;
    }

    float getScaleFactorY() const
    {
        return 1.0;
    }

    float getXHeight() const
    {
        return fXHeight;
    }

    float getItalicAngle() const
    {
        return fItalicAngle;
    }
};

#endif
