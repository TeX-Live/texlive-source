/*========================================================================*\

Copyright (c) 1990-2019  Paul Vojta

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
IN NO EVENT SHALL PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

NOTE:
xdvi is based on prior work, as noted in the modification history
in xdvi.c.

\*========================================================================*/

/*
 *	Original version by Eric C. Cooper, CMU
 */

#ifndef	XDVI_H_
#define	XDVI_H_

/* headers used by all modules */
#include "xdvi-debug.h"

#include "kpathsea/config.h" /* kpathsea definitions */

/********************************
 *	The C environment	*
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/* in case stdlib.h doesn't define these ... */
#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#  define EXIT_FAILURE 1
#endif

#ifdef HAVE_WORKING_NL_LANGINFO_CODESET
#  define USE_LANGINFO 1
#else
#  define USE_LANGINFO 0
#endif

#if HAVE_SYS_WAIT_H
#  include <sys/wait.h>
#endif

#ifndef WIFEXITED
#  define WIFEXITED(status)	(((status) & 255) == 0)
#endif

#ifndef WEXITSTATUS
#  define WEXITSTATUS(status)	((unsigned)(status) >> 8)
#endif

#ifndef WIFSIGNALED
#  ifndef WIFSTOPPED
#    define WIFSTOPPED(status)	(((status) & 0xff) == 0x7f)
#  endif
#  define WIFSIGNALED(status)	(!WIFSTOPPED(status) && !WIFEXITED(status))
#endif

#ifndef WTERMSIG
#  define WTERMSIG(status)	((status) & 0x7f)
#endif

#if A4
# define DEFAULT_PAPER		"a4"
#else
# define DEFAULT_PAPER		"us"
#endif

/*
 * Define to 1 if you want to use the experimental font creation code
 */
#define DELAYED_MKTEXPK 0

#ifdef __hpux
/* On HP-UX 10.10 B and 20.10, compiling with _XOPEN_SOURCE + ..._EXTENDED
 * leads to poll() not realizing that a file descriptor is writable in psgs.c.
 */
#  define _HPUX_SOURCE	1
#else
#  ifndef _XOPEN_SOURCE
#    define _XOPEN_SOURCE 600
#  endif
#  define _XOPEN_SOURCE_EXTENDED 1
#  define __EXTENSIONS__ 1	/* needed to get struct timeval on SunOS 5.5 */
#  define _SVID_SOURCE 1	/* needed to get S_IFLNK in glibc */
#  define _BSD_SOURCE 1		/* needed to get F_SETOWN in glibc-2.1.3 */
#endif

/* Some O/S dependent kludges. */
#ifdef _AIX
#  define _ALL_SOURCE 1
#endif

/* just a kludge, no real portability here ... */
#define DIR_SEPARATOR '/'

#if STDC_HEADERS
#  include <stddef.h>
#  include <stdlib.h>
/* the following works around the wchar_t problem */
#  include <X11/X.h>
#  if HAVE_X11_XOSDEFS_H
#    include <X11/Xosdefs.h>
#  endif
#  ifdef X_NOT_STDC_ENV
#    undef X_NOT_STDC_ENV
#    undef X_WCHAR
#    include <X11/Xlib.h>
#    define X_NOT_STDC_ENV
#  endif
#endif /* STDC_HEADERS */

/* For wchar_t et al., that the X files might want. */
#include "kpathsea/systypes.h"
#include "kpathsea/c-memstr.h"

#ifdef HAVE_SYS_PARAM_H
#  include <sys/param.h> /* this should define MAXPATHLEN */
#endif

#include "kpathsea/c-pathmx.h" /* get fallback for PATH_MAX if all else fails */

#include <X11/Xlib.h>	/* include Xfuncs.h, if available */
#include <X11/Xutil.h>	/* needed for XDestroyImage */
#include <X11/Xos.h>

#include <X11/Xfuncs.h>
#include <X11/Intrinsic.h>

#if HAVE_XI21
# include <X11/extensions/XInput2.h>	/* for hi-res (smooth) scrolling */
#endif

#ifndef MAXPATHLEN
#  ifdef PATH_MAX
#    define MAXPATHLEN PATH_MAX
#  else
#    define MAXPATHLEN 1024
#  endif
#endif

typedef	char Bool3; /* Yes/No/Maybe */
#define	True	1
#define	False	0
#define	Maybe	2


#ifdef DEBUG
#  include<asm/msr.h>
extern unsigned long time_start, time_end;
#endif

