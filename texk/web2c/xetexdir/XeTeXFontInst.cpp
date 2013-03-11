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
#include "XeTeX_ext.h"

#include <string.h>
#include FT_GLYPH_H
#include FT_ADVANCES_H

FT_Library gFreeTypeLibrary = 0;
// FT_LOAD_NO_BITMAP is needed to work around:
// http://lists.gnu.org/archive/html/freetype/2013-03/msg00009.html
static int ftLoadFlags = FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;

XeTeXFontInst::XeTeXFontInst(const char* pathname, int index, float pointSize, int &status)
    : fPointSize(pointSize)
    , fUnitsPerEM(0)
    , fAscent(0)
    , fDescent(0)
    , fCapHeight(0)
    , fXHeight(0)
    , fItalicAngle(0)
    , fVertical(false)
    , fFilename(NULL)
    , ftFace(0)
    , hbFont(NULL)
    , fMath(NULL)
{
	if (pathname != NULL)
		initialize(pathname, index, status);
}

XeTeXFontInst::~XeTeXFontInst()
{
	if (ftFace != 0) {
		FT_Done_Face(ftFace);
		ftFace = 0;
	}
	hb_font_destroy(hbFont);
}

/* HarfBuzz font functions */

static hb_bool_t
_get_glyph(hb_font_t*, void *font_data, hb_codepoint_t ch, hb_codepoint_t vs, hb_codepoint_t *gid, void*)
{
	FT_Face face = (FT_Face) font_data;
	*gid = 0;

	if (vs)
		*gid = FT_Face_GetCharVariantIndex (face, ch, vs);

	if (*gid == 0)
		*gid = FT_Get_Char_Index (face, ch);

	return *gid != 0;
}

static FT_Fixed
_get_glyph_advance(FT_Face face, FT_UInt gid, bool vertical)
{
	FT_Error error;
	FT_Fixed advance;
	int flags = ftLoadFlags;

	if (vertical)
		flags |= FT_LOAD_VERTICAL_LAYOUT;

	error = FT_Get_Advance(face, gid, ftLoadFlags, &advance);
	if (error)
		advance = 0;
	else
		advance = advance >> 10;

	return advance;
}

static hb_position_t
_get_glyph_h_advance(hb_font_t*, void *font_data, hb_codepoint_t gid, void*)
{
	return _get_glyph_advance((FT_Face) font_data, gid, false);
}

static hb_position_t
_get_glyph_v_advance(hb_font_t*, void *font_data, hb_codepoint_t gid, void*)
{
	return _get_glyph_advance((FT_Face) font_data, gid, true);
}

static hb_bool_t
_get_glyph_h_origin(hb_font_t*, void *font_data, hb_codepoint_t gid, hb_position_t *x, hb_position_t *y, void*)
{
	return true;
}

static hb_bool_t
_get_glyph_v_origin(hb_font_t*, void *font_data, hb_codepoint_t gid, hb_position_t *x, hb_position_t *y, void*)
{
	FT_Face face = (FT_Face) font_data;
	FT_Error error;

	error = FT_Load_Glyph (face, gid, ftLoadFlags);
	if (!error) {
		*x = face->glyph->metrics.horiBearingX -   face->glyph->metrics.vertBearingX;
		*y = face->glyph->metrics.horiBearingY - (-face->glyph->metrics.vertBearingY);
	}

	return !error;
}

static hb_position_t
_get_glyph_h_kerning(hb_font_t*, void *font_data, hb_codepoint_t gid1, hb_codepoint_t gid2, void*)
{
	FT_Face face = (FT_Face) font_data;
	FT_Error error;
	FT_Vector kerning;
	hb_position_t ret;

	error = FT_Get_Kerning (face, gid1, gid2, FT_KERNING_UNFITTED, &kerning);
	if (error)
		ret = 0;
	else
		ret = kerning.x;
	return ret;
}

static hb_position_t
_get_glyph_v_kerning(hb_font_t*, void *font_data, hb_codepoint_t gid1, hb_codepoint_t gid2, void*)
{
	/* FreeType does not support vertical kerning */
	return 0;
}

static hb_bool_t
_get_glyph_extents(hb_font_t*, void *font_data, hb_codepoint_t gid, hb_glyph_extents_t *extents, void*)
{
	FT_Face face = (FT_Face) font_data;
	FT_Error error;

	error = FT_Load_Glyph (face, gid, ftLoadFlags);
	if (!error) {
		extents->x_bearing = face->glyph->metrics.horiBearingX;
		extents->y_bearing = face->glyph->metrics.horiBearingY;
		extents->width  =  face->glyph->metrics.width;
		extents->height = -face->glyph->metrics.height;
	}

	return !error;
}

static hb_bool_t
_get_glyph_contour_point(hb_font_t*, void *font_data, hb_codepoint_t gid, unsigned int point_index, hb_position_t *x, hb_position_t *y, void*)
{
	FT_Face face = (FT_Face) font_data;
	FT_Error error;
	bool ret = false;

	error = FT_Load_Glyph (face, gid, ftLoadFlags);
	if (!error) {
		if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
			if (point_index < (unsigned int) face->glyph->outline.n_points) {
				*x = face->glyph->outline.points[point_index].x;
				*y = face->glyph->outline.points[point_index].y;
				ret = true;
			}
		}
	}

	return ret;
}

