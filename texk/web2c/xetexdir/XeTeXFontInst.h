/****************************************************************************\
 Part of the XeTeX typesetting system
 Copyright (c) 1994-2008 by SIL International
 Copyright (c) 2009, 2011 by Jonathan Kew

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
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#include "XeTeXFontMgr.h"
#include "XeTeX_ext.h"

#define MATH_TAG HB_TAG('M','A','T','H')

// create specific subclasses for each supported platform

class XeTeXFontInst
{
protected:
	unsigned short fUnitsPerEM;
	float fPointSize;
	float fAscent;
	float fDescent;
	float fCapHeight;
	float fXHeight;
	float fItalicAngle;

	bool fVertical; // false = horizontal, true = vertical

	char *fFilename; // actually holds [filename:index], as used in xetex

	FT_Face ftFace;
	hb_font_t* hbFont;
	const char *fMath;

public:
	XeTeXFontInst(float pointSize, int &status);
	XeTeXFontInst(const char* filename, int index, float pointSize, int &status);

	virtual ~XeTeXFontInst();

	void initialize(const char* pathname, int index, int &status);

	const void *getFontTable(OTTag tableTag) const;
	const void *getFontTable(FT_Sfnt_Tag tableTag) const;
	const char *getMathTable();

	const char *getFilename() const { return fFilename; }
	hb_font_t *getHbFont() const { return hbFont; }
	void setLayoutDirVertical(bool vertical);
	bool getLayoutDirVertical() const { return fVertical; };

	float getPointSize() const { return fPointSize; };
	float getAscent() const { return fAscent; }
	float getDescent() const { return fDescent; }
	float getCapHeight() const { return fCapHeight; }
	float getXHeight() const { return fXHeight; }
	float getItalicAngle() const { return fItalicAngle; }

	GlyphID mapCharToGlyph(UChar32 ch) const;
	GlyphID mapGlyphToIndex(const char* glyphName) const;

	uint16_t getNumGlyphs() const;

	void getGlyphBounds(GlyphID glyph, GlyphBBox* bbox);

	float getGlyphWidth(GlyphID glyph);
	void getGlyphHeightDepth(GlyphID glyph, float *ht, float* dp);
	void getGlyphSidebearings(GlyphID glyph, float* lsb, float* rsb);
	float getGlyphItalCorr(GlyphID glyph);

	const char* getGlyphName(GlyphID gid, int& nameLen);
	
	UChar32 getFirstCharCode();
	UChar32 getLastCharCode();

	float unitsToPoints(float units) const
	{
		return (units * fPointSize) / (float) fUnitsPerEM;
	}

	float pointsToUnits(float points) const
	{
		return (points * (float) fUnitsPerEM) / fPointSize;
	}
};

#endif
