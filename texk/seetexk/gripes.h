/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

#include "font.h"	/* for struct font */

/*
 * Declarations for the various gripes.
 */

void GripeOutOfMemory(int n, char *why);
void GripeCannotGetFont(char *name, i32 mag, i32 dsz, char *dev, char *fullname);
void GripeDifferentChecksums(char *font, i32 tfmsum, i32 fontsum);
void GripeMissingFontsPreventOutput(int n);
void GripeNoSuchFont(i32 n);
void GripeFontAlreadyDefined(i32 n);
void GripeUnexpectedDVIEOF(void);
void GripeUnexpectedOp(char *s);
void GripeMissingOp(char *s);
void GripeCannotFindPostamble(void);
void GripeMismatchedValue(char *s);
void GripeUndefinedOp(int n);
void GripeBadGlyph(i32 c, struct font *f);
