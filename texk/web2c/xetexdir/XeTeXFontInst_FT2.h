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
 *   file name:  XeTeXFontInst_FT2.h
 *
 *   created on: 2005-10-25
 *   created by: Jonathan Kew
 */


#ifndef __XeTeXFontInst_FT2_H
#define __XeTeXFontInst_FT2_H

#include "XeTeXFontInst.h"

#include <ft2build.h>
#include FT_FREETYPE_H

class XeTeXFontInst_FT2 : public XeTeXFontInst
{
protected:

    const void *	readTable(OTTag tag, uint32_t *length) const;

	FT_Face			face;
	
public:
					XeTeXFontInst_FT2(const char* filename, int index, float pointSize, int &status);

    virtual 		~XeTeXFontInst_FT2();

	virtual void	initialize(int &status);

	virtual void	getGlyphBounds(GlyphID gid, GlyphBBox* bbox);

	// overrides of XeTeXFontInst methods, in case it's not an sfnt
	virtual uint16_t getNumGlyphs() const;
    virtual void getGlyphAdvance(GlyphID glyph, realpoint &advance) const;
    virtual GlyphID mapCharToGlyph(UChar32 ch) const;
    virtual GlyphID mapGlyphToIndex(const char* glyphName) const;
	virtual const char* getGlyphName(GlyphID gid, int& nameLen);
	virtual UChar32 getFirstCharCode();
	virtual UChar32 getLastCharCode();
};

#endif
