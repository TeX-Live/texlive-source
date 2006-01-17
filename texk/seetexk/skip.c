/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/skip.c,v 3.1 89/08/22 21:45:13 chris Exp $";
#endif

#include <stdio.h>
#include "types.h"
#include "fio.h"

/*
 * Skip a font definition.  The font number has already been consumed.
 */
SkipFontDef(fp)
	register FILE *fp;
{
	register int i;

	(void) GetLong(fp);	/* checksum */
	(void) GetLong(fp);	/* mag */
	(void) GetLong(fp);	/* designsize */
	i = UnSign8(GetByte(fp)) + UnSign8(GetByte(fp));
	while (--i >= 0)
		(void) GetByte(fp);
}
