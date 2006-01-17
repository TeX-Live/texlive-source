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

NOTE:
	xdvi is based on prior work, as noted in the modification history
	in xdvi.c.

\*========================================================================*/

/*
 *	Written by Eric C. Cooper, CMU
 */

#ifndef	XDVI_H
#define	XDVI_H

#define T1 1

/********************************
 *	The C environment	*
 *******************************/

#include "c-auto.h"

#ifdef __linux
#define FLAKY_SIGPOLL 1
#endif

#if STDC_HEADERS
# include <stddef.h>
# include <stdlib.h>
	/* the following works around the wchar_t problem */
# include <X11/X.h>
# if HAVE_X11_XOSDEFS_H
#  include <X11/Xosdefs.h>
# endif
# ifdef X_NOT_STDC_ENV
#  undef X_NOT_STDC_ENV
#  undef X_WCHAR
#  include <X11/Xlib.h>
#  define X_NOT_STDC_ENV
# endif
#endif

/* Avoid name clashes with kpathsea.  */
#define xfopen xdvi_xfopen

/* For wchar_t et al., that the X files might want. */
#include <kpathsea/systypes.h>
#include <kpathsea/c-memstr.h>

#include <X11/Xlib.h>	/* include Xfuncs.h, if available */
#include <X11/Xutil.h>	/* needed for XDestroyImage */
#include <X11/Xos.h>
#undef wchar_t

#if	XlibSpecificationRelease >= 5
#include <X11/Xfuncs.h>
#endif

#ifndef	NOTOOL

#include <X11/Intrinsic.h>
#if	(defined(VMS) && (XtSpecificationRelease <= 4)) || defined(lint)
#include <X11/IntrinsicP.h>
#endif
#define	TOOLKIT	1

#else	/* NOTOOL */

#define	XtNumber(arr)	(sizeof(arr)/sizeof(arr[0]))
typedef	unsigned long	Pixel;
typedef	char		Boolean;
typedef	unsigned int	Dimension;
#undef	TOOLKIT
#undef	MOTIF
#undef	BUTTONS
#undef	CFG2RES

#endif	/* NOTOOL */

#if defined(CFG2RES) && !defined(SELFAUTO)
#define	SELFAUTO 1
#endif

#if defined(SELFAUTO) && !defined(DEFAULT_CONFIG_PATH)
#define	DEFAULT_CONFIG_PATH	"$SELFAUTODIR:$SELFAUTOPARENT"
#endif

#undef CFGFILE				/* no cheating */

#if defined(DEFAULT_CONFIG_PATH)
#define	CFGFILE	1
#endif

typedef	char		Bool3;		/* Yes/No/Maybe */

#define	True	1
#define	False	0
#define	Maybe	2

#define INVALID_WIDTH 017777777777

#ifdef	VMS
#include <string.h>
#define	index	strchr
#define	rindex	strrchr
#define	bzero(a, b)	(void) memset ((void *) (a), 0, (size_t) (b))
#define bcopy(a, b, c)  (void) memmove ((void *) (b), (void *) (a), (size_t) (c))
#endif

#include <stdio.h>
#include <setjmp.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) (dirent)->d_namlen
# if HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# if HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# if HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

/* These are for X headers */
#ifndef	NeedFunctionPrototypes
#if	__STDC__
#define	NeedFunctionPrototypes	1
#else	/* STDC */
#define	NeedFunctionPrototypes	0
#endif	/* STDC */
#endif	/* NeedFunctionPrototypes */

#ifndef HAVE_PROTOTYPES
#if	__STDC__
#define HAVE_PROTOTYPES 1
#else   /* STDC */
#define HAVE_PROTOTYPES 0
#endif  /* STDC */
#endif

/* These are for xdvi */
#if	HAVE_PROTOTYPES
#define	ARGS(x)	x
#else
#define	ARGS(x)	()
#endif

#ifndef KPATHSEA

/* These macros munge function declarations to make them work in both
   cases.  The P?H macros are used for declarations, the P?C for
   definitions.  See <ansidecl.h> from the GNU C library.  P1H(void)
   also works for definitions of routines which take no args.  */

#if __STDC__

#define	P1H(p1) (p1)
#define P2H(p1,p2) (p1, p2)
#define P3H(p1,p2,p3) (p1, p2, p3)
#define P4H(p1,p2,p3,p4) (p1, p2, p3, p4)
#define P5H(p1,p2,p3,p4,p5) (p1, p2, p3, p4, p5)
#define P6H(p1,p2,p3,p4,p5,p6) (p1, p2, p3, p4, p5, p6)
#define P7H(p1,p2,p3,p4,p5,p6,p7) (p1, p2, p3, p4, p5, p6,p7)

