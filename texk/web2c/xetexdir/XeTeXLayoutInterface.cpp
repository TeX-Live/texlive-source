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

#include "XeTeXLayoutInterface.h"

#include "XeTeXOTLayoutEngine.h"
#include "XeTeXFontInst.h"
#ifdef XETEX_MAC
#include "XeTeXFontInst_Mac.h"
#else
#include "XeTeXFontInst_FC.h"
#endif

#include "XeTeXFontMgr.h"

#include "XeTeXswap.h"

#include "Features.h"
#include "ScriptAndLanguage.h"

#include "unicode/ubidi.h"

#include <math.h>

struct XeTeXLayoutEngine_rec
{
	LayoutEngine*	layoutEngine;
	XeTeXFontInst*	font;
	UInt32			scriptTag;
	UInt32			languageTag;
	UInt32*			addedFeatures;
	UInt32*			removedFeatures;
	UInt32			rgbValue;
};

/*******************************************************************/
/* Glyph bounding box cache to speed up \XeTeXuseglyphmetrics mode */
/*******************************************************************/
#include <map>

// key is combined value representing (font_id << 16) + glyph
// value is glyph bounding box in TeX points
static std::map<UInt32,GlyphBBox>	sGlyphBoxes;

int
getCachedGlyphBBox(UInt16 fontID, UInt16 glyphID, GlyphBBox* bbox)
{
	UInt32	key = ((UInt32)fontID << 16) + glyphID;
	std::map<UInt32,GlyphBBox>::const_iterator i = sGlyphBoxes.find(key);
	if (i == sGlyphBoxes.end()) {
		return 0;
	}
	*bbox = i->second;
	return 1;
}

void
cacheGlyphBBox(UInt16 fontID, UInt16 glyphID, const GlyphBBox* bbox)
{
	UInt32	key = ((UInt32)fontID << 16) + glyphID;
	sGlyphBoxes[key] = *bbox;
}
/*******************************************************************/

void terminatefontmanager()
{
	XeTeXFontMgr::Terminate();
}

XeTeXFont createFont(PlatformFontRef fontRef, Fixed pointSize)
{
	LEErrorCode status = LE_NO_ERROR;
#ifdef XETEX_MAC
	XeTeXFontInst* font = new XeTeXFontInst_Mac((ATSFontRef)fontRef, Fix2X(pointSize), status);
#else
	XeTeXFontInst* font = new XeTeXFontInst_FC((FcPattern*)fontRef, Fix2X(pointSize), status);
#endif
	if (LE_FAILURE(status)) {
		delete font;
		return NULL;
	}
	return (XeTeXFont)font;
}

void setFontLayoutDir(XeTeXFont font, int vertical)
{
	((XeTeXFontInst*)font)->setLayoutDirVertical(vertical != 0);
}

PlatformFontRef getFontRef(XeTeXLayoutEngine engine)
{
	return engine->font->getFontRef();
}

PlatformFontRef findFontByName(const char* name, char* var, double size)
{
	return (XeTeXFontMgr::GetFontManager()->findFont(name, var, size));
}

char getReqEngine()
{
	return XeTeXFontMgr::GetFontManager()->getReqEngine();
}

const char* getFullName(PlatformFontRef fontRef)
{
	return XeTeXFontMgr::GetFontManager()->getFullName(fontRef);
}

const char* getPSName(PlatformFontRef fontRef)
{
	return XeTeXFontMgr::GetFontManager()->getPSName(fontRef);
}

void getNames(PlatformFontRef fontRef, const char** psName, const char** famName, const char** styName)
{
	XeTeXFontMgr::GetFontManager()->getNames(fontRef, psName, famName, styName);
}

void deleteFont(XeTeXFont font)
{
	delete (XeTeXFontInst*)font;
}

void* getFontTablePtr(XeTeXFont font, UInt32 tableTag)
{
	return const_cast<void*>(((XeTeXFontInst*)font)->getFontTable(tableTag));
}

/* apparently M_PI isn't defined by <math.h> under VC++ */
#ifdef WIN32
#ifndef M_PI
#define M_PI 3.1415926535898
#endif
#endif

