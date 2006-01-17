/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/pxlfont.c,v 2.5 89/08/22 21:55:34 chris Exp $";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "types.h"
#include "error.h"
#include "font.h"
#include "fio.h"

/*
 * PXL font operations.
 */
static int pxl_read(), pxl_getgly(), pxl_rasterise();
static void pxl_freefont();

struct	fontops pxlops =
	{ "pxl", 0, 5.0, pxl_read, pxl_getgly, pxl_rasterise, pxl_freefont };

/*
 * Local info.
 */
#define	PXLID	1001		/* ID denoting PXL files */
#define	TAILSIZE (517 * 4)	/* size of pxl tail info */

/*
 * pc describes one PXL character information block.
 */
struct pc {
	i16	pc_width;	/* character width (pixels) */
	i16	pc_height;	/* character height (pixels) */
	i16	pc_xoffset;	/* X offset of reference point */
	i16	pc_yoffset;	/* Y offset of reference point */
	i32	pc_rastoff;	/* raster offset */
	i32	pc_TFMwidth;	/* TFM width (FIXes) */
};

/*
 * pxl_details are the PXL-specific font details.
 *
 * We keep track of the number of characters converted to internal
 * glyph form, and when all have been done, we discard the now-
 * useless details.
 */
struct pxl_details {
	int	pd_nconv;	/* number of characters converted */
	char	*pd_ras;	/* raster space, until converted */
	struct	pc pd_pc[128];	/* `font directory' */
};

/*
 * Get the pxl_details from font f.
 */
#define	ftopd(f) ((struct pxl_details *) (f)->f_details)

extern	errno;
#ifndef KPATHSEA
char	*malloc();
#endif

/*
 * Read a PXL file.
 */
static int
pxl_read(f, fd)
	struct font *f;
	int fd;
{
	register struct pxl_details *pd;
	register FILE *fp;
	register struct pc *pc;
	register int i;
	int saverr;
	i32 pxlid;
	struct stat st;

	fp = NULL;
	if ((pd = (struct pxl_details *)malloc(sizeof (*pd))) == 0)
		goto fail;
	pd->pd_ras = 0;

	/*
	 * There should be 4n bytes, with an absolute minimum of TAILSIZE+4
	 * (+4 for the initial PXLID).
	 */
	(void) fstat(fd, &st);
	if ((st.st_size & 3) != 0 || st.st_size < (TAILSIZE + 4)) {
		errno = EINVAL;
		goto fail;
	}

	/*
	 * Set up the raster pointer (if we need rasters).
	 */
	if (f->f_flags & FF_RASTERS) {
		i = st.st_size - (TAILSIZE + 4);
		if (i != 0)  {
			if ((pd->pd_ras = malloc((unsigned) i)) == 0)
				goto fail;
			(void) lseek(fd, 4L, 0);
			if (read(fd, pd->pd_ras, i) != i)
				goto fail;
		}
	}

	/*
	 * Read the glyph information.
	 */
	errno = 0;		/* try to make errno meaningful */
	if ((fp = fdopen(fd, "r")) == NULL) {
		if (errno == 0)
			errno = EMFILE;
		goto fail;
	}
	(void) fseek(fp, (long) -TAILSIZE, 2);
	for (i = 128, pc = pd->pd_pc; --i >= 0; pc++) {
		fGetWord(fp, pc->pc_width);
		fGetWord(fp, pc->pc_height);
		fGetWord(fp, pc->pc_xoffset);
		fGetWord(fp, pc->pc_yoffset);
		fGetLong(fp, pc->pc_rastoff);
		fGetLong(fp, pc->pc_TFMwidth);
	}
	f->f_checksum = GetLong(fp);
	(void) GetLong(fp);		/* mag */
	f->f_design_size = GetLong(fp);	/* designsize */
	f->f_hppp = 0;			/* ??? maybe 200? */
	f->f_vppp = 0;
	(void) GetLong(fp);		/* dirpointer */
	pxlid = GetLong(fp);
	if (pxlid != PXLID)
		error(0, 0, "Warning: strange PXL id (%d) in \"%s\"",
			pxlid, f->f_path);

	f->f_details = (char *)pd;
	if (FontHasGlyphs(f, 0, 128))
		goto fail;
	(void) fclose(fp);
	pd->pd_nconv = 0;
	return (0);

fail:
	saverr = errno;
	if (pd) {
		if (pd->pd_ras)
			free(pd->pd_ras);
		free((char *)pd);
	}
	if (fp)
		(void) fclose(fp);
	else
		(void) close(fd);
	errno = saverr;
	return (-1);
}

