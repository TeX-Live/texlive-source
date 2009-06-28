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
 *   file name:  XeTeXFontInst_FT2.cpp
 *
 *   created on: 2005-10-25
 *   created by: Jonathan Kew
 */


#include "XeTeXFontInst_FT2.h"

#include FT_TRUETYPE_TABLES_H
#include FT_TYPE1_TABLES_H
#include FT_GLYPH_H

#include <string.h>

FT_Library	gFreeTypeLibrary = 0;

XeTeXFontInst_FT2::XeTeXFontInst_FT2(const char* pathname, int index, float pointSize, LEErrorCode &status)
    : XeTeXFontInst(pointSize, status)
    , face(0)
    , fFreeTypeOnly(false)
{
    if (LE_FAILURE(status)) {
        return;
    }

	FT_Error	err;
	if (!gFreeTypeLibrary) {
		err = FT_Init_FreeType(&gFreeTypeLibrary);
		if (err != 0) {
			fprintf(stderr, "FreeType initialization failed! (%d)\n", err);
			exit(1);
		}
	}

	err = FT_New_Face(gFreeTypeLibrary, (char*)pathname, index, &face);

	if (err != 0) {
        status = LE_FONT_FILE_NOT_FOUND_ERROR;
        return;
    }

	/* for non-sfnt-packaged fonts (presumably Type 1), see if there is an AFM file we can attach */
	if (index == 0 && !FT_IS_SFNT(face)) {
		char*	afm = new char[strlen((const char*)pathname) + 5];	// room to append ".afm"
		strcpy(afm, (const char*)pathname);
		char*	p = strrchr(afm, '.');
		if (p == NULL || strlen(p) != 4 || tolower(*(p+1)) != 'p' || tolower(*(p+2)) != 'f')
			strcat(afm, ".afm");	// append .afm if the extension didn't seem to be .pf[ab]
		else
			strcpy(p, ".afm");		// else replace extension with .afm
		FT_Attach_File(face, afm);	// ignore error code; AFM might not exist
		delete[] afm;
		fFreeTypeOnly = true;
	}

	initialize(status);

	if (LE_FAILURE(status))
		return;
	
	char	buf[20];
	if (index > 0)
		sprintf(buf, ":%d", index);
	else
		buf[0] = 0;
	fFilename = new char[strlen(pathname) + 2 + strlen(buf) + 1];
	sprintf(fFilename, "[%s%s]", pathname, buf);
}

XeTeXFontInst_FT2::~XeTeXFontInst_FT2()
{
	if (face != 0) {
		FT_Done_Face(face);
		face = 0;
	}
}

void XeTeXFontInst_FT2::initialize(LEErrorCode &status)
{
    if (face == 0) {
        status = LE_FONT_FILE_NOT_FOUND_ERROR;
        return;
    }

	XeTeXFontInst::initialize(status);

	if (LE_FAILURE(status)) {
		/* font can ONLY be used via FreeType APIs, not direct table access */
		fFreeTypeOnly = true;
		status = LE_NO_ERROR;
		
		/* fill in fields that XeTeXFontInst::initialize failed to get for us */
		fUnitsPerEM = face->units_per_EM;
		fAscent = yUnitsToPoints(face->ascender);
		fDescent = yUnitsToPoints(face->descender);
//		fLeading = ;
		fItalicAngle = 0;
		PS_FontInfoRec	font_info;
		if (FT_Get_PS_Font_Info(face, &font_info) == 0) {
			// will return error if it wasn't a PS font
			fItalicAngle = font_info.italic_angle;
		}
	}
	
    return;
}

const void *XeTeXFontInst_FT2::readTable(LETag tag, le_uint32 *length) const
{
	*length = 0;
	FT_ULong	tmpLength = 0;
	FT_Error err = FT_Load_Sfnt_Table(face, tag, 0, NULL, &tmpLength);
	if (err != 0)
		return NULL;
	
	void*	table = LE_NEW_ARRAY(char, tmpLength);
	if (table != NULL) {
		err = FT_Load_Sfnt_Table(face, tag, 0, (FT_Byte*)table, &tmpLength);
		if (err != 0) {
			LE_DELETE_ARRAY(table);
			return NULL;
		}
		*length = tmpLength;
	}

    return table;
}

