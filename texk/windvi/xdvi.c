/*========================================================================*\

Copyright (c) 1990-1999  Paul Vojta

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL PAUL VOJTA OR ANYONE ELSE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.

NOTE: xdvi is based on prior work as noted in the modification history, below.

\*========================================================================*/

/*
 * DVI previewer for X.
 *
 * Eric Cooper, CMU, September 1985.
 *
 * Code derived from dvi-imagen.c.
 *
 * Modification history:
 * 1/1986	Modified for X.10	--Bob Scheifler, MIT LCS.
 * 7/1988	Modified for X.11	--Mark Eichin, MIT
 * 12/1988	Added 'R' option, toolkit, magnifying glass
 *					--Paul Vojta, UC Berkeley.
 * 2/1989	Added tpic support	--Jeffrey Lee, U of Toronto
 * 4/1989	Modified for System V	--Donald Richardson, Clarkson Univ.
 * 3/1990	Added VMS support	--Scott Allendorf, U of Iowa
 * 7/1990	Added reflection mode	--Michael Pak, Hebrew U of Jerusalem
 * 1/1992	Added greyscale code	--Till Brychcy, Techn. Univ. Muenchen
 *					  and Lee Hetherington, MIT
 * 7/1992       Added extra menu buttons--Nelson H. F. Beebe <beebe@math.utah.edu>
 * 4/1994	Added DPS support, bounding box
 *					--Ricardo Telichevesky
 *					  and Luis Miguel Silveira, MIT RLE.
 * 2/1995       Added rulers support    --Nelson H. F. Beebe <beebe@math.utah.edu>
 *
 *	Compilation options:
 *	VMS	compile for VMS
 *	NOTOOL	compile without toolkit
 *	BUTTONS	compile with buttons on the side of the window (needs toolkit)
 *	WORDS_BIGENDIAN	store bitmaps internally with most significant bit first
 *	BMTYPE	store bitmaps in unsigned BMTYPE
 *	BMBYTES	sizeof(unsigned BMTYPE)
 *	ALTFONT	default for -altfont option
 *	SHRINK	default for -s option (shrink factor)
 *	MFMODE	default for -mfmode option
 *	A4	use European size paper, and change default dimension to cm
 *	TEXXET	support reflection dvi codes (right-to-left typesetting)
 *	GREY	use grey levels to shrink fonts
 *	PS_GS	use Ghostscript to render pictures/bounding boxes
 *	PS_DPS	use display postscript to render pictures/bounding boxes
 *	PS_NEWS	use the NeWS server to render pictures/bounding boxes
 *	GS_PATH	path to call the Ghostscript interpreter by
 *	GRID	grid in magnification windows enabled
 *	HTEX	hypertex enabled.  EXPECT PROBLEMS IF USED WITH MOTIF!
 *		please send patches to janl@math.uio.no
 */

#if 0
static	char	copyright[] =
"@(#) Copyright (c) 1994-1999 Paul Vojta.  All rights reserved.\n";
#endif

#define	EXTERN
#define	INIT(x)	=x

#include "xdvi-config.h"
#include "c-openmx.h"
#include <kpathsea/c-ctype.h>
#include <kpathsea/c-fopen.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/proginit.h>
#include <kpathsea/progname.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-hush.h>
#include <kpathsea/tex-make.h>

#ifdef HTEX
#include "wwwconf.h"
#include "WWWLib.h"
#include "WWWInit.h"
#include "WWWCache.h"
#include "HTEscape.h"
#endif

#ifndef	ALTFONT
#define	ALTFONT	"cmr10"
#endif

#ifndef	SHRINK
#define	SHRINK	8
#endif

#ifndef	BDPI
#define	BDPI	600
#endif

#ifndef	MFMODE
#define	MFMODE	NULL
#endif

#undef MKTEXPK
#define MKTEXPK MAKEPK

#if	defined(PS_GS) && !defined(GS_PATH)
#define	GS_PATH	"gs"
#endif

#if A4
#define	DEFAULT_PAPER		"a4"
#else
#define	DEFAULT_PAPER		"us"
#endif

#include "version.h"

#ifdef HTEX
/* Application name and version for w3c-libwww routines.  
   This is what will show up in httpd's agent_log files. 
*/
#ifdef Omega
char *HTAppName = "oxdvik";
#else
char *HTAppName = "xdvik";
#endif
char *HTAppVersion = VERSION;
#endif


#ifdef	X_NOT_STDC_ENV
#ifndef	atof
extern	double	atof ARGS((_Xconst char *));
#endif
#endif

/* Xlib and Xutil are already included */
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include "xdvi.icon"

#ifdef	TOOLKIT

#ifdef	OLD_X11_TOOLKIT
#include <X11/Atoms.h>
#else /* not OLD_X11_TOOLKIT */
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#endif /* not OLD_X11_TOOLKIT */

#include <X11/Shell.h>	/* needed for def. of XtNiconX */

#ifndef	XtSpecificationRelease
#define	XtSpecificationRelease	0
#endif
#if	XtSpecificationRelease >= 4

#ifndef MOTIF
#include <X11/Xaw/Viewport.h>
#ifdef HTEX
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Scrollbar.h>
#endif /* HTEX */
#define	VPORT_WIDGET_CLASS	viewportWidgetClass
#define	DRAW_WIDGET_CLASS	drawWidgetClass
#else /* MOTIF */
#include <Xm/MainW.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>
#include <Xm/DrawingA.h>
#define	VPORT_WIDGET_CLASS	xmMainWindowWidgetClass
#define	DRAW_WIDGET_CLASS	xmDrawingAreaWidgetClass
#endif /* MOTIF */

#ifdef	BUTTONS
#ifndef MOTIF
#include <X11/Xaw/Command.h>
#define	FORM_WIDGET_CLASS	formWidgetClass
#else /* MOTIF */
#include <Xm/Form.h>
#define	FORM_WIDGET_CLASS	xmFormWidgetClass
#endif /* MOTIF */
#endif /* BUTTONS */

#else	/* XtSpecificationRelease < 4 */

#define	XtPointer caddr_t
#include <X11/Viewport.h>
#ifdef HTEX
#include <X11/AsciiText.h>
#include <X11/Box.h>
#include <X11/Command.h>
#include <X11/Dialog.h>
#include <X11/Form.h>
#include <X11/Paned.h>
#include <X11/Scroll.h>
#include <X11/VPaned.h>
#include <X11/Scrollbar.h>
#endif
#define	VPORT_WIDGET_CLASS	viewportWidgetClass
#define	DRAW_WIDGET_CLASS	drawWidgetClass
#ifdef	BUTTONS
#include <X11/Command.h>
#define	FORM_WIDGET_CLASS	formWidgetClass
#endif

#endif	/* XtSpecificationRelease */

#if	XtSpecificationRelease < 5
typedef	caddr_t		XPointer;
#endif

#else	/* not TOOLKIT */

typedef	int		Position;

#endif	/* not TOOLKIT */

#ifdef	VMS
/*
 * Magnifying glass cursor
 *
 * Developed by Tom Sawyer, April 1990
 * Contibuted by Hunter Goatley, January 1991
 *
 */

#define mag_glass_width 16
#define mag_glass_height 16
#define mag_glass_x_hot 6
#define mag_glass_y_hot 6
static char mag_glass_bits[] = {
	0xf8, 0x03, 0x0c, 0x06, 0xe2, 0x09, 0x13, 0x1a, 0x01, 0x14, 0x01, 0x14,
	0x01, 0x10, 0x01, 0x10, 0x01, 0x10, 0x03, 0x10, 0x02, 0x18, 0x0c, 0x34,
	0xf8, 0x6f, 0x00, 0xd8, 0x00, 0xb0, 0x00, 0xe0
};
#include <decw$cursor.h>	/* Include the DECWindows cursor symbols */
static	int	DECWCursorFont;	/* Space for the DECWindows cursor font  */
static	Pixmap	MagnifyPixmap;	/* Pixmap to hold our special mag-glass  */
#include <X11/Xresource.h>      /* Motif apparently needs this one */
#endif	/* VMS */

/*
 * Command line flags.
 */

static	Dimension	bwidth	= 2;

#define	fore_Pixel	resource._fore_Pixel
#define	back_Pixel	resource._back_Pixel
#ifdef	TOOLKIT
struct _resource	resource;
#define	brdr_Pixel	resource._brdr_Pixel
#define	hl_Pixel	resource._hl_Pixel
#define	cr_Pixel	resource._cr_Pixel
#ifdef GRID
#define grid1_Pixel     resource._grid1_Pixel
#define grid2_Pixel     resource._grid2_Pixel
#define grid3_Pixel     resource._grid3_Pixel
#endif /* GRID */
#else /* not TOOLKIT */
static	_Xconst char	*brdr_color;
static	_Xconst char	*high_color;
static	_Xconst char	*curs_color;
static	Pixel		hl_Pixel, cr_Pixel;
#ifdef GRID
static  Pixel           grid1_Pixel, grid2_Pixel, grid3_Pixel;
#endif /* GRID */
#endif	/* not TOOLKIT */

struct	mg_size_rec	mg_size[5]	= {{200, 150}, {400, 250}, {700, 500},
					   {1000, 800}, {1200, 1200}};

static	char	*curr_page;

