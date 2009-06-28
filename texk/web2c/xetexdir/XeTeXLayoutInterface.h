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

#ifndef XETEX_LAYOUT_INTERFACE_H
#define XETEX_LAYOUT_INTERFACE_H 1

#ifdef XETEX_MAC
#include <Carbon/Carbon.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef struct XeTeXFont_rec* XeTeXFont;
typedef struct XeTeXLayoutEngine_rec* XeTeXLayoutEngine;
#ifdef __cplusplus
};
#endif

#include "XeTeX_ext.h"
#include "XeTeXFontMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char	gPrefEngine;

int getCachedGlyphBBox(UInt16 fontID, UInt16 glyphID, GlyphBBox* bbox);
void cacheGlyphBBox(UInt16 fontID, UInt16 glyphID, const GlyphBBox* bbox);

void terminatefontmanager();

#ifdef XETEX_MAC
XeTeXFont createFont(ATSFontRef atsFont, Fixed pointSize);
#else
// appropriate functions for other platforms
XeTeXFont createFont(PlatformFontRef fontRef, Fixed pointSize);
#endif
XeTeXFont createFontFromFile(const char* filename, int index, Fixed pointSize);

void setFontLayoutDir(XeTeXFont font, int vertical);

PlatformFontRef findFontByName(const char* name, char* var, double size);

char getReqEngine();
const char* getFullName(PlatformFontRef fontRef);

const char* getFontFilename(XeTeXLayoutEngine engine);

void getNames(PlatformFontRef fontRef, const char** psName, const char** famName, const char** styName);
double getDesignSize(XeTeXFont font);

void deleteFont(XeTeXFont font);

void* getFontTablePtr(XeTeXFont font, UInt32 tableTag);

Fixed getSlant(XeTeXFont font);

UInt32 countScripts(XeTeXFont font);
UInt32 getIndScript(XeTeXFont font, UInt32 index);
UInt32 countScriptLanguages(XeTeXFont font, UInt32 script);
UInt32 getIndScriptLanguage(XeTeXFont font, UInt32 script, UInt32 index);
UInt32 countFeatures(XeTeXFont font, UInt32 script, UInt32 language);
UInt32 getIndFeature(XeTeXFont font, UInt32 script, UInt32 language, UInt32 index);
float getGlyphWidth(XeTeXFont font, UInt32 gid);
UInt32 countGlyphs(XeTeXFont font);

XeTeXLayoutEngine createLayoutEngine(PlatformFontRef fontRef, XeTeXFont font, UInt32 scriptTag, UInt32 languageTag,
						UInt32* addFeatures, SInt32* addParams, UInt32* removeFeatures, UInt32 rgbValue,
						float extend, float slant, float embolden);

void deleteLayoutEngine(XeTeXLayoutEngine engine);

XeTeXFont getFont(XeTeXLayoutEngine engine);
PlatformFontRef getFontRef(XeTeXLayoutEngine engine);

float getExtendFactor(XeTeXLayoutEngine engine);
float getSlantFactor(XeTeXLayoutEngine engine);
float getEmboldenFactor(XeTeXLayoutEngine engine);

SInt32 layoutChars(XeTeXLayoutEngine engine, UInt16* chars, SInt32 offset, SInt32 count, SInt32 max,
						char rightToLeft, float x, float y, SInt32* status);

void getGlyphs(XeTeXLayoutEngine engine, UInt32* glyphs, SInt32* status);

void getGlyphPositions(XeTeXLayoutEngine engine, float* positions, SInt32* status);

void getGlyphPosition(XeTeXLayoutEngine engine, SInt32 index, float* x, float* y, SInt32* status);

UInt32 getScriptTag(XeTeXLayoutEngine engine);

UInt32 getLanguageTag(XeTeXLayoutEngine engine);

float getPointSize(XeTeXLayoutEngine engine);

void getAscentAndDescent(XeTeXLayoutEngine engine, float* ascent, float* descent);

UInt32* getAddedFeatures(XeTeXLayoutEngine engine);

UInt32* getRemovedFeatures(XeTeXLayoutEngine engine);

int getDefaultDirection(XeTeXLayoutEngine engine);

UInt32 getRgbValue(XeTeXLayoutEngine engine);

void getGlyphBounds(XeTeXLayoutEngine engine, UInt32 glyphID, GlyphBBox* bbox);

float getGlyphWidthFromEngine(XeTeXLayoutEngine engine, UInt32 glyphID);

void getGlyphHeightDepth(XeTeXLayoutEngine engine, UInt32 glyphID, float* height, float* depth);

void getGlyphSidebearings(XeTeXLayoutEngine engine, UInt32 glyphID, float* lsb, float* rsb);

float getGlyphItalCorr(XeTeXLayoutEngine engine, UInt32 glyphID);

UInt32 mapCharToGlyph(XeTeXLayoutEngine engine, UInt32 charCode);

int	mapGlyphToIndex(XeTeXLayoutEngine engine, const char* glyphName);

int	findGlyphInPostTable(const char* p, int tableSize, const char* glyphName);

const char* getGlyphName(XeTeXFont font, UInt16 gid, int* len);

int getFontCharRange(XeTeXLayoutEngine engine, int reqFirst);

/* graphite interface functions... */
XeTeXLayoutEngine createGraphiteEngine(PlatformFontRef fontRef, XeTeXFont font,
										const char* name,
										UInt32 rgbValue, int rtl, UInt32 languageTag,
										float extend, float slant, float embolden,
										int nFeatures, const int* featureIDs, const int* featureValues);
int makeGraphiteSegment(XeTeXLayoutEngine engine, const UniChar* txtPtr, int txtLen);
void getGraphiteGlyphInfo(XeTeXLayoutEngine engine, int index, UInt16* glyphID, float* x, float* y);
float graphiteSegmentWidth(XeTeXLayoutEngine engine);
void initGraphiteBreaking(XeTeXLayoutEngine engine, const UniChar* txtPtr, int txtLen);
int findNextGraphiteBreak(int iOffset, int iBrkVal);

int usingOpenType(XeTeXLayoutEngine engine);
int usingGraphite(XeTeXLayoutEngine engine);
int isOpenTypeMathFont(XeTeXLayoutEngine engine);

int findGraphiteFeature(XeTeXLayoutEngine engine, const char* s, const char* e, int* f, int* v);

UInt32 countGraphiteFeatures(XeTeXLayoutEngine engine);
UInt32 getGraphiteFeatureCode(XeTeXLayoutEngine engine, UInt32 index);
UInt32 countGraphiteFeatureSettings(XeTeXLayoutEngine engine, UInt32 feature);
UInt32 getGraphiteFeatureSettingCode(XeTeXLayoutEngine engine, UInt32 feature, UInt32 index);
UInt32 getGraphiteFeatureDefaultSetting(XeTeXLayoutEngine engine, UInt32 feature);
void getGraphiteFeatureLabel(XeTeXLayoutEngine engine, UInt32 feature, unsigned short* buf);
void getGraphiteFeatureSettingLabel(XeTeXLayoutEngine engine, UInt32 feature, UInt32 setting, unsigned short* buf);
long findGraphiteFeatureNamed(XeTeXLayoutEngine engine, const char* name, int namelength);
long findGraphiteFeatureSettingNamed(XeTeXLayoutEngine engine, UInt32 feature, const char* name, int namelength);

#ifdef __cplusplus
};
#endif

#endif /* XETEX_LAYOUT_INTERFACE_H */

