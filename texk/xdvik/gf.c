/*========================================================================*\

Copyright (c) 1990-2004  Paul Vojta

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
PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

\*========================================================================*/

/*
 *	GF font reading routines.
 *	Public routines are read_GF_index and read_GF_char.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "dvi-init.h"
#include "util.h"

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
#define	TRAILER		223	/* Trailing bytes at end of file */

static FILE *GF_file;

static void
expect(ubyte ch)
{
    ubyte ch1 = get_byte(GF_file);

    if (ch1 != ch)
	XDVI_FATAL((stderr, "Bad GF file:  %d expected, %d received.", ch, ch1));
}

static void
too_many_bits(ubyte ch)
{
    XDVI_FATAL((stderr, "Too many bits found when loading character %d", ch));
}

/*
 *	Public routines
 */


static void
read_GF_char(struct font *fontp,
	     wide_ubyte ch)
{
    struct glyph *g;
    ubyte cmnd;
    int min_m, max_m, min_n, max_n;
    bmUnitT *cp, *basep, *maxp;
    int bytes_wide;
    Boolean paint_switch;
#define	White	False
#define	Black	True
    Boolean new_row;
    int count;
    int word_weight;

    g = &fontp->glyph[ch];
    GF_file = fontp->file;

    if (globals.debug & DBG_PK)
	printf("Loading gf char %d", ch);

    for (;;) {
	switch (cmnd = get_byte(GF_file)) {
	case XXX1:
	case XXX2:
	case XXX3:
	case XXX4:
	    fseek(GF_file, (long)get_bytes(GF_file, (int)(cmnd - XXX1 + 1)), SEEK_CUR);
	    continue;
	case YYY:
	    (void)get_bytes(GF_file, 4);
	    continue;
	case BOC:
	    (void)get_bytes(GF_file, 4);	/* skip character code */
	    (void)get_bytes(GF_file, 4);	/* skip pointer to prev char */
	    min_m = get_lbytes(GF_file, 4);
	    max_m = get_lbytes(GF_file, 4);
	    g->x = -min_m;
	    min_n = get_lbytes(GF_file, 4);
	    g->y = max_n = get_lbytes(GF_file, 4);
	    g->bitmap.w = max_m - min_m + 1;
	    g->bitmap.h = max_n - min_n + 1;
	    break;
	case BOC1:
	    (void)get_byte(GF_file);	/* skip character code */
	    g->bitmap.w = get_byte(GF_file);	/* max_m - min_m */
	    g->x = g->bitmap.w - get_byte(GF_file);	/* ditto - max_m */
	    ++g->bitmap.w;
	    g->bitmap.h = get_byte(GF_file) + 1;
	    g->y = get_byte(GF_file);
	    break;
	default:
	    XDVI_FATAL((stderr, "Bad BOC code:  %d", cmnd));
	}
	break;
    }
    paint_switch = White;

    if (globals.debug & DBG_PK)
	printf(", size=%dx%d, dvi_adv=%ld\n", g->bitmap.w, g->bitmap.h,
	       g->dvi_adv);

    alloc_bitmap(&g->bitmap);
    cp = basep = (bmUnitT *) g->bitmap.bits;
    /*
     *	Read character data into *basep
     */
    bytes_wide = ROUNDUP((int)g->bitmap.w, BMBITS) * BMBYTES;
    maxp = ADD(basep, g->bitmap.h * bytes_wide);
    memset(g->bitmap.bits, 0, g->bitmap.h * bytes_wide);
    new_row = False;
    word_weight = BMBITS;
    for (;;) {
	count = -1;
	cmnd = get_byte(GF_file);
	if (cmnd < 64)
	    count = cmnd;
	else if (cmnd >= NEW_ROW_0 && cmnd <= NEW_ROW_MAX) {
	    count = cmnd - NEW_ROW_0;
	    paint_switch = White;	/* it'll be complemented later */
	    new_row = True;
	}
	else
	    switch (cmnd) {
	    case PAINT1:
	    case PAINT2:
	    case PAINT3:
		count = get_bytes(GF_file, (int)(cmnd - PAINT1 + 1));
		break;
	    case EOC:
		if (cp >= ADD(basep, bytes_wide))
		    too_many_bits(ch);
		return;
	    case SKIP1:
	    case SKIP2:
	    case SKIP3:
		basep += get_bytes(GF_file, (int)(cmnd - SKIP0)) * bytes_wide / sizeof(bmUnitT);
		/* 		*((char **)&basep) += get_bytes(GF_file, WIDENINT cmnd - SKIP0) * bytes_wide; */
	    case SKIP0:
		new_row = True;
		paint_switch = White;
		break;
	    case XXX1:
	    case XXX2:
	    case XXX3:
	    case XXX4:
		fseek(GF_file, (long)get_bytes(GF_file, (int)(cmnd - XXX1 + 1)), SEEK_CUR);
		break;
	    case YYY:
		(void)get_bytes(GF_file, 4);
		break;
	    case NO_OP:
		break;
	    default:
		XDVI_FATAL((stderr, "Bad command in GF file:  %d", cmnd));
	    }	/* end switch */
	if (new_row) {
	    basep += bytes_wide / sizeof(bmUnitT);
	    /* 	    *((char **)&basep) += bytes_wide; */
	    if (basep >= maxp || cp >= basep)
		too_many_bits(ch);
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
			*cp |= bit_masks[word_weight] << (BMBITS - word_weight);
#else
		    *cp |= bit_masks[word_weight];
#endif
		    cp++;
		    count -= word_weight;
		    word_weight = BMBITS;
		}
	    paint_switch = 1 - paint_switch;
	}
    }	/* end for */
}