struct WindowRec mane	= {(Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0};
struct WindowRec alt	= {(Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0};
/*	currwin is temporary storage except for within redraw() */
struct WindowRec currwin= {(Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0};

#ifdef	lint
#ifdef	TOOLKIT
WidgetClass	widgetClass;
#ifndef MOTIF
WidgetClass	viewportWidgetClass;
#ifdef	BUTTONS
WidgetClass	formWidgetClass, compositeWidgetClass, commandWidgetClass;
#endif	/* BUTTONS */
#else /* MOTIF */
WidgetClass	xmMainWindowWidgetClass, xmDrawingAreaWidgetClass;
#ifdef	BUTTONS
WidgetClass	xmFormWidgetClass, xmBulletinBoardWidgetClass;
WidgetClass	xmPushButtonWidgetClass;
#endif	/* BUTTONS */
#endif /* MOTIF */
#endif	/* TOOLKIT */
#endif	/* lint */

/*
 *	Data for options processing
 */

static	_Xconst	char	silent[] = " ";	/* flag value for usage() */

static	_Xconst	char	subst[]	= "x";	/* another flag value */

static	_Xconst char	*subst_val[] = {
#ifdef	BUTTONS
					"-shrinkbutton[1-4] <shrink>",
#endif
					"-mgs[n] <size>"};

/* BEGIN CHUNK xdvi.c 1 */
#ifdef SRC_SPECIALS
#define src_tickSize			resource._src_tickSize
#define src_specialFormat		resource._src_specialFormat
#define src_jumpButton			resource._src_jumpButton
#endif
/* END CHUNK xdvi.c 1 */

#ifdef	TOOLKIT

static	XrmOptionDescRec	options[] = {
{"-s",          ".shrinkFactor", XrmoptionSepArg,       (caddr_t) SHRINK},
#ifndef	VMS
{"-S",		".densityPercent", XrmoptionSepArg,	(caddr_t) NULL},
#endif
{"-density",	".densityPercent", XrmoptionSepArg,	(caddr_t) NULL},
#ifdef	GREY
{"-nogrey",	".grey",	XrmoptionNoArg,		(caddr_t) "off"},
{"+nogrey",	".grey",	XrmoptionNoArg,		(caddr_t) "on"},
{"-gamma",	".gamma",	XrmoptionSepArg,	(caddr_t) NULL},
{"-install",	".install",	XrmoptionNoArg,		(caddr_t) "on"},
{"-noinstall",	".install",	XrmoptionNoArg,		(caddr_t) "off"},
#endif
#ifdef GRID
{"-grid1",      ".grid1Color",  XrmoptionSepArg,        (caddr_t) NULL},
{"-grid2",      ".grid2Color",  XrmoptionSepArg,        (caddr_t) NULL},
{"-grid3",      ".grid3Color",  XrmoptionSepArg,        (caddr_t) NULL},
#endif /* GRID */
{"-p",		".pixelsPerInch", XrmoptionSepArg,	(caddr_t) NULL},
{"-margins",	".Margin",	XrmoptionSepArg,	(caddr_t) NULL},
{"-sidemargin",	".sideMargin",	XrmoptionSepArg,	(caddr_t) NULL},
{"-topmargin",	".topMargin",	XrmoptionSepArg,	(caddr_t) NULL},
{"-offsets",	".Offset",	XrmoptionSepArg,	(caddr_t) NULL},
{"-xoffset",	".xOffset",	XrmoptionSepArg,	(caddr_t) NULL},
{"-yoffset",	".yOffset",	XrmoptionSepArg,	(caddr_t) NULL},
{"-paper",	".paper",	XrmoptionSepArg,	(caddr_t) NULL},
{"-altfont",	".altFont",	XrmoptionSepArg,	(caddr_t) NULL},
#ifdef MKTEXPK
{"-nomakepk",	".makePk",	XrmoptionNoArg,		(caddr_t) "off"},
{"+nomakepk",	".makePk",	XrmoptionNoArg,		(caddr_t) "on"},
#endif
{"-mfmode",     ".mfMode",      XrmoptionSepArg,        (caddr_t) MFMODE},
{"-l",		".listFonts",	XrmoptionNoArg,		(caddr_t) "on"},
{"+l",		".listFonts",	XrmoptionNoArg,		(caddr_t) "off"},
#ifdef	BUTTONS
{"-expert",	".expert",	XrmoptionNoArg,		(caddr_t) "on"},
{"+expert",	".expert",	XrmoptionNoArg,		(caddr_t) "off"},
{"-shrinkbutton1",".shrinkButton1",XrmoptionSepArg,	(caddr_t) NULL},
{"-shrinkbutton2",".shrinkButton2",XrmoptionSepArg,	(caddr_t) NULL},
{"-shrinkbutton3",".shrinkButton3",XrmoptionSepArg,	(caddr_t) NULL},
{"-shrinkbutton4",".shrinkButton4",XrmoptionSepArg,	(caddr_t) NULL},
#endif
{"-mgs",	".magnifierSize1",XrmoptionSepArg,	(caddr_t) NULL},
{"-mgs1",	".magnifierSize1",XrmoptionSepArg,	(caddr_t) NULL},
{"-mgs2",	".magnifierSize2",XrmoptionSepArg,	(caddr_t) NULL},
{"-mgs3",	".magnifierSize3",XrmoptionSepArg,	(caddr_t) NULL},
{"-mgs4",	".magnifierSize4",XrmoptionSepArg,	(caddr_t) NULL},
{"-mgs5",	".magnifierSize5",XrmoptionSepArg,	(caddr_t) NULL},
{"-warnspecials", ".warnSpecials", XrmoptionNoArg,	(caddr_t) "on"},
{"+warnspecials", ".warnSpecials", XrmoptionNoArg,	(caddr_t) "off"},
{"-hush",	".Hush",	XrmoptionNoArg,		(caddr_t) "on"},
{"+hush",	".Hush",	XrmoptionNoArg,		(caddr_t) "off"},
{"-hushchars",	".hushLostChars", XrmoptionNoArg,	(caddr_t) "on"},
{"+hushchars",	".hushLostChars", XrmoptionNoArg,	(caddr_t) "off"},
{"-hushchecksums", ".hushChecksums", XrmoptionNoArg,	(caddr_t) "on"},
{"+hushchecksums", ".hushChecksums", XrmoptionNoArg,	(caddr_t) "off"},
{"-safer",	".safer",	XrmoptionNoArg,		(caddr_t) "on"},
{"+safer",	".safer",	XrmoptionNoArg,		(caddr_t) "off"},
{"-fg",		".foreground",	XrmoptionSepArg,	(caddr_t) NULL},
{"-foreground",	".foreground",	XrmoptionSepArg,	(caddr_t) NULL},
{"-bg",		".background",	XrmoptionSepArg,	(caddr_t) NULL},
{"-background",	".background",	XrmoptionSepArg,	(caddr_t) NULL},
{"-hl",		".highlight",	XrmoptionSepArg,	(caddr_t) NULL},
{"-cr",		".cursorColor",	XrmoptionSepArg,	(caddr_t) NULL},
{"-icongeometry",".iconGeometry",XrmoptionSepArg,	(caddr_t) NULL},
{"-keep",	".keepPosition",XrmoptionNoArg,		(caddr_t) "on"},
{"+keep",	".keepPosition",XrmoptionNoArg,		(caddr_t) "off"},
{"-copy",	".copy",	XrmoptionNoArg,		(caddr_t) "on"},
{"+copy",	".copy",	XrmoptionNoArg,		(caddr_t) "off"},
{"-thorough",	".thorough",	XrmoptionNoArg,		(caddr_t) "on"},
{"+thorough",	".thorough",	XrmoptionNoArg,		(caddr_t) "off"},
#if	PS
{"-nopostscript",".postscript",	XrmoptionNoArg,		(caddr_t) "off"},
{"+nopostscript",".postscript",	XrmoptionNoArg,		(caddr_t) "on"},
{"-noscan",	".prescan",	XrmoptionNoArg,		(caddr_t) "off"},
{"+noscan",	".prescan",	XrmoptionNoArg,		(caddr_t) "on"},
{"-allowshell",	".allowShell",	XrmoptionNoArg,		(caddr_t) "on"},
{"+allowshell",	".allowShell",	XrmoptionNoArg,		(caddr_t) "off"},
#ifdef	PS_DPS
{"-nodps",	".dps",		XrmoptionNoArg,		(caddr_t) "off"},
{"+nodps",	".dps",		XrmoptionNoArg,		(caddr_t) "on"},
#endif
#ifdef	PS_NEWS
{"-nonews",	".news",	XrmoptionNoArg,		(caddr_t) "off"},
{"+nonews",	".news",	XrmoptionNoArg,		(caddr_t) "on"},
#endif
#ifdef	PS_GS
{"-noghostscript",".ghostscript", XrmoptionNoArg,	(caddr_t) "off"},
{"+noghostscript",".ghostscript", XrmoptionNoArg,	(caddr_t) "on"},
{"-nogssafer",	".gsSafer",	XrmoptionNoArg,		(caddr_t) "off"},
{"+nogssafer",	".gsSafer",	XrmoptionNoArg,		(caddr_t) "on"},
{"-gsalpha",	".gsAlpha",	XrmoptionNoArg,		(caddr_t) "on"},
{"+gsalpha",	".gsAlpha",	XrmoptionNoArg,		(caddr_t) "off"},
{"-interpreter",".interpreter",	XrmoptionSepArg,	(caddr_t) NULL},
{"-gspalette",	".palette",	XrmoptionSepArg,	(caddr_t) NULL},
#endif
#endif	/* PS */
{"-debug",	".debugLevel",	XrmoptionSepArg,	(caddr_t) NULL},
{"-version",	".version",	XrmoptionNoArg,		(caddr_t) "on"},
{"+version",	".version",	XrmoptionNoArg,		(caddr_t) "off"},
#ifdef HTEX
{"-underlink",  ".underLink",   XrmoptionNoArg,         (caddr_t) "on"},
{"+underlink",  ".underLink",   XrmoptionNoArg,         (caddr_t) "off"},
{"-browser",    ".wwwBrowser",  XrmoptionSepArg,        (caddr_t) NULL},
{"-base",       ".urlBase",     XrmoptionSepArg,        (caddr_t) NULL},
#endif
/* BEGIN CHUNK xdvi.c 2 */
#ifdef SRC_SPECIALS
{"-srcSpecialFormat", ".srcSpecialFormat", XrmoptionSepArg,   (caddr_t) NULL},
{"-srcJumpButton",    ".srcJumpButton",    XrmoptionSepArg,   (caddr_t) NULL},
{"-srcTickShape",     ".srcTickShape",     XrmoptionSepArg,   (caddr_t) NULL},
{"-srcTickSize",      ".srcTickSize",      XrmoptionSepArg,   (caddr_t) NULL},
{"-srcCursor",        ".srcCursor",        XrmoptionSepArg,   (caddr_t) NULL},
{"-srcEditorCommand", ".srcEditorCommand", XrmoptionSepArg,   (caddr_t) NULL},
{"-srcVerbosity",     ".srcVerbosity",     XrmoptionSepArg,   (caddr_t) NULL},
{"-srcVisibility",    ".srcVisibility",    XrmoptionNoArg,    (caddr_t) "on"},
{"+srcVisibility",    ".srcVisibility",    XrmoptionNoArg,    (caddr_t) "off"},
{"-srcMode",          ".srcMode",		   XrmoptionNoArg,    (caddr_t) "on"},
{"+srcMode",          ".srcMode",          XrmoptionNoArg,    (caddr_t) "off"},
#endif  
/* END CHUNK xdvi.c 2 */
};

#define	offset(field)	XtOffsetOf(struct _resource, field)

static int	base_tick_length = 4;

static	char	XtRBool3[]	= "Bool3";	/* resource for Bool3 */

static	XtResource	application_resources[] = {
#if CFGFILE
{"name", "Name", XtRString, sizeof(char *),
  offset(progname), XtRString, (caddr_t) NULL},
#endif
{"shrinkFactor", "ShrinkFactor", XtRInt, sizeof(int),
  offset(shrinkfactor), XtRImmediate, (XtPointer) SHRINK},
/*  offset(shrinkfactor), XtRString, SHRINK}, */
{"delayRulers", "DelayRulers", XtRBoolean, sizeof(Boolean),
  offset(_delay_rulers), XtRString, "true"},
{"densityPercent", "DensityPercent", XtRInt, sizeof(int),
  offset(_density), XtRString, "40"},
#ifdef	GREY
{"gamma", "Gamma", XtRFloat, sizeof(float),
  offset(_gamma), XtRString, "1"},
#endif
{"pixelsPerInch", "PixelsPerInch", XtRInt, sizeof(int),
  offset(_pixels_per_inch), XtRImmediate, (XtPointer) BDPI},
{"sideMargin", "Margin", XtRString, sizeof(char *),
  offset(sidemargin), XtRString, (caddr_t) NULL},
{"tickLength", "TickLength", XtRInt, sizeof(int),
  offset(_tick_length), XtRInt, (caddr_t) &base_tick_length},
{"tickUnits", "TickUnits", XtRString, sizeof(char *),
  offset(_tick_units), XtRString, "pt"},
{"topMargin", "Margin", XtRString, sizeof(char *),
  offset(topmargin), XtRString, (caddr_t) NULL},
{"xOffset", "Offset", XtRString, sizeof(char *),
  offset(xoffset), XtRString, (caddr_t) NULL},
{"yOffset", "Offset", XtRString, sizeof(char *),
  offset(yoffset), XtRString, (caddr_t) NULL},
{"paper", "Paper", XtRString, sizeof(char *),
  offset(paper), XtRString, (caddr_t) DEFAULT_PAPER},
{"altFont", "AltFont", XtRString, sizeof(char *),
  offset(_alt_font), XtRString, (caddr_t) ALTFONT},
{"makePk", "MakePk", XtRBoolean, sizeof(Boolean),
  offset(makepk), XtRString,
#if MAKE_TEX_PK_BY_DEFAULT
  "true"
#else
  "false"
#endif
  },
{"mfMode", "MfMode", XtRString, sizeof(char *),
  offset(mfmode), XtRString, MFMODE},
{"listFonts", "ListFonts", XtRBoolean, sizeof(Boolean),
  offset(_list_fonts), XtRString, "false"},
{"reverseVideo", "ReverseVideo", XtRBoolean, sizeof(Boolean),
  offset(reverse), XtRString, "false"},
{"warnSpecials", "WarnSpecials", XtRBoolean, sizeof(Boolean),
  offset(_warn_spec), XtRString, "false"},
{"hushLostChars", "Hush", XtRBoolean, sizeof(Boolean),
  offset(_hush_chars), XtRString, "false"},
{"hushChecksums", "Hush", XtRBoolean, sizeof(Boolean),
  offset(_hush_chk), XtRString, "false"},
{"safer", "Safer", XtRBoolean, sizeof(Boolean),
  offset(safer), XtRString, "false"},
#ifdef VMS
{"foreground", "Foreground", XtRString, sizeof(char *),
  offset(fore_color), XtRString, (caddr_t) NULL},
{"background", "Background", XtRString, sizeof(char *),
  offset(back_color), XtRString, (caddr_t) NULL},
#endif
{"iconGeometry", "IconGeometry", XtRString, sizeof(char *),
  offset(icon_geometry), XtRString, (caddr_t) NULL},
{"keepPosition", "KeepPosition", XtRBoolean, sizeof(Boolean),
  offset(keep_flag), XtRString, "false"},
#if	PS
{"postscript", "Postscript", XtRBoolean, sizeof(Boolean),
  offset(_postscript), XtRString, "true"},
{"prescan", "Prescan", XtRBoolean, sizeof(Boolean),
  offset(prescan), XtRString, "true"},
{"allowShell", "AllowShell", XtRBoolean, sizeof(Boolean),
  offset(allow_shell), XtRString, "false"},
#ifdef	PS_DPS
{"dps", "DPS", XtRBoolean, sizeof(Boolean),
  offset(useDPS), XtRString, "true"},
#endif
#ifdef	PS_NEWS
{"news", "News", XtRBoolean, sizeof(Boolean),
  offset(useNeWS), XtRString, "true"},
#endif
#ifdef	PS_GS
{"ghostscript", "Ghostscript", XtRBoolean, sizeof(Boolean),
  offset(useGS), XtRString, "true"},
{"gsSafer", "Safer", XtRBoolean, sizeof(Boolean),
  offset(gs_safer), XtRString, "true"},
{"gsAlpha", "Alpha", XtRBoolean, sizeof(Boolean),
  offset(gs_alpha), XtRString, "false"},
{"interpreter", "Interpreter", XtRString, sizeof(char *),
  offset(gs_path), XtRString, (caddr_t) GS_PATH},
{"palette", "Palette", XtRString, sizeof(char *),
  offset(gs_palette), XtRString, (caddr_t) "Color"},
#endif
#endif	/* PS */
{"copy", "Copy", XtRBoolean, sizeof(Boolean),
  offset(copy), XtRString, "false"},
{"thorough", "Thorough", XtRBoolean, sizeof(Boolean),
  offset(thorough), XtRString, "false"},
{"debugLevel", "DebugLevel", XtRString, sizeof(char *),
  offset(debug_arg), XtRString, (caddr_t) NULL},
{"version", "Version", XtRBoolean, sizeof(Boolean),
  offset(version_flag), XtRString, "false"},
#ifdef	BUTTONS
{"expert", "Expert", XtRBoolean, sizeof(Boolean),
  offset(expert), XtRString, "false"},
{"shrinkButton1", "ShrinkButton1", XtRInt, sizeof(int),
  offset(shrinkbutton[0]), XtRImmediate, (XtPointer) 0},
{"shrinkButton2", "ShrinkButton2", XtRInt, sizeof(int),
  offset(shrinkbutton[1]), XtRImmediate, (XtPointer) 0},
{"shrinkButton3", "ShrinkButton3", XtRInt, sizeof(int),
  offset(shrinkbutton[2]), XtRImmediate, (XtPointer) 0},
{"shrinkButton4", "ShrinkButton4", XtRInt, sizeof(int),
  offset(shrinkbutton[3]), XtRImmediate, (XtPointer) 0},
#endif
{"magnifierSize1", "MagnifierSize", XtRString, sizeof(char *),
  offset(mg_arg[0]), XtRString, (caddr_t) NULL},
{"magnifierSize2", "MagnifierSize", XtRString, sizeof(char *),
  offset(mg_arg[1]), XtRString, (caddr_t) NULL},
{"magnifierSize3", "MagnifierSize", XtRString, sizeof(char *),
  offset(mg_arg[2]), XtRString, (caddr_t) NULL},
{"magnifierSize4", "MagnifierSize", XtRString, sizeof(char *),
  offset(mg_arg[3]), XtRString, (caddr_t) NULL},
{"magnifierSize5", "MagnifierSize", XtRString, sizeof(char *),
  offset(mg_arg[4]), XtRString, (caddr_t) NULL},
#ifdef	GREY
{"grey", "Grey", XtRBoolean, sizeof(Boolean),
  offset(_use_grey), XtRString, "true"},
{"install", "Install", XtRBool3, sizeof(Bool3),
  offset(install), XtRString, "maybe"},
#endif
#ifdef GRID
{"grid1Color", "Grid1Color", XtRPixel, sizeof(Pixel),
  offset(_grid1_Pixel), XtRPixel, (caddr_t) &resource._grid1_Pixel},
{"grid1Color", "Grid1Color", XtRString, sizeof(char *),
  offset(grid1_color), XtRString, (caddr_t) NULL},
{"grid2Color", "Grid2Color", XtRPixel, sizeof(Pixel),
  offset(_grid2_Pixel), XtRPixel, (caddr_t) &resource._grid2_Pixel},
{"grid2Color", "Grid2Color", XtRString, sizeof(char *),
  offset(grid2_color), XtRString, (caddr_t) NULL},
{"grid3Color", "Grid3Color", XtRPixel, sizeof(Pixel),
  offset(_grid3_Pixel), XtRPixel, (caddr_t) &resource._grid3_Pixel},
{"grid3Color", "Grid3Color", XtRString, sizeof(char *),
  offset(grid3_color), XtRString, (caddr_t) NULL},
#endif /* GRID */
#ifdef HTEX
{"underLink", "UnderLink", XtRBoolean, sizeof(Boolean),
  offset(_underline_link), XtRString, (caddr_t) "true"},
{"wwwBrowser", "WWWBrowser", XtRString, sizeof(char *),
  offset(_browser), XtRString, (caddr_t) NULL},
{"urlBase", "URLBase", XtRString, sizeof(char *),
  offset(_URLbase), XtRString, (caddr_t) NULL},
#endif
/* BEGIN CHUNK xdvi.c 3 */
#ifdef SRC_SPECIALS
{"srcSpecialFormat", "SRCSpecialFormat", XtRInt, sizeof(int),
   offset(_src_specialFormat), XtRString, "1"},
{"srcJumpButton", "SRCJumpButton", XtRInt, sizeof(int),
   offset(_src_jumpButton), XtRString, "2"}, 
{"srcTickShape", "SRCTickShape", XtRInt, sizeof(int),
   offset(_src_tickShape), XtRString, "0"}, 
{"srcTickSize", "SRCTickSize", XtRString, sizeof(char *),
   offset(_src_tickSize), XtRString, "40x70"},
{"srcCursor", "SRCCursor", XtRInt, sizeof(int),
   offset(_src_cursor_shape), XtRString, "90"}, 
{"srcEditorCommand", "SRCEditorCommand", XtRString, sizeof(char *),
   offset(_src_editorCommand), XtRString, "emacsclient --no-wait '+%u' '%s'"},
{"srcVerbosity", "SRCVerbosity", XtRInt, sizeof(int),
   offset(_src_warn_verbosity), XtRString, "2"},
{"srcVisibility", "SRCVisibility", XtRBoolean, sizeof(Boolean),
   offset(_src_tickVisibility), XtRString, "false"},
{"srcMode", "SRCMode", XtRBoolean, sizeof(Boolean),
   offset(_src_evalMode), XtRString, "false"},
#endif  
/* END CHUNK xdvi.c 3 */
#ifdef	GREY
};

static	XtResource	app_pixel_resources[] = {	/* get these later */
#endif /* GREY */
{"foreground", "Foreground", XtRPixel, sizeof(Pixel),
  offset(_fore_Pixel), XtRString, XtDefaultForeground},
{"background", "Background", XtRPixel, sizeof(Pixel),
  offset(_back_Pixel), XtRString, XtDefaultBackground},
{"borderColor", "BorderColor", XtRPixel, sizeof(Pixel),
  offset(_brdr_Pixel), XtRPixel, (caddr_t) &resource._fore_Pixel},
{"highlight", "Highlight", XtRPixel, sizeof(Pixel),
  offset(_hl_Pixel), XtRPixel, (caddr_t) &resource._fore_Pixel},
{"cursorColor", "CursorColor", XtRPixel, sizeof(Pixel),
  offset(_cr_Pixel), XtRPixel, (caddr_t) &resource._fore_Pixel},
};
#undef	offset

static	_Xconst	char	*usagestr[] = {
	/* shrinkFactor */	"shrink",
#ifndef	VMS
	/* S */			"density",
	/* density */		silent,
#else
	/* density */		"density",
#endif
#ifdef	GREY
	/* gamma */		"g",
#endif
#ifdef GRID
        /* grid1 */             "color",
        /* grid2 */             "color",
        /* grid3 */             "color",
#endif /* GRID */
	/* p */			"pixels",
	/* margins */		"dimen",
	/* sidemargin */	"dimen",
	/* topmargin */		"dimen",
	/* offsets */		"dimen",
	/* xoffset */		"dimen",
	/* yoffset */		"dimen",
	/* paper */		"papertype",
	/* altfont */		"font",
	/* mfmode */		"mode-def",
	/* rv */		"^-l", "-rv",
#ifdef	BUTTONS
	/* shrinkbutton1 */	subst,
	/* shrinkbutton2 */	silent,
	/* shrinkbutton3 */	silent,
	/* shrinkbutton4 */	silent,
#endif
	/* mgs */		subst,
	/* mgs1 */		silent,
	/* mgs2 */		silent,
	/* mgs3 */		silent,
	/* mgs4 */		silent,
	/* mgs5 */		silent,
	/* bw */		"^-safer", "-bw <width>",
	/* fg */		"color",
	/* foreground */	silent,
	/* bg */		"color",
	/* background */	silent,
	/* hl */		"color",
	/* bd */		"^-hl", "-bd <color>",
	/* cr */		"color",
#ifndef VMS
	/* display */		"^-cr", "-display <host:display>",
#else
	/* display */		"^-cr", "-display <host::display>",
#endif
	/* geometry */		"^-cr", "-geometry <geometry>",
	/* icongeometry */	"geometry",
	/* iconic */		"^-icongeometry", "-iconic",
#ifdef	BUTTONS
	/* font */		"^-icongeometry", "-font <font>",
#endif
#ifdef	PS_GS
	/* interpreter */	"path",
	/* gspalette */		"monochrome|grayscale|color",
#endif
	/* debug */		"bitmask",
#ifdef HTEX
        /* browser */           "WWWbrowser",
        /* URLbase */           "base URL",
#endif
/* BEGIN CHUNK xdvi.c 4 */
#ifdef SRC_SPECIALS
	/* src_SpecialFormat */      "[0,1,2]",
	/* src_JumpButtons */        "button",
	/* src_TickShape */          "[0,1,2,3]",
	/* src_TickSize */           "geometry",
	/* src_Cursor */             "shape",
	/* src_EditorCommand */      "commandstring",
	/* src_verbosity */			 "[0,1,2]",
	/* _src_tickVisibility */	 silent,
	/* _src_evalMode */          silent,
#endif						
/* END CHUNK xdvi.c 4 */
	/* [dummy] */		"z"
};

static char *TmpDir=NULL;

#ifndef MOTIF

#ifdef	NOQUERY
#define	drawWidgetClass	widgetClass
#else

/* ARGSUSED */
static	XtGeometryResult
QueryGeometry(w, constraints, reply)
	Widget	w;
	XtWidgetGeometry *constraints, *reply;
{
	reply->request_mode = CWWidth | CWHeight;
	reply->width = page_w;
	reply->height = page_h;
	return XtGeometryAlmost;
}

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

#ifdef	lint
WidgetClassRec	widgetClassRec;
#endif