Fixed getSlant(XeTeXFont font)
{
	float italAngle = ((XeTeXFontInst*)font)->getItalicAngle();
	return X2Fix(tan(-italAngle * M_PI / 180.0));
}

static const ScriptListTable*
getLargerScriptListTable(XeTeXFont font)
{
	const ScriptListTable* scriptListSub = NULL;
	const ScriptListTable* scriptListPos = NULL;
	
    const GlyphSubstitutionTableHeader* gsubTable = (const GlyphSubstitutionTableHeader*)((XeTeXFontInst*)font)->getFontTable(kGSUB);
	UInt32	scriptCountSub = 0;
	if (gsubTable != NULL) {
		scriptListSub = (const ScriptListTable*)((const char*)gsubTable + SWAP(gsubTable->scriptListOffset));
		scriptCountSub = SWAP(scriptListSub->scriptCount);
	}

    const GlyphPositioningTableHeader* gposTable = (const GlyphPositioningTableHeader*)((XeTeXFontInst*)font)->getFontTable(kGPOS);
	UInt32	scriptCountPos = 0;
	if (gposTable != NULL) {
		scriptListPos = (const ScriptListTable*)((const char*)gposTable + SWAP(gposTable->scriptListOffset));
		scriptCountPos = SWAP(scriptListPos->scriptCount);
	}

	return scriptCountPos > scriptCountSub ? scriptListPos : scriptListSub;
}

UInt32 countScripts(XeTeXFont font)
{
	const ScriptListTable*	scriptList = getLargerScriptListTable(font);
	if (scriptList == NULL)
		return 0;

	return SWAP(scriptList->scriptCount);
}

UInt32 getIndScript(XeTeXFont font, UInt32 index)
{
	const ScriptListTable* scriptList = getLargerScriptListTable(font);
	if (scriptList == NULL)
		return 0;

	if (index < SWAP(scriptList->scriptCount))
		return SWAPT(scriptList->scriptRecordArray[index].tag);

	return 0;
}

UInt32 countScriptLanguages(XeTeXFont font, UInt32 script)
{
	const ScriptListTable* scriptList = getLargerScriptListTable(font);
	if (scriptList == NULL)
		return 0;

	const ScriptTable*  scriptTable = scriptList->findScript(script);
	if (scriptTable == NULL)
		return 0;
	
	UInt32  langCount = SWAP(scriptTable->langSysCount);
	
	return langCount;
}

UInt32 getIndScriptLanguage(XeTeXFont font, UInt32 script, UInt32 index)
{
	const ScriptListTable* scriptList = getLargerScriptListTable(font);
	if (scriptList == NULL)
		return 0;

	const ScriptTable*  scriptTable = scriptList->findScript(script);
	if (scriptTable == NULL)
		return 0;

	if (index < SWAP(scriptTable->langSysCount))
		return SWAPT(scriptTable->langSysRecordArray[index].tag);

	return 0;
}

UInt32 countFeatures(XeTeXFont font, UInt32 script, UInt32 language)
{
	UInt32  total = 0;
    const GlyphLookupTableHeader* table;
	for (int i = 0; i < 2; ++i) {
		table = (const GlyphLookupTableHeader*)((XeTeXFontInst*)font)->getFontTable(i == 0 ? kGSUB : kGPOS);
		if (table != NULL) {
			const ScriptListTable* scriptList = (const ScriptListTable*)((const char*)table + SWAP(table->scriptListOffset));
			const ScriptTable*  scriptTable = scriptList->findScript(script);
			if (scriptTable != NULL) {
				const LangSysTable* langTable = scriptTable->findLanguage(language, (language != 0));
				if (langTable != NULL) {
					total += SWAP(langTable->featureCount);
					if (langTable->reqFeatureIndex != 0xffff)
						total += 1;
				}
			}
		}
	}
	
	return total;
}

