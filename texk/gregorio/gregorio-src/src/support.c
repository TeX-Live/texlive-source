/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This file contains miscellaneous support functions.
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

#include "config.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include "messages.h"

/* Our version of snprintf; this is NOT semantically the same as C99's
 * snprintf; rather, it's a "lowest common denominator" implementation
 * blending C99 and MS-C */
void gregorio_snprintf(char *s, size_t size, const char *format, ...)
{
    va_list args;

#ifdef _MSC_VER
    memset(s, 0, size);
#endif

    va_start(args, format);
#ifdef _MSC_VER
    _vsnprintf_s(s, size, _TRUNCATE, format, args);
#else
    vsnprintf(s, size, format, args);
#endif
    va_end(args);
}

void *gregorio_malloc(size_t size)
{
    void *result = malloc(size);
    if (!result) {
        gregorio_message(_("error in memory allocation"),
                "gregorio_malloc", VERBOSITY_FATAL, 0);
        exit(1);
    }
    return result;
}

void *gregorio_calloc(size_t nmemb, size_t size)
{
    void *result = calloc(nmemb, size);
    if (!result) {
        gregorio_message(_("error in memory allocation"),
                "gregorio_calloc", VERBOSITY_FATAL, 0);
        exit(1);
    }
    return result;
}

void *gregorio_realloc(void *ptr, size_t size)
{
    void *result = realloc(ptr, size);
    if (!result) {
        gregorio_message(_("error in memory allocation"),
                "gregorio_realloc", VERBOSITY_FATAL, 0);
        exit(1);
    }
    return result;
}

char *gregorio_strdup(const char *s)
{
    char *result = strdup(s);
    if (!result) {
        gregorio_message(_("error in memory allocation"),
                "gregorio_strdup", VERBOSITY_FATAL, 0);
        exit(1);
    }
    return result;
}

bool gregorio_readline(char **buf, size_t *bufsize, FILE *file)
{
    size_t oldsize;
    if (*buf == NULL) {
        *bufsize = 128;
        *buf = (char *)gregorio_malloc(*bufsize);
    } else {
        if (*bufsize < 128) {
            gregorio_message(_("invalid buffer size"), "gregorio_getline",
                    VERBOSITY_FATAL, 0);
            exit(1);
        }
    }
    (*buf)[0] = '\0';
    oldsize = 1;
    for (;;) {
        (*buf)[*bufsize - 2] = '\0';

        if (feof(file) || ferror(file)
                || !fgets((*buf) + oldsize - 1, (int)(*bufsize - oldsize + 1),
                    file)
                || (*buf)[*bufsize - 2] == '\0') {
            if (ferror(file)) {
                gregorio_message(_("Error reading from the file"),
                        "gregorio_getline", VERBOSITY_FATAL, 0);
                exit(1);
            }
            return (*buf)[0] != '\0';
        }

        if (*bufsize >= MAX_BUF_GROWTH) {
            gregorio_message(_("Line too long"), "gregorio_getline",
                    VERBOSITY_FATAL, 0);
            exit(1);
        }

        oldsize = *bufsize;
        *bufsize <<= 1;
        *buf = gregorio_realloc(*buf, *bufsize);
    }
}
