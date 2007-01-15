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

/* XeTeX_ext.c
 * additional plain C extensions for XeTeX - mostly platform-neutral
 */

#ifdef XETEX_OTHER
#include "xpdf/config.h"
#include "png.h"
#endif

#include "zlib.h"

#define EXTERN extern
#define Byte my_Byte /* hack to work around typedef conflict with zlib */
#include "xetexd.h"
#undef Byte

#ifdef XETEX_MAC
#undef input /* this is defined in texmfmp.h, but we don't need it and it confuses the carbon headers */
#include <Carbon/Carbon.h>
#endif

#include "XeTeX_ext.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "TECkit_Engine.h"

#include <kpathsea/c-ctype.h>
#include <kpathsea/line.h>
#include <kpathsea/readable.h>
#include <kpathsea/variable.h>
#include <kpathsea/absolute.h>

#include <math.h> /* for fabs() */

#include <time.h> /* For `struct tm'.  */
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#elif defined (HAVE_SYS_TIMEB_H)
#include <sys/timeb.h>
#endif

#if defined(__STDC__)
#include <locale.h>
#endif

#include <signal.h> /* Catch interrupts.  */

#include "XeTeXLayoutInterface.h"

#include "XeTeXswap.h"

#include "unicode/ubidi.h"
#include "unicode/ubrk.h"
#include "unicode/ucnv.h"

/* 
#include "sfnt.h"
	doesn't work in plain C files :(
*/

typedef struct
{
    Fixed    version;
    UInt16   numGlyphs;
    UInt16   maxPoints;
    UInt16   maxContours;
    UInt16   maxComponentPoints;
    UInt16   maxComponentContours;
    UInt16   maxZones;
    UInt16   maxTwilightPoints;
    UInt16   maxStorage;
    UInt16   maxFunctionDefs;
    UInt16   maxInstructionDefs;
    UInt16   maxStackElements;
    UInt16   maxSizeOfInstructions;
    UInt16   maxComponentElements;
    UInt16   maxComponentDepth;
} MAXPTable;

typedef struct
{
	Fixed	version;
	Fixed	italicAngle;
	SInt16	underlinePosition;
	UInt16	underlineThickness;
	UInt32	isFixedPitch;
	UInt32	minMemType42;
	UInt32	maxMemType42;
	UInt32	minMemType1;
	UInt32	maxMemType1;
} POSTTable;

enum {
    LE_MAXP_TABLE_TAG = 0x6D617870UL, /**< 'maxp' */
    LE_POST_TABLE_TAG = 0x706F7374UL, /**< 'post' */
};

/* tables/values used in UTF-8 interpretation - 
   code is based on ConvertUTF.[ch] sample code
   published by the Unicode consortium */
const UInt32
offsetsFromUTF8[6] =	{
	0x00000000UL,
	0x00003080UL,
	0x000E2080UL, 
	0x03C82080UL,
	0xFA082080UL,
	0x82082080UL
};

const UInt8
bytesFromUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

const UInt8
firstByteMark[7] = {
	0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};

const int halfShift					= 10;
const UInt32 halfBase				= 0x0010000UL;
const UInt32 halfMask				= 0x3FFUL;
const UInt32 kSurrogateHighStart	= 0xD800UL;
const UInt32 kSurrogateHighEnd		= 0xDBFFUL;
const UInt32 kSurrogateLowStart		= 0xDC00UL;
const UInt32 kSurrogateLowEnd		= 0xDFFFUL;
const UInt32 byteMask				= 0x000000BFUL;
const UInt32 byteMark				= 0x00000080UL;


/* if the user specifies a paper size or output driver program */
string papersize;
#ifdef XETEX_MAC
string outputdriver = "xdv2pdf"; /* default for backward compatibility on Mac OS X */
#else
string outputdriver = "xdvipdfmx -q -E"; /* else default to portable xdvipdfmx driver */
#endif


void initversionstring(char **versions)
{
#ifdef XETEX_OTHER
	int	fc_version = FcGetVersion();
#endif
	extern FT_Library	gFreeTypeLibrary; /* in XeTeXFontInst_FT2 */
	FT_Int	ftMajor, ftMinor, ftPatch;

	char* fmt =
		"Compiled with ICU version %s [with modifications for XeTeX]\n"
		"Compiled with zlib version %s; using %s\n"
		"Compiled with FreeType2 version %d.%d.%d; using %d.%d.%d\n"
#ifdef XETEX_MAC
		"Using Mac OS X Carbon, Cocoa & QuickTime frameworks\n"
#else
		"Compiled with fontconfig version %d.%d.%d; using %d.%d.%d\n"
		"Compiled with libpng version %s; using %s\n"
		"Compiled with xpdf version %s\n"
#endif
		;

	int	len = strlen(fmt)
			+ strlen(U_ICU_VERSION)
			+ strlen(ZLIB_VERSION)
			+ strlen(zlib_version)
#ifdef XETEX_OTHER
			+ strlen(PNG_LIBPNG_VER_STRING)
			+ strlen(png_libpng_ver)
			+ strlen(xpdfVersion)
			+ 6 * 3 /* for fontconfig version #s (won't really need 3 digits per field!) */
#endif
			+ 6 * 3; /* for freetype version #s (ditto) */

	*versions = xmalloc(len + 1);
		/* len will be more than enough, because of the placeholder chars in fmt
			that get replaced by the arguments */

	if (gFreeTypeLibrary == 0 && FT_Init_FreeType(&gFreeTypeLibrary) != 0) {
		fprintf(stderr, "FreeType initialization failed!\n");
		exit(9);
	}
	FT_Library_Version(gFreeTypeLibrary, &ftMajor, &ftMinor, &ftPatch);

    (void)sprintf(*versions, fmt,
		U_ICU_VERSION,
		ZLIB_VERSION, zlib_version,
		FREETYPE_MAJOR, FREETYPE_MINOR, FREETYPE_PATCH,
		ftMajor, ftMinor, ftPatch
#ifdef XETEX_OTHER
		,
		FC_VERSION / 10000, (FC_VERSION % 10000) / 100, FC_VERSION % 100,
		fc_version / 10000, (fc_version % 10000) / 100, fc_version % 100,
		PNG_LIBPNG_VER_STRING, png_libpng_ver,
		xpdfVersion
#endif
		);
}


extern char*	gettexstring(int strNumber);
void
setinputfileencoding(UFILE* f, int mode, int encodingData)
{
	if ((f->encodingMode == ICUMAPPING) && (f->conversionData != NULL))
		ucnv_close((UConverter*)(f->conversionData));
	f->conversionData = 0;
	
	switch (mode) {
		case UTF8:
		case UTF16BE:
		case UTF16LE:
		case RAW:
			f->encodingMode = mode;
			break;
		
		case ICUMAPPING:
			{
				char*	name = gettexstring(encodingData);
				UErrorCode	err = 0;
				UConverter*	cnv = ucnv_open(name, &err);
				if (cnv == NULL) {
					fprintf(stderr, "! Error %d creating Unicode converter for %s\n", err, name);
					f->encodingMode = RAW;
				}
				else {
					f->encodingMode = ICUMAPPING;
					f->conversionData = cnv;
				}
				free(name);
			}
			break;
	}
}

void
uclose(UFILE* f)
{
	if (f != 0) {
		fclose(f->f);
		if ((f->encodingMode == ICUMAPPING) && (f->conversionData != NULL))
			ucnv_close((UConverter*)(f->conversionData));
		free((void*)f);
	}
}

int
input_line_icu(UFILE* f)
{
static char* byteBuffer = NULL;

	UInt32		bytesRead = 0;
	int			i;
	UConverter*	cnv;
	long		outLen;
	UErrorCode	errorCode = 0;

	if (byteBuffer == NULL)
		byteBuffer = xmalloc(bufsize + 1);

	/* Recognize either LF or CR as a line terminator; skip initial LF if prev line ended with CR.  */
	i = getc(f->f);
	if (f->skipNextLF) {
		f->skipNextLF = 0;
		if (i == '\n')
			i = getc(f->f);
	}

	if (i != EOF && i != '\n' && i != '\r')
		byteBuffer[bytesRead++] = i;
	if (i != EOF && i != '\n' && i != '\r')
		while (bytesRead < bufsize && (i = getc(f->f)) != EOF && i != '\n' && i != '\r')
			byteBuffer[bytesRead++] = i;
	
	if (i == EOF && errno != EINTR && last == first)
		return false;

	if (i != EOF && i != '\n' && i != '\r') {
		fprintf (stderr, "! Unable to read an entire line---bufsize=%u.\n",
						 (unsigned) bufsize);
		fputs ("Please increase buf_size in texmf.cnf.\n", stderr);
		uexit (1);
	}

	/* If line ended with CR, remember to skip following LF. */
	if (i == '\r')
		f->skipNextLF = 1;
	
	/* now apply the mapping to turn external bytes into Unicode characters in buffer */
	cnv = (UConverter*)(f->conversionData);
	outLen = ucnv_toUChars(cnv, &buffer[first], (bufsize - first), byteBuffer, bytesRead, &errorCode);
	if (errorCode != 0) {
		fprintf(stderr, "! Unicode conversion failed: error code = %d\n", (int)errorCode);
		return false;
	}
	last = first + outLen;
	buffer[last] = ' ';

	if (last >= maxbufstack)
		maxbufstack = last;

	/* Trim trailing whitespace.  */
	while (last > first && ISBLANK(buffer[last - 1]))
		--last;
	
	return true;
}

static void die(char*s, int i)
{
	fprintf(stderr, s, i);
	fprintf(stderr, " - exiting\n");
	exit(3);
}