	/* if the following gives you trouble, just compile with -DNOQUERY */
static	WidgetClassRec	drawingWidgetClass = {
  {
    /* superclass         */    &widgetClassRec,
    /* class_name         */    "Draw",
    /* widget_size        */    sizeof(WidgetRec),
    /* class_initialize   */    NULL,
    /* class_part_initialize*/  NULL,
    /* class_inited       */    FALSE,
    /* initialize         */    NULL,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    NULL,
    /* num_resources      */    0,
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    FALSE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    FALSE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    XtInheritResize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    NULL,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    XtInheritAcceptFocus,
    /* version            */    XtVersion,
    /* callback_offsets   */    NULL,
    /* tm_table           */    XtInheritTranslations,
    /* query_geometry       */  QueryGeometry,
    /* display_accelerator  */  XtInheritDisplayAccelerator,
    /* extension            */  NULL
  }
};

#define	drawWidgetClass	&drawingWidgetClass

#endif /* NOQUERY */
#endif /* MOTIF */

static	Arg	vport_args[] = {
#ifndef MOTIF
#ifdef	BUTTONS
	{XtNborderWidth,	(XtArgVal) 0},
	{XtNtop,		(XtArgVal) XtChainTop},
	{XtNbottom,		(XtArgVal) XtChainBottom},
	{XtNleft,		(XtArgVal) XtChainLeft},
	{XtNright,		(XtArgVal) XtChainRight},
#endif
	{XtNallowHoriz,		(XtArgVal) True},
	{XtNallowVert,		(XtArgVal) True},
#else /* MOTIF */
	{XmNscrollingPolicy,	(XtArgVal) XmAUTOMATIC},
	{XmNborderWidth,	(XtArgVal) 0},
	{XmNleftAttachment,	(XtArgVal) XmATTACH_FORM},
	{XmNtopAttachment,	(XtArgVal) XmATTACH_FORM},
	{XmNbottomAttachment,	(XtArgVal) XmATTACH_FORM},
	{XmNrightAttachment,	(XtArgVal) XmATTACH_FORM},
#endif /* MOTIF */
};

static	Arg	draw_args[] = {
#ifndef MOTIF
	{XtNwidth,		(XtArgVal) 0},
	{XtNheight,		(XtArgVal) 0},
	{XtNx,			(XtArgVal) 0},
	{XtNy,			(XtArgVal) 0},
	{XtNlabel,		(XtArgVal) ""},
#else /* MOTIF */
	{XmNwidth,		(XtArgVal) 0},
	{XmNheight,		(XtArgVal) 0},
	{XmNbottomAttachment,	(XtArgVal) XmATTACH_WIDGET},
#endif /* MOTIF */
};

#ifdef	BUTTONS
static	Arg	form_args[] = {
#ifndef MOTIF
	{XtNdefaultDistance, (XtArgVal) 0},
#else /* MOTIF */
	{XmNhorizontalSpacing, (XtArgVal) 0},
	{XmNverticalSpacing, (XtArgVal) 0},
#endif /* MOTIF */
};
#endif

#ifdef HTEX

static Arg      pane_args[] = {
        {XtNorientation,        (XtArgVal) XtorientVertical},
};

/* Create the anchor information stuff at the bottom of the page */

/* Anchor search: a dialog box */
char anchorsearchstring[1024];
char anchorask[] = "Access new URL:";
static  Arg     anchorsearch_args[] = {
        {XtNwidth,      (XtArgVal) 500},
        {XtNheight,     (XtArgVal) 70},
        {XtNlabel,      (XtArgVal) anchorask},
        {XtNvalue,      (XtArgVal) anchorsearchstring},
        {XtNmin,        (XtArgVal) 70},
        {XtNscrollHorizontal,   (XtArgVal) XawtextScrollWhenNeeded},
};

/* anchorinfo = Ascii text widget */
static  Arg     anchorinfo_args[] = {
        {XtNwidth,      (XtArgVal) 500},
        {XtNheight,     (XtArgVal) 20},
        {XtNstring,     (XtArgVal) ""},
        {XtNmin,        (XtArgVal) 20},
        {XtNscrollVertical,     (XtArgVal) XawtextScrollWhenNeeded},
        {XtNscrollHorizontal,   (XtArgVal) XawtextScrollWhenNeeded},
};

#endif

#else	/* not TOOLKIT */

static	char	*display;
static	char	*geometry;
static	char	*margins;
static	char	*offsets;
static	Boolean	hush;
static	Boolean	iconic	= False;

#define	ADDR(x)	(caddr_t) &resource.x

static	struct option {
	_Xconst	char	*name;
	_Xconst	char	*resource;
	enum {FalseArg, TrueArg, StickyArg, SepArg}
			argclass;
	enum {BooleanArg, Bool3Arg, StringArg, NumberArg, FloatArg}
			argtype;
	int		classcount;
	_Xconst	char	*usagestr;
	caddr_t		address;
}	options[] = {
{"+",		NULL,		StickyArg, StringArg, 1,
  NULL,		(caddr_t) &curr_page},
{"-s",		"shrinkFactor", SepArg, NumberArg, 1,
  "shrink",	(caddr_t) &shrink_factor},
#ifndef VMS
{"-S",		NULL,		SepArg, NumberArg, 2,
  "density",	ADDR(_density)},
{"-density",	"densityPercent", SepArg, NumberArg, 1,
  silent,	ADDR(_density)},
#else
{"-density",	"densityPercent", SepArg, NumberArg, 1,
  "density",	ADDR(_density)},
#endif
#ifdef	GREY
{"-nogrey",	NULL,		FalseArg, BooleanArg, 2,
  NULL,		ADDR(_use_grey)},
{"+nogrey",	"grey",		TrueArg, BooleanArg, 1,
  NULL,		ADDR(_use_grey)},
{"-gamma",	"gamma",	SepArg,	FloatArg, 1,
  "g",		ADDR(_gamma)},
{"-install",	NULL,		TrueArg, Bool3Arg, 2,
  NULL,		ADDR(install)},
{"-noinstall",	"install",	FalseArg, Bool3Arg, 1,
  NULL,		ADDR(install)},
#endif
#ifdef GRID
{"-grid1",      "grid1Color",   SepArg, StringArg, 1,
  "color",      ADDR(grid1_color)},
{"-grid2",      "grid2Color",   SepArg, StringArg, 1,
  "color",      ADDR(grid2_color)},
{"-grid3",      "grid3Color",   SepArg, StringArg, 1,
  "color",      ADDR(grid3_color)},
#endif /* GRID */
{"-p",		"pixelsPerInch", SepArg, NumberArg, 1,
  "pixels",	ADDR(_pixels_per_inch)},
{"-margins",	"Margin",	SepArg,	StringArg, 3,
  "dimen",	(caddr_t) &margins},
{"-sidemargin",	"sideMargin",	SepArg,	StringArg, 1,
  "dimen",	ADDR(sidemargin)},
{"-topmargin",	"topMargin",	SepArg,	StringArg, 1,
  "dimen",	ADDR(topmargin)},
{"-offsets",	"Offset",	SepArg,	StringArg, 3,
  "dimen",	(caddr_t) &offsets},
{"-xoffset",	"xOffset",	SepArg,	StringArg, 1,
  "dimen",	ADDR(xoffset)},
{"-yoffset",	"yOffset",	SepArg,	StringArg, 1,
  "dimen",	ADDR(yoffset)},
{"-paper",	"paper",	SepArg,	StringArg, 1,
  "papertype",	ADDR(paper)},
{"-altfont",	"altFont",	SepArg,	StringArg, 1,
  "font",	ADDR(_alt_font)},
{"-nomakepk",	"makePk",	FalseArg, BooleanArg, 2,
  NULL,		ADDR(makepk)},
{"+nomakepk",	"makePk",	TrueArg, BooleanArg, 1,
  NULL,		ADDR(makepk)},
{"-mfmode",	"mfMode",	SepArg,	StringArg, 1,
  "mode-def",	ADDR(mfmode)},
{"-l",		NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(_list_fonts)},
{"+l",		"listFonts",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(_list_fonts)},
{"-rv",		NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(reverse)},
{"+rv",		"reverseVideo",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(reverse)},
{"-mgs",	NULL,		SepArg, StringArg, 2,
  subst,	ADDR(mg_arg[0])},
{"-mgs1",	"magnifierSize1",SepArg, StringArg, 1,
  silent,	ADDR(mg_arg[0])},
{"-mgs2",	"magnifierSize2",SepArg, StringArg, 1,
  silent,	ADDR(mg_arg[1])},
{"-mgs3",	"magnifierSize3",SepArg, StringArg, 1,
  silent,	ADDR(mg_arg[2])},
{"-mgs4",	"magnifierSize4",SepArg, StringArg, 1,
  silent,	ADDR(mg_arg[3])},
{"-mgs5",	"magnifierSize5",SepArg, StringArg, 1,
  silent,	ADDR(mg_arg[4])},
{"-warnspecials", NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(_warn_spec)},
{"+warnspecials", "warnSpecials", FalseArg, BooleanArg, 1,
  NULL,		ADDR(_warn_spec)},
{"-hush",	NULL,		TrueArg, BooleanArg, 6,
  NULL,		(caddr_t) &hush},
{"+hush",	"Hush",		FalseArg, BooleanArg, 5,
  NULL,		(caddr_t) &hush},
{"-hushchars",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(_hush_chars)},
{"+hushchars",	"hushLostChars", FalseArg, BooleanArg, 1,
  NULL,		ADDR(_hush_chars)},
{"-hushchecksums", NULL,	TrueArg, BooleanArg, 2,
  NULL,		ADDR(_hush_chk)},
{"+hushchecksums","hushChecksums", FalseArg, BooleanArg, 1,
  NULL,		ADDR(_hush_chk)},
{"-safer",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(safer)},
{"+safer",	"safer",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(safer)},
{"-bw",		NULL,		SepArg,	NumberArg, 2,
  "width",	(caddr_t) &bwidth},
{"-borderwidth", "borderWidth",	SepArg,	NumberArg, 1,
  silent,	(caddr_t) &bwidth},
{"-fg",		NULL,		SepArg,	StringArg, 2,
  "color",	ADDR(fore_color)},
{"-foreground",	"foreground",	SepArg,	StringArg, 1,
  silent,	ADDR(fore_color)},
{"-bg",		NULL,		SepArg,	StringArg, 2,
  "color",	ADDR(back_color)},
{"-background",	"background",	SepArg,	StringArg, 1,
  silent,	ADDR(back_color)},
{"-hl",		"highlight",	SepArg,	StringArg, 1,
  "color",	(caddr_t) &high_color},
{"-bd",		NULL,		SepArg,	StringArg, 2,
  "color",	(caddr_t) &brdr_color},
{"-bordercolor","borderColor",	SepArg,	StringArg, 1,
  silent,	(caddr_t) &brdr_color},
{"-cr",		"cursorColor",	SepArg,	StringArg, 1,
  "color",	(caddr_t) &curs_color},
#ifndef VMS
{"-display",	NULL,		SepArg,	StringArg, 1,
  "host:display", (caddr_t) &display},
#else
{"-display",	NULL,		SepArg,	StringArg, 1,
  "host::display", (caddr_t) &display},
#endif
{"-geometry",	"geometry",	SepArg,	StringArg, 1,
  "geometry",	(caddr_t) &geometry},
{"-icongeometry","iconGeometry",StickyArg, StringArg, 1,
  "geometry",	ADDR(icon_geometry)},
{"-iconic",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		(caddr_t) &iconic},
{"+iconic",	"iconic",	FalseArg, BooleanArg, 1,
  NULL,		(caddr_t) &iconic},
{"-keep",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(keep_flag)},
{"+keep",	"keepPosition",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(keep_flag)},
{"-copy",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(copy)},
{"+copy",	"copy",		FalseArg, BooleanArg, 1,
  NULL,		ADDR(copy)},
{"-thorough",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(thorough)},
{"+thorough",	"thorough",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(thorough)},
#if	PS
{"-nopostscript", NULL,		FalseArg, BooleanArg, 2,
  NULL,		ADDR(_postscript)},
{"+nopostscript", "postscript",	TrueArg, BooleanArg, 1,
  NULL,		ADDR(_postscript)},
{"-noscan", NULL,		FalseArg, BooleanArg, 2,
  NULL,		ADDR(prescan)},
{"+noscan",	"prescan",	TrueArg, BooleanArg, 1,
  NULL,		ADDR(prescan)},
{"-allowshell",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(allow_shell)},
{"+allowshell",	"allowShell",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(allow_shell)},
#ifdef	PS_DPS
{"-nodps",	NULL,		FalseArg, BooleanArg, 2,
  NULL,		ADDR(useDPS)},
{"+nodps",	"dps",		TrueArg, BooleanArg, 1,
  NULL,		ADDR(useDPS)},
#endif
#ifdef	PS_NEWS
{"-nonews",	NULL,		FalseArg, BooleanArg, 2,
  NULL,		ADDR(useNeWS)},
{"+nonews",	"news",		TrueArg, BooleanArg, 1,
  NULL,		ADDR(useNeWS)},
#endif
#ifdef	PS_GS
{"-noghostscript",NULL,		FalseArg, BooleanArg, 2,
  NULL,		ADDR(useGS)},
{"+noghostscript","ghostscript", TrueArg, BooleanArg, 1,
  NULL,		ADDR(useGS)},
{"-nogssafer",	NULL,		FalseArg, BooleanArg, 2,
  NULL,		ADDR(gs_safer)},
{"+nogssafer",	"gsSafer",	TrueArg, BooleanArg, 1,
  NULL,		ADDR(gs_safer)},
{"-nogsalpha",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(gs_alpha)},
{"+nogsalpha",	"gsAlpha",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(gs_alpha)},
{"-interpreter", "interpreter",	SepArg,	StringArg, 1,
  "path",	ADDR(gs_path)},
{"-gspalette",	"palette",	SepArg,	StringArg, 1,
  "monochrome|grayscale|color",	ADDR(gs_palette)},
#endif
#endif	/* PS */
{"-debug",	"debugLevel",	SepArg,	StringArg, 1,
  "bitmask",	ADDR(debug_arg)},
{"-version",	NULL,		TrueArg, BooleanArg, 2,
  NULL,		ADDR(version_flag)},
{"+version",	"version",	FalseArg, BooleanArg, 1,
  NULL,		ADDR(version_flag)},
#ifdef HTEX
{"-underlink",  "underLink",    TrueArg, BooleanArg, 2,
  NULL,         ADDR(_underline_link)},
{"+underlink",  "underLink",    FalseArg, BooleanArg, 1,
  NULL,         ADDR(_underline_link)},
{"-browser",    "wwwBrowser",   SepArg, StringArg, 1,   ADDR(_browser)},
{"-base",       "urlBase",      SepArg, StringArg, 1,   ADDR(_URLbase)},
#endif
/* BEGIN CHUNK xdvi.c 5 */
#ifdef SRC_SPECIALS
{"-srcSpecialFormat", "srcSpecialFormat", SepArg,    NumberArg,    1, "[0,1,2]", (caddr_t)&src_specialFormat},
{"-srcJumpButton",    "srcJumpButton",    SepArg,    NumberArg,    1, "button", (caddr_t)&src_jumpButton},
{"-srcTickShape",     "srcTickShape",     SepArg,    NumberArg,    1, "[0,1,2,3]", (caddr_t)&src_tickShape},
{"-srcTickSize",      "srcTickSize",      SepArg,    StringArg,    1, "geometry", "40x70"},
{"-srcCursor",        "srcCursor",        SepArg,    NumberArg,    1, "shape", (caddr_t)&src_cursor_shape},
{"-srcEditorCommand", "srcEditorCommand", SepArg,    StringArg,    1, "commandstring", (caddr_t)&src_editorCommand},
{"-srcVerbosity",     "srcVerbosity",     SepArg,    NumberArg,    1, "[0,1,2]", (caddr_t)&src_warn_verbosity},
{"-srcVisibility",    "srcVisibility",    TrueArg,   BooleanArg,   2, NULL, ADDR(_src_tickVisibility)},
{"+srcVisibility",    "srcVisibility",    FalseArg,  BooleanArg,   1, NULL, ADDR(_src_tickVisibility)},  
{"-srcMode",          "srcMode",		  TrueArg,   BooleanArg,   2, NULL, ADDR(_src_evalMode)},
{"+srcMode",          "srcMode",          FalseArg,  BooleanArg,   1, NULL, ADDR(_src_evalMode)},  
#endif  
/* END CHUNK xdvi.c 5 */
};

#endif	/* not TOOLKIT */

static	NORETURN void
usage() {
#ifdef	TOOLKIT
	XrmOptionDescRec *opt;
	_Xconst	char	**usageptr = usagestr;
#else
	struct option	*opt;
#endif
	_Xconst char	**sv	= subst_val;
	_Xconst char	*str1;
	_Xconst char	*str2;
	int		col	= 23;
	int		n;

#ifdef Omega
        Fputs("Usage: oxdvi [+[<page>]]", stderr);
#else
	Fputs("Usage: xdvi [+[<page>]]", stderr);
#endif
	for (opt = options; opt < options + XtNumber(options); ++opt) {
#ifdef	TOOLKIT
	    str1 = opt->option;
	    if (*str1 != '-') continue;
	    str2 = NULL;
	    if (opt->argKind != XrmoptionNoArg) {
		str2 = *usageptr++;
		if (str2 == silent) continue;
		if (str2 == subst) {
		    str1 = *sv++;
		    str2 = NULL;
		}
	    }
	    for (;;) {
		n = strlen(str1) + 3;
		if (str2 != NULL) n += strlen(str2) + 3;
		if (col + n < 80) Putc(' ', stderr);
		else {
		    Fputs("\n\t", stderr);
		    col = 8 - 1;
		}
		if (str2 == NULL)
		    Fprintf(stderr, "[%s]", str1);
		else
		    Fprintf(stderr, "[%s <%s>]", str1, str2);
		col += n;
		if (**usageptr != '^'
			 || strcmp(*usageptr + 1, opt->option) != 0) break;
		++usageptr;
		str1 = *usageptr++;
		str2 = NULL;
	    }
#else /* not TOOLKIT */
	    str1 = opt->name;
	    str2 = opt->usagestr;
	    if (*str1 != '-' || str2 == silent) continue;
	    if (str2 == subst) {
		str1 = *sv++;
		str2 = NULL;
	    }
	    n = strlen(str1) + 3;
	    if (str2 != NULL) n += strlen(str2) + 3;
	    if (col + n < 80) Putc(' ', stderr);
	    else {
		Fputs("\n\t", stderr);
		col = 8 - 1;
	    }
	    if (str2 == NULL)
		Fprintf(stderr, "[%s]", str1);
	    else
		Fprintf(stderr, "[%s <%s>]", str1, str2);
	    col += n;
#endif /* not TOOLKIT */
	}
	if (col + 9 < 80) Putc(' ', stderr);
	else Fputs("\n\t", stderr);
#ifdef SELFILE
        Fputs("[dvi_file]\n", stderr);
#else
	Fputs("dvi_file\n", stderr);
#endif
	exit(1);
}

static	int
atopix(arg)
	_Xconst	char	*arg;
{
	int		len	= strlen(arg);
	_Xconst char	*arg_end = arg;
	char		tmp[11];
	double		factor;

	while ((*arg_end >= '0' && *arg_end <= '9') || *arg_end == '.')
	    if (arg_end >= arg + XtNumber(tmp) - 1) return 0;
	    else ++arg_end;
	bcopy(arg, tmp, arg_end - arg);
	tmp[arg_end - arg] = '\0';

#if A4
	factor = 1.0 / 2.54;	/* cm */
#else
	factor = 1.0;		/* inches */
#endif
	if (len > 2)
	    switch (arg[len - 2] << 8 | arg[len - 1]) {
#if A4
		case 'i' << 8 | 'n':  factor = 1.0;			break;
#else
		case 'c' << 8 | 'm':  factor = 1.0 / 2.54;		break;
#endif
		case 'm' << 8 | 'm':  factor = 1.0 / 25.4;		break;
		case 'p' << 8 | 't':  factor = 1.0 / 72.27;		break;
		case 'p' << 8 | 'c':  factor = 12.0 / 72.27;		break;
		case 'b' << 8 | 'p':  factor = 1.0 / 72.0;		break;
		case 'd' << 8 | 'd':  factor = 1238.0 / 1157.0 / 72.27;	break;
		case 'c' << 8 | 'c':  factor = 12 * 1238.0 / 1157.0 / 72.27;
									break;
		case 's' << 8 | 'p':  factor = 1.0 / 72.27 / 65536;	break;
	    }

	return factor * atof(tmp) * pixels_per_inch + 0.5;
}

#ifdef GRID
/* extract the unit used in paper size specification */
/* the information is used to decide the initial grid separation */
static  int
atopixunit(arg)
        _Xconst char    *arg;
{
        int     len     = strlen(arg);

        return (len > 2 && arg[len - 2] == 'c' && arg[len - 1] == 'm' ?
                1.0 / 2.54 : 1.0) * pixels_per_inch + 0.5;
}
#endif /* GRID */

/**
 **	Main programs start here.
 **/

#ifdef	TOOLKIT

#ifdef GREY
static	Arg	temp_args1[] = {
	{XtNdepth,	(XtArgVal) 0},
	{XtNvisual,	(XtArgVal) 0},
	{XtNcolormap,	(XtArgVal) 0},
};
#define	temp_args1a	(temp_args1 + 2)
#endif

static	Arg	temp_args2[] = {
	{XtNiconX,	(XtArgVal) 0},
	{XtNiconY,	(XtArgVal) 0},
};

static	Arg	temp_args3 = {XtNborderWidth,	(XtArgVal) &bwidth};

static	Pixmap	icon_pm;
static	Arg	temp_args4 = {XtNiconPixmap,	(XtArgVal) &icon_pm};

static	Arg	temp_args5[] = {
	{XtNtitle,	(XtArgVal) 0},
	{XtNiconName,	(XtArgVal) 0},
	{XtNinput,	(XtArgVal) True},
};

static	Arg	set_wh_args[] = {
	{XtNwidth,	(XtArgVal) 0},
	{XtNheight,	(XtArgVal) 0},
};


#ifdef GREY

/*
 *	Alternate routine to convert color name to Pixel (needed to substitute
 *	"black" or "white" for BlackPixelOfScreen, etc., since a different
 *	visual and colormap are in use).
 */

#if XtSpecificationRelease >= 5

/*ARGSUSED*/
static	Boolean
XdviCvtStringToPixel(dpy, args, num_args, fromVal, toVal, closure_ret)
	Display		*dpy;
	XrmValuePtr	args;
	Cardinal	*num_args;
	XrmValuePtr	fromVal;
	XrmValuePtr	toVal;
	XtPointer	*closure_ret;
{
	XrmValue	replacement_val;
	Boolean		default_is_fg;

	if ((strcmp((String) fromVal->addr, XtDefaultForeground) == 0
	    && (default_is_fg = True))
	  || (strcmp((String) fromVal->addr, XtDefaultBackground) == 0
	    && ((default_is_fg = False), True))) {
	    replacement_val.size = sizeof(String);
	    replacement_val.addr = (default_is_fg == resource.reverse)
	      ? "white" : "black";
	    fromVal = &replacement_val;
	}

	return
	  XtCvtStringToPixel(dpy, args, num_args, fromVal, toVal, closure_ret);
}

#else /* XtSpecificationRelease < 5 */

/*
 *	Copied from the X11R4 source code.
 */

#define	done(type, value) \
	{							\
	    if (toVal->addr != NULL) {				\
		if (toVal->size < sizeof(type)) {		\
		    toVal->size = sizeof(type);			\
		    return False;				\
		}						\
		*(type*)(toVal->addr) = (value);		\
	    }							\
	    else {						\
		static type static_val;				\
		static_val = (value);				\
		toVal->addr = (XtPointer)&static_val;		\
	    }							\
	    toVal->size = sizeof(type);				\
	    return True;					\
	}

static	Boolean
XdviCvtStringToPixel(dpy, args, num_args, fromVal, toVal, closure_ret)
    Display*	dpy;
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
    XtPointer	*closure_ret;
{
    String	    str = (String)fromVal->addr;
    XColor	    screenColor;
    XColor	    exactColor;
    Screen	    *screen;
    Colormap	    colormap;
    Status	    status;
    String          params[1];
    Cardinal	    num_params=1;

    if (*num_args != 2)
     XtErrorMsg("wrongParameters", "cvtStringToPixel",
		   "XtToolkitError",
	"String to pixel conversion needs screen and colormap arguments",
        (String *)NULL, (Cardinal *)NULL);

    screen = *((Screen **) args[0].addr);
    colormap = *((Colormap *) args[1].addr);

    if (strcmp(str, XtDefaultBackground) == 0) {
	*closure_ret = False;
	str = (resource.reverse ? "black" : "white");
    }
    else if (strcmp(str, XtDefaultForeground) == 0) {
	*closure_ret = False;
	str = (resource.reverse ? "white" : "black");
    }

    if (*str == '#') {  /* some color rgb definition */

        status = XParseColor(DisplayOfScreen(screen), colormap,
			     (char*)str, &screenColor);

        if (status != 0)
           status = XAllocColor(DisplayOfScreen(screen), colormap,
                                &screenColor);
    } else  /* some color name */

        status = XAllocNamedColor(DisplayOfScreen(screen), colormap,
                                  (char*)str, &screenColor, &exactColor);
    if (status == 0) {
	params[0] = str;
	XtWarningMsg("noColormap", "cvtStringToPixel",
			"XtToolkitError",
                 "Cannot allocate colormap entry for \"%s\"",
                  params,&num_params);
	return False;
    } else {
	*closure_ret = (char*)True;
        done(Pixel, screenColor.pixel);
    }
}
#undef	done

#endif /* XtSpecificationRelease < 5 */

/*
 *	Convert string to yes/no/maybe.  Adapted from the X toolkit.
 */

/*ARGSUSED*/
Boolean XdviCvtStringToBool3(dpy, args, num_args, fromVal, toVal, closure_ret)
	Display		*dpy;
	XrmValuePtr	args;
	Cardinal	*num_args;
	XrmValuePtr	fromVal;
	XrmValuePtr	toVal;
	XtPointer	*closure_ret;
{
	String		str	= (String)fromVal->addr;
	static Bool3	value;

	if (   memicmp(str, "true", 5) == 0
	    || memicmp(str, "yes", 4) == 0
	    || memicmp(str, "on", 3) == 0
	    || memicmp(str, "1", 2) == 0)	value = True;

	else if (memicmp(str, "false", 6) == 0
	    ||   memicmp(str, "no", 3) == 0
	    ||   memicmp(str, "off", 4) == 0
	    ||   memicmp(str, "0", 2) == 0)	value = False;

	else if (memicmp(str, "maybe", 6) == 0)	value = Maybe;

	else {
	    XtDisplayStringConversionWarning(dpy, str, XtRBoolean);
	    return False;
	}

	if (toVal->addr != NULL) {
	    if (toVal->size < sizeof(Bool3)) {
		toVal->size = sizeof(Bool3);
		return False;
	    }
	    *(Bool3 *)(toVal->addr) = value;
	}
	else toVal->addr = (XPointer) &value;

	toVal->size = sizeof(Bool3);
	return True;
}

#endif /* GREY */

#else	/* not TOOLKIT */

struct _resource	resource = {
	/* density		*/	40,
#ifdef	GREY
	/* gamma		*/	1.0,
#endif
	/* pixels_per_inch	*/	BDPI,
        /* _delay_rulers	*/	True,
	/* _tick_length		*/	4,
	/* _tick_units		*/	"pt",
	/* sidemargin		*/	NULL,
	/* topmargin		*/	NULL,
	/* xoffset		*/	NULL,
	/* yoffset		*/	NULL,
	/* paper		*/	DEFAULT_PAPER,
	/* alt_font		*/	ALTFONT,
	/* makepk		*/	MAKE_TEX_PK_BY_DEFAULT,
	/* mfmode		*/	MFMODE,
	/* list_fonts		*/	False,
	/* reverse		*/	False,
	/* warn_spec		*/	False,
	/* hush_chars		*/	False,
	/* hush_chk		*/	False,
	/* safer		*/	False,
	/* fore_color		*/	NULL,
	/* back_color		*/	NULL,
	/* fore_Pixel		*/	(Pixel) 0,
	/* back_Pixel		*/	(Pixel) 0,
	/* icon_geometry	*/	NULL,
	/* keep_flag		*/	False,
	/* copy			*/	False,
	/* thorough		*/	False,
#if	PS
	/* postscript		*/	True,
	/* prescan		*/	True,
	/* allow_shell		*/	False,
#ifdef	PS_DPS
	/* useDPS		*/	True,
#endif
#ifdef	PS_NEWS
	/* useNeWS		*/	True,
#endif
#ifdef	PS_GS
	/* useGS		*/	True,
	/* gs_safer		*/	True,
	/* gs_alpha		*/	False,
	/* gs_path		*/	GS_PATH,
	/* gs_palette		*/	"Color",
#endif
#endif	/* PS */
	/* debug_arg		*/	NULL,
	/* version_flag		*/	False,
	/* mg_arg		*/	{NULL, NULL, NULL, NULL, NULL},
#ifdef	GREY
	/* use_grey		*/	True,
	/* install		*/	Maybe,
#endif
#ifdef GRID
        /* grid_mode            */      0,
        /* grid1_color          */      NULL,
        /* grid2_color          */      NULL,
        /* grid3_color          */      NULL,
#endif /* GRID */                                       
#ifdef HTEX
        /* _underline_link      */      True,
        /* _browser             */      (char *) NULL,
        /* _URLbase             */      (char *) NULL,
#endif
/* BEGIN CHUNK xdvi.c 6 */
#ifdef SRC_SPECIALS
		/* _src_specialFormat   */		2,		
		/* _src_jumpButton      */		1,		
		/* _src_tickShape      */		1,		
		/* _src_tickSize      */		"40x70",		
		/* _src_cursor_shape  */  		90,		
		/* _src_editorCommand */		"emacsclient --no-wait '+%u' '%s'",		
		/* _src_warn_verbosity */		2,
		/* _src_tickVisibility */		False,
		/* _src_evalMode */				False,
#endif						
/* END CHUNK xdvi.c 6 */
};

static	Pixel
string_to_pixel(strp)		/* adapted from the toolkit */
	char	**strp;
{
	char	*str = *strp;
	Status	status;
	XColor	color, junk;

	if (*str == '#') {	/* an rgb definition */
	    status = XParseColor(DISP, our_colormap, str, &color);
	    if (status != 0)
		status = XAllocColor(DISP, our_colormap, &color);
	}
	else	/* a name */
	    status = XAllocNamedColor(DISP, our_colormap, str, &color, &junk);
	if (status == 0) {
	    Fprintf(stderr, "Cannot allocate colormap entry for \"%s\"\n", str);
	    *strp = NULL;
	    return (Pixel) 0;
	}
	return color.pixel;
}

/*
 *	Process the option table.  This is not guaranteed for all possible
 *	option tables, but at least it works for this one.
 */

static	void
parse_options(argc, argv)
	int argc;
	char **argv;
{
	char	**arg;
	char	**argvend = argv + argc;
	char	*optstring;
	caddr_t	addr;
	struct option *opt, *lastopt, *candidate;
	int	len1, len2, matchlen;

	/*
	 * Step 1.  Process command line options.
	 */
	for (arg = argv + 1; arg < argvend; ++arg) {
	    len1 = strlen(*arg);
	    candidate = NULL;
	    matchlen = 0;
	    for (opt = options; opt < options + XtNumber(options); ++opt) {
		len2 = strlen(opt->name);
		if (opt->argclass == StickyArg) {
		    if (matchlen <= len2 && !strncmp(*arg, opt->name, len2)) {
			candidate = opt;
			matchlen = len2;
		    }
		}
		else if (len1 <= len2 && matchlen <= len1 &&
		    !strncmp(*arg, opt->name, len1)) {
		    if (len1 == len2) {
			candidate = opt;
			break;
		    }
		    if (matchlen < len1) candidate = opt;
		    else if (candidate && candidate->argclass != StickyArg)
			candidate = NULL;
		    matchlen = len1;
		}
	    }
	    if (candidate == NULL) {
		if (**arg == '-' || dvi_name) usage();
		else {
                    /* need to make sure that dvi_name can be freed safely */
                    dvi_name = xmalloc((unsigned) strlen(*arg)+1);
                    Strcpy(dvi_name, *arg);
		    continue;
		}
	    }
		/* flag it for subsequent processing */
	    candidate->resource = (char *) candidate;
		/* store the value */
	    addr = candidate->address;
	    switch (candidate->argclass) {
		case FalseArg:	*((Boolean *) addr) = False; continue;
		case TrueArg:	*((Boolean *) addr) = True; continue;
		case StickyArg:	optstring = *arg + strlen(candidate->name);
		    break;
		case SepArg:
		    ++arg;
		    if (arg >= argvend) usage();
		    optstring = *arg;
		    break;
	    }
	    switch (candidate->argtype) {
		case StringArg:	*((char **) addr) = optstring; break;
		case NumberArg:	*((int *) addr) = atoi(optstring); break;
		case FloatArg:  *((float *) addr) = atof(optstring); break;
		default:  ;
	    }
	}
	/*
	 * Step 2.  Propagate classes for command line arguments.  Backwards.
	 */
	for (opt = options + XtNumber(options) - 1; opt >= options; --opt)
	    if (opt->resource == (char *) opt) {
		addr = opt->address;
		lastopt = opt + opt->classcount;
		for (candidate = opt; candidate < lastopt; ++candidate) {
		    if (candidate->resource != NULL) {
			switch (opt->argtype) {
			    case BooleanArg:
			    case Bool3Arg:	/* same type as Boolean */
				*((Boolean *) candidate->address) =
				    *((Boolean *) addr);
				break;
			    case StringArg:
				*((char **) candidate->address) =
				    *((char **) addr);
				break;
			    case NumberArg:
				*((int *) candidate->address) = *((int *) addr);
				break;
			    case FloatArg:
				*((float *) candidate->address) =
				    *((float *) addr);
				break;
			}
			candidate->resource = NULL;
		    }
		}
	    }

	if ((DISP = XOpenDisplay(display)) == NULL)
	    oops("Can't open display");
	SCRN = DefaultScreenOfDisplay(DISP);
	/*
	 * Step 3.  Handle resources (including classes).
	 */
	for (opt = options; opt < options + XtNumber(options); ++opt)
	    if (opt->resource &&
		    ((optstring = XGetDefault(DISP, prog, opt->resource)) ||
		    (optstring = XGetDefault(DISP, "XDvi", opt->resource))))
		{
		    lastopt = opt + opt->classcount;
		    for (candidate = opt; candidate < lastopt; ++candidate)
			if (candidate->resource != NULL) switch (opt->argtype) {
			    case Bool3Arg:
				if (       memicmp(optstring, "maybe", 6) == 0)
				{
				    * (Bool3 *) candidate->address = Maybe;
				    break;
				}
				/* otherwise, fall through; the underlying */
				/* types of Bool3 and Boolean are the same. */
			    case BooleanArg:
				* (Boolean *) candidate->address =
				    (  memicmp(optstring, "true", 5) == 0
				    || memicmp(optstring, "yes", 4) == 0
				    || memicmp(optstring, "on", 3) == 0
				    || memicmp(optstring, "1", 2) == 0);
				break;
			    case StringArg:
				* (char **) candidate->address = optstring;
				break;
			    case NumberArg:
				* (int *) candidate->address = atoi(optstring);
				break;
			    case FloatArg:
				* (float *) candidate->address =
				    atof(optstring);
			}
		}
}

#endif	/* not TOOLKIT */

static	_Xconst	char	*paper_types[] = {
	"us",		"8.5x11in",
	"usr",		"11x8.5in",
	"legal",	"8.5x14in",
	"foolscap",	"13.5x17.0in",	/* ??? */

	/* ISO `A' formats, Portrait */
	"a1",		"59.4x84.0cm",
	"a2",		"42.0x59.4cm",
	"a3",		"29.7x42.0cm",
	"a4",		"21.0x29.7cm",
	"a5",		"14.85x21.0cm",
	"a6",		"10.5x14.85cm",
	"a7",		"7.42x10.5cm",

	/* ISO `A' formats, Landscape */
	"a1r",		"84.0x59.4cm",
	"a2r",		"59.4x42.0cm",
	"a3r",		"42.0x29.7cm",
	"a4r",		"29.7x21.0cm",
	"a5r",		"21.0x14.85cm",
	"a6r",		"14.85x10.5cm",
	"a7r",		"10.5x7.42cm",

	/* ISO `B' formats, Portrait */
	"b1",		"70.6x100.0cm",
	"b2",		"50.0x70.6cm",
	"b3",		"35.3x50.0cm",
	"b4",		"25.0x35.3cm",
	"b5",		"17.6x25.0cm",
	"b6",		"13.5x17.6cm",
	"b7",		"8.8x13.5cm",

	/* ISO `B' formats, Landscape */
	"b1r",		"100.0x70.6cm",
	"b2r",		"70.6x50.0cm",
	"b3r",		"50.0x35.3cm",
	"b4r",		"35.3x25.0cm",
	"b5r",		"25.0x17.6cm",
	"b6r",		"17.6x13.5cm",
	"b7r",		"13.5x8.8cm",

	/* ISO `C' formats, Portrait */
	"c1",		"64.8x91.6cm",
	"c2",		"45.8x64.8cm",
	"c3",		"32.4x45.8cm",
	"c4",		"22.9x32.4cm",
	"c5",		"16.2x22.9cm",
	"c6",		"11.46x16.2cm",
	"c7",		"8.1x11.46cm",

	/* ISO `C' formats, Landscape */
	"c1r",		"91.6x64.8cm",
	"c2r",		"64.8x45.8cm",
	"c3r",		"45.8x32.4cm",
	"c4r",		"32.4x22.9cm",
	"c5r",		"22.9x16.2cm",
	"c6r",		"16.2x11.46cm",
	"c7r",		"11.46x8.1cm",
};

static	Boolean
set_paper_type() {
	_Xconst	char	*arg, *arg1;
	char	temp[21];
	_Xconst	char	**p;
	char	*q;

	if (strlen(resource.paper) > sizeof(temp) - 1) return False;
	arg = resource.paper;
	q = temp;
	for (;;) {	/* convert to lower case */
	    char c = *arg++;
	    if (c >= 'A' && c <= 'Z') c ^= ('a' ^ 'A');
	    *q++ = c;
	    if (c == '\0') break;
	}
	arg = temp;
	/* perform substitutions */
	for (p = paper_types; p < paper_types + XtNumber(paper_types); p += 2)
	    if (strcmp(temp, *p) == 0) {
		arg = p[1];
		break;
	    }
        arg1 = strchr(arg, 'x');
	if (arg1 == NULL) return False;
	unshrunk_paper_w = atopix(arg);
	unshrunk_paper_h = atopix(arg1 + 1);
#ifdef GRID
        unshrunk_paper_unit = atopixunit(arg);
#endif /* GRID */
	return (unshrunk_paper_w != 0 && unshrunk_paper_h != 0);
}

/* Set the icon name and title name standard properties on `top_level'
   (which we don't pass in because it is a different type for TOOLKIT
   !and TOOLKIT).  We use the basename of the DVI file (without the
   .dvi), so different xdvi invocations can be distinguished, yet 
   do not use up too much real estate.  */

void
set_icon_and_title (dvi_name, icon_ret, title_ret, set_std)
    char *dvi_name;
    char **icon_ret;
    char **title_ret;
    int set_std;
{
  /* Use basename of DVI file for name in icon and title.  */
  unsigned baselen;
  char *icon_name, *title_name;

        icon_name = strrchr(dvi_name, '/');
        if (icon_name != NULL) ++icon_name; else icon_name = dvi_name;
        baselen = strlen(icon_name);
        if (baselen >= sizeof(".dvi")
                && strcmp(icon_name + baselen - sizeof(".dvi") + 1, ".dvi")
                == 0) {         /* remove the .dvi */
            char *p;

            baselen -= sizeof(".dvi") - 1;
            p = xmalloc(baselen + 1);
            (void) strncpy(p, icon_name, (int) baselen);
            p[baselen] = '\0';
            icon_name = p;
        }
#ifdef Omega
        title_name = xmalloc(baselen + sizeof("(Omega) OXdvi:  "));
        Strcpy(title_name, "(Omega) OXdvi:  ");
#else
        title_name = xmalloc(baselen + sizeof("Xdvi:  "));
        Strcpy(title_name, "Xdvi:  ");
#endif
        Strcat(title_name, icon_name);

  if (icon_ret) *icon_ret = icon_name;
  if (title_ret) *title_ret = title_name;
  if (set_std) {
    Window top_window =
#ifdef TOOLKIT
      XtWindow (top_level);
#else
      top_level;
#endif

    XSetStandardProperties(DISP, top_window, title_name, icon_name,
                         (Pixmap) 0, NULL, 0, NULL);
  }
}


static void remove_temporary_dir(void)

{
  int err;

  if (TmpDir!=NULL) {
    err=rmdir(TmpDir);
    if (err!=0 && errno!=ENOENT) {
      char *buffer=malloc(160);
      if (buffer==NULL) oops("Please buy more RAM");
      sprintf(buffer,"xdvik: Warning: Could not remove temporary directory %s",
	      TmpDir);
      perror(buffer);
    }
  }
}

/* Create a temporary directory where we can write files from this program */
static void make_temporary_dir P1C(char **,tmpdir)

{
  int ret;

  *tmpdir = tempnam(NULL,"xdvi");

  if (*tmpdir == NULL)
    oops("Out of memory. Exiting.");

  if (debug & DBG_HYPER)
    fprintf(stderr,"Making temporary directory %s\n",*tmpdir);

  ret = mkdir(*tmpdir, 0700);

  /* The use of atexit and one variable to store the directory name
     makes this procedure a oneoff.  Enforce it */
  if (TmpDir!=NULL)
    oops("make_temporary_dir called twice!\n");

  TmpDir=strdup(*tmpdir);

  atexit(remove_temporary_dir);

  if (ret != 0)
    /* Didn't work, PANIC! */
    oops("Could not create temporary directory. Exiting.");

}


/*
 *	main program
 */

int
main(argc, argv)
	int argc;
	char **argv;
{

#ifndef	TOOLKIT
	XSizeHints	size_hints;
	XWMHints	wmhints;
	int		flag;
	int		x_thick	= 0;
	int		y_thick	= 0;
#endif	/* TOOLKIT */
#ifdef MOTIF
	Widget		menubar;
	Widget		scale_menu;
#endif
	Dimension	screen_w, screen_h;
	char		*title_name;
	char		*icon_name;
	int		i;

	/*
	 *	Step 1:  Process command-line options and resources.
	 */

	/* This has to be a special case, for now. */

	if (argc == 2 && (strcmp(argv[1], "-version") == 0 ||
			  strcmp(argv[1], "--version") == 0 ||
			  strcmp(argv[1], "+version") == 0
			  ))
	{
	    Printf("xdvi(k) version %s\n", TVERSION);
	    Printf("Copyright (C) 1990-1999 Paul Vojta.\n\
Primary author of Xdvi: Paul Vojta;\n\
Xdvi-k maintainer: janl@math.uio.no.\n\
\n\
xdvi itself is licensed under the X Consortium license.  xdvik relies\n\
on the kpathsea library which is covered by the GNU LIBRARY General\n\
Public License (see COPYING.LIB for full details) and libwww which is\n\
copyrighted by the World Wide Web Consortium and CERN (see COPYRIGH for\n\
full details).\n\
\n\
There is NO WARRANTY of anything.\n");
	    exit(0);
	}

        if (argc == 2 && ((strcmp (argv[1], "-help") == 0)
                         || (strcmp (argv[1], "+help") == 0)
                         || (strcmp (argv[1], "--help") == 0)
                         ))
          {
            extern char *kpse_bug_address;
	    Printf("xdvi(k) version %s\n", TVERSION);
            Printf("Preview a DVI file under the X window system.\n%s\n",
		   kpse_bug_address);
            usage (); /* Unfortunately this exits with status 1.  */
          }


#ifndef	VMS
	prog = strrchr(*argv, '/');
#else
	prog = strrchr(*argv, ']');
#endif
	if (prog != NULL) ++prog; else prog = *argv;

#ifdef	VMS
	if (strchr(prog, '.') != NULL) *strchr(prog, '.') = '\0';
#endif

        /* We can't initialize this at compile time, because it might be
           the result of a function call under POSIX. */
        n_files_left = OPEN_MAX;

        kpse_set_program_name (argv[0], "xdvi");
        kpse_set_program_enabled (kpse_any_glyph_format,
                                  MAKE_TEX_PK_BY_DEFAULT, kpse_src_compile);


#if	CFGFILE
	/* get the debug value (if any) from the environment */
	/* (it's too early to get it from the command line) */
	{
	    _Xconst char *dbg_str;

	    dbg_str = getenv("XDVIDEBUG");
	    if (dbg_str != NULL)
		debug = atoi(dbg_str);
	}
#ifdef	SELFAUTO
	argv0 = argv[0];
#endif
#ifndef	CFG2RES
	readconfig();		/* read config file(s). */
#else	/* CFG2RES */
	{
	    static _Xconst struct cfg2res cfg2reslist[] = {
		{"MFMODE",		"mfMode",		False},
		{"PIXELSPERINCH",	"pixelsPerInch",	True},
		{"SHRINKFACTOR",	"shrinkFactor",		True},
#ifdef	BUTTONS
		{"SHRINKBUTTON1",	"shrinkButton1",	True},
		{"SHRINKBUTTON2",	"shrinkButton2",	True},
		{"SHRINKBUTTON3",	"shrinkButton3",	True},
		{"SHRINKBUTTON4",	"shrinkButton4",	True},
#endif
		{"PAPER",		"paper",		False},
	    };

	    readconfig(cfg2reslist, cfg2reslist + XtNumber(cfg2reslist),
	      application_resources,
	      application_resources + XtNumber(application_resources));
	}
#endif	/* CFG2RES */
#endif	/* CFGFILE */

#ifdef	TOOLKIT

	top_level = XtInitialize(prog, "XDvi", options, XtNumber(options),
	  &argc, argv);
	while (--argc > 0) {
	    if (*(*++argv) == '+')
		if (curr_page != NULL) usage();
		else curr_page = *argv + 1;
	    else if (dvi_name != NULL) usage();
                else {
                    /* need to make sure that dvi_name can be freed safely */
                    dvi_name = xmalloc((unsigned) strlen(*argv)+1);
                    Strcpy(dvi_name, *argv);
                }
	}

	DISP = XtDisplay(top_level);
	SCRN = XtScreen(top_level);

#ifdef GREY
	XtSetTypeConverter(XtRString, XtRBool3, XdviCvtStringToBool3,
	  NULL, 0, XtCacheNone, NULL);
#endif

	XtGetApplicationResources(top_level, (XtPointer) &resource,
	  application_resources, XtNumber(application_resources),
	  (ArgList) NULL, 0);
	shrink_factor = resource.shrinkfactor;

#if CFGFILE
	if (resource.progname != NULL)
	    prog = resource.progname;
#endif

#else	/* not TOOLKIT */

	parse_options(argc, argv);

#endif /* not TOOLKIT */

	if (resource.mfmode != NULL) {
	    char *p;

	    p = strrchr(resource.mfmode, ':');
	    if (p != NULL) {
		unsigned int	len;
		char		*p1;

		++p;
		len = p - resource.mfmode;
		p1 = xmalloc(len);
		bcopy(resource.mfmode, p1, len - 1);
		p1[len - 1] = '\0';
		resource.mfmode = p1;
		pixels_per_inch = atoi(p);
	    }
	}
	if (shrink_factor < 0 || density <= 0 || pixels_per_inch <= 0
#ifndef SELFILE
	    || dvi_name == NULL
#endif
	    ) usage();
	if (shrink_factor > 1) {
	    bak_shrink = shrink_factor;
	    mane.shrinkfactor = shrink_factor;	/* otherwise it's 1 */
	}
	if (resource.debug_arg != NULL) {
	    debug = isdigit(*resource.debug_arg) ? atoi(resource.debug_arg)
		: DBG_ALL;
	    if (debug & DBG_OPEN) KPSE_DEBUG_SET (KPSE_DEBUG_FOPEN);
	    if (debug & DBG_STAT) KPSE_DEBUG_SET (KPSE_DEBUG_STAT);
	    if (debug & DBG_HASH) KPSE_DEBUG_SET (KPSE_DEBUG_HASH);
	    if (debug & DBG_PATHS) KPSE_DEBUG_SET (KPSE_DEBUG_PATHS);
	    if (debug & DBG_EXPAND) KPSE_DEBUG_SET (KPSE_DEBUG_EXPAND);
	    if (debug & DBG_SEARCH) KPSE_DEBUG_SET (KPSE_DEBUG_SEARCH);
	}

	if (resource.sidemargin) home_x = atopix(resource.sidemargin);
	if (resource.topmargin) home_y = atopix(resource.topmargin);
	offset_x = resource.xoffset ? atopix(resource.xoffset)
	    : pixels_per_inch;
	offset_y = resource.yoffset ? atopix(resource.yoffset)
	    : pixels_per_inch;
	if (!set_paper_type())
	    oops("Don't recognize paper type %s", resource.paper);
	for (i = 0; i < 5; ++i)
	    if (resource.mg_arg[i] != NULL) {
		char	*s;

		mg_size[i].w = mg_size[i].h = atoi(resource.mg_arg[i]);
                s = strchr(resource.mg_arg[i], 'x');
		if (s != NULL) {
		    mg_size[i].h = atoi(s + 1);
		    if (mg_size[i].h <= 0) mg_size[i].w = 0;
		}
	    }
#if	PS
	if (resource.safer) {
	    resource.allow_shell = False;
#ifdef	PS_GS
	    resource.gs_safer = True;
#endif
	}
#endif	/* PS */
#ifdef	PS_GS
	{
	    _Xconst char *CGMcgm = "CGMcgm";
	    _Xconst char *cgmp;

	    cgmp = strchr(CGMcgm, resource.gs_palette[0]);
	    if (cgmp == NULL)
		oops("Invalid value %s for gs palette option",
		    resource.gs_palette);
	    if (cgmp >= CGMcgm + 3) {
		static	char	gsp[]	= "x";

		gsp[0] = *(cgmp - 3);
		resource.gs_palette = gsp;
	    }
	}
#endif

        if (resource.version_flag)
            Printf("xdvi version %s\n", TVERSION);

        kpse_init_prog ("XDVI", pixels_per_inch, resource.mfmode, alt_font);

	if (debug) {
	  fprintf(stderr,"XDVI %d/%d/%s/%s\n", pixels_per_inch, shrink_factor,
		resource.mfmode, alt_font);
	  fprintf(stderr,"Paper %s\n", resource.paper);
	}

        kpse_set_program_enabled (kpse_any_glyph_format,
                                  resource.makepk, kpse_src_compile);

        /* janl 16/11/98: I have changed this. The above line used to
	   say the settings in resource.makepk was supplied on the
	   commandline, resulting in it overriding _all other_
	   settings, derived from the environment or texmf.cnf, no
	   matter what the value. The value in resource.makepk could
	   be the compile-time default...

	   Personaly I like the environment/texmf.cnf to override
	   resources and thus changed the 'level' of this setting to
	   kpse_src_compile so the environment/texmf.cnf will override
	   the values derived by Xt.

	   Previous comment here:

	   ``Let true values as an X resource/command line override false
           values in texmf.cnf/envvar.''  */

	/*
	 *	Step 2:  Settle colormap issues.  This should be done before
	 *	other widgets are created, so that they get the right
	 *	pixel values.  (The top-level widget won't have the right
	 *	values, but I don't think that makes any difference.)
	 */

#ifdef GREY

	screen_number = 0;	/* yyuucchh */
	while (SCRN != ScreenOfDisplay(DISP, screen_number)) ++screen_number;

	our_depth = DefaultDepthOfScreen(SCRN);
	our_visual = DefaultVisualOfScreen(SCRN);
	our_colormap = DefaultColormapOfScreen(SCRN);
	copyGC = DefaultGCOfScreen(SCRN);

	if (resource.install != False && our_visual->class == PseudoColor) {
	    /* look for a TrueColor visual with more bits */
	    XVisualInfo	template;
	    XVisualInfo	*list;
	    int		nitems_return;

	    template.screen = screen_number;
	    template.class = TrueColor;
	    list = XGetVisualInfo(DISP, VisualScreenMask | VisualClassMask,
	      &template, &nitems_return);
	    if (list != NULL) {
		XVisualInfo	*list1;
		XVisualInfo	*best	= NULL;

		for (list1 = list; list1 < list + nitems_return; ++list1)
		    if (list1->depth > our_depth
		      && (best == NULL || list1->depth > best->depth))
			best = list1;
		if (best != NULL) {
		    our_depth = best->depth;
		    our_visual = best->visual;
		    our_colormap = XCreateColormap(DISP,
		      RootWindowOfScreen(SCRN), our_visual, AllocNone);
		    XInstallColormap(DISP, our_colormap);
		    copyGC = NULL;
#ifdef TOOLKIT
		    temp_args1[0].value = (XtArgVal) our_depth;
		    temp_args1[1].value = (XtArgVal) our_visual;
		    temp_args1[2].value = (XtArgVal) our_colormap;
		    XtSetValues(top_level, temp_args1, XtNumber(temp_args1));
		    XtSetTypeConverter(XtRString, XtRPixel,
		      XdviCvtStringToPixel,
		      (XtConvertArgList) colorConvertArgs, 2,
		      XtCacheByDisplay, NULL);
#else
		    /* Can't use {Black,White}PixelOfScreen() any more */
		    if (!resource.fore_color)
			resource.fore_color =
			  (resource.reverse ? "white" : "black");
		    if (!resource.back_color)
			resource.back_color =
			  (resource.reverse ? "black" : "white");
#endif
		}
		XFree(list);
	    }
	}

	if (resource.install == True && our_visual->class == PseudoColor) {
	    XColor tmp_color;

	    /* This next bit makes sure that the standard black and white pixels
	       are allocated in the new colormap. */

	    tmp_color.pixel = BlackPixelOfScreen(SCRN);
	    XQueryColor(DISP, our_colormap, &tmp_color);
	    XAllocColor(DISP, our_colormap, &tmp_color);

	    tmp_color.pixel = WhitePixelOfScreen(SCRN);
	    XQueryColor(DISP, our_colormap, &tmp_color);
	    XAllocColor(DISP, our_colormap, &tmp_color);

	    our_colormap = XCopyColormapAndFree(DISP, our_colormap);
#ifdef TOOLKIT
	    temp_args1a[0].value = (XtArgVal) our_colormap;
	    XtSetValues(top_level, temp_args1a, 1);
#endif
	}

#ifdef TOOLKIT
	XtGetApplicationResources(top_level, (XtPointer) &resource,
	  app_pixel_resources, XtNumber(app_pixel_resources),
	  (ArgList) NULL, 0);
#endif

#endif /* GREY */

#ifndef TOOLKIT
	fore_Pixel = (resource.fore_color
	  ? string_to_pixel(&resource.fore_color)
	  : (resource.reverse ? WhitePixelOfScreen(SCRN)
	    : BlackPixelOfScreen(SCRN)));
	back_Pixel = (resource.back_color
	  ? string_to_pixel(&resource.back_color)
	  : (resource.reverse ? BlackPixelOfScreen(SCRN)
	    : WhitePixelOfScreen(SCRN)));
	brdr_Pixel = (brdr_color ? string_to_pixel(&brdr_color) : fore_Pixel);
	hl_Pixel = (high_color ? string_to_pixel(&high_color) : fore_Pixel);
	cr_Pixel = (curs_color ? string_to_pixel(&curs_color) : fore_Pixel);

#ifdef GRID
        if (resource.grid1_color)
            grid1_Pixel = string_to_pixel(&resource.grid1_color);
        if (resource.grid2_color)
            grid2_Pixel = string_to_pixel(&resource.grid2_color);
        if (resource.grid3_color)
            grid3_Pixel = string_to_pixel(&resource.grid3_color);
#endif /* GRID */

#endif	/* not TOOLKIT */

#ifdef	GREY
	if (our_depth == 1)
	    use_grey = False;

	if (use_grey && our_visual->class != TrueColor) {
	    fore_color_data.pixel = fore_Pixel;
	    XQueryColor(DISP, our_colormap, &fore_color_data);
	    back_color_data.pixel = back_Pixel;
	    XQueryColor(DISP, our_colormap, &back_color_data);
	    init_plane_masks();
	    if (!copy) {
		back_color_data.pixel = back_Pixel;
		XStoreColor(DISP, our_colormap, &back_color_data);
	    }
	}
#endif

	/*
	 *	Step 3:  Open the dvi file and set titles.
	 */

#ifdef SELFILE
        /* If no filename by here, so pop up a selection box. */
        if (dvi_name == NULL)
          {
            extern FILE *select_filename ();
            (void) select_filename(False, False) ;
	    /* User might have cancled  -janl */
	    if (dvi_name == NULL) exit(1);
          }
#endif  /* SELFILE */

        make_temporary_dir(&temporary_dir); /* For storing temporary files.. */

#ifdef HTEX
        {
            char *cp = getenv("WWWBROWSER"); 
            if (cp) browser = cp;
        }
        HTProfile_newPreemptiveClient(HTAppName, HTAppVersion); 
        HTCacheMode_setEnabled(NO);

#if 0
        /* Optionally, turn on tracing of WWW library calls. */
        if (debug & DBG_HYPER) {
            WWWTRACE = SHOW_STREAM_TRACE; 
        }
#endif

#if T1
	/* At this point DISP, our_visual, our_depth and our_colormap must
	   be defined, and they are */
	init_t1();
#endif

	/* Open the input file.  Turn filename into URL first if needed */
        URL_aware = TRUE;

        if (!(URLbase || htex_is_url(dvi_name))) {
	  char *new_name;
	  int n;

	  n = strlen(dvi_name);
	  new_name = dvi_name;
	  /* Find the right filename */
	  if (n < sizeof(".dvi")
	      || strcmp(dvi_name + n - sizeof(".dvi") + 1, ".dvi") != 0) {
	    dvi_name = xmalloc((unsigned) n + sizeof(".dvi"));
	    Strcpy(dvi_name, new_name);
	    Strcat(dvi_name, ".dvi");
	    /* Atempt $dvi_name.dvi */
	    dvi_file = xfopen(dvi_name, OPEN_MODE);
	    if (dvi_file == NULL) {
	      /* Didn't work, revert to original $dvi_name */
	      free(dvi_name);
	      dvi_name = new_name;
	    }
	  }

	  /* Turn filename into URL */
	  /* Escape dangers, esp. # from emacs */
	  new_name=HTEscape(dvi_name,URL_PATH);
	  free(dvi_name);
	  dvi_name = xmalloc((unsigned) strlen(new_name)+6);
	  strcat(strcpy(dvi_name,"file:"),new_name);
	  /* Now we have the right filename, in a URL */
	}
	
	detach_anchor();
	if (!open_www_file()) {
	  fprintf(stderr,"Could not open dvi file. Exiting.\n");
	  exit(1);
	}

        URL_aware = FALSE;
#else
        open_dvi_file();
#endif /* not HTEX */

	if (curr_page) {
		current_page = (*curr_page ? atoi(curr_page) : total_pages) - 1;
		if (current_page < 0 || current_page >= total_pages) usage();
	}
        set_icon_and_title (dvi_name, &icon_name, &title_name, 0);

	/*
	 *	Step 4:  Set initial window size.
	 *	This needs to be done before colors are assigned because if
	 *	-s 0 is specified, then we need to compute the shrink factor
	 *	(which in turn affects whether init_pix is called).
	 */

#ifdef	TOOLKIT

		/* The following code is lifted from Xterm */
	if (resource.icon_geometry != NULL) {
#ifndef GREY
	    int screen_number;
#endif
	    int junk;

#ifndef GREY
	    for (screen_number = 0;	/* yyuucchh */
	      SCRN != ScreenOfDisplay(DISP, screen_number);
	      ++screen_number);
#endif

	    (void) XGeometry(DISP, screen_number, resource.icon_geometry, "",
			0, 0, 0, 0, 0,
			(int *) &temp_args2[0].value,
			(int *) &temp_args2[1].value, &junk, &junk);
	    XtSetValues(top_level, temp_args2, XtNumber(temp_args2));
	}
		/* Set icon pixmap */
	XtGetValues(top_level, &temp_args4, 1);
	if (icon_pm == (Pixmap) 0) {
	    temp_args4.value = (XtArgVal) (XCreateBitmapFromData(DISP,
		RootWindowOfScreen(SCRN), (_Xconst char *) xdvi_bits,
		xdvi_width, xdvi_height));
	    XtSetValues(top_level, &temp_args4, 1);
	}
	temp_args5[0].value = (XtArgVal) title_name;
	temp_args5[1].value = (XtArgVal) icon_name;
	XtSetValues(top_level, temp_args5, XtNumber(temp_args5));

#ifdef	BUTTONS
	form_widget = XtCreateManagedWidget("form", FORM_WIDGET_CLASS,
	  top_level, form_args, XtNumber(form_args));
#define	form_or_top	form_widget	/* for calls later on */
#define	form_or_vport	form_widget
#else	/* !BUTTONS */
#define	form_or_top	top_level	/* for calls later on */
#define	form_or_vport	vport_widget
#endif	/* BUTTONS */

	vport_widget = XtCreateManagedWidget("vport", VPORT_WIDGET_CLASS,
	  form_or_top, vport_args, XtNumber(vport_args));

#ifndef MOTIF

	clip_widget = XtNameToWidget(vport_widget, "clip");

#else /* MOTIF */

	menubar = XmVaCreateSimpleMenuBar(vport_widget, "menubar",
	  XmNdepth, (XtArgVal) our_depth,
	  XmNvisual, (XtArgVal) our_visual,
	  XmNcolormap, (XtArgVal) our_colormap,
	  XmVaCASCADEBUTTON, (XtArgVal) XmCvtCTToXmString("File"), 0,
	  XmVaCASCADEBUTTON, (XtArgVal) XmCvtCTToXmString("Navigate"), 0,
	  XmVaCASCADEBUTTON, (XtArgVal) XmCvtCTToXmString("Scale"), 0,
	  0);
	XmVaCreateSimplePulldownMenu(menubar,
	  "file_pulldown", 0, file_pulldown_callback,
	  XmNtearOffModel, (XtArgVal) XmTEAR_OFF_ENABLED,
	  XmNdepth, (XtArgVal) our_depth,
	  XmNvisual, (XtArgVal) our_visual,
	  XmNcolormap, (XtArgVal) our_colormap,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Reload"), 0, 0, 0,
	  XmVaSEPARATOR,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Quit"), 0, 0, 0,
	  0);
	XmVaCreateSimplePulldownMenu(menubar,
	  "navigate_pulldown", 1, navigate_pulldown_callback,
	  XmNtearOffModel, (XtArgVal) XmTEAR_OFF_ENABLED,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Page-10"), 0, 0, 0,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Page-5"), 0, 0, 0,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Prev"), 0, 0, 0,
	  XmVaSEPARATOR,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Next"), 0, 0, 0,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Page+5"), 0, 0, 0,
	  XmVaPUSHBUTTON, (XtArgVal) XmCvtCTToXmString("Page+10"), 0, 0, 0,
	  0);
	scale_menu = XmVaCreateSimplePulldownMenu(menubar,
	  "scale_pulldown", 2, scale_pulldown_callback,
	  XmNradioBehavior, (XtArgVal) True,
	  XmNtearOffModel, (XtArgVal) XmTEAR_OFF_ENABLED,
	  XmVaRADIOBUTTON, (XtArgVal) XmCvtCTToXmString("Shrink1"), 0, 0, 0,
	  XmVaRADIOBUTTON, (XtArgVal) XmCvtCTToXmString("Shrink2"), 0, 0, 0,
	  XmVaRADIOBUTTON, (XtArgVal) XmCvtCTToXmString("Shrink3"), 0, 0, 0,
	  XmVaRADIOBUTTON, (XtArgVal) XmCvtCTToXmString("Shrink4"), 0, 0, 0,
	  0);
	shrink_button[0] = XtNameToWidget(scale_menu, "button_0");
	shrink_button[1] = XtNameToWidget(scale_menu, "button_1");
	shrink_button[2] = XtNameToWidget(scale_menu, "button_2");
	shrink_button[3] = XtNameToWidget(scale_menu, "button_3");

