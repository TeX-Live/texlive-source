/*========================================================================*\

Copyright (c) 1990-2004  Paul Vojta and others

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL PAUL
VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

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
 * 1/2001	Added source specials	--including ideas from Stefan Ulrich,
 *					  U Munich
 *
 * Compilation options:
 *
 * VMS		    compile for VMS
 * WORDS_BIGENDIAN  store bitmaps internally with most significant bit first
 * BMTYPE	    store bitmaps in unsigned BMTYPE
 * BMBYTES	    sizeof(unsigned BMTYPE)
 * ALTFONT	    default for -altfont option
 * SHRINK	    default for -s option (shrink factor)
 * MFMODE	    default for -mfmode option
 * A4		    use European size paper, and change default dimension to cm
 * TEXXET	    support reflection dvi codes (right-to-left typesetting)
 * GREY		    use grey levels to shrink fonts
 * PS_GS	    use Ghostscript to render pictures/bounding boxes
 * PS_DPS	    use display postscript to render pictures/bounding boxes
 * PS_NEWS	    use the NeWS server to render pictures/bounding boxes
 * GS_PATH	    path to call the Ghostscript interpreter by
 * MAGICK	    use ImageMagick to render (external) image files
 */


#include "xdvi-config.h"
#include "xdvi.h"
#include "version.h"

/* Xlib and Xutil are already included */
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>	/* needed for def. of XtNiconX */

#ifdef HAVE_X11_XMU_EDITRES_H
# include <X11/Xmu/Editres.h>
#endif

/* to allow one common dependency file for Xaw/Motif,
   we always include all the headers and have
   #ifdef MOTIF
   tests inside the headers.
*/
#if defined(NEW_MENU_CREATION) || defined(MOTIF)
#include "menu.h"	
#else
#include "xm_menu.h"	
#include "xaw_menu.h"
#endif /* NEW_MENU_CREATION */
#include "xm_toolbar.h"

#ifdef T1LIB
#include "t1lib.h"
#endif /* T1LIB */

#ifdef MOTIF

# include <Xm/Xm.h>
# include <Xm/Frame.h>
# include <Xm/PushB.h>
# include <Xm/MainW.h>
# include <Xm/ToggleB.h>
# include <Xm/RowColumn.h>
# include <Xm/ScrolledW.h>
# include <Xm/MenuShell.h>
# include <Xm/DrawingA.h>
# include <Xm/Form.h>
# include <Xm/PanedW.h>
# include <Xm/List.h>
# include <Xm/Protocols.h>

# include <Xm/Display.h>

# ifdef MOTIF11 /* FIXME: We'll probably need a config check for this? */
/* used to set up XmStrings */
XmStringCharSet G_charset = (XmStringCharSet)XmSTRING_DEFAULT_CHARSET;
# else
XmStringCharSet G_charset = XmFONTLIST_DEFAULT_TAG;
# endif

#else /* MOTIF */

# include <X11/Xaw/Viewport.h>
# include <X11/Xaw/AsciiText.h>
# include <X11/Xaw/Box.h>
# include <X11/Xaw/Command.h>
# include <X11/Xaw/Dialog.h>
# include <X11/Xaw/Form.h>
# include <X11/Xaw/Paned.h>
# include <X11/Xaw/Scrollbar.h>
# include <X11/Xaw/Command.h>

#endif /* MOTIF */

#include <signal.h>
#include <stdlib.h>
#include <ctype.h>

#include "xserver-info.h"

#include "kpathsea/c-fopen.h"
#include "kpathsea/c-pathch.h"
#include "kpathsea/c-stat.h"
#include "kpathsea/progname.h"
#include "kpathsea/tex-file.h"
#include "kpathsea/tex-hush.h"
#include "kpathsea/tex-make.h"
#include "string-utils.h"
#include "kpathsea/c-errno.h"

#include "translations.h"
#include "dvi-init.h"
#include "c-openmx.h"
#include "xicon.h"
#include "x_util.h"
#include "message-window.h"
#include "events.h"
#include "mag.h"
#include "pagesel.h"
#include "dvi-draw.h"
#include "statusline.h"
#include "util.h"
#include "hypertex.h"
#include "xaw_menu.h"
#include "xdvi-debug.h"
#include "pagehist.h"
#include "filehist.h"
#include "print-internal.h"
#include "exit-handlers.h"
#include "xm_prefsP.h" /* for Xdvi_PREFS_BROWSER_DEFAULTS and Xdvi_PREFS_EDITOR_DEFAULTS */

#ifdef VMS 
#  include "pixmaps/hand.xbm"
#  include "pixmaps/hand_mask.xbm"
#endif
#include "pixmaps/magglass.xbm"
#include "pixmaps/magglass_mask.xbm"

#ifdef DEBUG
#include<asm/msr.h>
unsigned long time_start=0, time_end=0;
#endif


#ifdef MOTIF
#include <Xm/MwmUtil.h>

#else /* MOTIF */

/* need to fake it */

/* bit definitions for MwmHints.flags */
#define MWM_HINTS_FUNCTIONS (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_HINTS_INPUT_MODE (1L << 2)
#define MWM_HINTS_STATUS (1L << 3)

/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL  (1L << 0)
#define MWM_DECOR_BORDER (1L << 1)
#define MWM_DECOR_RESIZEH (1L << 2)
#define MWM_DECOR_TITLE  (1L << 3)
#define MWM_DECOR_MENU  (1L << 4)
#define MWM_DECOR_MINIMIZE (1L << 5)
#define MWM_DECOR_MAXIMIZE (1L << 6)

struct PropMotifWmHints {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long          inputMode;
    unsigned long status;
};

#define PROP_MOTIF_WM_HINTS_ELEMENTS 5

#endif /* MOTIF */

#if HAVE_X11_INTRINSICI_H
# include <X11/IntrinsicI.h>
#else

/* Taken from <X11/TranslateI.h> in libXt-1.0.4 (Oct. 2006) */
typedef struct _LateBindings {
	unsigned int	knot:1;
	unsigned int	pair:1;
	unsigned short	ref_count;
	KeySym		keysym;
} LateBindings, *LateBindingsPtr;

#endif /* HAVE_X11_INTRINSICI_H */

struct mouse_acts	*mouse_actions;

char *dvi_property;		/* for setting in window */
size_t dvi_property_length;	/* length of above, for efficiency */
XImage *G_image;
int G_backing_store;
Display *DISP;
Screen *SCRN;
XtAccelerators G_accels_cr;

#ifdef GREY
Visual		*G_visual;
unsigned int	G_depth;
Colormap	G_colormap;
#else
# define G_depth	(unsigned int) DefaultDepthOfScreen(SCRN)
# define G_visual	DefaultVisualOfScreen(SCRN)
# define G_colormap	DefaultColormapOfScreen(SCRN)
#endif

/* global widgets */

#if defined(MOTIF)
Widget page_list, page_list_scroll;
#if USE_XAW_PANNER
#include "Panner.h"
Widget panner;
static Dimension g_save_shadow_thickness;
#endif
#endif

static Atom mainDeleteWindow;

#ifndef MOTIF
static Atom wmProtocols;
static void
handle_delete_message(Widget widget, XtPointer closure, XEvent *event, Boolean *cont)
{
    UNUSED(cont);
    UNUSED(widget);

    ASSERT(closure == NULL, "handle_delete_message doesn't accept a non-NULL closure argument");
    if (event->type == ClientMessage
	&& event->xclient.message_type == wmProtocols
	&& (unsigned)(event->xclient.data.l[0]) == mainDeleteWindow) {
	/* non-k xdvi also evaluates closure, but we don't, since it's a function pointer
	   which isn't convertible to void * in ANSI C */
	xdvi_exit(EXIT_SUCCESS);
    }
}
#endif

#ifdef TEST_SCROLLING
#warning ========== compiling with TEST_SCROLLING ==========
#endif


/* for measuring distance from the ruler */
int g_ruler_pos_x, g_ruler_pos_y;

Boolean ignore_papersize_specials = False;

#ifndef	ALTFONT
# define ALTFONT    "cmr10"
#endif

#ifndef	SHRINK
# define SHRINK	    8
#endif

#ifndef	MFMODE
# define MFMODE	    NULL
#endif

#undef MKTEXPK
#define MKTEXPK MAKEPK

#if defined(PS_GS) && !defined(GS_PATH)
# define GS_PATH "gs"
#endif

static Dimension bwidth = 2;

struct x_resources resource;

struct program_globals globals;

/* color of cursor */
static XColor m_cursor_color;


