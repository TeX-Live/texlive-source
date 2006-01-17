#include "wingui.h"
#include "xdvi-config.h"
#include "version.h"
#include "c-openmx.h"
#include <kpathsea/proginit.h>
#include <kpathsea/tex-hush.h>
#include <kpathsea/tex-make.h>
#include <kpathsea/line.h>
#ifndef ALTFONT
#define ALTFONT "cmr10"
#endif

#if     defined(PS_GS) && !defined(GS_PATH)
#define GS_PATH "gsdll32.dll"
#endif

#ifdef WIN32
#define DEFAULT_PAPER NULL
#else
#ifndef A4
#define DEFAULT_PAPER           "us"
#else
#define DEFAULT_PAPER           "a4"
#endif
#endif

#ifndef BDPI
#define BDPI 600
#endif

#ifndef MFMODE
#define MFMODE NULL
#endif
#ifndef SHRINK
#define SHRINK "6"
#endif

#define option xdviw32_option

/*
 *      Data for options processing
 */

static  _Xconst char    silent[] = " "; /* flag value for usage() */

static  _Xconst char    subst[] = "x";  /* another flag value */

static  _Xconst char    *subst_val[] = {"-mgs[n] <size>"};

static	_Xconst char	*brdr_color;
static	_Xconst char	*high_color;
static	_Xconst char	*curs_color;

/* BEGIN CHUNK xdvi.c 1 */
#ifdef SRC_SPECIALS
#define src_tickSize			resource._src_tickSize
#define src_specialFormat		resource._src_specialFormat
#define src_jumpButton			resource._src_jumpButton
#endif
/* END CHUNK xdvi.c 1 */

Boolean iconic  = False;
Dimension       bwidth  = 2;

/*
 * Command line flags.
 */

int argc;
char **argv;
#define SEPARATORS " \t"
#define MAX_ARG 20

static  _Xconst char    *usagestr[] = {
  /* shrinkFactor */      "shrink",
#ifndef VMS
  /* S */                 "density",
  /* density */           silent,
#else
  /* density */           "density",
#endif
#ifdef  GREY
  /* gamma */             "g",
#endif
#ifdef GRID
  /* grid1 */             "color",
  /* grid2 */             "color",
  /* grid3 */             "color",
#endif /* GRID */
  /* p */                 "pixels",
  /* margins */           "dimen",
  /* sidemargin */        "dimen",
  /* topmargin */         "dimen",
  /* offsets */           "dimen",
  /* xoffset */           "dimen",
  /* yoffset */           "dimen",
  /* paper */             "papertype",
  /* altfont */           "font",
  /* mfmode */            "mode-def",
  /* rv */                "^-l", "-rv",
#ifdef	BUTTONS
  /* shrinkbutton1 */	subst,
  /* shrinkbutton2 */	silent,
  /* shrinkbutton3 */	silent,
  /* shrinkbutton4 */	silent,
#endif
  /* mgs */               subst,
  /* msg1 */              silent,
  /* msg2 */              silent,
  /* msg3 */              silent,
  /* msg4 */              silent,
  /* msg5 */              silent,
  /* bw */		"^-safer", "-bw <width>",
  /* fg */                "color",
  /* foreground */        silent,
  /* bg */                "color",
  /* background */        silent,
  /* hl */                "color",
  /* bd */                "^-hl", "-bd <color>",
  /* cr */                "color",
#ifndef VMS
  /* display */           "^-cr", "-display <host:display>",
#else
  /* display */           "^-cr", "-display <host::display>",
#endif
  /* geometry */          "^-cr", "-geometry <geometry>",
  /* icongeometry */      "geometry",
  /* iconic */            "^-icongeometry", "-iconic",
#ifdef  BUTTONS
  /* font */              "^-icongeometry", "-font <font>",
#endif
#ifdef  PS_GS
  /* interpreter */       "path",
  /* gspalette */         "monochrome|grayscale|color",
#endif
  /* debug */             "bitmask",
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
#ifdef WIN32
  /* single */            "single instance",
  /* colours */           "number of colours",
  /* auto scan */         "auto scan",
  /* in memory */         "in memory redraw",
  /* transformations */   "use transformations from postscript",
  /* two pages mode */    "two pages mode",
#endif
/* [dummy] */           "z"
};

#define ADDR(x) (caddr_t) &resource.x

