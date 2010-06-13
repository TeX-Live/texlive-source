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

/* XeTeX_mac.c
 * additional plain C extensions for XeTeX - MacOS-specific routines
 */

#ifdef __POWERPC__
#define MAC_OS_X_VERSION_MIN_REQUIRED	1030
#else
#define MAC_OS_X_VERSION_MIN_REQUIRED	1040
#endif

#define EXTERN extern
#include "xetexd.h"

#undef input /* this is defined in texmfmp.h, but we don't need it and it confuses the carbon headers */
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

#include <teckit/TECkit_Engine.h>
#include "XeTeX_ext.h"
#include "XeTeXLayoutInterface.h"

#include "XeTeXswap.h"

static ATSUTextLayout	sTextLayout = 0;

static void
InitializeLayout()
{
	OSStatus	status = ATSUCreateTextLayout(&sTextLayout);
	ATSUFontFallbacks fallbacks;
	status = ATSUCreateFontFallbacks(&fallbacks);
	status = ATSUSetObjFontFallbacks(fallbacks, 0, 0, kATSULastResortOnlyFallback);
	ATSUAttributeTag		tag = kATSULineFontFallbacksTag;
	ByteCount				valueSize = sizeof(fallbacks);
	ATSUAttributeValuePtr	value = &fallbacks;
	status = ATSUSetLayoutControls(sTextLayout, 1, &tag, &valueSize, &value);
	status = ATSUSetTransientFontMatching(sTextLayout, true);
}

static inline double
TeXtoPSPoints(double pts)
{
	return pts * 72.0 / 72.27;
}

static inline double
PStoTeXPoints(double pts)
{
	return pts * 72.27 / 72.0;
}

static inline Fixed
FixedTeXtoPSPoints(Fixed pts)
{
	return X2Fix(TeXtoPSPoints(Fix2X(pts)));
}

static inline Fixed
FixedPStoTeXPoints(Fixed pts)
{
	return X2Fix(PStoTeXPoints(Fix2X(pts)));
}

void
DoAtsuiLayout(void* p, int justify)
{
	memoryword*	node = (memoryword*)p;

	unsigned	f = native_font(node);
	if (fontarea[f] != AAT_FONT_FLAG) {
		fprintf(stderr, "internal error: do_atsui_layout called for non-ATSUI font\n");
		exit(1);
	}

	if (sTextLayout == 0)
		InitializeLayout();

	OSStatus	status = noErr;
	
	long		txtLen = native_length(node);
	const UniChar*	txtPtr = (UniChar*)(node + native_node_size);

	status = ATSUSetTextPointerLocation(sTextLayout, txtPtr, 0, txtLen, txtLen);
	
	// we're using this font in AAT mode, so fontlayoutengine[f] is actually an ATSUStyle
	ATSUStyle	style = (ATSUStyle)(fontlayoutengine[native_font(node)]);
	status = ATSUSetRunStyle(sTextLayout, style, 0, txtLen);

	ATSUAttributeTag		tags[] = { kATSULineWidthTag, kATSULineJustificationFactorTag };
	ItemCount				numTags = sizeof(tags) / sizeof(ATSUAttributeTag);
	if (justify) {
		ByteCount				valSizes[] = { sizeof(Fixed), sizeof(Fract) };
		Fixed					wid = FixedTeXtoPSPoints(node_width(node));
		Fract					just = fract1;
		ATSUAttributeValuePtr	valPtrs[] = { &wid, &just };
		status = ATSUSetLayoutControls(sTextLayout, numTags, tags, valSizes, valPtrs);
	}
	
	ItemCount	count;
	ATSLayoutRecord*	layoutRec = NULL;
	status = ATSUDirectGetLayoutDataArrayPtrFromTextLayout(sTextLayout, 0,
		kATSUDirectDataLayoutRecordATSLayoutRecordCurrent, (void*)&layoutRec, &count);

	int i;
	int	realGlyphCount = 0;
	int lastRealGlyph = 0;
	for (i = 0; i < count; ++i)
		if (layoutRec[i].glyphID < 0xfffe) {
			lastRealGlyph = i;
			++realGlyphCount;
		}

	void*		glyph_info = xmalloc(realGlyphCount * native_glyph_info_size);
	FixedPoint*	locations = (FixedPoint*)glyph_info;
	UInt16*		glyphIDs = (UInt16*)(locations + realGlyphCount);

	Fixed		lsUnit = justify ? 0 : fontletterspace[f];
	Fixed		lsDelta = 0;

	realGlyphCount = 0;
	for (i = 0; i < count; ++i) {
		if (layoutRec[i].glyphID < 0xfffe) {
			if ((layoutRec[i].flags & kATSGlyphInfoIsAttachment) && (lsDelta != 0))
				lsDelta -= lsUnit;
			glyphIDs[realGlyphCount] = layoutRec[i].glyphID;
			locations[realGlyphCount].y = 0;	/* FIXME: won't handle baseline offsets */
			locations[realGlyphCount].x = FixedPStoTeXPoints(layoutRec[i].realPos) + lsDelta;
			lsDelta += lsUnit;
			++realGlyphCount;
		}
	}
	if (lsDelta != 0)
		lsDelta -= lsUnit;

	native_glyph_count(node) = realGlyphCount;
	native_glyph_info_ptr(node) = glyph_info;
	
	if (!justify)
		node_width(node) = FixedPStoTeXPoints(layoutRec[count-1].realPos) + lsDelta;

	ATSUDirectReleaseLayoutDataArrayPtr(NULL, kATSUDirectDataLayoutRecordATSLayoutRecordCurrent, (void*)&layoutRec);

	if (justify)
		ATSUClearLayoutControls(sTextLayout, numTags, tags);
}

