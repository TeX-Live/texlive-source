/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2005 by SIL International
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

// \special{} support for xdv2pdf

#define MAC_OS_X_VERSION_MIN_REQUIRED	1020

#include <ApplicationServices/ApplicationServices.h>

#include "xdv2pdf_globals.h"


struct dimenRec {
	const char*	name;
	double_t	factor;	// multiplier to convert to BP
};

static dimenRec sDimenRatios[] =
{
	{ "in", 72.0 },
	{ "mm", 72.0 / 25.4 },
	{ "cm", 72.0 / 2.54 },
	{ "bp", 1.0 },
	{ "pt", 72.0 / 72.27 },
	{ "pc", 12.0 * 72.0 / 72.27 },
	{ "dd", 1238.0 * 72.0 / 72.27 / 1157.0 },
	{ "cc", 14856.0 * 72.0 / 72.27 / 1157.0 },
	{ 0, 0 }
};

static int
readHexDigit(const char*& cp)
{
    if (*cp >= '0' && *cp <= '9')
        return *cp++ - '0';
    if (*cp >= 'A' && *cp <= 'F')
        return *cp++ - 'A' + 10;
    if (*cp >= 'a' && *cp <= 'f')
        return *cp++ - 'a' + 10;
    return 0;
}

static CGColorRef
readColorValue(const char* s)
{
	float components[4];
    int	x;
    x = readHexDigit(s);
    x <<= 4;
    x += readHexDigit(s);
    components[0] = x / 255.0;
    x = readHexDigit(s);
    x <<= 4;
    x += readHexDigit(s);
	components[1] = x / 255.0;
    x = readHexDigit(s);
    x <<= 4;
    x += readHexDigit(s);
	components[2] = x / 255.0;
	components[3] = 1.0;
	return CGColorCreate(gRGBColorSpace, components);
}

static CGColorRef
readColorValueWithAlpha(const char* s)
{
	float components[4];
    int	x;
    x = readHexDigit(s);
    x <<= 4;
    x += readHexDigit(s);
    components[0] = x / 255.0;
    x = readHexDigit(s);
    x <<= 4;
    x += readHexDigit(s);
	components[1] = x / 255.0;
    x = readHexDigit(s);
    x <<= 4;
    x += readHexDigit(s);
	components[2] = x / 255.0;
    x = readHexDigit(s);
    x <<= 4;
    x += readHexDigit(s);
	components[3] = x / 255.0;
	return CGColorCreate(gRGBColorSpace, components);
}

static float
readFloat(const char*& cp)
{
	float	result = 0.0;
	bool	negate = false;
	if (*cp == '-') {
		negate = true;
		++cp;
	}
	while (*cp >= '0' && *cp <= '9')
		result = result * 10 + *cp++ - '0';
	if (*cp == '.') {
		++cp;
		float	dec = 1.0;
		while (*cp >= '0' && *cp <= '9')
			result = result + (dec /= 10.0) * (*cp++ - '0');
	}
	if (negate)
		result = -result;
	return result;
}

void
doPDFspecial(const char* special)
{
	ensurePageStarted();

	if (gPdfMarkFile == NULL) {
		static bool firstTime = true;
		if (firstTime) {
			firstTime = false;
			CFURLRef	markURL = CFURLCreateCopyAppendingPathExtension(kCFAllocatorDefault, gSaveURL, CFSTR("marks"));
			Boolean	gotPath = CFURLGetFileSystemRepresentation(markURL, true, (UInt8*)gPdfMarkPath, _POSIX_PATH_MAX);
			if (gotPath) {
				gPdfMarkFile = fopen(gPdfMarkPath, "w");
				if (gPdfMarkFile == NULL)
					fprintf(stderr, "*** unable to write to PDFmark file \"%s\"\n", gPdfMarkPath);
			}
			else
				fprintf(stderr, "*** unable to get pathname for PDFmark file\n");
		}
	}
	if (gPdfMarkFile != NULL) {
		if (strncmp(special, "bann", 4) == 0) {
			gTagLevel = gDviLevel;
			initAnnotBox();
			gTrackingBoxes = true;
		}
		else if (strncmp(special, "eann", 4) == 0) {
			flushAnnotBox();
			gTrackingBoxes = false;
			gTagLevel = -1;
		}
		fprintf(gPdfMarkFile, "%d\t%f\t%f\t%s\n",
			gPageIndex, kDvi2Scr * dvi.h + 72.0, kDvi2Scr * (gPageHt - dvi.v) - 72.0, special);
	}
}

