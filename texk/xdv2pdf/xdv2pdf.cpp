/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2005 by SIL International
 written by Jonathan Kew

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
NONINFRINGEMENT. IN NO EVENT SHALL SIL INTERNATIONAL BE LIABLE FOR  
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION  
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of SIL International  
shall not be used in advertising or otherwise to promote the sale,  
use or other dealings in this Software without prior written  
authorization from SIL International.
\****************************************************************************/

/*
	xdv2pdf
	Convert xdv file from XeTeX to PDF format for viewing/printing

	usage: xdv2pdf [-d debug] [-m mag] [-p papersize] [-v] xdvfile [-o pdffile]

		-m	override magnification from xdv file
		-v	show progress messages (page counters)
		-d  set kpathsea_debug values
        -p	papersize [default: from Mac OS X printing system]
        
        known papersize values:
            a0-a10
			b0-b10
			c0-c10
			jb0-jb10
            letter
            legal
            ledger
            tabloid
		can append ":landscape"
		or use "x,y" where x and y are dimensions in "big points" or with units
        
		output file defaults to <xdvfile>.pdf

	also permissible to omit xdvfile but specify pdffile; then input is from stdin
*/

#ifdef __POWERPC__
#define MAC_OS_X_VERSION_MIN_REQUIRED	MAC_OS_X_VERSION_10_2
#else
#define MAC_OS_X_VERSION_MIN_REQUIRED	MAC_OS_X_VERSION_10_4
#endif

#include "config.h"

#include <ApplicationServices/ApplicationServices.h>
#include <QuickTime/QuickTime.h>

#include <string>
#include <vector>
#include <list>
#include <map>

#include <sys/stat.h>

#include "DVIops.h"

#define XDV_ID	5	// current id_byte value for .xdv files from xetex

#define XDV_FLAG_FONTTYPE_ATSUI	0x0001
#define XDV_FLAG_FONTTYPE_ICU	0x0002

#define XDV_FLAG_VERTICAL		0x0100
#define XDV_FLAG_COLORED		0x0200
#define XDV_FLAG_FEATURES		0x0400
#define XDV_FLAG_VARIATIONS		0x0800
#define XDV_FLAG_EXTEND			0x1000
#define XDV_FLAG_SLANT			0x2000
#define XDV_FLAG_EMBOLDEN		0x4000

#define pdfbox_crop	1
#define pdfbox_media	2
#define pdfbox_bleed	3
#define pdfbox_trim	4
#define pdfbox_art	5


#define DEFINE_GLOBALS	1
#include "xdv2pdf_globals.h"

paperSizeRec	gPaperSizes[] =
{
	{ "a0",		2383.937008,	3370.393701	},
	{ "a1",		1683.779528,	2383.937008	},
	{ "a2",		1190.551181,	1683.779528	},
	{ "a3",		841.8897638,	1190.551181	},
	{ "a4",		595.2755906,	841.8897638	},
	{ "a5",		419.5275591,	595.2755906	},
	{ "a6",		297.6377953,	419.5275591	},
	{ "a7",		209.7637795,	297.6377953	},
	{ "a8",		147.4015748,	209.7637795	},
	{ "a9",		104.8818898,	147.4015748	},
	{ "a10",	73.7007874,		104.8818898	},
	
	{ "b0",		2834.645669,	4008.188976	},
	{ "b1",		2004.094488,	2834.645669	},
	{ "b2",		1417.322835,	2004.094488	},
	{ "b3",		1000.629921,	1417.322835	},
	{ "b4",		708.6614173,	1000.629921	},
	{ "b5",		498.8976378,	708.6614173	},
	{ "b6",		354.3307087,	498.8976378	},
	{ "b7",		249.4488189,	354.3307087	},
	{ "b8",		175.7480315,	249.4488189	},
	{ "b9",		124.7244094,	175.7480315	},
	{ "b10",	87.87401575,	124.7244094	},
	
	{ "c0",		2599.370079,	3676.535433	},
	{ "c1",		1836.850394,	2599.370079	},
	{ "c2",		1298.267717,	1836.850394	},
	{ "c3",		918.4251969,	1298.267717	},
	{ "c4",		649.1338583,	918.4251969	},
	{ "c5",		459.2125984,	649.1338583	},
	{ "c6",		323.1496063,	459.2125984	},
	{ "c7",		229.6062992,	323.1496063	},
	{ "c8",		161.5748031,	229.6062992	},
	{ "c9",		113.3858268,	161.5748031	},
	{ "c10",	79.37007874,	113.3858268	},
	
	{ "jb0",	2919.685039,	4127.244094	},
	{ "jb1",	2063.622047,	2919.685039	},
	{ "jb2",	1459.84252,		2063.622047	},
	{ "jb3",	1031.811024,	1459.84252	},
	{ "jb4",	728.503937,		1031.811024	},
	{ "jb5",	515.9055118,	728.503937	},
	{ "jb6",	362.8346457,	515.9055118	},
	{ "jb7",	257.9527559,	362.8346457	},
	{ "jb8",	181.4173228,	257.9527559	},
	{ "jb9",	127.5590551,	181.4173228	},
	{ "jb10",	90.70866142,	127.5590551	},

	{ "letter",		612.0,	792.0	},
	{ "legal",		612.0,	1008.0	},
	{ "tabloid",	792.0,	1224.0	},
	{ "ledger",		1224.0,	792.0	},

	{ 0, 0, 0 }
};

static const CGAffineTransform	kHorizontalMatrix = { 1.0,  0.0,  0.0,  1.0,  0.0,  0.0 };
static const CGAffineTransform	kVerticalMatrix   = { 0.0,  1.0, -1.0,  0.0,  0.0,  0.0 };

struct nativeFont {
				nativeFont()
                    : cgFont(0)
                    , atsFont(0)
					, atsuStyleH(0)
					, atsuStyleV(0)
                    , size(Long2Fix(10))
                    , matrix(kHorizontalMatrix)
                    , isColored(false)
                    , isVertical(false)
						{
						}
	CGFontRef			cgFont;
	ATSFontRef			atsFont;
	ATSUStyle			atsuStyleH;	// only present if isVertical is true
	ATSUStyle			atsuStyleV;
	Fixed				size;
	CGColorRef			color;
	CGAffineTransform	matrix;
	float				embolden;
	bool				isColored;
	bool				isVertical;
};

static std::map<UInt32,nativeFont>	sNativeFonts;

static UInt32	sMag = 0;

static bool	sVerbose = false;

static std::map<ATSFontRef,CGFontRef>	sCGFonts;

static CGFontRef
getCGFontForATSFont(ATSFontRef fontRef)
{
	std::map<ATSFontRef,CGFontRef>::const_iterator	i = sCGFonts.find(fontRef);
	if (i != sCGFonts.end()) {
		return i->second;
	}
	CGFontRef	newFont;
	if (fontRef == 0) {
		ATSFontRef	tmpRef = ATSFontFindFromPostScriptName(CFSTR("Helvetica"), kATSOptionFlagsDefault);
		newFont = CGFontCreateWithPlatformFont(&tmpRef);
	}
	else
		newFont = CGFontCreateWithPlatformFont(&fontRef);
	sCGFonts[fontRef] = newFont;
	return newFont;
}

inline UInt32
SWAP(UInt32 v)
{
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return (v << 24)
		+ ((v << 8) & 0x00FF0000)
		+ ((v >> 8) & 0x0000FF00)
		+ (v >> 24);
#endif
}

inline SInt32
SWAP(SInt32 v)
{
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return (v << 24)
		+ ((v << 8) & 0x00FF0000)
		+ ((v >> 8) & 0x0000FF00)
		+ ((v >> 24) & 0x000000FF);
#endif
}

inline UInt16
SWAP(UInt16 v)
{
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return (v << 8) + (v >> 8);
#endif
}

inline SInt16
SWAP(SInt16 v)
{
#ifdef WORDS_BIGENDIAN
	return v;
#else
	return (v << 8) + ((v >> 8) & 0xFF);
#endif
}

/* precaution, just in case we apply these to byte-sized values! */
inline UInt8
SWAP(UInt8 v)
{
	return v;
}

inline SInt8
SWAP(SInt8 v)
{
	return v;
}

void
initAnnotBox()
{
	gAnnotBox.llx = gPageWd;
	gAnnotBox.lly = 0;
	gAnnotBox.urx = 0;
	gAnnotBox.ury = gPageHt;
}

void
flushAnnotBox()
{
	char	buf[200];
	sprintf(buf, "ABOX [%f %f %f %f]",
					kDvi2Scr * gAnnotBox.llx + 72.0,
					kDvi2Scr * (gPageHt - gAnnotBox.lly) - 72.0,
					kDvi2Scr * gAnnotBox.urx + 72.0,
					kDvi2Scr * (gPageHt - gAnnotBox.ury) - 72.0);
	doPDFspecial(buf);
	initAnnotBox();
}

static void
mergeBox(const box& b)
{
	if (b.llx < gAnnotBox.llx)
		gAnnotBox.llx = b.llx;
	if (b.lly > gAnnotBox.lly)
		gAnnotBox.lly = b.lly;	// NB positive is downwards!
	if (b.urx > gAnnotBox.urx)
		gAnnotBox.urx = b.urx;
	if (b.ury < gAnnotBox.ury)
		gAnnotBox.ury = b.ury;
}

long
readSigned(FILE* f, int k)
{
	long	s = (long)(signed char)getc(f);
	while (--k > 0) {
		s <<= 8;
		s += (getc(f) & 0xff);
	}
	return s;
}

unsigned long
readUnsigned(FILE* f, int k)
{
	unsigned long	u = getc(f);
	while (--k > 0) {
		u <<= 8;
		u += (getc(f) & 0xff);
	}
	return u;
}


void
setColor(CGColorRef color, bool force = false)
{
	if (force || !gCurrentColor || !CGColorEqualToColor(color, gCurrentColor)) {
		CGContextSetFillColorWithColor(gCtx, color);
		CGContextSetStrokeColorWithColor(gCtx, color);
	}
	gCurrentColor = color;
}

inline double
radians(double degrees)
{
	return degrees / 180.0 * M_PI;
}

