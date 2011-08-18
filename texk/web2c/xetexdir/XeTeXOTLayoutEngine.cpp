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

#include "XeTeXOTLayoutEngine.h"

/****************************************************************************\
class XeTeXHanLayoutEngine
  moved from XeTeXOTLayoutEngine.h to here in order not to include it into
  XeTeXLayoutInterface.cpp.
\****************************************************************************/
class XeTeXHanLayoutEngine : public XeTeXOTLayoutEngine
{
public:
    XeTeXHanLayoutEngine(const XeTeXFontInst *fontInstance, LETag scriptTag, LETag languageTag,
                            const GlyphSubstitutionTableHeader *gsubTable,
                            const GlyphPositioningTableHeader* gposTable,
							const LETag* addFeatures, const le_int32* addParams,
							const LETag* removeFeatures, LEErrorCode &success);

    virtual ~XeTeXHanLayoutEngine();

    virtual UClassID getDynamicClassID() const;
    static UClassID getStaticClassID();
};

#include "layout/ArabicLayoutEngine.h"
#include "layout/IndicLayoutEngine.h"
#include "layout/TibetanLayoutEngine.h"
#include "layout/ThaiLayoutEngine.h"
#include "layout/KhmerLayoutEngine.h"

#include "layout/LEScripts.h"
#include "layout/LELanguages.h"

const LETag emptyTag = 0x00000000;

static le_int32 getScriptCode(LETag scriptTag)
{
	for (le_int32 i = 0; i < scriptCodeCount; ++i)
		if (OpenTypeLayoutEngine::getScriptTag(i) == scriptTag)
			return i;
	return -1;
}

static le_int32 getLanguageCode(LETag languageTag)
{
	for (le_int32 i = 0; i < languageCodeCount; ++i)
		if (OpenTypeLayoutEngine::getLangSysTag(i) == languageTag)
			return i;
	return -1;
}

/*
 * XeTeXOTLayoutEngine
 */

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(XeTeXOTLayoutEngine)

LayoutEngine* XeTeXOTLayoutEngine::LayoutEngineFactory
				(const XeTeXFontInst* fontInstance,
					LETag scriptTag, LETag languageTag,
					const LETag* addFeatures, const le_int32* addParams,
					const LETag* removeFeatures,
					LEErrorCode &success)
{
    static le_uint32 gsubTableTag = LE_GSUB_TABLE_TAG;
    static le_uint32 gposTableTag = LE_GPOS_TABLE_TAG;

    if (LE_FAILURE(success))
        return NULL;

    const GlyphSubstitutionTableHeader* gsubTable = (const GlyphSubstitutionTableHeader*)fontInstance->getFontTable(gsubTableTag);
    const GlyphPositioningTableHeader* gposTable = (const GlyphPositioningTableHeader*)fontInstance->getFontTable(gposTableTag);
    LayoutEngine *result = NULL;
	
	le_uint32   scriptCode = getScriptCode(scriptTag);
	le_uint32   languageCode = getLanguageCode(languageTag);

	le_int32	typoFlags = 3;

    if ((gsubTable != NULL && gsubTable->coversScript(scriptTag))
    	|| (gposTable != NULL && gposTable->coversScript(scriptTag))) {
        switch (scriptCode) {
        case bengScriptCode:
        case devaScriptCode:
        case gujrScriptCode:
        case kndaScriptCode:
        case mlymScriptCode:
        case oryaScriptCode:
        case guruScriptCode:
        case tamlScriptCode:
        case teluScriptCode:
        case sinhScriptCode:
//            result = new XeTeXIndicLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, addFeatures, removeFeatures);
            result = new IndicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, FALSE, gsubTable, success);
            break;

        case arabScriptCode:
        case syrcScriptCode:
        case mongScriptCode:
//            result = new XeTeXArabicLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, addFeatures, removeFeatures);
            result = new ArabicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, gsubTable, success);
            break;

        case bopoScriptCode:
        case haniScriptCode:
        case hangScriptCode:
        case hiraScriptCode:
        case kanaScriptCode:
        case hrktScriptCode:
            result = new XeTeXHanLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, gposTable, addFeatures, addParams, removeFeatures, success);
            break;

        case tibtScriptCode:
            result = new TibetanOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, gsubTable, success);
            break;

        case khmrScriptCode:
            result = new KhmerOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, gsubTable, success);
            break;

        default:
            result = new XeTeXOTLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, gposTable, addFeatures, addParams, removeFeatures, success);
            break;
        }
    }
	else {
		switch (scriptCode) {
		case bengScriptCode:
		case devaScriptCode:
		case gujrScriptCode:
		case kndaScriptCode:
		case mlymScriptCode:
		case oryaScriptCode:
		case guruScriptCode:
		case tamlScriptCode:
		case teluScriptCode:
		case sinhScriptCode:
			result = new IndicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, success);
			break;

		case arabScriptCode:
//		case hebrScriptCode:
			result = new UnicodeArabicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, success);
			break;

		case thaiScriptCode:
			result = new ThaiLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, success);
			break;

		default:
			result = new OpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, success);
			break;
		}
    }

    if (LE_FAILURE(success))
        return NULL;

    return result;
}

