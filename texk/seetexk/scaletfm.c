/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/scaletfm.c,v 3.2 89/11/23 16:34:43 chris Exp $";
#endif

#include "types.h"
#include "error.h"
#include "font.h"

/*
 * From DVITYPE.WEB:
 *
 * ``The most important part of in_TFM is the width computation, which
 *   involvles multiplying the relative widths in the TFM file by the scaling
 *   factor in the DVI file.  This fixed-point multiplication must be done with
 *   precisely the same accuracy by all DVI-reading programs, in order to
 *   validate the assumptions made by DVI-writing programs like \TeX 82.
 *
 *   Let us therefore summarize what needs to be done.  Each width in a TFM
 *   file appears as a four-byte quantity called a fix_word.  A fix_word whose
 *   respective bytes are (a,b,c,d) represents the number
 *
 *	   {{ b * 2^{-4} + c * 2^{-12} + d * 2^{-20},        if a = 0;
 *    x = {{
 *	   {{ -16 + b * 2^{-4} + c * 2^{-12} + d * 2^{-20},  if a = 255.
 *
 *   (No other choices of a are allowed, since the magnitude of a TFM dimension
 *   must be less than 16.)  We want to multiply this quantity by the integer
 *   z, which is known to be less than 2^{27}.  Let \alpha = 16z.  If z <
 *   2^{23}, the individual multiplications b * z, c * z, d * z cannot
 *   overflow; otherwise we will divide z by 2, 4, 8, or 16, to obtain a
 *   multiplier less than 2^{23}, and we can compensate for this later.  If z
 *   has thereby been replaced by z' = z/2^e, let \beta = 2^{4-e}; we shall
 *   compute
 *
 *	\lfloor (b + c * 2^{-8} + d * 2^{-16})z' / \beta \rfloor
 *
 *   if a = 0, or the same quantity minus \alpha if a = 255.  This calculation
 *   must be done exactly, for the reasons stated above; the following program
 *   does the job in a system-independent way, assuming that arithmetic is
 *   exact on numbers less than 2^{31} in magnitude.''
 */
#define	ZLIM	((i32)1 << 23)	/* z must be < zlim */

/*
 * Scale the single TFM width t by z.
 */
i32
ScaleOneWidth(t, z)
	register i32 t, z;
{
	register i32 alpha, log2beta;
	register int a0;

	/* First compute \alpha, \beta, and z': */
	alpha = 16 * z;
	log2beta = 4;
	while (z >= ZLIM) {
		z >>= 1;
		log2beta--;
	}

	/* The four values 'a', 'b', 'c', and 'd' are fields within t: */
#define	a	(UnSign8(t >> 24))
#define	b	(UnSign8(t >> 16))
#define	c	(UnSign8(t >> 8))
#define	d	(UnSign8(t))
	if (t) {
		a0 = a;
		t = (((((d * z) >> 8) + c * z) >> 8) + b * z) >> log2beta;
		if (a0) {
			if (a0 != 255)
				error(0, 0, "bad TFM width! [ScaleOneWidth]");
			t -= alpha;
		}
	}
	return (t);
}

/*
 * Scale a set of glyphs [l..h) in font f according to f->f_dvimag
 * (that is, set g_tfmwidth from g_rawtfmwidth).
 */
ScaleGlyphs(f, l, h)
	register struct font *f;
	int l, h;
{
	register int i, a0;
	register i32 t, z, alpha, log2beta;

	z = f->f_dvimag;
	alpha = 16 * z;
	log2beta = 4;
	while (z >= ZLIM) {
		z >>= 1;
		log2beta--;
	}

	for (i = l; i < h; i++) {
		if ((t = f->f_gly[i]->g_rawtfmwidth) != 0) {
			a0 = a;
			t = (((((d * z) >> 8) + c * z) >> 8) + b * z) >>
			    log2beta;
			if (a0) {
				if (a0 != 255)
					error(0, 0,
					    "\"%s\", glyph %d: bad TFM width",
					    f->f_path, i);
				t -= alpha;
			}
		}
		f->f_gly[i]->g_tfmwidth = t;
	}
}