static  struct option {
  _Xconst char    *name;
  _Xconst char    *resource;
  _Xconst char    *mark;
  enum {FalseArg, TrueArg, StickyArg, SepArg}
  argclass;
  enum {BooleanArg, Bool3Arg, StringArg, NumberArg, FloatArg}
  argtype;
  int             classcount;
  _Xconst char    *usagestr;
  caddr_t         address;
  Boolean         save_it;
}       options[] = {
  {"+",           NULL,  NULL,           StickyArg, StringArg, 1,
   NULL,         (caddr_t) &curr_page, False},
  {"-s",          "shrinkFactor",  NULL, SepArg, NumberArg, 1,
   "shrink",     ADDR(shrinkfactor), True},
#ifndef VMS
  {"-S",          NULL,  NULL,           SepArg, NumberArg, 2,
   "density",    ADDR(_density), True},
  {"-density",    "densityPercent",  NULL, SepArg, NumberArg, 1,
   silent,       ADDR(_density), True},
#else
  {"-density",    "densityPercent",  NULL, SepArg, NumberArg, 1,
   "density",    ADDR(_density), True},
#endif
#ifdef  GREY
  {"-nogrey",     NULL,    NULL,         FalseArg, BooleanArg, 2,
   NULL,         ADDR(_use_grey), True},
  {"+nogrey",     "grey",    NULL,       TrueArg, BooleanArg, 1,
   NULL,         ADDR(_use_grey), True},
  {"-gamma",      "gamma",   NULL,       SepArg, FloatArg, 1,
   "g",          ADDR(_gamma), True},
  {"-install",	NULL, NULL,		TrueArg, Bool3Arg, 2,
   NULL,		ADDR(install), False},
  {"-noinstall",  "install", NULL, FalseArg, Bool3Arg, 1,
   NULL,		ADDR(install), False},
#endif
#ifdef GRID
  {"-grid1",      "grid1Color",   NULL,  SepArg, StringArg, 1,
   "color",      ADDR(grid1_color), True},
  {"-grid2",      "grid2Color",   NULL,  SepArg, StringArg, 1,
   "color",      ADDR(grid2_color), True},
  {"-grid3",      "grid3Color",  NULL,   SepArg, StringArg, 1,
   "color",      ADDR(grid3_color), True},
#endif /* GRID */
  {"-p",          "pixelsPerInch",  NULL, SepArg, NumberArg, 1,
   "pixels",     ADDR(_pixels_per_inch), True},
  {"-margins",    "Margin",  NULL,       SepArg, StringArg, 3,
   "dimen",      (caddr_t) &margins, True},
  {"-sidemargin", "sideMargin",   NULL,  SepArg, StringArg, 1,
   "dimen",      ADDR(sidemargin), True},
  {"-topmargin",  "topMargin",    NULL,  SepArg, StringArg, 1,
   "dimen",      ADDR(topmargin), True},
  {"-offsets",    "Offset",    NULL,     SepArg, StringArg, 3,
   "dimen",      (caddr_t) &offsets, True},
  {"-xoffset",    "xOffset",    NULL,    SepArg, StringArg, 1,
   "dimen",      ADDR(xoffset), True},
  {"-yoffset",    "yOffset",   NULL,     SepArg, StringArg, 1,
   "dimen",      ADDR(yoffset), True},
  {"-paper",      "paper",   NULL,       SepArg, StringArg, 1,
   "papertype",  ADDR(paper), True},
  {"-altfont",    "altFont",   NULL,     SepArg, StringArg, 1,
   "font",       ADDR(_alt_font), True},
  {"-nomakepk",   NULL,    NULL,     FalseArg, BooleanArg, 2,
   NULL,         ADDR(makepk), True},
  {"+nomakepk",   "makePk",    NULL,     TrueArg, BooleanArg, 1,
   NULL,         ADDR(makepk), True},
  {"-mfmode",     "mfMode",    NULL,     SepArg, StringArg, 1,
   "mode-def",   ADDR(mfmode), True},
  {"-l",          NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(_list_fonts), True},
  {"+l",          "listFonts",   NULL,   FalseArg, BooleanArg, 1,
   NULL,         ADDR(_list_fonts), True},
  {"-rv",         NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(reverse), True},
  {"+rv",         "reverseVideo",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(reverse), True},
  {"-mgs",        NULL,     NULL,        SepArg, StringArg, 2,
   subst,        ADDR(mg_arg[0]), True},
  {"-mgs1",       "magnifierSize1",  NULL,SepArg, StringArg, 1,
   silent,       ADDR(mg_arg[0]), True},
  {"-mgs2",       "magnifierSize2",  NULL,SepArg, StringArg, 1,
   silent,       ADDR(mg_arg[1]), True},
  {"-mgs3",       "magnifierSize3",  NULL,SepArg, StringArg, 1,
   silent,       ADDR(mg_arg[2]), True},
  {"-mgs4",       "magnifierSize4",  NULL,SepArg, StringArg, 1,
   silent,       ADDR(mg_arg[3]), True},
  {"-mgs5",       "magnifierSize5",  NULL,SepArg, StringArg, 1,
   silent,       ADDR(mg_arg[4]), True},
  {"-warnspecials", NULL, NULL, 	TrueArg, BooleanArg, 2,
   NULL,		ADDR(_warn_spec), True},
  {"+warnspecials", "warnSpecials", NULL, FalseArg, BooleanArg, 1,
   NULL,		ADDR(_warn_spec), True},
  {"-hush",       NULL,     NULL,        TrueArg, BooleanArg, 6,
   NULL,         (caddr_t) &hush, True},
  {"+hush",       "Hush",    NULL,       FalseArg, BooleanArg, 5,
   NULL,         (caddr_t) &hush, True},
  {"-hushchars",  NULL,   NULL,          TrueArg, BooleanArg, 2,
   NULL,         ADDR(_hush_chars), True},
  {"+hushchars",  "hushLostChars",   NULL,FalseArg, BooleanArg, 1,
   NULL,         ADDR(_hush_chars), True},
  {"-hushchecksums", NULL,    NULL,      TrueArg, BooleanArg, 2,
   NULL,         ADDR(_hush_chk), True},
  {"+hushchecksums","hushChecksums",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(_hush_chk), True},
  {"-safer",      NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(safer), True},
  {"+safer",      "safer",    NULL,      FalseArg, BooleanArg, 1,
   NULL,         ADDR(safer), True},
  {"-bw",		NULL, NULL,	SepArg,	NumberArg, 2,
   "width",	(caddr_t) &bwidth},
  {"-borderwidth", "borderWidth",  NULL, SepArg, NumberArg, 1,
   silent,       (caddr_t) &bwidth, True},
  {"-fg",         NULL,      NULL,       SepArg, StringArg, 2,
   "color",      ADDR(fore_color), True},
  {"-foreground", "foreground",   NULL,  SepArg, StringArg, 1,
   silent,       ADDR(fore_color), True},
  {"-bg",         NULL,   NULL,          SepArg, StringArg, 2,
   "color",      ADDR(back_color), True},
  {"-background", "background",   NULL,  SepArg, StringArg, 1,
   silent,       ADDR(back_color), True},
#ifndef VMS
  {"-display",    NULL,     NULL,        SepArg, StringArg, 1,
   "host:display", (caddr_t) &display, False},
#else
  {"-display",    NULL,    NULL,         SepArg, StringArg, 1,
   "host::display", (caddr_t) &display, False},
#endif
  {"-geometry",   "geometry",   NULL,    SepArg, StringArg, 1,
   "geometry",   (caddr_t) &geometry, True},
  {"-icongeometry","iconGeometry",  NULL,StickyArg, StringArg, 1,
   "geometry",   ADDR(icon_geometry), False},
  {"-iconic",     NULL,     NULL,        TrueArg, BooleanArg, 2,
   NULL,         (caddr_t) &iconic, False},
  {"+iconic",     "iconic",    NULL,     FalseArg, BooleanArg, 1,
   NULL,         (caddr_t) &iconic, False},
  {"-keep",       NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(keep_flag), True},
  {"+keep",       "keepPosition",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(keep_flag), True},
  {"-copy",       NULL,     NULL,        TrueArg, Bool3Arg, 2,
   NULL,         ADDR(copy), False},
  {"+copy",       "copy",    NULL,       FalseArg, Bool3Arg, 1,
   NULL,         ADDR(copy), False},
  {"-thorough",   NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(thorough), False},
  {"+thorough",   "thorough",    NULL,   FalseArg, BooleanArg, 1,
   NULL,         ADDR(thorough), False},
#if     PS
  {"-nopostscript", NULL,    NULL,       FalseArg, BooleanArg, 2,
   NULL,         ADDR(_postscript), True},
  {"+nopostscript", "postscript",   NULL,TrueArg, BooleanArg, 1,
   NULL,         ADDR(_postscript), True},
  {"-noscan", NULL,     NULL,            FalseArg, BooleanArg, 2,
   NULL,         ADDR(prescan), True},
  {"+noscan",     "prescan",    NULL,    TrueArg, BooleanArg, 1,
   NULL,         ADDR(prescan), True},
  {"-allowshell", NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(allow_shell), True},
  {"+allowshell", "allowShell",   NULL,  FalseArg, BooleanArg, 1,
   NULL,         ADDR(allow_shell), True},
#ifdef  PS_DPS
  {"-nodps",      NULL,     NULL,        FalseArg, BooleanArg, 2,
   NULL,         ADDR(useDPS), True},
  {"+nodps",      "dps",    NULL,        TrueArg, BooleanArg, 1,
   NULL,         ADDR(useDPS), True},
#endif
#ifdef  PS_NEWS
  {"-nonews",     NULL,     NULL,        FalseArg, BooleanArg, 2,
   NULL,         ADDR(useNeWS), True},
  {"+nonews",     "news",    NULL,       TrueArg, BooleanArg, 1,
   NULL,         ADDR(useNeWS), True},
#endif
#ifdef  PS_GS
  {"-noghostscript",NULL,     NULL,      FalseArg, BooleanArg, 2,
   NULL,         ADDR(useGS), True},
  {"+noghostscript","ghostscript",  NULL, TrueArg, BooleanArg, 1,
   NULL,         ADDR(useGS), True},
  {"-nogssafer",  NULL,    NULL,         FalseArg, BooleanArg, 2,
   NULL,         ADDR(gs_safer), True},
  {"+nogssafer",  "gsSafer",    NULL,    TrueArg, BooleanArg, 1,
   NULL,         ADDR(gs_safer), True},
  {"-nogsalpha",	NULL,	NULL,	TrueArg, BooleanArg, 2,
   NULL,		ADDR(gs_alpha), True},
  {"+nogsalpha",	"gsAlpha", NULL,	FalseArg, BooleanArg, 1,
   NULL,		ADDR(gs_alpha), True},
  {"-interpreter", "interpreter",  NULL, SepArg, StringArg, 1,
   "path",       ADDR(gs_path), True},
  {"-gspalette",  "palette",    NULL,    SepArg, StringArg, 1,
   "monochrome|grayscale|color", ADDR(gs_palette), True},
#endif
#endif  /* PS */
  {"-debug",      "debugLevel",   NULL,  SepArg, StringArg, 1,
   "bitmask",    ADDR(debug_arg), False},
  {"-version",    NULL,     NULL,        TrueArg, BooleanArg, 2,
   NULL,         ADDR(version_flag), False},
  {"+version",    "version",   NULL,     FalseArg, BooleanArg, 1,
   NULL,         ADDR(version_flag), False},
#ifdef HTEX
  {"-underlink",  NULL,    NULL,  TrueArg, BooleanArg, 2,
   NULL,         ADDR(_underline_link), True},
  {"+underlink",  "underLink",  NULL,    FalseArg, BooleanArg, 1,
   NULL,         ADDR(_underline_link), True},
  {"-browser",    "wwwBrowser",   NULL,  SepArg, StringArg, 1,   
   NULL, ADDR(_browser), True},
  {"-base",       "urlBase",    NULL,    SepArg, StringArg, 1,   
   NULL, ADDR(_URLbase), True},
#endif
/* BEGIN CHUNK xdvi.c 5 */
#ifdef SRC_SPECIALS
  {"-srcSpecialFormat", "srcSpecialFormat", NULL, SepArg,    NumberArg,    1, 
   "[0,1,2]", (caddr_t)&src_specialFormat, True},
  {"-srcJumpButton",    "srcJumpButton",    NULL, SepArg,    NumberArg,    1, 
   "button", (caddr_t)&src_jumpButton, True},
  {"-srcTickShape",     "srcTickShape",     NULL, SepArg,    NumberArg,    1, 
   "[0,1,2,3]", (caddr_t)&src_tickShape, True},
  {"-srcTickSize",      "srcTickSize",      NULL, SepArg,    StringArg,    1, 
   "geometry", ADDR(_src_tickSize), True},
  {"-srcCursor",        "srcCursor",        NULL, SepArg,    NumberArg,    1, 
   "shape", (caddr_t)&src_cursor_shape, True},
  {"-srcEditorCommand", "srcEditorCommand", NULL, SepArg,    StringArg,    1, 
   "commandstring", (caddr_t)&src_editorCommand, True},
  {"-srcVerbosity",     "srcVerbosity",     NULL, SepArg,    NumberArg,    1, 
   "[0,1,2]", (caddr_t)&src_warn_verbosity, True},
  {"-srcVisibility",    "srcVisibility",    NULL, TrueArg,   BooleanArg,   2, 
   NULL, ADDR(_src_tickVisibility), True},
  {"+srcVisibility",    "srcVisibility",    NULL, FalseArg,  BooleanArg,   1, 
   NULL, ADDR(_src_tickVisibility), True},  
  {"-srcMode",          "srcMode",		  NULL, TrueArg,   BooleanArg,   2, 
   NULL, ADDR(_src_evalMode), True},
  {"+srcMode",          "srcMode",          NULL, FalseArg,  BooleanArg,   1, 
   NULL, ADDR(_src_evalMode), True},  
#endif  
#ifdef WIN32
  /* Run only one instance */
  {"-single",       NULL,    NULL,         TrueArg, BooleanArg, 3,
   NULL,         ADDR(single_flag), True},
  {"-1",       NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(single_flag), True},
  {"+single",       "singleInstance",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(single_flag), True},
  /* Depth of the main bitmap */
  {"-depth",      "depthText",   NULL,  SepArg, NumberArg, 1,
   "1|2|4|8|16|24",    ADDR(numColors), False},
  /* Support of color */

  /* Auto scan of dvi files */
  {"-autoscan",       NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(scan_flag), True},
  {"+autoscan",       "autoScan",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(scan_flag), True},
  {"-autolog",       NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(log_flag), True},
  {"+autolog",       "autoLog",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(log_flag), True},
  {"-inmemory",       NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(in_memory), True},
  {"+inmemory",       "inMemory",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(in_memory), True},
  {"-xform",       NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(use_xform), True},
  {"+xform",       "useXform",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(use_xform), True},
  {"-book",       NULL,    NULL,         TrueArg, BooleanArg, 2,
   NULL,         ADDR(book_mode), True},
  {"+book",       "bookMode",  NULL, FalseArg, BooleanArg, 1,
   NULL,         ADDR(book_mode), True},
#endif
};