static hb_font_funcs_t *
_get_font_funcs(void)
{
	static hb_font_funcs_t* funcs = hb_font_funcs_create();

	hb_font_funcs_set_glyph_func				(funcs, _get_glyph, NULL, NULL);
	hb_font_funcs_set_glyph_h_advance_func		(funcs, _get_glyph_h_advance, NULL, NULL);
	hb_font_funcs_set_glyph_v_advance_func		(funcs, _get_glyph_v_advance, NULL, NULL);
	hb_font_funcs_set_glyph_h_origin_func		(funcs, _get_glyph_h_origin, NULL, NULL);
	hb_font_funcs_set_glyph_v_origin_func		(funcs, _get_glyph_v_origin, NULL, NULL);
	hb_font_funcs_set_glyph_h_kerning_func		(funcs, _get_glyph_h_kerning, NULL, NULL);
	hb_font_funcs_set_glyph_v_kerning_func		(funcs, _get_glyph_v_kerning, NULL, NULL);
	hb_font_funcs_set_glyph_extents_func		(funcs, _get_glyph_extents, NULL, NULL);
	hb_font_funcs_set_glyph_contour_point_func	(funcs, _get_glyph_contour_point, NULL, NULL);

	return funcs;
}

static hb_blob_t *
_get_table(hb_face_t *, hb_tag_t tag, void *user_data)
{
	FT_Face face = (FT_Face) user_data;
	FT_ULong length = 0;
	FT_Byte *table;
	FT_Error error;
	hb_blob_t* blob = NULL;

	error = FT_Load_Sfnt_Table(face, tag, 0, NULL, &length);
	if (!error) {
		table = (FT_Byte *) xmalloc(length * sizeof(char));
		if (table != NULL) {
			error = FT_Load_Sfnt_Table(face, tag, 0, (FT_Byte*)table, &length);
			if (!error) {
				blob = hb_blob_create((const char*) table, length, HB_MEMORY_MODE_WRITABLE, table, free);
			} else {
				free(table);
			}
		}
	}

    return blob;
}

void
XeTeXFontInst::initialize(const char* pathname, int index, int &status)
{
	TT_Postscript *postTable;
	TT_OS2* os2Table;
	FT_Error error;
	hb_face_t *hbFace;

	if (!gFreeTypeLibrary) {
		error = FT_Init_FreeType(&gFreeTypeLibrary);
		if (error) {
			fprintf(stderr, "FreeType initialization failed! (%d)\n", error);
			exit(1);
		}
	}

	error = FT_New_Face(gFreeTypeLibrary, (char*)pathname, index, &ftFace);
	if (error) {
        status = 1;
        return;
    }

	if (!FT_IS_SCALABLE(ftFace)) {
		status = 1;
		return;
	}

	/* for non-sfnt-packaged fonts (presumably Type 1), see if there is an AFM file we can attach */
	if (index == 0 && !FT_IS_SFNT(ftFace)) {
		char* afm = new char[strlen((const char*)pathname) + 5]; // room to append ".afm"
		strcpy(afm, (const char*)pathname);
		char* p = strrchr(afm, '.');
		if (p == NULL || strlen(p) != 4 || tolower(*(p+1)) != 'p' || tolower(*(p+2)) != 'f')
			strcat(afm, ".afm");   // append .afm if the extension didn't seem to be .pf[ab]
		else
			strcpy(p, ".afm"); 	   // else replace extension with .afm
		FT_Attach_File(ftFace, afm); // ignore error code; AFM might not exist
		delete[] afm;
	}

	FT_Set_Char_Size(ftFace, fPointSize * 64, 0, 0, 0);

	char buf[20];
	if (index > 0)
		sprintf(buf, ":%d", index);
	else
		buf[0] = 0;
	fFilename = new char[strlen(pathname) + 2 + strlen(buf) + 1];
	sprintf(fFilename, "[%s%s]", pathname, buf);
	fUnitsPerEM = ftFace->units_per_EM;
	fAscent = unitsToPoints(ftFace->ascender);
	fDescent = unitsToPoints(ftFace->descender);

	postTable = (TT_Postscript *) getFontTable(ft_sfnt_post);
	if (postTable != NULL) {
		fItalicAngle = Fix2D(postTable->italicAngle);
	}

	os2Table = (TT_OS2*) getFontTable(ft_sfnt_os2);
	if (os2Table) {
		fCapHeight = unitsToPoints(os2Table->sCapHeight);
		fXHeight = unitsToPoints(os2Table->sxHeight);
	}

	// Set up HarfBuzz font
	hbFace = hb_face_create_for_tables(_get_table, ftFace, NULL);
	hb_face_set_index(hbFace, index);
	hb_face_set_upem(hbFace, fUnitsPerEM);
	hbFont = hb_font_create(hbFace);
	hb_face_destroy(hbFace);

	hb_font_set_funcs(hbFont, _get_font_funcs(), ftFace, NULL);
	hb_font_set_scale(hbFont,
		((uint64_t) ftFace->size->metrics.x_scale * (uint64_t) fUnitsPerEM) >> 16,
		((uint64_t) ftFace->size->metrics.y_scale * (uint64_t) fUnitsPerEM) >> 16);
	// We don’t want device tables adjustments
	hb_font_set_ppem(hbFont, 0, 0);

    return;
}