/*
 * Obtain the specified range of glyphs.
 */
static int
pxl_getgly(f, l, h)
	register struct font *f;
	int l;
	register int h;
{
	register struct glyph *g;
	register struct pc *pc;
	register int i;
	struct pxl_details *pd;

	if ((pd = ftopd(f)) == NULL)
		error(1, 0, "pxl_getgly details==0: cannot happen");
	/*
	 * By definition, all 128 glyphs are valid.  Just copy the
	 * PXL information.
	 */
	for (pc = &pd->pd_pc[i = l]; i < h; i++, pc++) {
		g = f->f_gly[i];
		g->g_flags = GF_VALID;
		g->g_height = pc->pc_height;
		g->g_width = pc->pc_width;
		g->g_yorigin = pc->pc_yoffset;
		g->g_xorigin = pc->pc_xoffset;
		g->g_rawtfmwidth = pc->pc_TFMwidth;
	}
	return (0);
}

/*
 * Helper function for rasterise: return a pointer to a converted
 * (malloc()ed and minimised) raster.
 */
static char *
makeraster(h, w, rp)
	register int h, w;
	register char *rp;
{
	register char *cp;
	register int i, o;
	char *rv;

	/*
	 * The height and width values are in bits.  Convert width to
	 * bytes, rounding up.  The raw raster (at rp) is almost what
	 * we want, but not quite: it has `extra' bytes at the end of
	 * each row, to pad out to a multiple of four bytes.
	 */
	w = (w + 7) >> 3;
	o = (4 - w) & 3;	/* offset (number of `extra' bytes) */
	if ((cp = malloc((unsigned) (h * w))) == NULL)
		return (NULL);
	if (o == 0) {
		/*
		 * The raster fits exactly; just copy it to the allocated
		 * memory space.  (We must copy anyway, so that glyphs
		 * can be freed, e.g., after rotation.)
		 */
		bcopy(rp, cp, h * w);
		return (cp);
	}
	rv = cp;
	while (--h >= 0) {
		/*
		 * Copy each row, then skip over the extra stuff.
		 */
		for (i = w; --i >= 0;)
			*cp++ = *rp++;
		rp += o;
	}
	return (rv);
}

/*
 * Obtain rasters for the specified glyphs.
 */
static int
pxl_rasterise(f, l, h)
	register struct font *f;
	int l;
	register int h;
{
	register struct glyph *g;
	register struct pc *pc;
	register int i;
	register struct pxl_details *pd;

	if ((pd = ftopd(f)) == NULL)
		error(1, 0, "pxl_rasterise details==0: cannot happen");
	if (pd->pd_ras == NULL)
		error(1, 0, "pxl_rasterise pd_ras==NULL: cannot happen");
	for (pc = &pd->pd_pc[i = l]; i < h; i++, pc++) {
		g = f->f_gly[i];
		if (pc->pc_rastoff == 0) {
			/*
			 * g should not claim a raster, since it has none.
			 */
			if (HASRASTER(g))
				error(1, 0, "bad PXL glyph %d in \"%s\"",
					g->g_index, f->f_path);
		} else {
			g->g_raster = makeraster(pc->pc_height, pc->pc_width,
				pd->pd_ras + ((pc->pc_rastoff - 1) << 2));
			if (g->g_raster == NULL)
				return (-1);	/* ??? */
			g->g_rotation = ROT_NORM;
		}
	}

	/*
	 * If we have converted all the characters, dump the
	 * pre-conversion rasters.  In fact, dump everything.
	 */
	pd->pd_nconv += h - l;
	if (pd->pd_nconv == 128) {
		free(pd->pd_ras);
		free((char *)pd);
		f->f_details = NULL;
	}
	return (0);
}

/*
 * Discard the font details.
 */
static void
pxl_freefont(f)
	struct font *f;
{
	struct pxl_details *pd;

	if ((pd = ftopd(f)) != NULL) {
		if (pd->pd_ras != NULL)
			free(pd->pd_ras);
		free((char *)pd);
	}
}