	x_bar = XtNameToWidget(vport_widget, "HorScrollBar");
	y_bar = XtNameToWidget(vport_widget, "VertScrollBar");

#endif /* MOTIF */

	draw_args[0].value = (XtArgVal) page_w;
	draw_args[1].value = (XtArgVal) page_h;
	draw_widget = XtCreateManagedWidget("drawing", DRAW_WIDGET_CLASS,
	  vport_widget, draw_args, XtNumber(draw_args));

#ifdef MOTIF
	XtVaSetValues(vport_widget, XmNworkWindow, draw_widget,
	  XmNmenuBar, menubar, NULL);
	XtManageChild(menubar);
	XtVaGetValues(vport_widget, XmNclipWindow, &clip_widget, NULL);
#endif /* MOTIF */

	{	/* set default window size */
#ifdef	BUTTONS
	    int xtra_wid = resource.expert ? 0 : XTRA_WID;
#else
#define	xtra_wid	0
#endif
	    XtWidgetGeometry constraints;
	    XtWidgetGeometry reply;

	    XtGetValues(top_level, &temp_args3, 1);	/* get border width */
	    screen_w = WidthOfScreen(SCRN) - 2 * bwidth - xtra_wid;
	    screen_h = HeightOfScreen(SCRN) - 2 * bwidth;
	    for (;;) {	/* actually, at most two passes */
		constraints.request_mode = reply.request_mode = 0;
		constraints.width = page_w;
		if (page_w > screen_w) {
		    constraints.request_mode = CWWidth;
		    constraints.width = screen_w;
		}
		constraints.height = page_h;
		if (page_h > screen_h) {
		    constraints.request_mode |= CWHeight;
		    constraints.height = screen_h;
		}
		if (constraints.request_mode != 0
			&& constraints.request_mode != (CWWidth | CWHeight))
		    (void) XtQueryGeometry(vport_widget, &constraints, &reply);
		if (!(reply.request_mode & CWWidth))
		    reply.width = constraints.width;
		if (reply.width >= screen_w)
		    reply.width = screen_w;
		if (!(reply.request_mode & CWHeight))
		    reply.height = constraints.height;
		if (reply.height >= screen_h)
		    reply.height = screen_h;

		/* now reply.{width,height} contain max. usable window size */

		if (shrink_factor != 0)
		    break;

		shrink_factor = ROUNDUP(unshrunk_page_w, reply.width - 2);
		i = ROUNDUP(unshrunk_page_h, reply.height - 2);
		if (i >= shrink_factor) shrink_factor = i;
		if (shrink_factor > 1) bak_shrink = shrink_factor;
		mane.shrinkfactor = shrink_factor;
		init_page();
		set_wh_args[0].value = (XtArgVal) page_w;
		set_wh_args[1].value = (XtArgVal) page_h;
		XtSetValues(draw_widget, set_wh_args, XtNumber(set_wh_args));
	    }
	    set_wh_args[0].value = (XtArgVal) (reply.width + xtra_wid);
	    set_wh_args[1].value = (XtArgVal) reply.height;
	    XtSetValues(top_level, set_wh_args, XtNumber(set_wh_args));
#ifdef	BUTTONS
	    set_wh_args[0].value -= xtra_wid;
	    XtSetValues(vport_widget, set_wh_args, XtNumber(set_wh_args));
#endif	/* BUTTONS */
	}
#ifdef MOTIF
	set_shrink_factor(mane.shrinkfactor);
#endif

#else	/* not TOOLKIT */