#define	P1C(t1,n1)(t1 n1)
#define	P2C(t1,n1, t2,n2) (t1 n1, t2 n2)
#define P3C(t1,n1, t2,n2, t3,n3)(t1 n1, t2 n2, t3 n3)
#define P4C(t1,n1, t2,n2, t3,n3, t4,n4) (t1 n1, t2 n2, t3 n3, t4 n4)
#define P5C(t1,n1, t2,n2, t3,n3, t4,n4, t5,n5) \
  (t1 n1, t2 n2, t3 n3, t4 n4, t5 n5)
#define P6C(t1,n1, t2,n2, t3,n3, t4,n4, t5,n5, t6,n6) \
  (t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6)
#define P7C(t1,n1, t2,n2, t3,n3, t4,n4, t5,n5, t6,n6, t7,n7) \
  (t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7,n7)

#else /* not __STDC__ */

#define	P1H(p1) ()
#define P2H(p1, p2) ()
#define P3H(p1, p2, p3) ()
#define P4H(p1,p2,p3,p4) ()
#define P5H(p1, p2, p3, p4, p5) ()
#define P6H(p1, p2, p3, p4, p5, p6) ()
#define P6H(p1, p2, p3, p4, p5, p6, p7) ()

#define	P1C(t1,n1) (n1) t1 n1;
#define	P2C(t1,n1, t2,n2) (n1,n2) t1 n1; t2 n2;
#define P3C(t1,n1, t2,n2, t3,n3) (n1,n2,n3) t1 n1; t2 n2; t3 n3;
#define P4C(t1,n1, t2,n2, t3,n3, t4,n4) (n1,n2,n3,n4) \
  t1 n1; t2 n2; t3 n3; t4 n4;
#define P5C(t1,n1, t2,n2, t3,n3, t4,n4, t5,n5) (n1,n2,n3,n4,n5) \
  t1 n1; t2 n2; t3 n3; t4 n4; t5 n5;
#define P6C(t1,n1, t2,n2, t3,n3, t4,n4, t5,n5, t6,n6) (n1,n2,n3,n4,n5,n6) \
  t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6;
#define P7C(t1,n1, t2,n2, t3,n3, t4,n4, t5,n5, t6,n6, t7,n7) \
  (n1,n2,n3,n4,n5,n6,n7) \
  t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7,n7;

#endif /* not __STDC__ */

#endif /* not KPATHSEA */

#ifndef	NeedWidePrototypes
#define	NeedWidePrototypes	NeedFunctionPrototypes
#endif

#include <kpathsea/c-vararg.h>

#ifndef	_XFUNCPROTOBEGIN
#define	_XFUNCPROTOBEGIN
#define	_XFUNCPROTOEND
#endif

#ifndef	_Xconst
#if	__STDC__
#define	_Xconst	const
#else	/* STDC */
#define	_Xconst
#endif	/* STDC */
#endif	/* _Xconst */

#ifndef	VOLATILE
#if	__STDC__ || (defined(__stdc__) && defined(__convex__))
#define	VOLATILE	volatile
#else
#define	VOLATILE	/* nothing */
#endif
#endif

#ifndef	NORETURN
#ifdef	__GNUC__
#define	NORETURN	volatile
#else
#define	NORETURN	/* nothing */
#endif
#endif

#ifndef	OPEN_MODE
#define OPEN_MODE FOPEN_R_MODE
#endif	/* OPEN_MODE */

#ifndef	VMS
#define	OPEN_MODE_ARGS	_Xconst char *
#else
#define	OPEN_MODE_ARGS	_Xconst char *, _Xconst char *
#endif

#define	Printf	(void) printf
#define	Puts	(void) puts
#define	Fprintf	(void) fprintf
#define	Sprintf	(void) sprintf
#define	Fseek	(void) fseek
#define	Fread	(void) fread
#define	Fputs	(void) fputs
#define	Putc	(void) putc
#define	Putchar	(void) putchar
#define	Fclose	(void) fclose
#define	Fflush	(void) fflush
#define	Strcat	(void) strcat
#define	Strcpy	(void) strcpy

#ifdef HTEX
char *urlocalize P1H(char *filename);
extern int lastwwwopen;
#endif

/********************************
 *	 Types and data		*
 *******************************/

#ifndef	EXTERN
#define	EXTERN	extern
#define	INIT(x)
#endif

#define	MAXDIM		32767

typedef	unsigned char	ubyte;

#if	NeedWidePrototypes
typedef	unsigned int	wide_ubyte;
typedef	int		wide_bool;
#define	WIDENINT	(int)
#else
typedef	ubyte		wide_ubyte;
typedef	Boolean		wide_bool;
#define	WIDENINT
#endif

#if defined(MAKETEXPK) && !defined(MKTEXPK)
#define	MKTEXPK 1
#endif

/*
 *	pixel_conv is currently used only for converting absolute positions
 *	to pixel values; although normally it should be
 *		((int) ((x) / shrink_factor + (1 << 15) >> 16)),
 *	the rounding is achieved instead by moving the constant 1 << 15 to
 *	PAGE_OFFSET in dvi-draw.c.
 */