struct WindowRec mane = { (Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0 };

/* currwin is temporary storage except for within redraw() */
struct WindowRec currwin = { (Window) 0, 1, 0, 0, 0, 0, MAXDIM, 0, MAXDIM, 0 };

#define	offset(field)	XtOffsetOf(struct x_resources, field)
static int base_tick_length = 4;

static char XtRBool3[] = "Bool3";	/* resource for Bool3 */

static XtResource application_resources[] = {
    {"regression", "Regression", XtRBoolean, sizeof(Boolean),
     offset(regression), XtRString, "false"},
    {"geometry", "Geometry", XtRString, sizeof(char *),
     offset(geometry), XtRString, (XtPointer) NULL},
    {"windowSize", "WindowSize", XtRString, sizeof(char *),
     offset(windowsize), XtRString, (XtPointer) NULL},
    {"rememberWindowSize", "RememberWindowSize", XtRBoolean, sizeof(Boolean),
     offset(remember_windowsize), XtRString, "false"},
    /* used to check whether app-defaults file is out of sync. Initialize
       to a low default value (one before the resource was introduced) */
    {"appDefaultsFileVersion", "AppDefaultsFileVersion", XtRInt, sizeof(int),
     offset(app_defaults_fileversion), XtRImmediate, (XtPointer)20030302 },
    {"shrinkFactor", "ShrinkFactor", XtRInt, sizeof(int),
     offset(shrinkfactor), XtRImmediate, (XtPointer) SHRINK},
    {"delayRulers", "DelayRulers", XtRBoolean, sizeof(Boolean),
     offset(delay_rulers), XtRString, "true"},
    {"useTeXPages", "UseTeXPages", XtRBoolean, sizeof(Boolean),
     offset(use_tex_pages), XtRString, "false"},
    {"densityPercent", "DensityPercent", XtRInt, sizeof(int),
     offset(density), XtRString, "40"},
    {"omega", "Omega", XtRBoolean, sizeof(Boolean),
     offset(omega), XtRString, "true"},
    {"mainTranslations", "MainTranslations", XtRString, sizeof(char *),
     offset(main_translations), XtRString, (XtPointer) NULL},
    {"mouseTranslations", "MouseTranslations", XtRString, sizeof(char *),
     offset(mouse_translations), XtRString, (XtPointer) NULL },
    {"wheelUnit", "WheelUnit", XtRInt, sizeof(int),
     offset(wheel_unit), XtRImmediate, (XtPointer) 80},
    {"mouseMode", "MouseMode", XtRInt, sizeof(int),
     offset(mouse_mode), XtRImmediate, (XtPointer) MOUSE_MODE1 },
    {"mouseMode1Name", "MouseMode1Name", XtRString, sizeof(char *),
     offset(mouse_mode1_name), XtRString, (XtPointer) "Magnifier"},
    {"mouseMode1Description", "MouseMode1Description", XtRString, sizeof(char *),
     offset(mouse_mode1_description), XtRString, (XtPointer) "click to enlarge text"},
    {"mouseMode1Cursor", "MouseMode1Cursor", XtRInt, sizeof(int),
     offset(mouse_mode1_cursor), XtRImmediate, (XtPointer) -1},
    {"mouseMode2Name", "MouseMode2Name", XtRString, sizeof(char *),
     offset(mouse_mode2_name), XtRString, (XtPointer) "Text Selection"},
    {"mouseMode2Description", "MouseMode2Description", XtRString, sizeof(char *),
     offset(mouse_mode2_description), XtRString, (XtPointer) "click and drag to select a region of text"},
    {"mouseMode2Cursor", "MouseMode2Cursor", XtRInt, sizeof(int),
     offset(mouse_mode2_cursor), XtRImmediate, (XtPointer) XC_cross },
    {"mouseMode3Name", "MouseMode3Name", XtRString, sizeof(char *),
     offset(mouse_mode3_name), XtRString, (XtPointer) "Ruler"},
    {"mouseMode3Description", "MouseMode3Description", XtRString, sizeof(char *),
     offset(mouse_mode3_description), XtRString, (XtPointer) "click and drag to set/move ruler"},
    {"mouseMode3Cursor", "MouseMode3Cursor", XtRInt, sizeof(int),
     offset(mouse_mode3_cursor), XtRImmediate, (XtPointer) XC_crosshair },
#ifdef GREY
    {"gamma", "Gamma", XtRFloat, sizeof(float),
     offset(gamma), XtRString, "1"},
    /*     {"invertedFactor", "InvertedFactor", XtRFloat, sizeof(float), */
    /*      offset(inverted_factor), XtRString, "3.0"}, */
#endif
    {"pixelsPerInch", "PixelsPerInch", XtRInt, sizeof(int),
     offset(pixels_per_inch), XtRImmediate, (XtPointer) BDPI},
    {"sideMargin", "Margin", XtRString, sizeof(char *),
     offset(sidemargin), XtRString, (XtPointer) NULL},
    {"tickLength", "TickLength", XtRInt, sizeof(int),
     offset(tick_length), XtRInt, (XtPointer) &base_tick_length},
    {"tickUnits", "TickUnits", XtRString, sizeof(char *),
     offset(tick_units), XtRString, "mm"},
    {"topMargin", "Margin", XtRString, sizeof(char *),
     offset(topmargin), XtRString, (XtPointer) NULL},
    {"xOffset", "Offset", XtRString, sizeof(char *),
     offset(xoffset), XtRString, (XtPointer) NULL},
    {"yOffset", "Offset", XtRString, sizeof(char *),
     offset(yoffset), XtRString, (XtPointer) NULL},
    {"useCurrentOffset", "UseCurrentOffset", XtRBoolean, sizeof(Boolean),
     offset(use_current_offset), XtRString, "False" },
    {"paper", "Paper", XtRString, sizeof(char *),
     offset(paper), XtRString, (XtPointer) DEFAULT_PAPER},
    {"paperLandscape", "PaperLandscape", XtRBoolean, sizeof(Boolean),
     offset(paper_landscape), XtRString, "false"},
    {"altFont", "AltFont", XtRString, sizeof(char *),
     offset(alt_font), XtRString, (XtPointer) ALTFONT},
    {"makePk", "MakePk", XtRBoolean, sizeof(Boolean),
     offset(makepk), XtRString,
#ifdef MAKE_TEX_PK_BY_DEFAULT
     "true"
#else
     "false"
#endif
    },
    {"mfMode", "MfMode", XtRString, sizeof(char *),
     offset(mfmode), XtRString, MFMODE},
    {"editor", "Editor", XtRString, sizeof(char *),
     offset(editor), XtRString, (XtPointer) NULL},
#ifdef T1LIB
    {"t1lib", "T1lib", XtRBoolean, sizeof(Boolean),
     offset(t1lib), XtRString, "true"},
#endif
    {"sourcePosition", "SourcePosition", XtRString, sizeof(char *),
     offset(src_pos), XtRString, (XtPointer) NULL},
    {"findString", "FindString", XtRString, sizeof(char *),
     offset(find_string), XtRString, (XtPointer) NULL},
    {"textEncoding", "TextEncoding", XtRString, sizeof(char *),
     offset(text_encoding), XtRString, (XtPointer) NULL},
    {"fork", "Fork", XtRBoolean, sizeof(Boolean),
     offset(src_fork), XtRString, "true"},
#ifdef RGB_ANTI_ALIASING
    {"subPixels", "SubPixels", XtRString, sizeof(char *),
     offset(sub_pixels), XtRString, "Unknown"},
#endif
    {"noFileArgUseHistory", "NoFileArgUseHistory", XtRBoolean, sizeof(Boolean),
     offset(no_file_arg_use_history), XtRString, "true"},
    {"fileHistory", "FileHistory", XtRString, sizeof(char *),
     offset(file_history), XtRString, (XtPointer) NULL},
    {"fileHistorySize", "FileHistorySize", XtRInt, sizeof(int),
     offset(file_history_size), XtRImmediate, (XtPointer)20},
    {"unique", "Unique", XtRBoolean, sizeof(Boolean),
     offset(unique), XtRString, "false"},
    {"listFonts", "ListFonts", XtRBoolean, sizeof(Boolean),
     offset(list_fonts), XtRString, "false"},
    {"reverseVideo", "ReverseVideo", XtRBoolean, sizeof(Boolean),
     offset(reverse), XtRString, "false"},
    {"warnSpecials", "WarnSpecials", XtRBoolean, sizeof(Boolean),
     offset(warn_spec), XtRString, "false"},
    {"hush", "Hush", XtRBoolean, sizeof(Boolean),
     offset(hush), XtRString, "false"},
    {"hushLostChars", "HushLostChars", XtRBoolean, sizeof(Boolean),
     offset(hush_chars), XtRString, "false"},
    {"hushChecksums", "HushChecksums", XtRBoolean, sizeof(Boolean),
     offset(hush_chk), XtRString, "false"},
    {"hushStdout", "HushStdout", XtRBoolean, sizeof(Boolean),
     offset(hush_stdout), XtRString, "false"},
    {"hushBell", "HushBell", XtRBoolean, sizeof(Boolean),
     offset(hush_bell), XtRString, "false"},
    {"safer", "Safer", XtRBoolean, sizeof(Boolean),
     offset(safer), XtRString, "false"},
#ifdef VMS
    {"foreground", "Foreground", XtRString, sizeof(char *),
     offset(fore_color), XtRString, (XtPointer) NULL},
    {"background", "Background", XtRString, sizeof(char *),
     offset(back_color), XtRString, (XtPointer) NULL},
#endif
    {"iconGeometry", "IconGeometry", XtRString, sizeof(char *),
     offset(icon_geometry), XtRString, (XtPointer) NULL},
    {"keepPosition", "KeepPosition", XtRBoolean, sizeof(Boolean),
     offset(keep_flag), XtRString, "false"},
#ifdef PS
    {"postscript", "Postscript", XtRInt, sizeof(int),
     offset(postscript), XtRImmediate, (XtPointer)1},
    {"allowShell", "AllowShell", XtRBoolean, sizeof(Boolean),
     offset(allow_shell), XtRString, "false"},
# ifdef	PS_DPS
    {"dps", "DPS", XtRBoolean, sizeof(Boolean),
     offset(useDPS), XtRString, "true"},
# endif
# ifdef	PS_NEWS
    {"news", "News", XtRBoolean, sizeof(Boolean),
     offset(useNeWS), XtRString, "true"},
# endif
# ifdef	PS_GS
    {"ghostscript", "Ghostscript", XtRBoolean, sizeof(Boolean),
     offset(useGS), XtRString, "true"},
    {"gsSafer", "Safer", XtRBoolean, sizeof(Boolean),
     offset(gs_safer), XtRString, "true"},
    {"gsAlpha", "Alpha", XtRBoolean, sizeof(Boolean),
     offset(gs_alpha), XtRString, "false"},
    {"interpreter", "Interpreter", XtRString, sizeof(char *),
     offset(gs_path), XtRString, (XtPointer) GS_PATH},
    {"palette", "Palette", XtRString, sizeof(char *),
     offset(gs_palette), XtRString, (XtPointer) "Color"},
    {"gsTimeout", "GSTimeout", XtRInt, sizeof(int),
     offset(gs_timeout), XtRImmediate, (XtPointer)3000},
# endif /* PS_GS */
# ifdef	MAGICK
    {"magick", "ImageMagick", XtRBoolean, sizeof(Boolean),
     offset(useMAGICK), XtRString, "true"},
    {"magick_cache", "MagickCache", XtRString, sizeof(char *),
     offset(magick_cache), XtRString, (XtPointer) NULL},
# endif
#endif /* PS */
    {"prescan", "Prescan", XtRBoolean, sizeof(Boolean),
     offset(prescan), XtRString, "true"},
    {"tempFile", "TempFile", XtRBoolean, sizeof(Boolean),
     offset(use_temp_fp), XtRString, "true"},
    {"copy", "Copy", XtRBoolean, sizeof(Boolean),
     offset(copy), XtRString, "false"},
    {"thorough", "Thorough", XtRBoolean, sizeof(Boolean),
     offset(thorough), XtRString, "false"},
    {"fullscreen", "Fullscreen", XtRBoolean, sizeof(Boolean),
     offset(fullscreen), XtRString, "false"},
    {"pause", "Pause", XtRBoolean, sizeof(Boolean),
     offset(pause), XtRString, "false"},
    {"pauseSpecial", "PauseSpecial", XtRString, sizeof(char *),
     offset(pause_special), XtRString, (XtPointer)"xdvi:pause"},
    {"debugLevel", "DebugLevel", XtRString, sizeof(char *),
     offset(debug_arg), XtRString, (XtPointer) NULL},
    {"menuTranslations", "MenuTranslations", XtRString, sizeof(char *),
     offset(menu_translations), XtRString, (XtPointer) default_menu_config},
    {"watchFile", "WatchFile", XtRFloat, sizeof(float),
     offset(watch_file), XtRString, "0"},
    {"expert", "Expert", XtRBoolean, sizeof(Boolean),
     offset(expert), XtRString, (XtPointer) NULL},
    {"expertMode", "ExpertMode", XtRInt, sizeof(int),
     offset(expert_mode), XtRImmediate, (XtPointer)31 /* everything on */ },
#ifndef MOTIF
    {"buttonSideSpacing", "ButtonSpacing", XtRDimension, sizeof(Dimension),
     offset(btn_side_spacing), XtRImmediate, (XtPointer) 8},
    {"buttonTopSpacing", "ButtonSpacing", XtRDimension, sizeof(Dimension),
     offset(btn_top_spacing), XtRImmediate, (XtPointer) 16},
    {"buttonBetweenSpacing", "ButtonSpacing", XtRDimension, sizeof(Dimension),
     offset(btn_between_spacing), XtRImmediate, (XtPointer) 8},
    /* only used if menus consist of buttons only */
    {"buttonBetweenExtra", "ButtonSpacing", XtRDimension, sizeof(Dimension),
     offset(btn_between_extra), XtRImmediate, (XtPointer)16},
    {"buttonBorderWidth", "BorderWidth", XtRDimension, sizeof(Dimension),
     offset(btn_border_width), XtRImmediate, (XtPointer) 1},
#endif /* MOTIF */
    {"statusline", "Statusline", XtRBoolean, sizeof(Boolean),
     offset(statusline), XtRString, (XtPointer) NULL},
#ifdef MOTIF
    {"toolbarTranslations", "ToolbarTranslations", XtRString, sizeof(char *),
     offset(toolbar_translations), XtRString, (XtPointer) default_toolbar_translations},
    {"toolbarPixmapFile", "ToolbarPixmapFile", XtRString, sizeof(char *),
     offset(toolbar_pixmap_file), XtRString, (XtPointer) "toolbar.xpm"},
    {"toolbarButtonsRaised", "ToolbarButtonsRaised", XtRBoolean, sizeof(Boolean),
     offset(toolbar_buttons_raised), XtRString, "True"},
    {"tooltipsInStatusline", "TooltipsInStatusline", XtRBoolean, sizeof(Boolean),
     offset(tooltips_in_statusline), XtRString, "True"},
    {"showTooltips", "ShowTooltips", XtRBoolean, sizeof(Boolean),
     offset(show_tooltips), XtRString, "True"},
#endif /* MOTIF */
    {"pageListHighlightCurrent", "PageListHighlightCurrent", XtRBoolean, sizeof(Boolean),
     offset(pagelist_highlight_current), XtRString, "True"},
    {"pageListWidth", "PageListWidth", XtRDimension, sizeof(Dimension),
     offset(pagelist_width), XtRImmediate, (XtPointer) 80},
    {"magnifierSize1", "MagnifierSize", XtRString, sizeof(char *),
     offset(mg_arg[0]), XtRString, (XtPointer) NULL},
    {"magnifierSize2", "MagnifierSize", XtRString, sizeof(char *),
     offset(mg_arg[1]), XtRString, (XtPointer) NULL},
    {"magnifierSize3", "MagnifierSize", XtRString, sizeof(char *),
     offset(mg_arg[2]), XtRString, (XtPointer) NULL},
    {"magnifierSize4", "MagnifierSize", XtRString, sizeof(char *),
     offset(mg_arg[3]), XtRString, (XtPointer) NULL},
    {"magnifierSize5", "MagnifierSize", XtRString, sizeof(char *),
     offset(mg_arg[4]), XtRString, (XtPointer) NULL},
#if COLOR
    {"color", "Color", XtRBoolean, sizeof(Boolean),
     offset(use_color), XtRString, "true"},
#endif /* COLOR */
    {"dvipsPath", "DvipsPath", XtRString, sizeof(char *),
     offset(dvips_path), XtRString, (XtPointer)DEFAULT_DVIPS_PATH},
    {"ps2pdfPath", "Ps2PdfPath", XtRString, sizeof(char *),
     offset(ps2pdf_path), XtRString, (XtPointer)DEFAULT_PS2PDF_PATH},
    {"dvipsHangTime", "DvipsHangTime", XtRInt, sizeof(int),
     offset(dvips_hang), XtRImmediate, (XtPointer) -1500},
    {"dvipsFailHangTime", "DvipsFailHangTime", XtRInt, sizeof(int),
     offset(dvips_fail_hang), XtRImmediate, (XtPointer) -5000},
    {"dvipsPrinterString", "DvipsPrinterString", XtRString, sizeof(char *),
     offset(dvips_printer_str), XtRString, (XtPointer)NULL},
    {"dvipsOptionsString", "DvipsOptionsString", XtRString, sizeof(char *),
     offset(dvips_options_str), XtRString, (XtPointer)NULL},
    {"defaultSavingFormat", "DefaultSavingFormat", XtRInt, sizeof(int),
     offset(default_saving_format), XtRImmediate, (XtPointer)0},
    {"defaultPrintingTarget", "DefaultPrintingTarget", XtRInt, sizeof(int),
     offset(default_printing_target), XtRImmediate, (XtPointer)1},
#ifdef GREY
    {"grey", "Grey", XtRBoolean, sizeof(Boolean),
     offset(use_grey), XtRString, "true"},
    {"install", "Install", XtRBool3, sizeof(Bool3),
     offset(install), XtRString, "maybe"},
#endif /* GREY */
    {"matchInverted", "MatchInverted", XtRBoolean, sizeof(Boolean),
     offset(match_highlight_inverted), XtRString, "true"},
    {"ruleColor", "RuleColor", XtRPixel, sizeof(Pixel),
     offset(rule_pixel), XtRPixel, (XtPointer) &resource.rule_pixel},
    {"ruleColor", "RuleColor", XtRString, sizeof(char *),
     offset(rule_color), XtRString, (XtPointer) NULL},
    /* linkStyle: style for presenting links:
       link_style is a bitsmask `xy' with x = color, y = underline, viz.:
       0: no highlighting at all
       1: underline with linkColor
       2: no underlining, text with linkColor
       3: underlining and text with linkColor
    */
    {"linkStyle", "LinkStyle", XtRInt, sizeof(int),
     offset(link_style), XtRImmediate, (XtPointer) 3},
    {"linkColor", "LinkColor", XtRString, sizeof(char *),
     offset(link_color), XtRString, (XtPointer)LINK_COLOR_FALLBACK},
    {"visitedLinkColor", "VisitedLinkColor", XtRString, sizeof(char *),
     offset(visited_link_color), XtRString, (XtPointer)VISITED_LINK_COLOR_FALLBACK},
    {"wwwBrowser", "WWWBrowser", XtRString, sizeof(char *),
     offset(browser), XtRString, (XtPointer) NULL},
#ifdef MOTIF
    {"prefsBrowserList", "PrefsBrowserList", XtRString, sizeof(char *),
     offset(prefs_browser_list), XtRString, (XtPointer)Xdvi_PREFS_BROWSER_DEFAULTS },
    {"prefsEditorList", "PrefsEditorList", XtRString, sizeof(char *),
     offset(prefs_editor_list), XtRString, (XtPointer)Xdvi_PREFS_EDITOR_DEFAULTS },
#  if USE_COMBOBOX
    {"searchHistory", "SearchHistory", XtRString, sizeof(char *),
     offset(search_history), XtRString, (XtPointer)NULL },
    {"searchHistorySize", "SearchHistorySize", XtRInt, sizeof(int),
     offset(search_history_size), XtRImmediate, (XtPointer)20},
#  endif
#endif
    /* defaults for unknown mime types */
    {"unknownMimeSuffix", "UnknownMimeSuffix", XtRString, sizeof(char *),
     offset(unknown_mime_suffix), XtRString, "application/x-unknown"},
    {"noMimeSuffix", "NoMimeSuffix", XtRString, sizeof(char *),
     offset(no_mime_suffix), XtRString, "application/x-unknown"},
    {"anchorPosition", "AnchorPosition", XtRString, sizeof(char *),
     offset(anchor_pos), XtRString, (XtPointer) NULL},
    /* bitmask for current search window settings; only used internally! */
    {"searchWindowDefaults", "SearchWindowDefaults", XtRInt, sizeof(int),
     offset(search_window_defaults), XtRImmediate, (XtPointer)0},
    /* whether to open file in new window from file selector (only for DVI files) */
    {"fileselOpenNewWindow", "FileselOpenNewWindow", XtRBoolean, sizeof(Boolean),
     offset(filesel_open_new_window), XtRString, "False"},
    /* resources for help text */
    {"helpGeneral", "HelpGeneral", XtRString, sizeof(char *),
     offset(help_general), XtRString, NULL},
    {"helpHypertex", "HelpHypertex", XtRString, sizeof(char *),
     offset(help_hypertex), XtRString, NULL},
    {"helpOthercommands", "HelpOthercommands", XtRString, sizeof(char *),
     offset(help_othercommands), XtRString, NULL},
    {"helpMarking", "HelpMarking", XtRString, sizeof(char *),
     offset(help_marking), XtRString, NULL},
    {"helpPagemotion", "HelpPagemotion", XtRString, sizeof(char *),
     offset(help_pagemotion), XtRString, NULL},
    {"helpMousebuttons", "HelpMousebuttons", XtRString, sizeof(char *),
     offset(help_mousebuttons), XtRString, NULL},
    {"helpModes", "HelpModes", XtRString, sizeof(char *),
     offset(help_modes), XtRString, NULL},
    {"helpSearch", "HelpSearch", XtRString, sizeof(char *),
     offset(help_search), XtRString, NULL},
    {"helpSourcespecials", "HelpSoucespecials", XtRString, sizeof(char *),
     offset(help_sourcespecials), XtRString, NULL},
#ifdef GREY
    {"pageHistorySize", "PageHistorySize", XtRInt, sizeof(int),
     offset(page_history_size), XtRImmediate, (XtPointer)1000},
};

static XtResource app_pixel_resources[] = {	/* get these later */
#endif /* GREY */
    {"foreground", "Foreground", XtRPixel, sizeof(Pixel),
     offset(fore_Pixel), XtRString, XtDefaultForeground},
    {"background", "Background", XtRPixel, sizeof(Pixel),
     offset(back_Pixel), XtRString, XtDefaultBackground},
    /*     {"borderColor", "BorderColor", XtRPixel, sizeof(Pixel), */
    /*      offset(brdr_Pixel), XtRPixel, (XtPointer) &resource.fore_Pixel}, */
    {"highlight", "Highlight", XtRPixel, sizeof(Pixel),
     offset(hl_Pixel), XtRPixel, (XtPointer) &resource.fore_Pixel},
    {"cursorColor", "CursorColor", XtRPixel, sizeof(Pixel),
     offset(cr_Pixel), XtRImmediate, (XtPointer)ULONG_MAX},
};
#undef offset


#ifndef MOTIF

# ifdef	NOQUERY
#  define drawWidgetClass widgetClass
# else

static XtGeometryResult
QueryGeometry(Widget w,
	      XtWidgetGeometry *constraints,
	      XtWidgetGeometry *reply)
{
    UNUSED(w);
    UNUSED(constraints);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = globals.page.w;
    reply->height = globals.page.h;

    return XtGeometryAlmost;
}

# include <X11/IntrinsicP.h>
# include <X11/CoreP.h>

/* if the following gives you trouble, just compile with -DNOQUERY */
static WidgetClassRec drawingWidgetClass = {
    {
	/* superclass		*/ &widgetClassRec,
	/* class_name		*/ "Draw",
	/* widget_size		*/ sizeof(WidgetRec),
	/* class_initialize	*/ NULL,
	/* class_part_initialize	*/ NULL,
	/* class_inited		*/ FALSE,
	/* initialize		*/ NULL,
	/* initialize_hook		*/ NULL,
	/* realize			*/ XtInheritRealize,
	/* actions			*/ NULL,
	/* num_actions		*/ 0,
	/* resources		*/ NULL,
	/* num_resources		*/ 0,
	/* xrm_class		*/ NULLQUARK,
	/* compress_motion		*/ FALSE,
	/* compress_exposure	*/ TRUE,
	/* compress_enterleave	*/ FALSE,
	/* visible_interest	*/ FALSE,
	/* destroy			*/ NULL,
	/* resize			*/ XtInheritResize,
	/* expose			*/ XtInheritExpose,
	/* set_values		*/ NULL,
	/* set_values_hook		*/ NULL,
	/* set_values_almost	*/ XtInheritSetValuesAlmost,
	/* get_values_hook		*/ NULL,
	/* accept_focus		*/ XtInheritAcceptFocus,
	/* version			*/ XtVersion,
	/* callback_offsets	*/ NULL,
	/* tm_table		*/ XtInheritTranslations,
	/* query_geometry		*/ QueryGeometry,
	/* display_accelerator	*/ XtInheritDisplayAccelerator,
	/* extension		*/ NULL
    }
};

#  define drawWidgetClass &drawingWidgetClass

# endif /* NOQUERY */
#endif /* not MOTIF */


int
atopix(const char *arg, Boolean allow_minus)
{
    int len = strlen(arg);
    const char *arg_end = arg;
    char tmp[11];
    double factor;

    if (allow_minus && *arg_end == '-')
	++arg_end;
    while ((*arg_end >= '0' && *arg_end <= '9') || *arg_end == '.')
	if (arg_end >= arg + XtNumber(tmp) - 1)
	    return 0;
	else
	    ++arg_end;
    memcpy(tmp, arg, arg_end - arg);
    tmp[arg_end - arg] = '\0';

#if A4
    factor = 1.0 / 2.54;	/* cm */
#else
    factor = 1.0;	/* inches */
#endif
    if (len > 2)
	switch (arg[len - 2] << 8 | arg[len - 1]) {
#if A4
	case 'i' << 8 | 'n':
	    factor = 1.0;
	    break;
#else
	case 'c' << 8 | 'm':
	    factor = 1.0 / 2.54;
	    break;
#endif
	case 'm' << 8 | 'm':
	    factor = 1.0 / 25.4;
	    break;
	case 'p' << 8 | 't':
	    factor = 1.0 / 72.27;
	    break;
	case 'p' << 8 | 'c':
	    factor = 12.0 / 72.27;
	    break;
	case 'b' << 8 | 'p':
	    factor = 1.0 / 72.0;
	    break;
	case 'd' << 8 | 'd':
	    factor = 1238.0 / 1157.0 / 72.27;
	    break;
	case 'c' << 8 | 'c':
	    factor = 12 * 1238.0 / 1157.0 / 72.27;
	    break;
	case 's' << 8 | 'p':
	    factor = 1.0 / 72.27 / 65536;
	    break;
	}

    return factor * atof(tmp) * resource.pixels_per_inch + 0.5;
}

#if CHECK_APP_FILEVERSION
static void
check_app_defaults_fileversion(void)
{
    /* update this when new essential resources are introduced */
    static const int required_version = 20030303;
    
    if (resource.app_defaults_fileversion < required_version) {
	const char *filename = kpse_find_file("XDvi", kpse_program_text_format, 0);
	if (filename == NULL)
	    filename = "XDvi";

	fprintf(stderr, "filename: %d\n", resource.app_defaults_fileversion);
	popup_message(globals.widgets.top_level,
		      MSG_WARN,
		      NULL,
		      "Your application defaults file `%s' is outdated. "
		      "This version of xdvi requires a file "
		      "with date >= %d. Your file is older, or doesn't have a date.\n"
		      "Please obtain a new version of the file `XDvi', e.g. from:\n"
		      "http://cvs.sourceforge.net/cgi-bin/viewcvs.cgi/xdvi/xdvik/texk/xdvik/texmf/\n"
		      "and copy it to a directory in your $XDVIINPUTS path.",
		      filename, required_version);
    }
}
#endif



#ifdef GREY
static Arg temp_args1[] = {
    {XtNdepth, (XtArgVal) 0},
    {XtNvisual, (XtArgVal) 0},
    {XtNcolormap, (XtArgVal) 0},
};

/*
 * Alternate routine to convert color name to Pixel (needed to substitute
 * "black" or "white" for BlackPixelOfScreen, etc., since a different visual
 * and colormap are in use).
 */

static Boolean
XdviCvtStringToPixel(Display *dpy,
		     XrmValuePtr args, Cardinal *num_args,
		     XrmValuePtr fromVal, XrmValuePtr toVal,
		     XtPointer *closure_ret)
{
    XrmValue replacement_val;
    Boolean default_is_fg;

    if ((strcmp((String) fromVal->addr, XtDefaultForeground) == 0
	 && (default_is_fg = True, True))
	|| (strcmp((String) fromVal->addr, XtDefaultBackground) == 0
	    && ((default_is_fg = False), True))) {
	replacement_val.size = sizeof(String);
	replacement_val.addr = (default_is_fg == resource.reverse)
	    ? "white" : "black";
	fromVal = &replacement_val;
    }

    return XtCvtStringToPixel(dpy, args, num_args, fromVal, toVal, closure_ret);
}

#endif


/*
 * Set the `sourceposition' propery of the window `w' to `source_str'.
 * This is usually done in `client' mode to notify the instance of
 * xdvi running in window `w' to start a forward search. After this,
 * the client usually exits normally.
 */
static void
set_sourceposition_property(const char *source_str, Window win)
{
    /* parse the special in order to expand the filename */
    struct src_parsed_special data;
    char *new_special = NULL;
    char *expanded_filename = NULL;
	    
    data.filename_len = 0;
    data.filename = NULL;
	    
    src_parse(source_str, strlen(source_str), &data);

    if (data.filename_len == 0) {
	/* can't give a GUI warning in `client' mode - just exit with error */
	XDVI_FATAL((stderr,
		    "Filename missing in -sourceposition argument (%s)!",
		    source_str));
    }
	    
    TRACE_CLIENT((stderr, "got data: line %d, col %d, file |%s|, len %lu\n",
		  data.line, data.col, data.filename, (unsigned long)data.filename_len));

    /* expand -sourceposition argument if it contains a path component.
       We don't use REALPATH here, because `tex -src' doesn't expand
       symlinks either. Instead, use canonicalize_path() to expand
       `../' and './' manually. */
    if (strchr(data.filename, '/') != NULL
	&& (expanded_filename = expand_filename(data.filename, USE_CWD_PATH)) != NULL) {
	char *tmp = canonicalize_path(expanded_filename);
	free(data.filename);
	free(expanded_filename);
	expanded_filename = tmp;		
    }
    else
	expanded_filename = data.filename;
	    
    TRACE_CLIENT((stderr, "expanded1: |%s|\n", expanded_filename));

    new_special = xmalloc(2 * LENGTH_OF_INT + 2 /* 2 for `:' and separating space */
			  + strlen(expanded_filename) + 1);
    sprintf(new_special, "%d:%d %s", data.line, data.col, expanded_filename);
    free(expanded_filename);				  
				  
    TRACE_CLIENT((stderr, "matched!"));
    set_string_property(new_special, atom_src_goto(), win);
    free(new_special);
    set_string_property("", atom_raise(), win);
}

static void
set_stringsearch_property(const char *str, Window win)
{
    set_string_property(str, atom_find_string(), win);
    set_string_property("", atom_raise(), win);
}



/* Translation of valid paper types to dimensions,
   which are used internally. The newline characters are a hack
   to format the list neatly for error messages.
   A* series measures are taken from
   http://www.cl.cam.ac.uk/~mgk25/iso-paper.html
*/
static const char *paper_types[] = {
    "us", "8.5x11in",
    "letter", "8.5x11in",	/* dvips compatibility */
    "ledger", "17x11in",	/* dvips compatibility */
    "tabloid", "11x17in",	/* dvips compatibility */
    "usr", "11x8.5in",
    "legal", "8.5x14in",
    "legalr", "14x8.5in",
    "foolscap", "13.5x17.0in",	/* ??? */
    "foolscapr", "17.0x13.5in",
    "", "0",

    /* ISO `A' formats, Portrait */
    "a0", "841x1189mm",
    "a1", "594x841mm",
    "a2", "420x594mm",
    "a3", "297x420mm",
    "a4", "210x297mm",
    "a5", "148x210mm",
    "a6", "105x148mm",
    "a7", "74x105mm",
    "a8", "52x74mm",
    "a9", "37x52mm",
    "a10","26x37mm",
    "", "0",

    /* ISO `A' formats, Landscape */
    "a0r", "1189x841mm",
    "a1r", "841x594mm",
    "a2r", "594x420mm",
    "a3r", "420x297mm",
    "a4r", "297x210mm",
    "a5r", "210x148mm",
    "a6r", "148x105mm",
    "a7r", "105x74mm",
    "a8r", "74x52mm",
    "a9r", "52x37mm",
    "a10r","37x26mm",
    "", "0",

    /* ISO `B' formats, Portrait */
    "b0", "1000x1414mm",
    "b1", "707x1000mm",
    "b2", "500x707mm",
    "b3", "353x500mm",
    "b4", "250x353mm",
    "b5", "176x250mm",
    "b6", "125x176mm",
    "b7", "88x125mm",
    "b8", "62x88mm",
    "b9", "44x62mm",
    "b10","31x44mm",
    "", "0",

    /* ISO `B' formats, Landscape */
    "b0r", "1414x1000mm",
    "b1r", "1000x707mm",
    "b2r", "707x500mm",
    "b3r", "500x353mm",
    "b4r", "353x250mm",
    "b5r", "250x176mm",
    "b6r", "176x125mm",
    "b7r", "125x88mm",
    "b8r", "88x62mm",
    "b9r", "62x44mm",
    "b10r","44x31mm",
    "", "0",

    /* ISO `C' formats, Portrait */
    "c0", "917x1297mm",
    "c1", "648x917mm",
    "c2", "458x648mm",
    "c3", "324x458mm",
    "c4", "229x324mm",
    "c5", "162x229mm",
    "c6", "114x162mm",
    "c7", "81x114mm",
    "c8", "57x81mm",
    "c9", "40x57mm",
    "c10","28x40mm",
    "", "0",

    /* ISO `C' formats, Landscape */
    "c0r", "1297x917mm",
    "c1r", "917x648mm",
    "c2r", "648x458mm",
    "c3r", "458x324mm",
    "c4r", "324x229mm",
    "c5r", "229x162mm",
    "c6r", "162x114mm",
    "c7r", "114x81mm",
    "c8r", "81x57mm",
    "c9r", "57x40mm",
    "c10r","40x28mm",
};

/* access methods for paper_types */
const char **get_paper_types(void) {
    return paper_types;
}

size_t get_paper_types_size(void) {
    return XtNumber(paper_types);
}

/* Set the icon name and title name standard properties on `globals.widgets.top_level'.
 * We use the basename of the DVI file (without the .dvi), so different xdvi
 * invocations can be distinguished, yet do not use up too much real estate.
 *
 * This function returns freshly allocated memory in *icon_name and *title_name
 * which the caller is responsible for free()ing again.
 */
void
get_icon_and_title(const char *filename, char **icon_name, char **title_name)
{
    /* Use basename of DVI file for name in icon and title.  */
    const char *ptr;
    char *ptr2;

    /* SU 2000/12/16: added page number information */
    const char *const title_name_fmt = "%s:  %s   (%d page%s)";

    MYTRACE((stderr, "get_icon_and_title called with: |%s|", filename));

    ptr = strrchr(filename, '/');
    if (ptr != NULL)
	++ptr;
    else {
	ptr = filename;
    }
        
    /*
     * Remove the `file:' prefix from the icon name; since some windowmanagers
     * only display a prefix in window lists etc., it's more significant this
     * way.
     */

    if (memcmp(ptr, "file:", 5) == 0) {
	ptr += 5;
    }
    *icon_name = xstrdup(ptr);
    
    MYTRACE((stderr, "before chopping: icon_name: |%s|", *icon_name));
    if ((ptr2 = strstr(*icon_name, ".dvi")) != NULL) {
	/* chop off .dvi extension */
	*ptr2 = '\0';
    }
    MYTRACE((stderr, "after chopping: icon_name: |%s|", *icon_name));

    *title_name = xmalloc(strlen(title_name_fmt)
			  + strlen(XDVIK_PROGNAME)
			  + strlen(*icon_name)
			  + LENGTH_OF_INT
			  + 2);	/* 2 for additional plural `s' */
    MYTRACE((stderr, "total_pages: %d", total_pages));
    sprintf(*title_name, title_name_fmt, XDVIK_PROGNAME, *icon_name, total_pages,
	    (total_pages > 1) ? "s" : "");

    MYTRACE((stderr, "title_name, icon_name: |%s|%s|", *title_name, *icon_name));
}

void
set_icon_and_title(const char *icon_name, const char *title_name)
{
    if (!XtIsRealized(globals.widgets.top_level)) {
	MYTRACE((stderr, "set_icon_and_title: returning"));
	return;
    }
    XtVaSetValues(globals.widgets.top_level, XtNtitle, (XtArgVal) title_name, XtNiconName, (XtArgVal) icon_name, NULL);
    XSetStandardProperties(DISP, XtWindow(globals.widgets.top_level), title_name, icon_name,
			   (Pixmap) 0, NULL, 0, NULL);
}

static void
get_window_constraints(XtWidgetGeometry *reply,
		       Dimension screen_w, Dimension screen_h,
		       int *add_h)
{
    XtWidgetGeometry constraints;

    constraints.request_mode = reply->request_mode = 0;

    /* fprintf(stderr, "setting constraints.width to %d\n", globals.page.w); */
    constraints.width = globals.page.w;
    if (globals.page.w > screen_w) {
	constraints.request_mode |= CWWidth;
	constraints.width = screen_w;
    }

/*     fprintf(stderr, "setting constraints.height to %d; screen_h = %d\n", globals.page.h, screen_h); */
    constraints.height = globals.page.h;
    if (constraints.height > screen_h) {
	constraints.request_mode |= CWHeight;
	constraints.height = screen_h;
    }

    if (constraints.request_mode != 0
	&& constraints.request_mode != (CWWidth | CWHeight)) {
#ifdef MOTIF
	(void)XtQueryGeometry(globals.widgets.main_window, &constraints, reply);
#else
	(void)XtQueryGeometry(globals.widgets.vport_widget, &constraints, reply);
#endif
    }
/*     fprintf(stderr, "reply height: %d; screen_h: %d; add_h: %d\n", reply->height, screen_h, *add_h); */
    if (!(reply->request_mode & CWWidth))
	reply->width = constraints.width;
    if (reply->width >= screen_w)
	reply->width = screen_w;
    if (!(reply->request_mode & CWHeight)) {
	reply->height = constraints.height - 2 * bwidth;
    }

    if (reply->height + *add_h >= screen_h) {
	reply->height = screen_h
#ifdef MOTIF
	    - 1.5
#else
	    - 2
#endif
	    * *add_h - 2 * bwidth;
    }
}

void
set_windowsize(Dimension *ret_w, Dimension *ret_h, int add_w, int add_h, Boolean override)
{
    static Arg set_wh_args[] = {
	{XtNwidth, (XtArgVal) 0},
	{XtNheight, (XtArgVal) 0},
    };
    Dimension screen_w, screen_h;

    const char *test_geometry = resource.geometry;
    
    if (resource.fullscreen) {
	Dimension w = WidthOfScreen(SCRN), h = HeightOfScreen(SCRN);

	if (currwin.shrinkfactor == 0) { /* if not set by user */
	    Dimension height_factor = ROUNDUP(globals.page.unshrunk_h, h);
	    currwin.shrinkfactor = ROUNDUP(globals.page.unshrunk_w, w);
	    if (height_factor >= currwin.shrinkfactor)
		currwin.shrinkfactor = height_factor;
	    /*  	    fprintf(stderr, "factor was 0, using %d\n", currwin.shrinkfactor); */
	}
	/*  	else */
	/*  	    fprintf(stderr, "factor != 0, using %d\n", currwin.shrinkfactor); */

	mane.shrinkfactor = currwin.shrinkfactor;
	init_page();
	set_wh_args[0].value = (XtArgVal)w;
	set_wh_args[1].value = (XtArgVal)h;
	*ret_w = w;
	*ret_h = h;
#ifdef MOTIF
#if USE_XAW_PANNER
	XtVaGetValues(globals.widgets.main_window, XmNshadowThickness, &g_save_shadow_thickness, NULL);
#endif
	XtVaSetValues(globals.widgets.main_window, XmNshadowThickness, 0, NULL);
#endif
	XtSetValues(globals.widgets.top_level, set_wh_args, XtNumber(set_wh_args));
    }
    else if (override) {
	set_wh_args[0].value = (XtArgVal)*ret_w;
	set_wh_args[1].value = (XtArgVal)*ret_h;
	XtSetValues(globals.widgets.top_level, set_wh_args, XtNumber(set_wh_args));
    }
    else { /* determine a window size that fits the current shrink factor */
	Arg temp_args3 = { XtNborderWidth, (XtArgVal)&bwidth };
	XtWidgetGeometry reply;
	
	XtGetValues(globals.widgets.top_level, &temp_args3, 1);	/* get border width */
	screen_w = WidthOfScreen(SCRN) - 2 * bwidth;

	screen_w -= add_w;
	/* screen_h = HeightOfScreen(SCRN) - 2 * bwidth - get_statusline_height() - 6; */
	screen_h = HeightOfScreen(SCRN) - 2 * bwidth;
	for (;;) {	/* actually, at most two passes */
	    Dimension height_factor;

	    TRACE_GUI((stderr, "geometry: |%s|; remember: %d, windowsize: %s",
		       resource.geometry ? resource.geometry : "<NULL>",
		       resource.remember_windowsize,
		       resource.windowsize ? resource.windowsize : "<NULL>"));
	    
	    if (resource.geometry == NULL && !resource.remember_windowsize) {
		/* geometry not set by user, try to find geometry that fits the shrink factor */
		get_window_constraints(&reply, screen_w, screen_h, &add_h);
		TRACE_GUI((stderr, "w: %d, h: %d, add_h: %d, reply: %d x %d",
			   screen_w, screen_h, add_h, reply.width, reply.height));
	    }
	    else {
		int x, y;
		unsigned int width, height;
		int flags;

		if (resource.remember_windowsize && resource.windowsize != NULL)
		    test_geometry = resource.windowsize;
		else
		    test_geometry = resource.geometry;
		flags = XParseGeometry(test_geometry, &x, &y, &width, &height);

		if (!(flags & WidthValue) || !(flags & HeightValue)) {
		    /* no geometry specified, use fallback */
		    get_window_constraints(&reply, screen_w, screen_h, &add_h);
		}

		/* warn about bad values */
		if (flags & WidthValue) {
		    if (width > (unsigned int)(2 * bwidth + add_w)) {
			TRACE_GUI((stderr, "width: %hu, bwidth: %hu, add_w: %d",
				   width, bwidth, add_w));
			reply.width = width - 2 * bwidth - add_w;
		    }
		    else {
			reply.width = width;
		    }
		}
		if (flags & HeightValue) {
		    if (height > (unsigned int)(2 * bwidth + add_h)) {
			TRACE_GUI((stderr, "height: %hu, bwidth: %hu, add_h: %d",
				   height, bwidth, add_h));
			reply.height = height - 2 * bwidth - add_h;
		    }
		    else {
			reply.height = height;
		    }
		}
		TRACE_GUI((stderr, "setting geometry: %dx%d", (int)reply.width, (int)reply.height));
	    }
	    
	    /* now reply.{width,height} contain max. usable window size */

	    /* User didn't use `-s 0', use either default or other user-specified value */
	    if (currwin.shrinkfactor != 0) {
		/*  		fprintf(stderr, "factor != 0, using %d\n", currwin.shrinkfactor); */
		break;
	    }
	    /*  	    else { */
	    /*  		fprintf(stderr, "factor was 0, using %d\n", currwin.shrinkfactor); */
	    /*  	    } */

	    /* else, try to find a suitable shrink factor: */
	    currwin.shrinkfactor = ROUNDUP(globals.page.unshrunk_w, reply.width - 2);
	    /*  	    fprintf(stderr, "factor w: %d\n", currwin.shrinkfactor); */
	    
	    height_factor = ROUNDUP(globals.page.unshrunk_h, reply.height - 2);
	    /*  	    fprintf(stderr, "factor h: %d\n", height_factor); */
	    if (height_factor >= currwin.shrinkfactor)
		currwin.shrinkfactor = height_factor;

	    /*  	    fprintf(stderr, "factor now is: %d\n", currwin.shrinkfactor); */
	    
	    mane.shrinkfactor = currwin.shrinkfactor;
	    init_page();
	    set_wh_args[0].value = (XtArgVal)globals.page.w;
	    set_wh_args[1].value = (XtArgVal)globals.page.h;
	    *ret_w = globals.page.w;
	    *ret_h = globals.page.h;
	    XtSetValues(globals.widgets.draw_widget, set_wh_args, XtNumber(set_wh_args));
	}
#ifdef MOTIF
	/*
	  SU 2002/11/23: Added scrollbar width to height computation.
	  Otherwise, when the vertical space isn't sufficient, a
	  vertical scrollbar will be added, but since this makes the
	  display narrower, a horizontal scrollbar will be added as
	  well, even if this wouldn't be neccessary.

	  SU 2003/09/30: Apparently the size computation works now, even
	  though I'm not sure why (scrollbar value isn't in add_w). Investigate.
	*/
	/* HACK ALERT: 4 for window decoration borders - FIXME: get actual values?? */
	set_wh_args[0].value = reply.width + add_w + (test_geometry == NULL ? 4 : 0);
	set_wh_args[1].value = reply.height + add_h + (test_geometry == NULL ? 4 : 0);
	XtSetValues(globals.widgets.top_level, set_wh_args, XtNumber(set_wh_args));

#else /* MOTIF */

	set_wh_args[0].value = reply.width + add_w + 2 * bwidth; /*  + (test_geometry == NULL ? 15 : (2 * bwidth)); */
	/*
	  FIXME: use real height of statusline here
	  Somehow I didn't manage to use XtVaCreateWidget in a call to
	  create_statusline() above, and XtManageChild() below.
	  In that case, we could do without get_statusline_height().
	*/
	set_wh_args[1].value = reply.height + (test_geometry == NULL ? (2 * bwidth + add_h) : 0);
	XtSetValues(globals.widgets.top_level, set_wh_args, XtNumber(set_wh_args));
	set_wh_args[0].value -= add_w;
	XtSetValues(globals.widgets.vport_widget, set_wh_args, XtNumber(set_wh_args));

#endif /* MOTIF */
	*ret_w = set_wh_args[0].value;
	*ret_h = set_wh_args[1].value;
	TRACE_GUI((stderr, "returning: w=%d, h=%d", *ret_w, *ret_h));
    }
}

static void
net_wm_toggle_fullscreen(int flag)
{
    Atom NET_WM_FULLSCREEN = XInternAtom(DISP, "_NET_WM_STATE_FULLSCREEN", True);

    if (NET_WM_FULLSCREEN) {
	XEvent ev;
	Atom NET_WM_STATE = XInternAtom(DISP, "_NET_WM_STATE", False);

	/* 	XDVI_INFO((stdout, "trying _NET_WM_STATE_FULLSCREEN ...")); */

	ev.type = ClientMessage;
	ev.xclient.serial = 0;
	ev.xclient.send_event = True;
	ev.xclient.display = DISP;
	ev.xclient.window = XtWindow(globals.widgets.top_level);
	ev.xclient.message_type = NET_WM_STATE;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = flag; /* _NET_WM_STATE_REMOVE (0) or _NET_WM_STATE_ADD (1) */
	ev.xclient.data.l[1] = NET_WM_FULLSCREEN;
	ev.xclient.data.l[2] = 0L;
	
	XSendEvent(DISP, DefaultRootWindow(DISP), False,
		   SubstructureNotifyMask, &ev);
    }
    else {
	XDVI_INFO((stdout, "_NET_WM_STATE_FULLSCREEN not supported by this window manager."));
    }
}

void
reconfigure_window(Boolean fullscreen, Dimension width, Dimension height,
		   Boolean save_position)
{
    static int x_old = 5, y_old = 15;
#ifdef SIZECONFIGURE_WORKS
    int sizeconfiguremask;
    XWindowChanges sizeconfigure;
#endif /* SIZECONFIGURE_WORKS */
#ifdef MOTIF
    static int save_wm_decorations;
#else
    static struct PropMotifWmHints MWMHints = {MWM_HINTS_DECORATIONS, 0, 0, 0, 0};
    Atom WM_HINTS = XInternAtom(DISP, "_MOTIF_WM_HINTS", True);
#endif
    int x, y;

#ifdef SIZECONFIGURE_WORKS
    sizeconfiguremask = CWX | CWY | CWWidth | CWHeight | CWBorderWidth;
    sizeconfigure.width = width;
    sizeconfigure.height = height;
#endif /* SIZECONFIGURE_WORKS */
    
    if (fullscreen) {
#if 0
	static Boolean first_time = True;
	/* offsets between the raw window and the decorated window */
	static int wm_x_offset = 0;
	static int wm_y_offset = 0;
#endif
	Window dummy;

#ifdef SIZECONFIGURE_WORKS
	sizeconfigure.x = 0;
	sizeconfigure.y = 0;
	sizeconfigure.border_width = 0;
#endif /* SIZECONFIGURE_WORKS */
	
	/* Note ZLB: avoid to call XTranslateCoordinates if the window is
	   not yet mapped since XTranslateCoordinates implicitly maps the
	   window (note: calling the function XGetWindowAttributes also
	   makes the window mapped).

	   This effectively eliminates the flashing effect when xdvik is
	   started in fullscreen mode for the Motif version. For the Xaw
	   version we still get flashing effect (seems that the window is
	   already mapped). */

        if (save_position) {
	    /* save current window coordinates so that we can change them back */
	    (void)XTranslateCoordinates(DISP, XtWindow(globals.widgets.top_level),
					RootWindowOfScreen(SCRN),
					0, 0,
					&x_old, &y_old,
					&dummy);
#if 0
            if (first_time) {
		first_time = False;
		/* This is a hack for finding out wm_x_offset, wm_y_offset */
		XMoveWindow(DISP, XtWindow(globals.widgets.top_level), 0, 0);
		XSync(DISP, False);
		(void)XTranslateCoordinates(DISP, XtWindow(globals.widgets.top_level),
					    RootWindowOfScreen(SCRN),
					    0, 0, &wm_x_offset, &wm_y_offset,
					    &dummy);
		fprintf(stderr, "wm offset = (%d,%d)\n",
			wm_x_offset, wm_y_offset);
	    }
	    x_old -= wm_x_offset;
	    y_old -= wm_y_offset;
#endif
        }

	x = y = 0;

#ifdef MOTIF
	XtVaGetValues(globals.widgets.top_level, XmNmwmDecorations, &save_wm_decorations, NULL);
	/* FIXME: this doesn't work e.g. with KDE */
	XtVaSetValues(globals.widgets.top_level,
		      XmNmwmDecorations, 0,
		      XmNgeometry, "+0+0",
		      NULL);
	XtVaSetValues(globals.widgets.main_window, XmNshadowThickness, 0, NULL);
#else
	MWMHints.decorations = 0;
	if (WM_HINTS != None) {
	    XChangeProperty(DISP, XtWindow(globals.widgets.top_level),
			    WM_HINTS, WM_HINTS, 32,
			    PropModeReplace, (unsigned char *)&MWMHints,
			    sizeof(MWMHints) / 4);
	}
#endif
	/* SU: apparently some new standard that is supposed to work
	   with some WMs ... but it doesn't with my versions of Gnome and KDE. */
	net_wm_toggle_fullscreen(1);

#ifdef SIZECONFIGURE_WORKS
	XConfigureWindow(DISP, XtWindow(globals.widgets.top_level), sizeconfiguremask, &sizeconfigure);
#endif /* SIZECONFIGURE_WORKS */
    }
    else {
#ifdef SIZECONFIGURE_WORKS
	sizeconfiguremask = CWWidth | CWHeight | CWX | CWY;
	sizeconfigure.x = x_old;
	sizeconfigure.y = y_old;
	sizeconfigure.border_width = 20;
#endif /* SIZECONFIGURE_WORKS */
	XtVaSetValues(globals.widgets.top_level, XtNx, x_old, XtNy, y_old, NULL);
#ifdef MOTIF
	XtVaSetValues(globals.widgets.top_level, XmNmwmDecorations, save_wm_decorations, NULL);
#if USE_XAW_PANNER
	XtVaSetValues(globals.widgets.main_window, XmNshadowThickness, g_save_shadow_thickness, NULL);
#endif
#else
	MWMHints.decorations = MWM_DECOR_ALL;
	if (WM_HINTS != None) {
	    XChangeProperty(DISP, XtWindow(globals.widgets.top_level),
			    WM_HINTS, WM_HINTS, 32,
			    PropModeReplace, (unsigned char *)&MWMHints,
			    sizeof(MWMHints) / 4);
	}
#endif
	net_wm_toggle_fullscreen(0);

	x = x_old;
	y = y_old;
    }

#if 1
#if 0
    XUnmapWindow(DISP, XtWindow(globals.widgets.top_level));
#else
    XWithdrawWindow(DISP, XtWindow(globals.widgets.top_level), XScreenNumberOfScreen(SCRN));
#endif

    /* Note ZLB: Placing XResizeWindow before XUnmapWindow or after XMapWindow
       makes the fullscreen window size smaller than the screen size when
       using `mwm' of Lesstif */
    XSetWindowBorderWidth(DISP, XtWindow(globals.widgets.top_level), 0);
    XResizeWindow(DISP, XtWindow(globals.widgets.top_level), width, height);

    XMapRaised(DISP, XtWindow(globals.widgets.top_level));

    /* Note ZLB: XMapWindow might change the window position with some WMs
       (like Sawfish), so we place the window position after it's mapped. */
    XMoveWindow(DISP, XtWindow(globals.widgets.top_level), x, y);
#endif /* 0 */

    /* need to redraw the page to avoid artifacts */
    globals.ev.flags |= EV_NEWPAGE;
    XFlush(DISP);
}


/*
 * Parse colors from resource.{visited_}link_color, saving them
 * to g_{visited_}link_color and {visited_}link_pix.
 */
static void
get_link_colors(Pixel *link_pix, Pixel *visited_link_pix)
{
    XrmValue from1, from2, to1, to2;
	
    XColor exact, approx;
    double r, g, b;
    double factor = 65535.0;
    int ret;

    /* get rgb values from color for links */
    if ((ret = XLookupColor(DISP, G_colormap,
			    resource.link_color,
			    &exact, &approx)) != 0) {
    }
    else {
	XDVI_WARNING((stderr, "XLookupColor failed for resource.link_color \"%s\"\n"
		      "- using fallback color \"%s\".",
		      resource.visited_link_color, LINK_COLOR_FALLBACK));
	XLookupColor(DISP, G_colormap, LINK_COLOR_FALLBACK, &exact, &approx);
    }
	
#if 0
    fprintf(stderr, "lookup color for %s returned: %d, %d, %d\n",
	    resource.link_color, exact.red, exact.green, exact.blue);
#endif
    r = exact.red / factor;
    g = exact.green / factor;
    b = exact.blue / factor;
    g_link_color_rgb = xmalloc(strlen("push rgb 0.00 0.00 0.00") + 1);
    sprintf(g_link_color_rgb, "push rgb %.2f %.2f %.2f", r, g, b);
	
    /* same for visited links */
    if ((ret = XLookupColor(DISP, G_colormap,
			    resource.visited_link_color,
			    &exact, &approx)) != 0) {
    }
    else {
	XDVI_WARNING((stderr, "XLookupColor failed for resource.visited_link_color \"%s\"\n"
		      "- using fallback color \"%s\".",
		      resource.visited_link_color, VISITED_LINK_COLOR_FALLBACK));
	XLookupColor(DISP, G_colormap, VISITED_LINK_COLOR_FALLBACK, &exact, &approx);
    }
	
#if 0
    fprintf(stderr, "lookup color for %s returned: %d, %d, %d\n",
	    resource.visited_link_color, exact.red, exact.green, exact.blue);
#endif
    r = exact.red / factor;
    g = exact.green / factor;
    b = exact.blue / factor;
    g_visited_link_color_rgb = xmalloc(strlen("push rgb 0.00 0.00 0.00") + 1);
    sprintf(g_visited_link_color_rgb, "push rgb %.2f %.2f %.2f", r, g, b);

    /* 2nd part: Create CG for the underlines. */
    from1.addr = resource.link_color;
    from1.size = strlen(from1.addr) + 1;
    to1.addr = (XtPointer)link_pix;
    to1.size = sizeof(Pixel);
    if (!XtConvertAndStore(globals.widgets.top_level, XtRString, &from1, XtRPixel, &to1)) {
	XDVI_WARNING((stderr, "String to pixel conversion failed for resource.link_color \"%s\"\n"
		      "- using fallback color \"%s\".",
		      resource.link_color, LINK_COLOR_FALLBACK));
	from1.addr = LINK_COLOR_FALLBACK;
	from1.size = strlen(from1.addr) + 1;
	to1.addr = (XtPointer)link_pix;
	to1.size = sizeof(Pixel);
	XtConvertAndStore(globals.widgets.top_level, XtRString, &from1, XtRPixel, &to1);
    }
    from2.addr = resource.visited_link_color;
    from2.size = strlen(from2.addr) + 1;
    to2.addr = (XtPointer)visited_link_pix;
    to2.size = sizeof(Pixel);
    if (!XtConvertAndStore(globals.widgets.top_level, XtRString, &from2, XtRPixel, &to2)) {
	XDVI_WARNING((stderr, "String to pixel conversion failed for resource.visited_link_color \"%s\"\n"
		      "- using fallback color \"%s\".",
		      resource.visited_link_color, VISITED_LINK_COLOR_FALLBACK));
	from2.addr = VISITED_LINK_COLOR_FALLBACK;
	from2.size = strlen(from2.addr) + 1;
	to2.addr = (XtPointer)visited_link_pix;
	to2.size = sizeof(Pixel);
	XtConvertAndStore(globals.widgets.top_level, XtRString, &from2, XtRPixel, &to2);
    }
}

/*  Widget globals.widgets.main_row, globals.widgets.menu_bar; */


#ifdef MOTIF
/* make sure the translations for the drawing area are properly set
   (Motif 2.x seems to somehow overwrite them somewhere in the
   initialization phase; bug #610206).
*/
void
motif_translations_hack(void)
{
    static XtTranslations xlats = NULL;
    const char *const translations = \
	"<Key>osfPageUp:back-page()\n"
	"<Key>osfPageDown:forward-page()\n"
	"Ctrl<Key>osfBeginLine:goto-page(1)\n"
	"Ctrl<Key>osfEndLine:goto-page()\n"
	"<Key>osfBeginLine:home-or-top()\n"
	"<Key>osfEndLine:end-or-bottom()\n"
#if 0
	/* AFAIK the following don't have any effect with Motif */
#ifdef XK_KP_Left
	"<Key>KP_Home:home()\n"
	"<Key>KP_End:down()\n"
	"<Key>KP_Prior:back-page()\n"
	"<Key>KP_Next:forward-page()\n"
#endif
#endif
	;

    if (xlats == NULL) {
	xlats = XtParseTranslationTable(translations);
    }
	    
    ASSERT(globals.widgets.clip_widget != NULL, "globals.widgets.clip_widget must have been initialized");
    ASSERT(globals.widgets.draw_widget != NULL, "globals.widgets.draw_widget must have been initialized");
    XtOverrideTranslations(globals.widgets.clip_widget, xlats);
}
#endif /* MOTIF */

/* return an empty cursor. Lifted from unclutter.c */
static Cursor
h_get_empty_cursor(Display *display, Window root)
{
    Pixmap cursormask;
    XGCValues xgc;
    GC gc;
    XColor dummycolour;
    Cursor cursor;

    cursormask = XCreatePixmap(display, root, 1, 1, 1/*depth*/);
    xgc.function = GXclear;
    gc =  XCreateGC(display, cursormask, GCFunction, &xgc);
    XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
    dummycolour.pixel = 0;
    dummycolour.red = 0;
    dummycolour.flags = 04;
    cursor = XCreatePixmapCursor(display,
				 cursormask, cursormask,
				 &dummycolour, &dummycolour,
				 0, 0);
    XFreePixmap(display, cursormask);
    XFreeGC(display, gc);
    return cursor;
}



static void
create_cursors(void)
{
    XColor bg_Color;
    Pixmap temp;
    Pixmap mask;

    /* first try colors by name, as fix for #804294; fall back on
       WhitePixelOfScreen() etc. if this fails:
    */
    if (resource.cr_Pixel != (Pixel)ULONG_MAX) {
	m_cursor_color.pixel = resource.cr_Pixel;
	XQueryColor(DISP, G_colormap, &m_cursor_color);
    }
    else {
	if (XParseColor(DISP, G_colormap, "black", &m_cursor_color) == 0) { /* lookup failure */
	    m_cursor_color.pixel = BlackPixelOfScreen(SCRN);
	    XQueryColor(DISP, G_colormap, &m_cursor_color);
	}
    }
    if (XParseColor(DISP, G_colormap, "white", &bg_Color) == 0) { /* lookup failure */
	bg_Color.pixel = WhitePixelOfScreen(SCRN);
	XQueryColor(DISP, G_colormap, &bg_Color);
    }

    /* wait cursor */
    globals.cursor.wait = XCreateFontCursor(DISP, XC_watch);

    if (resource.mouse_mode1_cursor == -1) { /* Use default ready cursor (custom bitmap) */
	temp = XCreateBitmapFromData(DISP, RootWindowOfScreen(SCRN),
				     (char *)magglass_bits,
				     magglass_width, magglass_height);
	mask = XCreateBitmapFromData(DISP, RootWindowOfScreen(SCRN),
				     (char *)magglass_mask_bits,
				     magglass_mask_width, magglass_mask_height);
	
	globals.cursor.mode1 = XCreatePixmapCursor(DISP, temp, mask,
						   &m_cursor_color, &bg_Color,
						   magglass_x_hot, magglass_y_hot);
	XFreePixmap(DISP, temp);
	XFreePixmap(DISP, mask);
    }
    else {
	globals.cursor.mode1 = XCreateFontCursor(DISP, resource.mouse_mode1_cursor);
    }

    globals.cursor.corrupted = XCreateFontCursor(DISP, XC_watch);

    /* empty cursor */
    globals.cursor.empty = h_get_empty_cursor(DISP, RootWindowOfScreen(SCRN));
    
#if !COLOR
    XRecolorCursor(DISP, globals.cursor.ready, &m_cursor_color, &bg_Color);
    XRecolorCursor(DISP, globals.cursor.wait, &m_cursor_color, &bg_Color);
#endif
    /* Cursor when page is paused */
#ifdef VMS
    temp = XCreateBitmapFromData(DISP, RootWindowOfScreen(SCRN),
				 (char *)hand_bits, hand_width, hand_height);
    mask = XCreateBitmapFromData(DISP, RootWindowOfScreen(SCRN),
				 (char *)hand_mask_bits, hand_mask_width, hand_mask_height);
    globals.cursor.pause = XCreatePixmapCursor(DISP, temp, mask,
					       &m_cursor_color, &bg_Color, 6, 6);
    XFreePixmap(DISP, temp);
    XFreePixmap(DISP, mask);
#else
    globals.cursor.pause = XCreateFontCursor(DISP, XC_watch);
#endif

    /* cursors indicating dragging direction */
    globals.cursor.drag_v = XCreateFontCursor(DISP, XC_sb_v_double_arrow);

    globals.cursor.drag_h = XCreateFontCursor(DISP, XC_sb_h_double_arrow);

    globals.cursor.drag_a = XCreateFontCursor(DISP, XC_fleur);

#ifdef VMS
    globals.cursor.link = globals.cursor.ready;
    globals.cursor.mode2 = globals.cursor.ready;
    globals.cursor.mode3 = globals.cursor.ready;
#else
    globals.cursor.link = XCreateFontCursor(DISP, XC_hand2);
    globals.cursor.mode2 = XCreateFontCursor(DISP, resource.mouse_mode2_cursor);
    globals.cursor.mode3 = XCreateFontCursor(DISP, resource.mouse_mode3_cursor);
    /*  globals.cursor.text = XCreateFontCursor(DISP, XC_tcross); */
#endif
}

/* Initialize a forward search if the source_position argument is not NULL. */
static void
do_forward_search(const char *source_position)
{
    /* parse the special to expand the filename */
    struct src_parsed_special data;
    char *new_special = NULL;
    char *expanded_filename = NULL;
	    
    if (source_position == NULL) /* nothing to do */
	return;
    
    data.filename_len = 0;
    data.filename = NULL;

    src_parse(source_position, strlen(source_position), &data);
	    
    if (data.filename_len == 0) { /* malformed argument: tell user, and ignore it */
	popup_message(globals.widgets.top_level,
		      MSG_ERR,
		      /* helptext */
		      "The format of the -sourceposition argument should be:\n"
		      "-sourceposition '<nn>[ ]*<filename'\n"
		      "with <nn> = linenumber, [ ]* = an arbitrary number of spaces, "
		      "and <filename> = TeX source file.",
		      /* error message */
		      "Filename missing in `-sourceposition' argument \"%s\". "
		      "Could not perform forward search.",
		      source_position);
    }
    else {
	TRACE_CLIENT((stderr, "got data: line %d, col %d, file |%s|, len %lu\n",
		      data.line, data.col, data.filename, (unsigned long)data.filename_len));
		
	/* expand -sourceposition argument if it contains a path component, like above */
	if (strchr(data.filename, '/') != NULL
	    && (expanded_filename = expand_filename(data.filename, USE_CWD_PATH)) != NULL) {
	    char *tmp = canonicalize_path(expanded_filename);
	    free(data.filename);
	    free(expanded_filename);
	    expanded_filename = tmp;		
	}
	else
	    expanded_filename = data.filename;
		
	TRACE_CLIENT((stderr, "expanded source_position: |%s|\n", expanded_filename));
		
	new_special = xmalloc(2 * LENGTH_OF_INT + 2 /* 2 for `:' and separating space */
			      + strlen(expanded_filename) + 1);
	sprintf(new_special, "%d:%d %s", data.line, data.col, expanded_filename);
	free(expanded_filename);
	globals.src.fwd_string = new_special;
	globals.ev.flags |= EV_SRC;
    }
}


/*
 *	Routines for compile_mouse_actions
 */

struct modifierinf {
    int		len;
    const char	*name;
    Modifiers	mask;
    KeySym	keysym;
};

/* Allowed modifiers, sorted by length and then lexicographically.  */

static	struct modifierinf	modifiers[] = {
    {1,		"a",		0,		XK_Alt_L},
    {1,		"c",		ControlMask,	0},
    {1,		"h",		0,		XK_Hyper_L},
    {1,		"l",		LockMask,	0},
    {1,		"m",		0,		XK_Meta_L},
    {1,		"s",		ShiftMask,	0},
    {2,		"su",		0,		XK_Super_L},
    {3,		"Alt",		0,		XK_Alt_L},
    {4,		"Ctrl",		ControlMask,	0},
    {4,		"Lock",		LockMask,	0},
    {4,		"Meta",		0,		XK_Meta_L},
    {4,		"Mod1",		Mod1Mask,	0},
    {4,		"Mod2",		Mod2Mask,	0},
    {4,		"Mod3",		Mod3Mask,	0},
    {4,		"Mod4",		Mod4Mask,	0},
    {4,		"Mod5",		Mod5Mask,	0},
    {5,		"Hyper",	0,		XK_Hyper_L},
    {5,		"Shift",	ShiftMask,	0},
    {5,		"Super",	0,		XK_Super_L},
    {7,		"Button1",	Button1Mask,	0},
    {7,		"Button2",	Button2Mask,	0},
    {7,		"Button3",	Button3Mask,	0},
    {7,		"Button4",	Button4Mask,	0},
    {7,		"Button5",	Button5Mask,	0},
};

#define	MODSCTRLINDEX	1	/* index of "c" in the above array */
#define	MODSMETAINDEX	4	/* index of "m" */


static Boolean
compile_modifiers(const char **pp, struct mouse_acts *mactp)
{
    const char		*p = *pp;
    const char		*p1;
    Boolean			exclusive = False;
    LateBindingsPtr		latep = NULL;
    int			nlate;

    while (*p == ' ' || *p == '\t')
	++p;

    p1 = p;
    while (isalpha((int) *p1))
	++p1;

    if (p1 - p == 3 && memcmp(p, "Any", 3) == 0) {
	mactp->mask = mactp->value = 0;
	p = p1;
	while (*p == ' ' || *p == '\t')
	    ++p;
	if (*p != '<')
	    return False;
    }
    else if (p1 - p == 4 && memcmp(p, "None", 4) == 0) {
	mactp->mask = ~0;
	mactp->value = 0;
	p = p1;
	while (*p == ' ' || *p == '\t')
	    ++p;
	if (*p != '<')
	    return False;
    }
    else {
	if (*p == '!') {
	    exclusive = True;
	    do {
		++p;
	    } while (*p == ' ' || *p == '\t');
	}

	for (;;) {
	    Boolean negated = False;
	    struct modifierinf *mp;

	    if (*p == '<')
		break;

	    if (*p == '~') {
		negated = True;
		++p;
	    }

	    if (*p == '^') {
		mp = &modifiers[MODSCTRLINDEX];
		++p;
	    }
	    else if (*p == '$') {
		mp = &modifiers[MODSMETAINDEX];
		++p;
	    }
	    else {
		int min, max;

		p1 = p;
		while (isalnum((int) *p))
		    ++p;

		/* do binary search */
		min = -1;
		max = XtNumber(modifiers);
		for (;;) {
		    int i, diff;

		    i = (min + max) / 2;
		    if (i == min)
			return False;	/* if not found */
		    mp = &modifiers[i];

		    diff = (p - p1) - mp->len;
		    if (diff == 0)
			diff = memcmp(p1, mp->name, p - p1);

		    if (diff == 0)
			break;
		    if (diff > 0) min = i;
		    else max = i;
		}
	    }
	    if (mp->mask) {
		mactp->mask |= mp->mask;
		if (!negated) mactp->value |= mp->mask;
	    }
	    else {
		LateBindingsPtr lp1;

		if (latep == NULL) {
		    nlate = 3;
		    latep = xmalloc(3 * sizeof(LateBindings));
		    latep->ref_count = 1;
		}
		else {
		    nlate += 2;
		    latep = xrealloc(latep, nlate * sizeof(LateBindings));
		}
		lp1 = &latep[nlate - 3];
		lp1->knot = lp1[1].knot = negated;
		lp1->pair = True;
		lp1->keysym = mp->keysym;
		++lp1;
		lp1->pair = False;
		lp1->ref_count = 0;
		lp1->keysym = mp->keysym + 1;
		++lp1;
		lp1->knot = lp1->pair = False;
		lp1->ref_count = 0;
		lp1->keysym = 0;
	    }

	    while (*p == ' ' || *p == '\t')
		++p;
	}
    }

    mactp->late_bindings = latep;
    *pp = p;

    return True;
}

static Boolean
compile_evtype(const char **pp, unsigned int *buttonp)
{
    const char		*p = *pp;
    const char		*p0;

    ++p;	/* already assumed to be '<' */
    while (*p == ' ' || *p == '\t')
	++p;

    p0 = p;
    while (isalpha((int) *p) && p - p0 < 3)
	++p;

    if (p - p0 != 3 || memcmp(p0, "Btn", 3) != 0)
	return False;

    if (*p >= '1' && *p <= '9') {
	unsigned int n = *p - '0';

	while (*++p >= '0' && *p <= '9')
	    n = n * 10 + (*p - '0');

	*buttonp = n;
    }

    p0 = p;
    while (isalpha((int) *p))
	++p;

    if (p - p0 != 4 || memcmp(p0, "Down", 4) != 0)
	return False;

    while (*p == ' ' || *p == '\t')
	++p;

    if (*p++ != '>')
	return False;

    while (*p == ' ' || *p == '\t')
	++p;

    if (*p++ != ':')
	return False;

    *pp = p;

    return True;
}

static void
compile_mouse_actions(void)
{
    struct mouse_acts	**mactpp;
    struct mouse_acts	*mactp;
    const char		*p = resource.mouse_translations;
    const char		*p_end;
    const char		*p_base = base_mouse_translations;
    struct mouse_acts	mact;

    mactpp = &mouse_actions;

    if (p == NULL) {
	p = p_base;
	p_base = NULL;
    }

    do {	/* loop over translations strings */
	p_end = p + strlen(p);
	for (;;) {
	    while (*p == ' ' || *p == '\t')
		++p;

	    if (*p == '\n') continue;
	    if (*p == '\0') break;

	    mact.mask = mact.value = 0;
	    mact.button = 0;

	    if (!compile_modifiers(&p, &mact)
	      || !compile_evtype(&p, &mact.button)) {
		XDVI_WARNING((stderr, "syntax error in wheel translations"));
	    }
	    else if (compile_action(p, &mact.action) || mact.action != NULL) {
		mactp = xmalloc(sizeof(struct mouse_acts));
		*mactp = mact;

		*mactpp = mactp;
		mactpp = &mactp->next;
	    }

	    p = memchr(p, '\n', p_end - p);
	    if (p == NULL) break;
	    ++p;
	}

	p = p_base;
	p_base = NULL;
    }
    while (p != NULL);

    *mactpp = NULL;
}


static void
create_colormaps(void)
{
#ifdef GREY
    G_depth = (unsigned int)DefaultDepthOfScreen(SCRN);
    G_visual = DefaultVisualOfScreen(SCRN);
    G_colormap = DefaultColormapOfScreen(SCRN);
#ifdef XSERVER_INFO
    if (globals.debug & DBG_ALL)
	fprintf(stdout, "--- G_depth: %d\n", G_depth);
#endif

    if (resource.install != False && G_visual->class == PseudoColor) {
	/* look for a TrueColor visual with more bits */
	XVisualInfo template;
	XVisualInfo *list;
	int nitems_return;
#ifdef XSERVER_INFO
	if (globals.debug & DBG_ALL)
	    fprintf(stdout, "--- looking for a better TrueColor visual\n");
#endif

	template.screen = XScreenNumberOfScreen(SCRN);
	template.class = TrueColor;
	list = XGetVisualInfo(DISP, VisualScreenMask | VisualClassMask,
			      &template, &nitems_return);
	if (list != NULL) {
	    XVisualInfo *list1;
	    XVisualInfo *best = NULL;

	    for (list1 = list; list1 < list + nitems_return; ++list1) {
#ifdef XSERVER_INFO
		if (globals.debug & DBG_ALL)
		    fprintf(stdout, "--- checking %d\n", list1->depth);
#endif
		if ((unsigned int)list1->depth > G_depth
# if PS_GS
		    /* patch by Toni Ronkko <tronkko@hytti.uku.fi>, fixes bug #458057:
		     * SGI supports additional depths of 12 and 30, but
		     * these are not supported by ghostscript (see
		     * xdev->vinfo->depth in gdevxcmp.c (ghostscript-6.51)),
		     * so we restrict the values to those supported by gs.
		     */
		    && (list1->depth == 1 || list1->depth == 2
			|| list1->depth == 4 || list1->depth == 8
			|| list1->depth == 15 || list1->depth == 16
			|| list1->depth == 24 || list1->depth == 32)
# endif
		    && (best == NULL || list1->depth > best->depth))
		    best = list1;
	    }
	    if (best != NULL) {
#ifdef XSERVER_INFO
		if (globals.debug & DBG_ALL)
		    fprintf(stdout, "--- best depth: %d\n", best->depth);
#endif
		G_depth = best->depth;
		G_visual = best->visual;
		G_colormap = XCreateColormap(DISP,
					     RootWindowOfScreen(SCRN),
					     G_visual, AllocNone);
		XInstallColormap(DISP, G_colormap);
		temp_args1[0].value = (XtArgVal) G_depth;
		temp_args1[1].value = (XtArgVal) G_visual;
		temp_args1[2].value = (XtArgVal) G_colormap;
		XtSetValues(globals.widgets.top_level, temp_args1, XtNumber(temp_args1));
		XtSetTypeConverter(XtRString, XtRPixel,
				   XdviCvtStringToPixel,
				   (XtConvertArgList) colorConvertArgs, 2,
				   XtCacheByDisplay, NULL);
		{
		    /* This is needed so that popup windows have the right
		       visual and colormap.  It is unnecessary for newer
		       versions of Motif (Motif 2.1.0, Solaris 9) but
		       needed for older versions (Motif 1.2.5, Solaris 2.6),
		       and for Xaw.
		    */
		    XrmDatabase	db = XtScreenDatabase(SCRN);
		    XrmValue	val;
		    
		    val.size = sizeof G_visual;
		    val.addr = (XtPointer) &G_visual;
		    XrmPutResource(&db, "XDvi*visual", XtRVisual, &val);
		    
		    val.size = sizeof G_colormap;
		    val.addr = (XtPointer) &G_colormap;
		    XrmPutResource(&db, "XDvi*colormap", XtRColormap, &val);
		}
	    }
	    XFree(list);
	}
    }

#ifdef MOTIF
    if (globals.debug & DBG_GUI) {
	printf("Compiled with %s, runtime version %d.%d\n",
	       /* 	   XmVERSION, XmREVISION, XmUPDATE_LEVEL, */
	       XmVERSION_STRING,
	       xmUseVersion / 1000, xmUseVersion % 1000);
    }
#endif
    
    if (resource.install == True && G_visual->class == PseudoColor) {
	XColor tmp_color;
#ifdef XSERVER_INFO
	if (globals.debug & DBG_ALL)
	    fprintf(stdout, "--- PseudoColor, trying to install colormap\n");
#endif

	/* This next bit makes sure that the standard black and white pixels
	   are allocated in the new colormap. */
	tmp_color.pixel = BlackPixelOfScreen(SCRN);
	XQueryColor(DISP, G_colormap, &tmp_color);
	XAllocColor(DISP, G_colormap, &tmp_color);

	tmp_color.pixel = WhitePixelOfScreen(SCRN);
	XQueryColor(DISP, G_colormap, &tmp_color);
	XAllocColor(DISP, G_colormap, &tmp_color);

	G_colormap = XCopyColormapAndFree(DISP, G_colormap);
	temp_args1[2].value = (XtArgVal) G_colormap;
	XtSetValues(globals.widgets.top_level, temp_args1 + 2, 1);
    }

    XtGetApplicationResources(globals.widgets.top_level, (XtPointer)&resource,
			      app_pixel_resources, XtNumber(app_pixel_resources),
			      (ArgList)NULL, 0);

#endif /* GREY */

    globals.gc.do_copy = resource.copy;

#if GREY || COLOR
    color_data[0].pixel = resource.fore_Pixel;
    color_data[1].pixel = resource.back_Pixel;
    XQueryColors(DISP, G_colormap, color_data, 2);
#endif
    
#if COLOR
    fg_initial.r = color_data[0].red;
    fg_initial.g = color_data[0].green;
    fg_initial.b = color_data[0].blue;
    bg_initial.r = color_data[1].red;
    bg_initial.g = color_data[1].green;
    bg_initial.b = color_data[1].blue;

#endif

#if GREY
    if (G_depth == 1) {
#ifdef XSERVER_INFO
	if (globals.debug & DBG_ALL)
	    fprintf(stdout, "--- using depth 1\n");
#endif
	resource.use_grey = False;
    }
    if (resource.use_grey && G_visual->class != TrueColor) {
#ifdef XSERVER_INFO
	if (globals.debug & DBG_ALL)
	    fprintf(stdout, "--- using grey, but not TrueColor\n");
#endif
	init_plane_masks();
	if (!globals.gc.do_copy) {
	    /* Retain color_data[1].pixel for psgs.c.  */
	    XColor tmp_color;
#ifdef XSERVER_INFO
	    if (globals.debug & DBG_ALL)
		fprintf(stdout, "--- not using copy\n");
#endif
	    
	    tmp_color = color_data[1];
	    tmp_color.pixel = resource.back_Pixel;
	    XStoreColor(DISP, G_colormap, &tmp_color);
	}
    }
#endif
}

static void
create_widgets(
#ifdef MOTIF
	       Widget tool_bar, Widget form,
#endif
	       int *add_w, int *add_h)
{

#ifdef MOTIF
    Widget status_line;

#if 0
    {
	int i, n, longest_page = 0;
	Dimension width, height;
	char tmpbuf[1024];
	char *fontname;
	XmString tmpstring;
	Widget dummy_list;
	XmRendition rendition;
	XmRenderTable rtable;
	Arg args[10];
	/*
	 * Try to determine width of longest page string:
	 */
	for (i = 0; i < total_pages; i++) {
	    int curr = abs(pageinfo_get_number(i));
	    if (curr > longest_page)
		longest_page = curr;
	}
	fprintf(stderr, "longest page number: %d\n", longest_page);
	sprintf(tmpbuf, "* %d", longest_page);
	tmpstring = XmStringCreateLocalized(tmpbuf);
	dummy_list = XtCreateWidget("PageList", xmListWidgetClass, form, NULL, 0);
	fprintf(stderr, "rendition table\n");
	n = 0;
	/*     XtVaGetValues(globals.widgets.top_level, XmNfontList, &fontname, NULL); */
	/*     fprintf(stderr, "fontname: |%s|\n", fontname); */
	XtSetArg(args[n], XmNfontName, "8x16"); n++;
	XtSetArg(args[n], XmNfontType, XmFONT_IS_FONT); n++;
	rendition = XmRenditionCreate(dummy_list, XmFONTLIST_DEFAULT_TAG, args, n);
	rtable = XmRenderTableAddRenditions(NULL, &rendition, 1, XmMERGE_REPLACE);
	XtVaSetValues(dummy_list, XmNrenderTable, rtable, NULL);
    
	/* 	rendition = XmRenditionCreate(globals.widgets.top_level, XmFONTLIST_DEFAULT_TAG, NULL, 0); */
	fprintf(stderr, "extent\n");
	XmStringExtent(rtable, tmpstring, &width, &height);
	fprintf(stderr, "string %s has width %d, height %d\n", tmpbuf, width, height);
	XtDestroyWidget(dummy_list);
	XmRenditionFree(rendition);
	XmStringFree(tmpstring);
    }
#endif
    {
#define ARG_LEN 20
	int n = 0;
	Arg list_args[ARG_LEN];

	XtSetArg(list_args[n], XmNlistSizePolicy, XmVARIABLE);			n++;
	XtSetArg(list_args[n], XmNwidth, resource.pagelist_width);		n++;
#if defined(USE_PANNER) && USE_XAW_PANNER
	XtSetArg(list_args[n], XmNheight, globals.page.h - 62);			n++;
#else
	XtSetArg(list_args[n], XmNheight, globals.page.h);			n++;
#endif
	/* 	XtSetArg(list_args[n], XmNspacing, 10);					n++; */
#if defined(USE_PANNER) && USE_XAW_PANNER
	XtSetArg(list_args[n], XmNtopAttachment, XmATTACH_WIDGET);		n++;
	XtSetArg(list_args[n], XmNtopWidget, panner);				n++;
#else
	XtSetArg(list_args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
#endif
	XtSetArg(list_args[n], XmNbottomAttachment, XmATTACH_FORM);		n++;
	XtSetArg(list_args[n], XmNleftAttachment, XmATTACH_FORM);		n++;
	XtSetArg(list_args[n], XmNrightAttachment, XmATTACH_OPPOSITE_FORM);	n++;
	XtSetArg(list_args[n], XmNrightOffset, -resource.pagelist_width);	n++;
	XtSetArg(list_args[n], XmNresizable, True);				n++;
	XtSetArg(list_args[n], XmNtopOffset, 2);				n++;
	XtSetArg(list_args[n], XmNleftOffset, 2);				n++;
	ASSERT(n < ARG_LEN, "list_args too short");
#undef ARG_LEN
	/* TODO: Find a smart way to determine size of pagelist instead of using resource.pagelist_width,
	   and find a smart way of resizing it when switching to a file with larger page numbers!
	*/
	page_list = XmCreateScrolledList(form, "PageList", list_args, n);
    }
    
    if (resource.expert_mode == XPRT_SHOW_NONE) {
	XtUnmanageChild(XtParent(page_list));
	XtUnmanageChild(page_list);
    }
    
    globals.widgets.main_window = XtVaCreateManagedWidget("mainWindow",
							  xmScrolledWindowWidgetClass, form,
							  XmNscrollingPolicy, XmAUTOMATIC,
							  XmNleftAttachment, XmATTACH_WIDGET,
							  /* lesstif balks if we just use page_list, so use its parent,
							     the xmScrolledWindow, for alignment: */
							  XmNleftWidget, XtParent(page_list),
							  XmNallowResize, True,
							  /* 					  XmNtopOffset, 2, */
							  XmNtopAttachment, XmATTACH_FORM,
							  XmNbottomAttachment, XmATTACH_FORM,
							  XmNrightAttachment, XmATTACH_FORM,
							  /* 					      XmNleftOffset, 10, */
							  XmNresizable, True,
							  XmNborderWidth, 0,
							  NULL);
    XtManageChild(form);

    globals.widgets.x_bar = XtNameToWidget(globals.widgets.main_window, "HorScrollBar");
    globals.widgets.y_bar = XtNameToWidget(globals.widgets.main_window, "VertScrollBar");

    
#ifdef TEST_SCROLLING
    /* TODO: try the following to prevent `flashing' effect - still breaks
       scrolling in its current form though */
    globals.widgets.draw_background = XtVaCreateManagedWidget("drawing_bg",
							      xmFormWidgetClass, globals.widgets.main_window,
							      XmNwidth, globals.page.w,
							      XmNheight, globals.page.h,
							      XmNtopAttachment, XmATTACH_FORM,
							      XmNbottomAttachment, XmATTACH_FORM,
							      XmNrightAttachment, XmATTACH_FORM,
							      XmNleftAttachment, XmATTACH_FORM,
							      XmNhighlightThickness, 0,
							      XmNbackground, resource.back_Pixel,
							      XmNrubberPositioning, True,
							      NULL);
#endif
    globals.widgets.draw_widget = XtVaCreateWidget("drawing",
						   xmDrawingAreaWidgetClass,
#ifdef TEST_SCROLLING
						   globals.widgets.draw_background,
#else
						   globals.widgets.main_window,
#endif
						   XmNwidth, globals.page.w,
						   XmNheight, globals.page.h,
						   XmNhighlightThickness, 0,
						   XmNrubberPositioning, True,
						   XtNbackground, resource.back_Pixel,
						   NULL);

#if !FIXED_FLUSHING_PAGING
    ASSERT(XtParent(globals.widgets.draw_widget) != NULL, "");
    XtVaSetValues(XtParent(globals.widgets.draw_widget), XtNbackground, resource.back_Pixel, NULL);
#endif
    
#ifdef TEST_SCROLLING
    fprintf(stderr, "globals.widgets.draw_widget is: %p\n", globals.widgets.draw_widget);
#endif
    XtVaGetValues(globals.widgets.main_window, XmNclipWindow, &globals.widgets.clip_widget, NULL);

    XtVaSetValues(globals.widgets.main_row,
		  XmNmenuBar, globals.widgets.menu_bar,
		  XmNcommandWindow, XtParent(tool_bar),
		  XmNworkWindow, form,
		  NULL);
    
    XtVaSetValues(globals.widgets.main_window,
		  XmNworkWindow, globals.widgets.draw_widget,
		  NULL);

    XtManageChild(globals.widgets.draw_widget);

    status_line = create_statusline(globals.widgets.main_row);
    XtVaSetValues(globals.widgets.main_row,
		  XmNmessageWindow, status_line,
		  NULL);
    if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) == 0) {
	XtUnmanageChild(status_line);
    }

