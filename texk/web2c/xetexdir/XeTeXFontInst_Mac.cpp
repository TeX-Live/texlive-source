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
 *   file name:  XeTeXFontInst_Mac.cpp
 *
 *   created on: 2005-10-22
 *   created by: Jonathan Kew
 */


#include "XeTeXFontInst_Mac.h"
#include "XeTeX_ext.h"

XeTeXFontInst_Mac::XeTeXFontInst_Mac(ATSFontRef atsFont, float pointSize, LEErrorCode &status)
    : XeTeXFontInst(pointSize, status)
    , fFontRef(atsFont)
    , fStyle(0)
{
    if (LE_FAILURE(status)) {
        return;
    }

	initialize(status);
}

XeTeXFontInst_Mac::~XeTeXFontInst_Mac()
{
	if (fStyle != 0)
		ATSUDisposeStyle(fStyle);
}

void XeTeXFontInst_Mac::initialize(LEErrorCode &status)
{
    if (fFontRef == 0) {
        status = LE_FONT_FILE_NOT_FOUND_ERROR;
        return;
    }

	XeTeXFontInst::initialize(status);

	if (status != LE_NO_ERROR)
		fFontRef = 0;

	if (ATSUCreateStyle(&fStyle) == noErr) {
		ATSUFontID	font = FMGetFontFromATSFontRef(fFontRef);
		Fixed		size = X2Fix(fPointSize * 72.0 / 72.27); /* convert TeX to Quartz points */
		ATSStyleRenderingOptions	options = kATSStyleNoHinting;
		ATSUAttributeTag		tags[3] = { kATSUFontTag, kATSUSizeTag, kATSUStyleRenderingOptionsTag };
		ByteCount				valueSizes[3] = { sizeof(ATSUFontID), sizeof(Fixed), sizeof(ATSStyleRenderingOptions) };
		ATSUAttributeValuePtr	values[3] = { &font, &size, &options };
		ATSUSetAttributes(fStyle, 3, tags, valueSizes, values);
	}
	else {
		status = LE_FONT_FILE_NOT_FOUND_ERROR;
		fFontRef = 0;
	}
	
    return;
}

const void *XeTeXFontInst_Mac::readTable(LETag tag, le_uint32 *length) const
{
	OSStatus status = ATSFontGetTable(fFontRef, tag, 0, 0, 0, (ByteCount*)length);
	if (status != noErr) {
		*length = 0;
		return NULL;
	}
	void*	table = LE_NEW_ARRAY(char, *length);
	if (table != NULL) {
		status = ATSFontGetTable(fFontRef, tag, 0, *length, table, (ByteCount*)length);
		if (status != noErr) {
			*length = 0;
			LE_DELETE_ARRAY(table);
			return NULL;
		}
	}

    return table;
}

void XeTeXFontInst_Mac::getGlyphBounds(LEGlyphID gid, GlyphBBox* bbox)
{
	GetGlyphBBox_AAT(fStyle, gid, bbox);
}

LEGlyphID
XeTeXFontInst_Mac::mapGlyphToIndex(const char* glyphName) const
{
	LEGlyphID rval = XeTeXFontInst::mapGlyphToIndex(glyphName);
	if (rval)
		return rval;
	return GetGlyphIDFromCGFont(fFontRef, glyphName);
}

const char*
XeTeXFontInst_Mac::getGlyphName(LEGlyphID gid, int& nameLen)
{
	const char* rval = XeTeXFontInst::getGlyphName(gid, nameLen);
	if (rval)
		return rval;
	return GetGlyphNameFromCGFont(fFontRef, gid, &nameLen);
}
