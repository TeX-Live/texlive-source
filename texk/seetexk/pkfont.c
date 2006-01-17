/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/pkfont.c,v 2.10 89/08/22 21:54:49 chris Exp $";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "types.h"
#include "error.h"
#include "font.h"
#include "num.h"

/*
 * PK font operations.
 *
 * The spelling `nybble' is a concession to the authors of the PK format.
 */
static int pk_read(), pk_getgly(), pk_rasterise();
static void pk_freefont();

struct	fontops pkops =
	{ "pk", 0, 1.0, pk_read, pk_getgly, pk_rasterise, pk_freefont };

/*
 * Local info.
 */

/*
 * Commands.
 */
#define	PK_XXX1		240	/* 1 byte special */
#define	PK_XXX2		241	/* 2 byte special */
#define	PK_XXX3		242	/* 3 byte special */
#define	PK_XXX4		243	/* 4 byte special */
#define	PK_YYY		244	/* METAFONT numspecial */
#define	PK_POST		245	/* marks postamble */
#define	PK_NO_OP	246	/* do nothing */
#define	PK_PRE		247	/* marks preamble */
				/* 248..255 undefined */
#define PK_IsUndef(c)	((c) > PK_PRE)

#define	PK_ID		89	/* marks this brand of PK file */

/*
 * Information about a character packet.
 */
struct cp {
	char	*cp_packet;	/* the beginning of the packet */
	int	cp_type;	/* decoded pre type, see below */
};

#define	CP_SHORT	0	/* short preamble */
#define	CP_EXT_SHORT	1	/* extended short preamble */
#define	CP_LONG		2	/* long preamble */

/*
 * The PK details include:
 *  ->	a pointer to the next byte to fetch;
 *  ->	the most recent byte fetched (when we are using nextnyb());
 *  ->	a flag indicating that we have used nybble 0 (bits 4..7) and
 *	should use nybble 1 next;
 *  ->	the base address of the memory allocated for the PK file;
 *  ->	the value of dyn_f (during character translation);
 *  ->	the repeat count (during rasterisation);
 *  ->	the lowest glyph number that is legal;
 *  ->	the highest glyph number that is legal;
 *  ->	glyph instructions for the standard glyphs;
 *  ->	glyph instructions for more (nonstandard) glyphs;
 * and	the number of glyphs left unrasterised.
 */
struct pk_details {
	char	*pk_ptr;	/* next byte to fetch */
	int	pk_c;		/* most recent byte fetched, if nybbling */
	int	pk_1nyb;	/* true => nybble 1 is next (bits 0..3) */
	char	*pk_base;	/* base of allocated memory */
	int	pk_dyn_f;	/* the dyn_f value */
	int	pk_repeat;	/* the repeat count */
	int	pk_minc;	/* minimum character value */
	int	pk_maxc;	/* maximum character value */
#define MAXSTD	256		/* maximum `standard' character value */
	int	pk_gleft;	/* number of valid glyphs left uninterpreted */
	struct	cp pk_cpack[MAXSTD];	/* for characters in [0..MAXSTD) */
	struct	cp *pk_morec;		/* for characters in [MAXSTD..maxc] */
};

/*
 * Fetch the next byte from the PK file.
 */
#define	nextbyte(pk) pgetbyte((pk)->pk_ptr)

/*
 * PK packed number encoding.  Nybbles in [1..dyn_f] represent themselves.
 * Values in (dyn_f..13] are two-nybble values, and represent values
 * dyn_f+1 through (13-dyn_f+1)*16+15.  Zero marks a long number; 14 and
 * 15 specify repeat counts instead (which are another packed number).
 * Note that we cannot represent the number zero as a packed number.
 */
#define	PK_LONGNUM	0	/* a `long number' */
#define	PK_REPEAT	14	/* set repeat count */
#define	PK_REPEAT1	15	/* set repeat to 1 */

/*
 * Get the next nybble.  This is an expression rendition of
 *	if (--pk->pk_1nyb < 0) {
 *		pk->pk_1nyb = 1;
 *		pk->pk_c = nextbyte(pk);
 *		return (pk->pk_c >> 4);
 *	} else
 *		return (pk->pk_c & 0xf);
 */
