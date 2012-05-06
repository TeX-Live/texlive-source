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

#ifndef __XETEXEXT_H
#define __XETEXEXT_H

#include <w2c/c-auto.h>  /* needed for SIZEOF_LONG and NO_DUMP_SHARE */
/***** copied from TeX/texk/web2c/config.h -- difficult to include in C++ source files ******/
#ifndef INTEGER_TYPE

#if SIZEOF_LONG > 4 && !defined (NO_DUMP_SHARE)
/* If we have 64-bit longs and want to share format files (with 32-bit
   machines), use `int'.  */
#define INTEGER_IS_INT
#endif

#ifdef INTEGER_IS_INT
#define INTEGER_TYPE int
#define INTEGER_MAX INT_MAX
#define INTEGER_MIN INT_MIN
#else
#define INTEGER_TYPE long
#define INTEGER_MAX LONG_MAX
#define INTEGER_MIN LONG_MIN
#endif /* not INTEGER_IS_INT */

typedef INTEGER_TYPE integer;
#endif /* not INTEGER_TYPE */
/***** end of config.h stuff *****/

#ifndef XETEX_UNICODE_FILE_DEFINED
typedef struct UFILE* unicodefile;
#endif

#define AAT_FONT_FLAG	0xFFFFU
#define	OTGR_FONT_FLAG	0xFFFEU

#define FONT_FLAGS_COLORED	0x01
#define FONT_FLAGS_VERTICAL	0x02

#define kXeTeXEmboldenTag	0x10000 /* user-defined ATSUAttributeTag to carry 'embolden' value */


/* some typedefs that XeTeX uses - on Mac OS, we get these from Apple headers,
   but otherwise we'll need these substitute definitions */

#ifdef XETEX_MAC
#include <Carbon/Carbon.h>
#else
#ifndef __TECkit_Common_H__
typedef unsigned char	UInt8;
typedef unsigned short	UInt16;
typedef unsigned int	UInt32;
typedef UInt16			UniChar;
#endif

typedef signed char		SInt8;
typedef short			SInt16;
typedef int				SInt32;

typedef SInt32			OSStatus;
typedef SInt32			Fixed;
typedef struct {
	Fixed	x;
	Fixed	y;
} FixedPoint;
#endif


/* these are also in xetex-new.ch and must correspond! */

#define pdfbox_crop	1
#define pdfbox_media	2
#define pdfbox_bleed	3
#define pdfbox_trim	4
#define pdfbox_art	5


#define XeTeX_count_glyphs	1

#define XeTeX_count_variations	2
#define XeTeX_variation	3
#define XeTeX_find_variation_by_name	4
#define XeTeX_variation_min	5
#define XeTeX_variation_max	6
#define XeTeX_variation_default	7

#define XeTeX_count_features	8
#define XeTeX_feature_code	9
#define XeTeX_find_feature_by_name	10
#define XeTeX_is_exclusive_feature	11
#define XeTeX_count_selectors	12
#define XeTeX_selector_code	13
#define XeTeX_find_selector_by_name	14
#define XeTeX_is_default_selector	15

#define XeTeX_OT_count_scripts	16
#define XeTeX_OT_count_languages	17
#define XeTeX_OT_count_features	18
#define XeTeX_OT_script_code	19
#define XeTeX_OT_language_code	20
#define XeTeX_OT_feature_code	21

#define XeTeX_map_char_to_glyph_code	22

#define XeTeX_variation_name	7	/* must match xetex.web */
#define XeTeX_feature_name	8
#define XeTeX_selector_name	9


/* definitions used to access info in a native_word_node; must correspond with defines in xetex-new.ch */
#define width_offset		1
#define depth_offset		2
#define height_offset		3
#define native_info_offset	4
#define native_glyph_info_offset	5

#define node_width(node)			node[width_offset].cint
#define node_depth(node)			node[depth_offset].cint
#define node_height(node)			node[height_offset].cint
#define native_length(node)			node[native_info_offset].qqqq.b2
#define native_font(node)			node[native_info_offset].qqqq.b1
#define native_glyph_count(node)	node[native_info_offset].qqqq.b3
#define native_glyph_info_ptr(node)	node[native_glyph_info_offset].ptr
#define native_glyph_info_size		10	/* info for each glyph is location (FixedPoint) + glyph ID (UInt16) */