#include <X11/Xmd.h>	/* get WORD64 and LONG64 */

#if defined(HAVE_STDINT_H)
#include <stdint.h>
#elif defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif

/* for unused parameters */
#ifndef UNUSED
#  define UNUSED(x) ((void)(x))
#endif

#include <string.h>

#include <stdio.h>
#include <setjmp.h>

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

/* all of these are POSIX and should have been defined by unistd.h: */
#ifndef STDIN_FILENO
#  define STDIN_FILENO 0
#endif

#ifndef STDOUT_FILENO
#  define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#  define STDERR_FILENO 2
#endif

/* see C FAQ; additional +1 for '\0' */
#define LENGTH_OF_INT ((sizeof(int) * CHAR_BIT + 2) / 3 + 1 + 1)
#define LENGTH_OF_LONG ((sizeof(long) * CHAR_BIT + 2) / 3 + 1 + 1)
#define LENGTH_OF_ULONG ((sizeof(unsigned long) * CHAR_BIT + 2) / 3 + 1 + 1)

#include "kpathsea/c-dir.h" /* dirent.h, NAMLEN */

#include <stdarg.h>
#include "kpathsea/c-fopen.h"
#include "kpathsea/version.h"

#ifndef	_XFUNCPROTOBEGIN
#  define _XFUNCPROTOBEGIN
#  define _XFUNCPROTOEND
#endif


/* If xmkmf is broken and there's a symlink from /usr/include/X11 to the right
 * place, then there will be no -I... argument on the cc command line for the
 * X include files.  Since gcc version 3 and higher sets __STDC__ to 0 when
 * including system header files on some platforms, we may end up with
 * NeedFunctionPrototypes set to 0 when it should be 1.  So, let's force the
 * issue.
 */
#if __STDC__ && !defined(FUNCPROTO)
/* FUNCPROTO is a bitmask specifying ANSI conformance (see Xfuncproto.h).
   The single bits specify varargs, const availability, prototypes etc.;
   we enable everything here. */
#  define FUNCPROTO (-1)
#endif

#ifndef	VOLATILE
#  if __STDC__ || (defined(__stdc__) && defined(__convex__))
#    define VOLATILE	volatile
#  else
#    define VOLATILE	/* as nothing */
#  endif
#endif

#ifndef	OPEN_MODE
/*
 * SU, 2001/01/07: xdvi defines OPEN_MODE as "r" or as "r", "ctx=stm" (for VMS),
 * but we use the definition of FOPEN_R_MODE from kpathsea/c-fopen.h instead:
 */
#  define OPEN_MODE FOPEN_R_MODE
#endif	/* OPEN_MODE */

#ifndef	VMS
#  define OPEN_MODE_ARGS const char *
#else
#  define OPEN_MODE_ARGS const char *, const char *
#endif

#ifndef __LINE__
#  define __LINE__ 0
#endif

#ifndef __FILE__
#  define __FILE__ "?"
#endif

#define	MAXDIM 32767

typedef	unsigned char	ubyte;

typedef	unsigned int	wide_ubyte;
typedef	int		wide_bool;

#if defined(MAKEPK) && !defined(MKTEXPK)
#  define MKTEXPK 1
#endif

#define	spell_conv0(n, f) ((long) (n * f))
#define	spell_conv(n) spell_conv0(n, dimconv)

typedef BMTYPE bmTypeT;
typedef unsigned BMTYPE bmUnitT;
/* #define	BMUNIT		    unsigned BMTYPE */
#define	BMBITS (8 * BMBYTES) /* number of bits in a bmTypeT */

#define	ADD(a, b) ((bmUnitT *) (((char *) a) + b))
#define	SUB(a, b) ((bmUnitT *) (((char *) a) - b))

extern bmUnitT bit_masks[BMBITS + 1];

/* for safely printing char *s that might be NULL */
#define STRING_OR_NULL(x) ((x == NULL ? "<NULL>" : x))

#define INSIDE_MANE_WIN ((currwin.win == mane.win))
#define MAGNIFIER_ACTIVE ((magnifier.win != 0))
/* #define MAGNIFIER_ACTIVE ((globals.cursor.flags & CURSOR_MAG)) */

#define	mane_base_x 0
#define	mane_base_y 0


struct frame {
    /* dvi_h and dvi_v is the horizontal and vertical baseline position;
       it is the responsability of the set_char procedure to update
       them. */
    struct framedata {
	long dvi_h, dvi_v, w, x, y, z;
	int pxl_v;
    } data;
    struct frame *next, *prev;
};

