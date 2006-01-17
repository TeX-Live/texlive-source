/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * GF classification codes
 */

/*
 * Predicate for simple paint commands.  This is presumably the most
 * common GF operation; it may be profitable to check for this before
 * switching out on the command type.
 */
#define	GF_IsPaint(c)	((c) < 64)

/*
 * Symbolic names for command `types', as returned by the macro
 * GT_TYPE(int c).
 */
#define GT_PAINT0	0	/* paint without parameter */
#define	GT_PAINT	1	/* paint with parameter */
#define GT_BOC		2	/* long BOC */
#define	GT_BOC1		3	/* short BOC */
#define GT_EOC		4	/* EOC */
#define GT_SKIP0	5	/* parameterless SKIP */
#define	GT_SKIP		6	/* parmeterised SKIP */
#define GT_NEW_ROW	7	/* NEW_ROW_n */
#define GT_XXX		8	/* XXXn */
#define GT_YYY		9	/* YYY */
#define	GT_NOP		10	/* no op */
#define GT_CHAR_LOC	11	/* CHAR_LOC */
#define	GT_CHAR_LOC0	12	/* abbreviated CHAR_LOC */
#define	GT_PRE		13
#define	GT_POST		14
#define	GT_POSTPOST	15
#define	GT_UNDEF	16

/*
 * Symbolic names for parameter lengths, obtained via the macro
 * GT_OpLen(int c).
 */
#define	GPL_NONE	0	/* no parameter, or too complex */
#define	GPL_UNS1	1	/* one one-byte parameter in 0..255 */
#define	GPL_UNS2	2	/* one two-byte parameter in 0..65535 */
#define	GPL_UNS3	3	/* one three-byte parameter in 0..16777215 */
#define	GPL_SGN4	4	/* one four-byte signed parameter */
/*
 * there are no unsigned four byte parameters, and no shorter signed
 * parameters
 */

#define GF_OpLen(code)	(gf_oplen[code])
#define GF_TYPE(code)	(gf_gt[code])
extern char gf_oplen[];
extern char gf_gt[];