void
ensurePageStarted()
{
	if (gPageStarted)
		return;
	
	CGContextSaveGState(gCtx);
    CGContextBeginPage(gCtx, &gMediaBox);

	static CGAffineTransform	sInitTextMatrix;
	if (gPageIndex == 1) {
		sInitTextMatrix = CGContextGetTextMatrix(gCtx);
	}
	else
		CGContextSetTextMatrix(gCtx, sInitTextMatrix);

	gCurrentColor = NULL; /* ensure color will get set by first drawing op */

	CGContextTranslateCTM(gCtx, 72.0, -72.0);

    gPageWd = kScr2Dvi * gMediaBox.size.width;
    gPageHt = kScr2Dvi * gMediaBox.size.height;
	
	cur_cgFont = kUndefinedFont;

	gPageStarted = true;

	paintBackground();
}

void
paintBackground()
{
	if (gBackground != NULL) {
		if (!gPageStarted)
			ensurePageStarted(); /* this will call paintBackground() again */
		else {
			CGContextSaveGState(gCtx);
			CGContextSetFillColorWithColor(gCtx, gBackground);
			CGContextTranslateCTM(gCtx, -72.0, 72.0);
			CGContextFillRect(gCtx, gMediaBox);
			CGContextRestoreGState(gCtx);
		}
	}
}

#define MAX_BUFFERED_GLYPHS	1024
int			gGlyphCount = 0;
CGGlyph		gGlyphs[MAX_BUFFERED_GLYPHS];
CGSize		gAdvances[MAX_BUFFERED_GLYPHS+1];
CGPoint		gStartLoc;
CGPoint		gPrevLoc;

static void
myShowGlyphsWithAdvances(CGContextRef c, const CGGlyph* glyphs, const CGSize* advances, int count)
{
	CGPoint	loc = CGContextGetTextPosition(c);
	while (count > 0) {
		CGContextShowGlyphsAtPoint(c, loc.x, loc.y, glyphs, 1);
		loc.x += advances->width;
		loc.y += advances->height;
		++advances;
		++glyphs;
		--count;
	}
}

void
flushGlyphs()
{
	if (gGlyphCount > 0) {
		CGContextSetTextPosition(gCtx, gStartLoc.x, gStartLoc.y);
		gAdvances[gGlyphCount].width = 0;
		gAdvances[gGlyphCount].height = 0;

		if (sNativeFonts[cur_cgFont].isColored)
			setColor(sNativeFonts[cur_cgFont].color);
		else
			setColor(gTextColor);
		if (&CGContextShowGlyphsWithAdvances != NULL)
			CGContextShowGlyphsWithAdvances(gCtx, gGlyphs, gAdvances, gGlyphCount);
		else
			myShowGlyphsWithAdvances(gCtx, gGlyphs, gAdvances, gGlyphCount);

		gGlyphCount = 0;
	}
}

void
bufferGlyph(CGGlyph g)
{
	ensurePageStarted();

	CGPoint	curLoc;
	curLoc.x = kDvi2Scr * dvi.h;
	curLoc.y = kDvi2Scr * (gPageHt - dvi.v);
	if (gGlyphCount == 0)
		gStartLoc = curLoc;
	else {
		gAdvances[gGlyphCount-1].width = curLoc.x - gPrevLoc.x;
		gAdvances[gGlyphCount-1].height = curLoc.y - gPrevLoc.y;
	}
	gPrevLoc = curLoc;
	gGlyphs[gGlyphCount] = g;
	if (++gGlyphCount == MAX_BUFFERED_GLYPHS)
		flushGlyphs();
}

void
setChar(UInt32 c, bool adv)
{
	OSStatus	status;

	ensurePageStarted();

    if (f != cur_cgFont) {
		flushGlyphs();
        CGContextSetFont(gCtx, gTeXFonts[f].cgFont);
        CGContextSetFontSize(gCtx, Fix2X(gTeXFonts[f].size) * gMagScale);
		CGContextSetTextMatrix(gCtx, kHorizontalMatrix);
		CGContextSetTextDrawingMode(gCtx, kCGTextFill);
        cur_cgFont = f;
	}

    CGGlyph	g;
	if (gTeXFonts[f].charMap != NULL) {
		if (c < gTeXFonts[f].charMap->size())
			g = (*gTeXFonts[f].charMap)[c];
		else
			g = c;
	}
	else
		// default: glyph ID = char + 1, works for the majority of the CM PS fonts
	    g = c + 1;
	    
	bufferGlyph(g);

	if (gTrackingBoxes) {
		if (gTeXFonts[f].hasMetrics) {
			box	b = {
				dvi.h,
				dvi.v + gTeXFonts[f].depths[c],
				dvi.h + gTeXFonts[f].widths[c],
				dvi.v - gTeXFonts[f].heights[c] };
			mergeBox(b);
		}
	}

	if (adv && gTeXFonts[f].hasMetrics)
		dvi.h += gTeXFonts[f].widths[c];
}


static FixedPoint
getGlyphVOrg(const nativeFont& f, UInt16 gid)
{
	FixedPoint	vorg;
	ATSGlyphIdealMetrics	metrics;
	OSStatus	status = ATSUGlyphGetIdealMetrics(f.atsuStyleV, 1, &gid, 0, &metrics);
/*
	fprintf(stderr, "\nmetrics(V) for glyph %d:  adv = (%.3f, %.3f)  sb = (%.3f, %.3f)  os = (%.3f, %.3f)\n", gid,
			metrics.advance.x, metrics.advance.y, metrics.sideBearing.x, metrics.sideBearing.y,
			metrics.otherSideBearing.x, metrics.otherSideBearing.y);
*/
	vorg.y = X2Fix(-metrics.advance.y/* - metrics.otherSideBearing.y*/);
	status = ATSUGlyphGetIdealMetrics(f.atsuStyleH, 1, &gid, 0, &metrics);
/*
	fprintf(stderr, "metrics(H) for glyph %d:  adv = (%.3f, %.3f)  sb = (%.3f, %.3f)  os = (%.3f, %.3f)\n", gid,
			metrics.advance.x, metrics.advance.y, metrics.sideBearing.x, metrics.sideBearing.y,
			metrics.otherSideBearing.x, metrics.otherSideBearing.y);
*/
	vorg.x = X2Fix(metrics.advance.x / 2);
	return vorg;
}

static void
doGlyphArray(FILE* xdv, bool yLocs)
{
	static int			maxGlyphs = 0;
	static FixedPoint*	locations = 0;
	static CGSize*		advances = 0;
	static UInt16*		glyphs = 0;

	flushGlyphs();

	Fixed	wid = readUnsigned(xdv, 4);
	int	glyphCount = readUnsigned(xdv, 2);
	
	if (glyphCount >= maxGlyphs) {
		maxGlyphs = glyphCount + 100;
		if (glyphs != 0)
			delete[] glyphs;
		glyphs = new UInt16[maxGlyphs];
		if (locations != 0)
			delete[] locations;
		locations = new FixedPoint[maxGlyphs];
		if (advances != 0)
			delete[] advances;
		advances = new CGSize[maxGlyphs];
	}

	for (int i = 0; i < glyphCount; ++i) {
		locations[i].x = readSigned(xdv, 4);
		locations[i].y = yLocs ? readSigned(xdv, 4) : 0;
	}
	for (int i = 0; i < glyphCount; ++i)
		glyphs[i] = readUnsigned(xdv, 2);
	
	if (f != cur_cgFont) {
		CGContextSetFont(gCtx, sNativeFonts[f].cgFont);
		CGContextSetFontSize(gCtx, Fix2X(sNativeFonts[f].size));
		CGContextSetTextMatrix(gCtx, sNativeFonts[f].matrix);
		if (sNativeFonts[f].embolden == 0.0) {
			CGContextSetTextDrawingMode(gCtx, kCGTextFill);
		}
		else {
			CGContextSetTextDrawingMode(gCtx, kCGTextFillStroke);
			CGContextSetLineWidth(gCtx, sNativeFonts[f].embolden);
		}
		cur_cgFont = f;
	}

	if (sNativeFonts[f].isColored)
        setColor(sNativeFonts[f].color);
	else
        setColor(gTextColor);

	if (sNativeFonts[f].isVertical) {
		for (int i = 0; i < glyphCount; ++i) {
			// need to adjust position to right by VOrg.y and down by VOrg.x
			FixedPoint	vorg = getGlyphVOrg(sNativeFonts[f], glyphs[i]);
			locations[i].x += vorg.y;
			locations[i].y += vorg.x;
		}
	}

	CGContextSetTextPosition(gCtx, kDvi2Scr * dvi.h + Fix2X(locations[0].x) * 72 / 72.27,
									kDvi2Scr * (gPageHt - dvi.v) - Fix2X(locations[0].y) * 72 / 72.27);

	for (int i = 0; i < glyphCount - 1; ++i) {
		advances[i].width = gMagScale * Fix2X(locations[i+1].x - locations[i].x) * 72 / 72.27;
		advances[i].height = gMagScale * Fix2X(locations[i].y - locations[i+1].y) * 72 / 72.27;
	}
	advances[glyphCount-1].width = 0.0;
	advances[glyphCount-1].height = 0.0;

	if (&CGContextShowGlyphsWithAdvances != NULL)
		CGContextShowGlyphsWithAdvances(gCtx, glyphs, advances, glyphCount);
	else
		myShowGlyphsWithAdvances(gCtx, glyphs, advances, glyphCount);

	if (gTrackingBoxes) {
		box	b = {
			dvi.h,
			dvi.v + (sNativeFonts[f].size / 3),
			dvi.h + wid,
			dvi.v - (2 * sNativeFonts[f].size / 3) };
		mergeBox(b);
	}

	dvi.h += wid;
}

/* code lifted almost verbatim from Apple sample "QTtoCG" */
typedef struct {
	size_t width;
	size_t height;
	size_t bitsPerComponent;
	size_t bitsPerPixel;
	size_t bytesPerRow;
	size_t size;
	CGImageAlphaInfo ai;
	CGColorSpaceRef cs;
	unsigned char *data;
	CMProfileRef prof;
} BitmapInfo;

