/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/tfm.c,v 3.1 89/08/22 22:00:12 chris Exp $";
#endif

/*
 * TFM file reading routines.
 *
 * TODO:
 *	finish
 */

#include <stdio.h>
#include "types.h"
#include "fio.h"
#include "tfm.h"

static int trd_header(), trd_ci(), trd_fix();

#if !defined( WIN32 ) && !defined( _AMIGA )
char	*malloc();
#endif

#define	ALLOC(n, type)	((type *)malloc((unsigned)((n) * sizeof(type))))

int
readtfmfile(f, t, stopafterwidth)
	register FILE *f;
	register struct tfmdata *t;
	int stopafterwidth;	/* ??? */
{
	i32 nc;

	if (trd_header(f, &t->t_hdr))
		return (-1);
	nc = t->t_hdr.th_ec - t->t_hdr.th_bc + 1;

	t->t_ci = NULL;
	t->t_width = NULL;
	t->t_height = NULL;
	t->t_depth = NULL;

	(void) fseek(f, t->t_hdr.th_lh * 4L, 1);	/* XXX */

	if ((t->t_ci = ALLOC(nc, struct char_info_word)) == NULL ||
	    trd_ci(f, nc, t->t_ci) ||
	    (t->t_width = ALLOC(t->t_hdr.th_nw, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nw, t->t_width))
		goto bad;
	if (stopafterwidth)
		return (0);
	if ((t->t_height = ALLOC(t->t_hdr.th_nh, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nh, t->t_height) ||
	    (t->t_depth = ALLOC(t->t_hdr.th_nd, i32)) == NULL ||
	    trd_fix(f, t->t_hdr.th_nd, t->t_depth))
		goto bad;
	return (0);

bad:
	if (t->t_ci != NULL)
		free((char *) t->t_ci);
	if (t->t_width != NULL)
		free((char *) t->t_width);
	if (t->t_height != NULL)
		free((char *) t->t_height);
	if (t->t_depth != NULL)
		free((char *) t->t_depth);
	return (-1);
}

static int
trd_header(f, th)
	register FILE *f;
	register struct tfmheader *th;
{
	register i32 *p;

	for (p = &th->th_lf; p <= &th->th_np; p++)
		fGetWord(f, *p);
	if (feof(f))
		return (-1);
	return (0);
}

static int
trd_ci(f, nc, ci)
	register FILE *f;
	register int nc;
	register struct char_info_word *ci;
{

	while (--nc >= 0) {
		ci->ci_width = fgetbyte(f);
		ci->ci_h_d = fgetbyte(f);
		ci->ci_i_t = fgetbyte(f);
		ci->ci_remainder = fgetbyte(f);
		ci++;
	}
	if (feof(f))
		return (-1);
	return (0);
}

static int
trd_fix(f, nf, p)
	register FILE *f;
	register int nf;
	register i32 *p;
{

	while (--nf >= 0) {
		fGetLong(f, *p);
		p++;
	}
	if (feof(f))
		return (-1);
	return (0);
}