static UBreakIterator*	brkIter = NULL;
static int				brkLocaleStrNum = 0;

void
linebreakstart(int localeStrNum, const UniChar* text, int textLength)
{
	UErrorCode	status = 0;
	int i;

	if ((localeStrNum != brkLocaleStrNum) && (brkIter != NULL)) {
		ubrk_close(brkIter);
		brkIter = NULL;
	}
	
	if (brkIter == NULL) {
		char* locale = (char*)gettexstring(localeStrNum);
		brkIter = ubrk_open(UBRK_LINE, locale, NULL, 0, &status);
		if (U_FAILURE(status)) {
			fprintf(stderr, "\n! error %d creating linebreak iterator for locale \"%s\", trying default. ", status, locale);
			if (brkIter != NULL)
				ubrk_close(brkIter);
			status = 0;
			brkIter = ubrk_open(UBRK_LINE, "en_us", NULL, 0, &status);
		}
		free(locale);
		brkLocaleStrNum = localeStrNum;
	}
	
	if (brkIter == NULL) {
		die("! failed to create linebreak iterator, status=%d", (int)status);
	}

	ubrk_setText(brkIter, text, textLength, &status);
}

int
linebreaknext()
{
	return ubrk_next((UBreakIterator*)brkIter);
}

long
getencodingmodeandinfo(long* info)
{
	/* \XeTeXinputencoding "enc-name"
	 *   -> name is packed in |nameoffile| as a C string, starting at [1]
	 * Check if it's a built-in name; if not, try to open an ICU converter by that name
	 */
	UErrorCode	err = 0;
	UConverter*	cnv;
	char*	name = (char*)nameoffile + 1;
	*info = 0;
	if (strcasecmp(name, "auto") == 0) {
		return AUTO;
	}
	if (strcasecmp(name, "utf8") == 0) {
		return UTF8;
	}
	if (strcasecmp(name, "utf16") == 0) {	/* depends on host platform */
#ifdef WORDS_BIGENDIAN
		return UTF16BE;
#else
		return UTF16LE;
#endif
	}
	if (strcasecmp(name, "utf16be") == 0) {
		return UTF16BE;
	}
	if (strcasecmp(name, "utf16le") == 0) {
		return UTF16LE;
	}
	if (strcasecmp(name, "bytes") == 0) {
		return RAW;
	}
	
	/* try for an ICU converter */
	cnv = ucnv_open(name, &err);
	if (cnv == NULL) {
		fprintf(stderr, "! unknown encoding \"%s\"; reading as raw bytes\n", name);
		return RAW;
	}
	else {
		ucnv_close(cnv);
		*info = maketexstring(name);
		return ICUMAPPING;
	}
}

void
printutf8str(const unsigned char* str, int len)
{
	while (len-- > 0)
		printvisiblechar(*(str++)); /* bypass utf-8 encoding done in print_char() */
}

void
printchars(const unsigned short* str, int len)
{
	while (len-- > 0)
		printchar(*(str++));
}

void*
load_mapping_file(const char* s, const char* e)
{
	char*	mapPath;
	TECkit_Converter	cnv = 0;
	char*	buffer = xmalloc(e - s + 5);
	strncpy(buffer, s, e - s);
	buffer[e - s] = 0;
	strcat(buffer, ".tec");
	mapPath = kpse_find_file(buffer, kpse_miscfonts_format, 1);

	if (mapPath) {
		FILE*	mapFile = fopen(mapPath, "rb");
		free(mapPath);
		if (mapFile) {
			UInt32	mappingSize;
			Byte*	mapping;
			TECkit_Status	status;
			fseek(mapFile, 0, SEEK_END);
			mappingSize = ftell(mapFile);
			fseek(mapFile, 0, SEEK_SET);
			mapping = xmalloc(mappingSize);
			fread(mapping, 1, mappingSize, mapFile);
			fclose(mapFile);
			status = TECkit_CreateConverter(mapping, mappingSize,
											true,
#ifdef WORDS_BIGENDIAN
											kForm_UTF16BE, kForm_UTF16BE,
#else
											kForm_UTF16LE, kForm_UTF16LE,
#endif
											&cnv);
			free(mapping);
		}
	}
	else
		fontmappingwarning(buffer, strlen(buffer));

	free(buffer);

	return cnv;
}

static UInt32
read_tag(const char* cp)
{
	UInt32	tag = 0;
	int i;
	for (i = 0; i < 4; ++i) {
		tag <<= 8;
		if (*cp && /* *cp < 128 && */ *cp != ',' && *cp != ';' && *cp != ':') {
			tag += *(unsigned char*)cp;
			++cp;
		}
		else
			tag += ' ';
	}
	return tag;
}

static UInt32
read_tag_with_param(const char* cp, SInt32* param)
{
	UInt32	tag = 0;
	int i;
	for (i = 0; i < 4; ++i) {
		tag <<= 8;
		if (*cp && /* *cp < 128 && */ *cp != ',' && *cp != ';' && *cp != ':') {
			tag += *(unsigned char*)cp;
			++cp;
		}
		else
			tag += ' ';
	}
	if (*cp == '=') {
		int	neg = 0;
		++cp;
		if (*cp == '-') {
			++neg;
			++cp;
		}
		while (*cp >= '0' && *cp <= '9') {
			*param = *param * 10 + *cp - '0';
			++cp;
		}
		if (neg)
			*param = -(*param);
	}
	return tag;
}

static void*
loadOTfont(PlatformFontRef fontRef, XeTeXFont font, Fixed scaled_size, const char* cp1)
{
	XeTeXLayoutEngine   engine;
	UInt32	scriptTag = kLatin;
	UInt32	languageTag = 0;
	
	UInt32*	addFeatures = 0;
	UInt32*	removeFeatures = 0;
	SInt32* addParams = 0;
	
	int	nAdded = 0;
	int nRemoved = 0;
	
	const char*	cp2;
	const char*	cp3;

	UInt32	tag;

	UInt32	rgbValue = 0x000000FF;

	/* scan the feature string (if any) */
	if (cp1 != NULL) {
		while (*cp1) {
			if ((*cp1 == ':') || (*cp1 == ';') || (*cp1 == ','))
				++cp1;
			while ((*cp1 == ' ') || (*cp1 == '\t'))	/* skip leading whitespace */
				++cp1;
			if (*cp1 == 0)	/* break if end of string */
				break;
	
			cp2 = cp1;
			while (*cp2 && (*cp2 != ':') && (*cp2 != ';') && (*cp2 != ','))
				++cp2;
			
			if (strncmp(cp1, "script", 6) == 0) {
				cp3 = cp1 + 6;
				if (*cp3 != '=')
					goto bad_option;
				scriptTag = read_tag(cp3 + 1);
				goto next_option;
			}
			
			if (strncmp(cp1, "language", 8) == 0) {
				cp3 = cp1 + 8;
				if (*cp3 != '=')
					goto bad_option;
				languageTag = read_tag(cp3 + 1);
				goto next_option;
			}
			
			if (strncmp(cp1, "mapping", 7) == 0) {
				cp3 = cp1 + 7;
				if (*cp3 != '=')
					goto bad_option;
				loadedfontmapping = load_mapping_file(cp3 + 1, cp2);
				goto next_option;
			}
	
			if (strncmp(cp1, "color", 5) == 0) {
				unsigned	alpha = 0;
				int i;
				cp3 = cp1 + 5;
				if (*cp3 != '=')
					goto bad_option;
				++cp3;

				rgbValue = 0;
				for (i = 0; i < 6; ++i) {
					if ((*cp3 >= '0') && (*cp3 <= '9'))
						rgbValue = (rgbValue << 4) + *cp3 - '0';
					else if ((*cp3 >= 'A') && (*cp3 <= 'F'))
						rgbValue = (rgbValue << 4) + *cp3 - 'A' + 10;
					else if ((*cp3 >= 'a') && (*cp3 <= 'f'))
						rgbValue = (rgbValue << 4) + *cp3 - 'a' + 10;
					else
						goto bad_option;
					++cp3;
				}
				rgbValue <<= 8;
				for (i = 0; i < 2; ++i) {
					if ((*cp3 >= '0') && (*cp3 <= '9'))
						alpha = (alpha << 4) + *cp3 - '0';
					else if ((*cp3 >= 'A') && (*cp3 <= 'F'))
						alpha = (alpha << 4) + *cp3 - 'A' + 10;
					else if ((*cp3 >= 'a') && (*cp3 <= 'f'))
						alpha = (alpha << 4) + *cp3 - 'a' + 10;
					else
						break;
					++cp3;
				}
				if (i == 2)
					rgbValue += alpha;
				else
					rgbValue += 0xFF;

				loadedfontflags |= FONT_FLAGS_COLORED;

				goto next_option;
			}

			if (strncmp(cp1, "letterspace", 11) == 0) {
				int		sign = 1;
				double	val = 0.0;

				cp3 = cp1 + 11;
				if (*cp3 != '=')
					goto bad_option;
				++cp3;

				if (*cp3 == '-') {
					sign = -1;
					++cp3;
				}
				else if (*cp3 == '+') {
					++cp3;
				}

				while (*cp3 >= '0' && *cp3 <= '9') {
					val = val * 10.0 + *cp3 - '0';
					++cp3;
				}
				if (*cp3 == '.' || *cp3 == ',') {
					double	dec = 10.0;
					++cp3;
					while (*cp3 >= '0' && *cp3 <= '9') {
						val = val + (*cp3 - '0') / dec;
						++cp3;
						dec = dec * 10.0;
					}
				}
				
				loadedfontletterspace = sign * (val / 100.0) * scaled_size;
				
				goto next_option;
			}
			
			if (*cp1 == '+') {
				SInt32	param = 0;
				tag = read_tag_with_param(cp1 + 1, &param);
				++nAdded;
				if (nAdded == 1) {
					addFeatures = xmalloc(sizeof(UInt32));
					addParams = xmalloc(sizeof(SInt32));
				}
				else {
					addFeatures = xrealloc(addFeatures, nAdded * sizeof(UInt32));
					addParams = xrealloc(addParams, nAdded * sizeof(SInt32));
				}
				addFeatures[nAdded-1] = tag;
				addParams[nAdded-1] = param;
				goto next_option;
			}
			
			if (*cp1 == '-') {
				tag = read_tag(cp1 + 1);
				++nRemoved;
				if (nRemoved == 1)
					removeFeatures = xmalloc(sizeof(UInt32));
				else
					removeFeatures = xrealloc(removeFeatures, nRemoved * sizeof(UInt32));
				removeFeatures[nRemoved-1] = tag;
				goto next_option;
			}

			if (strncmp(cp1, "vertical", 8) == 0) {
				cp3 = cp2;
				if (*cp3 == ';' || *cp3 == ':')
					--cp3;
				while (*cp3 == ' ' || *cp3 == '\t')
					--cp3;
				if (*cp3)
					++cp3;
				if (cp3 == cp1 + 8) {
					loadedfontflags |= FONT_FLAGS_VERTICAL;
					goto next_option;
				}
			}
			
		bad_option:
			fontfeaturewarning(cp1, cp2 - cp1, 0, 0);
		
		next_option:
			cp1 = cp2;
		}
		
		if (addFeatures != 0) {
			addFeatures = realloc(addFeatures, (nAdded + 1) * sizeof(UInt32));
			addFeatures[nAdded] = 0;
		}
		if (removeFeatures != 0) {
			removeFeatures = realloc(removeFeatures, (nRemoved + 1) * sizeof(UInt32));
			removeFeatures[nRemoved] = 0;
		}
	}
	
	if ((loadedfontflags & FONT_FLAGS_COLORED) == 0)
		rgbValue = 0x000000FF;

	if ((loadedfontflags & FONT_FLAGS_VERTICAL) != 0)
		setFontLayoutDir(font, 1);

	engine = createLayoutEngine(fontRef, font, scriptTag, languageTag,
					addFeatures, addParams, removeFeatures, rgbValue);
	if (engine == 0) {
		deleteFont(font);
		if (addFeatures)
			free(addFeatures);
		if (addParams)
			free(addParams);
		if (removeFeatures)
			free(removeFeatures);
	}
	else
		nativefonttypeflag = OT_FONT_FLAG;

	return engine;
}