void readBitmapInfo(GraphicsImportComponent gi, BitmapInfo *bi)
{
	ComponentResult result;
	ImageDescriptionHandle imageDescH = NULL;
	ImageDescription *desc;
	Handle profile = NULL;
	
	result = GraphicsImportGetImageDescription(gi, &imageDescH);
	if( noErr != result || imageDescH == NULL ) {
		fprintf(stderr, "Error while retrieving image description");
		exit(1);
	}
	
	desc = *imageDescH;
	
	bi->width = desc->width;
	bi->height = desc->height;
	bi->bitsPerComponent = 8;
	bi->bitsPerPixel = 32;
	bi->bytesPerRow = (bi->bitsPerPixel * bi->width + 7)/8;
	bi->ai = (desc->depth == 32) ? kCGImageAlphaFirst : kCGImageAlphaNoneSkipFirst;
	bi->size = bi->bytesPerRow * bi->height;
	bi->data = (unsigned char*)malloc(bi->size);
	
	bi->cs = NULL;
	bi->prof = NULL;
	GraphicsImportGetColorSyncProfile(gi, &profile);
	if( NULL != profile ) {
		CMError err;
		CMProfileLocation profLoc;
		Boolean bValid, bPreferredCMMNotFound;

		profLoc.locType = cmHandleBasedProfile;
		profLoc.u.handleLoc.h = profile;
		
		err = CMOpenProfile(&bi->prof, &profLoc);
		if( err != noErr ) {
			fprintf(stderr, "Cannot open profile");
			exit(1);
		}
		
		/* Not necessary to validate profile, but good for debugging */
		err = CMValidateProfile(bi->prof, &bValid, &bPreferredCMMNotFound);
		if( err != noErr ) {
			fprintf(stderr, "Cannot validate profile : Valid: %d, Preferred CMM not found : %d", bValid, 
				  bPreferredCMMNotFound);
			exit(1);
		}
		
		bi->cs = CGColorSpaceCreateWithPlatformColorSpace( bi->prof );

		if( bi->cs == NULL ) {
			fprintf(stderr, "Error creating cg colorspace from csync profile");
			exit(1);
		}
		DisposeHandle(profile);
	}	
	
	if( imageDescH != NULL)
		DisposeHandle((Handle)imageDescH);
}

void getBitmapData(GraphicsImportComponent gi, BitmapInfo *bi)
{
	GWorldPtr gWorld;
	QDErr err = noErr;
	Rect boundsRect = { 0, 0, bi->height, bi->width };
	ComponentResult result;
	
	if( bi->data == NULL ) {
		fprintf(stderr, "no bitmap buffer available");
		exit(1);
	}
	
	err = NewGWorldFromPtr( &gWorld, k32ARGBPixelFormat, &boundsRect, NULL, NULL, 0, 
							(char*)bi->data, bi->bytesPerRow );
	if (noErr != err) {
		fprintf(stderr, "error creating new gworld - %d", err);
		exit(1);
	}
	
	if( (result = GraphicsImportSetGWorld(gi, gWorld, NULL)) != noErr ) {
		fprintf(stderr, "error while setting gworld");
		exit(1);
	}
	
	if( (result = GraphicsImportDraw(gi)) != noErr ) {
		fprintf(stderr, "error while drawing image through qt");
		exit(1);
	}
	
	DisposeGWorld(gWorld);	
}
/* end of code from "QTtoCG" */

struct imageRec {
	CGImageRef	ref;
	CGRect		bounds;
};
std::map<std::string,imageRec>			sCGImages;
std::map<std::string,CGPDFDocumentRef>	sPdfDocs;

static void
doPicFile(FILE* xdv, int pdfBoxType)	// t[4][6] p[2] l[2] a[l]
{
    CGAffineTransform	t;
    t.a = Fix2X(readSigned(xdv, 4));
    t.b = Fix2X(readSigned(xdv, 4));
    t.c = Fix2X(readSigned(xdv, 4));
    t.d = Fix2X(readSigned(xdv, 4));
    t.tx = Fix2X(readSigned(xdv, 4));
    t.ty = Fix2X(readSigned(xdv, 4));
	if (sMag != 1000)
		t = CGAffineTransformConcat(t, CGAffineTransformMakeScale(gMagScale, gMagScale));

    int		p = readSigned(xdv, 2);
    int		l = readUnsigned(xdv, 2);
    unsigned char*	pathname = new unsigned char[l + 1];
	for (int i = 0; i < l; ++i)
		pathname[i] = readUnsigned(xdv, 1);
	pathname[l] = 0;

	CFURLRef	url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, pathname, l, false);
	if (url != NULL) {
		std::string	pathString((char*)pathname, l);	// key for our map<>s of already-used images

		// is it a \pdffile instance?
		CGPDFDocumentRef	document = NULL;
		CGImageRef			image = NULL;
		CGRect				bounds;
		CGPDFBox			cgPdfBoxType;
		if (pdfBoxType > 0) {
			std::map<std::string,CGPDFDocumentRef>::const_iterator	i = sPdfDocs.find(pathString);
			if (i != sPdfDocs.end())
				document = i->second;
			else {
				document = CGPDFDocumentCreateWithURL(url);
				sPdfDocs[pathString] = document;
			}
			if (document != NULL) {
				int	nPages = CGPDFDocumentGetNumberOfPages(document);
				if (p < 0)			p = nPages + 1 + p;
				if (p > nPages)		p = nPages;
				if (p < 1)			p = 1;
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3
				if (&CGPDFDocumentGetPage == NULL) {
					switch (pdfBoxType) {
						case pdfbox_crop:
						default:
							bounds = CGPDFDocumentGetCropBox(document, p);
							break;
						case pdfbox_media:
							bounds = CGPDFDocumentGetMediaBox(document, p);
							break;
						case pdfbox_bleed:
							bounds = CGPDFDocumentGetBleedBox(document, p);
							break;
						case pdfbox_trim:
							bounds = CGPDFDocumentGetTrimBox(document, p);
							break;
						case pdfbox_art:
							bounds = CGPDFDocumentGetArtBox(document, p);
							break;
					}
				}
				else
#endif
				{
					CGPDFPageRef	pageRef = CGPDFDocumentGetPage(document, p);
					switch (pdfBoxType) {
						case pdfbox_crop:
						default:
							cgPdfBoxType = kCGPDFCropBox;
							break;
						case pdfbox_media:
							cgPdfBoxType = kCGPDFMediaBox;
							break;
						case pdfbox_bleed:
							cgPdfBoxType = kCGPDFBleedBox;
							break;
						case pdfbox_trim:
							cgPdfBoxType = kCGPDFTrimBox;
							break;
						case pdfbox_art:
							cgPdfBoxType = kCGPDFArtBox;
							break;
					}
					bounds = CGPDFPageGetBoxRect(pageRef, cgPdfBoxType);
				}
			}
		}

		// otherwise use GraphicsImport
		else {
			std::map<std::string,imageRec>::const_iterator	i = sCGImages.find(pathString);
			if (i != sCGImages.end()) {
				image = i->second.ref;
				bounds = i->second.bounds;
			}
			else {
				FSRef	ref;
				if (CFURLGetFSRef(url, &ref)) {
					FSSpec	spec;
					if (FSGetCatalogInfo(&ref, kFSCatInfoNone, NULL, NULL, &spec, NULL) == noErr) {
						ComponentInstance	gi;
						OSErr	result = GetGraphicsImporterForFile(&spec, &gi);
						if (result == noErr) {
							BitmapInfo	bi;
							readBitmapInfo(gi, &bi);
							getBitmapData(gi, &bi);
			
							if (bi.cs == NULL)
								bi.cs = CGColorSpaceCreateDeviceRGB();
							CGDataProviderRef	provider = CGDataProviderCreateWithData(NULL, bi.data, bi.size, NULL);
							image = CGImageCreate(bi.width, bi.height, bi.bitsPerComponent, bi.bitsPerPixel,
													bi.bytesPerRow, bi.cs, bi.ai, provider, NULL, 0, kCGRenderingIntentDefault);
							CGColorSpaceRelease(bi.cs);
		
							ImageDescriptionHandle	desc = NULL;
							result = GraphicsImportGetImageDescription(gi, &desc);
							bounds.origin.x = 0;
							bounds.origin.y = 0;
							bounds.size.width = (*desc)->width * 72.0 / Fix2X((*desc)->hRes);
							bounds.size.height = (*desc)->height * 72.0 / Fix2X((*desc)->vRes);
							DisposeHandle((Handle)desc);
							result = CloseComponent(gi);
						}
						imageRec	ir = { image, bounds };
						sCGImages[pathString] = ir;
					}
				}
			}
		}

		CGContextSaveGState(gCtx);

		CGContextTranslateCTM(gCtx, kDvi2Scr * dvi.h, kDvi2Scr * (gPageHt - dvi.v));
		CGContextConcatCTM(gCtx, t);

		if (document != NULL) {
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_3
			if (&CGPDFDocumentGetPage == NULL) {
				CGRect	mediaBox = CGPDFDocumentGetMediaBox(document, p);
				CGRect	cropBox = CGPDFDocumentGetCropBox(document, p);
				CGContextClipToRect(gCtx, cropBox);
				CGContextDrawPDFDocument(gCtx, mediaBox, document, p);
			}
			else
#endif
			{
				CGPDFPageRef	pageRef = CGPDFDocumentGetPage(document, p);
				CGAffineTransform	xf = CGPDFPageGetDrawingTransform(pageRef, cgPdfBoxType, bounds, 0, true);
				CGContextConcatCTM(gCtx, xf);
				CGRect	cropBox = CGPDFPageGetBoxRect(pageRef, kCGPDFCropBox);
				CGContextClipToRect(gCtx, cropBox);
				CGContextDrawPDFPage(gCtx, pageRef);
			}
		}
		else if (image != NULL)
			CGContextDrawImage(gCtx, bounds, image);

		if (gTrackingBoxes) {
			// figure out the corners of the transformed and positioned image,
			// and remember the lower left and upper right of the result
			CGPoint	p[4];
			p[0].x = bounds.origin.x;
			p[0].y = bounds.origin.y;
			p[1].x = bounds.origin.x;
			p[1].y = bounds.origin.y + bounds.size.height;
			p[2].x = bounds.origin.x + bounds.size.width;
			p[2].y = bounds.origin.y + bounds.size.height;
			p[3].x = bounds.origin.x + bounds.size.width;
			p[3].y = bounds.origin.y;
			
			CGPoint	ll = { MAXFLOAT, MAXFLOAT };
			CGPoint	ur = { -MAXFLOAT, -MAXFLOAT };

			t = CGContextGetCTM(gCtx);
			// now t is the CTM, including positioning as well as transformations of the image

			for (int i = 0; i < 4; ++i) {
				p[i] = CGPointApplyAffineTransform(p[i], t);
				if (p[i].x < ll.x)
					ll.x = p[i].x;
				if (p[i].y < ll.y)
					ll.y = p[i].y;
				if (p[i].x > ur.x)
					ur.x = p[i].x;
				if (p[i].y > ur.y)
					ur.y = p[i].y;
			}
			
			// convert back to dvi space and add to the annotation area
			box	b = {
				(ll.x - 72.0) * kScr2Dvi,
				gPageHt - (ur.y + 72.0) * kScr2Dvi,
				(ur.x - 72.0) * kScr2Dvi,
				gPageHt - (ll.y + 72.0) * kScr2Dvi
			};
			mergeBox(b);
		}

		CGContextRestoreGState(gCtx);

		CFRelease(url);
	}
	
}