static OSStatus QuadraticClosePath(void *callBackDataPtr)
{
	return 0;
}

static OSStatus QuadraticCurve(const Float32Point *pt1, const Float32Point *controlPt, const Float32Point *pt2, void *callBackDataPtr)
{
	GlyphBBox*	data = (GlyphBBox*)callBackDataPtr;
	
	if (pt1->x < data->xMin)
		data->xMin = pt1->x;
	if (pt1->x > data->xMax)
		data->xMax = pt1->x;
	if (pt1->y < data->yMin)
		data->yMin = pt1->y;
	if (pt1->y > data->yMax)
		data->yMax = pt1->y;

	if (pt2->x < data->xMin)
		data->xMin = pt2->x;
	if (pt2->x > data->xMax)
		data->xMax = pt2->x;
	if (pt2->y < data->yMin)
		data->yMin = pt2->y;
	if (pt2->y > data->yMax)
		data->yMax = pt2->y;

	if (controlPt->x < data->xMin)
		data->xMin = controlPt->x;
	if (controlPt->x > data->xMax)
		data->xMax = controlPt->x;
	if (controlPt->y < data->yMin)
		data->yMin = controlPt->y;
	if (controlPt->y > data->yMax)
		data->yMax = controlPt->y;

	return 0;
}

static OSStatus QuadraticLine(const Float32Point *pt1, const Float32Point *pt2, void *callBackDataPtr)
{
	GlyphBBox*	data = (GlyphBBox*)callBackDataPtr;
	
	if (pt1->x < data->xMin)
		data->xMin = pt1->x;
	if (pt1->x > data->xMax)
		data->xMax = pt1->x;
	if (pt1->y < data->yMin)
		data->yMin = pt1->y;
	if (pt1->y > data->yMax)
		data->yMax = pt1->y;

	if (pt2->x < data->xMin)
		data->xMin = pt2->x;
	if (pt2->x > data->xMax)
		data->xMax = pt2->x;
	if (pt2->y < data->yMin)
		data->yMin = pt2->y;
	if (pt2->y > data->yMax)
		data->yMax = pt2->y;

	return 0;
}

static OSStatus QuadraticNewPath(void *callBackDataPtr)
{
	return 0;
}

static OSStatus CubicMoveTo(const Float32Point *pt, void *callBackDataPtr)
{
	GlyphBBox*	data = (GlyphBBox*)callBackDataPtr;
	
	if (pt->x < data->xMin)
		data->xMin = pt->x;
	if (pt->x > data->xMax)
		data->xMax = pt->x;
	if (pt->y < data->yMin)
		data->yMin = pt->y;
	if (pt->y > data->yMax)
		data->yMax = pt->y;

	return 0;
}

static OSStatus CubicLineTo(const Float32Point *pt, void *callBackDataPtr)
{
	GlyphBBox*	data = (GlyphBBox*)callBackDataPtr;
	
	if (pt->x < data->xMin)
		data->xMin = pt->x;
	if (pt->x > data->xMax)
		data->xMax = pt->x;
	if (pt->y < data->yMin)
		data->yMin = pt->y;
	if (pt->y > data->yMax)
		data->yMax = pt->y;

	return 0;
}

static OSStatus CubicCurveTo(const Float32Point *pt1, const Float32Point *pt2, const Float32Point *pt3, void *callBackDataPtr)
{
	GlyphBBox*	data = (GlyphBBox*)callBackDataPtr;

	if (pt1->x < data->xMin)
		data->xMin = pt1->x;
	if (pt1->x > data->xMax)
		data->xMax = pt1->x;
	if (pt1->y < data->yMin)
		data->yMin = pt1->y;
	if (pt1->y > data->yMax)
		data->yMax = pt1->y;

	if (pt2->x < data->xMin)
		data->xMin = pt2->x;
	if (pt2->x > data->xMax)
		data->xMax = pt2->x;
	if (pt2->y < data->yMin)
		data->yMin = pt2->y;
	if (pt2->y > data->yMax)
		data->yMax = pt2->y;

	if (pt3->x < data->xMin)
		data->xMin = pt3->x;
	if (pt3->x > data->xMax)
		data->xMax = pt3->x;
	if (pt3->y < data->yMin)
		data->yMin = pt3->y;
	if (pt3->y > data->yMax)
		data->yMax = pt3->y;

	return 0;
}

