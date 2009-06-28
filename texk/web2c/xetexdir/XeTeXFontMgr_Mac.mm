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

#ifdef XETEX_MAC // this file only for Mac OS X

#include "XeTeXFontMgr_Mac.h"

#include <Cocoa/Cocoa.h>

XeTeXFontMgr::NameCollection*
XeTeXFontMgr_Mac::readNames(ATSFontRef fontRef)
{
	const int	BUFUNIT = 256;
	static int		bufSize = BUFUNIT;
	static char*	buffer = new char[bufSize];

	NameCollection*	names = new NameCollection;

	CFStringRef	psName;
	OSStatus	status = ATSFontGetPostScriptName(fontRef, kATSOptionFlagsDefault, &psName);
	if (status != noErr)
		return names;
	
	// extract UTF-8 form of name
	CFIndex length = CFStringGetLength(psName);	// in 16-bit character units
	if (length > 0) {
		length = length * 6 + 1;
		if (length >= bufSize) {
			delete[] buffer;
			bufSize = ((length / BUFUNIT) + 1) * BUFUNIT;
			buffer = new char[bufSize];
		}
		if (CFStringGetCString(psName, buffer, bufSize, kCFStringEncodingUTF8))
			names->psName = buffer;
	}
	CFRelease(psName); 

	ATSUFontID	fontID = FMGetFontFromATSFontRef(fontRef);

	ItemCount	nameCount;
	status = ATSUCountFontNames(fontID, &nameCount);
	if (status != noErr)
		die("ATSUCountFontNames failed, status=%d", status);

	std::list<std::string>	familyNames;
	std::list<std::string>	subFamilyNames;

	for (int n = 0; n < nameCount; ++n) {
		ByteCount			nameLength;
		FontNameCode		nameCode;
		FontPlatformCode	namePlatform;
		FontScriptCode		nameScript;
		FontLanguageCode	nameLang;
		status = ATSUGetIndFontName(fontID, n, 0, 0, &nameLength,
									&nameCode, &namePlatform, &nameScript, &nameLang);
		if (status != noErr && status != kATSUNoFontNameErr)
			die("ATSUGetIndFontName failed, status=%d", status);
		if (status == noErr && nameLength > 0) {
			switch (nameCode) {
				case kFontFullName:
				case kFontFamilyName:
				case kFontStyleName:
				case 16:	// preferred family -- use instead of family, if present
				case 17:	// preferred subfamily -- use instead of style, if present
					{
						bool	preferredName = false;
						TextEncoding	encoding;
						CFStringRef		nameStr = 0;
						if (nameLength >= bufSize) {
							delete[] buffer;
							bufSize = ((nameLength / BUFUNIT) + 1) * BUFUNIT;
							buffer = new char[bufSize];
						}
						status = ATSUGetIndFontName(fontID, n, bufSize, buffer, &nameLength,
													&nameCode, &namePlatform, &nameScript, &nameLang);
						if (namePlatform == kFontMacintoshPlatform) {
							GetTextEncodingFromScriptInfo(nameScript, nameLang, 0, &encoding);
							nameStr = CFStringCreateWithBytes(0, (UInt8*)buffer, nameLength, encoding, false);
							if (nameScript == kFontRomanScript && nameLang == kFontEnglishLanguage)
								preferredName = true;
						}
						else if ((namePlatform == kFontUnicodePlatform) || (namePlatform == kFontMicrosoftPlatform))
							nameStr = CFStringCreateWithCharacters(0, (UniChar*)buffer, nameLength / 2);
						if (nameStr != 0) {
							std::list<std::string>*	nameList = NULL;
							switch (nameCode) {
								case kFontFullName:
									nameList = &names->fullNames;
									break;
								case kFontFamilyName:
									nameList = &names->familyNames;
									break;
								case kFontStyleName:
									nameList = &names->styleNames;
									break;
								case 16:
									nameList = &familyNames;
									break;
								case 17:
									nameList = &subFamilyNames;
									break;
							}
							
							// extract UTF-8 form of name
							length = CFStringGetLength(nameStr);	// in 16-bit character units
							if (length > 0) {
								length = length * 6 + 1;
								if (length >= bufSize) {
									delete[] buffer;
									bufSize = ((length / BUFUNIT) + 1) * BUFUNIT;
									buffer = new char[bufSize];
								}
								if (CFStringGetCString(nameStr, buffer, bufSize, kCFStringEncodingUTF8)) {
									if (buffer[0] != 0) {
										if (nameList != NULL) {
											if (preferredName)
												prependToList(nameList, buffer);
											else
												appendToList(nameList, buffer);
										}
									}
								}
							}
		
							CFRelease(nameStr); 
						}
					}
					break;
			}
		}
	}

	if (familyNames.size() > 0)
		names->familyNames = familyNames;
	if (subFamilyNames.size() > 0)
		names->styleNames = subFamilyNames;

	return names;
}