#define	pixel_conv(x)		((int) ((x) / shrink_factor >> 16))
#define	pixel_round(x)		((int) ROUNDUP(x, shrink_factor << 16))
#define	spell_conv0(n, f)	((long) (n * f))
#define	spell_conv(n)		spell_conv0(n, dimconv)

#define	BMUNIT			unsigned BMTYPE
#define	BMBITS			(8 * BMBYTES)

#define	ADD(a, b)	((BMUNIT *) (((char *) a) + b))
#define	SUB(a, b)	((BMUNIT *) (((char *) a) - b))

extern	BMUNIT	bit_masks[BMBITS + 1];

struct frame {
  	/* dvi_h and dvi_v is the horizontal and vertical baseline position
	   it is the responsebility of the set_char procedure to update
	   them. */
	struct framedata {
		long dvi_h, dvi_v, w, x, y, z;
		int pxl_v;
	} data;
	struct frame *next, *prev;
};

#ifndef	TEXXET
typedef	long	(*set_char_proc) P1H(wide_ubyte);
#else
typedef	void	(*set_char_proc) P2H(wide_ubyte, wide_ubyte);
#endif

struct drawinf {	/* this information is saved when using virtual fonts */
	struct framedata data;
	struct font	*fontp;
	set_char_proc	set_char_p;
	int		tn_table_len;
	struct font	**tn_table;
	struct tn	*tn_head;
	ubyte		*pos, *end;
	struct font	*virtual;
#ifdef	TEXXET
	int		dir;
#endif
};

EXTERN	struct drawinf	currinf;

/* entries below with the characters 'dvi' in them are actually stored in
   scaled pixel units */

#define DVI_H   currinf.data.dvi_h
#define PXL_H   pixel_conv(currinf.data.dvi_h)
#define DVI_V   currinf.data.dvi_v
#define PXL_V   currinf.data.pxl_v
#define WW      currinf.data.w
#define XX      currinf.data.x
#define YY      currinf.data.y
#define ZZ      currinf.data.z
#define ROUNDUP(x,y) (((x)+(y)-1)/(y))

EXTERN	int	current_page;
EXTERN	int	total_pages;
EXTERN	int	pageno_correct	INIT(1);
EXTERN	long	magnification;
EXTERN	double	dimconv;
EXTERN	double	tpic_conv;
EXTERN	int	n_files_left;	/* for LRU closing of fonts */
EXTERN	time_t	dvi_time;		/* last mod. time for dvi file */
EXTERN	unsigned int	page_w, page_h;

#if	defined(GS_PATH) && !defined(PS_GS)
#define	PS_GS
#endif

#if	defined(PS_DPS) || defined(PS_NEWS) || defined(PS_GS)
#define	PS	1
#else
#define	PS	0
#endif

#if	PS
EXTERN	int	scanned_page;		/* last page prescanned */
EXTERN	int	scanned_page_bak;	/* actual value of the above */
EXTERN	int	scanned_page_reset;	/* number to reset the above to */
#endif

/*
 * Table of page offsets in DVI file, indexed by page number - 1.
 * Initialized in prepare_pages().
 */
EXTERN	long	*page_offset;

/*
 * Mechanism for reducing repeated warning about specials, lost characters, etc.
 */
EXTERN	Boolean	warn_spec_now;

// added SU
EXTERN Boolean delay_src;

/*
 * If we're in the middle of a PSFIG special.
 */
EXTERN	Boolean	psfig_begun	INIT(False);

/* BEGIN CHUNK xdvi.h 1 */
#ifdef SRC_SPECIALS
EXTERN Boolean src_specials_are_evaluated INIT(False);
EXTERN Boolean src_EditorCmd_filename_first;
EXTERN int src_tickShape;
EXTERN int src_tick_height, src_tick_width;   /* size of the src tick glyphs */
#define SPECIAL_SHAPE_MAX_NUM 3		/* number of different shapes (starting from 0)
									   for drawing src specials. Hardcoding these here is a bad idea ... */
EXTERN Cursor save_cursor;
#define SRC_WARNINGS_SILENT 0
#define SRC_WARNINGS_MEDIUM 1
#define SRC_WARNINGS_VERBOSE 2
#endif
/* END CHUNK xdvi.h 1 */

/*
 * Bitmap structure for raster ops.
 */
struct bitmap {
	unsigned short	w, h;		/* width and height in pixels */
	short		bytes_wide;	/* scan-line width in bytes */
	char		*bits;		/* pointer to the bits */
};

/*
 * Per-character information.
 * There is one of these for each character in a font (raster fonts only).
 * All fields are filled in at font definition time,
 * except for the bitmap, which is "faulted in"
 * when the character is first referenced.
 */
