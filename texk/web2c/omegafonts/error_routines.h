/* error_routines.h: General error routines.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous

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

extern int num_errors;

extern void lex_error_1(const_string, int);
extern void lex_error_s(const_string, const_string);
extern void lex_error_s_1(const_string, const_string, int);

extern void fatal_error_0(const_string);
extern void fatal_error_1(const_string, int);
extern void fatal_error_2(const_string, int, int);
extern void fatal_error_s(const_string, const_string);

extern void yyerror(const_string);

extern void warning_0(const_string);
extern void warning_1(const_string, int);
extern void warning_2(const_string, int, int);
extern void warning_s(const_string, const_string);
extern void warning_s_1(const_string, const_string, int);
extern void warning_s_2(const_string, const_string, int, int);

extern void internal_error_0(const_string);
extern void internal_error_1(const_string, int);
