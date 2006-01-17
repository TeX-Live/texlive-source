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

NOTE:
	xdvi is based on prior work, as noted in the modification history
	in xdvi.c.

\*========================================================================*/

/*
 *	PK font reading routines.
 *	Public routines are read_PK_index and read_PK_char.
 */
#define HAVE_BOOLEAN
#include "xdvi-config.h"

#define PK_ID      89
#define PK_CMD_START 240
#define PK_X1     240
#define PK_X2     241
#define PK_X3     242
#define PK_X4     243
#define PK_Y      244
#define PK_POST   245
#define PK_NOOP   246
#define PK_PRE    247

static	int	PK_flag_byte;
static	unsigned PK_input_byte;
static	int	PK_bitpos;
static	int	PK_dyn_f;
static	int	PK_repeat_count;

static	int
PK_get_nyb(fp)
	FILE *fp;
{
	unsigned temp;
	if (PK_bitpos < 0) {
	    PK_input_byte = one(fp);
	    PK_bitpos = 4;
	}
	temp = PK_input_byte >> PK_bitpos;
	PK_bitpos -= 4;
	return (temp & 0xf);
}


static	int
PK_packed_num(fp)
	FILE *fp;
{
	int	i,j;

	if ((i = PK_get_nyb(fp)) == 0) {
	    do {
		j = PK_get_nyb(fp);
		++i;
	    }
	    while (j == 0);
	    while (i > 0) {
		j = (j << 4) | PK_get_nyb(fp);
		--i;
	    }
	    return (j - 15 + ((13 - PK_dyn_f) << 4) + PK_dyn_f);
	}
	else {
	    if (i <= PK_dyn_f) return i;
	    if (i < 14)
		return (((i - PK_dyn_f - 1) << 4) + PK_get_nyb(fp)
		    + PK_dyn_f + 1);
	    if (i == 14) PK_repeat_count = PK_packed_num(fp);
	    else PK_repeat_count = 1;
	    return PK_packed_num(fp);
	}
}


static	void
PK_skip_specials(fontp)
	struct font *fontp;
{
	int i,j;
	FILE *fp = fontp->file;

	do {
	    PK_flag_byte = one(fp);
	    if (PK_flag_byte >= PK_CMD_START) {
		switch (PK_flag_byte) {
		    case PK_X1 :
		    case PK_X2 :
		    case PK_X3 :
		    case PK_X4 :
			i = 0;
			for (j = PK_CMD_START; j <= PK_flag_byte; ++j)
			    i = (i << 8) | one(fp);
			while (i--) (void) one(fp);
			break;
		    case PK_Y :
			(void) four(fp);
		    case PK_POST :
		    case PK_NOOP :
			break;
		    default :
			oops("Unexpected %d in PK file %s", PK_flag_byte,
			    fontp->filename);
			break;
		}
	    }
	}
	while (PK_flag_byte != PK_POST && PK_flag_byte >= PK_CMD_START);
}

/*
 *	Public routines
 */

static	void
#if	NeedFunctionPrototypes
read_PK_char(struct font *fontp, wide_ubyte ch)
#else	/* !NeedFunctionPrototypes */
read_PK_char(fontp, ch)
	struct font *fontp;
	ubyte	ch;