static void
pushTextColor()
{
	gTextColorStack.push_back(gTextColor);
}

static void
popTextColor()
{
	if (gTextColorStack.size() == 0) {
		fprintf(stderr, "\n*** text color stack underflow\n");
	}
	else {
		gTextColor = gTextColorStack.back();
		gTextColorStack.pop_back();
	}
}

static void
pushRuleColor()
{
	gRuleColorStack.push_back(gRuleColor);
}

static void
popRuleColor()
{
	if (gRuleColorStack.size() == 0) {
		fprintf(stderr, "\n*** rule color stack underflow\n");
	}
	else {
		gRuleColor = gRuleColorStack.back();
		gRuleColorStack.pop_back();
	}
}

static void
setRuleColor(CGColorRef color)
{
	gRuleColor = color;
}

static void
resetRuleColor()
{
	gRuleColor = kBlackColor;
}

static void
setTextColor(CGColorRef color)
{
	gTextColor = color;
}

static void
resetTextColor()
{
	gTextColor = kBlackColor;
}

static void
setBackground(CGColorRef bg)
{
	gBackground = bg;
	paintBackground();
}

static bool
readColorValues(const char*& s, int n, float* v)
{
	while (n-- > 0) {
		while (*s == ' ')
			++s;
		if ( !( ( (*s >= '0') && (*s <= '9') )		// unless we have a digit
				|| (*s == '.') || (*s == ',') ) )	// or decimal sign, break
			return false;
		float val = 0.0;
		while ((*s >= '0') && (*s <= '9')) {
			val = val * 10.0 + *s - '0';
			++s;
		}
		if ((*s == '.') || (*s == ',')) {
			++s;
			float	dec = 10.0;
			while ((*s >= '0') && (*s <= '9')) {
				val += (*s - '0') / dec;
				dec *= 10;
				++s;
			}
		}
		*v++ = val;
	}
	return true;
}

struct namedColor {
  char *name;
  float cmyk[4];
} colorsByName[] = {
  {"GreenYellow", {0.15, 0, 0.69, 0}},
  {"Yellow", {0, 0, 1, 0}},
  {"Goldenrod", {0, 0.10, 0.84, 0}},
  {"Dandelion", {0, 0.29, 0.84, 0}},
  {"Apricot", {0, 0.32, 0.52, 0}},
  {"Peach", {0, 0.50, 0.70, 0}},
  {"Melon", {0, 0.46, 0.50, 0}},
  {"YellowOrange", {0, 0.42, 1, 0}},
  {"Orange", {0, 0.61, 0.87, 0}},
  {"BurntOrange", {0, 0.51, 1, 0}},
  {"Bittersweet", {0, 0.75, 1, 0.24}},
  {"RedOrange", {0, 0.77, 0.87, 0}},
  {"Mahogany", {0, 0.85, 0.87, 0.35}},
  {"Maroon", {0, 0.87, 0.68, 0.32}},
  {"BrickRed", {0, 0.89, 0.94, 0.28}},
  {"Red", {0, 1, 1, 0}},
  {"OrangeRed", {0, 1, 0.50, 0}},
  {"RubineRed", {0, 1, 0.13, 0}},
  {"WildStrawberry", {0, 0.96, 0.39, 0}},
  {"Salmon", {0, 0.53, 0.38, 0}},
  {"CarnationPink", {0, 0.63, 0, 0}},
  {"Magenta", {0, 1, 0, 0}},
  {"VioletRed", {0, 0.81, 0, 0}},
  {"Rhodamine", {0, 0.82, 0, 0}},
  {"Mulberry", {0.34, 0.90, 0, 0.02}},
  {"RedViolet", {0.07, 0.90, 0, 0.34}},
  {"Fuchsia", {0.47, 0.91, 0, 0.08}},
  {"Lavender", {0, 0.48, 0, 0}},
  {"Thistle", {0.12, 0.59, 0, 0}},
  {"Orchid", {0.32, 0.64, 0, 0}},
  {"DarkOrchid", {0.40, 0.80, 0.20, 0}},
  {"Purple", {0.45, 0.86, 0, 0}},
  {"Plum", {0.50, 1, 0, 0}},
  {"Violet", {0.79, 0.88, 0, 0}},
  {"RoyalPurple", {0.75, 0.90, 0, 0}},
  {"BlueViolet", {0.86, 0.91, 0, 0.04}},
  {"Periwinkle", {0.57, 0.55, 0, 0}},
  {"CadetBlue", {0.62, 0.57, 0.23, 0}},
  {"CornflowerBlue", {0.65, 0.13, 0, 0}},
  {"MidnightBlue", {0.98, 0.13, 0, 0.43}},
  {"NavyBlue", {0.94, 0.54, 0, 0}},
  {"RoyalBlue", {1, 0.50, 0, 0}},
  {"Blue", {1, 1, 0, 0}},
  {"Cerulean", {0.94, 0.11, 0, 0}},
  {"Cyan", {1, 0, 0, 0}},
  {"ProcessBlue", {0.96, 0, 0, 0}},
  {"SkyBlue", {0.62, 0, 0.12, 0}},
  {"Turquoise", {0.85, 0, 0.20, 0}},
  {"TealBlue", {0.86, 0, 0.34, 0.02}},
  {"Aquamarine", {0.82, 0, 0.30, 0}},
  {"BlueGreen", {0.85, 0, 0.33, 0}},
  {"Emerald", {1, 0, 0.50, 0}},
  {"JungleGreen", {0.99, 0, 0.52, 0}},
  {"SeaGreen", {0.69, 0, 0.50, 0}},
  {"Green", {1, 0, 1, 0}},
  {"ForestGreen", {0.91, 0, 0.88, 0.12}},
  {"PineGreen", {0.92, 0, 0.59, 0.25}},
  {"LimeGreen", {0.50, 0, 1, 0}},
  {"YellowGreen", {0.44, 0, 0.74, 0}},
  {"SpringGreen", {0.26, 0, 0.76, 0}},
  {"OliveGreen", {0.64, 0, 0.95, 0.40}},
  {"RawSienna", {0, 0.72, 1, 0.45}},
  {"Sepia", {0, 0.83, 1, 0.70}},
  {"Brown", {0, 0.81, 1, 0.60}},
  {"Tan", {0.14, 0.42, 0.56, 0}},
  {"Gray", {0, 0, 0, 0.50}},
  {"Black", {0, 0, 0, 1}},
  {"White", {0, 0, 0, 0}}
};