struct glyph {
	long addr;		/* address of bitmap in font file */
	long dvi_adv;		/* DVI units to move reference point */
	short x, y;		/* x and y offset in pixels */
	struct bitmap bitmap;	/* bitmap for character */
	short x2, y2;		/* x and y offset in pixels (shrunken bitmap) */
#ifdef	GREY
	XImage *image2;
	char *pixmap2;
	char *pixmap2_t;
#endif
	struct bitmap bitmap2;	/* shrunken bitmap for character */
};

/*
 * Per character information for virtual fonts
 */
struct macro {
	ubyte	*pos;		/* address of first byte of macro */
	ubyte	*end;		/* address of last+1 byte */
	long	dvi_adv;	/* DVI units to move reference point */
	Boolean	free_me;	/* if free(pos) should be called when */
				/* freeing space */
};

/*
 * The layout of a font information block.
 * There is one of these for every loaded font or magnification thereof.
 * Duplicates are eliminated:  this is necessary because of possible recursion
 * in virtual fonts.
 *
 * Also note the strange units.  The design size is in 1/2^20 point
 * units (also called micro-points), and the individual character widths
 * are in the TFM file in 1/2^20 ems units, i.e., relative to the design size.
 *
 * We then change the sizes to SPELL units (unshrunk pixel / 2^16).
 */

#define	NOMAGSTP (-29999)

typedef	void (*read_char_proc) ARGS((register struct font *, wide_ubyte));

struct font {
  struct font *next;		/* link to next font info block */
  char *fontname;		/* name of font */
  float fsize;			/* size information (dots per inch) */
  int magstepval;		/* magstep number * two, or NOMAGSTP */
  FILE *file;			/* open font file or NULL */
  char *filename;		/* name of font file */
  long checksum;		/* checksum */
  unsigned short timestamp;	/* for LRU management of fonts */
  ubyte flags;			/* flags byte (see values below) */
#ifdef Omega
  wide_ubyte maxchar;		/* largest character code */
#else
  ubyte maxchar;		/* largest character code */
#endif
/* BEGIN CHUNK xdvi.h 2 */
#ifdef SRC_SPECIALS  
    float pt_size;					/* font size in pt, needed for specials evaluation */
#endif  
/* END CHUNK xdvi.h 2 */
  double dimconv;		/* size conversion factor */
  set_char_proc set_char_p;	/* proc used to set char */
  /* these fields are used by (loaded) raster fonts */
  read_char_proc read_char;	/* function to read bitmap */
  struct glyph *glyph;
  /* these fields are used by (loaded) virtual fonts */
  struct font **vf_table;	/* list of fonts used by this vf */
  struct tn *vf_chain;		/* ditto, if TeXnumber >= VFTABLELEN */
  struct font *first_font;	/* first font defined */
  struct macro *macro;
  /* I suppose the above could be put into a union, but we */
  /* wouldn't save all that much space. */

  /* These were added for t1 use */
  int t1id;
  long scale;
};

#define	FONT_IN_USE	1	/* used for housekeeping */
#define	FONT_LOADED	2	/* if font file has been read */
#define	FONT_VIRTUAL	4	/* if font is virtual */

#define	TNTABLELEN	30	/* length of TeXnumber array (dvi file) */
#define	VFTABLELEN	5	/* length of TeXnumber array (virtual fonts) */

struct tn {
	struct tn *next;		/* link to next TeXnumber info block */
	int TeXnumber;			/* font number (in DVI file) */
	struct font *fontp;		/* pointer to the rest of the info */
};

EXTERN	struct font	*tn_table[TNTABLELEN];
EXTERN	struct font	*font_head	INIT(NULL);
EXTERN	struct tn	*tn_head	INIT(NULL);
#ifdef Omega
EXTERN	wide_ubyte		maxchar;
#else
EXTERN	ubyte		maxchar;
#endif
EXTERN	unsigned short	current_timestamp INIT(0);

/*
 *	Command line flags.
 */

