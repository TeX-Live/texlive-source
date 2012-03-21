/* Copyright (c) 1987, 1989, 2012 University of Maryland Department of
   Computer Science.
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation, the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions: The above copyright notice, this permission
   notice and the disclaimer statement shall be included in all copies
   or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _MCTEX_FONT_
#define _MCTEX_FONT_

/*
 * Font file information, readers, etc.
 */

#ifndef _MCTEX_TYPES_
#include "types.h"
#endif

/*
 * First, font independent information: per glyph info, and per font
 * info.
 */
struct glyph {
	short	g_flags;	/* see below */
	/*
	 * The following cannot be used with GetRasterlessFont
	 */
	short	g_rotation;	/* see below */
	char	*g_raster;	/* raster, if known */
	/*
	 * These, however, do come with rasterless fonts,
	 * even though they relate only to the raster.
	 */
	i32	g_height;	/* height of bounding box */
	i32	g_width;	/* width of bounding box */
	i32	g_yorigin;	/* y origin (>= 0 -> within box) */
	i32	g_xorigin;	/* x origin (>= 0 -> within box) */
	/*
	 * These of course come with every font.
	 */
	i32	g_rawtfmwidth;	/* raw TFM width (in FIXes) */
	i32	g_tfmwidth;	/* TFM width in scaled points */
	/*
	 * The X and Y escapements come from PK and GF fonts.
	 * They are in scaled pixels (65536 scaled pixels = 1 pixel).
	 * If they are not defined by the font, these are set to
	 * NO_ESCAPEMENT.
	 */
	i32	g_xescapement;	/* x escapement (`chardx') */
	i32	g_yescapement;	/* y escapement (`chardy') */
#define	NO_ESCAPEMENT	((i32)0x89abcdef)
	/*
	 * This is provided purely for DVI to device conversion programs.
	 */
	int	g_pixwidth;	/* width in pixels */
	/*
	 * Mainly for internal use, index is the glyph index within the
	 * font.  That is, f->f_gly[i]->g_index == i.
	 */
	int	g_index;	/* character index */
	/*
	 * g_details and g_integer are purely for the font reading
	 * subroutines to use however they will.  g_next makes lists
	 * of glyphs while the glyphs are free.
	 */
	union {			/* various options */
		char	*g_details;	/* details: arbitrary */
		i32	g_integer;	/* 32 bit integer */
		struct	glyph *g_next;	/* linked list */
	} g_un;
};

/*
 * Glyph flags.
 */
#define	GF_VALID	0x0001	/* glyph is `real' */
#define	GF_SEEN		0x0002	/* glyph has been encountered/loaded */

#define	GF_USR0		0x0100	/* reserved to user code */
#define	GF_USR1		0x0200	/* reserved to user code */
#define	GF_USR2		0x0400	/* reserved to user code */
#define	GF_USR3		0x0800	/* reserved to user code */

/*
 * Rotations are in quarter-pi steps, counterclockwise of course.
 * This order must be maintained; see rotate.c.
 */
#define	ROT_NORM	0		/* no rotation: `normal' position */
#define	ROT_LEFT	1		/* 1/4 turn counterclockwise */
#define	ROT_DOWN	2		/* 1/2 turn, i.e., upside-down */
#define	ROT_RIGHT	3		/* 3/4 turn ccw, or 1/4 turn cw */

