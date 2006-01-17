/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/rotate.c,v 2.4 89/08/22 21:56:05 chris Exp $";
#endif

/*
 * Routines to generate rotated bitmaps given unrotated inputs.
 *
 * The rotated bitmap is indistinguishable from the unrotated one (aside
 * from being rotated of course!).
 */

#include "types.h"
#include "error.h"
#include "font.h"

static void RotateClockwise();

#if !defined( WIN32 ) && !defined( _AMIGA )
extern	int errno;
char	*malloc();
#endif

/*
 * Rounding, but by powers of two only.
 */
#define ROUND(n,r) (((n) + ((r) - 1)) & ~((r) - 1))

/*
 * Set the rotation of glyph g to r.
 */
void SetRotation(g, r)
	register struct glyph *g;
	int r;
{

	if (r < ROT_NORM || r > ROT_RIGHT)
		error(1, 0, "bad rotation value %d", r);

	/*
	 * The null glyph is trivial to rotate by any amount.
	 *
	 * Note that this assumes that any raster has been obtained
	 * BEFORE calling SetRotation()!
	 */
	if (g->g_raster == NULL) {
		g->g_rotation = r;
		return;
	}

	/*
	 * This is hardly efficient, but it *is* expedient....
	 */
	while (g->g_rotation != r) {
		RotateClockwise(g);
		g->g_rotation = (g->g_rotation - 1) & 3;
	}
}

/*
 * Rotation by 1/4 turn clockwise (from ROT_NORM to ROT_RIGHT, e.g.).
 */
static void
RotateClockwise(glyph)
	struct glyph *glyph;
{
	register char *nrast;	/* new raster */
	register char *orast;	/* old raster */
	register int oheight;	/* old raster height, new raster width */
	register int owidth;	/* old raster width, new raster height */
	unsigned int size;	/* size of new raster (in bytes) */
	int nplus;		/* offset between rows in nrast */

	/*
	 * First, get a new raster.
	 */
	{
		register struct glyph *g = glyph;
		register int t;

		oheight = g->g_height;
		owidth = g->g_width;

		/*
		 * Offset is (new width) rounded to bytes.
		 */
		nplus = ROUND(oheight, 8) >> 3;

		/*
		 * Size of new raster is (new height) * (new rounded width,
		 * in bytes).
		 */
		size = nplus * owidth;
		if ((nrast = malloc(size)) == NULL)
			error(1, errno, "out of memory");
		bzero(nrast, size);

		/*
		 * New y origin is old x origin; new x origin is old height
		 * minus old y origin - 1.
		 */
		t = g->g_yorigin;
		g->g_yorigin = g->g_xorigin;
		g->g_xorigin = oheight - t - 1;

		/* While we are at it, exchange height & width... */
		g->g_height = owidth;
		g->g_width = oheight;

		/* and grab a pointer to the old raster. */
		orast = g->g_raster;
	}

	/*
	 * Now copy bits from the old raster to the new one.  The mapping
	 * function is
	 *
	 *	for i in [0..height)
	 *		for j in [0..width)
	 *			new[j, height-i-1] = old[i, j]
	 *
	 * Thus i maps to height-i-1 and (since we have to do our own 2
	 * dimensional array indexing) j to nplus*j.  We call the mapped
	 * variables mapi and mapj, and, since we scan sequentially through
	 * the old raster, can discard the original i and j.
	 */
	{
		register int mapj, c, k, mapi;

		mapi = oheight;
		owidth *= nplus;
		while (--mapi >= 0) {
			k = 7;
			for (mapj = 0; mapj < owidth; mapj += nplus) {
				if (++k == 8)	/* get another byte */
					c = *orast++, k = 0;
				if (c & 0x80)	/* old[i,j] was set */
					nrast[mapj + (mapi >> 3)] |=
						1 << (7 - (mapi & 7));
				c <<= 1;
			}
		}
	}

	/*
	 * Finally, free the storage associated with the original raster.
	 */
	free(glyph->g_raster);
	glyph->g_raster = nrast;
}