	screen_w = WidthOfScreen(SCRN) - 2*bwidth;
	screen_h = HeightOfScreen(SCRN) - 2*bwidth;

	size_hints.flags = PMinSize;
	size_hints.min_width = size_hints.min_height = 50;
	size_hints.x = size_hints.y = 0;

	/* compute largest possible window */
	flag = 0;
	if (geometry != NULL) {
	    flag = XParseGeometry(geometry, &size_hints.x, &size_hints.y,
	      &window_w, &window_h);
	    if (flag & (XValue | YValue))
		size_hints.flags |= USPosition;
	    if (flag & (WidthValue | HeightValue))
		size_hints.flags |= USSize;
	}
	if (!(flag & WidthValue)) window_w = screen_w;
	if (!(flag & HeightValue)) window_h = screen_h;

	if (shrink_factor == 0) {
	    /* compute best shrink factor based on window_w and window_h */
	    shrink_factor = ROUNDUP(unshrunk_page_w, window_w - 2);
	    i = ROUNDUP(unshrunk_page_h, window_h - 2);
	    if (i >= shrink_factor) shrink_factor = i;
	    if (shrink_factor > 1) bak_shrink = shrink_factor;
	    mane.shrinkfactor = shrink_factor;
	    init_page();
	}

	if (window_w < page_w) x_thick = BAR_THICK;
	if (window_h < page_h + x_thick) y_thick = BAR_THICK;
	if (!(flag & WidthValue)) {
	    window_w = page_w + y_thick;
	    if (window_w > screen_w) {
		x_thick = BAR_THICK;
		window_w = screen_w;
	    }
	    size_hints.flags |= PSize;
	}
	if (!(flag & HeightValue)) {
	    window_h = page_h + x_thick;
	    if (window_h > screen_h) window_h = screen_h;
	    size_hints.flags |= PSize;
	}