#define	nextnyb(pk) \
	(--(pk)->pk_1nyb < 0 ? \
	 ((pk)->pk_1nyb = 1, ((pk)->pk_c = nextbyte(pk)) >> 4) : \
	 (pk)->pk_c & 0xf)

/*
 * Get the pk_details from font f.
 */
#define	ftopk(f) ((struct pk_details *)(f)->f_details)

static int scan_characters();

#if !defined( WIN32 ) && !defined( _AMIGA )
extern	int errno;
char	*malloc();
#endif

/*
 * PK subroutines.
 */

/*
 * Unpack a packed number.
 */
static int
pk_unpack(pk)
	register struct pk_details *pk;
{
	register int i, j;

top:
	if ((i = nextnyb(pk)) == PK_LONGNUM) {
#if PK_LONGNUM != 0		/* this may be silly, but . . . */
		i = 0;
#endif
		/*
		 * Expand a long number.  There are one fewer leading
		 * zeros than there are nonzero digits to obtain, so
		 * count up the leading zeros, add one, and get that
		 * many digits.  (The `digits' are hexadecimal values.)
		 */
		do {
			i++;
		} while ((j = nextnyb(pk)) == 0);
		while (--i >= 0) {
			j <<= 4;
			j += nextnyb(pk);
		}
		return (j - 15 + (13 - pk->pk_dyn_f) * 16 + pk->pk_dyn_f);
	}
	if (i <= pk->pk_dyn_f)
		return (i);
	if (i < PK_REPEAT)
		return ((i - pk->pk_dyn_f - 1) * 16 + nextnyb(pk) +
			pk->pk_dyn_f + 1);

	/*
	 * There is a repeat count, either one or a packed number.
	 * Get it first, then start over.  (tail recursion)
	 */
	if (i == PK_REPEAT)
		pk->pk_repeat = pk_unpack(pk);
	else
		pk->pk_repeat = 1;
	goto top;
}

/*
 * Skip over special commands (PK_XXX?, PK_YYY).
 */
static void
skip_specials(f)
	struct font *f;
{
	struct pk_details *pk = ftopk(f);
	register char *p = pk->pk_ptr;
	register i32 i;

	for (;;) {
		switch (UnSign8(*p++)) {

		case PK_XXX1:
			i = UnSign8(*p++);
			p += i;
			break;

		case PK_XXX2:
			pGetWord(p, i);
			p += i;
			break;

		case PK_XXX3:
			pGet3Byte(p, i);
			p += i;
			break;

		case PK_XXX4:
			pGetLong(p, i);
			p += i;
			break;

		case PK_YYY:
			p += 4;
			break;

		case PK_NO_OP:
			break;

		case PK_PRE:
			error(1, 0, "unexpected PK_PRE in \"%s\"", f->f_path);
			break;

		default:
			p--;
			if (PK_IsUndef(UnSign8(*p)))
				error(1, 0, "invalid opcode %d in \"%s\"",
					f->f_path);
			pk->pk_ptr = p;
			return;
		}
	}
}

/*
 * Read a PK file.
 */
static int
pk_read(f, fd)
	register struct font *f;
	int fd;
{
	register struct pk_details *pk;
	register char *p;
	int i, minc;
	struct stat st;
	char *reason;

	pk = NULL;		/* prepare for failure */
	reason = NULL;
	(void) fstat(fd, &st);
	if (st.st_size < 4) {	/* ??? */
		reason = "file is too small";
		goto fail;
	}
	if ((pk = (struct pk_details *)malloc(sizeof (*pk))) == NULL)
		goto fail;
	pk->pk_morec = NULL;
	if ((pk->pk_base = malloc(st.st_size)) == NULL)
		goto fail;
	if (read(fd, pk->pk_base, st.st_size) != st.st_size)
		goto fail;
	pk->pk_ptr = pk->pk_base;
	if (nextbyte(pk) != PK_PRE) {
		reason = "file does not begin with PK_PRE";
		goto fail;
	}
	if (nextbyte(pk) != PK_ID) {
		reason = "bad PK_ID";
		goto fail;
	}
	i = nextbyte(pk);
	p = pk->pk_ptr + i;	/* skip comment */
	pGetLong(p, f->f_design_size);
	pGetLong(p, f->f_checksum);
	pGetLong(p, f->f_hppp);
	pGetLong(p, f->f_vppp);
	pk->pk_ptr = p;

	f->f_details = (char *)pk;

	/* scan the characters, fail if necessary */
	if (scan_characters(f, &reason))
		goto fail;

	/* ignore the postamble */

	/* COMPRESS pk->pk_base DATA? */

	if (FontHasGlyphs(f, pk->pk_minc, pk->pk_maxc + 1))
		goto fail;
	(void) close(fd);
	return (0);

fail:
	if (reason) {
		error(0, 0, "%s", reason);
		error(0, 0, "(are you sure %s is a PK file?)", f->f_path);
		errno = 0;
	}
	if (pk != NULL) {
		if (pk->pk_base != NULL)
			free(pk->pk_base);
		if (pk->pk_morec != NULL)
			free((char *)pk->pk_morec);
		free((char *)pk);
	}
	(void) close(fd);
	return (-1);
}

