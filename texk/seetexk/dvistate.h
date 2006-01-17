/*
 * Copyright (c) 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/* DVI file information (as used by dvi-to-device programs) */

/*
 * Units of distance are stored in scaled points, but we can convert to
 * units of 10^-7 meters by multiplying by the numbers in the preamble.
 * The values hh and vv are kept in device coordinates.
 */

/* the structure of the stack used to hold the values (h,v,hh,vv,w,x,y,z) */
typedef struct dvi_stack {
	i32	h;		/* the saved h */
	i32	v;		/* the saved v */
	i32	hh;		/* the saved hh */
	i32	vv;		/* the saved vv */
	i32	w;		/* etc */
	i32	x;
	i32	y;
	i32	z;
} DviStack;

/*
 * DVI state machine interpreter encapsulation (global state).
 */
struct dvi_state {
	/*
	 * Variables that must be set by user before calling SetDVIState().
	 */
	int	ds_usermag;	/* user-supplied (additional) magnification */
	int	ds_maxdrift;	/* maximum drift, typically 3 */

	/* the rest are set by DVISetState() */

	/*
	 * Values based on the (postamble of the) DVI file,
	 * or used by the interpreter.
	 */
	i32	ds_num;		/* numerator from DVI file */
	i32	ds_denom;	/* denominator from DVI file */
	i32	ds_dvimag;	/* magnification from DVI file */
	i32	ds_maxheight;	/* height of tallest page */
	i32	ds_maxwidth;	/* width of widest page */
	i32	ds_prevpage;	/* previous page of DVI file */
	int	ds_npages;	/* number of DVI pages */
	DviStack *ds_stack;	/* base of stack of DVI values */
	DviStack *ds_sp;	/* current place in stack */
	DviStack ds_cur;	/* current values */
	DviStack ds_fresh;	/* fresh values for new pages: not all
				   zero unless there are no margins.
				   This works because all DVI motion
				   is relative. */
	/*
	 * Although this is not strictly within the realm of DVI
	 * state, we need a place to store the fonts we allocate
	 * while reading through the postamble.  Each entry in
	 * the ds_fonts search table is a pointer to a font,
	 * so that *(struct font **)SSearch(...) is the pointer.
	 */
	struct	search *ds_fonts;/* font table */

	/*
	 * A few routines want a private copy of the DVI file pointer.
	 */
	FILE	*ds_fp;		/* input DVI file */
};

extern struct dvi_state ds;

/* shorthand */
#define	dvi_h	ds.ds_cur.h
#define	dvi_v	ds.ds_cur.v
#define	dvi_hh	ds.ds_cur.hh
#define	dvi_vv	ds.ds_cur.vv
#define	dvi_w	ds.ds_cur.w
#define	dvi_x	ds.ds_cur.x
#define	dvi_y	ds.ds_cur.y
#define	dvi_z	ds.ds_cur.z

/*
 * Set up the state for the interpreter loop.
 */
void DVISetState();

/*
 * Service routines (called from main loop).
 */
struct font *DVIFindFont();
void DVIRule(), DVIBeginPage();

/*
 * Drift fixing.  Note that the first argument must be an lvalue.
 * It is the desired position in pixels, and is maintained within
 * ds.ds_maxdrift pixels of the second argument, which is the
 * `absolute' DVI position rounded to the nearest pixel.
 */
#define	FIXDRIFT(xx, x) \
	if ((x) >= (xx)) \
		if ((x) - (xx) <= ds.ds_maxdrift) \
			/* position is in bounds */; \
		else \
			(xx) = (x) - ds.ds_maxdrift; \
	else \
		if ((xx) - (x) <= ds.ds_maxdrift) \
			/* position is in bounds */; \
		else \
			(xx) = (x) + ds.ds_maxdrift