    XtManageChild(globals.widgets.main_row);

    if ((resource.expert_mode & XPRT_SHOW_TOOLBAR) != 0) {
	if (resource.toolbar_unusable)
	    statusline_error(STATUS_LONG,
			     "Error creating the toolbar pixmaps - toolbar is disabled!");
	else
	    XtManageChild(XtParent(tool_bar));
    }

    if ((resource.expert_mode & XPRT_SHOW_MENUBAR) != 0)
	XtManageChild(globals.widgets.menu_bar);
    
    XmAddTabGroup(globals.widgets.draw_widget);

    /*
      note: a few more custom translations for page_list are defined in
      pagesel.c, since the actions are only known there.
    */
    {
	size_t i, k = 0;
#define WIDGETS_SIZE 10
	Widget widgets[WIDGETS_SIZE];
	XtTranslations xlats = XtParseTranslationTable("<BtnDown>: press()\n"
						       "<Motion>: motion()\n"
						       "<BtnUp>: release()\n");

	widgets[k++] = globals.widgets.main_row;
	/* 	widgets[k++] = globals.widgets.menu_bar; */
	widgets[k++] = tool_bar;
	widgets[k++] = page_list;
	widgets[k++] = globals.widgets.main_window;
	widgets[k++] = globals.widgets.draw_widget;
	widgets[k++] = globals.widgets.clip_widget;

	ASSERT(k < WIDGETS_SIZE, "widgets list too short");
#undef WIDGETS_SIZE
	for (i = 0; i < k; i++) {
	    XtOverrideTranslations(widgets[i],
				XtParseTranslationTable(base_key_translations));
	    if (i > 3) { /* widgets for which we want to use our own mouse translations */
		XtOverrideTranslations(widgets[i], xlats);
	    }
	}
    }
    
#if 0 /* wasn't clip_widget already done above (i=5)? */
    if (resource.mouse_translations != NULL) {
 	TRACE_GUI((stderr, "merging in mouse translations |%s|", resource.mouse_translations));
	XtOverrideTranslations(globals.widgets.clip_widget, XtParseTranslationTable(resource.mouse_translations));
    }
#endif

