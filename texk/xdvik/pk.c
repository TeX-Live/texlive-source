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

NOTE:
xdvi is based on prior work, as noted in the modification history
in xdvi.c.

\*========================================================================*/

/*
 *	PK font reading routines.
 *	Public routines are read_PK_index and read_PK_char.
 */

#include "xdvi-config.h"
#include "xdvi.h"
#include "dvi-init.h"
#include "util.h"

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

static int PK_flag_byte;
static unsigned PK_input_byte;
static int PK_bitpos;
static int PK_dyn_f;
static int PK_repeat_count;

static int
PK_get_nyb(FILE *fp)
{
    unsigned temp;
    if (PK_bitpos < 0) {
	PK_input_byte = get_byte(fp);
	PK_bitpos = 4;
    }
    temp = PK_input_byte >> PK_bitpos;
    PK_bitpos -= 4;
    return (temp & 0xf);
}


static int
PK_packed_num(FILE *fp)
{
    int i, j;

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
	if (i <= PK_dyn_f)
	    return i;
	if (i < 14)
	    return (((i - PK_dyn_f - 1) << 4) + PK_get_nyb(fp)
		    + PK_dyn_f + 1);
	if (i == 14)
	    PK_repeat_count = PK_packed_num(fp);
	else
	    PK_repeat_count = 1;
	return PK_packed_num(fp);
    }
}


static void
PK_skip_specials(struct font *fontp)
{
    int i, j;
    FILE *fp = fontp->file;

    do {
	PK_flag_byte = get_byte(fp);
	if (PK_flag_byte >= PK_CMD_START) {
	    switch (PK_flag_byte) {
	    case PK_X1:
	    case PK_X2:
	    case PK_X3:
	    case PK_X4:
		i = 0;
		for (j = PK_CMD_START; j <= PK_flag_byte; ++j)
		    i = (i << 8) | get_byte(fp);
		while (i--)
		    (void)get_byte(fp);
		break;
	    case PK_Y:
		(void)get_bytes(fp, 4);
	    case PK_POST:
	    case PK_NOOP:
		break;
	    default:
		XDVI_FATAL((stderr, "Unexpected %d in PK file %s", PK_flag_byte,
			    fontp->filename));
		break;
	    }
	}
    }
    while (PK_flag_byte != PK_POST && PK_flag_byte >= PK_CMD_START);
}

/*
 *	Public routines
 */