#endif	/* NeedFunctionPrototypes */
{
	int	i, j;
	int	n;
	int	row_bit_pos;
	Boolean	paint_switch;
	BMUNIT	*cp;
	struct glyph *g;
	FILE *fp = fontp->file;
	long	fpwidth;
	BMUNIT	word;
	int	word_weight, bytes_wide;
	int	rows_left, h_bit, count;

	g = &fontp->glyph[ch];
	PK_flag_byte = g->x2;
	PK_dyn_f = PK_flag_byte >> 4;
	paint_switch = ((PK_flag_byte & 8) != 0);
	PK_flag_byte &= 0x7;
	if (PK_flag_byte == 7) n = 4;
	else if (PK_flag_byte > 3) n = 2;
	else n = 1;

	if (debug & DBG_PK) Printf("loading pk char %d, char type %d ", ch, n);

	/*
	 * now read rest of character preamble
	 */
	if (n != 4) fpwidth = num(fp, 3);
	else {
	    fpwidth = sfour(fp);
	    (void) four(fp);	/* horizontal escapement */
	}
	(void) num(fp, n);	/* vertical escapement */
	{
	    unsigned long w, h;

	    w = num(fp, n);
	    h = num(fp, n);
	    if (w > 0x7fff || h > 0x7fff)
		oops("Character %d too large in file %s", ch, fontp->fontname);
	    g->bitmap.w = w;
	    g->bitmap.h = h;
	}
	g->x = snum(fp, n);
	g->y = snum(fp, n);
	g->dvi_adv = fontp->dimconv * fpwidth;

	if (debug & DBG_PK) {
	    if (g->bitmap.w != 0)
		Printf(", size=%dx%d, dvi_adv=%ld", g->bitmap.w, g->bitmap.h,
		    g->dvi_adv);
	    Putchar('\n');
	}

	alloc_bitmap(&g->bitmap);
	cp = (BMUNIT *) g->bitmap.bits;

	/*
	 * read character data into *cp
	 */
	bytes_wide = ROUNDUP((int) g->bitmap.w, BMBITS) * BMBYTES;
	PK_bitpos = -1;
	if (PK_dyn_f == 14) {	/* get raster by bits */
	    bzero(g->bitmap.bits, (int) g->bitmap.h * bytes_wide);
	    for (i = 0; i < (int) g->bitmap.h; i++) {	/* get all rows */
		cp = ADD(g->bitmap.bits, i * bytes_wide);
#ifndef	WORDS_BIGENDIAN
		row_bit_pos = -1;
#else
		row_bit_pos = BMBITS;
#endif
		for (j = 0; j < (int) g->bitmap.w; j++) {    /* get one row */
		    if (--PK_bitpos < 0) {
			word = one(fp);
			PK_bitpos = 7;
		    }
#ifndef	WORDS_BIGENDIAN
		    if (++row_bit_pos >= BMBITS) {
			cp++;
			row_bit_pos = 0;
		    }
#else
		    if (--row_bit_pos < 0) {
			cp++;
			row_bit_pos = BMBITS - 1;
		    }
#endif
		    if (word & (1 << PK_bitpos)) *cp |= 1 << row_bit_pos;
		}
	    }
	}
	else {		/* get packed raster */
	    rows_left = g->bitmap.h;
	    h_bit = g->bitmap.w;
	    PK_repeat_count = 0;
	    word_weight = BMBITS;
	    word = 0;
	    while (rows_left > 0) {
		count = PK_packed_num(fp);
		while (count > 0) {
		    if (count < word_weight && count < h_bit) {
#ifndef	WORDS_BIGENDIAN
			if (paint_switch)
			    word |= bit_masks[count] << (BMBITS - word_weight);
#endif
			h_bit -= count;
			word_weight -= count;
#ifdef	WORDS_BIGENDIAN
			if (paint_switch)
			    word |= bit_masks[count] << word_weight;
#endif
			count = 0;
		    }
		    else if (count >= h_bit && h_bit <= word_weight) {
			if (paint_switch)
			    word |= bit_masks[h_bit] <<
#ifndef	WORDS_BIGENDIAN
				(BMBITS - word_weight);
#else
				(word_weight - h_bit);
#endif
			*cp++ = word;
			/* "output" row(s) */
			for (i = PK_repeat_count * bytes_wide / BMBYTES;
			  i > 0; --i) {
			    *cp = *SUB(cp, bytes_wide);
			    ++cp;
			}
			rows_left -= PK_repeat_count + 1;
			PK_repeat_count = 0;
			word = 0;
			word_weight = BMBITS;
			count -= h_bit;
			h_bit = g->bitmap.w;
		    }
		    else {
			if (paint_switch)
#ifndef	WORDS_BIGENDIAN
			    word |= bit_masks[word_weight] <<
				(BMBITS - word_weight);
#else
			    word |= bit_masks[word_weight];
#endif
			*cp++ = word;
			word = 0;
			count -= word_weight;
			h_bit -= word_weight;
			word_weight = BMBITS;
		    }
		}
		paint_switch = 1 - paint_switch;
	    }
	    if (cp != ((BMUNIT *) (g->bitmap.bits + bytes_wide * g->bitmap.h)))
		oops("Wrong number of bits stored:  char. %d, font %s", ch,
		    fontp->fontname);
	    if (rows_left != 0 || h_bit != g->bitmap.w)
		oops("Bad pk file (%s), too many bits", fontp->fontname);
	}
#ifdef WIN32
	g->bitmap.endian_permuted = 0;
#endif
}

void
read_PK_index(fontp, hushcs)
	struct font	*fontp;
	wide_bool		hushcs;
{
	int	hppp, vppp;
	long	checksum;

	fontp->read_char = read_PK_char;
	if (debug & DBG_PK)
	    Printf("Reading PK pixel file %s\n", fontp->filename);

	Fseek(fontp->file, (long) one(fontp->file), 1);	/* skip comment */

	(void) four(fontp->file);		/* skip design size */
	checksum = four(fontp->file);
	if (checksum != fontp->checksum && checksum != 0 && fontp->checksum != 0
		&& !hushcs)
	    Fprintf(stderr,
		"Checksum mismatch (dvi = %lu, pk = %lu) in font file %s\n",
		fontp->checksum, checksum, fontp->filename);
	hppp = sfour(fontp->file);
	vppp = sfour(fontp->file);
	if (hppp != vppp && (debug & DBG_PK))
	    Printf("Font has non-square aspect ratio %d:%d\n", vppp, hppp);
	/*
	 * Prepare glyph array.
	 */
	fontp->glyph = xmalloc(256 * sizeof(struct glyph));
	bzero((char *) fontp->glyph, 256 * sizeof(struct glyph));
	/*
	 * Read glyph directory (really a whole pass over the file).
	 */
	for (;;) {
	    int bytes_left, flag_low_bits;
	    unsigned int ch;

	    PK_skip_specials(fontp);
	    if (PK_flag_byte == PK_POST) break;
	    flag_low_bits = PK_flag_byte & 0x7;
	    if (flag_low_bits == 7) {
		bytes_left = four(fontp->file);
		ch = four(fontp->file);
	    } else if (flag_low_bits > 3) {
		bytes_left = ((flag_low_bits - 4) << 16) + two(fontp->file);
		ch = one(fontp->file);
	    } else {
		bytes_left = (flag_low_bits << 8) + one(fontp->file);
		ch = one(fontp->file);
	    }
	    fontp->glyph[ch].addr = ftell(fontp->file);
	    fontp->glyph[ch].x2 = PK_flag_byte;
#ifdef linux
#ifndef SHORTSEEK
#define SHORTSEEK 2048
#endif
          /* A bug in Linux libc (as of 18oct94) makes a short read faster
             than a short forward seek. Totally non-intuitive.  */
          if (bytes_left > 0 && bytes_left < SHORTSEEK) {
            char *dummy = xmalloc (bytes_left);
            Fread (dummy, 1, bytes_left, fontp->file);
            free (dummy);
          } else
            /* seek backward, or long forward */
#endif /* linux */
	    Fseek(fontp->file, (long) bytes_left, 1);
	    if (debug & DBG_PK)
		Printf("Scanning pk char %u, at %ld.\n", ch,
		    fontp->glyph[ch].addr);
	}
}