UInt32 getIndFeature(XeTeXFont font, UInt32 script, UInt32 language, UInt32 index)
{
    const GlyphLookupTableHeader* table;
	UInt16  featureIndex = 0xffff;
	for (int i = 0; i < 2; ++i) {
		table = (const GlyphLookupTableHeader*)((XeTeXFontInst*)font)->getFontTable(i == 0 ? kGSUB : kGPOS);
		if (table != NULL) {
			const ScriptListTable* scriptList = (const ScriptListTable*)((const char*)table + SWAP(table->scriptListOffset));
			const ScriptTable*  scriptTable = scriptList->findScript(script);
			if (scriptTable != NULL) {
				const LangSysTable* langTable = scriptTable->findLanguage(language, (language != 0));
				if (langTable != NULL) {
					if (SWAP(langTable->reqFeatureIndex) != 0xffff)
						if (index == 0)
							featureIndex = SWAP(langTable->reqFeatureIndex);
						else
							index -= 1;
					if (index < SWAP(langTable->featureCount))
						featureIndex = SWAP(langTable->featureIndexArray[index]);
					index -= SWAP(langTable->featureCount);
				}
			}
			if (featureIndex != 0xffff) {
				LETag   featureTag;
				const FeatureListTable* featureListTable = (const FeatureListTable*)((const char*)table + SWAP(table->featureListOffset));
				(void)featureListTable->getFeatureTable(featureIndex, &featureTag);
				return featureTag;
			}
		}
	}
	
	return 0;
}

float getGlyphWidth(XeTeXFont font, UInt32 gid)
{
	LEPoint	adv;
	((XeTeXFontInst*)font)->getGlyphAdvance(gid, adv);
	return adv.fX;
}

UInt32
countGlyphs(XeTeXFont font)
{
	return ((XeTeXFontInst*)font)->getNumGlyphs();
}

XeTeXFont getFont(XeTeXLayoutEngine engine)
{
	return (XeTeXFont)(engine->font);
}

XeTeXLayoutEngine createLayoutEngine(XeTeXFont font, UInt32 scriptTag, UInt32 languageTag,
										UInt32* addFeatures, UInt32* removeFeatures, UInt32 rgbValue)
{
	LEErrorCode status = LE_NO_ERROR;
	XeTeXLayoutEngine result = new XeTeXLayoutEngine_rec;
	result->font = (XeTeXFontInst*)font;
	result->scriptTag = scriptTag;
	result->languageTag = languageTag;
	result->addedFeatures = addFeatures;
	result->removedFeatures = removeFeatures;
	result->rgbValue = rgbValue;
	result->layoutEngine = XeTeXOTLayoutEngine::LayoutEngineFactory((XeTeXFontInst*)font,
						scriptTag, languageTag, (LETag*)addFeatures, (LETag*)removeFeatures, status);
	if (LE_FAILURE(status) || result->layoutEngine == NULL) {
		delete result;
		return NULL;
	}

	return result;
}

void deleteLayoutEngine(XeTeXLayoutEngine engine)
{
	delete engine->layoutEngine;
	delete engine->font;
}

SInt32 layoutChars(XeTeXLayoutEngine engine, UInt16 chars[], SInt32 offset, SInt32 count, SInt32 max,
						char rightToLeft, float x, float y, SInt32* status)
{
	LEErrorCode success = (LEErrorCode)*status;
	le_int32	glyphCount = engine->layoutEngine->layoutChars(chars, offset, count, max, rightToLeft, x, y, success);
	*status = success;
	return glyphCount;
}

void getGlyphs(XeTeXLayoutEngine engine, UInt32 glyphs[], SInt32* status)
{
	LEErrorCode success = (LEErrorCode)*status;
	engine->layoutEngine->getGlyphs((LEGlyphID*)glyphs, success);
	*status = success;
}

void getGlyphPositions(XeTeXLayoutEngine engine, float positions[], SInt32* status)
{
	LEErrorCode success = (LEErrorCode)*status;
	engine->layoutEngine->getGlyphPositions(positions, success);
	*status = success;
}

void getGlyphPosition(XeTeXLayoutEngine engine, SInt32 index, float* x, float* y, SInt32* status)
{
	LEErrorCode success = (LEErrorCode)*status;
	engine->layoutEngine->getGlyphPosition(index, *x, *y, success);
	*status = success;
}