/*
 * Scan through the characters in the PK file, and set the offsets
 * and preamble types for each of the character packets.
 */
static int
scan_characters(f, reason)
	struct font *f;
	char **reason;
{
	register struct pk_details *pk = ftopk(f);
	register i32 c, pl;
	register char *p;
	register struct cp *cp;
	int type;

#ifdef lint
	/* reason will be used someday ... I think */
	reason = reason;
#endif

	/* set up the minimisers and the glyph count */
	pk->pk_minc = 1;
	pk->pk_maxc = 0;
	pk->pk_gleft = 0;

	/* mark all character packets as untouched */
	for (cp = pk->pk_cpack, c = MAXSTD; --c >= 0; cp++)
		cp->cp_packet = NULL;

	/*
	 * Loop through the packets until we reach a POST, skipping
	 * the glyph instructions themselves after each definition,
	 * and specials (if any) before each.
	 */
	for (;; pk->pk_ptr = p + pl) {
		skip_specials(f);
		p = pk->pk_ptr;
		if ((c = pgetbyte(p)) == PK_POST)
			break;	/* whoops, done after all */

		/*
		 * Look at the low three bits to decide preamble size.
		 * A value of 7 is a `long preamble'; 4, 5, and 6 are
		 * `extended short preambles'; and 0, 1, 2, and 3 are
		 * `short preambles'.
		 *
		 * We ignore most of the preamble, reading only the
		 * `packet length' and the character code at this time.
		 */
		switch (c & 7) {

		case 7:		/* long */
			type = CP_LONG;
			pGetLong(p, pl);
			pGetLong(p, c);
			break;

		case 6:
		case 5:
		case 4:		/* extended short */
			type = CP_EXT_SHORT;
			pGetWord(p, pl);
			pl += (c & 3) << 16;
			c = pgetbyte(p);
			break;

		default:	/* short */
			type = CP_SHORT;
			pl = ((c & 3) << 8) + pgetbyte(p);
			c = pgetbyte(p);
			break;
		}

		if (c >= MAXSTD) {
			/*
			 * BEGIN XXX - should alloc pk_morec, but is hard
			 * and not now useful
			 */
			error(0, 0, "ignoring character %d in \"%s\"",
				f->f_path);
			error(0, 0, "because some darn programmer was lazy!");
			continue;
			/* END XXX */
		} else
			cp = &pk->pk_cpack[c];

		cp->cp_packet = pk->pk_ptr;
		cp->cp_type = type;

		/* adjust range */
		if (c < pk->pk_minc)
			pk->pk_minc = c;
		if (c > pk->pk_maxc)
			pk->pk_maxc = c;

		pk->pk_gleft++;	/* and count the glyph */
	}
	return (0);		/* no problems */
}

/*
 * Obtain the specified range of glyphs.
 */
