/* error_routines.c: General error routines.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright 1994--2018 John Plaice and Yannis Haralambous

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

#include "cpascal.h"
#include "parser.h"
#include "error_routines.h"

int num_errors = 0;

/* Error routines:

All of the routines in this file have the same general format.
They 

*/

void
lex_error_1(const_string fmt, int item)
{
    fprintf(stderr, "line %d (lexing): ", line_number);
    fprintf(stderr, fmt, item);
    fprintf(stderr, "\n");
}

void
lex_error_s(const_string fmt, const_string item)
{
    fprintf(stderr, "line %d (lexing): ", line_number);
    fprintf(stderr, fmt, item);
    fprintf(stderr, "\n");
}

void
lex_error_s_1(const_string fmt, const_string item, int item1)
{
    fprintf(stderr, "line %d (lexing): ", line_number);
    fprintf(stderr, fmt, item, item1);
    fprintf(stderr, "\n");
}

void
yyerror(const_string fmt)
{
    fprintf(stderr, "line %d (parsing): ", line_number);
    fprintf(stderr, "%s", fmt);
    fprintf(stderr, "\n");
    num_errors++;
}


void
warning_0(const_string fmt)
{
    fprintf(stderr, "line %d (warning): ", line_number);
    fprintf(stderr, "%s", fmt);
    fprintf(stderr, "\n");
}

void
warning_1(const_string fmt, int item)
{
    fprintf(stderr, "line %d (warning): ", line_number);
    fprintf(stderr, fmt, item);
    fprintf(stderr, "\n");
}

void
warning_2(const_string fmt, int item, int item2)
{
    fprintf(stderr, "line %d (warning): ", line_number);
    fprintf(stderr, fmt, item, item2);
    fprintf(stderr, "\n");
}

void
warning_s(const_string fmt, const_string item)
{
    fprintf(stderr, "line %d (warning): ", line_number);
    fprintf(stderr, fmt, item);
    fprintf(stderr, "\n");
}

void
warning_s_1(const_string fmt, const_string item, int item1)
{
    fprintf(stderr, "line %d (warning): ", line_number);
    fprintf(stderr, fmt, item, item1);
    fprintf(stderr, "\n");
}

void
warning_s_2(const_string fmt, const_string item, int item1, int item2)
{
    fprintf(stderr, "line %d (warning): ", line_number);
    fprintf(stderr, fmt, item, item1, item2);
    fprintf(stderr, "\n");
}

void
fatal_error_0(const_string fmt)
{
    fprintf(stderr, "line %d (fatal): ", line_number);
    fprintf(stderr, "%s", fmt);
    fprintf(stderr, "\n");
    exit(1);
}

void
fatal_error_1(const_string fmt, int item)
{
    fprintf(stderr, "line %d (fatal): ", line_number);
    fprintf(stderr, fmt, item);
    fprintf(stderr, "\n");
    exit(1);
}

void
fatal_error_2(const_string fmt, int item, int item2)
{
    fprintf(stderr, "line %d (fatal): ", line_number);
    fprintf(stderr, fmt, item, item2);
    fprintf(stderr, "\n");
    exit(1);
}

void
fatal_error_s(const_string fmt, const_string  item)
{
    fprintf(stderr, "line %d (fatal): ", line_number);
    fprintf(stderr, fmt, item);
    fprintf(stderr, "\n");
    exit(1);
}

void
internal_error_0(const_string fmt)
{
    fprintf(stderr, "line %d (internal): ", line_number);
    fprintf(stderr, "%s", fmt);
    fprintf(stderr, "\n");
    exit(2);
}

void
internal_error_1(const_string fmt, int item)
{
    fprintf(stderr, "line %d (internal): ", line_number);
    fprintf(stderr, fmt, item);
    fprintf(stderr, "\n");
    exit(2);
}