static OSStatus CubicClosePath(void *callBackDataPtr)
{
	return 0;
}

void GetGlyphBBox_AAT(ATSUStyle style, UInt16 gid, GlyphBBox* bbox)
	/* returns glyph bounding box in TeX points */
{
	ATSCurveType	curveType;
	OSStatus		status;

	bbox->xMin = 65536.0;
	bbox->yMin = 65536.0;
	bbox->xMax = -65536.0;
	bbox->yMax = -65536.0;

	status = ATSUGetNativeCurveType(style, &curveType);
	if (status == noErr) {
		OSStatus	cbStatus;

		if (curveType == kATSCubicCurveType) {
			static ATSCubicMoveToUPP cubicMoveToProc;
			static ATSCubicLineToUPP cubicLineToProc;
			static ATSCubicCurveToUPP cubicCurveToProc;
			static ATSCubicClosePathUPP cubicClosePathProc;
			if (cubicMoveToProc == NULL) {
				cubicMoveToProc = NewATSCubicMoveToUPP(&CubicMoveTo);
				cubicLineToProc = NewATSCubicLineToUPP(&CubicLineTo);
				cubicCurveToProc = NewATSCubicCurveToUPP(&CubicCurveTo);
				cubicClosePathProc = NewATSCubicClosePathUPP(&CubicClosePath);
			}
			status = ATSUGlyphGetCubicPaths(style, gid,
						cubicMoveToProc, cubicLineToProc, cubicCurveToProc, cubicClosePathProc, 
						bbox, &cbStatus);
		}
		else {
			static ATSQuadraticNewPathUPP quadraticNewPathProc;
			static ATSQuadraticLineUPP quadraticLineProc;
			static ATSQuadraticCurveUPP quadraticCurveProc;
			static ATSQuadraticClosePathUPP quadraticClosePathProc;
			if (quadraticNewPathProc == NULL) {
				quadraticNewPathProc = NewATSQuadraticNewPathUPP(&QuadraticNewPath);
				quadraticLineProc = NewATSQuadraticLineUPP(&QuadraticLine);
				quadraticCurveProc = NewATSQuadraticCurveUPP(&QuadraticCurve);
				quadraticClosePathProc = NewATSQuadraticClosePathUPP(&QuadraticClosePath);
			}
			status = ATSUGlyphGetQuadraticPaths(style, gid,
						quadraticNewPathProc, quadraticLineProc, quadraticCurveProc, quadraticClosePathProc,
						bbox, &cbStatus);
		}
	}

	if (status != noErr || bbox->xMin == 65536.0)
		bbox->xMin = bbox->yMin = bbox->xMax = bbox->yMax = 0;
	else {
		// convert PS to TeX points and flip y-axis
		float	tmp = bbox->yMin;
		bbox->yMin = -bbox->yMax * 72.27 / 72.0;
		bbox->yMax = -tmp * 72.27 / 72.0;
		bbox->xMin *= 72.27 / 72.0;
		bbox->xMax *= 72.27 / 72.0;
	}
}

float GetGlyphWidth_AAT(ATSUStyle style, UInt16 gid)
	/* returns TeX points */
{
	ATSGlyphIdealMetrics	metrics;
	OSStatus	status = ATSUGlyphGetIdealMetrics(style, 1, &gid, 0, &metrics);
	if (status == noErr)
		return PStoTeXPoints(metrics.advance.x);
	else
		return 0;
}

void GetGlyphHeightDepth_AAT(ATSUStyle style, UInt16 gid, float* ht, float* dp)
	/* returns TeX points */
{
	GlyphBBox	bbox;
	
	GetGlyphBBox_AAT(style, gid, &bbox);

	*ht = bbox.yMax;
	*dp = -bbox.yMin;
}

void GetGlyphSidebearings_AAT(ATSUStyle style, UInt16 gid, float* lsb, float* rsb)
	/* returns TeX points */
{
	ATSGlyphIdealMetrics	metrics;
	OSStatus	status = ATSUGlyphGetIdealMetrics(style, 1, &gid, 0, &metrics);
	if (status == noErr) {
		GlyphBBox	bbox;
		GetGlyphBBox_AAT(style, gid, &bbox);
		*lsb = bbox.xMin;
		*rsb = PStoTeXPoints(metrics.advance.x) - bbox.xMax;
	}
	else {
		*lsb = *rsb = 0;
	}
}