/* declarations of KPATHSEARCH functions we use for finding TFMs and OTFs */
extern "C" {
#include <kpathsea/kpathsea.h>
#define kpse_find_file(name, type, must_exist) reinterpret_cast<UInt8*> \
  (kpse_find_file(reinterpret_cast<const_string>(name), \
                  (kpse_file_format_type)type, must_exist))
#define uppercasify(s) reinterpret_cast<UInt8*> \
  (uppercasify(reinterpret_cast<const_string>(s)))
};

#include "xdv_kpse_formats.h"

static void
loadMetrics(struct texFont& font, UInt8* name, Fixed d, Fixed s)
{
	UInt8*	pathname = kpse_find_file(name, xdv_kpse_tfm_format, true);
    if (pathname) {
        FILE*	tfmFile = fopen((char*)pathname, "rb");
        if (tfmFile != 0) {
            enum { lf = 0, lh, bc, ec, nw, nh, nd, ni, nl, nk, ne, np };
            SInt16	directory[12];
			for (int i = 0; i < 12; ++i)
				directory[i] = readSigned(tfmFile, 2);
            fseek(tfmFile, directory[lh] * 4, SEEK_CUR);
            int	nChars = directory[ec] - directory[bc] + 1;
            double_t	factor = Fix2X(d) / 16.0;
            if (s != d)
                factor = factor * Fix2X(s) / Fix2X(d);
            if (nChars > 0) {
                struct s_charinfo {
                    UInt8	widthIndex;
                    UInt8	heightDepth;
                    UInt8	italicIndex;
                    UInt8	remainder;
                };
                s_charinfo*	charInfo = new s_charinfo[nChars];
				for (int i = 0; i < nChars; ++i) {
					charInfo[i].widthIndex = readUnsigned(tfmFile, 1);
					charInfo[i].heightDepth = readUnsigned(tfmFile, 1);
					charInfo[i].italicIndex = readUnsigned(tfmFile, 1);
					charInfo[i].remainder = readUnsigned(tfmFile, 1);
				}
                Fixed*	widths = new Fixed[directory[nw]];
				for (int i = 0; i < directory[nw]; ++i)
					widths[i] = readSigned(tfmFile, 4);
                Fixed*	heights = new Fixed[directory[nh]];
				for (int i = 0; i < directory[nh]; ++i)
					heights[i] = readSigned(tfmFile, 4);
                Fixed*	depths = new Fixed[directory[nd]];
                for (int i = 0; i < directory[nd]; ++i)
                	depths[i] = readSigned(tfmFile, 4);
                
                font.widths.reserve(directory[ec] + 1);
                font.heights.reserve(directory[ec] + 1);
                font.depths.reserve(directory[ec] + 1);
                font.charMap = new std::vector<UInt16>;
                font.charMap->reserve(directory[ec] + 1);
                for (int i = 0; i < directory[bc]; ++i) {
                    font.widths.push_back(0);
                    font.heights.push_back(0);
                    font.depths.push_back(0);
                    font.charMap->push_back(0);
                }
                int	g = 0;
                for (int i = 0; i < nChars; ++i) {
                	if (charInfo[i].widthIndex == 0) {
						font.widths.push_back(0);
						font.heights.push_back(0);
						font.depths.push_back(0);
						font.charMap->push_back(0);
					}
					else {
						double_t	wid = Fix2X(widths[charInfo[i].widthIndex]) * factor;
						font.widths.push_back(X2Fix(wid));

						int	heightIndex = (charInfo[i].heightDepth >> 4);
						double_t	ht = Fix2X(heights[heightIndex]) * factor;
						font.heights.push_back(X2Fix(ht));
						
						int	depthIndex = (charInfo[i].heightDepth & 0x0f);
						double_t	dp = Fix2X(depths[depthIndex]) * factor;
						font.depths.push_back(X2Fix(dp));

						font.charMap->push_back(++g);
					}
                }
                
                delete[] widths;
                delete[] heights;
                delete[] depths;
                delete[] charInfo;
            }
    
            font.hasMetrics = true;
            fclose(tfmFile);
        }
        free(pathname);
    }
}

typedef std::vector<std::string>	encoding;
std::map<std::string,encoding>		sEncodings;

class fontMapRec {
public:
				fontMapRec()
					: cgFont(0)
					, cmap(NULL)
					, loaded(false)
					{ }

	std::string	psName;
	std::string	encName;
	std::string	pfbName;

	CGFontRef	cgFont;
	std::vector<UInt16>*	cmap;

	bool		loaded;
};

std::map<std::string,fontMapRec>	psFontsMap;

static void
clearFontMap()
{
	std::map<std::string,fontMapRec>::iterator	i;
	for (i = psFontsMap.begin(); i != psFontsMap.end(); ++i) {
		if (i->second.cmap != NULL)
			delete i->second.cmap;
	}
	psFontsMap.clear();
}

void
doPdfMapLine(const char* line, char mode)
{
	if (*line == '%')
		return;
	while (*line == ' ' || *line == '\t')
		++line;
	if (*line < ' ')
		return;

	if (mode == 0) {
		switch (*line) {
			case '+':
			case '-':
			case '=':
				mode = *line;
				++line;
				while (*line == ' ' || *line == '\t')
					++line;
				if (*line < ' ')
					return;
				break;
			default:
				clearFontMap();
				mode = '+';
				break;
		}
	}

	const char*	b = line;
	const char*	e = b;
	while (*e > ' ')
		++e;
	std::string	tfmName(b, e - b);
	
	if (mode == '-') {
		psFontsMap.erase(tfmName);
		return;
			// erase existing entry
	}
	
	if ((mode == '+') && (psFontsMap.find(tfmName) != psFontsMap.end()))
		return;
			// don't add if entry already exists
	
	while (*e && *e <= ' ')
		++e;
	b = e;
	while (*e > ' ')
		++e;
	if (e > b) {
		fontMapRec	fr;
		fr.psName.assign(b, e - b);
		while (*e && *e <= ' ')
			++e;
		if (*e == '"') {	// skip quoted string; we don't do oblique, stretch, etc. (yet)
			++e;
			while (*e && *e != '"')
				++e;
			if (*e == '"')
				++e;
			while (*e && *e <= ' ')
				++e;
		}
		while (*e == '<') {
			++e;
			b = e;
			while (*e > ' ')
				++e;
			if (strncmp(e - 4, ".enc", 4) == 0) {
				fr.encName.assign(b, e - b);
			}
			else if (strncmp(e - 4, ".pfb", 4) == 0) {
				fr.pfbName.assign(b, e - b);
			}
/*
			else if (strncmp(e - 4, ".pfa", 4) == 0) {
				fr.otfName.assign(b, e - b - 4);
			}
*/
			while (*e && *e <= ' ')
				++e;
		}
		psFontsMap[tfmName] = fr;
	}
}

static bool	sMapFileLoaded = false;

void
doPdfMapFile(const char* fileName)
{
	char	mode = 0;

	while (*fileName == ' ' || *fileName == '\t')
		++fileName;
	if (*fileName < ' ')
		return;

	switch (*fileName) {
		case '+':
		case '-':
		case '=':
			mode = *fileName;
			++fileName;
			break;
		default:
			clearFontMap();
			mode = '+';
			break;
	}
	while (*fileName == ' ' || *fileName == '\t')
		++fileName;
	if (*fileName < ' ')
		return;

	bool	loaded = false;
	UInt8*	pathname = kpse_find_file((UInt8*)fileName, xdv_kpse_font_map_format, true);
	if (pathname) {
		FILE*	f = fopen((char*)pathname, "r");
		if (f != NULL) {
			char	line[1000];
			while (!feof(f)) {
				if (fgets(line, 999, f) == 0)
					break;
				doPdfMapLine(line, mode);

			}
			loaded = true;
			fclose(f);
			if (sVerbose)
				fprintf(stderr, "\n{fontmap: %s} ", pathname);
		}
		free(pathname);
	}
	if (!loaded)
		fprintf(stderr, "\n*** fontmap %s not found; texmf.cnf may be broken\n", fileName);
	else
		sMapFileLoaded = true;
}

// return a pointer to the encoding vector with the given name, loading it if necessary
// we don't really "parse" the .enc file
static const encoding*
getEncoding(const std::string& name)
{
	std::map<std::string,encoding>::iterator	i = sEncodings.find(name);
	if (i == sEncodings.end()) {
		encoding	enc;
		UInt8*	pathname = kpse_find_file((UInt8*)(name.c_str()), xdv_kpse_enc_format, true);
		if (pathname != NULL) {
			FILE*	f = fopen((char*)pathname, "r");
			if (f != NULL) {
				int	c;
				bool	inVector = false;
				std::string	str;
				while ((c = getc(f)) != EOF) {
				got_ch:
					if (c == '%') {	// comment: skip rest of line
						while ((c = getc(f)) != EOF && c != '\r' && c != '\n')
							;
						goto got_ch;
					}
					if (c == '[') {
						inVector = true;
					}
					else if (c == '/' || c <= ' ' || c == ']' || c == EOF) {
						if (inVector && str.length() > 0)
							enc.push_back(str);
						str.clear();
					}
					else if (inVector && c != EOF)
						str.append(1, (char)c);
				}
				if (sVerbose)
					fprintf(stderr, "\n{encoding: %s} ", pathname);
				fclose(f);
			}
			free(pathname);
		}
		sEncodings[name] = enc;
		return &(sEncodings[name]);
	}

	return &(i->second);
}