XeTeXOTLayoutEngine::XeTeXOTLayoutEngine(
	const LEFontInstance* fontInstance, LETag scriptTag, LETag languageTag,
	const GlyphSubstitutionTableHeader* gsubTable, const GlyphPositioningTableHeader* gposTable,
	const LETag* addFeatures, const le_int32* addParams, const LETag* removeFeatures, LEErrorCode &success)
		: OpenTypeLayoutEngine(fontInstance, getScriptCode(scriptTag), getLanguageCode(languageTag), 3, gsubTable, success)
{
	fDefaultFeatureMap = fFeatureMap;
	
	// check the result of setScriptAndLanguageTags(), in case they were unknown to ICU
	if (fScriptTag != scriptTag || fLangSysTag != languageTag) {
		fScriptTag = scriptTag;
		fLangSysTag = languageTag;
	
		// reset the GPOS if the tags changed
		fGPOSTable = NULL;
		if (gposTable != NULL && gposTable->coversScriptAndLanguage(fScriptTag, fLangSysTag)) {
			fGPOSTable = gposTable;
		}
	}

	adjustFeatures(addFeatures, addParams, removeFeatures);
}

XeTeXOTLayoutEngine::~XeTeXOTLayoutEngine()
{
	if (fFeatureMap != NULL && fFeatureMap != fDefaultFeatureMap)
		LE_DELETE_ARRAY(fFeatureMap);
	if (fFeatureParamList != NULL)
		LE_DELETE_ARRAY(fFeatureParamList);
}