extern	struct _resource {
#if CFGFILE && TOOLKIT
	_Xconst char	*progname;
#endif
#if TOOLKIT
	int		shrinkfactor;
#endif
	int		_density;
#ifdef	GREY
	float		_gamma;
#endif
	int		_pixels_per_inch;
	Boolean		_delay_rulers;
	int		_tick_length;
	char		*_tick_units;
	_Xconst	char	*sidemargin;
	_Xconst	char	*topmargin;
	_Xconst	char	*xoffset;
	_Xconst	char	*yoffset;
	_Xconst	char	*paper;
	_Xconst	char	*_alt_font;
	Boolean		makepk;
	_Xconst	char	*mfmode;
	Boolean		_list_fonts;
	Boolean		reverse;
	Boolean		_warn_spec;
	Boolean		_hush_chars;
	Boolean		_hush_chk;
	Boolean		safer;
#if defined(VMS) || !defined(TOOLKIT)
	_Xconst	char	*fore_color;
	_Xconst	char	*back_color;
#endif
	Pixel		_fore_Pixel;
	Pixel		_back_Pixel;
#ifdef TOOLKIT
	Pixel		_brdr_Pixel;
	Pixel		_hl_Pixel;
	Pixel		_cr_Pixel;
#endif
	_Xconst	char	*icon_geometry;
	Boolean		keep_flag;
	Boolean		copy;
	Boolean		thorough;
#if	PS
	/* default is to use DPS, then NEWS, then GhostScript;
	 * we will figure out later on which one we will use */
	Boolean		_postscript;
	Boolean		prescan;
	Boolean		allow_shell;
#ifdef	PS_DPS
	Boolean		useDPS;
#endif
#ifdef	PS_NEWS
	Boolean		useNeWS;
#endif
#ifdef	PS_GS
	Boolean		useGS;
	Boolean		gs_safer;
	Boolean		gs_alpha;
	_Xconst	char	*gs_path;
	_Xconst	char	*gs_palette;
#endif
#endif	/* PS */
	_Xconst	char	*debug_arg;
	Boolean		version_flag;
#ifdef	BUTTONS
	Boolean		expert;
	int		shrinkbutton[4];
#endif
	_Xconst	char	*mg_arg[5];
#ifdef	GREY
	Boolean		_use_grey;
	Bool3		install;
#endif
#ifdef GRID
	int _grid_mode;
	char *grid1_color, *grid2_color, *grid3_color;
#ifdef	TOOLKIT
	Pixel	_grid1_Pixel, _grid2_Pixel, _grid3_Pixel;
#endif /* TOOLKIT */
#endif /* GRID */
#ifdef HTEX
	Boolean	_underline_link;
	char	*_browser;
	char	*_URLbase;
	char	*_scroll_pages;
#endif /* HTEX */
/* BEGIN CHUNK xdvi.h 3 */
#ifdef SRC_SPECIALS
  int _src_specialFormat;
  int _src_jumpButton;
  int _src_tickShape;
  char	*_src_tickSize;
  int _src_cursor_shape;
  char	*_src_editorCommand;
  int _src_warn_verbosity;
  Boolean _src_tickVisibility;
  Boolean _src_evalMode;
#endif  
/* END CHUNK xdvi.h 3 */
} resource;

/* As a convenience, we define the field names without leading underscores
 * to point to the field of the above record.  Here are the global ones;
 * the local ones are defined in each module.  */

/* BEGIN CHUNK xdvi.h 4 */
#ifdef SRC_SPECIALS
#define src_cursor_shape  	resource._src_cursor_shape
#define src_editorCommand  	resource._src_editorCommand
#define src_warn_verbosity 	resource._src_warn_verbosity
#define src_tickShape      	resource._src_tickShape
#define src_tickVisibility 	resource._src_tickVisibility
#define src_evalMode       	resource._src_evalMode
#endif
/* END CHUNK xdvi.h 4 */
#define	density		resource._density
#define	pixels_per_inch	resource._pixels_per_inch
#define	alt_font	resource._alt_font
#define	list_fonts	resource._list_fonts
#define	warn_spec	resource._warn_spec
#define	hush_chars	resource._hush_chars
#define	hush_chk	resource._hush_chk
#ifdef  GREY
#define	use_grey	resource._use_grey
#endif
#ifdef GRID
#define grid_mode	resource._grid_mode
#endif /* GRID */
#ifdef HTEX
#define underline_link	resource._underline_link
#define browser		resource._browser
#define URLbase		resource._URLbase
#define scroll_pages	resource._scroll_pages
#define KPSE_DEBUG_HYPER	6
#endif

#ifndef TOOLKIT
EXTERN	Pixel		brdr_Pixel;
#ifdef	GRID
EXTERN	Pixel		grid1_Pixel;
EXTERN	Pixel		grid2_Pixel;
EXTERN	Pixel		grid3_Pixel;
#endif /* GRID */
#endif



#ifdef GREY
EXTERN	Pixel		plane_masks[4];
EXTERN	XColor		fore_color_data, back_color_data;
#endif

extern	struct	mg_size_rec {
	int		w;
	int		h;
}
	mg_size[5];

EXTERN	int		debug		INIT(0);

#define	DBG_BITMAP	1
#define	DBG_DVI		2
#define	DBG_PK		4
#define	DBG_BATCH	8
#define	DBG_EVENT	16
#define	DBG_OPEN	32
#define	DBG_PS		64
#define	DBG_STAT	128
#define	DBG_HASH	256
#define	DBG_PATHS	512
#define	DBG_EXPAND	1024
#define	DBG_SEARCH	2048
#ifdef HTEX
#define	DBG_HYPER	4096
#define	DBG_ANCHOR	8192
#endif
/* BEGIN CHUNK xdvi.h 5 */
#ifdef SRC_SPECIALS
#define DBG_SRC_SPECIALS     16384
#endif
/* END CHUNK xdvi.h 5 */
#define	DBG_ALL		(~DBG_BATCH)


