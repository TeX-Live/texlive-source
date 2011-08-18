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

#ifndef __XeTeXOTLayoutEngine_h
#define __XeTeXOTLayoutEngine_h

#include "layout/OpenTypeLayoutEngine.h"

//  /* ICU-4.2 added 'success' as last parameter to LayoutEngine constructors.  */
//  #include "unicode/uversion.h"
//  #define U_ICU_VERSION_CODE (U_ICU_VERSION_MAJOR_NUM*10+U_ICU_VERSION_MINOR_NUM)

#include "XeTeXFontInst.h"

class XeTeXOTLayoutEngine : public OpenTypeLayoutEngine
{
public:
    XeTeXOTLayoutEngine(const LEFontInstance* fontInstance, LETag scriptTag, LETag languageTag,
                            const GlyphSubstitutionTableHeader* gsubTable,
                            const GlyphPositioningTableHeader* gposTable,
							const LETag* addFeatures, const le_int32* addParams,
							const LETag* removeFeatures, LEErrorCode &success);

    virtual ~XeTeXOTLayoutEngine();

	virtual void adjustFeatures(const LETag* addTags, const le_int32* addParams, const LETag* removeTags);

    virtual UClassID getDynamicClassID() const;
    static UClassID getStaticClassID();

    static LayoutEngine* LayoutEngineFactory
				(const XeTeXFontInst* fontInstance,
					LETag scriptTag, LETag languageTag,
					const LETag* addFeatures, const le_int32* addParams,
					const LETag* removeFeatures,
					LEErrorCode &success);

protected:
	const FeatureMap*	fDefaultFeatureMap;
	
private:
};

#endif

