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
 * Gripes having to do with DVI files.
 */

#include "types.h"
#include "error.h"
#include "font.h"
#include "gripes.h"
#include <stdio.h>

static const char *
dfn(void)
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
GripeNoSuchFont(i32 n)
{

	error(0, 0, "%s wants font %ld, which it never defined", dfl, (long)n);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * DVI file redefines a font.
 */
void
GripeFontAlreadyDefined(i32 n)
{

	error(0, 0, "%s redefines font %ld", dfl, n);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Unexpected end of DVI file.
 */
void
GripeUnexpectedDVIEOF(void)
{

	GripeUnexpectedOp("end of file");
	/* NOTREACHED */
}

/*
 * Unexpected DVI opcode.
 */
void
GripeUnexpectedOp(const char *s)
{

	error(0, 0, "unexpected %s in %s", s, dfl);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Missing DVI opcode.
 */
void
GripeMissingOp(const char *s)
{

	error(0, 0, "missing %s in %s", s, dfl);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Cannot find DVI postamble.
 */
void
GripeCannotFindPostamble(void)
{

	error(0, 0, "cannot find postamble");
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Inconsistent DVI value.
 */
void
GripeMismatchedValue(const char *s)
{

	error(0, 0, "mismatched %s in %s", s, dfl);
	error(1, 0, areyousure, dfn(), dfl);
	/* NOTREACHED */
}

/*
 * Undefined DVI opcode.
 */
void
GripeUndefinedOp(int n)
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
GripeBadGlyph(i32 c, struct font *f)
{

	if (f->f_path == NULL) {
		error(0, 0, "bad %s: char without setfont", dfl);
		error(1, 0, "(try checking %s with dvitype)", dfn());
		/* NOTREACHED */
	}
	error(0, 0, "there is no character %ld in %s!", (long)c, f->f_path);
}
