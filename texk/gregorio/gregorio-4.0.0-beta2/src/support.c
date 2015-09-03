/*
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

#include "config.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "support.h"

/* Our version of snprintf; this is NOT semantically the same as C99's
 * snprintf; rather, it's a "lowest common denominator" implementation
 * blending C99 and MS-C */
void gregorio_snprintf(char *s, size_t size, const char *format, ...)
{
    va_list args;

#ifdef _MSC_VER
    memset(s, 0, size * sizeof(char));
#endif

    va_start(args, format);
#ifdef _MSC_VER
    _vsnprintf_s(s, size, _TRUNCATE, format, args);
#else
    vsnprintf(s, size, format, args);
#endif
    va_end(args);
}
