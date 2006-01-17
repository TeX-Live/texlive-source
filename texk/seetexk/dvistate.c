/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/dvistate.c,v 1.1 89/08/22 21:49:03 chris Exp $";
#endif

/*
 * dvistate - basic DVI machine interpreter
 */

#include <stdio.h>
#include "types.h"
#include "conv.h"
#include "dvicodes.h"
#include "error.h"
#include "fio.h"
#include "font.h"
#include "gripes.h"
#include "postamble.h"
#include "search.h"
#include "seek.h"
#include "dvistate.h"

struct dvi_state ds;

/* yech, statics! */
static struct font *(*s_deffnt)();
static int s_dpi;
static int s_errs;

static void pre(), post(), fntdef();

#if !defined( WIN32 ) && !defined( _AMIGA )
char *malloc();
#endif

/*
 * Read through the postamble and the preamble and set up the
 * global DVI state.  Each font is passed to (*fontfn)().
 * Upon return, the input file (fp) is positioned at the beginning
 * of the first page.
 *
 * dpi is the resolution of the printer in pixels (dots) per inch,
 * and xoffset and yoffset are the values from the -X and -Y flags
 * (default zero).  We add 1000 to each offset (since the offsets
 * are in 1/1000ths of an inch), and then multiply by dpi and divide
 * by 1000, to get the actual offset.
 *
 * If a device introduces a constant offset, e.g., of .25 inch
 * horizontally and .31 inch vertically, the caller can compensate
 * by passing (xflag - 2500) and (yflag - 3100).
 */
void
DVISetState(fp, fontfn, dpi, xoffset, yoffset)
	FILE *fp;
	struct font *(*fontfn)();
	int dpi, xoffset, yoffset;
{

	/* make sure fseek() will work */
	if ((ds.ds_fp = SeekFile(fp)) == NULL)
		error(1, -1, "unable to copy input to temp file (disk full?)");

	/* h and v must be deferred until we have a conversion */
	ds.ds_fresh.hh = ((1000 + (i32)xoffset) * dpi) / 1000;
	ds.ds_fresh.vv = ((1000 + (i32)yoffset) * dpi) / 1000;

	ds.ds_fonts = SCreate(sizeof(struct font *));
	if (ds.ds_fonts == NULL)
		error(1, 0, "cannot create font table (out of memory?)");
	s_errs = 0;
	s_dpi = dpi;
	s_deffnt = fontfn;
	ScanPostAmble(ds.ds_fp, post, fntdef);
	if (s_errs)
		GripeMissingFontsPreventOutput(s_errs);
	pre();
}

/*
 * Read the preamble and apply sanity checks.
 * The presence of a preamble is virtually certain,
 * since we have already found and read a postamble.
 */
static void
pre()
{
	register int n;
	register FILE *fp = ds.ds_fp;

	rewind(fp);
	if (GetByte(fp) != Sign8(DVI_PRE))
		GripeMissingOp("PRE");
	if (GetByte(fp) != Sign8(DVI_VERSION))
		GripeMismatchedValue("version number");
	if (GetLong(fp) != ds.ds_num)
		GripeMismatchedValue("numerator");
	if (GetLong(fp) != ds.ds_denom)
		GripeMismatchedValue("denominator");
	if (GetLong(fp) != ds.ds_dvimag)
		GripeMismatchedValue("\\magnification");
	n = UnSign8(GetByte(fp));
	while (--n >= 0)
		(void) GetByte(fp);
}

/*
 * Store the relevant information from the DVI postamble.
 */
static void
post(p)
	register struct PostAmbleInfo *p;
{
	register int n;

	ds.ds_prevpage = p->pai_PrevPagePointer;
	ds.ds_num = p->pai_Numerator;
	ds.ds_denom = p->pai_Denominator;
	ds.ds_dvimag = p->pai_DVIMag;
	ds.ds_maxheight = p->pai_TallestPageHeight;
	ds.ds_maxwidth = p->pai_WidestPageWidth;
	ds.ds_npages = p->pai_NumberOfPages;

	/* set the global conversion factor */
	SetConversion(s_dpi,
	    ds.ds_usermag, ds.ds_num, ds.ds_denom, ds.ds_dvimag);

	ds.ds_fresh.h = toSP(ds.ds_fresh.hh);
	ds.ds_fresh.v = toSP(ds.ds_fresh.vv);
#ifdef unneeded			/* counting on init-to-zero ... */
	ds.ds_fresh.w = 0;
	ds.ds_fresh.x = 0;
	ds.ds_fresh.y = 0;
	ds.ds_fresh.z = 0;
#endif
	/* make the stack */
	n = p->pai_DVIStackSize * sizeof(DviStack);
	if ((ds.ds_stack = (struct dvi_stack *)malloc((unsigned)n)) == NULL)
		GripeOutOfMemory(n, "DVI stack");
	ds.ds_sp = ds.ds_stack;
}

/*
 * Handle a font definition from the postamble.
 */
static void
fntdef(p)
	register struct PostAmbleFont *p;
{
	register struct font *f, **fp;
	int def = S_CREATE | S_EXCL;

	fp = (struct font **)SSearch(ds.ds_fonts, p->paf_DVIFontIndex, &def);
	if (fp == NULL) {
		if (def & S_COLL)
			GripeFontAlreadyDefined(p->paf_DVIFontIndex);
		else
			error(1, 0, "cannot stash font %ld (out of memory?)",
			    (long)p->paf_DVIFontIndex);
		/* NOTREACHED */
	}

	/* apply driver function to get font */
	f = (*s_deffnt)(p->paf_name, p->paf_DVIMag, p->paf_DVIDesignSize);
	if (f == NULL) {
		s_errs++;
		return;
	}
	*fp = f;

	/* match checksums, unless one is (or both are) zero */
	if (p->paf_DVIChecksum && f->f_checksum &&
	    p->paf_DVIChecksum != f->f_checksum)
		GripeDifferentChecksums(f->f_path, p->paf_DVIChecksum,
		    f->f_checksum);
}

/*
 * Given a font index, return the corresponding pointer.
 */
struct font *
DVIFindFont(n)
	i32 n;
{
	int x = S_LOOKUP;
	char *p;

	if ((p = SSearch(ds.ds_fonts, n, &x)) == NULL) {
		GripeNoSuchFont(n);
		/* NOTREACHED */
	}
	return (*(struct font **)p);
}

/*
 * Handle a rule: read width and height, then call the given function.
 * If `advance', move dvi position afterwards.
 */
void
DVIRule(fn, advance)
	void (*fn)();
	int advance;
{
	register i32 h, w, ww;
	register FILE *fp = ds.ds_fp;

	fGetLong(fp, h);
	fGetLong(fp, ww);
	h = ConvRule(h);
	w = ConvRule(ww);
	(*fn)(h, w);
	if (advance) {
		dvi_hh += w;
		dvi_h += ww;
		w = fromSP(dvi_h);
		FIXDRIFT(dvi_hh, w);
	}
}

/*
 * Handle a BOP: read the ten \count values and the previous page pointer,
 * reset DVI position, and call the given function.
 */
void
DVIBeginPage(fn)
	void (*fn)();
{
	register int i;
	i32 count[10];

	for (i = 0; i < 10; i++)
		count[i] = GetLong(ds.ds_fp);
	ds.ds_prevpage = GetLong(ds.ds_fp);

	/* reset DVI page position and stack pointer, then call driver */
	ds.ds_cur = ds.ds_fresh;
	ds.ds_sp = ds.ds_stack;

	(*fn)(count);
}