void
XeTeXFontInst_FT2::getGlyphBounds(LEGlyphID gid, GlyphBBox* bbox)
{
	bbox->xMin = bbox->yMin = bbox->xMax = bbox->yMax = 0.0;

	FT_Error	err = FT_Load_Glyph(face, gid, FT_LOAD_NO_SCALE);
	if (err != 0)
		return;
    
    FT_Glyph	glyph;
    err = FT_Get_Glyph(face->glyph, &glyph);
	if (err == 0) {	    
		FT_BBox	ft_bbox;
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_UNSCALED, &ft_bbox);
		bbox->xMin = ft_bbox.xMin * fPointSize / fUnitsPerEM;
		bbox->yMin = ft_bbox.yMin * fPointSize / fUnitsPerEM;
		bbox->xMax = ft_bbox.xMax * fPointSize / fUnitsPerEM;
		bbox->yMax = ft_bbox.yMax * fPointSize / fUnitsPerEM;
		FT_Done_Glyph(glyph);
	}
}

LEGlyphID
XeTeXFontInst_FT2::mapCharToGlyph(LEUnicode32 ch) const
{
	if (!fFreeTypeOnly)
		return XeTeXFontInst::mapCharToGlyph(ch);

	return FT_Get_Char_Index(face, ch);
}

le_uint16
XeTeXFontInst_FT2::getNumGlyphs() const
{
	return face->num_glyphs;
}

void
XeTeXFontInst_FT2::getGlyphAdvance(LEGlyphID glyph, LEPoint &advance) const
{
	if (!fFreeTypeOnly)
		XeTeXFontInst::getGlyphAdvance(glyph, advance);
	else {
		FT_Error	err = FT_Load_Glyph(face, glyph, FT_LOAD_NO_SCALE);
		if (err != 0) {
			advance.fX = advance.fY = 0;
		}
		else {
			advance.fX = fVertical ? 0 : face->glyph->metrics.horiAdvance * fPointSize / fUnitsPerEM;
			advance.fY = fVertical ? face->glyph->metrics.vertAdvance * fPointSize / fUnitsPerEM : 0;
		}
	}
}

LEGlyphID
XeTeXFontInst_FT2::mapGlyphToIndex(const char* glyphName) const
{
	LEGlyphID	rval = FT_Get_Name_Index(face, const_cast<char*>(glyphName));
	if (rval == 0)
		rval = XeTeXFontInst::mapGlyphToIndex(glyphName);
	return rval;
}

void
XeTeXFontInst_FT2::getKernPair(LEGlyphID leftGlyph, LEGlyphID rightGlyph, LEPoint &kern) const
{
	FT_Vector	kerning;
	if (FT_Get_Kerning(face, leftGlyph, rightGlyph, FT_KERNING_UNSCALED, &kerning) == 0) {
		kern.fX = kerning.x;
		kern.fY = kerning.y;
	}
	else
		kern.fX = kern.fY = 0;
}

const char*
XeTeXFontInst_FT2::getGlyphName(LEGlyphID gid, int& nameLen)
{
	if (!fFreeTypeOnly)
		return XeTeXFontInst::getGlyphName(gid, nameLen);
	else if (FT_HAS_GLYPH_NAMES(face)) {
		static char	buffer[256];
		FT_Get_Glyph_Name(face, gid, buffer, 256);
		nameLen = strlen(buffer);
		return &buffer[0];
	}
	else {
		nameLen = 0;
		return NULL;
	}
}

LEUnicode32
XeTeXFontInst_FT2::getFirstCharCode()
{
	if (!fFreeTypeOnly)
		return XeTeXFontInst::getFirstCharCode();
	else {
		FT_UInt  gindex;
		return FT_Get_First_Char(face, &gindex);
	}
}

LEUnicode32
XeTeXFontInst_FT2::getLastCharCode()
{
	if (!fFreeTypeOnly)
		return XeTeXFontInst::getLastCharCode();
	else {
		FT_UInt  gindex;
		LEUnicode32	ch = FT_Get_First_Char(face, &gindex);
		LEUnicode32	prev = ch;
		while (gindex != 0) {
			prev = ch;
			ch = FT_Get_Next_Char(face, ch, &gindex);
		}
		return prev;
	}
}