struct font {
	int	f_flags;	/* see below */
	struct	fontops *f_ops;	/* operations */
	/*
	 * f_un is provided for DVI drivers, which typically need
	 * some way to translate DVI font numbers into local values.
	 * Most seem to need integers.
	 */
	union {
		int	f_int;	/* value if int */
		i32	f_i32;	/* value if i32 */
		char	*f_ptr;	/* value if pointer */
	} f_un;
#ifdef notdef
	/* for LRU cacheing; not yet implemented */
	i32	f_lastuse;
#endif
	/*
	 * f_details is provided for font reading subroutines.
	 * It is intended to be cast to a pointer to a structure
	 * that is allocated by those routines, and used to keep
	 * track of whatever information those routines need to
	 * determine glyph boxes and (if asked for) rasters.
	 */
	char	*f_details;	/* type dependent stuff */
	/*
	 * f_path is the full pathname to the font file, filled in
	 * by GetFont and GetRasterlessFont.  Note that since we
	 * hold on to the path until the font is freed, it should be
	 * possible to cache glyph accesses on memory-poor machines.
	 */
	char	*f_path;	/* font file pathname */
	/*
	 * f_dvimag and f_dvidsz are the magnification and design size
	 * values from the DVI file.  f_font and f_scaled correspond to
	 * TeX's idea of the proper name for the font (e.g., `cmr10',
	 * `cmbx10 scaled 1440').  (Note that f_scaled is just the
	 * ratio of f_dvimag and f_dvidsz; you could save a bit of memory
	 * by eliminating it and altering the routine Font_TeXName()).
	 * f_design_size is the font's design size, or 0 for fake (TFM)
	 * fonts.  f_checksum should be set by the font reading routines
	 * to the font checksum.  If the value is nonzero, it will be
	 * compared to the checksum in the DVI file.
	 */
	i32	f_dvimag;	/* magnification from DVI file */
	i32	f_dvidsz;	/* design size from DVI file */
	char	*f_font;	/* TeX's name for the font */
	int	f_scaled;	/* the ratio of dvimag to dvidsz, x1000 */
	i32	f_design_size;	/* font design size */
	i32	f_checksum;	/* font checksum, or 0 */
	/*
	 * The three values f_pspace, f_nspace, and f_vspace correspond
	 * to the amount of positive, negative, and vertical space that
	 * limits `kerning' (if horizontal) or `in-line motion' (vertical).
	 * f_pspace is equal to 1/6 of f_dvimag, and f_nspace and f_vspace
	 * are -4 and 5 times this value respectively.  DVItype puts it
	 * this way (slightly edited):
	 *
	 *	Rounding to the nearest pixel is best done in [this]
	 *	manner ... so as to be inoffensive to the eye:  When [a]
	 *	horizontal motion is small, like a kern, [the device
	 *	position] changes by rounding the kern; but when the
	 *	motion is large, [the device position] changes by rounding
	 *	the [DVI position] so that accumulated rounding errors
	 *	disappear.  We allow a larger space in the negative
	 *	direction than in the positive one, because TeX makes
	 *	comparatively large backspaces when it positions accents.
	 *
	 *	Vertical motion is done similarly, but with the threshold
	 *	between ``small'' and ``large'' increased by a factor of
	 *	five. The idea is to make fractions like ``$1\over2$''
	 *	round consistently, but to absorb accumulated rounding
	 *	errors in the baseline-skip moves.
	 *
	 * There is, however, an exception:
	 *
	 *	A sequence of consecutive rules, or consecutive characters
	 *	in a fixed-width font whose width is not an integer number
	 *	of pixels, can cause [the device position] to drift far
	 *	away from a correctly rounded value.
	 *
	 * The drift correction is applied after every horizontal or
	 * vertical motion, including any resulting from printing a
	 * character.
	 */
	i32	f_pspace;	/* limit on positive kerns */
	i32	f_nspace;	/* limit on negative kerns */
	i32	f_vspace;	/* limit on vertical `kerns' */

	/*
	 * If set, f_hppp and f_vppp tell how many pixels there are
	 * per scaled point in the font.  For instance, a 300 dpi font
	 * has 300pixels/1inch * 1inch/72.27pt * 65536pt/1sp ~=
	 * 272046 pixels/sp.
	 *
	 * If they are zero, the font does not tell us.
	 */
	i32	f_hppp;		/* horizontal pixels per point */
	i32	f_vppp;		/* vertical pixels per point */
	/*
	 * f_lowch and f_highch bound the region in which f_gly
	 * indicies are valid.  Specificially, f_gly[i] may be
	 * read or written if and only if i is in the half-open
	 * interval [f_lowch..f_highch).  f_gly is an array of
	 * pointers to glyph structures.  The structures themselves
	 * are not allocated until requested.
	 *
	 * f_glybase is the actual return from malloc(), since it
	 * is theoretically possible for f_gly-f_lowch to become
	 * NULL.
	 */
	int	f_lowch;	/* first character */
	int	f_highch;	/* last character, plus 1 */
	struct	glyph **f_gly;	/* glyphs */
	struct	glyph **f_glybase;
};

