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
 * File I/O: numbers.
 *
 * We deal in fixed format numbers and (FILE *)s here.
 * For pointer I/O, see pio.h.
 *
 * N.B.: These do the `wrong thing' at EOF.  It is imperative
 * that the caller add appropriate `if (feof(fp))' statements.
 */

/*
 * Get one unsigned byte.  Note that this is a proper expression.
 * The reset have more limited contexts, and are therefore OddLy
 * CapItaliseD.
 */
#define	fgetbyte(fp)	getc(fp)

/*
 * Get a two-byte unsigned integer, a three-byte unsigned integer,
 * or a four-byte signed integer.
 */
#define fGetWord(fp, r)	((r)  = getc(fp) << 8,  (r) |= getc(fp))
#define fGet3Byte(fp,r) ((r)  = getc(fp) << 16, (r) |= getc(fp) << 8, \
			 (r) |= getc(fp))
#define fGetLong(fp, r)	((r)  = getc(fp) << 24, (r) |= getc(fp) << 16, \
			 (r) |= getc(fp) << 8,  (r) |= getc(fp))

/*
 * Fast I/O write (and regular write) macros.
 */
#define	putbyte(fp, r)	((void) putc((r), fp))

#define PutWord(fp, r)	((void) putc((r) >> 8,  fp), \
			 (void) putc((r), fp))
#define Put3Byte(fp, r)	((void) putc((r) >> 16, fp), \
			 (void) putc((r) >> 8, fp), \
			 (void) putc((r), fp))
#define PutLong(fp, r)	((void) putc((r) >> 24, fp), \
			 (void) putc((r) >> 16, fp), \
			 (void) putc((r) >> 8, fp), \
			 (void) putc((r), fp))

/*
 * Function types
 */
i32	GetByte(FILE *), GetWord(FILE *), Get3Byte(FILE *), GetLong(FILE *);