float GetGlyphItalCorr_AAT(ATSUStyle style, UInt16 gid)
{
	float	rval = 0.0;
	ATSGlyphIdealMetrics	metrics;
	OSStatus	status = ATSUGlyphGetIdealMetrics(style, 1, &gid, 0, &metrics);
	if (status == noErr) {
		GlyphBBox	bbox;
		GetGlyphBBox_AAT(style, gid, &bbox);
		if (bbox.xMax > PStoTeXPoints(metrics.advance.x))
			rval = bbox.xMax - PStoTeXPoints(metrics.advance.x);
	}
	return rval;
}

int MapCharToGlyph_AAT(ATSUStyle style, UInt32 ch)
{
	UniChar	txt[2];
	int		len = 1;
	
	if (sTextLayout == 0)
		InitializeLayout();

	if (ch > 0xffff) {
		ch -= 0x10000;
		txt[0] = 0xd800 + ch / 1024;
		txt[1] = 0xdc00 + ch % 1024;
		len = 2;
	}
	else
		txt[0] = ch;

	OSStatus	status = ATSUSetTextPointerLocation(sTextLayout, &txt[0], 0, len, len);
	if (status != noErr)
		return 0;
	status = ATSUSetRunStyle(sTextLayout, style, 0, len);
	if (status != noErr)
		return 0;
	
	ByteCount	bufferSize = sizeof(ATSUGlyphInfoArray);
	ATSUGlyphInfoArray	info;
	status = ATSUGetGlyphInfo(sTextLayout, 0, len, &bufferSize, &info);
	if (status == noErr) {
		if (bufferSize > 0 && info.numGlyphs > 0)
			return info.glyphs[0].glyphID;
	}
	return 0;
}

int MapGlyphToIndex_AAT(ATSUStyle style, const char* glyphName)
{
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);

	ATSFontRef	fontRef = FMGetATSFontRefFromFont(fontID);

	ByteCount	length;
	OSStatus status = ATSFontGetTable(fontRef, kPost, 0, 0, 0, &length);
	if (status != noErr)
		goto ats_failed;

	void*	table = xmalloc(length);
	status = ATSFontGetTable(fontRef, kPost, 0, length, table, &length);
	if (status != noErr) {
		free(table);
		goto ats_failed;
	}
	
	int	rval = findGlyphInPostTable(table, length, glyphName);
	free(table);

    if (rval)
		return rval;

ats_failed:
	return GetGlyphIDFromCGFont(fontRef, glyphName);
}

int GetGlyphIDFromCGFont(ATSFontRef atsFontRef, const char* glyphName)
{
#if defined(MAC_OS_X_VERSION_10_5) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5)
	if (&CGFontGetGlyphWithGlyphName == NULL)
		return 0;

	int rval = 0;
	CFStringRef psname;
	OSStatus status = ATSFontGetPostScriptName(atsFontRef, 0, &psname);
	if ((status == noErr) && psname) {
		CGFontRef cgfont = CGFontCreateWithFontName(psname);
		CFRelease(psname);
		if (cgfont) {
			CFStringRef glyphname = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault,
																	glyphName,
																	kCFStringEncodingUTF8,
																	kCFAllocatorNull);
			rval = CGFontGetGlyphWithGlyphName(cgfont, glyphname);
			CFRelease(glyphname);
			CGFontRelease(cgfont);
		}
	}
	return rval;
#else
    return 0;
#endif
}

char*
GetGlyphName_AAT(ATSUStyle style, UInt16 gid, int* len)
{
	static char buffer[256];
	const char* namePtr;
	
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);

	ATSFontRef	fontRef = FMGetATSFontRefFromFont(fontID);

	ByteCount	length;
	OSStatus status = ATSFontGetTable(fontRef, kPost, 0, 0, 0, &length);
	if (status != noErr)
		goto ats_failed;

	void*	table = xmalloc(length);
	status = ATSFontGetTable(fontRef, kPost, 0, length, table, &length);
	if (status != noErr) {
		free(table);
		goto ats_failed;
	}
	
	buffer[0] = 0;
	namePtr = getGlyphNamePtr(table, length, gid, len);
	if (*len > 255)
		*len = 255;
	if (namePtr) {
		memcpy(buffer, namePtr, *len);
		buffer[*len] = 0;
	}
	
	free(table);

	if (buffer[0])
		return &buffer[0];

ats_failed:
	return GetGlyphNameFromCGFont(fontRef, gid, len);
}