#ifdef TEXXET
typedef void setcharRetvalT;
#else
typedef long setcharRetvalT;
#endif

typedef	setcharRetvalT (*set_char_proc) (
#ifdef TEXXET
					 wide_ubyte cmd,
#endif
					 wide_ubyte ch);

#define ROUNDUP(x,y) (((x)+(y)-1)/(y))

#ifndef	BDPI
#  define BDPI 600
#endif

#if defined(GS_PATH) && !defined(PS_GS)
#  define PS_GS 1
#endif

#if defined(PS_DPS) || defined(PS_NEWS) || defined(PS_GS)
#  define PS 1
#else
#  define PS 0
#endif

#ifdef MOTIF
#  include <Xm/Xm.h>

/* Note: non-k xdvi has MOTIF_TIMERS here, we use XDVI_XT_TIMER_HACK instead (see above) */

#  ifndef DDIST
#    define DDIST 4
#  endif
#  ifndef DDIST_MAJOR
#    define DDIST_MAJOR 10
#  endif
#  ifndef DDIST_MINOR
#    define DDIST_MINOR 5
#  endif

extern  XmStringCharSet G_charset;

/*
 * Whether to use some widgets which are only available with Motif 2.0.
 * In these cases, the LessTif versions are either buggy or visually
 * inferior (as of lesstif-0.93.18), in which case suitable replacements
 * should be used; e.g. cascade button or a textfield plus button for
 * a combobox, or a textfield plus button for a spinbox.
 */
#  if XmVersion >= 2000 && !defined(LESSTIF_VERSION)
#    define USE_COMBOBOX 1
#    define Xdvi_COMBO_BOX_ARROW_SIZE 15 /* the default arrows are *way* too big */
#    define USE_SPINBOX 1
#  else
#    define USE_COMBOBOX 0
#    define USE_SPINBOX 0
#  endif

#endif /* MOTIF */

typedef enum {
    XPRT_SHOW_NONE = 0U,
    XPRT_SHOW_STATUSLINE = 1,
    XPRT_SHOW_SCROLLBARS = 2,
#ifdef MOTIF
    XPRT_SHOW_PAGELIST = 4,
    XPRT_SHOW_TOOLBAR = 8,
    XPRT_SHOW_MENUBAR = 16,
    XPRT_SHOW_ALL = 31
#else
    XPRT_SHOW_BUTTONS = 4,
    XPRT_SHOW_ALL = 7
#endif /* MOTIF */
} expertFlagT;

typedef enum {
    MOUSE_MODE1,
    MOUSE_MODE2,
    MOUSE_MODE3,
    MOUSE_MODE_MAX
} mouseModeT;

typedef enum {
    NO_MODE_ACTIVE,
    RULER_MODE_ACTIVE,
    TEXT_MODE_ACTIVE
} modeFlagT;

/* SUBPIXEL_NONE for no subpixel rendering, otherwise order of subpixels
   (from option/resource subPixels) */
typedef enum { SUBPIXEL_NONE, SUBPIXEL_RGB, SUBPIXEL_BGR } subpixelOrderT;

/*
 *	X resources.
 */