static void
splitFontName(char* name, char** var, char** feat, char** end)
{
	*var = NULL;
	*feat = NULL;
	if (*name == '[') {
		int	withinFileName = 1;
#ifdef WIN32
		char* start = name + 1;
#endif
		++name;
		while (*name) {
			if (withinFileName && *name == ']') {
				withinFileName = 0;
				if (*var == NULL)
					*var = name;
			}
			else if (*name == ':') {
				if (withinFileName && *var == NULL
#ifdef WIN32
					&& !((name - start == 1) && isalpha(*start))
#endif
					)
					*var = name;
				else if (!withinFileName && *feat == NULL)
					*feat = name;
			}
			++name;
		}
		*end = name;
	}
	else {
		while (*name) {
			if (*name == '/' && *var == NULL && *feat == NULL)
				*var = name;
			else if (*name == ':' && *feat == NULL)
				*feat = name;
			++name;
		}
		*end = name;
	}
	if (*feat == NULL)
		*feat = name;
	if (*var == NULL)
		*var = *feat;
}

void*
findnativefont(unsigned char* uname, long scaled_size)
	/* scaled_size here is in TeX points */
{
	void*	rval = 0;
	char*	nameString;
	char*	var;
	char*	feat;
	char*	end;
	char*	name = (char*)uname;
	char*	varString = NULL;
	char*	featString = NULL;
	PlatformFontRef	fontRef;
	XeTeXFont	font;

	loadedfontmapping = NULL;
	loadedfontflags = 0;
	loadedfontletterspace = 0;

	splitFontName(name, &var, &feat, &end);

	nameString = xmalloc(var - name + 1);
	strncpy(nameString, name, var - name);
	nameString[var - name] = 0;

	if (feat > var) {
		varString = xmalloc(feat - var);
		strncpy(varString, var + 1, feat - var - 1);
		varString[feat - var - 1] = 0;
	}
		
	if (end > feat) {
		featString = xmalloc(end - feat);
		strncpy(featString, feat + 1, end - feat - 1);
		featString[end - feat - 1] = 0;
	}
	
	// check for "[filename]" form, don't search maps in this case
	if (nameString[0] == '[') {
		char* path = kpse_find_file(nameString + 1, kpse_opentype_format, 0);
		if (path == NULL)
			path = kpse_find_file(nameString + 1, kpse_truetype_format, 0);
		if (path == NULL)
			path = kpse_find_file(nameString + 1, kpse_type1_format, 0);
		if (path != NULL) {
			int index = 0;
			if (varString != NULL) {
				char* cp;
				for (cp = varString; *cp && isdigit(*cp); ++cp)
					index = index * 10 + *cp - '0';
			}
			font = createFontFromFile(path, index, scaled_size);
			if (font != NULL) {
				rval = loadOTfont(0, font, scaled_size, featString);
				if (rval == 0)
					deleteFont(font);
			}
		}
	}
	else {
		fontRef = findFontByName(nameString, varString, Fix2X(scaled_size));
	
		if (fontRef != 0) {
			/* update nameoffile to the full name of the font, for error messages during font loading */
			const char*	fullName = getFullName(fontRef);
			namelength = strlen(fullName);
			if (featString != NULL)
				namelength += strlen(featString) + 1;
			if (varString != NULL)
				namelength += strlen(varString) + 1;
			free(nameoffile);
			nameoffile = xmalloc(namelength + 4); /* +2 would be correct: initial space, final NUL */
			nameoffile[0] = ' ';
			strcpy((char*)nameoffile + 1, fullName);
	
#ifdef XETEX_MAC
			/* decide whether to use AAT or OpenType rendering with this font */
			if (getReqEngine() == 'A')
				goto load_aat;
#endif
	
			font = createFont(fontRef, scaled_size);
			if (font != 0) {
#ifdef XETEX_MAC
				if (getReqEngine() == 'I' || getFontTablePtr(font, kGSUB) != 0 || getFontTablePtr(font, kGPOS) != 0)
#endif
					rval = loadOTfont(fontRef, font, scaled_size, featString);
				if (rval == 0)
					deleteFont(font);
			}
	
#ifdef XETEX_MAC
			if (rval == 0) {
			load_aat:
				rval = loadAATfont(fontRef, scaled_size, featString);
			}
#endif
			/* append the style and feature strings, so that \show\fontID will give a full result */
			if (varString != NULL && *varString != 0) {
				strcat((char*)nameoffile + 1, "/");
				strcat((char*)nameoffile + 1, varString);
			}
			if (featString != NULL && *featString != 0) {
				strcat((char*)nameoffile + 1, ":");
				strcat((char*)nameoffile + 1, featString);
			}
			namelength = strlen((char*)nameoffile + 1);
		}
	}
	
	if (varString != NULL)
		free(varString);

	if (featString != NULL)
		free(featString);

	free(nameString);
	
	return rval;
}

void
releasefontengine(void* engine, int type_flag)
{
#ifdef XETEX_MAC
	if (type_flag == AAT_FONT_FLAG) {
		ATSUDisposeStyle((ATSUStyle)engine);
	}
	else
#endif
	if (type_flag == OT_FONT_FLAG) {
		deleteLayoutEngine((XeTeXLayoutEngine)engine);
	}
}

/* params are given as 'integer' in the header file, but are really TeX scaled integers */
void
otgetfontmetrics(void* pEngine, scaled* ascent, scaled* descent, scaled* xheight, scaled* capheight, scaled* slant)
{
	XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)pEngine;
	long	rval = 0;
	float	a, d;
	int		glyphID;

	getAscentAndDescent(engine, &a, &d);
	*ascent = X2Fix(a);
	*descent = X2Fix(d);

	*slant = getSlant(getFont(engine));

	glyphID = mapCharToGlyph(engine, 'x');
	if (glyphID != 0) {
		getGlyphHeightDepth(engine, glyphID, &a, &d);
		*xheight = X2Fix(a);
	}
	else
		*xheight = *ascent / 2; /* arbitrary figure if there's no 'x' in the font */

	glyphID = mapCharToGlyph(engine, 'X');
	if (glyphID != 0) {
		getGlyphHeightDepth(engine, glyphID, &a, &d);
		*capheight = X2Fix(a);
	}
	else
		*capheight = *ascent; /* arbitrary figure if there's no 'X' in the font */
}

long
otfontget(int what, void* pEngine)
{
	XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)pEngine;
	XeTeXFont	fontInst = getFont(engine);
	switch (what) {
		case XeTeX_count_glyphs:
			return countGlyphs(fontInst);
			break;
			
		case XeTeX_OT_count_scripts:
			return countScripts(fontInst);
			break;
	}
	return 0;
}


long
otfontget1(int what, void* pEngine, long param)
{
	XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)pEngine;
	XeTeXFont	fontInst = getFont(engine);
	switch (what) {
		case XeTeX_OT_count_languages:
			return countScriptLanguages(fontInst, param);
			break;

		case XeTeX_OT_script_code:
			return getIndScript(fontInst, param);
			break;
	}
	return 0;
}