EXTERN	int		offset_x, offset_y;
EXTERN	unsigned int	unshrunk_paper_w, unshrunk_paper_h;
EXTERN	unsigned int	unshrunk_page_w, unshrunk_page_h;
#ifdef GRID
EXTERN  unsigned int	unshrunk_paper_unit;
#endif /* GRID */

#ifdef HTEX
EXTERN	int	HTeXnext_extern; /* Bring up next file in new xdvi window */
EXTERN	char	*temporary_dir	INIT(NULL);
EXTERN	char	*anchor_name	INIT(NULL);
EXTERN	Boolean	URL_aware	INIT(False);
EXTERN	Boolean	highlight	INIT(False);
extern int HTeXAnestlevel;	/* Hypertext nesting level */
extern int HTeXreflevel;	/* flag for whether we are inside an href */
#endif

EXTERN	char		*dvi_name	INIT(NULL);
EXTERN	FILE		*dvi_file;		/* user's file */
EXTERN	_Xconst char	*prog;
EXTERN	int		bak_shrink;		/* last shrink factor != 1 */
EXTERN	Dimension	window_w, window_h;
EXTERN	XImage		*image;
EXTERN	int		backing_store;
EXTERN	int		home_x, home_y;

EXTERN	Display		*DISP;
EXTERN	Screen		*SCRN;
#ifdef GREY
EXTERN	int		screen_number;
EXTERN	Visual		*our_visual;
EXTERN	unsigned int	our_depth;
EXTERN	Colormap	our_colormap;
EXTERN	GC		copyGC;
#else
#define	our_depth	(unsigned int) DefaultDepthOfScreen(SCRN)
#define	our_visual	DefaultVisualOfScreen(SCRN)
#define	our_colormap	DefaultColormapOfScreen(SCRN)
#define	copyGC		DefaultGCOfScreen(SCRN)
#endif
EXTERN	GC		ruleGC;
EXTERN	GC		foreGC, highGC;
EXTERN	GC		foreGC2;
#ifdef GRID
EXTERN  GC      grid1GC, grid2GC, grid3GC;
#endif /* GRID */
EXTERN	Boolean		copy;

EXTERN	Cursor		redraw_cursor, ready_cursor;
/* BEGIN CHUNK xdvi.h 6 */
#ifdef SRC_SPECIALS
EXTERN	Cursor		src_cursor;
#endif
/* END CHUNK xdvi.h 6 */

#ifdef	GREY
EXTERN	Pixel		*pixeltbl;
EXTERN	Pixel		*pixeltbl_t;
#endif	/* GREY */

EXTERN	Boolean		canit		INIT(False);
EXTERN	jmp_buf		canit_env;
EXTERN	VOLATILE short	event_counter	INIT(0);
EXTERN	Boolean		terminate_flag	INIT(False);

struct	WindowRec {
	Window		win;
	int		shrinkfactor;
	int		base_x, base_y;
	unsigned int	width, height;
	int	min_x, max_x, min_y, max_y;	/* for pending expose events */
};

extern	struct WindowRec mane, alt, currwin;
EXTERN	int		min_x, max_x, min_y, max_y;

#define	shrink_factor	currwin.shrinkfactor

#ifdef	TOOLKIT
EXTERN	Widget		top_level, vport_widget, draw_widget, clip_widget;
#ifdef HTEX
EXTERN	Widget	pane_widget, anchor_search, anchor_info;
#endif
#ifdef MOTIF
EXTERN	Widget		shrink_button[4];
EXTERN	Widget		x_bar, y_bar;	/* horizontal and vert. scroll bars */
#endif /* MOTIF */
#ifdef	BUTTONS
#ifndef MOTIF
#define	XTRA_WID	79
#else
#define	XTRA_WID	120
#endif
EXTERN	Widget	form_widget;
#endif
#else	/* !TOOLKIT */
EXTERN	Window	top_level;

#define	BAR_WID		12	/* width of darkened area */
#define	BAR_THICK	15	/* gross amount removed */
#endif	/* TOOLKIT */

EXTERN	jmp_buf		dvi_env;	/* mechanism to relay dvi file errors */
EXTERN	_Xconst	char	*dvi_oops_msg;	/* error message */

EXTERN	char	*ffline	INIT(NULL);	/* an array used by filefind to store */
					/* the file name being formed.  */
					/* It expands as needed. */
					/* Also used elsewhere.  */
EXTERN	int	ffline_len INIT(0);	/* current length of ffline[] */

#ifdef	SELFAUTO
EXTERN	_Xconst	char	*argv0;		/* argv[0] */
#endif

#ifdef	CFG2RES
struct cfg2res {
	_Xconst	char	*cfgname;	/* name in config file */
	_Xconst	char	*resname;	/* name of resource */
	Boolean		numeric;	/* if numeric */
};
#endif

#if	PS
	