extern struct x_resources {
    /* NOTE: we don't use CFGFILE */
    Boolean	no_init_file;
    Boolean	regression;
    const char *geometry;
    const char *windowsize;
    Boolean	remember_windowsize;
    int		app_defaults_fileversion;
    mouseModeT	mouse_mode;
    Boolean	use_tex_pages;
    int		shrinkfactor;
    const char *main_translations;
    const char *mouse_translations;
/*     const char *wheel_translations; */
    int		wheel_unit;
    int		density;
    Boolean     omega;
    const char *mouse_mode1_name;
    const char *mouse_mode1_description;
    int		mouse_mode1_cursor;
    const char *mouse_mode2_name;
    const char *mouse_mode2_description;
    int		mouse_mode2_cursor;
    const char *mouse_mode3_name;
    const char *mouse_mode3_description;
    int		mouse_mode3_cursor;
#ifdef	GREY
    float	gamma;
/*     float	    inverted_factor; */
#endif
    int		pixels_per_inch;
    Boolean	delay_rulers;
    int		tick_length;
    char       *tick_units;
    const char *sidemargin;
    int		sidemargin_int;
    const char *topmargin;
    int		topmargin_int;
    const char *xoffset;
    int		xoffset_int;
    const char *yoffset;
    int		yoffset_int;
    Boolean	use_current_offset; /* only used internally */
    const char *paper;
    Boolean     paper_landscape; /* only used internally */
    const char *alt_font;
#ifdef MKTEXPK
    Boolean	makepk;
#endif
    const char *mfmode;
    const char *editor;
#ifdef MOTIF
    char       *prefs_editor_list;
#endif
    Boolean	freetype;
#if HAVE_XI21
    Boolean	xi2scrolling;
#endif
    const char *src_pos;
    const char *find_string;
    const char *text_encoding;
    Boolean	src_fork;
    const char *sub_pixels;
    const char *file_history;
    int		file_history_size;
    Boolean	no_file_arg_use_history;
#if RGB_ANTI_ALIASING
    subpixelOrderT subpixel_order;
    float	subpixel_energy[3];
#endif
    Boolean	unique;
    Boolean	list_fonts;
    Boolean	reverse;
    Boolean	warn_spec;
    Boolean	hush;
    Boolean	hush_chars;
    Boolean	hush_chk;
    Boolean	hush_stdout;
    Boolean	hush_bell;
    Boolean	safer;
#ifdef VMS
    const char *fore_color;
    const char *back_color;
#endif
    Pixel	fore_Pixel;
    Pixel	back_Pixel;
    /*     Pixel	brdr_Pixel; */
    Pixel	hl_Pixel;
    Pixel	cr_Pixel;
    const char *icon_geometry;
    Boolean	keep_flag;
    Boolean	copy;
    Boolean	thorough;
    Boolean	pause;
    const char *pause_special;
    Boolean	fullscreen;
#ifdef PS
    int		postscript;
    Boolean	allow_shell;
#  ifdef PS_DPS
    Boolean	useDPS;
#  endif
#  ifdef PS_NEWS
    Boolean	useNeWS;
#  endif
#  ifdef PS_GS
    Boolean	useGS;
    Boolean	gs_safer;
    Boolean	gs_alpha;
    const char *gs_path;
    const char *gs_palette;
    int		gs_timeout;
#  endif
#  ifdef MAGICK
    Boolean	useMAGICK;
    const char *magick_cache;
#  endif
#endif	/* PS */
    Boolean     prescan;
    Boolean     use_temp_fp;
    const char *debug_arg;
    const char *menu_translations;
    Boolean	expert;
    float	watch_file; /* check DVI file every so often, in seconds */
    int         expert_mode;
#ifndef MOTIF
    Dimension	btn_side_spacing;
    Dimension	btn_top_spacing;
    Dimension	btn_between_spacing;
    /* ignored, only for backwards compatibility */
    Dimension	btn_between_extra;
    Dimension	btn_border_width;
#else /* MOTIF */
    const char *toolbar_translations;
#endif /* MOTIF */
    Boolean	statusline;
#ifdef MOTIF
    /* not a user-level resource; indicates a problem with the toolbar, in
       which case the toolbar is disabled. */
    Boolean	toolbar_unusable;
    const char *toolbar_pixmap_file;
    Boolean	toolbar_buttons_raised;
    Boolean	tooltips_in_statusline;
    int		tooltips_wait_period; /* used for communication with Tip.c */
    int		tooltips_wait_period_bak; /* uncustomized value */
    Boolean	show_tooltips;
#endif /* MOTIF */
    Boolean	pagelist_highlight_current;
    Dimension	pagelist_width;
    const char *mg_arg[5];
#if COLOR
    Boolean     use_color;
#endif
#ifdef GREY
    Boolean	use_grey;
    Bool3	install;
#endif
    Boolean	match_highlight_inverted;
    const char *dvips_path;
    const char *ps2pdf_path;
    int		dvips_hang;
    int		dvips_fail_hang;
    const char *dvips_printer_str;
    const char *dvips_options_str;
    int		default_saving_format;
    int		default_printing_target;
    int         grid_mode;
    char       *rule_color;
    Pixel	rule_pixel;
    int		link_style;
    char       *link_color;
    char       *visited_link_color;
    char       *browser;
#ifdef MOTIF
    char       *prefs_browser_list;
#  if USE_COMBOBOX
    char       *search_history;
    int		search_history_size;
#  endif
#endif /* MOTIF */
    char       *unknown_mime_suffix;
    char       *no_mime_suffix;
    char       *anchor_pos;
    /* whether to open file in new window from file selector (only for DVI files) */
    Boolean    filesel_open_new_window;
    /* bitmask of current search window settings */
    unsigned int    search_window_defaults;
    /*     char *    _scroll_pages; */
    char       *help_general;
    char       *help_hypertex;
    char       *help_othercommands;
    char       *help_pagemotion;
    char       *help_marking;
    char       *help_modes;
    char       *help_search;
    char       *help_mousebuttons;
    char       *help_sourcespecials;
    int         page_history_size;
} resource;