long
otfontget2(int what, void* pEngine, long param1, long param2)
{
	XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)pEngine;
	XeTeXFont	fontInst = getFont(engine);
	switch (what) {
		case XeTeX_OT_language_code:
			return getIndScriptLanguage(fontInst, param1, param2);
			break;

		case XeTeX_OT_count_features:
			return countFeatures(fontInst, param1, param2);
			break;
	}
	
	return 0;
}


long
otfontget3(int what, void* pEngine, long param1, long param2, long param3)
{
	XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)pEngine;
	XeTeXFont	fontInst = getFont(engine);
	switch (what) {
		case XeTeX_OT_feature_code:
			return getIndFeature(fontInst, param1, param2, param3);
			break;
	}
	
	return 0;
}

#define XDV_FLAG_FONTTYPE_ATSUI	0x0001
#define XDV_FLAG_FONTTYPE_ICU	0x0002

#define XDV_FLAG_VERTICAL		0x0100
#define XDV_FLAG_COLORED		0x0200
#define XDV_FLAG_FEATURES		0x0400
#define XDV_FLAG_VARIATIONS		0x0800

#ifdef XETEX_MAC
static UInt32
atsuColorToRGBA32(ATSURGBAlphaColor a)
{
	UInt32	rval = (UInt8)(a.red * 255.0 + 0.5);
	rval <<= 8;
	rval += (UInt8)(a.green * 255.0 + 0.5);
	rval <<= 8;
	rval += (UInt8)(a.blue * 255.0 + 0.5);
	rval <<= 8;
	rval += (UInt8)(a.alpha * 255.0 + 0.5);
	return rval;
}
#endif

static int	xdvBufSize = 0;

int
makeXDVGlyphArrayData(void* pNode)
{
	unsigned char*	cp;
	UInt16*		glyphIDs;
	memoryword* p = pNode;
	void*		glyph_info;
	FixedPoint*	locations;
	int			opcode;
	Fixed		wid;
	UInt16		glyphCount = native_glyph_count(p);
	
	int	i = glyphCount * native_glyph_info_size + 8; /* to guarantee enough space in the buffer */
	if (i > xdvBufSize) {
		if (xdvbuffer != NULL)
			free(xdvbuffer);
		xdvBufSize = ((i / 1024) + 1) * 1024;
		xdvbuffer = (char*)xmalloc(xdvBufSize);
	}

	glyph_info = (void*)native_glyph_info_ptr(p);
	locations = (FixedPoint*)glyph_info;
	glyphIDs = (UInt16*)(locations + glyphCount);
	
	opcode = XDV_GLYPH_STRING;
	for (i = 0; i < glyphCount; ++i)
		if (locations[i].y != 0) {
			opcode = XDV_GLYPH_ARRAY;
			break;
		}
	
	cp = (unsigned char*)xdvbuffer;
	*cp++ = opcode;
	
	wid = node_width(p);
	*cp++ = (wid >> 24) & 0xff;
	*cp++ = (wid >> 16) & 0xff;
	*cp++ = (wid >> 8) & 0xff;
	*cp++ = wid & 0xff;
	
	*cp++ = (glyphCount >> 8) & 0xff;
	*cp++ = glyphCount & 0xff;
	
	for (i = 0; i < glyphCount; ++i) {
		Fixed	x = locations[i].x;
		*cp++ = (x >> 24) & 0xff;
		*cp++ = (x >> 16) & 0xff;
		*cp++ = (x >> 8) & 0xff;
		*cp++ = x & 0xff;
		if (opcode == XDV_GLYPH_ARRAY) {
			Fixed	y = locations[i].y;
			*cp++ = (y >> 24) & 0xff;
			*cp++ = (y >> 16) & 0xff;
			*cp++ = (y >> 8) & 0xff;
			*cp++ = y & 0xff;
		}
	}

	for (i = 0; i < glyphCount; ++i) {
		UInt16	g = glyphIDs[i];
		*cp++ = (g >> 8) & 0xff;
		*cp++ = g & 0xff;
	}
	
	return ((char*)cp - xdvbuffer);
}

long
makefontdef(long f)
{
	UInt16	flags = 0;
	UInt32	variationCount = 0;
	UInt32	rgba;
	Fixed	size;
	const	char* psName;
	const	char* famName;
	const	char* styName;
	UInt8	psLen;
	UInt8	famLen;
	UInt8	styLen;
	int		fontDefLength;
	char*	cp;
	PlatformFontRef	fontRef = 0;

#ifdef XETEX_MAC
	ATSUStyle	style = NULL;
	if (fontarea[f] == AAT_FONT_FLAG) {
		flags = XDV_FLAG_FONTTYPE_ATSUI;

		style = (ATSUStyle)fontlayoutengine[f];
		ATSUGetAllFontVariations(style, 0, 0, 0, &variationCount);
		
		ATSUFontID	fontID;
		ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);

		fontRef = FMGetATSFontRefFromFont(fontID);
		getNames(fontRef, &psName, &famName, &styName);
			/* returns ptrs to strings that belong to the font - do not free! */

		ATSUVerticalCharacterType	vert;
		ATSUGetAttribute(style, kATSUVerticalCharacterTag, sizeof(ATSUVerticalCharacterType), &vert, 0);
		if (vert == kATSUStronglyVertical)
			flags |= XDV_FLAG_VERTICAL;
		
		ATSURGBAlphaColor	atsuColor;
		ATSUGetAttribute(style, kATSURGBAlphaColorTag, sizeof(ATSURGBAlphaColor), &atsuColor, 0);
		rgba = atsuColorToRGBA32(atsuColor);

		ATSUGetAttribute(style, kATSUSizeTag, sizeof(Fixed), &size, 0);
	}
	else
#endif
	if (fontarea[f] == OT_FONT_FLAG) {
		XeTeXLayoutEngine	engine;
		flags = XDV_FLAG_FONTTYPE_ICU;

		engine = (XeTeXLayoutEngine)fontlayoutengine[f];
		fontRef = getFontRef(engine);
		if (fontRef != 0)
			getNames(fontRef, &psName, &famName, &styName);
		else {
			psName = getFontFilename(engine);
			famName = "";
			styName = "";
		}

		rgba = getRgbValue(engine);
		if ((fontflags[f] & FONT_FLAGS_VERTICAL) != 0)
			flags |= XDV_FLAG_VERTICAL;

		size = X2Fix(getPointSize(engine));
	}
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}

	psLen = strlen(psName);
	famLen = strlen(famName);
	styLen = strlen(styName);

	/* parameters after internal font ID:
	//	size[4]
	//	flags[2]
	//	lp[1] lf[1] ls[1] ps[lp] fam[lf] sty[ls]
	//	if flags & COLORED:
	//		c[4]
	//	if flags & VARIATIONS:
	//		nv[2]
	//		a[4nv]
	//		v[4nv]
	*/

	fontDefLength
		= 4 /* size */
		+ 2	/* flags */
		+ 3	/* name length */
		+ psLen + famLen + styLen;

	if ((fontflags[f] & FONT_FLAGS_COLORED) != 0) {
		fontDefLength += 4; /* 32-bit RGBA value */
		flags |= XDV_FLAG_COLORED;
	}

#ifdef XETEX_MAC
	if (variationCount > 0) {
		fontDefLength +=
			  2	/* number of variations */
			+ 4 * variationCount
			+ 4 * variationCount;	/* axes and values */
		flags |= XDV_FLAG_VARIATIONS;
	}
#endif
	
	if (fontDefLength > xdvBufSize) {
		if (xdvbuffer != NULL)
			free(xdvbuffer);
		xdvBufSize = ((fontDefLength / 1024) + 1) * 1024;
		xdvbuffer = (char*)xmalloc(xdvBufSize);
	}
	cp = xdvbuffer;
	
	*(Fixed*)cp = SWAP32(size);
	cp += 4;
	
	*(UInt16*)cp = SWAP16(flags);
	cp += 2;
	
	*(UInt8*)cp = psLen;
	cp += 1;
	*(UInt8*)cp = famLen;
	cp += 1;
	*(UInt8*)cp = styLen;
	cp += 1;
	memcpy(cp, psName, psLen);
	cp += psLen;
	memcpy(cp, famName, famLen);
	cp += famLen;
	memcpy(cp, styName, styLen);
	cp += styLen;

	if ((fontflags[f] & FONT_FLAGS_COLORED) != 0) {
		*(UInt32*)cp = SWAP32(rgba);
		cp += 4;
	}
	
#ifdef XETEX_MAC
	if (variationCount > 0) {
		*(UInt16*)cp = SWAP16(variationCount);
		cp += 2;
		if (variationCount > 0) {
			ATSUGetAllFontVariations(style, variationCount,
									(UInt32*)(cp),
									(SInt32*)(cp + 4 * variationCount),
									0);
			while (variationCount > 0) {
				/* (potentially) swap two 32-bit values per variation setting,
				   and advance cp past them all */
				*(UInt32*)(cp) = SWAP32(*(UInt32*)(cp));
				cp += 4;
				*(UInt32*)(cp) = SWAP32(*(UInt32*)(cp));
				cp += 4;
				--variationCount;
			}
		}
	}
#endif
	
	return fontDefLength;
}