extern	struct psprocs	{
	void	(*toggle)	ARGS((void));
	void	(*destroy)	ARGS((void));
	void	(*interrupt)	ARGS((void));
	void	(*endpage)	ARGS((void));
	void	(*drawbegin)	ARGS((int, int, _Xconst char *));
	void	(*drawraw)	ARGS((_Xconst char *));
	void	(*drawfile)	ARGS((_Xconst char *, FILE *));
	void	(*drawend)	ARGS((_Xconst char *));
	void	(*beginheader)	ARGS((void));
	void	(*endheader)	ARGS((void));
	void	(*newdoc)	ARGS((void));
}	psp, no_ps_procs;

#endif	/* PS */

/********************************
 *	   Procedures		*
 *******************************/

_XFUNCPROTOBEGIN
#ifdef	BUTTONS
extern	void	create_buttons ARGS((XtArgVal));
#endif
#ifdef	GREY
extern	void	init_plane_masks ARGS((void));
#endif
extern	void	init_colors ARGS((void));
extern	void	reconfig ARGS((void));
#ifdef	TOOLKIT
extern	void	handle_key ARGS((Widget, XtPointer, XEvent *, Boolean *));
extern	void	handle_resize ARGS((Widget, XtPointer, XEvent *, Boolean *));
extern	void	handle_button ARGS((Widget, XtPointer, XEvent *, Boolean *));
extern	void	handle_motion ARGS((Widget, XtPointer, XEvent *, Boolean *));
extern	void	handle_release ARGS((Widget, XtPointer, XEvent *, Boolean *));
extern	void	handle_exp ARGS((Widget, XtPointer, XEvent *, Boolean *));
#endif
#ifdef MOTIF
extern	void	file_pulldown_callback ARGS((Widget, XtPointer, XtPointer));
extern	void	navigate_pulldown_callback ARGS((Widget, XtPointer, XtPointer));
extern	void	scale_pulldown_callback ARGS((Widget, XtPointer, XtPointer));
extern	void	set_shrink_factor ARGS((int));
#endif
extern	void	showmessage ARGS((_Xconst char *));
#if	PS
extern	void	ps_read_events ARGS((wide_bool, wide_bool));
#define	read_events(wait)	ps_read_events(wait, True)
#else
extern	void	read_events ARGS((wide_bool));
#endif
extern	void	redraw_page ARGS((void));
extern	void	do_pages ARGS((void));
extern	void	reset_fonts ARGS((void));
extern	void	realloc_font ARGS((struct font *, wide_ubyte));
extern	void	realloc_virtual_font ARGS((struct font *, wide_ubyte));
extern	Boolean	load_font ARGS((struct font *));
extern	struct font	*define_font ARGS((FILE *, wide_ubyte,
			struct font *, struct font **, unsigned int,
			struct tn **));
extern	void	init_page ARGS((void));
extern	void	open_dvi_file ARGS((void));
extern	Boolean	check_dvi_file ARGS((void));
#ifdef GRID
extern	void	put_grid ARGS((int, int, unsigned int, unsigned int, unsigned int, GC, GC, GC));
#endif /* GRID */
#ifndef	TEXXET
extern	long	set_char ARGS((wide_ubyte));
extern	long	load_n_set_char ARGS((wide_ubyte));
extern	long	set_vf_char ARGS((wide_ubyte));
extern	long	set_t1_char P1H(wide_ubyte);
#else
extern	void	set_char ARGS((wide_ubyte, wide_ubyte));
extern	void	load_n_set_char ARGS((wide_ubyte, wide_ubyte));
extern	void	set_vf_char ARGS((wide_ubyte, wide_ubyte));
extern	void	set_t1_char P2H(wide_ubyte, wide_ubyte);
#endif
extern	int	tfmload P3H(char *, long *, long *);
extern	void	read_T1_char P2H(struct font *, ubyte);
extern  int	find_T1_font P1H(char *);
extern  int	getpsinfo P1H(char *);
extern	void	draw_page P1H(void);
extern  void	init_t1 P1H(void);
#if	CFGFILE
#ifndef	CFG2RES
extern	void	readconfig ARGS((void));
#else
extern	void	readconfig ARGS((_Xconst struct cfg2res *,
		  _Xconst struct cfg2res *, XtResource *, XtResource *));
#endif	/* CFG2RES */
#endif	/* CFGFILE */
extern	void	init_font_open ARGS((void));
extern	FILE	*font_open P7H(char *, char **, double, int *, int,
			char **,int *);
