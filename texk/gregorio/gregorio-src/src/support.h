/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This header prototypes the miscellaneous support functions.
 *
 * Copyright (C) 2015 The Gregorio Project (see CONTRIBUTORS.md)
 * 
 * This file is part of Gregorio.
 *
 * Gregorio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Gregorio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdlib.h>

void gregorio_snprintf(char *s, size_t size, const char *format, ...)
        __attribute__ ((__format__ (__printf__, 3, 4)));
void *gregorio_malloc(size_t size);
void *gregorio_calloc(size_t nmemb, size_t size);
void *gregorio_realloc(void *ptr, size_t size);
char *gregorio_strdup(const char *s);

#endif
