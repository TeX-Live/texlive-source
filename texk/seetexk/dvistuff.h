/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

static char *dvistuff_header_rcsid="$Header: /home/reed/grunwald/Iptex/drivers/RCS/dvistuff.h,v 1.11 89/02/15 16:31:56 grunwald Exp $" ;

/*
 *	Include the following from the CTex distribution
 */

#include <stdio.h>

#include "types.h"
#include "font.h"
#include "conv.h"

/*
 *	We use the following flags to keep track of shrunk fonts
 */

#define	GF_SHRUNK	GF_USR0

#define	GLYPH_SHRUNK(g) ((g) -> g_flags & GF_SHRUNK)

#define SHRUNK_GLYPH_BYTES_WIDE(g) ((((g) -> g_width + 15) >> 4) * 2)

typedef struct font DviFont;
typedef struct glyph DviGlyph;

#define MAX_GLYPH	128
#define MAX_FONTFAMILY	128

#define DEFAULT_DPI	300
#define DEFAULT_HHMARGIN	DEFAULT_DPI
#define DEFAULT_VVMARGIN	DEFAULT_DPI
#define DEFAULT_MAX_DRIFT	3

#define DEFAULT_BLACKNESS 3


struct fontinfo {
	struct font *f;		/* the font */
	i32 pspace;		/* boundary between `small & `large spaces
				   (for positive horizontal motion) */
	i32 nspace;		/* -4 * pspace, for negative motion */
	i32 vspace;		/* 5 * pspace, for vertical motion */
	int family;		/* Imagen family number (we pick one) */
};

typedef struct fontinfo DviFontInfo;

/* DVI file info */

/*
 * Units of distance are stored in scaled points, but we can convert to
 * units of 10^-7 meters by multiplying by the numbers in the preamble.
 */

/* the structure of the stack used to hold the values (h,v,w,x,y,z) */

typedef struct dvi_stack {
	i32	h;		/* the saved h */
	i32	v;		/* the saved v */
	i32	w;		/* etc */
	i32	x;
	i32	y;
	i32	z;
} DviStack;

extern DviStack dvi_current;	/* the current values of h, v, etc */
extern int	dvi_f;			/* the current font */

#define dvi_h dvi_current.h
#define dvi_v dvi_current.v
#define dvi_w dvi_current.w
#define dvi_x dvi_current.x
#define dvi_y dvi_current.y
#define dvi_z dvi_current.z



extern int	dviHH;		/* current horizontal position, in DEVs */
extern int	dviVV;		/* current vertical position, in DEVs */
#define DVI_COUNT	10	/* count0 .. count9 */
extern long	*dviCount[DVI_COUNT];	/* values of count */

extern Conv Conversion;

extern int	dviHHMargin;
extern int	dviVVMargin;
extern int	dviDPI;
extern int	dviTallestPage;
extern int	dviWidestPage;
extern int	dviTotalPages;
extern int	dviCurrentPage;
extern int	dviMaxDrift;
extern char 	*dviPrintEngine;
extern int      dviBlackness;
extern int	dviFontRotation;
extern int	dviUserMag;

extern DviFontInfo *dviCurrentFont;
extern char	*DVIFileName;
extern FILE	*dviFile;				/* user's file */
extern char	*ProgName;

extern int dviInit();
extern void dviFini();
extern void dviPreparePage( /* int */ );
extern struct glyph *dviShrinkGlyph();

void applicationPutChar( /* hh, vv, charCode */ );
DviFont *applicationNewFont( /* font structure */);
void applicationResetFont(/* font info, key */);
void applicationSetRule( /* hh, vv, height, width  : long*/ );
void applicationDoSpecial();
