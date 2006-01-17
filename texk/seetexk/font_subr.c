/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/font_subr.c,v 2.8 89/10/30 04:08:52 chris Exp $";
#endif

/*
 * Subroutines common to all fonts.
 */

#include "types.h"
#include "error.h"
#include "font.h"

static struct glyph *freeglyphs;

#if !defined( WIN32 ) && !defined( _AMIGA )
char	*malloc();
extern	errno;
#endif

/*
 * Set up the font structures to note that a font has glyphs in
 * the half-open interval [low, high).
 *
 * SHOULD I ALLOW ADDITIONS TO THE RANGE VIA SUBSEQUENT CALLS TO
 * FontHasGlyphs?
 */
FontHasGlyphs(f, low, high)
	register struct font *f;
	register int low, high;
{
	register struct glyph **gp;

	/* record bounds */
	f->f_lowch = low;
	f->f_highch = high;

	/*
	 * Allocate space for all the glyph pointers, and set
	 * them all to NULL.
	 */
	if (low >= high)	/* no glyphs */
		gp = NULL;
	else {
		gp = (struct glyph **) malloc((unsigned) (high - low) *
			sizeof (*gp));
		if (gp == NULL)
			return (-1);
	}
	f->f_glybase = gp;
	f->f_gly = gp - low;
	while (++low <= high)
		*gp++ = NULL;
	return (0);
}

/*
 * AllocGlyph allocates a new glyph.  ReleaseGlyph puts one onto the free
 * list.  We maintain a local list of free glyphs.
 */
#define ReleaseGlyph(g)	\
	((g)->g_un.g_next = freeglyphs, freeglyphs = (g))

static struct glyph *
AllocGlyph(n)
	int n;
{
	register struct glyph *g;
	register int i;

	if ((g = freeglyphs) == NULL) {
		g = (struct glyph *) malloc((unsigned) (128 * sizeof (*g)));
		if (g == NULL)
			error(1, errno, "out of glyph memory");
		g += (i = 128);
		while (--i >= 0) {
			g--;
			ReleaseGlyph(g);
		}
	}
	freeglyphs = g->g_un.g_next;
	g->g_flags = 0;
	g->g_raster = NULL;
	g->g_index = n;
	g->g_xescapement = NO_ESCAPEMENT;/* default, if not set by font */
	g->g_yescapement = NO_ESCAPEMENT;
	return (g);
}

/*
 * Free one glyph.
 */
void
FreeGlyph(f, g)
	struct font *f;
	register struct glyph *g;
{
	int n = g->g_index;

#ifdef notdef
	(*f->f_ops->fo_freegly)(f, n, n);
#endif
	if (g->g_raster != NULL)
		free(g->g_raster);
	ReleaseGlyph(g);
	f->f_gly[n] = NULL;
}

/*
 * Free a font.
 */
void
FreeFont(f)
	register struct font *f;
{
	register struct glyph *g;
	register int i;

#ifdef notdef
	(*f->f_ops->fo_freegly)(f, f->f_lowch, f->f_highch);
#endif
	for (i = f->f_lowch; i < f->f_highch; i++) {
		if ((g = f->f_gly[i]) == NULL)
			continue;
		if (g->g_raster != NULL)
			free(g->g_raster);
		ReleaseGlyph(g);
	}
	if (f->f_glybase != NULL)
		free((char *)f->f_glybase);
	(*f->f_ops->fo_freefont)(f);
	free(f->f_path);
	free(f->f_font);
	free((char *)f);
}

/*
 * Get glyph `c' in font `f'.  We pull in a few adjacent glyphs here
 * under the (perhaps naive) assumption that things will go faster
 * that way.
 *
 * TODO:
 *	try different adjacency values
 *	make adjacency a font attribute? (or an op)
 */
#define	ADJ	4		/* must be a power of 2 */
#define	GET_ADJ(c, l, h) ((h) = ADJ + ((l) = (c) & ~(ADJ - 1)))

struct glyph *
GetGlyph(f, c)
	register struct font *f;
	int c;
{
	register int i, h, l;

	GET_ADJ(c, l, h);
	if (l < f->f_lowch)
		l = f->f_lowch;
	if (h > f->f_highch)
		h = f->f_highch;
	if (l >= h)
		return (NULL);
	for (i = l; i < h; i++)
		if (f->f_gly[i] == NULL)
			f->f_gly[i] = AllocGlyph(i);

	if ((*f->f_ops->fo_getgly)(f, l, h)) {
		/*
		 * I do not know what to do about this just yet, so:
		 */
		panic("getgly fails and I am confused ... help!");
	}

	/*
	 * Apply the appropriate scale factor to the TFM widths.
	 * This makes them come out in scaled points, instead of FIXes.
	 */
	ScaleGlyphs(f, l, h);	/* ??? */

	return (f->f_gly[c]);
}

/*
 * Get the raster for glyph g in font f at rotation r.
 */
char *
GetRaster(g, f, r)
	register struct glyph *g;
	register struct font *f;
	int r;
{
	int l, h;

	/* abort if caller did not ask for rasters in advance */
	if ((f->f_flags & FF_RASTERS) == 0)
		panic("GetRaster(%s)", f->f_path);

	/*
	 * If there is no raster, get one.  Blank characters,
	 * however, never have rasters.
	 */
	if (g->g_raster == NULL) {
		if (!HASRASTER(g))
			return (NULL);
		/*
		 * THE FOLLOWING DEPENDS ON THE ADJACENCY MATCHING THAT IN
		 * GetGlyph() ABOVE.
		 */
		GET_ADJ(g->g_index, l, h);
		if (l < f->f_lowch)
			l = f->f_lowch;
		if (h > f->f_highch)
			h = f->f_highch;
		if ((*f->f_ops->fo_rasterise)(f, l, h))
			error(1, 0, "rasterise fails (out of memory?)");
	}
	if (g->g_rotation != r)
		SetRotation(g, r);
	return (g->g_raster);
}

void
FreeRaster(g)
	struct glyph *g;
{

	if (g->g_raster) {
		free((g)->g_raster);
		g->g_raster = NULL;
	}
}

/*
 * Return a TeX-style font name, including scale factor.
 * SHOULD I BOTHER WITH \magstep STYLE NAMES?
 */
char *
Font_TeXName(f)
	register struct font *f;
{
	static char result[200];

	if (f->f_scaled == 1000)
		return (f->f_font);
	(void) sprintf(result, "%s scaled %d", f->f_font, f->f_scaled);
	return (result);
}