static ATSFontRef
activateFromPath(const char* pathName)
{
	if (sVerbose)
		fprintf(stderr, "\n{activate %s", pathName);
	ItemCount	count = 0;
	FSRef		fontFileRef;
	bzero(&fontFileRef, sizeof(fontFileRef));
	OSStatus	status = FSPathMakeRef((UInt8*)pathName, &fontFileRef, 0);
	if (status == noErr) {
		++count;
		FSSpec		fontFileSpec;
		bzero(&fontFileSpec, sizeof(fontFileSpec));
		status = FSGetCatalogInfo(&fontFileRef, 0, 0, 0, &fontFileSpec, 0);
		if (status == noErr) {
			++count;
			ATSFontContainerRef containerRef = 0;
			status = ATSFontActivateFromFileSpecification(&fontFileSpec, kATSFontContextLocal,
							kATSFontFormatUnspecified, 0, kATSOptionFlagsDefault, &containerRef);
			if (status == noErr) {
				++count;
				ATSFontRef	fontRef;
				status = ATSFontFindFromContainer(containerRef, 0, 1, &fontRef, &count);
				if (status == noErr && count == 1) {
					if (sVerbose)
						fprintf(stderr, "}");
					return fontRef;
				}
				// failed, or container contained multiple fonts(!) -- confused
				ATSFontDeactivate(containerRef, 0, kATSOptionFlagsDefault);
			}
		}
	}
	if (sVerbose)
		fprintf(stderr, "... failed: status=%d, count=%d}", status, count);
	return 0;
}

struct postTable {
	Fixed	format;
	Fixed	italicAngle;
	SInt16	underlinePosition;
	SInt16	underlineThickness;
	UInt16	isFixedPitch;
	UInt16	reserved;
	UInt32	minMemType42;
	UInt32	maxMemType42;
	UInt32	minMemType1;
	UInt32	maxMemType1;
};

#include "appleGlyphNames.c"

#define	sfntCacheDir	"/Library/Caches/Type1-sfnt-fonts/"
#define	sfntWrapCommand	"T1Wrap"
#define sfntWrapSuffix	"-sfnt.otf"

static ATSFontRef
activatePFB(const char* pfbName)
{
	static std::map<std::string,ATSFontRef>	sActivatedFonts;

	std::map<std::string,ATSFontRef>::iterator i = sActivatedFonts.find(pfbName);
	if (i != sActivatedFonts.end())
		return i->second;

	ATSFontRef	fontRef = 0;
	OSStatus	status = noErr;

	static int firstTime = 1;
	if (firstTime) {
		firstTime = 0;
		status = mkdir(sfntCacheDir, S_IRWXU | S_IRWXG | S_IRWXO);
		if (status != 0) {
			if (errno == EEXIST) {
				// clean up possible residue from past failed conversions
				system("/usr/bin/find " sfntCacheDir " -maxdepth 1 -empty -type f -delete");
				status = 0;
			}
			else
				fprintf(stderr, "*** failed to create sfnt cache directory %s (errno = %d), cannot activate .pfb fonts\n",
						sfntCacheDir, errno);
		}
	}
	
	char*	sfntName = new char[strlen(sfntCacheDir) + strlen(pfbName) + strlen(sfntWrapSuffix) + 1];
	strcpy(sfntName, sfntCacheDir);
	strcat(sfntName, pfbName);
	strcat(sfntName, sfntWrapSuffix);

	FSRef	ref;
	status = FSPathMakeRef((const UInt8*)sfntName, &ref, NULL);
	if (status == fnfErr) {
		char*	pathName = (char*)kpse_find_file((UInt8*)pfbName, xdv_kpse_pfb_format, true);
		if (pathName != NULL) {
			char* cmd = new char[strlen(sfntWrapCommand) + strlen(pathName) + strlen(sfntName) + 6];
			strcpy(cmd, sfntWrapCommand " ");
			strcat(cmd, pathName);
			strcat(cmd, " > ");
			strcat(cmd, sfntName);
			status = system(cmd);
			delete[] cmd;
			free(pathName);
		}
	}
	
	if (status == noErr)
		fontRef = activateFromPath(sfntName);

	delete[] sfntName;

	if (fontRef == 0) {
		// try removing extension (.pfb) and looking for an .otf font...
		char*	baseName = new char[strlen(pfbName) + 1];
		strcpy(baseName, pfbName);
		char*	dot = strrchr(baseName, '.');
		if (dot != NULL)
			*dot = 0;
		char*	pathName = (char*)kpse_find_file((UInt8*)baseName, xdv_kpse_otf_format, true);
		delete[] baseName;
		if (pathName != NULL) {
			fontRef = activateFromPath(pathName);
			free(pathName);
		}
	}
	
	if (fontRef == 0)
		fprintf(stderr, "\n*** font activation failed (status=%d): %s\n", status, pfbName);

	sActivatedFonts[pfbName] = fontRef;
	return fontRef;
}

static std::vector<UInt16>*
readMacRomanCmap(ATSFontRef fontRef)
{
	std::vector<UInt16>*	cmap = NULL;
	ByteCount	size;
	OSStatus	status = ATSFontGetTable(fontRef, 'cmap', 0, 0, 0, &size);
	if (status == noErr) {
		UInt8*	buffer = new UInt8[size];
		ATSFontGetTable(fontRef, 'cmap', 0, size, buffer, &size);

		struct cmapHeader {
			UInt16	version;
			UInt16	numSubtables;
		};
		struct subtableHeader {
			UInt16	platform;
			UInt16	encoding;
			UInt32	offset;
		};
		struct format0 {
			UInt16	format;
			UInt16	length;
			UInt16	language;
			UInt8	glyphIndex[256];
		};
		struct format6 {
			UInt16	format;
			UInt16	length;
			UInt16	language;
			UInt16	firstCode;
			UInt16	entryCount;
			UInt16	glyphIndex[1];
		};
		
		struct cmapHeader*	h = (struct cmapHeader*)buffer;
		struct subtableHeader*	sh = (struct subtableHeader*)(buffer + sizeof(struct cmapHeader));
		int	subtable = 0;
		cmap = new std::vector<UInt16>;
		cmap->reserve(256);
		while (subtable < SWAP(h->numSubtables)) {
			if ((SWAP(sh->platform) == 1) && (SWAP(sh->encoding) == 0)) {
				struct format0*	f0 = (struct format0*)(buffer + SWAP(sh->offset));
				if (SWAP(f0->format) == 0) {
					for (int ch = 0; ch < 256; ++ch) {
						cmap->push_back(SWAP(f0->glyphIndex[ch]));
					}
				}
				else if (SWAP(f0->format) == 6) {
					struct format6*	f6 = (struct format6*)f0;
					for (int ch = 0; ch < 256; ++ch) {
						if ((ch < SWAP(f6->firstCode)) || (ch >= SWAP(f6->firstCode) + SWAP(f6->entryCount)))
							cmap->push_back(0);
						else
							cmap->push_back(SWAP(f6->glyphIndex[ch - SWAP(f6->firstCode)]));
					}
				}
				else {
					// unsupported cmap subtable format
					fprintf(stderr, "\n*** unsupported 'cmap' subtable format (%d)\n", SWAP(f0->format));
				}
				break;
			}
			++subtable;
			++sh;
		}
		
		delete[] buffer;
	}
	
	return cmap;
}

