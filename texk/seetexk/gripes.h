/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#include "font.h"	/* for struct font */

extern const char *DVIFileName;

/*
 * Declarations for the various gripes.
 */

void GripeOutOfMemory(int n, const char *why);
void GripeCannotGetFont(const char *name, i32 mag, i32 dsz, const char *dev, const char *fullname);
void GripeDifferentChecksums(const char *font, i32 tfmsum, i32 fontsum);
void GripeMissingFontsPreventOutput(int n);
void GripeNoSuchFont(i32 n);
void GripeFontAlreadyDefined(i32 n);
void GripeUnexpectedDVIEOF(void);
void GripeUnexpectedOp(const char *s);
void GripeMissingOp(const char *s);
void GripeCannotFindPostamble(void);
void GripeMismatchedValue(const char *s);
void GripeUndefinedOp(int n);
void GripeBadGlyph(i32 c, struct font *f);
