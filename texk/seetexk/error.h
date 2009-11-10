/*
 * Copyright (c) 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Errors.
 */

#include <stdarg.h>

void	error(int quit, int e, const char *fmt, ...);
void	panic(const char *fmt, ...);

void	SetErrorTrap(void (*fn)(int quit, const char *text));
