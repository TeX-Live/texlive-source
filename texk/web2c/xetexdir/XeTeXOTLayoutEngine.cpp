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

#include "XeTeXOTLayoutEngine.h"

#include "ThaiLayoutEngine.h"
#include "KhmerLayoutEngine.h"

#include "LEScripts.h"
#include "LELanguages.h"

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

LayoutEngine* XeTeXOTLayoutEngine::LayoutEngineFactory
				(const XeTeXFontInst* fontInstance,
					LETag scriptTag, LETag languageTag,
					const LETag* addFeatures, const le_int32* addParams,
					const LETag* removeFeatures,
					LEErrorCode &success)
{
    static le_uint32 gsubTableTag = LE_GSUB_TABLE_TAG;

    if (LE_FAILURE(success))
        return NULL;

    const GlyphSubstitutionTableHeader* gsubTable = (const GlyphSubstitutionTableHeader*)fontInstance->getFontTable(gsubTableTag);
    LayoutEngine *result = NULL;
	
	le_uint32   scriptCode = getScriptCode(scriptTag);
	le_uint32   languageCode = getLanguageCode(languageTag);

	le_int32	typoFlags = 3;

    if (gsubTable != NULL && gsubTable->coversScript(scriptTag)) {
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
//            result = new XeTeXIndicLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, addFeatures, removeFeatures);
            result = new IndicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, gsubTable);
            break;

        case arabScriptCode:
        case syrcScriptCode:
        case mongScriptCode:
//            result = new XeTeXArabicLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, addFeatures, removeFeatures);
            result = new ArabicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, gsubTable);
            break;

        case bopoScriptCode:
        case haniScriptCode:
        case hangScriptCode:
        case hiraScriptCode:
        case kanaScriptCode:
        case hrktScriptCode:
            result = new XeTeXHanLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, addFeatures, addParams, removeFeatures);
            break;

        case khmrScriptCode:
            result = new KhmerOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags, gsubTable);
            break;

        default:
            result = new XeTeXOTLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, addFeatures, addParams, removeFeatures);
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
			result = new IndicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags);
			break;

		case arabScriptCode:
		case hebrScriptCode:
			result = new UnicodeArabicOpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags);
			break;

		case thaiScriptCode:
			result = new ThaiLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags);
			break;

		default:
			result = new OpenTypeLayoutEngine(fontInstance, scriptCode, languageCode, typoFlags);
			break;
		}
    }

    if (result == NULL)
        success = LE_MEMORY_ALLOCATION_ERROR;

    return result;
}

/*
 * XeTeXOTLayoutEngine
 */

const char XeTeXOTLayoutEngine::fgClassID=0;

XeTeXOTLayoutEngine::XeTeXOTLayoutEngine(
	const LEFontInstance* fontInstance, LETag scriptTag, LETag languageTag,
	const GlyphSubstitutionTableHeader* gsubTable,
	const LETag* addFeatures, const le_int32* addParams, const LETag* removeFeatures)
		: OpenTypeLayoutEngine(fontInstance, getScriptCode(scriptTag), getLanguageCode(languageTag), 3, gsubTable)
{
    static le_uint32 gposTableTag = LE_GPOS_TABLE_TAG;

	fDefaultFeatures = fFeatureList;
	
	// check the result of setScriptAndLanguageTags(), in case they were unknown to ICU
	if (fScriptTag != scriptTag || fLangSysTag != languageTag) {
		fScriptTag = scriptTag;
		fLangSysTag = languageTag;
	
		// reset the GPOS if the tags changed
		fGPOSTable = NULL;
		const GlyphPositioningTableHeader *gposTable = (const GlyphPositioningTableHeader *) getFontTable(gposTableTag);
		if (gposTable != NULL && gposTable->coversScriptAndLanguage(fScriptTag, fLangSysTag)) {
			fGPOSTable = gposTable;
		}
	}

	adjustFeatures(addFeatures, addParams, removeFeatures);
}