extern void load_app_resources(Boolean also_pixels);


struct WindowRec {
    Window win;
    int	shrinkfactor;
    int	base_x;
    int	base_y;
    unsigned int width;
    unsigned int height;
    /* for pending expose events */
    int	min_x;
    int max_x;
    int min_y;
    int max_y;
};

struct event_info {
    int flags;
    VOLATILE int ctr;
    jmp_buf canit;
    jmp_buf next;	/* to skip next event */
};

struct pause_info {
    int num;
    int *num_save;
    Boolean flag;
};

struct gc_info {
    GC rule;
    GC fore;
    GC inverted;
    GC high;
    GC linkcolor;
    GC visited_linkcolor;
    GC fore2;
    GC fore2_bak;
    GC fore2_bak1;
    GC copy;
    GC ruler;
    Boolean do_copy;
};

/* flags for special cursor states */
#define CURSOR_LINK 1		/* hyperlink */
#define CURSOR_MAG 2		/* magnifier is mapped */
#define CURSOR_DRAG_V 4		/* vertical dragging */
#define CURSOR_DRAG_H 8		/* horizontal dragging */
#define CURSOR_DRAG_A 16	/* dragging in any direction */
#define CURSOR_CORRUPTED 32	/* DVI file corrupted */
#define CURSOR_TEXT 64		/* DVI file corrupted */
#define CURSOR_RULER 128	/* DVI file corrupted */

/* cursors for all states */
struct cursor_info {
    Cursor mode1;
    Cursor mode2;
    Cursor mode3;

    Cursor wait;
    Cursor corrupted;
    /* hyperlink */
    Cursor link;
    /* horizontal/vertical/all directions drag */
    Cursor drag_h;
    Cursor drag_v;
    Cursor drag_a;
    /* support for `pause' feature */
    Cursor pause;
    /* empty cursor */
    Cursor empty;
    
    /* one of the flags defined above */
    unsigned long flags;
};

struct window_expose_info {
    int min_x, max_x, min_y, max_y;
};

/* for communication with forward search in dvi-draw.c */
struct src_info {
    int fwd_box_page;
    const char *fwd_string;
};

struct widget_info {
    Widget top_level;
    Widget draw_widget, draw_background, clip_widget;
    Widget x_bar, y_bar;
#ifdef MOTIF
    Widget main_window, main_row, tool_bar, top_row, menu_bar;
#else
    Widget vport_widget, form_widget, paned;
#endif
};

struct page_info {
    unsigned int w, h;
    unsigned int unshrunk_w, unshrunk_h;
};

struct dvi_file_info {
    char *dirname;
    size_t dirlen;
    FILE *bak_fp;
    time_t time;	/* last file modification time */
};

/* struct to hold global settings that can't go into the resources,
 * as a replacement for global variables:
 */
extern struct program_globals {
    XtAppContext app;
    const char *program_name;	/* argv[0] without the directory part */
    const char *cwd;		/* directory where xdvi is running in (for childs who need to change back to it) */
    char *dvi_name;		/* dvi file name, fully expanded with REALPATH */
    Boolean load_init_file;	/* whether to read/save ~/.xdvirc */
    char *orig_locale;		/* original locale we are running in */
    unsigned long debug;	/* debugging option */

    /* for older Motif versions or LessTif, which have broken
       handling of Act_motion() / Act_release() events */
    Boolean broken_motif_event_handling;
    
    /* offset from c-style (0 based) numbers to real pagenumbers;
       also contains the offset that user has set via Act_declare_page_number() */
    int pageno_correct;

    /* paper unit used for grid mode */
    unsigned int grid_paper_unit;
    
    /* whether we warn about unrecognized specials. Copy of resource.warn_spec value,
       set to False in the drawing routine to reduce the amount of warnings given.
       Better maybe replace with hash lookup similar to fonts warnings? */
    Boolean warn_spec_now;

    /* used to save X resource values */
    char *curr_paper;
    char *curr_editor;
    char *curr_browser;
    float curr_gamma;
    Boolean curr_use_color;

    modeFlagT curr_mode;
    
    /* forward search info */
    struct src_info src;
    
    /* event information */
    struct event_info ev;

    /* support for `-pause' feature */
    struct pause_info pausing;
    
    /* window expose information */
    struct window_expose_info win_expose;

    struct gc_info gc;
    
    /* cursor information */
    struct cursor_info cursor;

    /* widget information */
    struct widget_info widgets;

    /* page size info */
    struct page_info page;

    /* DVI file and modification time info */
    struct dvi_file_info dvi_file;
    
} globals;


