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

#include "XeTeX_ext.h"
#include "XeTeXswap.h"

#include "XeTeXGrLayout.h"


/* XeTeXGrFont class */

XeTeXGrFont::XeTeXGrFont(const XeTeXFontInst* inFont, const char* name)
	: Font()
	, fXeTeXFont(inFont)
	, fName(NULL)
{
	fName = strdup(name);
	gr::Font::SetFlushMode(gr::kflushManual);
}

XeTeXGrFont::XeTeXGrFont(const XeTeXGrFont& orig)
	: Font(orig)
	, fXeTeXFont(orig.fXeTeXFont)
	, fName(NULL)
{
	fName = strdup(orig.fName);
}

XeTeXGrFont::~XeTeXGrFont()
{
	delete[] fName;
}

XeTeXGrFont *
XeTeXGrFont::copyThis()
{
	XeTeXGrFont*	rval = new XeTeXGrFont(*this);
	return rval;
}

void
XeTeXGrFont::UniqueCacheInfo(std::wstring & stuFace, bool & fBold, bool & fItalic)
{
	int	len = strlen(fName);
	wchar_t*	wname = new wchar_t[len + 1];
	mbstowcs(wname, fName, len);
	stuFace.assign(wname, wcslen(wname));
	delete[] wname;

	fBold = false;
	fItalic = false;
}

const void *
XeTeXGrFont::getTable(gr::fontTableId32 tableID, size_t * pcbSize)
{
	le_uint32	length;
	LETag		tag = tableID;
	const void *rval = fXeTeXFont->getFontTable(tag, &length);
	*pcbSize = length;
	return rval;
}

void
XeTeXGrFont::getFontMetrics(float * pAscent, float * pDescent, float * pEmSquare)
{
	if (pAscent)
		*pAscent = fXeTeXFont->getExactAscent();
	if (pDescent)
		*pDescent = - fXeTeXFont->getExactDescent();
	if (pEmSquare)
		*pEmSquare = fXeTeXFont->getXPixelsPerEm();
}


/* XeTeXGrTextSource class */

void
XeTeXGrTextSource::setText(const UniChar* iText, size_t iLen)
{
	fTextBuffer = iText;
	fTextLength = iLen;
}

void
XeTeXGrTextSource::setFeatures(int nFeatures, const int* featureIDs, const int* featureValues)
{
	if (fFeatureSettings != NULL)
		delete[] fFeatureSettings;
	
	fNumFeatures = nFeatures;
	if (nFeatures == 0)
		fFeatureSettings = NULL;
	else {
		gr::FeatureSetting* newFeatures = new gr::FeatureSetting[nFeatures];
		for (int i = 0; i < nFeatures; ++i) {
			newFeatures[i].id = featureIDs[i];
			newFeatures[i].value = featureValues[i];
		}
		fFeatureSettings = newFeatures;
	}
}

void
XeTeXGrTextSource::setFeatures(int nFeatures, const gr::FeatureSetting* features)
{
	if (fFeatureSettings != NULL)
		delete[] fFeatureSettings;
	
	fNumFeatures = nFeatures;
	if (nFeatures == 0)
		fFeatureSettings = NULL;
	else {
		gr::FeatureSetting* newFeatures = new gr::FeatureSetting[nFeatures];
		for (int i = 0; i < nFeatures; ++i)
			newFeatures[i] = features[i];
		fFeatureSettings = newFeatures;
	}
}

void
XeTeXGrTextSource::setLanguage(UInt32 iLanguage)
{
	fLanguage.rgch[0] = (iLanguage >> 24) & 0xff;
	fLanguage.rgch[1] = (iLanguage >> 16) & 0xff;
	fLanguage.rgch[2] = (iLanguage >> 8) & 0xff;
	fLanguage.rgch[3] = iLanguage & 0xff;
}

size_t
XeTeXGrTextSource::fetch(gr::toffset startChar, size_t n, gr::utf16* buffer)
{
	for (size_t i = 0; i < n; ++i)
		buffer[i] = fTextBuffer[i];
	return n;
}

size_t
XeTeXGrTextSource::getFontFeatures(gr::toffset charIndex, gr::FeatureSetting properties[64])
{
	for (int i = 0; i < fNumFeatures; ++i)
		properties[i] = fFeatureSettings[i];
	return fNumFeatures;
}

const gr::isocode XeTeXGrTextSource::kUnknownLanguage = { 0, 0, 0, 0 };