	if (flag & XNegative) size_hints.x += screen_w - window_w;
	if (flag & YNegative) size_hints.y += screen_h - window_h;

#endif	/* not TOOLKIT */

	/*
	 *	Step 5:  Realize the widgets (or windows).
	 */

#ifdef	TOOLKIT

	{
	    static Arg back_args = {XtNbackground, (XtArgVal) 0};

	    back_args.value = back_Pixel;
	    XtSetValues(draw_widget, &back_args, 1);
	    XtSetValues(clip_widget, &back_args, 1);
	}

#ifdef BUTTONS
	if (!resource.expert)
	    create_buttons(set_wh_args[1].value);
#endif
#ifdef HTEX
        pane_widget = XtCreateManagedWidget("pane", panedWidgetClass,
                top_level, pane_args, XtNumber(pane_args));
	/* Set up anchor search and information widgets: */
        anchor_search = XtCreateManagedWidget("anchorsearch", 
                dialogWidgetClass, pane_widget, anchorsearch_args,
		XtNumber(anchorsearch_args));
        XawDialogAddButton(anchor_search, "Open", search_callback, NULL);
        anchor_info = XtCreateManagedWidget("anchorinfo", asciiTextWidgetClass,
                pane_widget, anchorinfo_args, XtNumber(anchorinfo_args));
	/* Eliminate the silly caret */
        XawTextDisplayCaret(anchor_info, False);
#endif

#ifndef MOTIF
	XtAddEventHandler(form_or_vport, KeyPressMask, False,
	  handle_key, (caddr_t) NULL);
#else /* MOTIF */
	XtSetSensitive(draw_widget, TRUE);
	XtAddEventHandler(draw_widget, KeyPressMask, False,
	  handle_key, (caddr_t) NULL);
#endif /* MOTIF */
	XtAddEventHandler(vport_widget, StructureNotifyMask, False,
	  handle_resize, (caddr_t) NULL);
	XtAddEventHandler(draw_widget, ExposureMask, False, handle_exp,
	  (caddr_t) &mane);
	XtAddEventHandler(draw_widget, ButtonPressMask, False, handle_button,
	  (caddr_t) NULL);
	XtAddEventHandler(draw_widget, ButtonMotionMask, False, handle_motion,
	  (caddr_t) NULL);
	XtAddEventHandler(draw_widget, ButtonReleaseMask, False, handle_release,
	  (caddr_t) NULL);
	XtRealizeWidget(top_level);
#ifdef MOTIF
	XmProcessTraversal(draw_widget, XmTRAVERSE_CURRENT);
#endif