int
applymapping(void* pCnv, const UniChar* txtPtr, int txtLen)
{
	TECkit_Converter cnv = (TECkit_Converter)pCnv;
	UInt32	inUsed, outUsed;
	TECkit_Status	status;
	static UInt32	outLength = 0;

	/* allocate outBuffer if not big enough */
	if (outLength < txtLen * sizeof(UniChar) + 32) {
		if (mappedtext != 0)
			free(mappedtext);
		outLength = txtLen * sizeof(UniChar) + 32;
		mappedtext = xmalloc(outLength);
	}
	
	/* try the mapping */
retry:
	status = TECkit_ConvertBuffer(cnv,
			(Byte*)txtPtr, txtLen * sizeof(UniChar), &inUsed,
			(Byte*)mappedtext, outLength, &outUsed, true);
	
	switch (status) {
		case kStatus_NoError:
			txtPtr = (const UniChar*)mappedtext;
			return outUsed / sizeof(UniChar);
			
		case kStatus_OutputBufferFull:
			outLength += (txtLen * sizeof(UniChar)) + 32;
			free(mappedtext);
			mappedtext = xmalloc(outLength);
			goto retry;
			
		default:
			return 0;
	}
}

static void
snap_zone(scaled* value, scaled snap_value, scaled fuzz)
{
	scaled	difference = *value - snap_value;
	if (difference <= fuzz && difference >= -fuzz)
		*value = snap_value;
}

void
getnativecharheightdepth(int font, int ch, scaled* height, scaled* depth)
{
#define QUAD(f)			fontinfo[6+parambase[f]].cint
#define X_HEIGHT(f)		fontinfo[5+parambase[f]].cint
#define CAP_HEIGHT(f)	fontinfo[8+parambase[f]].cint

	float	ht = 0.0;
	float	dp = 0.0;
	Fixed	fuzz;

#ifdef XETEX_MAC
	if (fontarea[font] == AAT_FONT_FLAG) {
		ATSUStyle	style = (ATSUStyle)(fontlayoutengine[font]);
		int	gid = MapCharToGlyph_AAT(style, ch);
		GetGlyphHeightDepth_AAT(style, gid, &ht, &dp);
	}
	else
#endif
	if (fontarea[font] == OT_FONT_FLAG) {
		XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)fontlayoutengine[font];
		int	gid = mapCharToGlyph(engine, ch);
		getGlyphHeightDepth(engine, gid, &ht, &dp);
	}
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}

	*height = X2Fix(ht);
	*depth = X2Fix(dp);
	
	/* snap to "known" zones for baseline, x-height, cap-height if within 4% of em-size */
	fuzz = QUAD(font) / 25;
	snap_zone(depth, 0, fuzz);
	snap_zone(height, 0, fuzz);
	snap_zone(height, X_HEIGHT(font), fuzz);
	snap_zone(height, CAP_HEIGHT(font), fuzz);
}

scaled
getnativecharht(int f, int c)
{
	scaled	h, d;
	getnativecharheightdepth(f, c, &h, &d);
	return h;
}

scaled
getnativechardp(int f, int c)
{
	scaled	h, d;
	getnativecharheightdepth(f, c, &h, &d);
	return d;
}

void
getnativecharsidebearings(int font, int ch, scaled* lsb, scaled* rsb)
{
	float	l, r;

#ifdef XETEX_MAC
	if (fontarea[font] == AAT_FONT_FLAG) {
		ATSUStyle	style = (ATSUStyle)(fontlayoutengine[font]);
		int	gid = MapCharToGlyph_AAT(style, ch);
		GetGlyphSidebearings_AAT(style, gid, &l, &r);
	}
	else
#endif
	if (fontarea[font] == OT_FONT_FLAG) {
		XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)fontlayoutengine[font];
		int	gid = mapCharToGlyph(engine, ch);
		getGlyphSidebearings(engine, gid, &l, &r);
	}
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}

	*lsb = X2Fix(l);
	*rsb = X2Fix(r);
}

scaled
getnativecharic(int f, int c)
{
	scaled	lsb, rsb;
	getnativecharsidebearings(f, c, &lsb, &rsb);
	if (rsb < 0)
		return fontletterspace[f] - rsb;
	else
		return fontletterspace[f];
}

scaled
getnativecharwd(int f, int c)
{
	scaled wd = 0;
#ifdef XETEX_MAC
	if (fontarea[f] == AAT_FONT_FLAG) {
		ATSUStyle	style = (ATSUStyle)(fontlayoutengine[f]);
		int	gid = MapCharToGlyph_AAT(style, c);
		wd = X2Fix(GetGlyphWidth_AAT(style, gid));
	}
	else
#endif
	if (fontarea[f] == OT_FONT_FLAG) {
		XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)fontlayoutengine[f];
		int	gid = mapCharToGlyph(engine, c);
		wd = X2Fix(getGlyphWidthFromEngine(engine, gid));
	}
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}
	return wd;
}

UInt16
get_native_glyph_id(void* pNode, unsigned index)
{
	memoryword*	node = (memoryword*)pNode;
	FixedPoint*	locations = (FixedPoint*)native_glyph_info_ptr(node);
	UInt16*		glyphIDs = (UInt16*)(locations + native_glyph_count(node));
	if (index >= native_glyph_count(node))
		return 0;
	else
		return glyphIDs[index];
}

void
store_justified_native_glyphs(void* node)
{
#ifdef XETEX_MAC /* this is only called for fonts used via ATSUI */
	(void)DoAtsuiLayout(node, 1);
#endif
}

void
measure_native_node(void* pNode, int use_glyph_metrics)
{
	memoryword*		node = (memoryword*)pNode;
	int				txtLen = native_length(node);
	const UniChar*	txtPtr = (UniChar*)(node + native_node_size);

	unsigned		f = native_font(node);

#ifdef XETEX_MAC
	if (fontarea[f] == AAT_FONT_FLAG) {
		/* we're using this font in AAT mode, so fontlayoutengine[f] is actually an ATSUStyle */
		DoAtsuiLayout(node, 0);
	}
	else
#endif
	if (fontarea[f] == OT_FONT_FLAG) {
		/* using this font in OT Layout mode, so fontlayoutengine[f] is actually a XeTeXLayoutEngine */
		
		XeTeXLayoutEngine engine = (XeTeXLayoutEngine)(fontlayoutengine[f]);

		/* need to find direction runs within the text, and call layoutChars separately for each */

		int		nGlyphs;
		UBiDiDirection	dir;
		float	x, y;
		void*	glyph_info = 0;
		static	float*	positions = 0;
		static	UInt32*	glyphs = 0;
		static	long	maxGlyphs = 0;

		FixedPoint*	locations;
		UInt16*		glyphIDs;
		int			realGlyphCount = 0;

		UBiDi*	pBiDi = ubidi_open();
		
		UErrorCode	errorCode = (UErrorCode)0;
		ubidi_setPara(pBiDi, txtPtr, txtLen, getDefaultDirection(engine), NULL, &errorCode);
		
		dir = ubidi_getDirection(pBiDi);
		if (dir == UBIDI_MIXED) {
			/* we actually do the layout twice here, once to count glyphs and then again to get them;
			// which is inefficient, but i figure that MIXED is a relatively rare occurrence, so i can't be
			// bothered to deal with the memory reallocation headache of doing it differently
			*/
			int	nRuns = ubidi_countRuns(pBiDi, &errorCode);
			double		wid = 0;
			long		totalGlyphs = 0;
			int 		i, runIndex;
			int32_t		logicalStart, length;
			OSStatus	status = 0;
			for (runIndex = 0; runIndex < nRuns; ++runIndex) {
				dir = ubidi_getVisualRun(pBiDi, runIndex, &logicalStart, &length);
				nGlyphs = layoutChars(engine, (UniChar*)txtPtr, logicalStart, length, txtLen, (dir == UBIDI_RTL), 0.0, 0.0, &status);
				totalGlyphs += nGlyphs;

				if (nGlyphs >= maxGlyphs) {
					if (glyphs != 0) {
						free(glyphs);
						free(positions);
					}
					maxGlyphs = nGlyphs + 20;
					glyphs = xmalloc(maxGlyphs * sizeof(UInt32));
					positions = xmalloc((maxGlyphs * 2 + 2) * sizeof(float));
				}

				getGlyphs(engine, glyphs, &status);
				for (i = 0; i < nGlyphs; ++i)
					if (glyphs[i] < 0xfffe)
						++realGlyphCount;
			}
			
			if (realGlyphCount > 0) {
				double	x, y;
				glyph_info = xmalloc(realGlyphCount * native_glyph_info_size);
				locations = (FixedPoint*)glyph_info;
				glyphIDs = (UInt16*)(locations + realGlyphCount);
				realGlyphCount = 0;
				
				x = y = 0.0;
				for (runIndex = 0; runIndex < nRuns; ++runIndex) {
					dir = ubidi_getVisualRun(pBiDi, runIndex, &logicalStart, &length);
					nGlyphs = layoutChars(engine, (UniChar*)txtPtr, logicalStart, length, txtLen,
											(dir == UBIDI_RTL), x, y, &status);
	
					getGlyphs(engine, glyphs, &status);
					getGlyphPositions(engine, positions, &status);
				
					for (i = 0; i < nGlyphs; ++i) {
						if (glyphs[i] < 0xfffe) {
							glyphIDs[realGlyphCount] = glyphs[i];
							locations[realGlyphCount].x = X2Fix(positions[2*i]);
							locations[realGlyphCount].y = X2Fix(positions[2*i+1]);
							++realGlyphCount;
						}
					}
					x = positions[2*i];
					y = positions[2*i+1];
				}
				wid = x;
			}

			node_width(node) = X2Fix(wid);
			native_glyph_count(node) = realGlyphCount;
			native_glyph_info_ptr(node) = (long)glyph_info;
		}
		else {
			int i;
			OSStatus	status = 0;
			nGlyphs = layoutChars(engine, (UniChar*)txtPtr, 0, txtLen, txtLen, (dir == UBIDI_RTL), 0.0, 0.0, &status);
			getGlyphPosition(engine, nGlyphs, &x, &y, &status);
			node_width(node) = X2Fix(x);

			if (nGlyphs >= maxGlyphs) {
				if (glyphs != 0) {
					free(glyphs);
					free(positions);
				}
				maxGlyphs = nGlyphs + 20;
				glyphs = xmalloc(maxGlyphs * sizeof(UInt32));
				positions = xmalloc((maxGlyphs * 2 + 2) * sizeof(float));
			}
			getGlyphs(engine, glyphs, &status);
			getGlyphPositions(engine, positions, &status);

			for (i = 0; i < nGlyphs; ++i)
				if (glyphs[i] < 0xfffe)
					++realGlyphCount;

			if (realGlyphCount > 0) {
				glyph_info = xmalloc(realGlyphCount * native_glyph_info_size);
				locations = (FixedPoint*)glyph_info;
				glyphIDs = (UInt16*)(locations + realGlyphCount);
				realGlyphCount = 0;
				for (i = 0; i < nGlyphs; ++i) {
					if (glyphs[i] < 0xfffe) {
						glyphIDs[realGlyphCount] = glyphs[i];
						locations[realGlyphCount].x = X2Fix(positions[2*i]);
						locations[realGlyphCount].y = X2Fix(positions[2*i+1]);
						++realGlyphCount;
					}
				}
			}
						
			native_glyph_count(node) = realGlyphCount;
			native_glyph_info_ptr(node) = (long)glyph_info;
		}

		ubidi_close(pBiDi);
		
		if (fontletterspace[f] != 0) {
			Fixed	lsDelta = 0;
			Fixed	lsUnit = fontletterspace[f];
			int i;
			for (i = 0; i < realGlyphCount; ++i) {
				if (getGlyphWidth(getFont(engine), glyphIDs[i]) == 0 && lsDelta != 0)
					lsDelta -= lsUnit;
				locations[i].x += lsDelta;
				lsDelta += lsUnit;
			}
			if (lsDelta != 0) {
				lsDelta -= lsUnit;
				node_width(node) += lsDelta;
			}
		}
	}
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}
	
	if (use_glyph_metrics == 0 || native_glyph_count(node) == 0) {
		/* for efficiency, height and depth are the font's ascent/descent,
			not true values based on the actual content of the word,
			unless use_glyph_metrics is non-zero */
		node_height(node) = heightbase[f];
		node_depth(node) = depthbase[f];
	}
	else {
		/* this iterates over the glyph data whether it comes from ATSUI or ICU layout */
		FixedPoint*	locations = (FixedPoint*)native_glyph_info_ptr(node);
		UInt16*		glyphIDs = (UInt16*)(locations + native_glyph_count(node));
		float	yMin = 65536.0;
		float	yMax = -65536.0;
		int	i;
		for (i = 0; i < native_glyph_count(node); ++i) {
			float	ht, dp;
			float	y = Fix2X(-locations[i].y);	/* NB negative is upwards in locations[].y! */

			GlyphBBox	bbox;
			if (getCachedGlyphBBox(f, glyphIDs[i], &bbox) == 0) {
#ifdef XETEX_MAC
				if (fontarea[f] == AAT_FONT_FLAG)
					GetGlyphBBox_AAT((ATSUStyle)(fontlayoutengine[f]), glyphIDs[i], &bbox);
				else
#endif
				if (fontarea[f] == OT_FONT_FLAG)
					getGlyphBounds((XeTeXLayoutEngine)(fontlayoutengine[f]), glyphIDs[i], &bbox);
				
				cacheGlyphBBox(f, glyphIDs[i], &bbox);
			}

			ht = bbox.yMax;
			dp = -bbox.yMin;

			if (y + ht > yMax)
				yMax = y + ht;
			if (y - dp < yMin)
				yMin = y - dp;
		}
		node_height(node) = X2Fix(yMax);
		node_depth(node) = -X2Fix(yMin);
	}
}

