/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/tfmfont.c,v 3.4 89/11/29 02:29:58 chris Exp $";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "types.h"
#include "conv.h"
#include "font.h"
#include "tfm.h"

/*
 * TFM font operations.  This defines three fonts:
 *
 *	box   - prints as little square boxes, outlining what TeX
 *		thinks is the character; carries a warning.
 *	blank - prints as entirely blank; carries a warning.
 *	tfm   - prints as entirely blank; can be used for built-in fonts.
 *	invis - an alias for tfm.
 *
 * The first two also complain that no font is available in the
 * requested size; these are intended to be used as a last resort
 * so that users can always print DVI files.  You should configure
 * in exactly one of box or blank.
 *
 * TODO:
 *	base box edge widths on Conversion.c_dpi
 */
static int box_read(), blank_read(), tfm_read();
static int tfm_getgly(), tfm_rasterise();
static void tfm_freefont();

	/* magnifications are unused in tfm fonts */
struct	fontops boxops =	/* `boxtops'?  Is this a cereal driver? */
 { "box", 1, 0.0, box_read, tfm_getgly, tfm_rasterise, tfm_freefont };
struct	fontops blankops =
 { "blank", 2, 0.0, blank_read, tfm_getgly, tfm_rasterise, tfm_freefont };
struct	fontops tfmops =
 { "tfm", 0, 0.0, tfm_read, tfm_getgly, tfm_rasterise, tfm_freefont };
struct	fontops invisops =
 { "invis", 0, 0.0, tfm_read, tfm_getgly, tfm_rasterise, tfm_freefont };

/*
 * Local info.
 */
struct tfm_details {
	int	tfm_edge;		/* box edge widths, in pixels */
	struct	tfmdata tfm_data;	/* the TFM file data */
};

/*
 * Get the tfm_details from font f.
 */
#define	ftotfm(f) ((struct tfm_details *)(f)->f_details)

static int do_read();

#if !defined( WIN32 ) && !defined( _AMIGA )
extern	int errno;
char	*malloc();
#endif

/*
 * Read a Box font.
 */
static int
box_read(f, fd)
	struct font *f;
	int fd;
{

	return (do_read(f, fd, 0));
}

/*
 * Read a Blank font.
 */
static int
blank_read(f, fd)
	struct font *f;
	int fd;
{

	return (do_read(f, fd, 1));
}

/*
 * Read a TFM font.
 */
static int
tfm_read(f, fd)
	struct font *f;
	int fd;
{

	return (do_read(f, fd, 1));
}

/*
 * Read a TFM font.  It is blank if `blank'.
 */
static int
do_read(f, fd, blank)
	register struct font *f;
	int fd, blank;
{
	register struct tfm_details *tfm;
	FILE *fp;

	if ((fp = fdopen(fd, "r")) == NULL) {
		(void) close(fd);
		return (-1);
	}
	if ((tfm = (struct tfm_details *)malloc(sizeof *tfm)) == NULL)
		goto fail;
	if (readtfmfile(fp, &tfm->tfm_data, blank))
		goto fail;
	if (blank)
		tfm->tfm_edge = 0;
	else
		tfm->tfm_edge = 2;	/* XXX should be based on dpi */
	if (FontHasGlyphs(f, tfm->tfm_data.t_hdr.th_bc,
			  tfm->tfm_data.t_hdr.th_ec + 1))
		goto fail;
	f->f_checksum = 0;		/* ??? */
	f->f_design_size = 0;		/* ??? */
	f->f_hppp = 0;
	f->f_vppp = 0;
	f->f_details = (char *)tfm;
	(void) fclose(fp);
	return (0);

fail:
	(void) fclose(fp);
	if (tfm != NULL)
		free((char *)tfm);
	return (-1);
}

/*
 * Obtain the specified range of glyphs.
 */
static int
tfm_getgly(f, l, h)
	register struct font *f;
	int l;
	register int h;
{
	register struct tfm_details *tfm = ftotfm(f);
	register struct glyph *g;
	register int i;
	register struct char_info_word *ci;
#define	t (&tfm->tfm_data)
	extern i32 ScaleOneWidth(i32, i32);
#define ftop(fix) fromSP(ScaleOneWidth(fix, f->f_dvimag))

	for (i = l; i < h; i++) {
		ci = &t->t_ci[i - t->t_hdr.th_bc];
		/* zero widths mark invalid characters */
		if (ci->ci_width == 0)
			continue;
		g = f->f_gly[i];
		g->g_flags = GF_VALID;
		g->g_rawtfmwidth = t->t_width[UnSign8(ci->ci_width)];
		g->g_xorigin = 0;
		if (tfm->tfm_edge != 0) {
			g->g_yorigin = ftop(t->t_height[T_CI_H(ci)]);
			g->g_width = ftop(g->g_rawtfmwidth);
			g->g_height = g->g_yorigin +
			    ftop(t->t_depth[T_CI_D(ci)]);
		} else {
			g->g_yorigin = 0;
			g->g_width = 0;
			g->g_height = 0;
		}
	}
	return (0);
#undef t
}

/*
 * Obtain rasters for the specified glyphs.
 *
 * IGNORES tfm->tfm_edge: 2 HARDCODED FOR NOW
 */
static int
tfm_rasterise(f, l, h)
	struct font *f;
	int l, h;
{
	register struct glyph *g;
	register char *p;
	register int w, j, i;
	struct tfm_details *tfm = ftotfm(f);
#define EDGE 2

	if (tfm->tfm_edge == 0)
		return (0);
if (tfm->tfm_edge != 2) panic("tfm_rasterise");
	for (i = l; i < h; i++) {
		g = f->f_gly[i];
		if ((g->g_flags & GF_VALID) == 0 || !HASRASTER(g))
			continue;
		w = (g->g_width + 7) >> 3;
		p = malloc((unsigned) (g->g_height * w));
		if (p == NULL)
			return (-1);
		g->g_raster = p;
		g->g_rotation = ROT_NORM;
		if (g->g_width < 2 * EDGE) {
			w = 2 * EDGE - g->g_width;
			for (j = g->g_height; --j >= 0;)
				*p++ = 0xf0 << w;
		} else {
			bzero(p, g->g_height * w);
			for (j = 0; j < g->g_height;) {
				if (j < EDGE || j >= g->g_height - EDGE) {
					register int k = w;

					while (--k > 0)
						*p++ = 0xff;
					*p++ = 0xff << ((8 - g->g_width) & 7);
					j++;
					continue;
				}
				/* the following depends on EDGE==2 */
				*p = 0xc0;
				p += w - ((g->g_width & 7) == 1 ? 2 : 1);
				*p++ |= 0xc0 >> ((g->g_width - EDGE) & 7);
				if ((g->g_width & 7) == 1)
					*p++ = 0x80;
				/* end dependencies */
				if (++j == EDGE && g->g_height >= 2 * EDGE) {
					register int n = g->g_height - EDGE;

					p += (n - j) * w;
					j = n;
				}
			}
		}
	}
	return (0);
}

/*
 * Discard the font details.
 */
static void
tfm_freefont(f)
	struct font *f;
{

	free(f->f_details);
}