#define native_glyph(p)		native_length(p)	/* glyph ID field in a glyph_node */

#define XDV_GLYPH_STRING	254
#define	XDV_GLYPH_ARRAY		253

/* OT-related constants we need */
#define kGSUB	0x47535542
#define kGPOS	0x47504f53

#define kLatin	0x6c61746e
#define kSyriac	0x73797263
#define kArabic	0x61726162
#define kThaana	0x74686161
#define kHebrew	0x68656272


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

#define kPost	0x706f7374
#define kCmap	0x636d6170

typedef struct
{
	float	xMin;
	float	yMin;
	float	xMax;
	float	yMax;
} GlyphBBox;


/* For Unicode encoding form interpretation... */
extern const UInt32 offsetsFromUTF8[6];
extern const UInt8 bytesFromUTF8[256];
extern const UInt8 firstByteMark[7];

extern const int halfShift;
extern const UInt32 halfBase;
extern const UInt32 halfMask;
extern const UInt32 kSurrogateHighStart;
extern const UInt32 kSurrogateHighEnd;
extern const UInt32 kSurrogateLowStart;
extern const UInt32 kSurrogateLowEnd;
extern const UInt32 byteMask;
extern const UInt32 byteMark;

extern const char *papersize;
extern const char *outputdriver;

/* gFreeTypeLibrary is defined in XeTeXFontInst_FT2.cpp,
 * also used in XeTeXFontMgr_FC.cpp and XeTeX_ext.c.  */
#include <ft2build.h>
#include FT_FREETYPE_H
extern FT_Library gFreeTypeLibrary;

#include <stdio.h> /* for FILE */

#include "trans.h"

#include "XeTeXLayoutInterface.h"

