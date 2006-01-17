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

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
PAUL VOJTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

\*========================================================================*/

/*
 *	GF font reading routines.
 *	Public routines are read_GF_index and read_GF_char.
 */
#define HAVE_BOOLEAN
#include "xdvi-config.h"

#define	PAINT_0		0
#define	PAINT1		64
#define	PAINT2		65
#define	PAINT3		66
#define	BOC		67
#define	BOC1		68
#define	EOC		69
#define	SKIP0		70
#define	SKIP1		71
#define	SKIP2		72
#define	SKIP3		73
#define	NEW_ROW_0	74
#define	NEW_ROW_MAX	238
#define	XXX1		239
#define	XXX2		240
#define	XXX3		241
#define	XXX4		242
#define	YYY		243
#define	NO_OP		244
#define	CHAR_LOC	245
#define	CHAR_LOC0	246
#define	PRE		247
#define	POST		248
#define	POST_POST	249

#define	GF_ID_BYTE	131
#define	TRAILER		223		/* Trailing bytes at end of file */

static	FILE	*GF_file;

static	void
expect(ch)
	ubyte ch;
{
	ubyte ch1 = one(GF_file);

	if (ch1 != ch)
		oops("Bad GF file:  %d expected, %d received.", ch, ch1);
}

static	void
too_many_bits(ch)
	ubyte ch;
{
	oops("Too many bits found when loading character %d", ch);
}

/*
 *	Public routines
 */


static	void
#if	NeedFunctionPrototypes
read_GF_char(struct font *fontp, wide_ubyte ch)
#else	/* !NeedFunctionPrototypes */
read_GF_char(fontp, ch)
	struct font *fontp;
	ubyte	ch;
#endif	/* NeedFunctionPrototypes */
{
	struct glyph *g;
	ubyte	cmnd;
	int	min_m, max_m, min_n, max_n;
	BMUNIT	*cp, *basep, *maxp;
	int	bytes_wide;
	Boolean	paint_switch;
#define	White	False
#define	Black	True
	Boolean	new_row;
	int	count;
	int	word_weight;

	g = &fontp->glyph[ch];
	GF_file = fontp->file;

	if(debug & DBG_PK)
	    Printf("Loading gf char %d", ch);

	for (;;) {
	    switch (cmnd = one(GF_file)) {
		case XXX1:
		case XXX2:
		case XXX3:
		case XXX4:
		    Fseek(GF_file, (long) num(GF_file,
			WIDENINT cmnd - XXX1 + 1), 1);
		    continue;
		case YYY:
		    (void) four(GF_file);
		    continue;
		case BOC:
		    (void) four(GF_file);	/* skip character code */
		    (void) four(GF_file);	/* skip pointer to prev char */
		    min_m = sfour(GF_file);
		    max_m = sfour(GF_file);
		    g->x = -min_m;
		    min_n = sfour(GF_file);
		    g->y = max_n = sfour(GF_file);
		    g->bitmap.w = max_m - min_m + 1;
		    g->bitmap.h = max_n - min_n + 1;
		    break;
		case BOC1:
		    (void) one(GF_file);	/* skip character code */
		    g->bitmap.w = one(GF_file);	/* max_m - min_m */
		    g->x = g->bitmap.w - one(GF_file);	/* ditto - max_m */
		    ++g->bitmap.w;
		    g->bitmap.h = one(GF_file) + 1;
		    g->y = one(GF_file);
		    break;
		default:
		    oops("Bad BOC code:  %d", cmnd);
	    }
	    break;
	}
	paint_switch = White;

	if (debug & DBG_PK)
	    Printf(", size=%dx%d, dvi_adv=%ld\n", g->bitmap.w, g->bitmap.h,
		g->dvi_adv);

	alloc_bitmap(&g->bitmap);
	cp = basep = (BMUNIT *) g->bitmap.bits;
/*
 *	Read character data into *basep
 */
	bytes_wide = ROUNDUP((int) g->bitmap.w, BMBITS) * BMBYTES;
	maxp = ADD(basep, g->bitmap.h * bytes_wide);
	bzero(g->bitmap.bits, g->bitmap.h * bytes_wide);
	new_row = False;
	word_weight = BMBITS;
	for (;;) {
	    count = -1;
	    cmnd = one(GF_file);
	    if (cmnd < 64) count = cmnd;
	    else if (cmnd >= NEW_ROW_0 && cmnd <= NEW_ROW_MAX) {
		count = cmnd - NEW_ROW_0;
		paint_switch = White;	/* it'll be complemented later */
		new_row = True;
	    }
	    else switch (cmnd) {
		case PAINT1:
		case PAINT2:
		case PAINT3:
		    count = num(GF_file, WIDENINT cmnd - PAINT1 + 1);
		    break;
		case EOC:
		    if (cp >= ADD(basep, bytes_wide)) too_many_bits(ch);
		    return;
		case SKIP1:
		case SKIP2:
		case SKIP3:
		    *((char **) &basep) +=
			num(GF_file, WIDENINT cmnd - SKIP0) * bytes_wide;
		case SKIP0:
		    new_row = True;
		    paint_switch = White;
		    break;
		case XXX1:
		case XXX2:
		case XXX3:
		case XXX4:
		    Fseek(GF_file, (long) num(GF_file,
			WIDENINT cmnd - XXX1 + 1), 1);
		    break;
		case YYY:
		    (void) four(GF_file);
		    break;
		case NO_OP:
		    break;
		default:
		    oops("Bad command in GF file:  %d", cmnd);
	    } /* end switch */
	    if (new_row) {
		*((char **) &basep) += bytes_wide;
		if (basep >= maxp || cp >= basep) too_many_bits(ch);
		cp = basep;
		word_weight = BMBITS;
		new_row = False;
	    }
	    if (count >= 0) {
		while (count)
		    if (count <= word_weight) {
#ifndef	WORDS_BIGENDIAN
			if (paint_switch)
			    *cp |= bit_masks[count] << (BMBITS - word_weight);
#endif
			word_weight -= count;
#ifdef	WORDS_BIGENDIAN
			if (paint_switch)
			    *cp |= bit_masks[count] << word_weight;
#endif
			break;
		    }
		    else {
			if (paint_switch)
#ifndef	WORDS_BIGENDIAN
			    *cp |= bit_masks[word_weight] <<
				(BMBITS - word_weight);
#else
			    *cp |= bit_masks[word_weight];
#endif
			cp++;
			count -= word_weight;
			word_weight = BMBITS;
		    }
		paint_switch = 1 - paint_switch;
	    }
	} /* end for */
	g->bitmap.endian_permuted = 0;
}


