/* Copyright (c) 1987, 1989, 2012 University of Maryland Department of
   Computer Science.
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation, the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions: The above copyright notice, this permission
   notice and the disclaimer statement shall be included in all copies
   or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
#ifdef ASCIIPTEX
#define	DT_DIR		27
#define	DT_UNDEF	28
#else /* !ASCIIPTEX */
#define	DT_UNDEF	27
#endif /* !ASCIIPTEX */

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
