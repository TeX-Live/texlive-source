/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#ifndef lint
static char rcsid[] = "$Header: /usr/src/local/tex/local/mctex/lib/RCS/gripes1.c,v 1.1 89/08/22 21:54:19 chris Exp $";
#endif

/*
 * Gripes having to do with DVI files.
 */

#include <stdio.h>
#include "types.h"
#include "error.h"
#include "font.h"
#include "gripes.h"

extern char *DVIFileName;

static char *
dfn()
{

	return (DVIFileName ? DVIFileName : "the input");
}

/*
 * Save string space by declaring these here.
 */
#if __STDC__ >= 1
static const char dfl[] = "DVI file";
static const char areyousure[] = "(are you sure %s is a %s?)";
#else
static char dfl[] = "DVI file";
static char areyousure[] = "(are you sure %s is a %s?)";
#endif

/*
 * DVI file requests a font it never defined.
 */
void
GripeNoSuchFont(n)
	i32 n;
{

	error(0, 0, "%s wants font %ld, which it never defined", dfl, (long)n);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * DVI file redefines a font.
 */
void
GripeFontAlreadyDefined(n)
	i32 n;
{

	error(0, 0, "%s redefines font %ld", dfl, n);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Unexpected end of DVI file.
 */
void
GripeUnexpectedDVIEOF()
{

	GripeUnexpectedOp("end of file");
	/* NOTREACHED */
}

/*
 * Unexpected DVI opcode.
 */
void
GripeUnexpectedOp(s)
	char *s;
{

	error(0, 0, "unexpected %s in %s", s, dfl);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Missing DVI opcode.
 */
void
GripeMissingOp(s)
	char *s;
{

	error(0, 0, "missing %s in %s", s, dfl);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Cannot find DVI postamble.
 */
void
GripeCannotFindPostamble()
{

	error(0, 0, "cannot find postamble");
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Inconsistent DVI value.
 */
void
GripeMismatchedValue(s)
	char *s;
{

	error(0, 0, "mismatched %s in %s", s, dfl);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Undefined DVI opcode.
 */
void
GripeUndefinedOp(n)
	int n;
{

	error(0, 0, "undefined DVI opcode %d", n);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * DVI file requests glyph that is not in some font, or
 * when no font is set for this page.
 *
 * RETURNS TO CALLER
 */
void
GripeBadGlyph(c, f)
	i32 c;
	struct font *f;
{

	if (f->f_path == NULL) {
		error(0, 0, "bad %s: char without setfont", dfl);
		error(1, 0, "(try checking %s with dvitype)", dfn());
		/* NOTREACHED */
	}
	error(0, 0, "there is no character %ld in %s!", (long)c, f->f_path);
}