char*
GetGlyphNameFromCGFont(ATSFontRef atsFontRef, UInt16 gid, int* len)
{
	*len = 0;
	static char buffer[256];
	buffer[0] = 0;

#if defined(MAC_OS_X_VERSION_10_5) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5)
	if (&CGFontCopyGlyphNameForGlyph == NULL)
		return &buffer[0];

	CFStringRef psname;
	OSStatus status = ATSFontGetPostScriptName(atsFontRef, 0, &psname);
	if ((status == noErr) && psname) {
		CGFontRef cgfont = CGFontCreateWithFontName(psname);
		CFRelease(psname);
		if (cgfont && gid < CGFontGetNumberOfGlyphs(cgfont)) {
			CFStringRef glyphname = CGFontCopyGlyphNameForGlyph(cgfont, gid);
			if (glyphname) {
				if (CFStringGetCString(glyphname, buffer, 256, kCFStringEncodingUTF8)) {
					*len = strlen(buffer);
				}
				CFRelease(glyphname);
			}
			CGFontRelease(cgfont);
		}
	}
#endif

	return &buffer[0];
}

/*
int
GetFontCharRange_AAT(ATSUStyle style, int reqFirst)
{
	if (reqFirst) {
		int	ch = 0;
		while (MapCharToGlyph_AAT(style, ch) == 0 && ch < 0x10ffff)
			++ch;
		return ch;
	}
	else {
		int ch = 0x10ffff;
		while (MapCharToGlyph_AAT(style, ch) == 0 && ch > 0)
			--ch;
		return ch;
	}
}
*/

ATSUFontVariationAxis
find_axis_by_name(ATSUFontID fontID, const char* name, int nameLength)
{
	ATSUFontVariationAxis	result = 0;

	Str255	inName;
	inName[0] = nameLength;
	int i;
	for (i = 0; i < inName[0]; ++i)
		inName[i + 1] = name[i];

	ItemCount	varCount;
	ATSUCountFontVariations(fontID, &varCount);
	if (varCount > 0) {
		for (i = 0; i < varCount; ++i) {
			ATSUFontVariationAxis	axis;
			ATSUGetIndFontVariation(fontID, i, &axis, 0, 0, 0);
			FontNameCode	nameCode;
			ATSUGetFontVariationNameCode(fontID, axis, &nameCode);
			Str255		name;
			ByteCount	nameLen;
			ATSUFindFontName(fontID, nameCode, kFontMacintoshPlatform, kFontNoScript, kFontNoLanguage,
                                 255, (Ptr)&name[1], &nameLen, 0);
			name[0] = nameLen;
			if (EqualString(inName, name, false, true)) {
				result = axis;
				break;
			}
		}
	}
	
	return result;
}

ATSUFontFeatureType
find_feature_by_name(ATSUFontID fontID, const char* name, int nameLength)
{
	ATSUFontFeatureType	result = 0x0000FFFF;

	Str255	inName;
	inName[0] = nameLength;
	int i;
	for (i = 0; i < inName[0]; ++i)
		inName[i + 1] = name[i];

	ItemCount	typeCount;
	ATSUCountFontFeatureTypes(fontID, &typeCount);
	if (typeCount > 0) {
		ATSUFontFeatureType*	types = (ATSUFontFeatureType*)xmalloc(typeCount * sizeof(ATSUFontFeatureType));
		ATSUGetFontFeatureTypes(fontID, typeCount, types, 0);
		for (i = 0; i < typeCount; ++i) {
			FontNameCode	nameCode;
			ATSUGetFontFeatureNameCode(fontID, types[i], 0x0000FFFF, &nameCode);
			Str255		name;
			ByteCount	nameLen;
			ATSUFindFontName(fontID, nameCode, kFontMacintoshPlatform, kFontNoScript, kFontNoLanguage,
                                 255, (Ptr)&name[1], &nameLen, 0);
			name[0] = nameLen;
			if (EqualString(inName, name, false, true)) {
				result = types[i];
				break;
			}
		}
		free((char*)types);
	}
	
	return result;
}

ATSUFontFeatureSelector
find_selector_by_name(ATSUFontID fontID, ATSUFontFeatureType featureType, const char* name, int nameLength)
{
	ATSUFontFeatureSelector	result = 0x0000FFFF;

	Str255	inName;
	inName[0] = nameLength;
	int i;
	for (i = 0; i < inName[0]; ++i)
		inName[i + 1] = name[i];

	ItemCount	selectorCount;
	ATSUCountFontFeatureSelectors(fontID, featureType, &selectorCount);
	if (selectorCount > 0) {
		ATSUFontFeatureSelector*	selectors = (ATSUFontFeatureSelector*)xmalloc(selectorCount * sizeof(ATSUFontFeatureSelector));
		ATSUGetFontFeatureSelectors(fontID, featureType, selectorCount, selectors, 0, 0, 0);
		for (i = 0; i < selectorCount; ++i) {
			FontNameCode	nameCode;
			ATSUGetFontFeatureNameCode(fontID, featureType, selectors[i], &nameCode);
			Str255		name;
			ByteCount	nameLen;
			ATSUFindFontName(fontID, nameCode, kFontMacintoshPlatform, kFontNoScript, kFontNoLanguage,
                                 255, (Ptr)&name[1], &nameLen, 0);
			name[0] = nameLen;
			if (EqualString(inName, name, false, true)) {
				result = selectors[i];
				break;
			}
		}
		free((char*)selectors);
	}
	
	return result;
}

