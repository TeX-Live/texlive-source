/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Declarations for seek.c: SeekFile(FILE *f) returns a
 * version of `f' on which `fseek' functions properly;
 * CopyFile(FILE *f) returns a temporary copy of f.
 */
#ifdef WIN32
#undef CopyFile
#endif
FILE *SeekFile(FILE *f), *CopyFile(FILE *f);
