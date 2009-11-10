/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Common errors (`gripes').
 */

#include <stdio.h>
#include "types.h"
#include "error.h"
#include "gripes.h"

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