static const fontMapRec*
getFontRec(const std::string& name)
{
	if (!sMapFileLoaded)
		doPdfMapFile("psfonts.map");

	std::map<std::string,fontMapRec>::iterator	i = psFontsMap.find(name);
	if (i == psFontsMap.end())
		return NULL;

	fontMapRec&	fr = i->second;
	if (fr.loaded)
		return &fr;

	ATSFontRef	fontRef = 0;
	// if a filename is known, try to find and activate it

	if (fr.pfbName.length() > 0)
		fontRef = activatePFB(fr.pfbName.c_str());

	// if no downloadable file, see if it's installed in the OS
	if (fontRef == 0)
	    fontRef = ATSFontFindFromPostScriptName(CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, fr.psName.c_str(),
    	                                        CFStringGetSystemEncoding(), kCFAllocatorNull), kATSOptionFlagsDefault);

	if (fontRef == 0)
		fprintf(stderr, "\n*** font %s (%s: file '%s') not found\n", name.c_str(), fr.psName.c_str(), fr.pfbName.c_str());

	if (fontRef != 0) {
		// if re-encoding was called for, load the encoding vector and build a new cmap
		if (fr.encName.length() > 0) {
			const encoding* enc = getEncoding(fr.encName);
			if (enc != 0) {
				ByteCount	size;
				OSStatus	status = ATSFontGetTable(fontRef, 'post', 0, 0, 0, &size);
				if (status == noErr) {
					UInt8*	buffer = new UInt8[size];
					ATSFontGetTable(fontRef, 'post', 0, size, buffer, &size);
					postTable*	p = (postTable*)&buffer[0];
					std::map<std::string,UInt16>	name2gid;
					UInt16	g = 0;
					switch (SWAP(p->format)) {
						case 0x00010000:
							{
								char*	cp;
								while ((cp = appleGlyphNames[g]) != 0) {
									name2gid[cp] = g;
									++g;
								}
							}
							break;
						
						case 0x00020000:
							{
								UInt16*	n = (UInt16*)(p + 1);
								UInt16	numGlyphs = SWAP(*n++);
								UInt8*	ps = (UInt8*)(n + numGlyphs);
								std::vector<std::string>	newNames;
								while (ps < buffer + size) {
									newNames.push_back(std::string((char*)ps + 1, *ps));
									ps += *ps + 1;
								}
								for (g = 0; g < numGlyphs; ++g) {
									if (SWAP(*n) < 258)
										name2gid[appleGlyphNames[SWAP(*n)]] = g;
									else
										name2gid[newNames[SWAP(*n) - 258]] = g;
									++n;
								}
							}
							break;
						
						case 0x00028000:
							fprintf(stderr, "\n*** format 2.5 'post' table not supported\n");
							break;
						
						case 0x00030000:
							// TODO: see if it's a CFF OpenType font, and if so, get the glyph names from the CFF data
							fprintf(stderr, "\n*** format 3 'post' table; cannot reencode font %s\n", name.c_str());
							break;
						
						case 0x00040000:
							fprintf(stderr, "\n*** format 4 'post' table not supported\n");
							break;
						
						default:
							fprintf(stderr, "\n*** unknown 'post' table format %08x\n");
							break;
					}
					if (fr.cmap != NULL)
						delete fr.cmap;
					fr.cmap = new std::vector<UInt16>;
					for (encoding::const_iterator i = enc->begin(); i != enc->end(); ++i) {
						std::map<std::string,UInt16>::const_iterator	g = name2gid.find(*i);
						if (g == name2gid.end())
							fr.cmap->push_back(0);
						else
							fr.cmap->push_back(g->second);
					}
				}
				else {
					fprintf(stderr, "\n*** no 'post' table found, unable to re-encode font %s\n", name.c_str());
				}
			}
		}
		else {
			// read the MacOS/Roman cmap, if available
			std::vector<UInt16>*	cmap = readMacRomanCmap(fontRef);
			if (fr.cmap != NULL)
				delete fr.cmap;
			fr.cmap = cmap;
		}
	}
	
	if (fontRef == 0) {
		fprintf(stderr, "\n*** font %s (%s) not found, will substitute Helvetica glyphs\n", name.c_str(), fr.pfbName.c_str());
		fontRef = ATSFontFindFromPostScriptName(CFSTR("Helvetica"), kATSOptionFlagsDefault);
		if (fr.cmap != NULL)
			delete fr.cmap;
		fr.cmap = readMacRomanCmap(fontRef);
	}

	fr.cgFont = getCGFontForATSFont(fontRef);
	fr.loaded = true;
	
	return &fr;
}

static void
doFontDef(FILE* xdv, int k)
{
    OSStatus	status;

    UInt32	f = readUnsigned(xdv, k);	// font number we're defining
    UInt32	c = readUnsigned(xdv, 4);	// TFM checksum
    Fixed	s = readUnsigned(xdv, 4);	// at size
    Fixed	d = readUnsigned(xdv, 4);	// design size
    
    UInt16	alen = readUnsigned(xdv, 1);	// area length
    UInt16	nlen = readUnsigned(xdv, 1);	// name length

    UInt8*	name = new UInt8[alen + nlen + 2];
    if (alen > 0) {
        fread(name, 1, alen, xdv);
        name[alen] = '/';
        fread(name + alen + 1, 1, nlen, xdv);
        nlen += alen + 1;
    }
    else
        fread(name, 1, nlen, xdv);
    name[nlen] = 0;

    texFont	font;
    loadMetrics(font, name, d, s);

	if (alen > 0)
		name = name + alen + 1;	// point past the area to get the name by itself for the remaining operations

    ATSFontRef	fontRef;

	// look for a map entry for this font name
	std::string	nameStr((char*)name);
	const fontMapRec*	fr = getFontRec(nameStr);
	if (fr != NULL) {
		font.cgFont = fr->cgFont;
		if (fr->cmap != NULL && fr->cmap->size() > 0) {
			if (font.charMap != NULL)
				delete font.charMap;
			font.charMap = fr->cmap;	// replacing map that was synthesized from the tfm coverage
		}
	}
	else {
		/* try to find the font without the benefit of psfonts.map...
			first try the name as a pfb or otf filename
			and then as the PS name of an installed font
		*/
		
		// ****** FIXME ****** this needs re-working to read the 'cmap' properly
		
		fontRef = ATSFontFindFromPostScriptName(CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, (char*)name,
												CFStringGetSystemEncoding(), kCFAllocatorNull), kATSOptionFlagsDefault);
		UInt8*	ucname = 0;
		if (fontRef == 0) {
			ucname = uppercasify(name);
			fontRef = ATSFontFindFromPostScriptName(CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, (char*)ucname,
												CFStringGetSystemEncoding(), kCFAllocatorNull), kATSOptionFlagsDefault);
		}
		
		if (ucname != 0)
			free(ucname);

		if (fontRef == 0) {
			fprintf(stderr, "\n*** font %s not found in psfonts.map or host system; will substitute Helvetica glyphs\n", (char*)name);
			fontRef = ATSFontFindFromPostScriptName(CFSTR("Helvetica"), kATSOptionFlagsDefault);
			if (font.charMap != NULL)
				delete font.charMap;
			font.charMap = readMacRomanCmap(fontRef);
		}
		
		font.cgFont = getCGFontForATSFont(fontRef);

		delete[] name;
	}

    font.size = s;
    gTeXFonts.insert(std::pair<const UInt32,texFont>(f, font));
}

static void
variationWarning()
{
	static bool	alreadyWarned = false;
	if (alreadyWarned)
		return;
	fprintf(stderr, "\n"
					"## xdv2pdf: AAT variation axes/multiple master fonts not supported on this system\n"
					"## (at least Mac OS X 10.4 is required)\n");
	alreadyWarned = true;
}

static void
doNativeFontDef(FILE* xdv)
{
    UInt32	f = readUnsigned(xdv, 4);	// font ID
    Fixed	s = X2Fix(Fix2X(readUnsigned(xdv, 4)) * 72 / 72.27);	// size, converted to "big" points
    if (sMag != 1000)
    	s = X2Fix(gMagScale * Fix2X(s));
	UInt16	flags = readUnsigned(xdv, 2);

	int	psLen = readUnsigned(xdv, 1);	// PSName length
	int skipLen = readUnsigned(xdv, 1);	// family name length
	skipLen += readUnsigned(xdv, 1);	// style name length

	char*	name = new char[psLen+1];
	fread(name, 1, psLen, xdv);
	name[psLen] = 0;
	
	// don't use fseek, doesn't work when reading from a pipe!
	while (skipLen-- > 0)
		(void)readUnsigned(xdv, 1);

	UInt32	rgba = 0x000000FF;
	if (flags & XDV_FLAG_COLORED)
		rgba = readUnsigned(xdv, 4);

	ATSFontRef	fontRef = 0;
	ATSUFontID	fontID = kATSUInvalidFontID;

	if (name[0] == '[') { // uninstalled font specified by filename: not supported
		fprintf(stderr, "\n"
					"## xdv2pdf: use of uninstalled fonts (specified by filename) such as\n"
					"##   %s\n"
					"## is not supported; try using the xdvipdfmx driver instead.\n", name);
	}
	else {
		CFStringRef	psNameStr = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingMacRoman);
		fontRef = ATSFontFindFromPostScriptName(psNameStr, kATSOptionFlagsDefault);
		CFRelease(psNameStr);

		fontID = FMGetFontFromATSFontRef(fontRef);
	}
	
	if (fontID == kATSUInvalidFontID)
		rgba = 0xFF0000FF;
	
	CGFontRef	cgFont = getCGFontForATSFont(fontRef);

	OSStatus	status = noErr;

	if (flags & XDV_FLAG_VARIATIONS) {
		int n = readUnsigned(xdv, 2); // number of variations
		if (n > 0) {
			bool	applyVars = true;
			CFStringRef*	axes = new CFStringRef[n];
			for (int i = 0; i < n; ++i) {
				UInt32	axis = readUnsigned(xdv, 4);
				FontNameCode	nameCode;
				status = ATSUGetFontVariationNameCode(fontID, axis, &nameCode);
				ByteCount	len;
				status = ATSUFindFontName(fontID, nameCode, kFontMacintoshPlatform, kFontRomanScript,
											(FontLanguageCode)kFontNoLanguage, 0, NULL, &len, NULL);
				if (status == noErr) {
					char*	buf = new char[len + 1];
					status = ATSUFindFontName(fontID, nameCode, kFontMacintoshPlatform, kFontRomanScript,
												(FontLanguageCode)kFontNoLanguage, len, buf, &len, NULL);
					buf[len] = 0;
					axes[i] = CFStringCreateWithCString(kCFAllocatorDefault, buf, kCFStringEncodingMacRoman);
					delete[] buf;
				}
				else
					applyVars = false;
			}

			CFNumberRef*	values = new CFNumberRef[n];
			for (int i = 0; i < n; ++i) {
				SInt32	v = readSigned(xdv, 4);
				Float32	f = Fix2X(v);
				values[i] = CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat32Type, &f);
			}
			
			if (&CGFontCreateCopyWithVariations != NULL) {
				CFDictionaryRef	variations = CFDictionaryCreate(kCFAllocatorDefault,
					(const void**)axes, (const void**)values, n,
					&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	
				cgFont = CGFontCreateCopyWithVariations(cgFont, variations);
				CFRelease(variations);
			}
			else
				variationWarning();

			for (int i = 0; i < n; ++i) {
				CFRelease(axes[i]);
				CFRelease(values[i]);
			}
			delete[] values;
			delete[] axes;
		}
	}
	
	nativeFont	fontRec;
	fontRec.cgFont = cgFont;
	fontRec.atsFont = fontRef;
	fontRec.size = s;
	fontRec.isColored = (flags & XDV_FLAG_COLORED) != 0 || fontID == kATSUInvalidFontID;
	fontRec.isVertical = (flags & XDV_FLAG_VERTICAL) != 0;
	if (fontRec.isColored) {
		float	values[4];
		values[0] =  (rgba >> 24)         / 255.0;
		values[1] = ((rgba >> 16) & 0xFF) / 255.0;
		values[2] = ((rgba >>  8) & 0xFF) / 255.0;
		values[3] = ( rgba        & 0xFF) / 255.0;
		fontRec.color = CGColorCreate(gRGBColorSpace, values);
	}
	else
		fontRec.color = NULL;

	if (fontRec.isVertical) {
		ATSUStyle	style;
		status = ATSUCreateStyle(&style);
		ATSUVerticalCharacterType	vert = kATSUStronglyHorizontal;
		ATSUAttributeTag			tags[] = { kATSUFontTag, kATSUSizeTag, kATSUVerticalCharacterTag };
		ByteCount					sizes[] = { sizeof(fontID), sizeof(s), sizeof(vert) };
		ATSUAttributeValuePtr		values[] = { &fontID, &s, &vert };
		ItemCount	numTags = sizeof(tags) / sizeof(ATSUAttributeTag);
		status = ATSUSetAttributes(style, numTags, tags, sizes, values);
		fontRec.atsuStyleH = style;
	
		status = ATSUCreateStyle(&style);
		vert = kATSUStronglyVertical;
		status = ATSUSetAttributes(style, numTags, tags, sizes, values);
		fontRec.atsuStyleV = style;

		fontRec.matrix = kVerticalMatrix;
	}

	if (flags & XDV_FLAG_EXTEND) {
		Fixed	x = readUnsigned(xdv, 4);
		if (fontRec.isVertical)
			fontRec.matrix.c *= Fix2X(x);
		else
			fontRec.matrix.a *= Fix2X(x);
	}
	if (flags & XDV_FLAG_SLANT) {
		Fixed	s = readSigned(xdv, 4);
		if (fontRec.isVertical)
			fontRec.matrix.d = Fix2X(s);
		else
			fontRec.matrix.c = Fix2X(s);
	}
	if (flags & XDV_FLAG_EMBOLDEN) {
		Fixed	s = readSigned(xdv, 4);
		fontRec.embolden = Fix2X(s);
	}
	else
		fontRec.embolden = 0;

	sNativeFonts.insert(std::pair<const UInt32,nativeFont>(f, fontRec));
}