    if (resource.main_translations != NULL) {
	XtTranslations xlats = XtParseTranslationTable(resource.main_translations);
	XtOverrideTranslations(globals.widgets.draw_widget, xlats);
	XtOverrideTranslations(globals.widgets.clip_widget, xlats);
	XtOverrideTranslations(globals.widgets.main_row, xlats);
	XtOverrideTranslations(globals.widgets.menu_bar, xlats);
	XtOverrideTranslations(globals.widgets.main_window, xlats);
	/* don't do it for the page list, otherwise mouse customizations will
	   break the default list bindings too. */
	/* XtOverrideTranslations(page_list, xlats); */
    }

    compile_mouse_actions();

#else /* MOTIF */
    
    globals.widgets.form_widget = XtVaCreateManagedWidget("form", formWidgetClass, globals.widgets.top_level,
							  XtNdefaultDistance, 0,
							  NULL);
    globals.widgets.vport_widget = XtVaCreateManagedWidget("vport", viewportWidgetClass,
							   globals.widgets.form_widget,
							   XtNborderWidth, 0,
							   XtNtop, XtChainTop,
							   XtNbottom, XtChainBottom,
							   XtNleft, XtChainLeft,
							   XtNright, XtChainRight,
							   XtNallowHoriz, True,
							   XtNallowVert, True,
							   NULL);
    globals.widgets.clip_widget = XtNameToWidget(globals.widgets.vport_widget, "clip");
    
