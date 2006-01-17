/*
 * Copyright (c) 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Errors.
 */

#include <kpathsea/config.h>
#include <kpathsea/c-vararg.h>

void	error PVAR3H(int quit, int e, const char *fmt);
void	panic PVAR1H(const char *fmt);

void	SetErrorTrap(void (*fn)(int quit, char *text));