struct _resource        resource = {
#if CFGFILE && TOOLKIT
  /* progname             */      "windvi",
#endif
#if TOOLKIT
  /* shrinkfactor         */      3,
#endif
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
  /* _list_fonts		*/	False,
  /* reverse		*/	False,
  /* _warn_spec		*/	False,
  /* _hush_chars		*/	False,
  /* _hush_chk		*/	False,
  /* safer		*/	False,
#if defined(VMS) || defined(WIN32) || !defined(TOOLKIT)
  /* fore_color		*/	NULL,
  /* back_color		*/	NULL,
#endif
  /* _fore_Pixel		*/	(Pixel) 0,
  /* _back_Pixel		*/	(Pixel) 0,
#ifdef TOOLKIT
  /* _brdr_Pixel          */      (Pixel) 0,
  /* _hl_Pixel            */      (Pixel) 0,
  /* _cr_Pixel            */      (Pixel) 0, 
#endif
  /* icon_geometry	*/	NULL,
  /* keep_flag		*/	False,
  /* copy			*/	False,
  /* thorough		*/	False,
#if	PS
  /* default is to use DPS, then NEWS, then GhostScript;
   * we will figure out later on which one we will use */
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
#ifdef	BUTTONS
  /* expert               */      False,
  /* shrinkbutton[4]      */      {100, 50, 33, 35},
#endif
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
#ifdef	TOOLKIT
  /* _grid1_Pixel         */      (Pixel) 0,
  /* _grid2_Pixel         */      (Pixel) 0,
  /* _grid3_Pixel         */      (Pixel) 0, 
#endif /* TOOLKIT */
#endif /* GRID */                                       
#ifdef HTEX
  /* _underline_link      */      True,
  /* _browser             */      (char *) NULL,
  /* _URLbase             */      (char *) NULL,
  /* _scroll_pages        */      (char *) NULL, 
#endif
/* BEGIN CHUNK xdvi.c 6 */
#ifdef SRC_SPECIALS
		/* _src_specialFormat   */		2,		
		/* _src_jumpButton      */		1,		
		/* _src_tickShape      */		1,		
		/* _src_tickSize      */		"40x70",		
		/* _src_cursor_shape  */  		90,		
		/* _src_editorCommand */		"emacsclient --no-wait \"+%u\" \"%s\"",		
		/* _src_warn_verbosity */		2,
		/* _src_tickVisibility */		False,
		/* _src_evalMode */				False,
#endif						
#ifdef WIN32
  /* single_flag          */      False,
  /* num_colors           */      8,
  /* scan_flag            */      True,
  /* log_flag             */      False,
  /* in_memory            */      False,
  /* use_xform            */      False,
  /* book_mode            */      False
#endif
};