void
XeTeXFontInst::setLayoutDirVertical(bool vertical)
{
	fVertical = vertical;
}

const void *
XeTeXFontInst::getFontTable(OTTag tag) const
{
	FT_ULong tmpLength = 0;
	FT_Error error = FT_Load_Sfnt_Table(ftFace, tag, 0, NULL, &tmpLength);
	if (error)
		return NULL;

	void* table = xmalloc(tmpLength * sizeof(char));
	if (table != NULL) {
		error = FT_Load_Sfnt_Table(ftFace, tag, 0, (FT_Byte*)table, &tmpLength);
		if (error) {
			free((void *) table);
			return NULL;
		}
	}

    return table;
}

const char *
XeTeXFontInst::getMathTable()
{
	if (fMath == NULL)
		fMath = (const char*) getFontTable(MATH_TAG);
	return fMath;
}

const void *
XeTeXFontInst::getFontTable(FT_Sfnt_Tag tag) const
{
	return FT_Get_Sfnt_Table(ftFace, tag);
}

void
XeTeXFontInst::getGlyphBounds(GlyphID gid, GlyphBBox* bbox)
{
	bbox->xMin = bbox->yMin = bbox->xMax = bbox->yMax = 0.0;

	FT_Error error = FT_Load_Glyph(ftFace, gid, FT_LOAD_NO_SCALE);
	if (error)
		return;

    FT_Glyph glyph;
    error = FT_Get_Glyph(ftFace->glyph, &glyph);
	if (error == 0) {
		FT_BBox ft_bbox;
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_UNSCALED, &ft_bbox);
		bbox->xMin = unitsToPoints(ft_bbox.xMin);
		bbox->yMin = unitsToPoints(ft_bbox.yMin);
		bbox->xMax = unitsToPoints(ft_bbox.xMax);
		bbox->yMax = unitsToPoints(ft_bbox.yMax);
		FT_Done_Glyph(glyph);
	}
}

GlyphID
XeTeXFontInst::mapCharToGlyph(UChar32 ch) const
{
	return FT_Get_Char_Index(ftFace, ch);
}

uint16_t
XeTeXFontInst::getNumGlyphs() const
{
	return ftFace->num_glyphs;
}

float
XeTeXFontInst::getGlyphWidth(GlyphID gid)
{
	return _get_glyph_advance(ftFace, gid, false) / 64.0;
}

void
XeTeXFontInst::getGlyphHeightDepth(GlyphID gid, float* ht, float* dp)
{
	GlyphBBox bbox;
	getGlyphBounds(gid, &bbox);

	if (ht)
		*ht = bbox.yMax;
	if (dp)
		*dp = -bbox.yMin;
}

void
XeTeXFontInst::getGlyphSidebearings(GlyphID gid, float* lsb, float* rsb)
{
	float width = getGlyphWidth(gid);

	GlyphBBox bbox;
	getGlyphBounds(gid, &bbox);

	if (lsb)
		*lsb = bbox.xMin;
	if (rsb)
		*rsb = width - bbox.xMax;
}

float
XeTeXFontInst::getGlyphItalCorr(GlyphID gid)
{
	float rval = 0.0;

	float width = getGlyphWidth(gid);

	GlyphBBox bbox;
	getGlyphBounds(gid, &bbox);
	
	if (bbox.xMax > width)
		rval = bbox.xMax - width;
	
	return rval;
}

GlyphID
XeTeXFontInst::mapGlyphToIndex(const char* glyphName) const
{
	return FT_Get_Name_Index(ftFace, const_cast<char*>(glyphName));
}

const char*
XeTeXFontInst::getGlyphName(GlyphID gid, int& nameLen)
{
	if (FT_HAS_GLYPH_NAMES(ftFace)) {
		static char buffer[256];
		FT_Get_Glyph_Name(ftFace, gid, buffer, 256);
		nameLen = strlen(buffer);
		return &buffer[0];
	}
	else {
		nameLen = 0;
		return NULL;
	}
}

UChar32
XeTeXFontInst::getFirstCharCode()
{
	FT_UInt  gindex;
	return FT_Get_First_Char(ftFace, &gindex);
}

UChar32
XeTeXFontInst::getLastCharCode()
{
	FT_UInt  gindex;
	UChar32	ch = FT_Get_First_Char(ftFace, &gindex);
	UChar32	prev = ch;
	while (gindex != 0) {
		prev = ch;
		ch = FT_Get_Next_Char(ftFace, ch, &gindex);
	}
	return prev;
}