void
read_GF_index(struct font *fontp, wide_bool hushcs)
{
    int hppp, vppp;
    ubyte ch, cmnd;
    struct glyph *g;
    long checksum;

    fontp->read_char = read_GF_char;
    GF_file = fontp->file;
    if (globals.debug & DBG_PK)
	printf("Reading GF pixel file %s\n", fontp->filename);
    /*
     *	Find postamble.
     */
    fseek(GF_file, (long)-4, SEEK_END);
    while (get_bytes(GF_file, 4) != ((unsigned long)TRAILER << 24 | TRAILER << 16
				     | TRAILER << 8 | TRAILER))
	fseek(GF_file, (long)-5, SEEK_CUR);
    fseek(GF_file, (long)-5, SEEK_CUR);
    for (;;) {
	ch = get_byte(GF_file);
	if (ch != TRAILER)
	    break;
	fseek(GF_file, (long)-2, SEEK_CUR);
    }
    if (ch != GF_ID_BYTE)
	XDVI_FATAL((stderr, "Bad end of font file %s", fontp->fontname));
    fseek(GF_file, (long)-6, SEEK_CUR);
    expect(POST_POST);
    fseek(GF_file, get_lbytes(GF_file, 4), SEEK_SET);	/* move to postamble */
    /*
     *	Read postamble.
     */
    expect(POST);
    (void)get_bytes(GF_file, 4);	/* pointer to last eoc + 1 */
    (void)get_bytes(GF_file, 4);	/* skip design size */
    checksum = get_bytes(GF_file, 4);
    if (checksum != fontp->checksum && checksum != 0 && fontp->checksum != 0
	&& !hushcs)
	XDVI_WARNING((stderr, "Checksum mismatch (dvi = %lu, gf = %lu) in font file %s",
		      fontp->checksum, checksum, fontp->filename));
    hppp = get_lbytes(GF_file, 4);
    vppp = get_lbytes(GF_file, 4);
    if (hppp != vppp && (globals.debug & DBG_PK))
	printf("Font has non-square aspect ratio %d:%d\n", vppp, hppp);
    (void)get_bytes(GF_file, 4);	/* skip min_m */
    (void)get_bytes(GF_file, 4);	/* skip max_m */
    (void)get_bytes(GF_file, 4);	/* skip min_n */
    (void)get_bytes(GF_file, 4);	/* skip max_n */
    /*
     *	Prepare glyph array.
     */
    fontp->glyph = xmalloc(256 * sizeof(struct glyph));
    memset((char *)fontp->glyph, 0, 256 * sizeof(struct glyph));
    /*
     *	Read glyph directory.
     */
    while ((cmnd = get_byte(GF_file)) != POST_POST) {
	int addr;

	ch = get_byte(GF_file);	/* character code */
	g = &fontp->glyph[ch];
	switch (cmnd) {
	case CHAR_LOC:
	    /* g->pxl_adv = get_lbytes(GF_file, 4); */
	    (void)get_bytes(GF_file, 4);
	    (void)get_bytes(GF_file, 4);	/* skip dy */
	    break;
	case CHAR_LOC0:
	    /* g->pxl_adv = get_byte(GF_file) << 16; */
	    (void)get_byte(GF_file);
	    break;
	default:
	    XDVI_FATAL((stderr, "Non-char_loc command found in GF preamble:  %d", cmnd));
	}
	g->dvi_adv = fontp->dimconv * get_lbytes(GF_file, 4);
	addr = get_bytes(GF_file, 4);
	if (addr != -1)
	    g->addr = addr;
	if (globals.debug & DBG_PK)
	    printf("Read GF glyph for character %d; dy = %ld, addr = %x\n",
		   ch, g->dvi_adv, addr);
    }
}