UInt32 getScriptTag(XeTeXLayoutEngine engine)
{
	return engine->scriptTag;
}

UInt32 getLanguageTag(XeTeXLayoutEngine engine)
{
	return engine->languageTag;
}

float getPointSize(XeTeXLayoutEngine engine)
{
	return engine->font->getXPixelsPerEm();
}

void getAscentAndDescent(XeTeXLayoutEngine engine, float* ascent, float* descent)
{
	*ascent = engine->font->getExactAscent();
	*descent = engine->font->getExactDescent();
}

UInt32* getAddedFeatures(XeTeXLayoutEngine engine)
{
	return engine->addedFeatures;
}

UInt32* getRemovedFeatures(XeTeXLayoutEngine engine)
{
	return engine->removedFeatures;
}

int getDefaultDirection(XeTeXLayoutEngine engine)
{
	switch (engine->scriptTag) {
		case kArabic:
		case kSyriac:
		case kThaana:
		case kHebrew:
			return UBIDI_DEFAULT_RTL;
	}
	return UBIDI_DEFAULT_LTR;
}

UInt32 getRgbValue(XeTeXLayoutEngine engine)
{
	return engine->rgbValue;
}

void getGlyphBounds(XeTeXLayoutEngine engine, UInt32 glyphID, GlyphBBox* bbox)
{
	engine->font->getGlyphBounds(glyphID, bbox);
}

float getGlyphWidthFromEngine(XeTeXLayoutEngine engine, UInt32 glyphID)
{
	return engine->font->getGlyphWidth(glyphID);
}

void getGlyphHeightDepth(XeTeXLayoutEngine engine, UInt32 glyphID, float* height, float* depth)
{
	engine->font->getGlyphHeightDepth(glyphID, height, depth);
}

void getGlyphSidebearings(XeTeXLayoutEngine engine, UInt32 glyphID, float* lsb, float* rsb)
{
	engine->font->getGlyphSidebearings(glyphID, lsb, rsb);
}

float getGlyphItalCorr(XeTeXLayoutEngine engine, UInt32 glyphID)
{
	return engine->font->getGlyphItalCorr(glyphID);
}

UInt32 mapCharToGlyph(XeTeXLayoutEngine engine, UInt32 charCode)
{
	return engine->font->mapCharToGlyph(charCode);
}

#include "appleGlyphNames.c"

int
findGlyphInPostTable(const char* buffer, int tableSize, const char* glyphName)
{
	const postTable* p = (const postTable*)buffer;
	UInt16	g = 0;
	switch (SWAP(p->format)) {
		case 0x00010000:
			{
				char*	cp;
				while ((cp = appleGlyphNames[g]) != 0) {
					if (strcmp(glyphName, cp) == 0)
						return g;
					++g;
				}
			}
			break;
		
		case 0x00020000:
			{
				const UInt16*	n = (UInt16*)(p + 1);
				UInt16	numGlyphs = SWAP(*n++);
				const UInt8*	ps = (const UInt8*)(n + numGlyphs);
				std::vector<std::string>	newNames;
				while (ps < (const UInt8*)buffer + tableSize) {
					newNames.push_back(std::string((char*)ps + 1, *ps));
					ps += *ps + 1;
				}
				for (g = 0; g < numGlyphs; ++g) {
					if (SWAP(*n) < 258) {
						if (strcmp(appleGlyphNames[SWAP(*n)], glyphName) == 0)
							return g;
					}
					else {
						if (strcmp(newNames[SWAP(*n) - 258].c_str(), glyphName) == 0)
							return g;
					}
					++n;
				}
			}
			break;
		
		case 0x00028000:
			break;
		
		case 0x00030000:
			// TODO: see if it's a CFF OpenType font, and if so, get the glyph names from the CFF data
			break;
		
		case 0x00040000:
			break;
		
		default:
			break;
	}

	return 0;
}

int
mapGlyphToIndex(XeTeXLayoutEngine engine, const char* glyphName)
{
	return engine->font->mapGlyphToIndex(glyphName);
}