    globals.widgets.draw_widget = XtVaCreateManagedWidget("drawing", drawWidgetClass, globals.widgets.vport_widget,
							  XtNwidth, globals.page.w,
							  XtNheight, globals.page.h,
							  XtNx, 0,
							  XtNy, 0,
							  XtNlabel, "",
							  NULL);
    
    XtOverrideTranslations(globals.widgets.form_widget, XtParseTranslationTable(base_key_translations));

    if (resource.main_translations != NULL) {
	XtOverrideTranslations(globals.widgets.form_widget, XtParseTranslationTable(resource.main_translations));
    }

    XtOverrideTranslations(globals.widgets.form_widget,
			   XtParseTranslationTable("<BtnDown>: press()\n"
						   "<Motion>:  motion()\n"
						   "<BtnUp>:   release()\n"));

    compile_mouse_actions();

    
    /* set background colors of the drawing widget */
    XtVaSetValues(globals.widgets.draw_widget, XtNbackground, resource.back_Pixel, NULL);
    
#if !FIXED_FLUSHING_PAGING
    XtVaSetValues(globals.widgets.clip_widget, XtNbackground, resource.back_Pixel, NULL);
#endif

    /* initialize add_w with width of button panel */
    create_menu_buttons(globals.widgets.form_widget, add_w);

#endif /* MOTIF */