static void
read_PK_char(struct font *fontp,
	     wide_ubyte ch)
{
    int i, j;
    int n;
    int row_bit_pos;
    Boolean paint_switch;
    bmUnitT *cp;
    struct glyph *g;
    FILE *fp = fontp->file;
    long fpwidth;
    bmUnitT word = 0;
    int word_weight, bytes_wide;
    int rows_left, h_bit, count;

    g = &fontp->glyph[ch];
    PK_flag_byte = g->x2;
    PK_dyn_f = PK_flag_byte >> 4;
    paint_switch = ((PK_flag_byte & 8) != 0);
    PK_flag_byte &= 0x7;
    if (PK_flag_byte == 7)
	n = 4;
    else if (PK_flag_byte > 3)
	n = 2;
    else
	n = 1;

    if (globals.debug & DBG_PK)
	printf("loading pk char %d, char type %d ", ch, n);

    /*
     * now read rest of character preamble
     */
    if (n != 4)
	fpwidth = get_bytes(fp, 3);
    else {
	fpwidth = get_lbytes(fp, 4);
	(void)get_bytes(fp, 4);	/* horizontal escapement */
    }
    (void)get_bytes(fp, n);	/* vertical escapement */
    {
	unsigned long w, h;

	w = get_bytes(fp, n);
	h = get_bytes(fp, n);
	/* bitmap.w and bitmap.h are of type `unsigned short', so check for possible
	   overflow here. USHRT_MAX should be sufficient, since a character of
	   30pt at 1200dpi requires about 560 x 560 pixels; and the maximum
	   bitmap of size USHRT_MAX * USHRT_MAX would consume USHRT_MAX * USHRT_MAX / 8
	   bytes ~= 530MB of RAM (per character/font!)
	*/
	if (w > USHRT_MAX || h > USHRT_MAX)
	    XDVI_FATAL((stderr, "Character %d too large (%ld x %ld, max is %d x %d) in file %s",
			ch, w, h, USHRT_MAX, USHRT_MAX, fontp->fontname));
	g->bitmap.w = w;
	g->bitmap.h = h;
    }
    g->x = get_lbytes(fp, n);
    g->y = get_lbytes(fp, n);

    g->dvi_adv = fontp->dimconv * fpwidth;

    if (globals.debug & DBG_PK) {
	if (g->bitmap.w != 0)
	    printf(", size=%dx%d, dvi_adv=%ld", g->bitmap.w, g->bitmap.h,
		   g->dvi_adv);
	putchar('\n');
    }

    alloc_bitmap(&g->bitmap);
    cp = (bmUnitT *) g->bitmap.bits;

    /*
     * read character data into *cp
     */
    bytes_wide = ROUNDUP((int)g->bitmap.w, BMBITS) * BMBYTES;
    PK_bitpos = -1;
    if (PK_dyn_f == 14) {	/* get raster by bits */
	memset(g->bitmap.bits, 0, (int)g->bitmap.h * bytes_wide);
	for (i = 0; i < (int)g->bitmap.h; i++) {	/* get all rows */
	    cp = ADD(g->bitmap.bits, i * bytes_wide);
#ifndef	WORDS_BIGENDIAN
	    row_bit_pos = -1;
#else
	    row_bit_pos = BMBITS;
#endif
	    for (j = 0; j < (int)g->bitmap.w; j++) {	/* get one row */
		if (--PK_bitpos < 0) {
		    word = get_byte(fp);
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
		if (word & (1 << PK_bitpos))
		    *cp |= 1 << row_bit_pos;
	    }
	}
    }
    else {	/* get packed raster */
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
		    for (i = PK_repeat_count * bytes_wide / BMBYTES; i > 0; --i) {
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
	if (cp != ((bmUnitT *) (g->bitmap.bits + bytes_wide * g->bitmap.h)))
	    XDVI_FATAL((stderr, "Wrong number of bits stored:  char. %d, font %s", ch,
			fontp->fontname));
	if (rows_left != 0 || h_bit != g->bitmap.w)
	    XDVI_FATAL((stderr, "Bad pk file (%s), too many bits", fontp->fontname));
    }
}

void
read_PK_index(struct font *fontp, wide_bool hushcs)
{
    int hppp, vppp;
    long checksum;

    fontp->read_char = read_PK_char;
    if (globals.debug & DBG_PK)
	printf("Reading PK pixel file %s\n", fontp->filename);

    fseek(fontp->file, (long)get_byte(fontp->file), SEEK_CUR);	/* skip comment */

    (void)get_bytes(fontp->file, 4);	/* skip design size */
    checksum = get_bytes(fontp->file, 4);
    if (checksum != fontp->checksum && checksum != 0 && fontp->checksum != 0
	&& !hushcs)
	XDVI_WARNING((stderr, "Checksum mismatch (dvi = %lu, pk = %lu) in font file %s",
		      fontp->checksum, checksum, fontp->filename));
    hppp = get_lbytes(fontp->file, 4);
    vppp = get_lbytes(fontp->file, 4);
    if (hppp != vppp && (globals.debug & DBG_PK))
	printf("Font has non-square aspect ratio %d:%d\n", vppp, hppp);
    /*
     * Prepare glyph array.
     */
    fontp->glyph = xmalloc(256 * sizeof(struct glyph));
    memset((char *)fontp->glyph, 0, 256 * sizeof(struct glyph));
    /*
     * Read glyph directory (really a whole pass over the file).
     */
    for (;;) {
	int bytes_left, flag_low_bits;
	unsigned int ch;

	PK_skip_specials(fontp);
	if (PK_flag_byte == PK_POST)
	    break;
	flag_low_bits = PK_flag_byte & 0x7;
	if (flag_low_bits == 7) {
	    bytes_left = get_bytes(fontp->file, 4);
	    ch = get_bytes(fontp->file, 4);
	}
	else if (flag_low_bits > 3) {
	    bytes_left = ((flag_low_bits - 4) << 16) + get_bytes(fontp->file, 2);
	    ch = get_byte(fontp->file);
	}
	else {
	    bytes_left = (flag_low_bits << 8) + get_byte(fontp->file);
	    ch = get_byte(fontp->file);
	}
	fontp->glyph[ch].addr = ftell(fontp->file);
	fontp->glyph[ch].x2 = PK_flag_byte;
#ifdef linux
# ifndef SHORTSEEK
#  define SHORTSEEK 2048
# endif
	/* A bug in Linux libc (as of 18oct94) makes a short read faster
	   than a short forward seek. Totally non-intuitive.  */
	if (bytes_left > 0 && bytes_left < SHORTSEEK) {
	    char *dummy = xmalloc(bytes_left);
	    (void)fread(dummy, 1, bytes_left, fontp->file);
	    free(dummy);
	}
	else
	    /* seek backward, or long forward */
#endif /* linux */
	    fseek(fontp->file, (long)bytes_left, SEEK_CUR);
	if (globals.debug & DBG_PK)
	    printf("Scanning pk char %u, at %ld.\n", ch, fontp->glyph[ch].addr);
    }
}
