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

#ifndef __XETEX_FONT_MANAGER_H
#define __XETEX_FONT_MANAGER_H

#ifdef XETEX_MAC
#include <Carbon/Carbon.h>
typedef ATSFontRef	PlatformFontRef;
#else
#include <fontconfig/fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H
typedef FcPattern*	PlatformFontRef;
#endif

#include "XeTeX_ext.h"

#include "XeTeXLayoutInterface.h"

#ifdef __cplusplus	/* allow inclusion in plain C files just to get the typedefs above */

#include <string>
#include <map>
#include <list>
#include <vector>

class XeTeXFontMgr
{
public:
	static XeTeXFontMgr*			GetFontManager();
		// returns the global fontmanager (creating it if necessary)
	static void						Terminate();
		// clean up (may be required if using the cocoa implementation)

	PlatformFontRef					findFont(const char* name, char* variant, double ptSize);
		// 1st arg is name as specified by user (C string, UTF-8)
		// 2nd is /B/I/AAT/ICU[/USP]/S=## qualifiers
		// 1. try name given as "full name"
		// 2. if there's a hyphen, split and try "family-style"
		// 3. try as PostScript name
		// 4. try name as family with "Regular/Plain/Normal" style
		// apply style qualifiers and optical sizing if present

		// SIDE EFFECT: sets sReqEngine to 'A' or 'I' [or 'U'] if appropriate,
		//   else clears it to 0

		// SIDE EFFECT: updates TeX variables /nameoffile/ and /namelength/,
		//   to match the actual font found

		// SIDE EFFECT: edits /variant/ string in-place removing /B or /I
		
	const char*						getFullName(PlatformFontRef font) const;
		// return the full name of the font, suitable for use in XeTeX source
		// without requiring style qualifiers

	void							getNames(PlatformFontRef font, const char** psName,
											const char** famName, const char** styName) const;
		// return Postscript, family, and style names, for use in .xdv

	double							getDesignSize(XeTeXFont font);

	char							getReqEngine() const;
		// return the requested rendering technology for the most recent findFont
		// or 0 if no specific technology was requested

protected:
	static XeTeXFontMgr*			sFontManager;
	static char						sReqEngine;
	
									XeTeXFontMgr()
										{ }
	virtual							~XeTeXFontMgr()
										{ }
										
	virtual void					initialize() = 0;
	virtual void					terminate();

	virtual std::string				getPlatformFontDesc(PlatformFontRef font) const = 0;

	class Font;
	class Family;

	struct OpSizeRec {
		unsigned short	designSize;
		unsigned short	subFamilyID;
		unsigned short	nameCode;
		unsigned short	minSize;
		unsigned short	maxSize;
	};

	class Font {
		public:
							Font(PlatformFontRef ref)
								: fullName(NULL), psName(NULL), familyName(NULL), styleName(NULL)
								, parent(NULL)
								, fontRef(ref), weight(0), width(0), slant(0)
								, isReg(false), isBold(false), isItalic(false)
								{ opSizeInfo.subFamilyID = 0;
								  opSizeInfo.designSize = 100; } /* default to 10bp */
							~Font()
								{ delete fullName; delete psName; }

			std::string*	fullName;
			std::string*	psName;
			std::string*	familyName;	// default family and style names that should locate this font
			std::string*	styleName;
			Family*			parent;
			PlatformFontRef	fontRef;
			OpSizeRec		opSizeInfo;
			UInt16			weight;
			UInt16			width;
			SInt16			slant;
			bool			isReg;
			bool			isBold;
			bool			isItalic;
	};
	
	class Family {
		public:
											Family()
												: minWeight(0), maxWeight(0)
												, minWidth(0), maxWidth(0)
												, minSlant(0), maxSlant(0)
												{
													styles = new std::map<std::string,Font*>;
												}
											~Family()
												{
													delete styles;
												}

			std::map<std::string,Font*>*	styles;
			UInt16							minWeight;
			UInt16							maxWeight;
			UInt16							minWidth;
			UInt16							maxWidth;
			SInt16							minSlant;
			SInt16							maxSlant;
	};

	class NameCollection {
	public:
		std::list<std::string>	familyNames;
		std::list<std::string>	styleNames;
		std::list<std::string>	fullNames;
		std::string				psName;
		std::string				subFamily;
	};	

	std::map<std::string,Font*>					nameToFont;						// maps full name (as used in TeX source) to font record
	std::map<std::string,Family*>				nameToFamily;
	std::map<PlatformFontRef,Font*>				platformRefToFont;
	std::map<std::string,Font*>					psNameToFont;					// maps PS name (as used in .xdv) to font record

	int				weightAndWidthDiff(const Font* a, const Font* b) const;
	int				styleDiff(const Font* a, int wt, int wd, int slant) const;
	Font*			bestMatchFromFamily(const Family* fam, int wt, int wd, int slant) const;
	void			appendToList(std::list<std::string>* list, const char* str);
	void			prependToList(std::list<std::string>* list, const char* str);
	void			addToMaps(PlatformFontRef platformFont, const NameCollection* names);

	const OpSizeRec* getOpSizePtr(XeTeXFont font);

	virtual void	getOpSizeRecAndStyleFlags(Font* theFont);
	virtual void	searchForHostPlatformFonts(const std::string& name) = 0;
	
#ifdef XETEX_MAC
	virtual NameCollection*		readNames(ATSUFontID fontID) = 0;
#else
	virtual NameCollection*		readNames(PlatformFontRef fontRef) = 0;
#endif

	void	die(const char*s, int i) const;	/* for fatal internal errors! */
};

#endif	/* __cplusplus */


#endif	/* __XETEX_FONT_MANAGER_H */
