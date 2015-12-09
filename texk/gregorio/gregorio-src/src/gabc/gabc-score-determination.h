/*
 * Gregorio is a program that translates gabc files to GregorioTeX
 * This header shares definitions between the score parser and lexer.
 *
 * Gregorio score determination in gabc input.
 * Copyright (C) 2006-2015 The Gregorio Project (see CONTRIBUTORS.md)
 *
 * This file is part of Gregorio.
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#include "gabc.h"

typedef union gabc_score_determination_lval_t {
    char *text;
    char character;
} gabc_score_determination_lval_t;

#define YYSTYPE gabc_score_determination_lval_t
#define YYSTYPE_IS_DECLARED 1

int gabc_score_determination_lex(void);
#define YY_DECL int gabc_score_determination_lex(void)

#define YYLTYPE gregorio_scanner_location