Fixed
get_native_ital_corr(void* pNode)
{
	memoryword*	node = pNode;
	unsigned	f = native_font(node);
	unsigned	n = native_glyph_count(node);
	if (n > 0) {
		FixedPoint*	locations = (FixedPoint*)native_glyph_info_ptr(node);
		UInt16*		glyphIDs = (UInt16*)(locations + n);

#ifdef XETEX_MAC
		if (fontarea[f] == AAT_FONT_FLAG)
			return X2Fix(GetGlyphItalCorr_AAT((ATSUStyle)(fontlayoutengine[f]), glyphIDs[n-1]))
					+ fontletterspace[f];
#endif
		if (fontarea[f] == OT_FONT_FLAG)
			return X2Fix(getGlyphItalCorr((XeTeXLayoutEngine)(fontlayoutengine[f]), glyphIDs[n-1]))
					+ fontletterspace[f];
	}

	return 0;
}


Fixed
get_native_glyph_ital_corr(void* pNode)
{
	memoryword* node = pNode;
	UInt16		gid = native_glyph(node);
	unsigned	f = native_font(node);

#ifdef XETEX_MAC
	if (fontarea[f] == AAT_FONT_FLAG)
		return X2Fix(GetGlyphItalCorr_AAT((ATSUStyle)(fontlayoutengine[f]), gid));
#endif
	if (fontarea[f] == OT_FONT_FLAG)
		return X2Fix(getGlyphItalCorr((XeTeXLayoutEngine)(fontlayoutengine[f]), gid));

	return 0;	/* can't actually happen */
}

void
measure_native_glyph(void* pNode, int use_glyph_metrics)
{
	memoryword* node = pNode;
	UInt16		gid = native_glyph(node);
	unsigned	f = native_font(node);

	float	ht = 0.0;
	float	dp = 0.0;

#ifdef XETEX_MAC
	if (fontarea[f] == AAT_FONT_FLAG) {
		ATSUStyle	style = (ATSUStyle)(fontlayoutengine[f]);
		ATSGlyphIdealMetrics	metrics;
		OSStatus	status = ATSUGlyphGetIdealMetrics(style, 1, &gid, 0, &metrics);
			/* returns values in Quartz points, so we need to convert to TeX points */
		node_width(node) = X2Fix(metrics.advance.x * 72.27 / 72.0);
		if (use_glyph_metrics)
			GetGlyphHeightDepth_AAT(style, gid, &ht, &dp);
	}
	else
#endif
	if (fontarea[f] == OT_FONT_FLAG) {
		XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)fontlayoutengine[f];
		XeTeXFont		fontInst = getFont(engine);
		node_width(node) = X2Fix(getGlyphWidth(fontInst, gid));
		if (use_glyph_metrics)
			getGlyphHeightDepth(engine, gid, &ht, &dp);
	}
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}

	if (use_glyph_metrics) {
		node_height(node) = X2Fix(ht);
		node_depth(node) = X2Fix(dp);
	}
	else {
		node_height(node) = heightbase[f];
		node_depth(node) = depthbase[f];
	}
}

int
mapchartoglyph(int font, unsigned int ch)
{
	if (ch > 0x10ffff || ((ch >= 0xd800) && (ch <= 0xdfff)))
		return 0;
#ifdef XETEX_MAC
	if (fontarea[font] == AAT_FONT_FLAG)
		return MapCharToGlyph_AAT((ATSUStyle)(fontlayoutengine[font]), ch);
	else
#endif
	if (fontarea[font] == OT_FONT_FLAG)
		return mapCharToGlyph((XeTeXLayoutEngine)(fontlayoutengine[font]), ch);
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}
}

int
mapglyphtoindex(int font)
	/* glyph name is at nameoffile+1 */
{
#ifdef XETEX_MAC
	if (fontarea[font] == AAT_FONT_FLAG)
		return MapGlyphToIndex_AAT((ATSUStyle)(fontlayoutengine[font]), (const char*)nameoffile + 1);
	else
#endif
	if (fontarea[font] == OT_FONT_FLAG)
		return mapGlyphToIndex((XeTeXLayoutEngine)(fontlayoutengine[font]), (const char*)nameoffile + 1);
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}
}

int
getfontcharrange(int font, int first)
{
#ifdef XETEX_MAC
	if (fontarea[font] == AAT_FONT_FLAG)
		return GetFontCharRange_AAT((ATSUStyle)(fontlayoutengine[font]), first);
	else
#endif
	if (fontarea[font] == OT_FONT_FLAG)
		return getFontCharRange((XeTeXLayoutEngine)(fontlayoutengine[font]), first);
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}
}

#ifndef XETEX_MAC
Fixed X2Fix(double d)
{
	Fixed rval = (int)(d * 65536.0 + 0.5);
	return rval;
}

double Fix2X(Fixed f)
{
	double rval = f / 65536.0;
	return rval;
}
#endif

/* these are here, not XeTeX_mac.c, because we need stubs on other platforms */
#ifndef XETEX_MAC
typedef void* ATSUStyle; /* dummy declaration just so the stubs can compile */
#endif