void RestoreOptions();

/*
 *      Process the option table.  This is not guaranteed for all possible
 *      option tables, but at least it works for this one.
 */

void
parse_options(argc, argv)
     int argc;
     char **argv;
{
  char    **arg;
  char    **argvend = argv + argc;
  const char    *optstring;
  caddr_t addr;
  struct option *opt, *lastopt, *candidate;
  int     len1, len2, matchlen;

  /*
   * Step 0. Restore options from the last run.
   */
  for (opt = options; opt < options + XtNumber(options); ++opt) {
    opt->mark = opt->resource;
  }

  RestoreOptions();

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
    candidate->mark = (char *) candidate;
    /* store the value */
    addr = candidate->address;
    switch (candidate->argclass) {
    case FalseArg:  *((Boolean *) addr) = False; continue;
    case TrueArg:   *((Boolean *) addr) = True; continue;
    case StickyArg: optstring = *arg + strlen(candidate->name);
      break;
    case SepArg:
      ++arg;
      if (arg >= argvend) usage();
      optstring = *arg;
      break;
    }
    switch (candidate->argtype) {
    case StringArg: *((char **) addr) = (char *)optstring; break;
    case NumberArg: *((int *) addr) = atoi(optstring); break;
    case FloatArg:  *((float *) addr) = atof(optstring); break;
    default:  ;
    }
  }
  /*
   * Step 2.  Propagate classes for command line arguments.  Backwards.
   */
  for (opt = options + XtNumber(options) - 1; opt >= options; --opt)
    if (opt->mark == (char *) opt) {
      addr = opt->address;
      lastopt = opt + opt->classcount;
      for (candidate = opt; candidate < lastopt; ++candidate) {
	/* FIXME : resource or mark ? */
	if (candidate->mark != NULL) {
	  switch (opt->argtype) {
	  case BooleanArg:
	  case Bool3Arg:      /* same type as Boolean */
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
	  candidate->mark = NULL;
	}
      }
    }

#if 0
  if ((DISP = XOpenDisplay(display)) == NULL)
    oops("Can't open display");
  SCRN = DefaultScreenOfDisplay(DISP);
#endif

  /*
   * Step 3.  Handle resources (including classes).
   */
  for (opt = options; opt < options + XtNumber(options); ++opt)
    if (opt->mark &&
	((optstring = XGetDefault(DISP, prog, opt->resource)) ||
	 (optstring = XGetDefault(DISP, "XDvi", opt->resource))))
      {
	lastopt = opt + opt->classcount;
	for (candidate = opt; candidate < lastopt; ++candidate)
	  if (candidate->mark != NULL) switch (opt->argtype) {
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
	    * (char **) candidate->address = xstrdup((char *)optstring);
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

/* Analysis of arguments */
void ParseCmdLine(LPSTR lpCmdLine)
{
  int i;

#ifdef _TRACE
  fprintf(stderr, "Command Line : %s\n", lpCmdLine);
#endif

#if _DEBUG
  fprintf(stderr, "lpCmdLine = %s\n", lpCmdLine);
#endif
  sCmdArg = parse_cmdline(lpCmdLine, NULL, &sOutputName, NULL);

  argv = &sCmdArg[0][0];
  for(argc = 0; argv[argc] != NULL; argc++);

#if 0
  {
	int i;
	for (i = 0; i < argc; i++) {
	  fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
	}
  }
#endif
  /*
   *	Step 1:  Process command-line options and resources.
   */

  /* This has to be a special case, for now. */

  if (argc == 2 && (strcmp(argv[1], "-version") == 0 ||
		    strcmp(argv[1], "--version") == 0 ||
		    strcmp(argv[1], "+version") == 0
		    ))
    {
#ifdef WIN32
      Printf("windvi(k) version %s\n", TVERSION);
      Printf("Copyright (C) 1990-1998 Paul Vojta.\n\
Copyright (C) 1997-1999 Fabrice Popineau for the Win32 part.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License\n\
and the standard X consortium copyright notice.\n\
For more information about these matters, see the files\n\
named COPYING and windvi.c.\n\
Primary author of Xdvi: Paul Vojta; -k maintainer: janl@math.uio.no.\n\
Win32 maintainer: Fabrice.Popineau@supelec.fr\n");
#else
      Printf("xdvi(k) version %s\n", TVERSION);
      Printf("Copyright (C) 1990-1998 Paul Vojta.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License\n\
and the standard X consortium copyright notice.\n\
For more information about these matters, see the files\n\
named COPYING and xdvi.c.\n\
Primary author of Xdvi: Paul Vojta; -k maintainer: janl@math.uio.no.\n");
#endif
      /* Waiting acknowledge from user */
      while (bLogShown == TRUE);
      /* and exiting */
	  /*      PostMessage(hViewLog, WM_QUIT, 0, 0); */
      CleanExit(0);
    }

  if (argc == 2 && ((strcmp (argv[1], "-help") == 0)
		    || (strcmp (argv[1], "+help") == 0)
		    || (strcmp (argv[1], "--help") == 0)
		    ))
    {
      extern KPSEDLL char *kpse_bug_address;
#ifdef WIN32
      Printf("windvi(k) version %s\n", TVERSION);
      Printf("Preview a DVI file under the Win32 window system. %s\n",
	     kpse_bug_address);
#else
      Printf("xdvi(k) version %s\n", TVERSION);
      Printf("Preview a DVI file under the X window system. %s\n",
	     kpse_bug_address);
#endif
      usage (); /* Unfortunately this exits with status 1.  */
    }

#ifndef WIN32
#ifndef	VMS
  prog = strrchr(*argv, '/');
#else
  prog = strrchr(*argv, ']');
#endif
  if (prog != NULL) ++prog; else prog = *argv;

#ifdef	VMS
  if (strchr(prog, '.') != NULL) *strchr(prog, '.') = '\0';
#endif
#endif /* WIN32 */

  /* We can't initialize this at compile time, because it might be
     the result of a function call under POSIX. */
  n_files_left = OPEN_MAX;

  kpse_set_program_name (argv[0] = "windvi", "windvi");
  kpse_set_program_enabled (kpse_any_glyph_format,
			    MAKE_TEX_PK_BY_DEFAULT, kpse_src_compile);

#ifdef WIN32
  prog = kpse_program_name;
#endif

#ifdef	SELFAUTO
  argv0 = argv[0];
#endif

  parse_options(argc, argv);

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
#if !defined(SELFILE) && !defined(WIN32)
      || dvi_name == NULL
#endif
      ) usage();

  if (shrink_factor > 1) {
    bak_shrink = resource.shrinkfactor;
    mane.shrinkfactor = resource.shrinkfactor;	/* otherwise it's 1 */
  }
#if 0
  /* We do not allow -s 1 at startup. */
  if (resource.shrinkfactor == 1)
    resource.shrinkfactor = 2;
#endif

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


#ifdef GRID
  if (resource.grid1_color)
    grid1_Pixel = string_to_colorref(resource.grid1_color);
  if (resource.grid2_color)
    grid2_Pixel = string_to_colorref(resource.grid2_color);
  if (resource.grid3_color)
    grid3_Pixel = string_to_colorref(resource.grid3_color);
#endif /* GRID */

#if 0
  /* There was a -s option */

  /* We do not allow -s 1 at startup. */
  if (resource.shrinkfactor == 1)
    resource.shrinkfactor = 2;

  if (resource.shrinkfactor > 1)
    shrink_factor = resource.shrinkfactor;
#endif


#ifdef ALTFONT
  resource._alt_font = xstrdup(ALTFONT);
#else
  resource._alt_font = xstrdup("cmr10");
#endif

  if (resource.version_flag)
    Printf("windvi version %s\n", TVERSION);

  kpse_init_prog (uppercasify (kpse_program_name), 
		  pixels_per_inch, resource.mfmode, alt_font);
  
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

  hush_chk = hush_chk || kpse_tex_hush ("checksum");
  hush_chars = hush_chars || kpse_tex_hush ("lostchars");
  warn_spec = warn_spec || kpse_tex_hush ("special");
  kpse_make_tex_discard_errors = hush_chars;

  
  /* Can't use {Black,White}PixelOfScreen() any more */
  if (!resource.fore_color)
    resource.fore_color =
      xstrdup(resource.reverse ? "white" : "black");
  if (!resource.back_color)
    resource.back_color =
      xstrdup(resource.reverse ? "black" : "white");

  fore_Pixel = (resource.fore_color
		? string_to_colorref(resource.fore_color)
		: (resource.reverse ? WhitePixelOfScreen(SCRN)
		   : BlackPixelOfScreen(SCRN)));
  back_Pixel = (resource.back_color
		? string_to_colorref(resource.back_color)
		: (resource.reverse ? BlackPixelOfScreen(SCRN)
		   : WhitePixelOfScreen(SCRN)));
  /* FIXME : create brushes and pens for these ones, use accordingly ! */
  brdr_Pixel = (brdr_color ? string_to_colorref(brdr_color) : fore_Pixel);
  hl_Pixel = (high_color ? string_to_colorref(high_color) : fore_Pixel);
  cr_Pixel = (curs_color ? string_to_colorref(curs_color) : fore_Pixel);

#ifdef GRID
  if (resource.grid1_color)
    grid1_Pixel = string_to_colorref(resource.grid1_color);
  if (resource.grid2_color)
    grid2_Pixel = string_to_colorref(resource.grid2_color);
  if (resource.grid3_color)
    grid3_Pixel = string_to_colorref(resource.grid3_color);
#endif /* GRID */

#ifdef  GREY
  if (resource._gamma == 0.0) resource._gamma = 1.0;
#endif
  
}

NORETURN void
usage() {
#if defined(TOOLKIT) && !defined(WIN32)
  XrmOptionDescRec *opt;
  _Xconst	char	**usageptr = usagestr;
#else
  struct option	*opt;
#endif
  _Xconst char    **sv    = subst_val;
  _Xconst char    *str1;
  _Xconst char    *str2;
  int             col     = 23;
  int             n;

#ifdef Omega
  Fputs("Usage: owindvi [+[<page>]]", stderr);
#else
  Fputs("Usage: windvi [+[<page>]]", stderr);
#endif

#if 1
  for (opt = options; opt < options + XtNumber(options); ++opt) {
#if defined(TOOLKIT) && !defined(WIN32)
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
#else /* not TOOLKIT or WIN32 */
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
#endif
#ifdef SELFILE
  Fputs("[dvi_file]\n", stderr);
#else
  Fputs("dvi_file\n", stderr);
#endif
  /* Waiting acknowledge from user */
  while (bLogShown == TRUE);
  /* and exiting */
  CleanExit(1);
}
/*
  Saving options. In which file ?
  Saving can be done either in $(HOME)/windvi.cnf
  or in $(sysdir)/windvi.cnf
  Reading will be done first in $(TEXMFCNF)/windvi.cnf
  next in $(HOME)/windvi.cnf and in $(sysdir)/windvi.cnf if nothing found
  in $(HOME).
 */

/*
  Implement INI files.
  INI files are list of sections.
  Each section consist of a list of pairs (key, val).
  */

typedef struct ini_section {
  char *key;
  char *val;
  struct ini_section *n;
} ini_section;

typedef struct ini_file {
  char *s_name;
  ini_section *s;
  struct ini_file *n;
} ini_file;

/* 
   Retrieve a ret pointer to an existing ini section,
   or add it and return the new pointer.
   */
ini_file *GetIniSection(ini_file *pif, char *name, 
			ini_file **ret, boolean create)
{
  ini_file *p, *q;
  
  for(p = pif, q = NULL; p != NULL; q = p, p = p->n) {
    if (stricmp(p->s_name, name) == 0) {
      *ret = p;
      return pif;
    }
  }
  if (create) {
    p = (ini_file *)xmalloc(sizeof(ini_file));
    p->s_name = strdup(name);
    p->n = NULL;
    p->s = NULL;
    if (q != NULL) {
      /* Some q exists, so hang p to q->n and return p */
      q->n = p;
    }
    else {
      /* Not yet any section was built. */
      pif = p;
    }
    *ret = p;
    return pif;
  }
  else {
    *ret = NULL;
    return pif;
  }
}

/*
  Set the value of 'key' to 'val' in section 'name' of 'pif'.
  */
ini_file *SetIniValue(ini_file *pif, char *name, char *key, char*val)
{
  ini_file *f;
  ini_section *p, *q;

  /* With create flag, f can't be null */
  pif = GetIniSection(pif, name, &f, True);

  for (q = NULL, p = f->s; p != NULL; q = p, p = p->n) {
    if (stricmp(p->key, key) == 0) {
      /* set current value and return */
      if (p->val) free(p->val);
      p->val = strdup(val);
      return pif;
    }
  }
  p = (ini_section *)xmalloc(sizeof(ini_section));
  p->key = strdup(key);
  p->val = strdup(val);
  p->n = NULL;
  if (q != NULL)
    q->n = p;
  else
    f->s = p;
  return pif;
}

/*
  Retrieve the value of 'key' in section 'name' of 'pif'.
  */
char *GetIniValue(ini_file *pif, char *name, char *key)
{
  ini_file *f;
  ini_section *p;
  
  /* will not modify pif */
  pif = GetIniSection(pif, name, &f, False);
  
  if (!f) 
    return NULL;
  
  for (p = f->s; p != NULL; p = p->n) {
    if (stricmp(p->key, key) == 0) {
      return p->val;
    }
  }
  return NULL;
}

/*
  The windvi.cnf INI file. Needs a phantom node.
  */

ini_file *windvi_cnf;

/*
  Write an ini_file to file given.
  */
Boolean WriteOptions(FILE *f, ini_file *ini)
{
  /* First, save the implicit options structure.
   */
  
  struct option *opt;
  caddr_t addr;
  ini_file *p;
  ini_section *q;
  int i;
  extern iLastUsedFilesNum;
  extern char **lpLastUsedFiles;

  fprintf(f, "[Display]\n");
  for (opt = options; opt < options + XtNumber(options); ++opt) {
    if (opt->save_it && opt->resource) {
      fprintf(f, "%s=", opt->resource);
      addr = opt->address;
      switch (opt->argtype) {
      case BooleanArg:
	switch (*((Boolean *)addr)) {
	case False:
	  fprintf(f, "false\n");
	  break;
	case True:
	  fprintf(f, "true\n");
	  break;
	}
	break;
      case Bool3Arg:
	switch (*((Bool3 *)addr)) {
	case False:
	  fprintf(f, "false\n");
	  break;
	case Maybe:
	  fprintf(f, "maybe\n");
	  break;
	case True:
	  fprintf(f, "true\n");
	  break;
	}
	break;
      case StringArg:
	fprintf(f, "%s\n", (*((char **)addr) ? *((char **)addr) : ""));
	break;
      case NumberArg:
	fprintf(f, "%d\n", *((int *)addr));
	break;
      case FloatArg:
	fprintf(f, "%f\n", *((float *)addr));
	break;
      }
    }
  }
  /* Last used files */
  fprintf(f, "[Last Used Files]\n");
  fprintf(f, "numFiles=%d\n", iLastUsedFilesNum);
  for (i = 0; i < iLastUsedFilesNum; i++) {
    fprintf(f, "lastFile%d=%s\n", i, lpLastUsedFiles[i]);
#if 0
    fprintf(stderr, "SaveOptions : plLastUsedFiles[%d] = %s\n", i, 
	    lpLastUsedFiles[i]);
#endif
  }
  /* Save the rest of the ini_file */
  for(p = ini; p != NULL; p = p->n) {
    /* Do not write the Display section again. */
    if (stricmp(p->s_name, "Display") && stricmp(p->s_name, "Last Used Files")) {
      fprintf(f, "[%s]\n", p->s_name);
      for(q = p->s; q != NULL; q = q->n) {
	fprintf(f, "%s=%s\n", q->key, q->val);
      }
    }
  }

  return True;
}

void SaveOptions()
{
  FILE *outf = NULL;
  char *homedir = NULL;
  char *save_name = NULL;

  /* FIXME : change this for:
     - save in %HOME% if defined
     - else try TEXMFLOCAL/texmf/windvi.cnf (if writable)
     - else try TEXMFMAIN/texmf/windvi.cnf (if writable)
     - else do not save options and warn user
  */
  if (homedir = getenv("HOME")) {
	if (IS_DIR_SEP(homedir[strlen(homedir) - 1])) {
	  save_name = concat(homedir, "windvi.cnf");
	}
	else {
	  save_name = concat(homedir, "/windvi.cnf");
	}
	
	if ((outf = fopen(save_name, "w")) == NULL) {
	  char buf[_MAX_PATH];
	  sprintf(buf, "%s not writable !", save_name);
	  MessageBox(hWndMain, buf, NULL, 
				 MB_APPLMODAL | MB_ICONERROR | MB_OK);
	  return;
	}
  }
  else {
    save_name = concat (kpse_path_expand("$TEXMFLOCAL/web2c"), "/windvi.cnf");
    if (save_name == NULL || (outf = fopen(save_name, "w")) == NULL) {
      free(save_name);
      save_name = kpse_find_file("windvi.cnf", kpse_cnf_format, True);
      if (save_name == NULL || (outf = fopen(save_name, "w")) == NULL) {
		MessageBox(hWndMain, "No place in $TEXMFCNF to save options.", NULL, 
				   MB_APPLMODAL | MB_ICONERROR | MB_OK);
		return;
      }
    }
  }
  if (save_name) 
    free(save_name);
  if (outf) {
    WriteOptions(outf, windvi_cnf);
  }
  else {
    MessageBox(hWndMain, "No place to save options.", NULL, 
	       MB_APPLMODAL | MB_ICONERROR | MB_OK);
    return;
  }
  fclose(outf);
}

ini_file *ReadOptions(FILE *f, ini_file *ini)
{
  char *line;
  char *p;
  ini_file *pif;
  
  pif = NULL;
  
  while ((line = read_line(f)) != NULL) {
#if 0
    fprintf(stderr, "ReadOptions : %s\n", line);
#endif
    /* Wait for a section */
    if ((*line == '[') && (p = strchr(line, ']'))) {
      *p = '\0';
#if 0
      fprintf(stderr, "ReadOptions : new section %s\n", line+1);
#endif
      ini = GetIniSection(ini, line + 1, &pif, True);
    }
    else {
      /* there is a current pif, scan the line for = */
      if (!(p = strchr(line, '='))) {
		/* invalid line, discard it */
      }
      else {
		*p = '\0';
		ini = SetIniValue(ini, pif->s_name, line, p+1);
      }
    }
    free(line);
  }
  return ini;
}

void RestoreOptions()
{
  FILE *inf;
  char *save_name, *homedir;

  /* First, look for a site config file. */
  save_name = kpse_find_file("windvi.cnf", kpse_cnf_format, True);
  if (save_name && *save_name) {
    if ((inf = fopen(save_name, "r")) != NULL) {
      windvi_cnf = ReadOptions(inf, windvi_cnf);
      fclose(inf);
    }
    else {
      MessageBox(hWndMain, "Fail reading site configuration file.", NULL, 
		 MB_APPLMODAL | MB_ICONERROR | MB_OK);
    }
    free(save_name);
  }
  /* Second, overwrite options values by a private
     config file. */
  if ((homedir = getenv("HOME")) || (homedir = "c:/")) {
    if (IS_DIR_SEP(homedir + strlen(homedir) - 1)) {
      save_name = concat(homedir, "windvi.cnf");
    }
    else {
      save_name = concat(homedir, "/windvi.cnf");
    }
    if ((inf = fopen(save_name, "r")) != NULL) {
      windvi_cnf = ReadOptions(inf, windvi_cnf);
      fclose(inf);
    }
    free(save_name);
  }
  /* In case no .cnf file, create dummy entries */
  if (windvi_cnf == NULL) {
	ini_file *pif;
	windvi_cnf = GetIniSection(windvi_cnf, "Display", &pif, True);
	windvi_cnf = GetIniSection(windvi_cnf, "Last Used Files", &pif, True);
	windvi_cnf = SetIniValue(windvi_cnf, pif->s_name, "numFiles", "0");
  }

  /* Report the last used files in the queue */
  {
    int i;
    extern iLastUsedFilesNum;
    extern char **lpLastUsedFiles;
    char *num_files = GetIniValue(windvi_cnf, "Last Used Files", "numFiles");
    if (num_files) {
      char buf[sizeof("lastFile")+5];
      iLastUsedFilesNum = atoi(num_files);
      lpLastUsedFiles = (char **)xmalloc(iLastUsedFilesNum*sizeof(char*));
      for (i = 0; i < iLastUsedFilesNum; i++) {
	char *p;
	sprintf(buf, "lastFile%d", i);
	if (p = GetIniValue(windvi_cnf, "Last Used Files", buf)) {
	  lpLastUsedFiles[i] = xstrdup(p);
	}
	else
	  lpLastUsedFiles[i] = xstrdup("");
#if 0
	fprintf(stderr, "RestoreOptions : plLastUsedFiles[%d] = %s\n", i, lpLastUsedFiles[i]);
#endif
      }
    }
    else {
      /* Allocate 4 Used Files by default */
      iLastUsedFilesNum = 4;
      lpLastUsedFiles = (char **)xmalloc(iLastUsedFilesNum*sizeof(char*));
      for (i = 0; i < iLastUsedFilesNum; i++)
	lpLastUsedFiles[i] = xstrdup("");
    }
  }      
}

void free_ini_section(ini_section *s)
{
  if (s) {
    if (s->key) free(s->key);
    if (s->val) free(s->val);
    free_ini_section(s->n);
    free(s);
  }
}

void free_ini_file(ini_file *f)
{
  if (f) {
    if (f->s_name) free(f->s_name);
    free_ini_section(f->s);
    free_ini_file(f->n);
    free(f);
  }
}

void FreeOptions()
{
  free_ini_file(windvi_cnf);
}

const char *XGetDefault(void *v, char *prog, char *key)
{
  char *ret;
  ret = GetIniValue(windvi_cnf, "Display", key);
  /* We do not allow "" */
  if (ret && !*ret) return NULL;
  return ret;
}