    /* activate expert mode and related settings */
    toggle_statusline();
    toggle_scrollbars();
#ifdef MOTIF
    toggle_pagelist();
    toggle_toolbar();
    toggle_menubar();
#else
    if ((resource.expert_mode & XPRT_SHOW_BUTTONS) == 0)
	toggle_buttons();
#endif

    *add_h = 0;
#ifdef MOTIF
    *add_w = 0;
    if (resource.expert_mode & XPRT_SHOW_PAGELIST)
	*add_w += xm_get_width(page_list);
/*     if (globals.widgets.y_bar != NULL) */
/*      	add_w += xm_get_width(globals.widgets.y_bar); */
/*     if (globals.widgets.x_bar != NULL) */
/*      	add_h += xm_get_width(globals.widgets.x_bar); */
    if (resource.expert_mode & XPRT_SHOW_MENUBAR)
	*add_h += xm_get_height(globals.widgets.menu_bar);
    if (resource.expert_mode & XPRT_SHOW_TOOLBAR)
	*add_h += xm_get_height(tool_bar);
    if (resource.expert_mode & XPRT_SHOW_STATUSLINE)
	*add_h += xm_get_height(status_line);
#else
    /* add_w has been initialized by create_menu_buttons() call above.
       Reset to 0 if we're in expert mode. */
    if (!(resource.expert_mode & XPRT_SHOW_BUTTONS)) {
	*add_w = 0;
    }
    if (resource.expert_mode & XPRT_SHOW_STATUSLINE) {
	/* FIXME: Unfortunately the statusline hasn't been created at this point for Xaw,
	   so the value is still the built-in default.
	*/
	*add_h += get_statusline_height();
    }
#endif
}


