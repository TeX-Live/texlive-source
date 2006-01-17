/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/fio.c,v 2.4 89/08/22 21:50:31 chris Exp $";
#endif

/*
 * File I/O subroutines for getting bytes, words, 3bytes, and longwords.
 * N.B.: these believe they are working on a DVI file.
 */

#include <stdio.h>
#include "types.h"
#include "error.h"
#include "fio.h"

static char eofmsg[] = "unexpected EOF, help";

/* for symmetry: */
#define	fGetByte(fp, r)	((r) = getc(fp))
#define	Sign32(i)	(i)

#define make(name, func, signextend) \
i32 \
name(fp) \
	register FILE *fp; \
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