/* TODO: put these into globals as well */
extern struct WindowRec mane;
extern struct WindowRec currwin;
extern struct WindowRec magnifier;


extern XtAppContext app;
extern	char		*dvi_property;		/* for setting in window */
extern	size_t		dvi_property_length;
extern	XImage		*G_image;
extern int G_backing_store;
/* extern	int		home_x, home_y; */

extern	Display		*DISP;
extern	Screen		*SCRN;

extern	XtAccelerators	G_accels_cr;
#ifdef GREY
extern	Visual		*G_visual;
extern	unsigned int	G_depth;
extern	Colormap	G_colormap;
#else
#  define G_depth	(unsigned int) DefaultDepthOfScreen(SCRN)
#  define G_visual	DefaultVisualOfScreen(SCRN)
#  define G_colormap	DefaultColormapOfScreen(SCRN)
#endif


#define	TNTABLELEN	30	/* length of TeXnumber array (dvi file) */
#define	VFTABLELEN	5	/* length of TeXnumber array (virtual fonts) */

extern struct font *tn_table[TNTABLELEN];
extern struct font *font_head;
extern struct tn *tn_head;
extern wide_ubyte maxchar;
extern unsigned short current_timestamp;

extern int current_page;
extern int total_pages;

extern unsigned long magnification;
extern double dimconv;
extern double tpic_conv;

/* whether this file contains source specials, so that we need to
   update the `windows' property for forward search on expose events:
*/
extern Boolean have_src_specials;

extern Boolean dragcurs;	/* whether drag cursor is active; needed by hypertex.c */
extern int drag_flags;	/* 1 = vert, 2 = horiz; also needed by hypertex.c */

typedef void (*mouse_proc) (XEvent *);
extern mouse_proc mouse_motion;
extern mouse_proc mouse_release;

struct mouse_acts {
	Modifiers	mask;
	Modifiers	value;
	struct _LateBindings *late_bindings;
	unsigned int	button;
	struct xdvi_action *action;
	struct mouse_acts *next;
};

extern	struct mouse_acts	*mouse_actions;

#if GREY
extern Pixel plane_masks[4];
#endif

#if GREY || COLOR
extern XColor color_data[2];
#endif

#if COLOR
struct rgb {
    unsigned short r, g, b;
};

struct pagecolor {
    struct rgb bg;
    unsigned int stacksize;
    struct rgb *colorstack;
};
struct pagecolor_info {
  /* different from non-k xdvi, we also need the allocated stack size,
     since it might differ from the number of pages (e.g. when loading
     a new file). */
    size_t size;
    /* this is const in non-k xdvi, but some instances of it may ultimately
       get free()d in full_reset_colors(), so it's not *really* const ...
    */
    struct pagecolor *stack;
};

/* Information on background color and initial color stack for each page.  */
extern struct pagecolor_info page_colors;

/* The initial color stack is obtained from the pagecolor record for a page.  */
extern struct rgb *color_bottom;
extern unsigned int color_bot_size;	/* number of entries */

/* Additions to the runtime color stack on a given page are stored in a linked
   list.  "struct colorframe" is defined in special.c.  */
extern struct colorframe *rcs_top;

/* Color states.  */

/*
 * For each (foreground, background) color pair, we keep information (depending
 * on the color model).  It is organized as a linked list of linked lists,
 * with background color more significant.
 */

struct bgrec {
    struct bgrec *next;
    struct rgb color;
    struct fgrec *fg_head;
    Boolean pixel_good;	/* if the pixel entry is valid */
    Pixel pixel;
};

struct fgrec {
    struct fgrec *next;
    struct rgb color;
    Boolean pixel_good;		/* if the pixel entry is valid */
    Pixel pixel;
#  if GREY
    Boolean palette_good;	/* if the palette entry is valid */
    Pixel palette[16];		/* non-TrueColor only */
#  endif
};