void*
loadAATfont(ATSFontRef fontRef, long scaled_size, const char* cp1)
{
	ATSUFontID	fontID = FMGetFontFromATSFontRef(fontRef);
	ATSUStyle	style = 0;
	OSStatus	status = ATSUCreateStyle(&style);
	float		extend = 1.0;
	float		slant = 0.0;
	float		embolden = 0.0;
	float		letterspace = 0.0;
	int i;
	
	if (status == noErr) {
		UInt32	rgbValue;
		Fixed	tracking = 0x80000000;
		Fixed	atsuSize = FixedTeXtoPSPoints(scaled_size);
		
		ATSStyleRenderingOptions	options = kATSStyleNoHinting;
		Fract						hangInhibit = fract1;
		ATSUAttributeTag		tags[] = { kATSUFontTag, kATSUSizeTag, kATSUStyleRenderingOptionsTag, kATSUHangingInhibitFactorTag };
		ByteCount				sizes[] = { sizeof(ATSUFontID), sizeof(Fixed), sizeof(ATSStyleRenderingOptions), sizeof(Fract) };
		ATSUAttributeValuePtr	attrs[] = { &fontID, &atsuSize, &options, &hangInhibit };
		ATSUSetAttributes(style, sizeof(tags) / sizeof(ATSUAttributeTag), tags, sizes, attrs);
		
#define FEAT_ALLOC_CHUNK	8
#define VAR_ALLOC_CHUNK		4

		if (cp1 != NULL) {
			int	allocFeats = FEAT_ALLOC_CHUNK;
			UInt16*	featureTypes = (UInt16*)xmalloc(allocFeats * sizeof(UInt16));
			UInt16*	selectorValues = (UInt16*)xmalloc(allocFeats * sizeof(UInt16));
			int	numFeatures = 0;
			
			int	allocVars = VAR_ALLOC_CHUNK;
			UInt32*	axes = (UInt32*)xmalloc(allocVars * sizeof(UInt32));
			SInt32*	values = (SInt32*)xmalloc(allocVars * sizeof(SInt32));
			int	numVariations = 0;
			
			// interpret features & variations following ":"
			while (*cp1) {
				// locate beginning of name=value pair
				if (*cp1 == ':' || *cp1 == ';')	// skip over separator
					++cp1;
				while (*cp1 == ' ' || *cp1 == '\t')	// skip leading whitespace
					++cp1;
				if (*cp1 == 0)	// break if end of string
					break;
	
				// scan to end of pair
				const char*	cp2 = cp1;
				while (*cp2 && *cp2 != ';' && *cp2 != ':')
					++cp2;
	
				// look for the '=' separator
				const char*	cp3 = cp1;
				while (cp3 < cp2 && *cp3 != '=')
					++cp3;
				if (cp3 == cp2)
					goto bad_option;
	
				// now cp1 points to option name, cp3 to '=', cp2 to ';' or null
				
				// first try for a feature by this name
				ATSUFontFeatureType	featureType;
				featureType = find_feature_by_name(fontID, cp1, cp3 - cp1);
				if (featureType != 0x0000FFFF) {
					// look past the '=' separator for setting names
					int	featLen = cp3 - cp1;
					++cp3;
					while (cp3 < cp2) {
						// skip leading whitespace
						while (*cp3 == ' ' || *cp3 == '\t')
							++cp3;
					
						// possibly multiple settings...
						int	disable = 0;
						if (*cp3 == '!') {	// check for negation
							disable = 1;
							++cp3;
						}
						
						// scan for end of setting name
						const char*	cp4 = cp3;
						while (cp4 < cp2 && *cp4 != ',')
							++cp4;
						
						// now cp3 points to name, cp4 to ',' or ';' or null
						ATSUFontFeatureSelector	selectorValue = find_selector_by_name(fontID, featureType, cp3, cp4 - cp3);
						if (selectorValue != 0x0000FFFF) {
							if (numFeatures == allocFeats) {
								allocFeats += FEAT_ALLOC_CHUNK;
								featureTypes = xrealloc(featureTypes, allocFeats * sizeof(UInt16));
								selectorValues = xrealloc(selectorValues, allocFeats * sizeof(UInt16));
							}
							featureTypes[numFeatures] = featureType;
							selectorValues[numFeatures] = selectorValue + disable;
							++numFeatures;
						}
						else {
							fontfeaturewarning(cp1, featLen, cp3, cp4 - cp3);
						}
						
						// point beyond setting name terminator
						cp3 = cp4 + 1;
					}
					
					goto next_option;
				}
				
				// try to find a variation by this name
				ATSUFontVariationAxis	axis;
				axis = find_axis_by_name(fontID, cp1, cp3 - cp1);
				if (axis != 0) {
					// look past the '=' separator for the value
					++cp3;
					double	value = 0.0, decimal = 1.0;
					bool		negate = false;
					if (*cp3 == '-') {
						++cp3;
						negate = true;
					}
					while (cp3 < cp2) {
						int	v = *cp3 - '0';
						if (v >= 0 && v <= 9) {
							if (decimal != 1.0) {
								value += v / decimal;
								decimal *= 10.0;
							}
							else
								value = value * 10.0 + v;
						}
						else if (*cp3 == '.') {
							if (decimal != 1.0)
								break;
							decimal = 10.0;
						}
						else
							break;
						++cp3;
					}
					if (negate)
						value = -value;
					if (numVariations == allocVars) {
						allocVars += VAR_ALLOC_CHUNK;
						axes = xrealloc(axes, allocVars * sizeof(UInt32));
						values = xrealloc(values, allocVars * sizeof(SInt32));
					}
					axes[numVariations] = axis;
					values[numVariations] = value * 65536.0;	//	X2Fix(value);
					++numVariations;
					
					goto next_option;
				}
				
				// didn't find feature or variation, try other options....
	
				i = readCommonFeatures(cp1, cp2, &extend, &slant, &embolden, &letterspace, &rgbValue);
				if (i == 1)
					goto next_option;
				else if (i == -1)
					goto bad_option;
				
				if (strncmp(cp1, "tracking", 8) == 0) {
					cp3 = cp1 + 8;
					if (*cp3 != '=')
						goto bad_option;
					++cp3;
					double	val = read_double(&cp3);
					tracking = X2Fix(val);
					goto next_option;
				}
				
			bad_option:
				// not a name=value pair, or not recognized.... 
				// check for plain "vertical" before complaining
				if (strncmp(cp1, "vertical", 8) == 0) {
					cp3 = cp2;
					if (*cp3 == ';' || *cp3 == ':')
						--cp3;
					while (*cp3 == '\0' || *cp3 == ' ' || *cp3 == '\t')
						--cp3;
					if (*cp3)
						++cp3;
					if (cp3 == cp1 + 8) {
						ATSUVerticalCharacterType	vert = kATSUStronglyVertical;
						tags[0] = kATSUVerticalCharacterTag;
						sizes[0] = sizeof(ATSUVerticalCharacterType);
						attrs[0] = &vert;
						ATSUSetAttributes(style, 1, tags, sizes, attrs);
						goto next_option;
					}
				}
			
				fontfeaturewarning(cp1, cp2 - cp1, 0, 0);
				
			next_option:
				// go to next name=value pair
				cp1 = cp2;
			}
		
			if (numFeatures > 0)
				ATSUSetFontFeatures(style, numFeatures, featureTypes, selectorValues);
	
			if (numVariations > 0)
				ATSUSetVariations(style, numVariations, axes, values);
	
			if ((loadedfontflags & FONT_FLAGS_COLORED) != 0) {
				ATSURGBAlphaColor	rgba;
				rgba.red	= ((rgbValue & 0xFF000000) >> 24) / 255.0;
				rgba.green	= ((rgbValue & 0x00FF0000) >> 16) / 255.0;
				rgba.blue	= ((rgbValue & 0x0000FF00) >> 8 ) / 255.0;
				rgba.alpha	= ((rgbValue & 0x000000FF)      ) / 255.0;
				tags[0] = kATSURGBAlphaColorTag;
				sizes[0] = sizeof(ATSURGBAlphaColor);
				attrs[0] = &rgba;
				ATSUSetAttributes(style, 1, tags, sizes, attrs);
			}
			
			if (tracking != 0x80000000) {
				tags[0] = kATSUTrackingTag;
				sizes[0] = sizeof(Fixed);
				attrs[0] = &tracking;
				ATSUSetAttributes(style, 1, tags, sizes, attrs);
			}
			
			if (extend != 1.0 || slant != 0.0) {
				CGAffineTransform	t = { extend, slant, 0, 1.0, 0, 0 };
				tags[0] = kATSUFontMatrixTag;
				sizes[0] = sizeof(CGAffineTransform);
				attrs[0] = &t;
				ATSUSetAttributes(style, 1, tags, sizes, attrs);
			}
			
			if (embolden != 0.0) {
				embolden = embolden * Fix2X(scaled_size) / 100.0;
				tags[0] = kXeTeXEmboldenTag;
				sizes[0] = sizeof(float);
				attrs[0] = &embolden;
				ATSUSetAttributes(style, 1, tags, sizes, attrs);
			}
			
			if (letterspace != 0.0)
				loadedfontletterspace = (letterspace / 100.0) * scaled_size;
			
			free((char*)featureTypes);
			free((char*)selectorValues);
			free((char*)axes);
			free((char*)values);
		}
	}

	nativefonttypeflag = AAT_FONT_FLAG;
	return style;
}