	currwin.win = mane.win = XtWindow(draw_widget);

	{
	    XWindowAttributes attrs;

	    (void) XGetWindowAttributes(DISP, mane.win, &attrs);
	    backing_store = attrs.backing_store;
	}

#else	/* not TOOLKIT */

	size_hints.width = window_w;
	size_hints.height = window_h;
#ifndef GREY
	top_level = XCreateSimpleWindow(DISP, RootWindowOfScreen(SCRN),
	  size_hints.x, size_hints.y, window_w, window_h, bwidth,
	  brdr_Pixel, back_Pixel);
#else
	{
	    XSetWindowAttributes attr;

	    attr.border_pixel = brdr_Pixel;
	    attr.background_pixel = back_Pixel;
	    attr.colormap = our_colormap;
	    top_level = XCreateWindow(DISP, RootWindowOfScreen(SCRN),
	      size_hints.x, size_hints.y, window_w, window_h, bwidth,
	      our_depth, InputOutput, our_visual,
	      CWBorderPixel | CWBackPixel | CWColormap, &attr);
	}
#endif
	XSetStandardProperties(DISP, top_level, title_name, icon_name,
		(Pixmap) 0, argv, argc, &size_hints);

	wmhints.flags = InputHint | StateHint | IconPixmapHint;
	wmhints.input = True;	/* window manager must direct input */
	wmhints.initial_state = iconic ? IconicState : NormalState;
	wmhints.icon_pixmap = XCreateBitmapFromData(DISP,
	    RootWindowOfScreen(SCRN), (_Xconst char *) xdvi_bits,
	    xdvi_width, xdvi_height);
	if (resource.icon_geometry != NULL) {
	    int junk;

	    wmhints.flags |= IconPositionHint;
	    (void) XGeometry(DISP, DefaultScreen(DISP), resource.icon_geometry,
		"", 0, 0, 0, 0, 0, &wmhints.icon_x, &wmhints.icon_y,
		&junk, &junk);
	}
	XSetWMHints(DISP, top_level, &wmhints);

