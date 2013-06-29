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
 * Common errors (`gripes').
 */

#include "types.h"
#include "error.h"
#include "gripes.h"
#include <stdio.h>

#ifndef WIN32
extern int errno;
#endif

/*
 * Cannot allocate memory.
 */
void
GripeOutOfMemory(int n, const char *why)
{

	error(1, -1, "ran out of memory allocating %d bytes for %s", n, why);
	/* NOTREACHED */
}

/*
 * Cannot get a font.
 * RETURNS TO CALLER
 */
void
GripeCannotGetFont(const char *name, i32 mag, i32 dsz, const char *dev, const char *fullname)
{
	int e = errno;
	char scale[40];

	if (mag == dsz)		/* no scaling */
		scale[0] = 0;
	else
		(void) sprintf(scale, " scaled %d",
			(int) ((double) mag / (double) dsz * 1000.0 + .5));

	error(0, e, "no font for %s%s", name, scale);
	if (fullname)
		error(0, 0, "(wanted, e.g., \"%s\")", fullname);
	else {
		if (dev)
			error(1, 0, "(there are no fonts for the %s engine!)",
				dev);
		else
			error(1, 0, "(I cannot find any fonts!)");
		/* NOTREACHED */
	}
}

/*
 * Font checksums do not match.
 * RETURNS TO CALLER
 */
void
GripeDifferentChecksums(const char *font, i32 tfmsum, i32 fontsum)
{

	error(0, 0, "\
WARNING: TeX and I have different checksums for font\n\
\t\"%s\"\n\
\tPlease notify your TeX maintainer\n\
\t(TFM checksum = 0%lo, my checksum = 0%lo)",
		font, (long)tfmsum, (long)fontsum);
}

/*
 * A font, or several fonts, are missing, so no output.
 */
void
GripeMissingFontsPreventOutput(int n)
{
	static char s[2] = {'s', 0};

	error(1, 0, "%d missing font%s prevent%s output (sorry)", n,
		n > 1 ? s : &s[1], n == 1 ? s : &s[1]);
	/* NOTREACHED */
}