void
XeTeXFontMgr_Mac::addFontsToCaches(CFArrayRef fonts)
{
	NSEnumerator*	enumerator = [(NSArray*)fonts objectEnumerator];
	while (id aFont = [enumerator nextObject]) {
		ATSFontRef	fontRef = ATSFontFindFromPostScriptName((CFStringRef)[aFont objectAtIndex: 0], kATSOptionFlagsDefault);
		NameCollection*	names = readNames(fontRef);
		addToMaps(fontRef, names);
		delete names;
	}
}

void
XeTeXFontMgr_Mac::addFamilyToCaches(ATSFontFamilyRef familyRef)
{
	CFStringRef	nameStr;
	OSStatus	status = ATSFontFamilyGetName(familyRef, kATSOptionFlagsDefault, &nameStr);
	if (status == noErr) {
		NSArray*	members = [[NSFontManager sharedFontManager]
								availableMembersOfFontFamily: (NSString*)nameStr];
		CFRelease(nameStr);
		addFontsToCaches((CFArrayRef)members);
	}
}

void
XeTeXFontMgr_Mac::addFontAndSiblingsToCaches(ATSFontRef fontRef)
{
	CFStringRef	name;
	OSStatus	status = ATSFontGetPostScriptName(fontRef, kATSOptionFlagsDefault, &name);
	if (status == noErr) {
		NSFont*	font = [NSFont fontWithName:(NSString*)name size:10.0];
		CFRelease(name);
		NSArray*	members = [[NSFontManager sharedFontManager]
								availableMembersOfFontFamily: [font familyName]];
		addFontsToCaches((CFArrayRef)members);
	}
}

void
XeTeXFontMgr_Mac::searchForHostPlatformFonts(const std::string& name)
{
	// the name might be:
	//	FullName
	//	Family-Style (if there's a hyphen)
	//	PSName
	//	Family
	// ...so we need to try it as each of these

	CFStringRef	nameStr = CFStringCreateWithCString(kCFAllocatorDefault, name.c_str(), kCFStringEncodingUTF8);
	ATSFontRef	fontRef = ATSFontFindFromName(nameStr, kATSOptionFlagsDefault);
	if (fontRef != kATSFontRefUnspecified) {
		// found it, so locate the family, and add all members to the caches
		addFontAndSiblingsToCaches(fontRef);
		return;
	}

	int	hyph = name.find('-');
	if (hyph > 0 && hyph < name.length() - 1) {
		std::string			family(name.begin(), name.begin() + hyph);
		CFStringRef			familyStr = CFStringCreateWithCString(kCFAllocatorDefault, family.c_str(), kCFStringEncodingUTF8);

		NSArray*	familyMembers = [[NSFontManager sharedFontManager]
									availableMembersOfFontFamily: (NSString*)familyStr];
		if ([familyMembers count] > 0) {
			addFontsToCaches((CFArrayRef)familyMembers);
			return;
		}

		ATSFontFamilyRef	familyRef = ATSFontFamilyFindFromName(familyStr, kATSOptionFlagsDefault);
		if (familyRef != 0xffffffff) {
			addFamilyToCaches(familyRef);
			return;
		}
	}
	
	fontRef = ATSFontFindFromPostScriptName(nameStr, kATSOptionFlagsDefault);
	if (fontRef != kATSFontRefUnspecified) {
		addFontAndSiblingsToCaches(fontRef);
		return;
	}

	NSArray*	familyMembers = [[NSFontManager sharedFontManager]
								availableMembersOfFontFamily: (NSString*)nameStr];
	if ([familyMembers count] > 0) {
		addFontsToCaches((CFArrayRef)familyMembers);
		return;
	}

	ATSFontFamilyRef	familyRef = ATSFontFamilyFindFromName(nameStr, kATSOptionFlagsDefault);
	if (familyRef != 0xffffffff) {
		addFamilyToCaches(familyRef);
		return;
	}
}

NSAutoreleasePool* pool = NULL;

void
XeTeXFontMgr_Mac::initialize()
{
	pool = [[NSAutoreleasePool alloc] init];
}

void
XeTeXFontMgr_Mac::terminate()
{
	if (pool != NULL) {
		[pool release];
	}
}

std::string
XeTeXFontMgr_Mac::getPlatformFontDesc(PlatformFontRef font) const
{
	FSSpec fileSpec;
	std::string path;
	if (ATSFontGetFileSpecification(font, &fileSpec) == noErr) {
		FSRef fileRef;
		if (FSpMakeFSRef(&fileSpec, &fileRef) == noErr) {
			UInt8 posixPath[PATH_MAX];
			if (FSRefMakePath(&fileRef, posixPath, PATH_MAX) == noErr) {
				path = (char*)posixPath;
			}
		}
	}
	if (path.length() == 0)
		path = "[unknown]";
	return path;
}

#endif // XETEX_MAC