	XSelectInput(DISP, top_level, KeyPressMask | StructureNotifyMask);
	XMapWindow(DISP, top_level);
	XFlush(DISP);

#endif	/* not TOOLKIT */

{
	    static KeySym list[2] = {XK_Caps_Lock, XK_Num_Lock};

#define	rebindkey(ks, str)	XRebindKeysym(DISP, (KeySym) ks, \
		  (KeySym *) NULL, 0, (_Xconst ubyte *) str, 1); \
		XRebindKeysym(DISP, (KeySym) ks, \
		  list, 1, (_Xconst ubyte *) str, 1); \
		XRebindKeysym(DISP, (KeySym) ks, \
		  list + 1, 1, (_Xconst ubyte *) str, 1); \
		XRebindKeysym(DISP, (KeySym) ks, \
		  list, 2, (_Xconst ubyte *) str, 1);

	    rebindkey(XK_Help, "?");
	    rebindkey(XK_Cancel, "q");
	    rebindkey(XK_Redo, "A");
	    rebindkey(XK_Home, "^");
	    rebindkey(XK_Left, "l");
	    rebindkey(XK_Up, "u");
	    rebindkey(XK_Right, "r");
	    rebindkey(XK_Down, "d");
	    rebindkey(XK_End, "g");
#ifdef  XK_Page_Up
	    rebindkey(XK_Page_Up, "b");
	    rebindkey(XK_Page_Down, "f");
#endif
#ifdef	XK_KP_Left
	    rebindkey(XK_KP_Home, "^");
	    rebindkey(XK_KP_Left, "l");
	    rebindkey(XK_KP_Up, "u");
	    rebindkey(XK_KP_Right, "r");
	    rebindkey(XK_KP_Down, "d");
	    rebindkey(XK_KP_Prior, "b");
	    rebindkey(XK_KP_Next, "f");
	    rebindkey(XK_KP_Delete, "\177");
#ifdef  XK_KP_Page_Up
	    rebindkey(XK_KP_Page_Up, "b");
	    rebindkey(XK_KP_Page_Down, "f");
#endif  /* XK_KP_Page_Up */
#endif	/* XK_KP_Left */
}
#undef	rebindkey

	image = XCreateImage(DISP, our_visual, 1, XYBitmap, 0,
			     (char *) NULL, 0, 0, BMBITS, 0);
	image->bitmap_unit = BMBITS;
#ifdef	WORDS_BIGENDIAN
	image->bitmap_bit_order = MSBFirst;
#else
	image->bitmap_bit_order = LSBFirst;
#endif
	{
	    short endian = MSBFirst << 8 | LSBFirst;
	    image->byte_order = *((char *) &endian);
	}

	/*
	 *	Step 6:  Assign colors and GCs.
	 *		 Because of the latter, this has to go after Step 5.
	 */

#ifndef TOOLKIT
#define	XtWindow(win)	win
#endif

#define	MakeGC(fcn, fg, bg)	(values.function = fcn, \
	  values.foreground=fg, values.background=bg, \
	  XCreateGC(DISP, XtWindow(top_level), \
	    GCFunction | GCForeground | GCBackground, &values))

#ifdef GRID
       if (!resource.grid1_color) grid1_Pixel = fore_Pixel;
       if (!resource.grid2_color) grid2_Pixel = grid1_Pixel;
       if (!resource.grid3_color) grid3_Pixel = grid1_Pixel;
#endif /* GRID */


#ifdef	GREY
	if (resource._gamma == 0.0)
	    resource._gamma = 1.0;

#endif

	copy = resource.copy;

	init_colors();

	{
	    XGCValues	values;

	    highGC = ruleGC;
	    if (hl_Pixel != fore_Pixel
#ifdef GREY
	      || (!copy && our_visual != DefaultVisualOfScreen(SCRN))
#endif
	      )
#ifdef GREY
		copyGC =
#endif
		highGC = MakeGC(GXcopy, hl_Pixel, back_Pixel);

#ifdef GRID
	        grid1GC = MakeGC(GXcopy, grid1_Pixel, fore_Pixel);
		grid2GC = MakeGC(GXcopy, grid2_Pixel, fore_Pixel);
		grid3GC = MakeGC(GXcopy, grid3_Pixel, fore_Pixel);
#endif /* GRID */

	}

#ifndef	VMS
	ready_cursor = XCreateFontCursor(DISP, XC_cross);
	redraw_cursor = XCreateFontCursor(DISP, XC_watch);
/* BEGIN CHUNK xdvi.c 2 */
#ifdef SRC_SPECIALS
	src_cursor = XCreateFontCursor(DISP, src_cursor_shape);
#endif
/* END CHUNK xdvi.c 2 */
	
#else
	DECWCursorFont = XLoadFont(DISP, "DECW$CURSOR");
	XSetFont(DISP, foreGC, DECWCursorFont);
	redraw_cursor = XCreateGlyphCursor(DISP, DECWCursorFont, DECWCursorFont,
		decw$c_wait_cursor, decw$c_wait_cursor + 1,
		&resource.fore_color, &resource.back_color);
	MagnifyPixmap = XCreateBitmapFromData(DISP, RootWindowOfScreen(SCRN),
		mag_glass_bits, mag_glass_width, mag_glass_height);
	ready_cursor = XCreatePixmapCursor(DISP, MagnifyPixmap, MagnifyPixmap,
		&resource.back_color, &resource.fore_color,
		mag_glass_x_hot, mag_glass_y_hot);
/* BEGIN CHUNK xdvi.c 2 */
#ifdef SRC_SPECIALS
	/* sorry, I dunno about VMS. Someone else will have to tackle that */
	src_cursor = ready_cursor;
#endif
/* END CHUNK xdvi.c 2 */
#endif	/* VMS */

	{
	    XColor bg_Color, cr_Color;

	    bg_Color.pixel = back_Pixel;
	    XQueryColor(DISP, our_colormap, &bg_Color);
	    cr_Color.pixel = cr_Pixel;
	    XQueryColor(DISP, our_colormap, &cr_Color);
	    XRecolorCursor(DISP, ready_cursor, &cr_Color, &bg_Color);
	    XRecolorCursor(DISP, redraw_cursor, &cr_Color, &bg_Color);
	}

	do_pages();
	return 0;	/* do_pages() returns if DBG_BATCH is specified */
}
