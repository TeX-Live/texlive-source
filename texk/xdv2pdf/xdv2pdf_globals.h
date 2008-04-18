/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2005 by SIL International
 written by Jonathan Kew

 This software is distributed under the terms of the Common Public License,
 version 1.0.
 For details, see <http://www.opensource.org/licenses/cpl1.0.php> or the file
 cpl1.0.txt included with the software.
\****************************************************************************/


#ifndef XDV2PDF_GLOBALS
#define XDV2PDF_GLOBALS

#include <list>
#include <map>
#include <vector>

#ifdef DEFINE_GLOBALS
#define GLOBAL
#else
#define GLOBAL	extern
#endif

#define	kUndefinedFont	0x80000000UL

GLOBAL	CGContextRef	gCtx;

GLOBAL	CGColorSpaceRef	gRGBColorSpace;
GLOBAL	CGColorSpaceRef	gCMYKColorSpace;
GLOBAL	CGColorSpaceRef	gGrayColorSpace;

GLOBAL	bool 		gPageStarted;
GLOBAL	CGRect		gMediaBox;
GLOBAL	double_t	gPaperWd;
GLOBAL	double_t	gPaperHt;


struct dviVars {
	SInt32	h;
	SInt32	v;
	SInt32	w;
	SInt32	x;
	SInt32	y;
	SInt32	z;
};
GLOBAL dviVars			dvi;
GLOBAL UInt32			f, cur_cgFont;

GLOBAL double_t kScr2Dvi	// not quite const, actually - mag changes them
#ifdef DEFINE_GLOBALS
	= (72.27 * 65536.0) / 72.0
#endif
	;
GLOBAL double_t kDvi2Scr
#ifdef DEFINE_GLOBALS
	= 1.0 / kScr2Dvi
#endif
	;

GLOBAL CGColorRef	kBlackColor;

GLOBAL std::list<CGColorRef>	gTextColorStack;
GLOBAL std::list<CGColorRef>	gRuleColorStack;

GLOBAL CGColorRef	gTextColor;
GLOBAL CGColorRef	gRuleColor;
GLOBAL CGColorRef	gBackground;

GLOBAL CGColorRef	gCurrentColor;

struct texFont {
				texFont()
                    : cgFont(0)
                    , size(Long2Fix(10))
					, charMap(NULL)
					, hasMetrics(false)
						{
						}
	CGFontRef			cgFont;
    Fixed				size;
	std::vector<Fixed>	widths;
	std::vector<Fixed>	heights;
	std::vector<Fixed>	depths;
	std::vector<UInt16>*charMap;
	bool				hasMetrics;
	bool				hasMap;
};

GLOBAL	std::map<UInt32,texFont>	gTeXFonts;

GLOBAL	long	gPageIndex;
GLOBAL	int		gCounts[10];

struct box {
	float	llx;
	float	lly;
	float	urx;
	float	ury;
};
GLOBAL	box		gAnnotBox;

GLOBAL	int		gTagLevel;
GLOBAL	int		gDviLevel;
GLOBAL	bool	gTrackingBoxes;

GLOBAL	double_t	gMagScale;

GLOBAL	double_t	gPageHt;
GLOBAL	double_t	gPageWd;

struct paperSizeRec {
	const char*	name;
	double_t	width;
	double_t	height;	// dimensions in BP ("big" or PostScript/CG points)
};

#ifndef DEFINE_GLOBALS
GLOBAL	paperSizeRec	gPaperSizes[];
#endif

GLOBAL CFURLRef	gSaveURL;
GLOBAL char		gPdfMarkPath[_POSIX_PATH_MAX+1];
GLOBAL FILE*	gPdfMarkFile;


void	flushAnnotBox();
void	initAnnotBox();
bool	getPaperSize(const char* arg, double_t& paperWd, double_t& paperHt);
void	doPDFspecial(const char* special);
void	doSpecial(const char* special);


void	ensurePageStarted();
void	paintBackground();
void	setColor(CGColorRef color, bool force);
void	doPdfMapLine(const char* line, char mode);
void	doPdfMapFile(const char* fileName);

extern "C" {
	int	xdv2pdf(int argc, char** argv);
};

#endif /* XDV2PDF_GLOBALS */
