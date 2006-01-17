/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 *
 * The code in this file was partially derived from another version by
 * Tim Morgan at UCI.
 *
 * TODO:
 *	test
 */

#ifndef lint
static char rcsid[] = "$Header$";
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
 * RST (Imagen's font format) font operations.
 */
static int rst_read(), rst_getgly(), rst_rasterise();
static void rst_freefont();

#ifdef UCI
#define D 1.0 /* I think */
#else
#define D 150.0
#endif

struct	fontops rstops =
	{ "rst", 0, D, rst_read, rst_getgly, rst_rasterise, rst_freefont };

#undef D

/*
 * Local info.
 */
#define	FILEMARK_SIZE	8	/* header `file mark' bytes to skip */

/*
 * rc describes one RST character information block.
 */
struct rc {
	i16	rc_height;	/* character height (pixels) */
	i16	rc_width;	/* character width (pixels) */
	i16	rc_yoffset;	/* Y offset of reference point */
	i16	rc_xoffset;	/* X offset of reference point */
	i32	rc_rastoff;	/* raster offset */
	i32	rc_TFMwidth;	/* TFM width */
};

/*
 * rst_details are the RST-specific font details.
 *
 * We keep track of the number of characters converted to internal
 * glyph form, and when all have been done, we discard the now-
 * useless details.
 */
struct rst_details {
	int	rd_unconv;	/* number of characters unconverted */
	struct	rc *rd_rc;	/* `font directory' */
	char	*rd_ras;	/* raster space, until converted */
	long	rd_rastbase;	/* adjust rc_rastoff by rastbase */
};

/*
 * Get the rst_details from font f.
 */
#define	ftord(f) ((struct rst_details *)(f)->f_details)

#if !defined( WIN32 ) && !defined( _AMIGA )
extern	errno;
char	*malloc();
long	ftell();
#endif

/*
 * Read an RST file.
 */
static int
rst_read(f, fd)
	struct font *f;
	int fd;
{
	register struct rst_details *rd;
	register FILE *fp;
	register struct rc *rc;
	register int i;
	int saverr;
	long presize, diroff;
	i32 firstgly, lastgly;

	fp = NULL;
	if ((rd = (struct rst_details *)malloc(sizeof *rd)) == 0)
		goto fail;
	rd->rd_ras = 0;
	rd->rd_rc = 0;

	/*
	 * Read the header.
	 */
	errno = 0;		/* try to make errno meaningful */
	if ((fp = fdopen(fd, "r")) == NULL) {
		if (errno == 0)
			errno = EMFILE;
		goto fail;
	}
	(void) fseek(fp, (long)FILEMARK_SIZE, 0);
	fGetWord(fp, presize);	/* bytes in preamble (from FILEMARK_SIZE+2) */
	presize += 2 + FILEMARK_SIZE;
	i = getc(fp);		/* version */
	if (i != 0) {
		error(0, 0, "Warning: strange RST version (%d) in \"%s\"", i,
			f->f_path);
		errno = EINVAL;
		goto fail;
	}
	fGet3Byte(fp, diroff);	/* offset to glyph directory */
	fGetWord(fp, firstgly);	/* first glyph */
	fGetWord(fp, lastgly);	/* last glyph (inclusive) */
	lastgly++;		/* make it exclusive */

	(void) GetLong(fp);	/* mag */
	fGetLong(fp, f->f_design_size);
	/* interline, interword, rotation, charadv, lineadv */
	(void) fseek(fp, (long)(4 + 4 + 2 + 1 + 1), 1);
	fGetLong(fp, f->f_checksum);
	f->f_hppp = 0;		/* ??? */
	f->f_vppp = 0;

	/*
	 * Read the glyph directory.
	 */
	i = lastgly - firstgly;
	rd->rd_rc = (struct rc *)malloc((unsigned) (i * sizeof(*rc)));
	if (rd->rd_rc == 0)
		goto fail;
	(void) fseek(fp, presize, 0);
	for (rc = rd->rd_rc; --i >= 0; rc++) {
		fGetWord(fp, rc->rc_height);
		fGetWord(fp, rc->rc_width);
		fGetWord(fp, rc->rc_yoffset);
		fGetWord(fp, rc->rc_xoffset);
		fGetLong(fp, rc->rc_TFMwidth);
		fGet3Byte(fp, rc->rc_rastoff);
	}

	/*
	 * Set up the raster pointer (if we need rasters).
	 */
	if (f->f_flags & FF_RASTERS) {
		struct stat st;

		rd->rd_rastbase = ftell(fp);
		/* back to unbuffered I/O */
		(void) fstat(fd, &st);
		i = st.st_size - presize - 15 * (lastgly - firstgly);
		if (i < 0)
			error(1, 0, "calculated raster size < 0 for \"%s\"",
				f->f_path);
		if (i != 0)  {
			if ((rd->rd_ras = malloc((unsigned) i)) == 0)
				goto fail;
			(void) lseek(fd, rd->rd_rastbase, 0);
			if (read(fd, rd->rd_ras, i) != i)
				goto fail;
		}
	}

	f->f_details = (char *) rd;
	if (FontHasGlyphs(f, firstgly, lastgly))
		goto fail;

	(void) fclose(fp);
	rd->rd_unconv = lastgly - firstgly;
	return (0);

fail:
	saverr = errno;
	if (rd) {
		if (rd->rd_rc)
			free((char *)rd->rd_rc);
		if (rd->rd_ras)
			free(rd->rd_ras);
		free((char *) rd);
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
rst_getgly(f, l, h)
	register struct font *f;
	int l;
	register int h;
{
	register struct glyph *g;
	register struct rc *rc;
	register int i;
	struct rst_details *rd;

	if ((rd = ftord(f)) == NULL)
		panic("rst_getgly");

	/*
	 * By definition, all glyphs are valid.  Just copy the
	 * RST information.
	 */
	for (rc = &rd->rd_rc[(i = l) - f->f_lowch]; i < h; i++, rc++) {
		g = f->f_gly[i];
		g->g_flags = GF_VALID;
		g->g_height = rc->rc_height;
		g->g_width = rc->rc_width;
		g->g_yorigin = rc->rc_yoffset;
		g->g_xorigin = rc->rc_xoffset;
		g->g_rawtfmwidth = rc->rc_TFMwidth;
	}
	return (0);
}

/*
 * Obtain rasters for the specified glyphs.
 */
static int
rst_rasterise(f, l, h)
	register struct font *f;
	int l;
	register int h;
{
	register struct glyph *g;
	register struct rc *rc;
	register struct rst_details *rd;
	register int i;
	unsigned sz;

	if ((rd = ftord(f)) == NULL)
		panic("rst_rasterise details==0");
	if (rd->rd_ras == NULL)
		panic("rst_rasterise rd_ras==NULL");
	for (rc = &rd->rd_rc[(i = l) - f->f_lowch]; i < h; i++, rc++) {
		g = f->f_gly[i];
		if (!HASRASTER(g))
			continue;
		if (rc->rc_rastoff == 0) {
			error(1, 0, "bad RST glyph %d in \"%s\"",
				g->g_index, f->f_path);
		} else {
			sz = ((g->g_width + 7) >> 3) * g->g_height;
			if ((g->g_raster = malloc(sz)) == NULL)
				return (-1);	/* ??? */
			g->g_rotation = ROT_NORM;
			bcopy(rd->rd_ras + rc->rc_rastoff - rd->rd_rastbase,
				g->g_raster, sz);
		}
	}

	/*
	 * If we have converted all the characters, dump the
	 * pre-conversion rasters.  In fact, dump everything.
	 */
	rd->rd_unconv -= h - l;
	if (rd->rd_unconv == 0) {
		free((char *)rd->rd_rc);
		free(rd->rd_ras);
		free((char *)rd);
		f->f_details = NULL;
	}
	return (0);
}

/*
 * Discard the font details.
 */
static void
rst_freefont(f)
	struct font *f;
{
	struct rst_details *rd;

	if ((rd = ftord(f)) != NULL) {
		free((char *)rd->rd_rc);
		free(rd->rd_ras);
		free((char *)rd);
	}
}