static int
pk_getgly(f, l, h)
	register struct font *f;
	int l, h;
{
	register struct pk_details *pk = ftopk(f);
	register char *p;
	register struct glyph *g;
	register int i;
	register struct cp *cp;

	if (pk == NULL)
		panic("pk_getgly(%s)", f->f_path);
	for (i = l; i < h; i++) {
		if (i < MAXSTD)
			cp = &pk->pk_cpack[i];
		else {
			if (i > pk->pk_maxc)
				panic("pk_getgly(%s, %d)", f->f_path, i);
			cp = &pk->pk_morec[i - MAXSTD];
		}
		p = cp->cp_packet;
		if (p == NULL)	/* glyph is not valid */
			continue;
		g = f->f_gly[i];
		p++;		/* skip flag */
		switch (cp->cp_type) {

		case CP_LONG:
			p += 8;	/* skip packet len, character code */
			pGetLong(p, g->g_rawtfmwidth);
			pGetLong(p, g->g_xescapement);
			pGetLong(p, g->g_yescapement);
			pGetLong(p, g->g_width);
			pGetLong(p, g->g_height);
			pGetLong(p, g->g_xorigin);
			pGetLong(p, g->g_yorigin);
			break;

		case CP_EXT_SHORT: {
			i32 dm;

			p += 3;	/* skip packet len, character code */
			pGet3Byte(p, g->g_rawtfmwidth);
			pGetWord(p, dm);
			g->g_xescapement = dm << 16;
			g->g_yescapement = 0;
			pGetWord(p, g->g_width);
			pGetWord(p, g->g_height);
			pGetWord(p, g->g_xorigin);
			g->g_xorigin = Sign16(g->g_xorigin);
			pGetWord(p, g->g_yorigin);
			g->g_yorigin = Sign16(g->g_yorigin);
			break;
		}

		case CP_SHORT:
			p += 2;	/* skip packet len, character code */
			pGet3Byte(p, g->g_rawtfmwidth);
			g->g_xescapement = pgetbyte(p) << 16;
			g->g_yescapement = 0;
			g->g_width = pgetbyte(p);
			g->g_height = pgetbyte(p);
			g->g_xorigin = pgetbyte(p);
			g->g_xorigin = Sign8(g->g_xorigin);
			g->g_yorigin = pgetbyte(p);
			g->g_yorigin = Sign8(g->g_yorigin);
			break;
		}
		g->g_flags = GF_VALID;
		g->g_un.g_details = p;
	}
	return (0);
}

/*
 * Bit masks for pk_rasterise().
 */