void XeTeXOTLayoutEngine::adjustFeatures(const LETag* addTags, const le_int32* addParams, const LETag* removeTags)
{
	// bail out if nothing was requested!
	if ((addTags == NULL || *addTags == emptyTag) && (removeTags == NULL || *removeTags == emptyTag))
		return;

	// figure out total tag count: initial set - removed tags + added tags omitting duplicates
	le_int32	totalCount = 0;
	for (le_int32 i = 0; i < fFeatureMapCount; ++i) {
		// skip any that are disabled by the mask
		if ((fFeatureMask & fFeatureMap[i].mask) != 0) {
			bool	remove = false;
			for (const LETag* r = removeTags; r != NULL && *r != emptyTag; ++r)
				if (*r == fFeatureMap[i].tag) {
					remove = true;
					break;
				}
			if (!remove)
				++totalCount;
		}
	}
	for (const LETag* a = addTags; a != NULL && *a != emptyTag; ++a) {
		// before counting an add tag, check original map, and check for duplicates in list
		bool	add = true;
		for (le_int32 i = 0; i < fFeatureMapCount; ++i)
			if (*a == fFeatureMap[i].tag) {
				if ((fFeatureMask & fFeatureMap[i].mask) != 0)
					add = false;
				break;
			}
		if (add) {
			for (const LETag* t = addTags; t != a; ++t)
				if (*a == *t) {
					add = false;
					break;
				}
		}
		if (add)
			++totalCount;
	}
	
	if (totalCount > 32)
		fprintf(stderr, "\n*** feature count exceeds mask size; some features will be ignored\n");

	// allocate new map
	FeatureMap* newFeatureMap = LE_NEW_ARRAY(FeatureMap, totalCount);
	le_int32	newFeatureCount = 0;
	le_int32*	newParamList = (addParams == NULL) ? NULL : LE_NEW_ARRAY(le_int32, totalCount);
	
	// copy the features into the map and assign mask bits
	FeatureMask	newFeatureMask = 0;
	FeatureMask	maskBit = 0x80000000UL;
	for (le_int32 i = 0; i < fFeatureMapCount; ++i) {
		bool	remove = false;
		if ((fFeatureMask & fFeatureMap[i].mask) == 0)
			remove = true;
		else
			for (const LETag* r = removeTags; r != NULL && *r != emptyTag; ++r)
				if (*r == fFeatureMap[i].tag) {
					remove = true;
					break;
				}
		if (!remove) {
			newFeatureMap[newFeatureCount].tag = fFeatureMap[i].tag;
			newFeatureMap[newFeatureCount].mask = maskBit;
			if (newParamList != NULL)
				newParamList[newFeatureCount] = 0;
			++newFeatureCount;
			newFeatureMask |= maskBit;
			maskBit >>= 1;
		}
	}
	const le_int32* param = addParams;
	for (const LETag* a = addTags; a != NULL && *a != emptyTag; ++a) {
		bool	add = true;
		for (le_int32 i = 0; i < fFeatureMapCount; ++i)
			if (*a == fFeatureMap[i].tag) {
				if ((fFeatureMask & fFeatureMap[i].mask) != 0)
					add = false;
				break;
			}
		if (add) {
			for (const LETag* t = addTags; t != a; ++t)
				if (*a == *t) {
					add = false;
					break;
				}
		}
		if (add) {
			newFeatureMap[newFeatureCount].tag = *a;
			newFeatureMap[newFeatureCount].mask = maskBit;
			if (newParamList != NULL)
				newParamList[newFeatureCount] = *param++;
			++newFeatureCount;
			newFeatureMask |= maskBit;
			maskBit >>= 1;
		}
	}

	fFeatureMask = newFeatureMask;
	fFeatureMap = newFeatureMap;
	fFeatureMapCount = newFeatureCount;
	fFeatureParamList = newParamList;
}


/*
 * XeTeXHanLayoutEngine
 */

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(XeTeXHanLayoutEngine)

static const LETag loclFeatureTag = LE_LOCL_FEATURE_TAG;
static const LETag smplFeatureTag = LE_SMPL_FEATURE_TAG;
static const LETag tradFeatureTag = LE_TRAD_FEATURE_TAG;
static const LETag vertFeatureTag = LE_VERT_FEATURE_TAG;
static const LETag vrt2FeatureTag = LE_VRT2_FEATURE_TAG;

#define loclFeatureMask 0x80000000UL
#define smplFeatureMask 0x40000000UL
#define tradFeatureMask 0x20000000UL
#define vertFeatureMask 0x10000000UL
#define vrt2FeatureMask 0x08000000UL

static const FeatureMap featureMap[] = {
    {loclFeatureTag, loclFeatureMask},
    {smplFeatureTag, smplFeatureMask},
    {tradFeatureTag, tradFeatureMask},
    {vertFeatureTag, vertFeatureMask},
    {vrt2FeatureTag, vrt2FeatureMask},
};
static const le_int32 featureMapCount = LE_ARRAY_SIZE(featureMap);

#define HORIZ_FEATURES (loclFeatureMask)
#define VERT_FEATURES  (loclFeatureMask|vertFeatureMask|vrt2FeatureMask)

XeTeXHanLayoutEngine::XeTeXHanLayoutEngine(const XeTeXFontInst *fontInstance, LETag scriptTag, LETag languageTag,
                            const GlyphSubstitutionTableHeader *gsubTable, const GlyphPositioningTableHeader *gposTable,
							const LETag *addFeatures, const le_int32* addParams,
							const LETag *removeFeatures, LEErrorCode &success)
	: XeTeXOTLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, gposTable, NULL, NULL, NULL, success)
{
	// reset the feature map and default features
	fFeatureMap = featureMap;
	fDefaultFeatureMap = fFeatureMap;
	fFeatureMapCount = featureMapCount;

	fFeatureMask = fontInstance->getLayoutDirVertical() ? VERT_FEATURES : HORIZ_FEATURES;
	
	// then apply any adjustments
	adjustFeatures(addFeatures, addParams, removeFeatures);
}

XeTeXHanLayoutEngine::~XeTeXHanLayoutEngine()
{
}