/*
 * Macros for rounding DVI and device motions per DVItype rules.
 */
#define	F_SMALLH(f, m) ((m) < (f)->f_pspace && (m) > (f)->f_nspace)
#define	F_SMALLV(f, m) ((m) < (f)->f_vspace && -(m) < (f)->f_vspace)

/*
 * Font flags.
 */
#define	FF_RASTERS	0x0001	/* font has rasters */
#define	FF_USR0		0x0100	/* reserved to user code */
#define	FF_USR1		0x0200	/* reserved to user code */
#define	FF_USR2		0x0400	/* reserved to user code */
#define	FF_USR3		0x0800	/* reserved to user code */

/*
 * Operations on fonts.
 *
 * The `fo_dpitomag' field is used as a multiplier for a desired
 * resolution in dots per inch.  The result of the multiplication
 * is converted to a font name by multipying by 1000.0 and rounding.
 * The idea is that PXL files will have a multiplier of 5.0, and
 * others will have a multiplier of 1.0.  This suffices for the
 * present, at any rate; in the future, this field may be replaced
 * with something more general.
 *
 * N.B.: more operations may be added as they are discovered to be
 * useful.
 */
struct	fontops {
	char	*fo_name;		/* name, e.g., "gf" */
	int	fo_fakefont;		/* 1="box", 2="blank"; else 0 */
	double	fo_dpitomag;		/* multiplier */
	int	(*fo_read)();		/* open and read the font itself */
	int	(*fo_getgly)();		/* obtain specified glyphs (range) */
#ifdef notdef
	void	(*fo_freegly)();	/* release specified glyphs */
#endif
	int	(*fo_rasterise)();	/* rasterise specified glyphs */
	void	(*fo_freefont)();	/* discard font (free details) */
	struct	fontops *fo_next;	/* purely for font.c */
};

/*
 * Return a pointer to the glyph information for character `c' in
 * font `f'.
 */
#define	GLYPH(f, c) \
	((c) < (f)->f_lowch || (c) >= (f)->f_highch ? (struct glyph *)0 : \
	 ((f)->f_gly[c] ? (f)->f_gly[c] : GetGlyph(f, (int)(c))))

/*
 * True iff glyph `g' is valid.  Useful for checking return values
 * from GLYPH().
 */
#define	GVALID(g)	((g) && ((g)->g_flags & GF_VALID))

/*
 * True iff glyph g has a raster.
 */
#define	HASRASTER(g)	((g)->g_height && (g)->g_width)

/*
 * Return a pointer to the raster information for glyph `g' in font
 * `f' at rotation `r'.
 */
#define	RASTER(g, f, r)	((g)->g_rotation == (r) && (g)->g_raster ? \
			 (g)->g_raster : GetRaster(g, f, r))

/*
 * Function types.
 */
struct	font *GetFont(char *nm, i32 dvimag, i32 dvidsz, char *dev, char **fname);
struct	font *GetRasterlessFont(char *nm, i32 dvimag, i32 dvidsz, char *dev, char **fname);
struct	glyph *GetGlyph();
char	*GetRaster();
void	FreeFont();
void	FreeGlyph();
void	FreeRaster();
char	*Font_TeXName();
void	fontinit(char *file);

/*
 * Normally from stdio.h
 */
#ifndef NULL
#define	NULL	0
#endif

/*
 * The following environment variable overrides the default font
 * configuration file.  That default is used when fontinit() is not
 * called, or is passed a null pointer.
 */
#define	CONFENV	"TEXFONTDESC"

#endif /* _MCTEX_FONT_ */