#if	PS
extern	void	ps_newdoc ARGS((void));
extern	void	ps_destroy ARGS((void));
#endif
extern	void	applicationDoSpecial ARGS((char *));
#if	PS
extern	void	scan_special ARGS((char *));
#endif
extern	NORETURN void	oops ARGS((_Xconst char *message, ...));
#ifndef KPATHSEA
extern	void	*xmalloc ARGS((unsigned));
extern	void	*xrealloc ARGS((void *, unsigned));
extern	char	*xstrdup ARGS((_Xconst char *, int));
extern	char	*xmemdump ARGS((_Xconst char *, int));
#endif
extern	void	expandline ARGS((int));
extern	void	alloc_bitmap ARGS((struct bitmap *));
#ifndef KPATHSEA
extern	void	xputenv ARGS((_Xconst char *, _Xconst char *));
#endif
extern	int	memicmp ARGS((_Xconst char *, _Xconst char *, size_t));
extern	FILE	*xfopen P2H(char *, OPEN_MODE_ARGS);
#ifdef HTEX
extern	FILE	*xfopen_local P2H(char *, OPEN_MODE_ARGS);
#else
#define xfopen_local	xfopen
#endif
extern	int	xpipe ARGS((int *));
extern	DIR	*xdvi_xopendir ARGS((_Xconst char *));
extern	_Xconst	struct passwd *ff_getpw ARGS((_Xconst char **, _Xconst char *));
extern	unsigned long	num ARGS((FILE *, int));
extern	long	snum ARGS((FILE *, int));
extern	void	read_PK_index ARGS((struct font *, wide_bool));
extern	void	read_GF_index ARGS((struct font *, wide_bool));
#ifdef Omega
extern	unsigned long read_VF_index ARGS((struct font *, wide_bool));
#else
extern	void	read_VF_index ARGS((struct font *, wide_bool));
#endif

#if	PS
extern	void	drawbegin_none ARGS((int, int, _Xconst char *));
extern	void	draw_bbox ARGS((void));
extern	void	NullProc ARGS((void));
#ifdef	PS_DPS
extern	Boolean	initDPS ARGS((void));
#endif
#ifdef	PS_NEWS
extern	Boolean	initNeWS ARGS((void));
#endif
#ifdef	PS_GS
extern	Boolean	initGS ARGS((void));
#endif
#endif	/* PS */
#ifdef HTEX
extern	int	open_www_file ARGS((void));
extern	void	htex_can_it ARGS((void));
extern	void	search_callback ARGS((Widget, XtPointer, XtPointer));
extern	void	detach_anchor ARGS((void));
extern	char	*MyStrAllocCopy ARGS((char **, char *));
extern	int	pointerlocate ARGS((int *, int *));
extern	void	htex_recordbits ARGS((int, int, int, int));
extern	void	htex_initpage ARGS((void));
extern	void	htex_donepage ARGS((int, int));
extern	void	htex_parsepages ARGS((void));
extern	void	htex_parse_page ARGS((int));
extern	void	htex_scanpage ARGS((int));
extern	void	htex_scanpage ARGS((int));
extern	void	htex_dospecial ARGS((long, int));
extern	void	htex_reinit ARGS((void));
extern	void	htex_do_loc ARGS((char *));
extern	void	add_search ARGS((char *, int));
extern	int	htex_handleref ARGS((int, int, int));
extern	void	htex_displayanchor ARGS((int, int, int));
extern	void	htex_goback ARGS((void));
extern	int	checkHyperTeX ARGS((char *, int));
extern	void	htex_handletag ARGS((char *, int));
extern	void	htex_anchor ARGS((int, char *, int));
extern	void	htex_dohref ARGS((char *));
extern	void	htex_drawboxes ARGS((void));
extern	void	htex_to_page ARGS((int));
extern	void	htex_to_anchor ARGS((int, int));
extern	int	htex_is_url ARGS((const char *));
extern	void	htex_do_url ARGS((char *));
extern  void 	paint_anchor ARGS((char *));
extern	int	fetch_relative_url ARGS((char *, const char *, char *));
extern	void	wait_for_urls ARGS((void));
char *figure_mime_type ARGS((char *));
#endif

/* BEGIN CHUNK xdvi.h 7 */
#ifdef SRC_SPECIALS
/* functions in src_special.c */
extern void src_find_special ARGS((int, Position, Position));
extern void src_delete_all_specials ARGS((void));
extern void src_warn_outdated ARGS((void));
extern void src_eval_special ARGS((char *, Position, Position));
extern void src_check_editorCommand ARGS((void));
extern void src_cleanup ARGS((void));
// extern void src_do_special ARGS((float));
extern float src_look_forward ARGS((double));
#endif
/* END CHUNK xdvi.h 7 */
#if !HAVE_TEMPNAM
char *tempnam ARGS((char *, char *))
#endif

_XFUNCPROTOEND

#define one(fp)		((unsigned char) getc(fp))
#define sone(fp)	((long) one(fp))
#define two(fp)		num (fp, 2)
#define stwo(fp)	snum(fp, 2)
#define four(fp)	num (fp, 4)
#define sfour(fp)	snum(fp, 4)

#ifdef HTEX
/* From xhdvi/url.h */
/* Some URL stuff: */
typedef struct {
	char *url;    /* Full address of the URL we have locally */
	char *file;   /* Local file name it is stored as */
} FiletoURLconv;

#define FILELISTCHUNK 20
EXTERN FiletoURLconv *filelist INIT(NULL);
EXTERN int nURLs INIT(0);

#endif /* HTEX */

#endif	/* XDVI_H */
