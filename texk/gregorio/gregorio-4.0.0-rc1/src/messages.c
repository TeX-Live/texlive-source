/*
 * Copyright (C) 2009-2015 The Gregorio Project (see CONTRIBUTORS.md)
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
#include <stdlib.h>             /* for exit() */
#include <stdarg.h>             /* for exit() */
#include "bool.h"
#include "messages.h"

static FILE *error_out;
static const char *file_name = NULL;
static gregorio_verbosity verbosity_mode = 0;
static bool debug_messages = false;
static int return_value = 0;

int gregorio_get_return_value(void)
{
    return return_value;
}

void gregorio_set_error_out(FILE *const f)
{
    error_out = f;
}

void gregorio_set_file_name(const char *const new_name)
{
    file_name = new_name;
}

void gregorio_set_verbosity_mode(const gregorio_verbosity verbosity)
{
    verbosity_mode = verbosity;
}

#if 0 /* unused */
static void gregorio_set_debug_messages(bool debug)
{
    debug_messages = debug;
}
#endif

static const char *verbosity_to_str(const gregorio_verbosity verbosity)
{
    const char *str;
    switch (verbosity) {
    case VERBOSITY_WARNING:
        str = _("warning:");
        break;
    case VERBOSITY_DEPRECATION:
        str = _("deprecation:");
        break;
    case VERBOSITY_ERROR:
        str = _("error:");
        break;
    case VERBOSITY_FATAL:
        str = _("fatal error:");
        break;
    default:
        /* INFO, for example */
        str = " ";
        break;
    }
    return str;
}

void gregorio_messagef(const char *function_name,
        gregorio_verbosity verbosity, int line_number,
        const char *format, ...)
{
    va_list args;
    const char *verbosity_str;

    if (!debug_messages) {
        line_number = 0;
        function_name = NULL;
    }

    if (!error_out) {
        fprintf(stderr, _("warning: error_out not set in gregorio_messages, "
                    "assumed stderr\n"));
        error_out = stderr;
    }
    if (!verbosity_mode) {
        fprintf(stderr, _("warning: verbosity mode not set in "
                    "gregorio_messages, assumed warnings\n"));
        verbosity_mode = VERBOSITY_WARNING;
    }
    if (verbosity < verbosity_mode) {
        return;
    }
    verbosity_str = verbosity_to_str(verbosity);
    if (line_number) {
        if (function_name) {
            if (!file_name) {
                fprintf(error_out, "line %d: in function `%s': %s",
                        line_number, function_name, verbosity_str);
            } else {
                fprintf(error_out, "%d: in function `%s': %s", line_number,
                        function_name, verbosity_str);
            }
        } else {
            /* no function_name specified */
            if (!file_name) {
                fprintf(error_out, "line %d: %s", line_number, verbosity_str);
            } else {
                fprintf(error_out, "%d: %s", line_number, verbosity_str);
            }
        }
    } else {
        if (function_name) {
            /*
             * if (!file_name) {
             *     fprintf (error_out, "in function `%s': %s",
             *     function_name, verbosity_str);
             *     return;
             * } else {
             */
            fprintf(error_out, "in function `%s': %s", function_name,
                    verbosity_str);
            /* } */
        } else {
            /* no function_name specified */
            /*
             * if (!file_name) {
             *     fprintf (error_out, "%s", verbosity_str);
             *     return;
             * } else {
             */
            fprintf(error_out, "%s", verbosity_str);
            /* } */
        }
    }
    va_start(args, format);
    vfprintf(error_out, format, args);
    va_end(args);
    fprintf(error_out, "\n");

    switch (verbosity) {
    case VERBOSITY_ERROR:
        return_value = 1;
        break;
    case VERBOSITY_FATAL:
        exit(1);
        break;
    default:
        break;
    }
}

void gregorio_message(const char *string, const char *function_name,
        gregorio_verbosity verbosity, int line_number)
{
    gregorio_messagef(function_name, verbosity, line_number, "%s", string);
}