XeTeXOTLayoutEngine::~XeTeXOTLayoutEngine()
{
	if (fFeatureList != NULL && fFeatureList != fDefaultFeatures)
		LE_DELETE_ARRAY(fFeatureList);
	if (fFeatureParamList != NULL)
		LE_DELETE_ARRAY(fFeatureParamList);
}

void XeTeXOTLayoutEngine::adjustFeatures(const LETag* addTags, const le_int32* addParams, const LETag* removeTags)
{
	// bail out if nothing was requested!
	if ((addTags == NULL || *addTags == emptyTag) && (removeTags == NULL || *removeTags == emptyTag))
		return;
	
	// count the max possible number of tags we might end up with
	le_uint32   count = 0;
	const LETag*	pTag = fFeatureList;
	while (*pTag++ != emptyTag)
		count++;
	if (addTags != NULL && *addTags != emptyTag) {
		pTag = addTags;
		while (*pTag++ != emptyTag)
			count++;
	}
	
	// allocate new array for the tag list, big enough for max count
	LETag* newList = LE_NEW_ARRAY(LETag, count + 1);
	
	// copy the existing tags, skipping any in the remove list
	LETag*  dest = newList;
	le_int32* newParams = LE_NEW_ARRAY(le_int32, count);
	le_int32*	dest2 = newParams;
	pTag = fFeatureList;
	while (*pTag != emptyTag) {
		const LETag*	t = removeTags;
		if (t != NULL)
			while (*t != emptyTag)
				if (*t == *pTag)
					break;
				else
					t++;
		if (t == NULL || *t == emptyTag) {
			*dest++ = *pTag;
			*dest2++ = 0;
		}
		pTag++;
	}
	
	// copy the added tags and parameters, skipping any already present
	pTag = addTags;
	const le_int32* pParam = addParams;
	if (pTag != NULL)
		while (*pTag != emptyTag) {
			const LETag*	t = newList;
			while (t < dest)
				if (*t == *pTag)
					break;
				else
					t++;
			if (t == dest) {
				*dest++ =  *pTag;
				*dest2++ = *pParam;
			}
			pTag++;
			pParam++;
		}
	
	// terminate the new list
	*dest = emptyTag;
	
	// delete the previous list, unless it was the static default array
	if (fFeatureList != fDefaultFeatures)
		LE_DELETE_ARRAY(fFeatureList);
	
	fFeatureList = newList;
	fFeatureParamList = newParams;
}


/*
 * XeTeXHanLayoutEngine
 */

const char XeTeXHanLayoutEngine::fgClassID=0;

static const LETag loclFeatureTag = LE_LOCL_FEATURE_TAG;
static const LETag smplFeatureTag = LE_SMPL_FEATURE_TAG;
static const LETag tradFeatureTag = LE_TRAD_FEATURE_TAG;
static const LETag vertFeatureTag = LE_VERT_FEATURE_TAG;
static const LETag vrt2FeatureTag = LE_VRT2_FEATURE_TAG;

static const LETag horizontalFeatures[] = {loclFeatureTag, emptyTag};
static const LETag verticalFeatures[] = {loclFeatureTag, vrt2FeatureTag, vertFeatureTag, emptyTag};

XeTeXHanLayoutEngine::XeTeXHanLayoutEngine(const XeTeXFontInst *fontInstance, LETag scriptTag, LETag languageTag,
                            const GlyphSubstitutionTableHeader *gsubTable,
							const LETag *addFeatures, const le_int32* addParams,
							const LETag *removeFeatures)
	: XeTeXOTLayoutEngine(fontInstance, scriptTag, languageTag, gsubTable, NULL, NULL, NULL)
{
	// reset the default feature list
	fFeatureList = fontInstance->getLayoutDirVertical() ? verticalFeatures : horizontalFeatures;
	fDefaultFeatures = fFeatureList;
	
	// then apply any adjustments
	adjustFeatures(addFeatures, addParams, removeFeatures);
}

XeTeXHanLayoutEngine::~XeTeXHanLayoutEngine()
{
}