#ifdef __cplusplus
extern "C" {
#endif
	void initversionstring(char **versions);

	void setinputfileencoding(unicodefile f, integer mode, integer encodingData);
	void uclose(unicodefile f);
	int input_line_icu(unicodefile f);
	void linebreakstart(integer localeStrNum, const UniChar* text, integer textLength);
	int linebreaknext();
	int getencodingmodeandinfo(integer* info);
	void printutf8str(const unsigned char* str, int len);
	void printchars(const unsigned short* str, int len);
	void* load_mapping_file(const char* s, const char* e, char byteMapping);
	void* findnativefont(unsigned char* name, integer scaled_size);
	void releasefontengine(void* engine, int type_flag);
	int readCommonFeatures(const char* feat, const char* end, float* extend, float* slant, float* embolden, float* letterspace, UInt32* rgbValue);

	/* the metrics params here are really TeX 'scaled' values, but that typedef isn't available every place this is included */
	void otgetfontmetrics(void* engine, integer* ascent, integer* descent, integer* xheight, integer* capheight, integer* slant);
	void getnativecharheightdepth(integer font, integer ch, integer* height, integer* depth);
	void getnativecharsidebearings(integer font, integer ch, integer* lsb, integer* rsb);

	/* single-purpose metrics accessors */
	integer getnativecharwd(integer font, integer ch);
	integer getnativecharht(integer font, integer ch);
	integer getnativechardp(integer font, integer ch);
	integer getnativecharic(integer font, integer ch);

	integer getglyphbounds(integer font, integer edge, integer gid);

	integer otfontget(integer what, void* engine);
	integer otfontget1(integer what, void* engine, integer param);
	integer otfontget2(integer what, void* engine, integer param1, integer param2);
	integer otfontget3(integer what, void* engine, integer param1, integer param2, integer param3);
	int makeXDVGlyphArrayData(void* p);
	int makefontdef(integer f);
	int applymapping(void* cnv, const UniChar* txtPtr, int txtLen);
	void store_justified_native_glyphs(void* node);
	void measure_native_node(void* node, int use_glyph_metrics);
	Fixed get_native_ital_corr(void* node);
	Fixed get_native_glyph_ital_corr(void* node);
	void measure_native_glyph(void* node, int use_glyph_metrics);
	integer mapchartoglyph(integer font, integer ch);
	integer mapglyphtoindex(integer font);
	integer getfontcharrange(integer font, int first);
	void printglyphname(integer font, integer gid);
	UInt16 get_native_glyph_id(void* pNode, unsigned index);

	void grprintfontname(integer what, void* pEngine, integer param1, integer param2);
	integer grfontgetnamed(integer what, void* pEngine);
	integer grfontgetnamed1(integer what, void* pEngine, integer param);

	double read_double(const char** s);
	unsigned int read_rgb_a(const char** cp);

	const char* getGlyphNamePtr(const char* buffer, int tableSize, UInt16 gid, int* len);

	int countpdffilepages();
	int find_pic_file(char** path, realrect* bounds, int pdfBoxType, int page);
	int u_open_in(unicodefile* f, integer filefmt, const char* fopen_mode, integer mode, integer encodingData);
	int open_dvi_output(FILE** fptr);
	int dviclose(FILE* fptr);
	int get_uni_c(UFILE* f);
	int input_line(UFILE* f);
	void makeutf16name();

	void terminatefontmanager();
	int maketexstring(const char* s);

	void checkfortfmfontmapping();
	void* loadtfmfontmapping();
	int applytfmfontmapping(void* mapping, int c);

#ifndef XETEX_MAC
typedef void* ATSUStyle; /* dummy declaration just so the stubs can compile */
#endif

	int atsufontget(int what, ATSUStyle style);
	int atsufontget1(int what, ATSUStyle style, int param);
	int atsufontget2(int what, ATSUStyle style, int param1, int param2);
	int atsufontgetnamed(int what, ATSUStyle style);
	int atsufontgetnamed1(int what, ATSUStyle style, int param);
	void atsuprintfontname(int what, ATSUStyle style, int param1, int param2);
	/* the metrics params here are really TeX 'scaled' (or MacOS 'Fixed') values, but that typedef isn't available every place this is included */
	void atsugetfontmetrics(ATSUStyle style, integer* ascent, integer* descent, integer* xheight, integer* capheight, integer* slant);

#ifdef XETEX_MAC

#if defined(MAC_OS_X_VERSION_10_7) && (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_7)
	#include <ApplicationServices/ApplicationServices.h>
	/* These functions have been removed on 10.7; as a workaround, just do
	 * a cast as the ATSFontRef and ATSUFontID should have the same value... */
	#define FMGetFontFromATSFontRef(x) ((ATSUFontID) x)
	#define FMGetATSFontRefFromFont(x) ((ATSFontRef) x)
#endif

/* functions in XeTeX_mac.c */
	void* loadAATfont(ATSFontRef fontRef, integer scaled_size, const char* cp1);
	void DoAtsuiLayout(void* node, int justify);
	void GetGlyphBBox_AAT(ATSUStyle style, UInt16 gid, GlyphBBox* bbox);
	float GetGlyphWidth_AAT(ATSUStyle style, UInt16 gid);
	void GetGlyphHeightDepth_AAT(ATSUStyle style, UInt16 gid, float* ht, float* dp);
	void GetGlyphSidebearings_AAT(ATSUStyle style, UInt16 gid, float* lsb, float* rsb);
	int MapCharToGlyph_AAT(ATSUStyle style, UInt32 ch);
	int MapGlyphToIndex_AAT(ATSUStyle style, const char* glyphName);
	int GetGlyphIDFromCGFont(ATSFontRef atsFontRef, const char* glyphName);
	float GetGlyphItalCorr_AAT(ATSUStyle style, UInt16 gid);
	char* GetGlyphName_AAT(ATSUStyle style, UInt16 gid, int* len);
	char* GetGlyphNameFromCGFont(ATSFontRef atsFontRef, UInt16 gid, int* len);
	int GetFontCharRange_AAT(ATSUStyle style, int reqFirst);
	ATSUFontVariationAxis find_axis_by_name(ATSUFontID fontID, const char* name, int nameLength);
	ATSUFontFeatureType find_feature_by_name(ATSUFontID fontID, const char* name, int nameLength);
	ATSUFontFeatureSelector find_selector_by_name(ATSUFontID fontID, ATSUFontFeatureType featureType, const char* name, int nameLength);
#endif
#ifdef __cplusplus
};
#endif

/* some Mac OS X functions that we provide ourselves for other platforms */
#ifndef XETEX_MAC
#ifdef __cplusplus
extern "C" {
#endif
	double	Fix2X(Fixed f);
	Fixed	X2Fix(double d);
#ifdef __cplusplus
};
#endif
#endif

// copied from xetex-hz.ch
#define LEFT_SIDE  0    
#define RIGHT_SIDE 1    

#endif /* __XETEX_EXT_H */