static void
realize_widgets(Dimension main_win_w, Dimension main_win_h, int add_w)
{    
    /*
     *	Realize the widgets (or windows).
     */

#ifndef MOTIF
    realize_button_panel(main_win_h);
#endif
    
    XtAddEventHandler(
#ifdef MOTIF
		      globals.widgets.clip_widget,
#else
		      globals.widgets.vport_widget,
#endif
		      StructureNotifyMask, False,
		      handle_resize, (XtPointer) NULL);
    
    XtAddEventHandler(globals.widgets.top_level, PropertyChangeMask, False,
		      handle_property_change, (XtPointer) NULL);
    XtAddEventHandler(globals.widgets.draw_widget, ExposureMask, False, handle_expose,
		      (XtPointer) &mane);
    XtRealizeWidget(globals.widgets.top_level);

    mainDeleteWindow = XInternAtom(DISP, "WM_DELETE_WINDOW", False);

#ifdef MOTIF
    /* for Xaw, event handlers for scrollbars are added inside get_geom(), events.c */
    ASSERT(globals.widgets.x_bar != NULL, "");
    ASSERT(globals.widgets.y_bar != NULL, "");
#ifdef USE_PANNER
    XtAddEventHandler(globals.widgets.x_bar, ButtonPressMask | ButtonReleaseMask, False, handle_x_scroll, NULL);
    XtAddEventHandler(globals.widgets.y_bar, ButtonPressMask | ButtonReleaseMask, False, handle_y_scroll, NULL);
#endif
    XmAddWMProtocolCallback(globals.widgets.top_level, mainDeleteWindow, xdvi_exit_callback, NULL);
#else
    wmProtocols = XInternAtom(DISP, "WM_PROTOCOLS", False);
    XSetWMProtocols(DISP, XtWindow(globals.widgets.top_level), &mainDeleteWindow, 1);
    XtAddEventHandler(globals.widgets.top_level, NoEventMask, True, handle_delete_message, NULL);
#endif
    
    /* check whether we want to run in fullscreen mode */
    if (resource.fullscreen) {
	reconfigure_window(resource.fullscreen, main_win_w + add_w, main_win_h, False);
    }

#ifdef MOTIF
    XmProcessTraversal(globals.widgets.draw_widget, XmTRAVERSE_CURRENT);
    TRACE_GUI((stderr, "toplevel: %p", (void *)globals.widgets.top_level));
    create_tips(globals.widgets.top_level);
#else
    if ((resource.expert_mode & XPRT_SHOW_STATUSLINE) != 0) {
	create_statusline();
    }
#endif

    currwin.win = mane.win = XtWindow(globals.widgets.draw_widget);

    {
	XWindowAttributes attrs;

	(void)XGetWindowAttributes(DISP, mane.win, &attrs);
	G_backing_store = attrs.backing_store;
    }

#ifdef HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    XtAddEventHandler(globals.widgets.top_level, (EventMask)0, True, _XEditResCheckMessages,
		      (XtPointer)NULL);
#endif
}

/*
 * Create colors and GCs.
 * In color mode, color changes affect globals.gc.fore, globals.gc.fore2,
 * and globals.gc.rule, but not globals.gc.copy or globals.gc.high.
 */
static void
create_gcs(void)
{
    
#if GREY
    if (resource.gamma == 0.0)
	resource.gamma = 1.0;
#endif

    if (!resource.rule_color)
	resource.rule_pixel = resource.fore_Pixel;

#if !COLOR

#if GREY
    if (resource.use_grey)
	init_pix();
    else
#endif
    { /* not #defined GREY, or not resource.use_grey */
	XGCValues values;
	Pixel set_bits = (Pixel) (resource.fore_Pixel & ~resource.back_Pixel);
	Pixel clr_bits = (Pixel) (resource.back_Pixel & ~resource.fore_Pixel);
	Boolean copy_tmp = resource.copy;
	    
	globals.gc.copy = set_or_make_gc(NULL, GXcopy, resource.fore_Pixel, resource.back_Pixel);
	if (copy_tmp || (set_bits && clr_bits)) {
	    globals.gc.rule = globals.gc.copy;
	    if (!resource.thorough)
		copy_tmp = True;
	}
	if (copy_tmp) {
	    globals.gc.fore = globals.gc.rule;
	    if (!resource.copy) {
		warn_overstrike();
	    }
	}
	else {
	    if (set_bits) {
		globals.gc.fore = set_or_make_gc(NULL, GXor, set_bits, 0);
	    }
	    if (clr_bits || !set_bits)
		*(globals.gc.fore ? &globals.gc.fore2 : &globals.gc.fore) = set_or_make_gc(NULL, GXandInverted, clr_bits, 0);
	    if (!globals.gc.rule)
		globals.gc.rule = globals.gc.fore;
	}
    }
#endif /* !COLOR */
    
    {
	Pixel link_pix, visited_link_pix;
	get_link_colors(&link_pix, &visited_link_pix);
	globals.gc.linkcolor = set_or_make_gc(NULL, GXcopy, link_pix, resource.back_Pixel);
	globals.gc.visited_linkcolor = set_or_make_gc(NULL, GXcopy, visited_link_pix, resource.back_Pixel);
    }
	
#if COLOR
    /* Not affected by color changes.  */
    globals.gc.copy = set_or_make_gc(NULL, GXcopy, resource.fore_Pixel, resource.back_Pixel);
#endif
    globals.gc.high = set_or_make_gc(NULL, GXcopy, resource.hl_Pixel, resource.back_Pixel);
    
    globals.gc.ruler = set_or_make_gc(NULL, GXcopy, resource.rule_pixel, resource.fore_Pixel);
    
#if XAW
    /*
     * There's a bug in the Xaw toolkit, in which it uses the
     * DefaultGCOfScreen to do vertical scrolling in the Text widget.
     * This leads to a BadMatch error if our visual is not the default one.
     * The following kludge works around this.
     */
    DefaultGCOfScreen(SCRN) = globals.gc.copy;
#endif
}

static void
do_fork(void)
{
    TRACE_CLIENT((stderr, "no other instance of xdvi found, forking ..."));
    /*
     * No suitable xdvi found, so we start one by
     * self-backgrounding.
     */
    /* flush output buffers to avoid double buffering (i.e. data
       waiting in the output buffer being written twice, by the parent
       and the child) */
    fflush(stdout);
    fflush(stderr);
    XFlush(DISP);
    if (fork())	/* if initial process (do NOT use vfork()!) */
	_exit(0);
}

#if defined(MOTIF) && defined(USE_PANNER) && USE_XAW_PANNER
static void 
panner_cb(Widget widget, XtPointer closure, XtPointer report_ptr)
{
    fprintf(stderr, "panner_cb called!\n");
}
#endif

#if !DELAYED_MKTEXPK

static XtIntervalId m_font_popup_id = 0;
static Widget m_font_popup = 0;

static void
remove_font_popup(XtPointer client_data, XtIntervalId *id)
{
    UNUSED(client_data);
    UNUSED(id);

    if (m_font_popup != 0) {
	kill_message_window(m_font_popup);
    }
}

static void
remove_font_popup_exit_cb(XtPointer arg)
{
    UNUSED(arg);
    xdvi_exit(EXIT_SUCCESS);
}

static void
create_font_popup(XtPointer client_data, XtIntervalId *id)
{
    int *curr_timeout = (int *)client_data;
    static int new_timeout = 0;
    
    UNUSED(client_data);
    UNUSED(id);
    
/*     fprintf(stderr, "+++++++++++++++ create_font_popup\n"); */
    
    if (m_font_popup_id) {
	if (*curr_timeout > 0) {
	    XtRemoveTimeOut(m_font_popup_id);
	    m_font_popup_id = XtAppAddTimeOut(globals.app, *curr_timeout, create_font_popup, (XtPointer)&new_timeout);
	}
	else {
	    m_font_popup = choice_dialog(globals.widgets.top_level,
					 MSG_INFO, NULL,
#ifndef MOTIF				 
					 NULL,
#endif
					 NULL, NULL, /* no pre_callbacks */
					 NULL, NULL, NULL, /* default arguments for `OK' */
					 "Exit Xdvi", remove_font_popup_exit_cb, (XtPointer)NULL,
					 "Loading %s\n(may take some time creating fonts ...)",
					 globals.dvi_name);
	}
    }
}

void
register_font_popup(void)
{
    /* Use a two-step process, so that when the timeout is removed by unregister_font_popup(),
       it will occur before new_timeout. create_font_popup() will call XtAppAddTimeOut() again
       with new_timeout = 0.
    */
    static int new_timeout = 50;
/*     fprintf(stderr, "+++++++++++++++ registered font popup\n"); */
    m_font_popup_id = XtAppAddTimeOut(globals.app, 100, create_font_popup, (XtPointer)&new_timeout);
}

void
unregister_font_popup(void)
{
/*     fprintf(stderr, "+++++++++++++++ unregister_font_popup\n"); */
    if (m_font_popup_id) {
	XtRemoveTimeOut(m_font_popup_id);
	m_font_popup_id = 0;
	/* FIXME: calling this directly crashes xdvi?? */
	/*  	m_font_popup_id = XtAppAddTimeOut(globals.app, 1, remove_font_popup, (XtPointer)NULL); */
	remove_font_popup(NULL, NULL);
    }    
}
#endif /* !DELAYED_MKTEXPK */

/* revert resources */
void
load_app_resources(Boolean also_pixels)
{
    /*     /\* reset some resources to built-in defaults *\/ */
    /*     resource.browser = NULL; */
    /*     resource.editor = NULL; */
    /*     resource.gamma = 1; */
    /*     resource.link_style = 3; */
    /*     resource.link_color = LINK_COLOR_FALLBACK; */
    /*     resource.visited_link_color = VISITED_LINK_COLOR_FALLBACK; */
    /*     resource.expert_mode = 31; */
    /*     resource.use_color = True; */
    /*     resource.match_highlight_inverted = True; */
    
    XtGetApplicationResources(globals.widgets.top_level, (XtPointer)&resource,
			      application_resources, XtNumber(application_resources),
			      (ArgList)NULL, 0);

    if (also_pixels) {
	XtGetApplicationResources(globals.widgets.top_level, (XtPointer)&resource,
				  app_pixel_resources, XtNumber(app_pixel_resources),
				  (ArgList)NULL, 0);
    }
    /*      fprintf(stderr, "gamma: %f\n", resource.gamma); */
}