extern struct rgb fg_initial;	/* Initial fg (from command line) */
extern struct rgb bg_initial;	/* Initial bg */
extern struct bgrec *bg_head;		/* head of list */
extern struct bgrec *bg_current;	/* current bg value */
extern struct fgrec *fg_current;	/* current fg value */
extern struct fgrec *fg_active;		/* where the GCs are */

/* List of allocated colors (to be deallocated upon document change) */
extern Pixel *color_list;		/* list of colors */
extern unsigned int color_list_len;	/* current len of list */
extern unsigned int color_list_max;	/* allocated size */

/* Whether the color situation has been warned about.  */
extern Boolean color_warned;
#endif /* COLOR */


#if HAVE_XI21
struct xi2_valinfo {
	int		number;
	double		increment;
	double		lastval;
	double		lastexact;
	double		factor;
	unsigned long	serial;
};

struct xi2_slave {
	int		id;			/* slave device id */
	unsigned int	flags;
	unsigned char	enabled;
	unsigned char	btn_mask;	/* buttons corresponding to valuators */
	struct xi2_valinfo vert, horiz;
	struct xi2_slave *next;
};

	/* flag bits */
# define XI2_SLAVE_VERT			(1<<0)	/* if vert. valuator present */
# define XI2_SLAVE_HORIZ		(1<<1)	/* if horiz. valuator present */

struct xi2_master {
	int		id;			/* master device id */
	struct xi2_slave *slave;		/* currently active slave dev */
	struct xi2_master *next;
};


extern	int		xi2_opcode;
extern	Boolean		xi2_active;
extern	struct xi2_master *xi2_masters;		/* linked list of master devs */
extern	struct xi2_master *xi2_current;		/* current master device */
extern	struct xi2_slave *xi2_slaves;		/* linked list of slave devs */

extern	struct xi2_slave xi2_no_slave;		/* if no slave assigned yet */

extern	void	xi2_init_valuators(struct xi2_slave *, XIAnyClassInfo **, int);
extern	void	xi2_activate(void);

#endif /* HAVE_XI21 */


extern Boolean dvi_file_corrupted;

extern short magnifier_stat;	/* 1 = wait for expose, -1 = destroy upon expose */

extern void run_dvi_file(const char *filename, void *data);

/*
  ================================================================================
  globals from dvi-draw.c
  ================================================================================
*/

/*
 * The following is set when we're prescanning before opening up the windows,
 * and we hit a PostScript header file.  We can't start up gs until we get
 * a window to associate the process to, so we have to prescan twice.
 */
#if PS_GS
extern Boolean gs_postpone_prescan;
#endif

#if PS
extern int scanned_page_ps;	/* last page scanned for PS specials */
extern int scanned_page_ps_bak;	/* save the above if PS is turned off */
#endif

#if COLOR
extern int scanned_page_color;	/* last page scanned for color spcls */
#endif /* COLOR */

extern int scanned_page; /* last page prescanned */
extern int scanned_page_reset; 	/* number to reset the above to */
extern ubyte *G_dvi_buf_ptr;
extern struct drawinf currinf;
extern Boolean drawing_mag;
extern Boolean htex_inside_href;

/* globals from hypertex.h */
/* current anchor to search for, either from command-line or from clicking mouse */
extern char *g_anchor_pos;
extern size_t g_anchor_len;

extern char *g_link_color_rgb;
extern char *g_visited_link_color_rgb;

/* globals from special.h */
/*
 * If we're in the middle of a PSFIG special.
 */
extern Boolean psfig_begun;
/*
 * Set if the -paper option overrides papersize specials.
 */
extern Boolean ignore_papersize_specials;
extern Boolean have_raw_postscript;
#if PS
extern struct psprocs psp, no_ps_procs;
#endif

#ifdef MAGICK
/* TODO: put access functions for these in special.c */
extern int bbox_angle;
extern Boolean bbox_valid;
extern unsigned int bbox_width;
extern unsigned int bbox_height;
extern int bbox_voffset;
#endif

/* globals from statusline.h */
extern Widget statusline;


/*
 * The cursor shape in the magnifying glass is determined by which
 * window received the button press event.  Under Motif, it's mane.win,
 * under XAW, it's the parent of mane.win.
 */
#ifdef MOTIF
#  define CURSORWIN	mane.win
#else
/* # define CURSORWIN	XtWindow(globals.widgets.form_widget) */
#  define CURSORWIN	mane.win
#endif