inline bool
kwmatch(const char*& s, const char* k)
{
	while (*s == ' ')
		++s;
	int kwlen = strlen(k);
	if ( (s[kwlen] == 0) || (s[kwlen] == ' ') ) {
		if (strncmp(s, k, kwlen) == 0) {
			s += kwlen;
			return true;
		}
	}
	return false;
}

static float
readAlphaIfPresent(const char*& s)
{
	if (kwmatch(s, "alpha")) {
		float	alpha;
		if (readColorValues(s, 1, &alpha))
			return alpha;
	}
	return 1.0;
}

static void
doColorSpecial(const char* s, bool background)
{
	while (*s != 0) {
		if (!background) {
			if (kwmatch(s, "push")) {
				pushRuleColor();
				pushTextColor();
				continue;
			}
			
			if (kwmatch(s, "pop")) {
				popRuleColor();
				popTextColor();
				continue;
			}
		}
		
		if (kwmatch(s, "rgb")) {
			float	rgba[4];
			if (readColorValues(s, 3, rgba)) {
				rgba[3] = readAlphaIfPresent(s);
				CGColorRef	c = CGColorCreate(gRGBColorSpace, rgba);
				if (background)
					setBackground(c);
				else {
					setRuleColor(c);
					setTextColor(c);
				}
			}
			break;
		}
		
		if (kwmatch(s, "cmyk")) {
			float	cmyka[5];
			if (readColorValues(s, 4, cmyka)) {
				cmyka[4] = readAlphaIfPresent(s);
				CGColorRef	c = CGColorCreate(gCMYKColorSpace, cmyka);
				if (background)
					setBackground(c);
				else {
					setRuleColor(c);
					setTextColor(c);
				}
			}
			break;
		}
		
		if (kwmatch(s, "hsb") || kwmatch(s, "hsv")) {
			float	hsb[3];
			if (readColorValues(s, 3, hsb)) {
				CMColor	cm;
				cm.hsv.hue = (UInt16)(hsb[0] * 65535);
				cm.hsv.saturation = (UInt16)(hsb[1] * 65535);
				cm.hsv.value = (UInt16)(hsb[2] * 65535);
				CMConvertHSVToRGB(&cm, &cm, 1);
				float	rgba[4];
				rgba[0] = cm.rgb.red / 65535.0;
				rgba[1] = cm.rgb.green / 65535.0;
				rgba[2] = cm.rgb.blue / 65535.0;
				rgba[3] = readAlphaIfPresent(s);
				CGColorRef	c = CGColorCreate(gRGBColorSpace, rgba);
				if (background)
					setBackground(c);
				else {
					setRuleColor(c);
					setTextColor(c);
				}
			}
			break;
		}
		
		if (kwmatch(s, "hls")) {
			float	hls[3];
			if (readColorValues(s, 3, hls)) {
				CMColor	cm;
				cm.hls.hue = (UInt16)(hls[0] * 65535);
				cm.hls.lightness = (UInt16)(hls[1] * 65535);
				cm.hls.saturation = (UInt16)(hls[2] * 65535);
				CMConvertHLSToRGB(&cm, &cm, 1);
				float	rgba[4];
				rgba[0] = cm.rgb.red / 65535.0;
				rgba[1] = cm.rgb.green / 65535.0;
				rgba[2] = cm.rgb.blue / 65535.0;
				rgba[3] = readAlphaIfPresent(s);
				CGColorRef	c = CGColorCreate(gRGBColorSpace, rgba);
				if (background)
					setBackground(c);
				else {
					setRuleColor(c);
					setTextColor(c);
				}
			}
			break;
		}
		
		if (kwmatch(s, "gray")) {
			float	gray[2];
			if (readColorValues(s, 1, gray)) {
				gray[1] = readAlphaIfPresent(s);
				CGColorRef	c = CGColorCreate(gGrayColorSpace, gray);
				if (background)
					setBackground(c);
				else {
					setRuleColor(c);
					setTextColor(c);
				}
			}
			break;
		}
		
		const namedColor*	nc = colorsByName + sizeof(colorsByName) / sizeof(namedColor);
		while (nc-- > colorsByName) {
			if (kwmatch(s, nc->name)) {
				float	cmyka[5];
				cmyka[0] = nc->cmyk[0];
				cmyka[1] = nc->cmyk[1];
				cmyka[2] = nc->cmyk[2];
				cmyka[3] = nc->cmyk[3];
				cmyka[4] = readAlphaIfPresent(s);
				CGColorRef	c = CGColorCreate(gCMYKColorSpace, cmyka);
				if (background)
					setBackground(c);
				else {
					setRuleColor(c);
					setTextColor(c);
				}
				break;
			}
		}
		break;		
	}
}