/*
 * Unfortunately this must be a callback, for the file selector ...
 * This is the second part of Main: Create all widgets, initialize the DVI file,
 * and enter the main event loop.
 */
void
run_dvi_file(const char *filename, void *data)
{
    Boolean tried_dvi_ext = False;
    struct startup_info *cb = (struct startup_info *)data;
    
#ifdef MOTIF
    Widget tool_bar = 0;
    Widget form = 0;
#endif

    char *title_name = NULL;
    char *icon_name = NULL;
    dviErrFlagT errflag = NO_ERROR;
    
    int add_w = 0, add_h = 0;
    Dimension main_win_w, main_win_h;
    
    UNUSED(data);
    ASSERT(filename != NULL, "filename must have been initialized here!");
    
    globals.dvi_name = xstrdup(filename);
    file_history_push(globals.dvi_name);
    
    TRACE_FILES((stderr, "globals.dvi_name is: |%s| %p\n", globals.dvi_name, globals.dvi_name));

    globals.dvi_file.dirname = get_dir_component(globals.dvi_name);
    xdvi_assert(XDVI_VERSION_INFO, __FILE__, __LINE__,
		globals.dvi_file.dirname != NULL,
		"globals.dvi_name (%s) must contain a dir component",
		globals.dvi_name);
    globals.dvi_file.dirlen = strlen(globals.dvi_file.dirname);

    form_dvi_property();
    
    /*
      If `unique' is active, we may need to pass the file to a different instance of xdvi:
    */
    if (resource.unique) {
	Window w1 = 0, w2 = 0;
	if ((w1 = get_xdvi_window_id(True, NULL)) != 0 || (w2 = get_xdvi_window_id(False, NULL)) != 0) {
	    if (w1 != 0) { /* another xdvi instance, same file: reload */
		w2 = w1;
		set_string_property("", atom_reload(), w2);
	    }
	    else { /* another xdvi instance, different file: load new file */
		set_string_property(globals.dvi_name, atom_newdoc(), w2);
	    }
	    if (cb->page_arg != NULL) { /* switch to different page */
		if (strlen(cb->page_arg) == 0) { /* special case: treat `+' as last page */
		    set_string_property("+", atom_newpage(), w2);
		}
		else {
		    set_string_property(cb->page_arg, atom_newpage(), w2);
		}
	    }
	    else {
		/* if page_arg is empty, go to 1st page so that this page is
		   inserted into the page history (fix for #1044891) */
		set_string_property("1", atom_newpage(), w2);
	    }
	    /* in all cases, raise the window of the other instance */
	    set_string_property("", atom_raise(), w2);
	    xdvi_exit(EXIT_SUCCESS);
	}
	else if (resource.src_fork) {
	    do_fork();
	}
    }

    /*
      Similar for forward search or string search:
    */
    if (resource.src_pos != NULL || resource.find_string != NULL) {
	Window w;
	if ((w = get_xdvi_window_id(True, NULL)) != 0) {
	    /* another instance of xdvi running, displaying the same file */
	    TRACE_CLIENT((stderr, "Match; changing property of client and exiting ..."));
	    if (resource.src_pos != NULL)
		set_sourceposition_property(resource.src_pos, w);
	    else
		set_stringsearch_property(resource.find_string, w);
	    xdvi_exit(EXIT_SUCCESS);
	}
	else if (resource.src_fork) {
	    do_fork();
	}
    }

    /* Needed for source specials and for calling ghostscript. */
    xputenv("DISPLAY", XDisplayString(DISP));


    if (globals.debug) {
	fprintf(stderr, "%s %s, kpathsea: %s\n", XDVIK_PROGNAME, XDVI_VERSION_INFO, kpathsea_version_string);
	fprintf(stderr,
		"configured with: ppi=%d shrink=%d mfmode=%s alt_font=%s paper=%s\n",
		resource.pixels_per_inch,
		currwin.shrinkfactor,
		resource.mfmode ? resource.mfmode : "<NONE>",
		resource.alt_font,
		resource.paper);
    }
    
#ifdef T1LIB
    /* needed so that T1lib can produce tfm's e.g. for cm-super */
    kpse_set_program_enabled(kpse_tfm_format, True, kpse_src_compile);
#endif
    kpse_set_program_enabled(kpse_any_glyph_format, resource.makepk, kpse_src_compile);
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
     *		Step 2:  Settle colormap issues.  This should be done before
     *		other widgets are created, so that they get the right
     *		pixel values.  (The top-level widget won't have the right
     *		values, but I don't think that makes any difference.)
     */
    
#ifdef XSERVER_INFO
    print_xserver_info();
#endif

    create_colormaps();

    
#ifdef TESTING_OPEN_FILES
    fprintf(stderr, "open_max: %ld\n", OPEN_MAX);
    for (i = 0; i < OPEN_MAX - 10; i++) {
	FILE *fp;
	if ((fp = fopen("/tmp/foo", "r")) == NULL) {
	    perror("fopen");
	    xdvi_exit(EXIT_FAILURE);
	}
    }
    fprintf(stderr, "opened %d files.\n", i);
#endif
    

    /* toolbar code may open files, but we have no check close_a_file() in
       the toolbar code; so do this before prescan() possibly opens lots of files.
    */
#ifdef MOTIF
    globals.widgets.main_row = XmCreateMainWindow(globals.widgets.top_level, "main", NULL, 0);
    
    create_menu_buttons(globals.widgets.main_row, &globals.widgets.menu_bar);

    /* seems to be needed for enabling `XmNhighlightOnEnter' for the toolbar buttons
       - is this the correct place to do it? */
    XtVaSetValues(globals.widgets.top_level, XmNkeyboardFocusPolicy, (XtArgVal)XmPOINTER, NULL);

    form = XtVaCreateWidget("form", xmFormWidgetClass, globals.widgets.main_row,
			    XmNshadowThickness, 0,
			    NULL);

    if (resource.main_translations != NULL) {
	XtOverrideTranslations(form, XtParseTranslationTable(resource.main_translations));
    }

    
#if defined(USE_PANNER) && USE_XAW_PANNER
    panner = XtVaCreateWidget("panner", pannerWidgetClass, form,
			      XmNtopAttachment, XmATTACH_FORM,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNleftOffset, 20,
			      XmNrightAttachment, XmATTACH_OPPOSITE_FORM,
			      XmNrightOffset, -resource.pagelist_width + 20,
			      XmNtopOffset, 2,
			      XmNleftOffset, 2,
			      XtNheight, 60,
			      XtNwidth, resource.pagelist_width - 50,
			      XtNsliderX, 5,
			      XtNsliderY, 7,
			      XtNinternalSpace, 0,
			      XtNshadowThickness, 0,
			      NULL);
#endif
#endif

    /* use bounding box for highlighting if our visual isn't TrueColor
       (not worth the trouble ...) */
    if (G_visual->class != TrueColor) {
	resource.match_highlight_inverted = False;
    }
    
    /*
     *		Step 3:  Initialize the dvi file and set titles.
     */

#ifdef T1LIB
    /*
      At this point DISP, G_visual, G_depth and G_colormap must
      be defined. Also, init_t1() must go before internal_open_dvi(), since
      read_postamble will define some fonts and insert them into
      fontmaps_hash, but we need a clean fontmaps_hash for detecting
      duplicate entries in the map file.
    */

    if (resource.t1lib) {
	init_t1();
    }
#endif /* T1LIB */

#if DELAYED_MKTEXPK
    /* Open and initialize the DVI file. First, disable creation of PK fonts
     * so that we can count the missing fonts that are to be generated. */
    kpse_set_program_enabled(kpse_any_glyph_format, False, kpse_src_compile);
#endif
    
    setup_signal_handlers(False);

#if !DELAYED_MKTEXPK
    /* Notify users that fonts are being created. This is just a hack
       and no replacement for true asynchronous font creation since it
       doesn't give details (is just invoked if startup takes somewhat
       longer) and freezes during font creation.
    */
    register_font_popup();
#endif
    
    /* open and initialize the DVI file, but don't read the fonts yet */
    if (!internal_open_dvi(globals.dvi_name, &errflag, True
#if DELAYED_MKTEXPK
			   , False /* read fonts, but don't initialize data structures */
#endif
			   )) {
	if (tried_dvi_ext) {
	    XDVI_FATAL((stderr, "Could not open %s: %s, and %s.dvi doesn't exist either - exiting.",
			globals.dvi_name, get_dvi_error(errflag), globals.dvi_name));
	}
	else {
	    XDVI_FATAL((stderr, "Could not open %s: %s.",
			globals.dvi_name, get_dvi_error(errflag)));
	}
    }

#if DELAYED_MKTEXPK
    fprintf(stderr, "after opening ...\n");
    /* Now re-enable PK creation and read the postamble for a second time.
     * FIXME: Actually we don't need this re-reading, could as well read the
     * entire thing in the first run, not quit early and correctly initialize
     * the fonts without creating them. */
    kpse_set_program_enabled(kpse_any_glyph_format, resource.makepk, kpse_src_compile);

    if (!internal_open_dvi(globals.dvi_name, &errflag, True, True)) {
	if (tried_dvi_ext) {
	    XDVI_FATAL((stderr, "Could not open %s: %s, and %s.dvi doesn't exist either - exiting.",
			globals.dvi_name, get_dvi_error(errflag), globals.dvi_name));
	}
	else {
	    XDVI_FATAL((stderr, "Could not open %s: %s.",
			globals.dvi_name, get_dvi_error(errflag)));
	}
    }
#else
    unregister_font_popup();
#endif
    
    if (cb->page_arg != NULL) {
	if (cb->page_arg[0] == '\0') { /* empty page_arg -> goto last page */
	    current_page = total_pages - 1;
	    page_history_insert(current_page);
	}
	else {
	    char *testptr;
	    current_page = strtoul(cb->page_arg, &testptr, 10) - 1;
	    if (*testptr != '\0') {
		XDVI_FATAL((stderr, "Invalid page number: `%s'.", cb->page_arg));
	    }
	    current_page = check_goto_page(current_page, True);
	}
    }
    else {
	page_history_insert(current_page);
    }
    file_history_set_page(current_page);
    
    ASSERT(globals.dvi_file.bak_fp != NULL, "Backup file pointer must have been initialized here");
    if (resource.prescan) {
	prescan(globals.dvi_file.bak_fp);
    }

    globals.page.unshrunk_w = pageinfo_get_page_width(current_page);
    globals.page.unshrunk_h = pageinfo_get_page_height(current_page);
    TRACE_GUI((stderr, "globals.page.unshrunk_w: %d, h: %d; window: %d, %d",
	       globals.page.unshrunk_w, globals.page.unshrunk_h,
	       pageinfo_get_window_width(current_page),
	       pageinfo_get_window_height(current_page)));
    
    init_page();

    /*
     *		Step 4:  Create widgets, and set initial window size.
     */

    /* currently these override expert mode - using this is deprecated
       in favour of `-expertmode'; inform user about this: */
    if (resource.statusline) {
	XDVI_WARNING((stderr, "The option/X resource `statusline' is obsolete; "
		      "use `-expertmode <flag>' instead, e.g. `-expertmode 1'\n"
		      "to switch on the status line, or `-expertmode 6'\n"
		      "to switch it off. See the xdvi man page for details."));
	resource.expert_mode |= XPRT_SHOW_STATUSLINE;
    }

    /*      XtRealizeWidget(globals.widgets.top_level); */

#ifdef MOTIF
    tool_bar = create_toolbar(globals.widgets.main_row, globals.widgets.menu_bar);
    if (resource.main_translations != NULL) {
	XtOverrideTranslations(tool_bar, XtParseTranslationTable(resource.main_translations));
    }
#endif

    create_widgets(
#ifdef MOTIF
		   tool_bar, form,
#endif
		   &add_w, &add_h);

    TRACE_GUI((stderr, "add_w = %d, add_h = %d\n", add_w, add_h));
    /*  fprintf(stderr, "geometry xdvirc: |%s|, orig: |%s|\n", resource.xdvirc_geometry, resource.geometry); */
    
    /*
     *	Set initial window size.
     *	This needs to be done before colors are assigned because if
     *	-s 0 is specified, we need to compute the shrink factor
     *	(which in turn affects whether init_pix is called).
     */
    set_windowsize(&main_win_w, &main_win_h, add_w, add_h, False);

    realize_widgets(main_win_w, main_win_h, add_w);

    /* this needs to be done after total_pages is known (via internal_open_dvi) */
    get_icon_and_title(globals.dvi_name, &icon_name, &title_name);
    add_icon(globals.widgets.top_level, title_name, icon_name);
    /* this needs to be done after the widgets have been created */
    set_icon_and_title(icon_name, title_name);
    free(icon_name);
    free(title_name);
    icon_name = title_name = NULL;

    
    G_image = XCreateImage(DISP, G_visual, 1, XYBitmap, 0,
			   (char *)NULL, 0, 0, BMBITS, 0);
    G_image->bitmap_unit = BMBITS;
#ifdef	WORDS_BIGENDIAN
    G_image->bitmap_bit_order = MSBFirst;
#else
    G_image->bitmap_bit_order = LSBFirst;
#endif
    {
	short endian = MSBFirst << 8 | LSBFirst;
	G_image->byte_order = *((char *)&endian);
    }

    /* Store window id for use by src_client_check().  */
    {
	xuint32 data;
#if !(defined(WORD64) || defined(LONG64))
	data = XtWindow(globals.widgets.top_level);
#else
	set_window_id(XtWindow(globals.widgets.top_level),
		      (unsigned char *)&data);
#endif
	XChangeProperty(DISP, DefaultRootWindow(DISP),
			atom_xdvi_windows(), atom_xdvi_windows(), 32,
			PropModePrepend, (unsigned char *)&data, 1);
	set_dvi_property();
    }

    /*
     *	Step 5:  Assign colors and GCs.
     *		 Because of the latter, this has to go after the widgets are realized.
     */

    create_gcs();

    create_cursors();

#ifdef MOTIF
#if defined(USE_PANNER) && USE_XAW_PANNER
    XtVaSetValues(panner, XtNsliderWidth, globals.page.w / 2,
		  XtNsliderHeight, globals.page.h / 2,
		  XtNcanvasWidth, globals.page.w,
		  XtNcanvasHeight, globals.page.h,
		  NULL);
    XtManageChild(panner);
    XtAddCallback(panner, XtNreportCallback, panner_cb, (XtPointer)NULL);
#endif
    create_pagelist();
#endif

    /* trigger forward search */
    do_forward_search(resource.src_pos);

    /* trigger string search */
    if (resource.find_string != NULL) {
	globals.ev.flags |= EV_FIND;
    }

    /* trigger anchor search */
    if (resource.anchor_pos != NULL) {
	g_anchor_pos = xstrdup(resource.anchor_pos);
	g_anchor_len = strlen(g_anchor_pos);
	globals.ev.flags |= EV_ANCHOR;
    }

#if defined(MOTIF) && HAVE_XPM
    tb_check_navigation_sensitivity(current_page);
#endif

#if CHECK_APP_FILEVERSION
    check_app_defaults_fileversion();
#endif

    /* can do this only after scrollbars have been realized */
    if (!BROKEN_RECONFIG && (resource.expert_mode & XPRT_SHOW_SCROLLBARS) == 0) {
	toggle_scrollbars();
    }

    /* initialize file watching */
    if (resource.watch_file > 0.0) {
#if XDVI_XT_TIMER_HACK
	watch_file_cb(NULL, NULL);
#else
	XDVI_WARNING((stderr, "Could not redefine XtAppAddTimeOut(); `watchfile' not available."));
#endif
    }

    /* raise `early' message windows */
    raise_message_windows();

    {    
	String args[1];
	char mmode[LENGTH_OF_INT];
	snprintf(mmode, LENGTH_OF_INT, "%d", resource.mouse_mode);
	args[0] = mmode;
	XtCallActionProc(globals.widgets.top_level, "switch-mode", NULL, args, 1);
    }

    /* go to home position on first page to honor -(side|top)margin flags */
    if (!resource.keep_flag)
    	home(False);

    do_pages();
}
