/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Macros to convert DVI opcodes to (hopefully) simpler values.
 */

/*
 * Large range types---code may be any value (including EOF).
 */
#define DVI_IsChar(code) ((ui32)(code) < 128)
#define DVI_IsFont(code) ((code) >= 171 && (code) < 235)

/*
 * Symbolic names for generic types (for things with parameters).
 * These are obtained via the macro DVI_DT(int c), where 0 <= c <= 255.
 */
#define	DT_CHAR		 0
#define DT_SET		 1
#define	DT_SETRULE	 2
#define DT_PUT		 3
#define	DT_PUTRULE	 4
#define	DT_NOP		 5
#define	DT_BOP		 6
#define	DT_EOP		 7
#define	DT_PUSH		 8
#define	DT_POP		 9
#define DT_RIGHT	10
#define DT_W0		11
#define	DT_W		12
#define	DT_X0		13
#define DT_X		14
#define DT_DOWN		15
#define	DT_Y0		16
#define DT_Y		17
#define	DT_Z0		18
#define DT_Z		19
#define	DT_FNTNUM	20
#define DT_FNT		21
#define DT_XXX		22
#define DT_FNTDEF	23
#define	DT_PRE		24
#define	DT_POST		25
#define	DT_POSTPOST	26
#define	DT_UNDEF	27

/*
 * Symbolic names for parameter lengths, obtained via the macro
 * DVL_OpLen(int c).
 *
 * N.B.: older drivers may assume that 0 => none, 1-4 => 1-4 bytes
 * and 5-7 => unsigned version of 1-4---so DO NOT change these values!
 */
#define	DPL_NONE	0
#define	DPL_SGN1	1
#define	DPL_SGN2	2
#define	DPL_SGN3	3
#define	DPL_SGN4	4
#define	DPL_UNS1	5
#define	DPL_UNS2	6
#define	DPL_UNS3	7
/* there are no unsigned four byte parameters */

#define DVI_OpLen(code)  (dvi_oplen[code])
#define DVI_DT(code)	 (dvi_dt[code])
extern char dvi_oplen[];
extern char dvi_dt[];
