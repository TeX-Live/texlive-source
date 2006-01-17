/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/gfclass.c,v 2.3 89/08/22 21:52:30 chris Exp $";
#endif

/*
 * gfclass - GF code classification tables.
 */

#include "gfclass.h"

/* shorthand---in lowercase for contrast (read on!) */
#define	three(x)	x, x, x
#define	four(x)		x, x, x, x
#define	five(x)		four(x), x
#define	six(x)		four(x), x, x
#define	eight(x)	four(x), four(x)
#define	sixteen(x)	eight(x), eight(x)
#define	thirty_two(x)	sixteen(x), sixteen(x)
#define	sixty_four(x)	thirty_two(x), thirty_two(x)
#define	one_twenty_eight(x) sixty_four(x), sixty_four(x)
#define	one_sixty_five(x) one_twenty_eight(x), thirty_two(x), five(x)

/*
 * Length of the single (or first) operand, if any.
 */
char gf_oplen[256] = {
	sixty_four(GPL_NONE),	/* GF_PAINT_0 through GF_PAINT_63 */
	GPL_UNS1,		/* GF_PAINT1 */
	GPL_UNS2,		/* GF_PAINT2 */
	GPL_UNS3,		/* GF_PAINT3 */
	GPL_NONE,		/* GF_BOC */
	GPL_NONE,		/* GF_BOC1 */
	GPL_NONE,		/* GF_EOC */
	GPL_NONE,		/* GF_SKIP0 */
	GPL_UNS1,		/* GF_SKIP1 */
	GPL_UNS2,		/* GF_SKIP2 */
	GPL_UNS3,		/* GF_SKIP3 */
	one_sixty_five(GPL_NONE),/* GF_NEW_ROW_0 through GF_NEW_ROW_164 */
	GPL_UNS1,		/* GF_XXX1 */
	GPL_UNS2,		/* GF_XXX2 */
	GPL_UNS3,		/* GF_XXX3 */
	GPL_SGN4,		/* GF_XXX4 */
	GPL_SGN4,		/* GF_YYY */
	GPL_NONE,		/* GF_NOP */
	GPL_NONE,		/* GF_CHAR_LOC */
	GPL_NONE,		/* GF_CHAR_LOC0 */
	GPL_NONE,		/* GF_PRE */
	GPL_NONE,		/* GF_POST */
	GPL_NONE,		/* GF_POSTPOST */
	six(GPL_NONE)		/* 250 through 255 */
};

/*
 * Types of the various opcodes.
 */
char gf_gt[256] = {
	sixty_four(GT_PAINT0),	/* GF_PAINT_0 through GF_PAINT_63 */
	three(GT_PAINT),	/* GF_PAINT1 through GF_PAINT3 */
	GT_BOC,			/* GF_BOC */
	GT_BOC1,		/* GF_BOC1 */
	GT_EOC,			/* GF_EOC */
	GT_SKIP0,		/* GF_SKIP0 */
	three(GT_SKIP),		/* GF_SKIP1 through GF_SKIP3 */
	one_sixty_five(GT_NEW_ROW),/* GF_NEW_ROW_0 throgh GF_NEW_ROW_164 */
	four(GT_XXX),		/* GF_XXX1 through GF_XXX4 */
	GT_YYY,			/* GF_YYY */
	GT_NOP,			/* GF_NOP */
	GT_CHAR_LOC,		/* GF_CHAR_LOC */
	GT_CHAR_LOC0,		/* GF_CHAR_LOC0 */
	GT_PRE,			/* GF_PRE */
	GT_POST,		/* GF_POST */
	GT_POSTPOST,		/* GF_POSTPOST */
	six(GT_UNDEF)		/* 250 through 255 */
};