static bool
prefixMatch(const char* str, const char* prefix, const char*& remainder)
{
	int	len = strlen(prefix);
	if (strncmp(str, prefix, len) == 0) {
		remainder = str + len;
		return true;
	}
	return false;
}

static double_t
readDimen(const char*& arg)	// currently reads unsigned dimens only; no negative paper sizes!
{
	double_t	rval = 0.0;
	while ((*arg >= '0') && (*arg <= '9')) {
		rval *= 10.0;
		rval += *arg - '0';
		++arg;
	}
	if (*arg == '.') {
		++arg;
		double_t	frac = 10.0;
		while ((*arg >= '0') && (*arg <= '9')) {
			rval += (*arg - '0') / frac;
			frac *= 10.0;
			++arg;
		}
	}
	const dimenRec*	dim = &sDimenRatios[0];
	while (dim->name != 0) {
		if (prefixMatch(arg, dim->name, arg)) {
			rval *= dim->factor;
			break;
		}
		++dim;
	}
	return rval;
}

bool
getPaperSize(const char* arg, double_t& paperWd, double_t& paperHt)
{
	paperHt = paperWd = 0.0;

	char*   s1 = strdup(arg);
	char*   s2 = strchr(s1, ':');
	if (s2 != NULL)
		*s2++ = 0;

	const paperSizeRec*	ps = &gPaperSizes[0];
	while (ps->name != 0) {
		if (strcasecmp(s1, ps->name) == 0) {
			paperWd = ps->width;
			paperHt = ps->height;
			break;
		}
		++ps;
	}
	if (ps->name == 0) {
		char*   s3 = strchr(s1, ',');
		if (s3 != NULL) {
			*s3++ = 0;
			const char* s4 = s1;
			paperWd = readDimen(s4);
			s4 = s3;
			paperHt = readDimen(s4);
		}
	}
	if (s2 != NULL && strcasecmp(s2, "landscape") == 0) {
		double_t  t = paperHt;
		paperHt = paperWd;
		paperWd = t;
	}
	
	free(s1);
	
	return (paperHt > 0.0) && (paperWd > 0.0);
}