/*
 * This was MOTIF_TIMERS in the non-k xdvi version, but since xdvik
 * also uses XtAppAddTimeOut() for Xaw, we always need to define it.
 * See the comment in events.c for further explanations.
 */
#if LD_ALLOWS_MULTIPLE_DEFINITIONS
#  define XDVI_XT_TIMER_HACK 1
#else
#  define XDVI_XT_TIMER_HACK 0
#endif

extern const char **get_paper_types(void);
extern size_t get_paper_types_size(void);

extern void register_font_popup(void);
extern void unregister_font_popup(void);

struct startup_info {
    int file_idx;
    char *page_arg;
};

#ifdef STATUSLINE
/* this is only for the initialization; the statusline will reset it to a more adequate value: */
#  define XTRA_H 17
#endif

/*
 * Generic structure for DVI scans; contains a buffer for longjmp()ing
 * out of the scanning process if it's interrupted by the user (ugh ...)
 * and a generic `void *' to scan-specific info.
 */
struct scan_info {
    jmp_buf done_env;
    void (*geom_special)(struct scan_info *info, const char *str, int str_len);
    void *data;
};

/*
 * Used by the geometry-scanning routines.
 * It passes pointers to routines to be called at certain
 * points in the dvi file, and other information.
 */
struct geom_info {
    void (*geom_box)(struct scan_info *, long, long, long, long);
    void *geom_data;
};

#define get_byte(fp)	((unsigned char)getc(fp))
#define get_lbyte(fp)	((long)get_byte(fp))

extern void get_icon_and_title(const char *filename, char **icon_name, char **title_name);
extern void set_icon_and_title(const char *icon_name, const char *title_name);
extern void reconfigure_window(Boolean fullsize, Dimension w, Dimension h,
			       Boolean save_position);
extern void set_windowsize(Dimension *ret_w, Dimension *ret_h, int add_w, int add_h, Boolean override);

#ifdef MOTIF
extern void motif_translations_hack(void);
#endif

#ifndef	MAX
# define MAX(i, j)  ((i) > (j) ? (i) : (j))
#endif

#ifndef	MIN
# define MIN(i, j)       ((i) < (j) ? (i) : (j))
#endif

#ifndef ABS
# define ABS(x)  (((x) < 0) ? (-(x)) : (x))
#endif

#define REPORT_XDVI_BUG_TEMPLATE "Please report this as a bug to:\n\
   http://sourceforge.net/tracker/?group_id=23164&atid=377580\n"

#if HAVE_ICONV && HAVE_ICONV_H
#  undef HAVE_ICONV_H
#  define HAVE_ICONV_H 1
#else
#  undef HAVE_ICONV_H
#  define HAVE_ICONV_H 0
#endif


#if HAVE_GOOD_SETSID_VFORK
#  if HAVE_VFORK_H
#    include <vfork.h>
#  endif
#else /* HAVE_GOOD_SETSID_VFORK */
/* Mac OS X 10.3 (Panther) (11/2003) doesn't allow setsid() within vfork() */
#  undef vfork
#  define vfork fork
#endif /* HAVE_GOOD_SETSID_VFORK */


/* XawtextScrollWhenNeeded has been ditched in Xaw7
   (e.g. Xfree >= 4.x); from the man page:
   
   The value XawtextScrollWhenNeeded (and whenNeeded, recognized by
   the converter), is accepted for backwards compatibilty with
   resource specifications written for the Xaw6 Text widget, but
   ignored (effectively treated as XawtextScrollNever).

   So we're forced to use `scrollAlways' here.
*/
#define XAW_SCROLL_ALWAYS XawtextScrollAlways

/* the reconfig stuff has been fixed in XFree 4.1.0, vendor release 6510 (Slackware 8.0) */
#define BROKEN_RECONFIG ((				\
	(strstr(ServerVendor(DISP), "XFree") != NULL)	\
	&& VendorRelease(DISP) >= 4000			\
	&& VendorRelease(DISP) < 4002			\
	))

/* enable the following if you're getting unresolved messages about `iconv_open' etc.: */
#if 0
#define LIBICONV_PLUG
#define iconv_open libiconv_open
#define iconv_close libiconv_close
#define iconv libiconv
#endif /* 0 */

#if HAVE_ICONV_H
typedef ICONV_CHAR_PPTR_TYPE iconv_char_pptrT;
#endif /* HAVE_ICONV_H */

#define DEVEL_MODE 0

#endif	/* XDVI_H_ */