void
atsugetfontmetrics(ATSUStyle style, Fixed* ascent, Fixed* descent, Fixed* xheight, Fixed* capheight, Fixed* slant)
{
#ifdef XETEX_MAC
	*ascent = *descent = *xheight = *capheight = *slant = 0;

	ATSUFontID	fontID;
	OSStatus	status = ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);
	if (status != noErr)
		return;

	ATSFontRef	fontRef = FMGetATSFontRefFromFont(fontID);

	Fixed		size;
	status = ATSUGetAttribute(style, kATSUSizeTag, sizeof(Fixed), &size, 0);
	if (status != noErr)
		return;
	/* size from the ATSUStyle is in Quartz points; convert to TeX points here */
	double		floatSize = Fix2X(size) * 72.27 / 72.0;

	ATSFontMetrics	metrics;
	status = ATSFontGetHorizontalMetrics(fontRef, kATSOptionFlagsDefault, &metrics);
	if (status != noErr)
		return;

	*ascent = X2Fix(metrics.ascent * floatSize);
	*descent = X2Fix(metrics.descent * floatSize);

	if (metrics.italicAngle != 0.0) {
		if (fabs(metrics.italicAngle) < 0.090)
			metrics.italicAngle *= 1000.0;	/* hack around apparent ATS bug */
		*slant = X2Fix(tan(-metrics.italicAngle * M_PI / 180.0));
	}
	else {
		/* try to get a (possibly synthetic) POST table, as ATSFontGetHorizontalMetrics
		   doesn't seem to return this value for OT/CFF fonts */
		ByteCount	tableSize;
		if (ATSFontGetTable(fontRef, LE_POST_TABLE_TAG, 0, 0, 0, &tableSize) == noErr) {
			POSTTable*      post = xmalloc(tableSize);
			ATSFontGetTable(fontRef, LE_POST_TABLE_TAG, 0, tableSize, post, 0);
			*slant = X2Fix(tan(Fix2X( - SWAP32(post->italicAngle)) * M_PI / 180.0));
			free(post);
		}
	}

	if (0 && metrics.xHeight != 0.0) {
		/* currently not using this, as the values from ATS don't seem quite what I'd expect */
		*xheight = X2Fix(metrics.xHeight * floatSize);
		*capheight = X2Fix(metrics.capHeight * floatSize);
	}
	else {
		int	glyphID = MapCharToGlyph_AAT(style, 'x');
		float	ht, dp;
		if (glyphID != 0) {
			GetGlyphHeightDepth_AAT(style, glyphID, &ht, &dp);
			*xheight = X2Fix(ht);
		}
		else
			*xheight = *ascent / 2; /* arbitrary figure if there's no 'x' in the font */
		
		glyphID = MapCharToGlyph_AAT(style, 'X');
		if (glyphID != 0) {
			GetGlyphHeightDepth_AAT(style, glyphID, &ht, &dp);
			*capheight = X2Fix(ht);
		}
		else
			*capheight = *ascent; /* arbitrary figure if there's no 'X' in the font */
	}
#endif
}

long
atsufontget(int what, ATSUStyle style)
{
	long	rval = -1;

#ifdef XETEX_MAC
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);
	ItemCount	count;

	switch (what) {
		case XeTeX_count_glyphs:
			{
				ByteCount	tableSize;
				ATSFontRef	fontRef = FMGetATSFontRefFromFont(fontID);
				if (ATSFontGetTable(fontRef, LE_MAXP_TABLE_TAG, 0, 0, 0, &tableSize) == noErr) {
					MAXPTable*	table = xmalloc(tableSize);
					ATSFontGetTable(fontRef, LE_MAXP_TABLE_TAG, 0, tableSize, table, 0);
					rval = SWAP16(table->numGlyphs);
					free(table);
				}
			}
			break;

		case XeTeX_count_variations:
			if (ATSUCountFontVariations(fontID, &count) == noErr)
				rval = count;
			break;

		case XeTeX_count_features:
			if (ATSUCountFontFeatureTypes(fontID, &count) == noErr)
				rval = count;
			break;
	}
#endif
	return rval;
}

long
atsufontget1(int what, ATSUStyle style, int param)
{
	long	rval = -1;

#ifdef XETEX_MAC
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);
	
	ATSUFontVariationAxis	axis;
	ATSUFontVariationValue	value;
	ItemCount	count;
	Boolean		exclusive;
	switch (what) {
		case XeTeX_variation:
			if (ATSUGetIndFontVariation(fontID, param, &axis, 0, 0, 0) == noErr)
				rval = axis;
			break;

		case XeTeX_variation_min:
		case XeTeX_variation_max:
		case XeTeX_variation_default:
			if (ATSUCountFontVariations(fontID, &count) == noErr)
				while (count-- > 0)
					if (ATSUGetIndFontVariation(fontID, count, &axis,
							(what == XeTeX_variation_min) ? &value : 0,
							(what == XeTeX_variation_max) ? &value : 0,
							(what == XeTeX_variation_default) ? &value : 0) == noErr)
						if (axis == param) {
							rval = value;
							break;
						}
			break;

		case XeTeX_feature_code:
			if (ATSUCountFontFeatureTypes(fontID, &count) == noErr) {
				if (param < count) {
					ATSUFontFeatureType*	types = xmalloc(count * sizeof(ATSUFontFeatureType));
					if (ATSUGetFontFeatureTypes(fontID, count, types, 0) == noErr)
						rval = types[param];
					free(types);
				}
			}
			break;

		case XeTeX_is_exclusive_feature:
			if (ATSUGetFontFeatureSelectors(fontID, param, 0, 0, 0, 0, &exclusive) == noErr)
				rval = exclusive ? 1 : 0;
			break;

		case XeTeX_count_selectors:
			if (ATSUCountFontFeatureSelectors(fontID, param, &count) == noErr)
				rval = count;
			break;
	}
#endif
	
	return rval;
}

long
atsufontget2(int what, ATSUStyle style, int param1, int param2)
{
	long	rval = -1;

#ifdef XETEX_MAC
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);

	ItemCount	count;
	if (ATSUCountFontFeatureSelectors(fontID, param1, &count) == noErr) {
		ATSUFontFeatureSelector*	selectors = xmalloc(count * sizeof(ATSUFontFeatureSelector));
		Boolean*					isDefault = xmalloc(count * sizeof(Boolean));
		if (ATSUGetFontFeatureSelectors(fontID, param1, count, selectors, isDefault, 0, 0) == noErr) {
			switch (what) {
				case XeTeX_selector_code:
					if (param2 < count)
							rval = selectors[param2];
					break;
					
				case XeTeX_is_default_selector:
					while (count-- > 0)
						if (selectors[count] == param2) {
							rval = isDefault[count] ? 1 : 0;
							break;
						}
					break;
			}
		}
		free(isDefault);
		free(selectors);
	}
#endif
	
	return rval;
}

long
atsufontgetnamed(int what, ATSUStyle style)
{
	long	rval = -1;

#ifdef XETEX_MAC
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);
	
	switch (what) {
		case XeTeX_find_variation_by_name:
			rval = find_axis_by_name(fontID, nameoffile + 1, namelength);
			if (rval == 0)
				rval = -1;
			break;
		
		case XeTeX_find_feature_by_name:
			rval = find_feature_by_name(fontID, nameoffile + 1, namelength);
			if (rval == 0x0000FFFF)
				rval = -1;
			break;
	}
#endif
	
	return rval;
}

long
atsufontgetnamed1(int what, ATSUStyle style, int param)
{
	long	rval = -1;

#ifdef XETEX_MAC
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);
	
	switch (what) {
		case XeTeX_find_selector_by_name:
			rval = find_selector_by_name(fontID, param, nameoffile + 1, namelength);
			if (rval == 0x0000FFFF)
				rval = -1;
			break;
	}
#endif
	
	return rval;
}

void
atsuprintfontname(int what, ATSUStyle style, int param1, int param2)
{
#ifdef XETEX_MAC
	ATSUFontID	fontID;
	ATSUGetAttribute(style, kATSUFontTag, sizeof(ATSUFontID), &fontID, 0);

	FontNameCode	code;
	OSStatus		status = -1;
	ItemCount		count, i;
	Boolean			found = 0;

	switch (what) {
		case XeTeX_variation_name:
			status = ATSUCountFontVariations(fontID, &count);
			if (status == noErr) {
				for (i = 0; i < count; ++i) {
					ATSUFontVariationAxis	axis;
					ATSUFontVariationValue	min, max, def;
					status = ATSUGetIndFontVariation(fontID, i, &axis, &min, &max, &def);
					if (status == noErr && axis == param1) {
						status = ATSUGetFontVariationNameCode(fontID, param1, &code);
						found = 1;
						break;
					}
				}
			}
			break;
			
		case XeTeX_feature_name:
		case XeTeX_selector_name:
			status = ATSUCountFontFeatureTypes(fontID, &count);
			if (status == noErr) {
				ATSUFontFeatureType*	features = xmalloc(count * sizeof(ATSUFontFeatureType));
				status = ATSUGetFontFeatureTypes(fontID, count, features, &count);
				if (status == noErr) {
					for (i = 0; i < count; ++i) {
						if (features[i] == param1) {
							/* the requested feature code is valid */
							if (what == XeTeX_feature_name) {
								status = ATSUGetFontFeatureNameCode(fontID, param1, kATSUNoSelector, &code);
								found = 1;
							}
							else {
								status = ATSUCountFontFeatureSelectors(fontID, param1, &count);
								if (status == noErr) {
									ATSUFontFeatureSelector*	selectors = xmalloc(count * sizeof(ATSUFontFeatureSelector));
									Boolean*					onByDefault = xmalloc(count * sizeof(Boolean));
									Boolean						exclusive;
									status = ATSUGetFontFeatureSelectors(fontID, param1, count, selectors, onByDefault, &count, &exclusive);
									if (status == noErr) {
										for (i = 0; i < count; ++i) {
											if (selectors[i] == param2) {
												/* feature/selector combination is valid */
												status = ATSUGetFontFeatureNameCode(fontID, param1, param2, &code);
												found = 1;
												break;
											}
										}
									}
									free(onByDefault);
									free(selectors);
								}
							}
							break;
						}
					}
				}
				free(features);
			}
			break;
	}

	if (found && status == noErr) {
#define NAME_BUF_SIZE	1024
		ByteCount	len = 0;
		char		name[NAME_BUF_SIZE]; /* should be more than enough for any sensible font name */
		do {
			if (ATSUFindFontName(fontID, code, kFontMacintoshPlatform, kFontRomanScript, kFontEnglishLanguage, NAME_BUF_SIZE, name, &len, 0) == noErr) break;
			if (ATSUFindFontName(fontID, code, kFontMacintoshPlatform, kFontRomanScript, kFontNoLanguageCode, NAME_BUF_SIZE, name, &len, 0) == noErr) break;
		} while (0);
		if (len > 0) {
			/* need to convert MacRoman name to Unicode */
			CFStringRef	str = CFStringCreateWithBytes(kCFAllocatorDefault, (UInt8*)name, len, kCFStringEncodingMacRoman, false);
			if (str != NULL) {
				len = CFStringGetLength(str);
				UniChar*	buf = xmalloc(len * sizeof(UniChar));
				CFStringGetCharacters(str, CFRangeMake(0, len), buf);
				printchars(buf, len);
				free(buf);
				CFRelease(str);
			}
		}
		else {
			do {
				if (ATSUFindFontName(fontID, code, kFontUnicodePlatform, kFontNoScriptCode, kFontEnglishLanguage, NAME_BUF_SIZE, name, &len, 0) == noErr) break;
				if (ATSUFindFontName(fontID, code, kFontMicrosoftPlatform, kFontNoScriptCode, kFontEnglishLanguage, NAME_BUF_SIZE, name, &len, 0) == noErr) break;
				if (ATSUFindFontName(fontID, code, kFontUnicodePlatform, kFontNoScriptCode, kFontNoLanguageCode, NAME_BUF_SIZE, name, &len, 0) == noErr) break;
				if (ATSUFindFontName(fontID, code, kFontMicrosoftPlatform, kFontNoScriptCode, kFontNoLanguageCode, NAME_BUF_SIZE, name, &len, 0) == noErr) break;
			} while (0);
			if (len > 0) {
				printchars((unsigned short*)(&name[0]), len / 2);
			}
		}
	}
#endif
}