int
countpdffilepages()
{
	int	rval = 0;

    char*		pic_path = kpse_find_file((char*)nameoffile + 1, kpse_pict_format, 1);
	CFURLRef	picFileURL = NULL;
	if (pic_path) {
		picFileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (UInt8*)pic_path, strlen(pic_path), false);
		if (picFileURL != NULL) {
			FSRef	picFileRef;
			CFURLGetFSRef(picFileURL, &picFileRef);
			CGPDFDocumentRef	document = CGPDFDocumentCreateWithURL(picFileURL);
			if (document != NULL) {
				rval = CGPDFDocumentGetNumberOfPages(document);
				CGPDFDocumentRelease(document);
			}
			CFRelease(picFileURL);
		}
		free(pic_path);
	}

	return rval;
}

int
find_pic_file(char** path, realrect* bounds, int pdfBoxType, int page)
	/* returns bounds in TeX points */
{
	*path = NULL;

	OSStatus	result = fnfErr;
    char*		pic_path = kpse_find_file((char*)nameoffile + 1, kpse_pict_format, 1);
	CFURLRef	picFileURL = NULL;
	if (pic_path) {
		picFileURL = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (UInt8*)pic_path, strlen(pic_path), false);

		if (picFileURL != NULL) {
			/* get an FSRef for the URL we constructed */
			FSRef	picFileRef;
			CFURLGetFSRef(picFileURL, &picFileRef);
			
			if (pdfBoxType > 0) {
				CGPDFDocumentRef	document = CGPDFDocumentCreateWithURL(picFileURL);
				if (document != NULL) {
					int	nPages = CGPDFDocumentGetNumberOfPages(document);
					if (page < 0)
						page = nPages + 1 + page;
					if (page < 1)
						page = 1;
					if (page > nPages)
						page = nPages;

					CGRect	r;
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3
					if (&CGPDFDocumentGetPage == NULL) {
						switch (pdfBoxType) {
							case pdfbox_crop:
							default:
								r = CGPDFDocumentGetCropBox(document, page);
								break;
							case pdfbox_media:
								r = CGPDFDocumentGetMediaBox(document, page);
								break;
							case pdfbox_bleed:
								r = CGPDFDocumentGetBleedBox(document, page);
								break;
							case pdfbox_trim:
								r = CGPDFDocumentGetTrimBox(document, page);
								break;
							case pdfbox_art:
								r = CGPDFDocumentGetArtBox(document, page);
								break;
						}
					}
					else
#endif
					{
						CGPDFPageRef	pageRef = CGPDFDocumentGetPage(document, page);
						CGPDFBox	boxType;
						switch (pdfBoxType) {
							case pdfbox_crop:
							default:
								boxType = kCGPDFCropBox;
								break;
							case pdfbox_media:
								boxType = kCGPDFMediaBox;
								break;
							case pdfbox_bleed:
								boxType = kCGPDFBleedBox;
								break;
							case pdfbox_trim:
								boxType = kCGPDFTrimBox;
								break;
							case pdfbox_art:
								boxType = kCGPDFArtBox;
								break;
						}
						r = CGPDFPageGetBoxRect(pageRef, boxType);
					}

					bounds->x = r.origin.x * 72.27 / 72.0;
					bounds->y = r.origin.y * 72.27 / 72.0;
					bounds->wd = r.size.width * 72.27 / 72.0;
					bounds->ht = r.size.height * 72.27 / 72.0;
					CGPDFDocumentRelease(document);
					result = noErr;
				}
			}
			else {
				/* make an FSSpec for the file, and try to import it as a pic */
				FSSpec	picFileSpec;
				result = FSGetCatalogInfo(&picFileRef, kFSCatInfoNone, NULL, NULL, &picFileSpec, NULL);
				if (result == noErr) {
					ComponentInstance	ci;
					result = GetGraphicsImporterForFile(&picFileSpec, &ci);
					if (result == noErr) {
						ImageDescriptionHandle	desc = NULL;
						result = GraphicsImportGetImageDescription(ci, &desc);
						bounds->x = 0;
						bounds->y = 0;
						bounds->wd = (*desc)->width * 72.27 / Fix2X((*desc)->hRes);
						bounds->ht = (*desc)->height * 72.27 / Fix2X((*desc)->vRes);
						DisposeHandle((Handle)desc);
						(void)CloseComponent(ci);
					}
				}
			}
			
			CFRelease(picFileURL);
		}

		/* if we couldn't import it, toss the pathname as we'll be returning an error */
		if (result != noErr)
			free(pic_path);
		else
			*path = (char*)pic_path;
	}
	
	return result;
}