void
read_GF_index(fontp, hushcs)
	struct font	*fontp;
	wide_bool		hushcs;
{
	int	hppp, vppp;
	ubyte	ch, cmnd;
	struct glyph *g;
	long	checksum;

	fontp->read_char = read_GF_char;
	GF_file = fontp->file;
	if (debug & DBG_PK)
	    Printf("Reading GF pixel file %s\n", fontp->filename);
/*
 *	Find postamble.
 */
	Fseek(GF_file, (long) -4, 2);
	while (four(GF_file) != ((unsigned long) TRAILER << 24 | TRAILER << 16
		| TRAILER << 8 | TRAILER))
	    Fseek(GF_file, (long) -5, 1);
	Fseek(GF_file, (long) -5, 1);
	for (;;) {
	    ch = one(GF_file);
	    if (ch != TRAILER) break;
	    Fseek(GF_file, (long) -2, 1);
	}
	if (ch != GF_ID_BYTE) oops("Bad end of font file %s", fontp->fontname);
	Fseek(GF_file, (long) -6, 1);
	expect(POST_POST);
	Fseek(GF_file, sfour(GF_file), 0);	/* move to postamble */
/*
 *	Read postamble.
 */
	expect(POST);
	(void) four(GF_file);		/* pointer to last eoc + 1 */
	(void) four(GF_file);		/* skip design size */
	checksum = four(GF_file);
	if (checksum != fontp->checksum && checksum != 0 && fontp->checksum != 0
		&& !hushcs)
	    Fprintf(stderr,
		"Checksum mismatch (dvi = %lu, gf = %lu) in font file %s\n",
		fontp->checksum, checksum, fontp->filename);
	hppp = sfour(GF_file);
	vppp = sfour(GF_file);
	if (hppp != vppp && (debug & DBG_PK))
	    Printf("Font has non-square aspect ratio %d:%d\n", vppp, hppp);
	(void) four(GF_file);		/* skip min_m */
	(void) four(GF_file);		/* skip max_m */
	(void) four(GF_file);		/* skip min_n */
	(void) four(GF_file);		/* skip max_n */
/*
 *	Prepare glyph array.
 */
	fontp->glyph = xmalloc(256 * sizeof(struct glyph));
	bzero((char *) fontp->glyph, 256 * sizeof(struct glyph));
/*
 *	Read glyph directory.
 */
	while ((cmnd = one(GF_file)) != POST_POST) {
	    int addr;

	    ch = one(GF_file);			/* character code */
	    g = &fontp->glyph[ch];
	    switch (cmnd) {
		case CHAR_LOC:
		    /* g->pxl_adv = sfour(GF_file); */
		    (void) four(GF_file);
		    (void) four(GF_file);	/* skip dy */
		    break;
		case CHAR_LOC0:
		    /* g->pxl_adv = one(GF_file) << 16; */
		    (void) one(GF_file);
		    break;
		default:
		    oops("Non-char_loc command found in GF preamble:  %d",
			cmnd);
	    }
	    g->dvi_adv = fontp->dimconv * sfour(GF_file);
	    addr = four(GF_file);
	    if (addr != -1) g->addr = addr;
	    if (debug & DBG_PK)
		Printf("Read GF glyph for character %d; dy = %ld, addr = %x\n",
			ch, g->dvi_adv, addr);
	}
}
