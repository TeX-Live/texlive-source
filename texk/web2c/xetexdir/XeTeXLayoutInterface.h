/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2006 by SIL International
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

#ifdef XETEX_MAC
#include <Carbon/Carbon.h>
#endif

#include "XeTeX_ext.h"
#include "XeTeXFontMgr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct XeTeXFont_rec* XeTeXFont;
typedef struct XeTeXLayoutEngine_rec* XeTeXLayoutEngine;

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
						UInt32* addFeatures, SInt32* addParams, UInt32* removeFeatures, UInt32 rgbValue);

void deleteLayoutEngine(XeTeXLayoutEngine engine);

XeTeXFont getFont(XeTeXLayoutEngine engine);
PlatformFontRef getFontRef(XeTeXLayoutEngine engine);

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

#ifdef __cplusplus
};
#endif
