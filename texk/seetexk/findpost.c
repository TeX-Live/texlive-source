/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/findpost.c,v 2.3 89/08/22 21:50:11 chris Exp $";
#endif

/*
 * FindPostAmble - Find the postamble of a DVI file.
 *
 * N.B.: This routine assumes that ftell() returns byte offsets,
 * not magic cookies.
 */

#include <stdio.h>
#include "types.h"
#include "dvicodes.h"
#include "fio.h"
#include "num.h"

/*
 * The end of the file consists of a four-byte postamble pointer,
 * followed by the DVI_VERSION byte, followed by at least four
 * bytes of DVI_FILLER, and at least enough bytes to make the file
 * size a multiple of four.  Normally, this would be between 4 and 7
 * inclusive, but some versions of TeX append huge amounts of padding.
 */
#define POSTSIZE	512	/* how many bytes to read at a time */

#ifndef WIN32
long	ftell();		/* should be declared in stdio.h */
#endif

FindPostAmble(f)
	register FILE *f;
{
	register long offset;
	register char *p;
	register int i;
	register i32 n;
	char postbuf[POSTSIZE];

	/*
	 * Working backwards from end-of-file, read POSTSIZE bytes.
	 * Keep looking until we find something or run out of filler.
	 */
	fseek(f, 0L, 2);
	offset = ftell(f);
	do {
		if ((offset -= POSTSIZE) < 0L)
			offset = 0L;	/* no negative fseeks please */
		fseek(f, offset, 0);
		if ((i = fread(postbuf, sizeof(char), POSTSIZE, f)) <= 0)
			break;
		for (p = postbuf + i; --i >= 0;)
			if (UnSign8(*--p) != DVI_FILLER)
				goto found_something;
	} while (offset);
	return (-1);		/* ran out of filler: not a DVI file */

	/*
	 * We found something other than a filler byte at *p (which is
	 * the same as postbuf[i]).  It had better be a version byte.
	 * If so, we want to start at the byte four bytes before that,
	 * and get one long; that will tell us where the postamble
	 * begins.
	 */
found_something:
	if (UnSign8(*p) != DVI_VERSION)
		return (-1);	/* not a DVI file */
	if ((i -= 4) >= 0) {
		/* it is in the buffer, so just fish it out */
		p -= 4;
		pGetLong(p, n);
	} else {
		if ((offset += i) < 0L)
			return (-1);	/* tsk */
		fseek(f, offset, 0);
		fGetLong(f, n);
	}
	offset = n;
	fseek(f, offset, 0);
	return (0);		/* success */
}