static bool
getPageSize(const char* arg, double_t& pageWd, double_t& pageHt)
{
	pageHt = pageWd = 0.0;
	while (*arg) {
		while (*arg == ' ')
			++arg;
		if (prefixMatch(arg, "width", arg)) {
			while (*arg == ' ')
				++arg;
			pageWd = readDimen(arg);
			continue;
		}
		if (prefixMatch(arg, "height", arg)) {
			while (*arg == ' ')
				++arg;
			pageHt = readDimen(arg);
			continue;
		}
		if (prefixMatch(arg, "default", arg)) {
			pageWd = gPaperWd;
			pageHt = gPaperHt;
			continue;
		}
		++arg;
	}
	return (pageHt > 0.0) && (pageWd > 0.0);
}

inline bool
strMatch(const char* s, const char* t)
{
	return (strcmp(s, t) == 0);
}

static void
shadowWarning()
{
	static bool	alreadyWarned = false;
	if (alreadyWarned)
		return;
	fprintf(stderr, "\n"
					"## xdv2pdf: CoreGraphics shadow effects not supported on this system\n"
					"## (at least Mac OS X 10.3 is required)\n");
	alreadyWarned = true;
}

void
doSpecial(const char* special)
{
	int	u = strlen(special);
	const char* specialArg;
	
	if (prefixMatch(special, "pdf:pagesize", specialArg)) {
		// NOTE: must come before the general pdf special check!
		// syntax: \special{pdf:pagesize width <dim> height <dim>} etc
		while (*specialArg && (*specialArg <= ' '))
			++specialArg;
		double_t	paperWd, paperHt;
		if (getPageSize(specialArg, paperWd, paperHt)) {
			gMediaBox = CGRectMake(0, 0, paperWd, paperHt);
			if (gPageStarted) {
				fprintf(stderr, "\n### warning on page [");
				int i, j;
				for (j = 9; j > 1; --j)
					if (gCounts[j] != 0)
						break;
				for (i = 0; i < j; ++i)
					fprintf(stderr, "%d.", gCounts[i]);
				fprintf(stderr, "%d", gCounts[j]);
				fprintf(stderr, "]: pdf page size \"%s\" will take effect from NEXT page ", specialArg);
			}
		}
	}

	else if (prefixMatch(special, "pdf:", specialArg))
		doPDFspecial(specialArg);
	
	else if (prefixMatch(special, "color ", specialArg))
		doColorSpecial(specialArg, false);
	
	else if (prefixMatch(special, "background ", specialArg))
		doColorSpecial(specialArg, true);
	
	else if (prefixMatch(special, "x:fontmapfile", specialArg))
		doPdfMapFile(specialArg);
	else if (prefixMatch(special, "x:fontmapline", specialArg))
		doPdfMapLine(specialArg, 0);
	
	else if (strMatch(special, "x:textcolorpush"))
		pushTextColor();
	else if (strMatch(special, "x:textcolorpop"))
		popTextColor();
	else if (strMatch(special, "x:rulecolorpush"))
		pushRuleColor();
	else if (strMatch(special, "x:rulecolorpop"))
		popRuleColor();

	else if (prefixMatch(special, "x:rulecolor", specialArg)) {
		// set rule color
		if (*specialArg != '=' || u < 18)
			// not long enough for a proper color spec, so set it to black
			resetRuleColor();
		else {
			++specialArg;
			CGColorRef	color;
			if (u < 20)
				color = readColorValue(specialArg);
			else
				color = readColorValueWithAlpha(specialArg);
			setRuleColor(color);
		}
	}

	else if (prefixMatch(special, "x:textcolor", specialArg)) {
		// set text color (overriding color of the font style)
		if (*specialArg != '=' || u < 18)
			resetTextColor();
		else {
			++specialArg;
			CGColorRef	color;
			if (u < 20)
				color = readColorValue(specialArg);
			else
				color = readColorValueWithAlpha(specialArg);
			setTextColor(color);
		}
	}

	else if (strMatch(special, "x:gsave")) {
		ensurePageStarted();
		CGContextSaveGState(gCtx);
	}
	else if (strMatch(special, "x:grestore")) {
		ensurePageStarted();
		CGContextRestoreGState(gCtx);
		setColor(gCurrentColor, true);
		cur_cgFont = kUndefinedFont;
	}
	else if (prefixMatch(special, "x:scale", specialArg)) {
		ensurePageStarted();
		while (*specialArg && (*specialArg <= ' '))
			++specialArg;
		float	xscale = atof(specialArg);
		while (*specialArg && (((*specialArg >= '0') && (*specialArg <= '9')) || *specialArg == '.' || *specialArg == '-'))
			++specialArg;
		while (*specialArg && (*specialArg <= ' '))
			++specialArg;
		float	yscale = atof(specialArg);
		if ((xscale != 0.0) && (yscale != 0.0)) {
			CGContextTranslateCTM(gCtx, kDvi2Scr * dvi.h, kDvi2Scr * (gPageHt - dvi.v));
			CGContextScaleCTM(gCtx, xscale, yscale);
			CGContextTranslateCTM(gCtx, -kDvi2Scr * dvi.h, -kDvi2Scr * (gPageHt - dvi.v));
		}
	}
	else if (prefixMatch(special, "x:rotate", specialArg)) {
		ensurePageStarted();
		while (*specialArg && (*specialArg <= ' '))
			++specialArg;
		float	rotation = atof(specialArg);
		CGContextTranslateCTM(gCtx, kDvi2Scr * dvi.h, kDvi2Scr * (gPageHt - dvi.v));
		CGContextRotateCTM(gCtx, rotation * M_PI / 180.0);
		CGContextTranslateCTM(gCtx, -kDvi2Scr * dvi.h, -kDvi2Scr * (gPageHt - dvi.v));
	}
	else if (prefixMatch(special, "x:backgroundcolor", specialArg)) {
		ensurePageStarted();
		if ((*specialArg != '=') || (u < 24)) {
			// ignore incorrect special
		}
		else {
			++specialArg;
			CGColorRef	bg;
			if (u < 26)
				bg = readColorValue(specialArg);
			else
				bg = readColorValueWithAlpha(specialArg);
			setBackground(bg);
	   }
	}
	else if (prefixMatch(special, "x:shadow", specialArg)) {
		// syntax: \special{x:shadow(x,y),b}
		do {
			CGSize	offset;
			float	blur;
			
			if (*specialArg++ != '(')	break;
			offset.width = readFloat(specialArg);
			
			if (*specialArg++ != ',')	break;
			offset.height = readFloat(specialArg);
			
			if (*specialArg++ != ')')	break;
			if (*specialArg++ != ',')	break;
			blur = readFloat(specialArg);
			
			// NB: API only available on 10.3 and later
			if (&CGContextSetShadow != NULL)
				CGContextSetShadow(gCtx, offset, blur);
			else
				shadowWarning();
		} while (false);
	}
	else if (prefixMatch(special, "x:colorshadow", specialArg)) {
		// syntax: \special{x:colorshadow(x,y),b,c}
		do {
			CGSize		offset;
			float		blur;
			CGColorRef	color;
			
			if (*specialArg++ != '(')	break;
			offset.width = readFloat(specialArg);
			
			if (*specialArg++ != ',')	break;
			offset.height = readFloat(specialArg);
			
			if (*specialArg++ != ')')	break;
			if (*specialArg++ != ',')	break;
			blur = readFloat(specialArg);
			
			if (*specialArg++ != ',')	break;
			if (special + u < specialArg + 6)	break;
			if (special + u < specialArg + 8)
				color = readColorValue(specialArg);
			else
				color = readColorValueWithAlpha(specialArg);
			
			// NB: APIs only available on 10.3 and later
			if (&CGContextSetShadowWithColor != NULL) {
				CGContextSetShadowWithColor(gCtx, offset, blur, color);
			}
			else
				shadowWarning();
			CGColorRelease(color);
		} while (false);
	}
	else if (prefixMatch(special, "x:papersize", specialArg) || prefixMatch(special, "papersize", specialArg)) {
		// syntax: \special{x:papersize=a4:landscape} etc
		while (*specialArg && (*specialArg <= ' '))
			++specialArg;
		if (*specialArg == '=') {
			++specialArg;
			while (*specialArg && (*specialArg <= ' '))
				++specialArg;
		}
		if (getPaperSize(specialArg, gPaperWd, gPaperHt)) {
			gMediaBox = CGRectMake(0, 0, gPaperWd, gPaperHt);
			if (gPageStarted) {
				fprintf(stderr, "\n### warning on page [");
				int i, j;
				for (j = 9; j > 1; --j)
					if (gCounts[j] != 0)
						break;
				for (i = 0; i < j; ++i)
					fprintf(stderr, "%d.", gCounts[i]);
				fprintf(stderr, "%d", gCounts[j]);
				fprintf(stderr, "]: paper size \"%s\" will take effect from NEXT page ", specialArg);
			}
		}
	}
}