static void
processPage(FILE* xdv)
{
	/* enter here having just read BOP from xdv file */
	++gPageIndex;

	OSStatus		status;
	int				i;

	std::list<dviVars>	stack;
	gDviLevel = 0;

    int	j = 0;
    for (i = 0; i < 10; ++i) {	// read counts
		gCounts[i] = readSigned(xdv, 4);
        if (gCounts[i] != 0)
            j = i;
    }
	if (sVerbose) {
		fprintf(stderr, "[");
		for (i = 0; i < j; ++i)
			fprintf(stderr, "%d.", gCounts[i]);
		fprintf(stderr, "%d", gCounts[j]);
	}

    (void)readUnsigned(xdv, 4);	// skip prev-BOP pointer
	
	cur_cgFont = kUndefinedFont;
	dvi.h = dvi.v = 0;
	dvi.w = dvi.x = dvi.y = dvi.z = 0;
	f = 0;
	
	unsigned int	cmd = DVI_BOP;
	while (cmd != DVI_EOP) {
		UInt32	u;
		SInt32	s;
		SInt32	ht, wd;
		int		k = 1;

		cmd = readUnsigned(xdv, 1);
		switch (cmd) {
			default:
				if (cmd < DVI_SET1)
					setChar(cmd, true);
				else if (cmd >= DVI_FNTNUM0 && cmd <= DVI_FNTNUM0 + 63)
					f = cmd - DVI_FNTNUM0;
				else
					goto ABORT_PAGE;
				break;
			
			case DVI_EOP:
				flushGlyphs();
				break;
			
			case DVI_NOP:
				break;
			
			case DVI_FNT4:
				++k;
			case DVI_FNT3:
				++k;
			case DVI_FNT2:
				++k;
			case DVI_FNT1:
				f = readUnsigned(xdv, k);	// that's it: just set |f|
				break;

			case DVI_XXX4:
				++k;
			case DVI_XXX3:
				++k;
			case DVI_XXX2:
				++k;
			case DVI_XXX1:
				flushGlyphs();
                u = readUnsigned(xdv, k);
                if (u > 0) {
                    char* special = new char[u+1];
                    fread(special, 1, u, xdv);
                    special[u] = '\0';
                    
                    doSpecial(special);
                    
                    delete[] special;
                }
				break;
			
			case DVI_SETRULE:
			case DVI_PUTRULE:
				ensurePageStarted();
				ht = readSigned(xdv, 4);
				wd = readSigned(xdv, 4);
				if (ht > 0 && wd > 0) {
					CGRect	r = CGRectMake(kDvi2Scr * dvi.h, kDvi2Scr * (gPageHt - dvi.v), kDvi2Scr * wd, kDvi2Scr * ht);
					setColor(gRuleColor);
                    CGContextFillRect(gCtx, r);
                    if (gTrackingBoxes) {
						box	b = {
							dvi.h,
							dvi.v,
							dvi.h + wd,
							dvi.v - ht };
						mergeBox(b);
                    }
				}
				if (cmd == DVI_SETRULE)
					dvi.h += wd;
				break;
			
			case DVI_SET4:
				++k;
			case DVI_SET3:
				++k;
			case DVI_SET2:
				++k;
			case DVI_SET1:
				u = readUnsigned(xdv, k);
				setChar(u, true);
				break;
			
			case DVI_PUT4:
				++k;
			case DVI_PUT3:
				++k;
			case DVI_PUT2:
				++k;
			case DVI_PUT1:
				u = readUnsigned(xdv, k);
				setChar(u, false);
				break;
			
			case DVI_PUSH:
				stack.push_back(dvi);
				++gDviLevel;
				break;

			case DVI_POP:
				if (gDviLevel == gTagLevel)
					flushAnnotBox();
				--gDviLevel;
				dvi = stack.back();
				stack.pop_back();
				break;
			
			case DVI_RIGHT4:
				++k;
			case DVI_RIGHT3:
				++k;
			case DVI_RIGHT2:
				++k;
			case DVI_RIGHT1:
				s = readSigned(xdv, k);
				dvi.h += s;
				break;
			
			case DVI_DOWN4:
				++k;
			case DVI_DOWN3:
				++k;
			case DVI_DOWN2:
				++k;
			case DVI_DOWN1:
				s = readSigned(xdv, k);
				dvi.v += s;
				break;
			
			case DVI_W4:
				++k;
			case DVI_W3:
				++k;
			case DVI_W2:
				++k;
			case DVI_W1:
				s = readSigned(xdv, k);
				dvi.w = s;
			case DVI_W0:
				dvi.h += dvi.w;
				break;
				
			case DVI_X4:
				++k;
			case DVI_X3:
				++k;
			case DVI_X2:
				++k;
			case DVI_X1:
				s = readSigned(xdv, k);
				dvi.x = s;
			case DVI_X0:
				dvi.h += dvi.x;
				break;
				
			case DVI_Y4:
				++k;
			case DVI_Y3:
				++k;
			case DVI_Y2:
				++k;
			case DVI_Y1:
				s = readSigned(xdv, k);
				dvi.y = s;
			case DVI_Y0:
				dvi.v += dvi.y;
				break;
				
			case DVI_Z4:
				++k;
			case DVI_Z3:
				++k;
			case DVI_Z2:
				++k;
			case DVI_Z1:
				s = readSigned(xdv, k);
				dvi.z = s;
			case DVI_Z0:
				dvi.v += dvi.z;
				break;
			
			case XDV_GLYPH_ARRAY:
				ensurePageStarted();
				doGlyphArray(xdv, true);
				break;
			
			case XDV_GLYPH_STRING:
				ensurePageStarted();
				doGlyphArray(xdv, false);
				break;
			
            case XDV_PIC_FILE:
				ensurePageStarted();
				k = readUnsigned(xdv, 1);	/* 0 for QT picfile, or PDF box code (1..5) */
                doPicFile(xdv, k);
                break;
                            
			case DVI_FNTDEF4:
				++k;
			case DVI_FNTDEF3:
				++k;
			case DVI_FNTDEF2:
				++k;
			case DVI_FNTDEF1:
				doFontDef(xdv, k);
				break;
				
			case XDV_NATIVE_FONT_DEF:
                doNativeFontDef(xdv);
				break;
		}
	}

	ensurePageStarted();	// needed for completely blank pages!

	if (sVerbose) {
		fprintf(stderr, "]%s", (gPageIndex % 10) == 0 ? "\n" : "");
	}

ABORT_PAGE:
    
	if (gPageStarted) {
		CGContextEndPage(gCtx);
		CGContextRestoreGState(gCtx);
	}
	
	gPageStarted = false;
}

#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4
/* From Apple Technical Q&A 1396, Listing 2 */
static CGColorSpaceRef
CreateICCColorSpaceFromPathToProfile(const char* iccProfilePath)
{
	CMProfileRef      iccProfile = (CMProfileRef)0;
	CGColorSpaceRef   iccColorSpace = NULL;
	CMProfileLocation loc;

	// Specify that the location of the profile will be a POSIX path to the profile.
	loc.locType = cmPathBasedProfile;

	// Make sure the path is not larger then the buffer
	if (strlen(iccProfilePath) > sizeof(loc.u.pathLoc.path))
		return NULL;

	// Copy the path the profile into the CMProfileLocation structure
	strcpy(loc.u.pathLoc.path, iccProfilePath);

	// Open the profile
	if (CMOpenProfile(&iccProfile, &loc) != noErr) {
		iccProfile = (CMProfileRef)0;
		return NULL;
	}

	// Create the ColorSpace with the open profile.
	iccColorSpace = CGColorSpaceCreateWithPlatformColorSpace(iccProfile);

	// Close the profile now that we have what we need from it.
	CMCloseProfile(iccProfile);

	return iccColorSpace;
}