void
printglyphname(int font, int gid)
{
	char* s;
	int   len = 0;
#ifdef XETEX_MAC
	if (fontarea[font] == AAT_FONT_FLAG) {
		ATSUStyle	style = (ATSUStyle)(fontlayoutengine[font]);
		s = GetGlyphName_AAT(style, gid, &len);
	}
	else
#endif
	if (fontarea[font] == OT_FONT_FLAG) {
		XeTeXLayoutEngine	engine = (XeTeXLayoutEngine)fontlayoutengine[font];
		s = (char*)getGlyphName(getFont(engine), gid, &len);
	}
	else {
		fprintf(stderr, "\n! Internal error: bad native font flag\n");
		exit(3);
	}
	while (len-- > 0)
		printchar(*s++);
}

boolean
u_open_in(unicodefile* f, int filefmt, const_string fopen_mode, int mode, int encodingData)
{
	boolean	rval;
	*f = malloc(sizeof(UFILE));
	(*f)->encodingMode = 0;
	(*f)->conversionData = 0;
	(*f)->savedChar = -1;
	(*f)->skipNextLF = 0;
	rval = open_input (&((*f)->f), filefmt, fopen_mode);
	if (rval) {
		int	B1, B2;
		if (mode == AUTO) {
			/* sniff encoding form */
			B1 = getc((*f)->f);
			B2 = getc((*f)->f);
			if (B1 == 0xfe && B2 == 0xff)
				mode = UTF16BE;
			else if (B2 == 0xfe && B1 == 0xff)
				mode = UTF16LE;
			else if (B1 == 0 && B2 != 0) {
				mode = UTF16BE;
				fseek((*f)->f, SEEK_SET, 0);
			}
			else if (B2 == 0 && B1 != 0) {
				mode = UTF16LE;
				fseek((*f)->f, SEEK_SET, 0);
			}
			else if (B1 == 0xef && B2 == 0xbb) {
				int	B3 = getc((*f)->f);
				if (B3 == 0xbf)
					mode = UTF8;
			}
			if (mode == AUTO) {
				fseek((*f)->f, SEEK_SET, 0);
				mode = UTF8;
			}
		}

		setinputfileencoding(*f, mode, encodingData);
	}
	return rval;
}

boolean
open_dvi_output(FILE** fptr)
{
	if (nopdfoutput) {
		return open_output(fptr, FOPEN_WBIN_MODE);
	}
	else {
		const char *p = (const char*)nameoffile+1;
		char	*cmd, *q;
		int len = strlen(p);
		while (*p)
			if (*p++ == '\"')
				++len;
		len += strlen(outputdriver);
		len += 8; /* space for -o flag, quotes, NUL */
		cmd = xmalloc(len);
		strcpy(cmd, outputdriver);
		strcat(cmd, " -o \"");
		q = cmd + strlen(cmd);
		for (p = (const char*)nameoffile+1; *p; p++) {
			if (*p == '\"')
				*q++ = '\\';
			*q++ = *p;
		}
		*q++ = '\"';
		*q = '\0';
		if (papersize != 0) {
			char* cmd2 = concat3(cmd, " -p ", papersize);
			free(cmd);
			cmd = cmd2;
		}
		*fptr = popen(cmd, "w");
		free(cmd);
		return (*fptr != 0);
	}
}

void
dviclose(FILE* fptr)
{
	if (nopdfoutput)
		fclose(fptr);
	else
		pclose(fptr);
}

int
get_uni_c(UFILE* f)
{
	int	rval;

	if (f->savedChar != -1) {
		rval = f->savedChar;
		f->savedChar = -1;
		return rval;
	}

	switch (f->encodingMode) {
		case UTF8:
			/* FIXME: we don't currently check for malformed UTF-8 */
			rval = getc(f->f);
			if (rval != EOF) {
				UInt16 extraBytes = bytesFromUTF8[rval];
				switch (extraBytes) {	/* note: code falls through cases! */
					case 5: rval <<= 6;	rval += getc(f->f);
					case 4: rval <<= 6;	rval += getc(f->f);
					case 3: rval <<= 6;	rval += getc(f->f);
					case 2: rval <<= 6;	rval += getc(f->f);
					case 1: rval <<= 6;	rval += getc(f->f);
					case 0:	;
				};
				rval -= offsetsFromUTF8[extraBytes];
				if (rval > 0xFFFF) {
					rval -= 0x10000;
					f->savedChar = 0xdc00 + rval % 0x0400;
					rval = 0xd800 + rval / 0x0400;
				}
			}
			break;

		case UTF16BE:
			rval = getc(f->f);
			rval <<= 8;
			rval += getc(f->f);
			break;

		case UTF16LE:
			rval = getc(f->f);
			rval += (getc(f->f) << 8);
			break;

		case RAW:
			rval = getc(f->f);
			break;

		default:
			/* this can't happen */
			fprintf(stderr, "! Internal error---file input mode=%d.\n", f->encodingMode);
			uexit(3);
	}

	return rval;
}

boolean
input_line(UFILE* f)
{
	int i;
	
	if (f->encodingMode == ICUMAPPING)
		return input_line_icu(f);
	
	/* Recognize either LF or CR as a line terminator; skip initial LF if prev line ended with CR.  */
	i = get_uni_c(f);
	if (f->skipNextLF) {
		f->skipNextLF = 0;
		if (i == '\n')
			i = get_uni_c(f);
	}

	last = first;
	if (last < bufsize && i != EOF && i != '\n' && i != '\r')
		buffer[last++] = i;
	if (i != EOF && i != '\n' && i != '\r')
		while (last < bufsize && (i = get_uni_c(f)) != EOF && i != '\n' && i != '\r')
			buffer[last++] = i;
	
	if (i == EOF && errno != EINTR && last == first)
		return false;
	
	/* We didn't get the whole line because our buffer was too small.  */
	if (i != EOF && i != '\n' && i != '\r') {
		fprintf (stderr, "! Unable to read an entire line---bufsize=%u.\n",
						 (unsigned) bufsize);
		fputs ("Please increase buf_size in texmf.cnf.\n", stderr);
		uexit (1);
	}
	
	buffer[last] = ' ';
	if (last >= maxbufstack)
		maxbufstack = last;
	
	/* If line ended with CR, remember to skip following LF. */
	if (i == '\r')
		f->skipNextLF = 1;
	
	/* Trim trailing whitespace.  */
	while (last > first && ISBLANK (buffer[last - 1]))
		--last;
	
	return true;
}

void
makeutf16name()
{
	unsigned char* s = nameoffile + 1;
	UInt32	rval;
	UInt16*	t;
	static int name16len = 0;
	if (name16len <= namelength) {
		if (nameoffile16 != 0)
			free(nameoffile16);
		name16len = namelength + 10;
		nameoffile16 = xmalloc(name16len * sizeof(UInt16));
	}
	t = nameoffile16;
	while (s <= nameoffile + namelength) {
		UInt16 extraBytes;
		rval = *(s++);
		extraBytes = bytesFromUTF8[rval];
		switch (extraBytes) {	/* note: code falls through cases! */
			case 5: rval <<= 6;	if (*s) rval += *(s++);
			case 4: rval <<= 6;	if (*s) rval += *(s++);
			case 3: rval <<= 6;	if (*s) rval += *(s++);
			case 2: rval <<= 6;	if (*s) rval += *(s++);
			case 1: rval <<= 6;	if (*s) rval += *(s++);
			case 0:	;
		};
		rval -= offsetsFromUTF8[extraBytes];
		if (rval > 0xffff) {
			rval -= 0x10000;
			*(t++) = 0xd800 + rval / 0x0400;
			*(t++) = 0xdc00 + rval % 0x0400;
		}
		else
			*(t++) = rval;
	}
	namelength16 = t - nameoffile16;
}