static char bmask[8] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};
static char rbits[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

/*
 * Obtain rasters for the specified glyphs.
 */
static int
pk_rasterise(f, l, h)
	struct font *f;
	int l, h;
{
	struct pk_details *pk0;
	struct glyph *g0;
	char *p0, *rp0;
	int flag, ii;

	if ((pk0 = ftopk(f)) == NULL)
		panic("pk_rasterise(%s)", f->f_path);
	for (ii = l; ii < h; ii++) {
		{
			register struct glyph *g;
			register char *p;
			register int i;

			g = f->f_gly[i = ii];
			if ((g->g_flags & GF_VALID) == 0)
				continue;	/* no glyph */
			if (!HASRASTER(g))	/* empty raster */
				goto done;

			/*
			 * Allocate a raster.
			 */
			rp0 = malloc(((g->g_width + 7) >> 3) * g->g_height);
			if ((g->g_raster = rp0) == NULL)
				return (-1);/* ??? */
			g->g_rotation = ROT_NORM;

			/*
			 * Pick up the flag byte, then start at the real
			 * packet, which we saved in g_details.
			 */
			if (i < MAXSTD)
				p = pk0->pk_cpack[i].cp_packet;
			else
				p = pk0->pk_morec[i - MAXSTD].cp_packet;
			flag = UnSign8(*p);
			p0 = g->g_un.g_details;
			g0 = g;
		}
		if ((pk0->pk_dyn_f = flag >> 4) == 14) {
			register char *p = p0, *rp = rp0;
			register int j, ls, rs, i, w;

			/*
			 * Expand a bit-packed representation.
			 * If we get lucky, it is byte packed and
			 * we can just copy it over.
			 */
			i = g0->g_height;
			j = g0->g_width;
			if ((j & 7) == 0) {
				bcopy(p, rp, i * (j >> 3));
				goto done;
			}

			/*
			 * No such luck.
			 */
			w = j;
			ls = 0;
			while (--i >= 0) {
				rs = 8 - ls;
				/* know j always != 8 */
				for (j = w; j > 8; j -= 8) {
					*rp = *p++ << ls;
					*rp++ |= UnSign8(*p) >> rs;
				}

				/*
				 * We need j more bits; there are rs
				 * bits available at *p.  Ask for j,
				 * which gets min(j, rs).
				 */
				*rp++ = (*p << ls) & bmask[j];
				/* account for j bits */
				ls += j; ls &= 7;
				/* then adjust j based on rs */
				j -= rs;
				/* still need j more bits */
				if (j < 0)	/* got them all */
					continue;
				p++;
				if (j == 0)	/* there were just enough */
					continue;
				/* take j more bits */
				rp[-1] |= UnSign8(*p & bmask[j]) >> rs;
			}
		} else {
			register struct pk_details *pk = pk0;
			register int on = flag & 8 ? 0xff : 0;
			register char *rowp;	/* pointer into this row */
			register int j;		/* trimmed run count */
			register int k;		/* misc */
			register int b;		/* bit index in current col */
			register int i;		/* run count */
			register int colsleft;	/* columns left this row */
			register int rowsleft;	/* rows left */
			static char *row;	/* a one-row buffer */
			static int rowsize;	/* and its size in bytes */
			int wb;			/* row width in bytes */

			wb = (g0->g_width + 7) >> 3;
			if (rowsize < wb) {	/* get more row space */
				if (row)
					free(row);
				/* keep a slop byte */
				row = malloc((unsigned) (wb + 1));
				if (row == NULL)
					return (-1);	/* ??? */
				rowsize = wb;
			}
			bzero(row, wb);
			rowsleft = g0->g_height;
			colsleft = g0->g_width;
			pk->pk_repeat = 0;
			pk->pk_ptr = p0;
			pk->pk_1nyb = 0;	/* start on nybble 0 */
			rowp = row;
			b = 0;
			while (rowsleft > 0) {	/* do all rows */
				/* EXPAND IN LINE? */
				i = pk_unpack(pk);
				/*
				 * Work until the run count is exhausted
				 * (or at least pretty tired).
				 *
				 * (Okay, so the joke is stolen!)
				 */
				while ((j = i) > 0) {
					/*
					 * If the count is more than the
					 * rest of this row, trim it down.
					 */
					if (j > colsleft)
						j = colsleft;
					i -= j;	/* call them done */
					/*
					 * We need k=8-b bits to finish
					 * up the current byte.  If we
					 * can finish it, do so; the proper
					 * bits to set are in rbits[k].
					 */
					if (j >= (k = 8 - b)) {
						j -= k;
						colsleft -= k;
						*rowp++ |= on & rbits[k];
						b = 0;
					}
					/*
					 * Set any full bytes.
					 */
					while (j >= 8) {
						*rowp++ = on;
						j -= 8;
						colsleft -= 8;
					}
					/*
					 * Finally, begin a new byte, or
					 * add to the current byte, with
					 * j more bits.  We know j <= 8-b.
					 * (If j==0, we may be writing on
					 * our slop byte, which is why we
					 * keep one around....)
					 */
if (j > 8-b) panic("pk_rasterise j>8-b");
					*rowp |= (on & bmask[j]) >> b;
					colsleft -= j;
					b += j; b &= 7;
					if (colsleft == 0) {
						pk->pk_repeat++;
						rowsleft -= pk->pk_repeat;
						while (--pk->pk_repeat >= 0) {
							bcopy(row, rp0, wb);
							rp0 += wb;
						}
if (rowsleft == 0 && i) panic("pk_rasterise leftover bits");
						pk->pk_repeat = 0;
						rowp = row;
						colsleft = g0->g_width;
						bzero(row, wb);
						b = 0;
					}
				}
				on = 0xff - on;
			}
		}

done:
		/*
		 * Successfully converted another glyph.
		 */
		pk0->pk_gleft--;
	}

if (pk0->pk_gleft < 0)
panic("%s: gleft==%d", f->f_path, pk0->pk_gleft);
	if (pk0->pk_gleft == 0) {
		free(pk0->pk_base);
		if (pk0->pk_morec != NULL)
			free((char *) pk0->pk_morec);
		free((char *) pk0);
		f->f_details = NULL;
	}
	return (0);
}

/*
 * Discard the font details.
 */
static void
pk_freefont(f)
	struct font *f;
{
	register struct pk_details *pk = ftopk(f);

	if (pk != NULL) {
		free(pk->pk_base);
		if (pk->pk_morec != NULL)
			free((char *) pk->pk_morec);
		free((char *) pk);
	}
}