static CGColorSpaceRef
CreateColorSpaceFromSystemICCProfileName(CFStringRef profileName)
{
	FSRef pathToProfilesFolder;
	FSRef pathToProfile;

	// Find the Systems Color Sync Profiles folder
	if (FSFindFolder(kOnSystemDisk, kColorSyncProfilesFolderType,
					kDontCreateFolder, &pathToProfilesFolder) == noErr) {

		// Make a UniChar string of the profile name
		UniChar uniBuffer[sizeof(CMPathLocation)];
		CFStringGetCharacters(profileName, CFRangeMake(0, CFStringGetLength(profileName)), uniBuffer);

		// Create a FSRef to the profile in the Systems Color Sync Profile folder
		if (FSMakeFSRefUnicode(&pathToProfilesFolder, CFStringGetLength(profileName), uniBuffer,
								kUnicodeUTF8Format, &pathToProfile) == noErr) {
			char path[sizeof(CMPathLocation)];

			// Write the posix path to the profile into our path buffer from the FSRef
			if (FSRefMakePath(&pathToProfile, (UInt8*)path, sizeof(CMPathLocation)) == noErr)
				return CreateICCColorSpaceFromPathToProfile(path);
		}
	}

	return NULL;
}
/* end of Apple Q&A code */
#endif

static void
processAllPages(FILE* xdv)
{
	// initialize some global variables that we use as CG "constants"
#if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_4
	if (&kCGColorSpaceGenericRGB == NULL) {
		gRGBColorSpace = CreateColorSpaceFromSystemICCProfileName(CFSTR("Generic RGB Profile.icc"));
		gCMYKColorSpace = CreateColorSpaceFromSystemICCProfileName(CFSTR("Generic CMYK Profile.icc"));
		gGrayColorSpace = CreateColorSpaceFromSystemICCProfileName(CFSTR("Generic Gray Profile.icc"));
	}
	else
#endif
	{
		gRGBColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
		gCMYKColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericCMYK);
		gGrayColorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericGray);
	}

	if (gRGBColorSpace == NULL || gCMYKColorSpace == NULL || gGrayColorSpace == NULL) {
		fprintf(stderr, "\n*** unable to initialize color spaces.... something's badly broken\n");
		exit(1);
	}
	
	const float black[2] = { 0.0, 1.0 };
	kBlackColor = CGColorCreate(gGrayColorSpace, black);

	gPageIndex = 0;
	unsigned int	cmd;
	while ((cmd = readUnsigned(xdv, 1)) != DVI_POST) {
		switch (cmd) {
			case DVI_BOP:
				processPage(xdv);
				break;
				
			case DVI_NOP:
				break;
			
			default:
				fprintf(stderr, "\n*** unexpected DVI command: %d\n", cmd);
                exit(1);
		}
	}
	if (sVerbose && (gPageIndex % 10) != 0)
		fprintf(stderr, "\n");
}

const char* progName;
static void
usage()
{
    fprintf(stderr, "usage: %s [-m mag] [-p papersize[:landscape]] [-v] [-o pdfFile] xdvFile\n", progName);
	fprintf(stderr, "    papersize values: ");
	paperSizeRec*	ps = &gPaperSizes[0];
	while (ps->name != 0) {
		fprintf(stderr, "%s/", ps->name);
		++ps;
	}
	fprintf(stderr, "wd,ht [in 'big' points or with explicit units]\n");
}

static void
version()
{
    fprintf(stderr, "%s version %s\n", PACKAGE, VERSION);
    fprintf(stderr, "Written by Jonathan Kew\n"
					"Copyright (c) 2008 SIL International\n"
    				"This program is part of the XeTeX typesetting system;\n"
    				"see the XeTeX license for details.\n\n");
}

int
xdv2pdf(int argc, char** argv)
{
	OSStatus			status;
    
    progName = argv[0];
    
	gMagScale = 1.0;
	gRuleColor = kBlackColor;
	gTextColor = gRuleColor;

	gTagLevel = -1;
	
	gPaperWd = gPaperHt = 0.0;
	
    int	ch;
    while ((ch = getopt(argc, argv, "o:p:m:d:hv" /*r:*/)) != -1) {
        switch (ch) {
            case 'o':
                {
                    CFStringRef	outFileName = CFStringCreateWithCString(kCFAllocatorDefault, optarg, kCFStringEncodingUTF8);
                    gSaveURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, outFileName, kCFURLPOSIXPathStyle, false);
                    CFRelease(outFileName);
                }
                break;
            
            case 'p':
				if (!getPaperSize(optarg, gPaperWd, gPaperHt)) {
					fprintf(stderr, "*** unknown paper name: %s\n", optarg);
					exit(1);
				}
                break;
                
            case 'm':
            	sMag = atoi(optarg);
            	break;
            
            case 'd':
            	kpathsea_debug |= atoi(optarg);
            	break;
            
            case 'v':
            	version();
            	sVerbose = true;
            	break;
            
            case 'h':
                usage();
                exit(0);
        }
    }

	if ((gPaperWd == 0.0) || (gPaperHt == 0.0)) {
		// get default paper size from printing system
		PMRect				paperRect = { 0, 0, 792, 612 };
		PMPrintSession		printSession;
		PMPrintSettings		printSettings;
		PMPageFormat		pageFormat;
		status = PMCreateSession(&printSession);
		if (status == noErr) {
			status = PMCreatePrintSettings(&printSettings);
			if (status == noErr) {
				status = PMSessionDefaultPrintSettings(printSession, printSettings);
				status = PMCreatePageFormat(&pageFormat);
				if (status == noErr) {
					status = PMSessionDefaultPageFormat(printSession, pageFormat);
					PMGetUnadjustedPaperRect(pageFormat, &paperRect);
					status = PMRelease(pageFormat);
				}
				status = PMRelease(printSettings);
			}
			status = PMRelease(printSession);
		}
		gPaperWd = paperRect.right - paperRect.left;
		gPaperHt = paperRect.bottom - paperRect.top;
	}

    // set the media box for PDF generation
    gMediaBox = CGRectMake(0, 0, gPaperWd, gPaperHt);
    
    argc -= optind;
    argv += optind;
    if (argc == 1 && gSaveURL == 0) {
        CFStringRef	inFileName = CFStringCreateWithCString(kCFAllocatorDefault, argv[0], CFStringGetSystemEncoding());
        CFURLRef	inURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inFileName, kCFURLPOSIXPathStyle, false);
        CFRelease(inFileName);
        CFURLRef	tmpURL = CFURLCreateCopyDeletingPathExtension(kCFAllocatorDefault, inURL);
        CFRelease(inURL);
        gSaveURL = CFURLCreateCopyAppendingPathExtension(kCFAllocatorDefault, tmpURL, CFSTR("pdf"));
        CFRelease(tmpURL);
    }

    if (argc > 1 || gSaveURL == 0) {
        usage();
        exit(1);
    }
    
	FILE*	xdv;
    if (argc == 1)
        xdv = fopen(argv[0], "r");
    else
        xdv = stdin;

	if (xdv != NULL) {
		// read the preamble
		unsigned	b = readUnsigned(xdv, 1);
		if (b != DVI_PRE) { fprintf(stderr, "*** bad XDV file: DVI_PRE not found, b=%d\n", b); exit(1); }
		b = readUnsigned(xdv, 1);
		if (b != XDV_ID) { fprintf(stderr, "*** bad XDV file: version=%d, expected %d\n", b, XDV_ID); exit(1); }
		(void)readUnsigned(xdv, 4);	// num
		(void)readUnsigned(xdv, 4);	// den
		if (sMag == 0)
			sMag = readUnsigned(xdv, 4);	// sMag
		else
			(void)readUnsigned(xdv, 4);

		unsigned numBytes = readUnsigned(xdv, 1);	// length of output comment
        UInt8* bytes = new UInt8[numBytes];
        fread(bytes, 1, numBytes, xdv);

        CFStringRef	keys[2] =   { CFSTR("Creator") , CFSTR("Title") /*, CFSTR("Author")*/ };
        CFStringRef values[2] = { CFSTR("xdv2pdf") , 0 /*, 0*/ };
        values[1] = CFStringCreateWithBytes(kCFAllocatorDefault, bytes, numBytes, CFStringGetSystemEncoding(), false);
        delete[] bytes;
//        values[2] = CFStringCreateWithCString(kCFAllocatorDefault, getlogin(), CFStringGetSystemEncoding());
        CFDictionaryRef	auxInfo = CFDictionaryCreate(kCFAllocatorDefault, (const void**)keys, (const void**)values, sizeof(keys) / sizeof(keys[0]),
                &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        CFRelease(values[1]);
//        CFRelease(values[2]);

        // create PDF graphics context
        gCtx = CGPDFContextCreateWithURL(gSaveURL, &gMediaBox, auxInfo);

		// handle magnification
		if (sMag != 1000) {
			gMagScale = sMag / 1000.0;
			kScr2Dvi /= gMagScale;
			kDvi2Scr *= gMagScale;
			CGContextScaleCTM(gCtx, gMagScale, gMagScale);
		}

		// draw all the pages
        processAllPages(xdv);

        CGContextRelease(gCtx);

		while (getc(xdv) != EOF)
			;
		fclose(xdv);
	}
	
	if (gPdfMarkFile != NULL) {
		fclose(gPdfMarkFile);
		char	pdfPath[_POSIX_PATH_MAX+1];
		Boolean	gotPath = CFURLGetFileSystemRepresentation(gSaveURL, true, (UInt8*)pdfPath, _POSIX_PATH_MAX);
		CFRelease(gSaveURL);
#if 0
		if (gotPath) {
			char*	mergeMarks = "xdv2pdf_mergemarks";
			execlp(mergeMarks, mergeMarks, pdfPath, gPdfMarkPath, 0);	// should not return!
			status = errno;
		}
		fprintf(stderr, "*** failed to run xdv2pdf_mergemarks: status = %d\n", status);
#else
		if (gotPath) {
			char*	mergeMarks = "`kpsewhich -progname=xetex -format=texmfscripts xdv2pdf_mergemarks"
								 " || echo echo '# ERROR: need xdv2pdf_mergemarks script to process:'`";
			char	cmd[_POSIX_PATH_MAX*2 + 150]; // enough for the mergeMarks command above, plus some
			sprintf(cmd, "%s \"%s\" \"%s\"", mergeMarks, pdfPath, gPdfMarkPath);
			status = system(cmd);
		}
		else
			status = fnfErr;
		if (status != 0)
			fprintf(stderr, "*** failed to run xdv2pdf_mergemarks: status = %d\n", status);
#endif
	}
	else
		CFRelease(gSaveURL);

	return status;
}
