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
 * File I/O subroutines for getting bytes, words, 3bytes, and longwords.
 * N.B.: these believe they are working on a DVI file.
 */

#include "types.h"
#include "error.h"
#include "fio.h"
#include <stdio.h>

static char eofmsg[] = "unexpected EOF, help";

/* for symmetry: */
#define	fGetByte(fp, r)	((r) = getc(fp))
#define	Sign32(i)	(i)

#define make(name, func, signextend) \
i32 \
name(FILE *fp) \
{ \
	register i32 n; \
 \
	func(fp, n); \
	if (feof(fp)) \
		error(1, 0, eofmsg); \
	return (signextend(n)); \
}

make(GetByte,  fGetByte,  Sign8)
make(GetWord,  fGetWord,  Sign16)
make(Get3Byte, fGet3Byte, Sign24)
make(GetLong,  fGetLong,  Sign32)
