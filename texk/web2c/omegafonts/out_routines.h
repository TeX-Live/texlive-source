/* out_routines.h: Low-level routines for outputting OPL and OVP files.

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

extern const_string extensible_pieces[];
extern const_string ligature_commands[];
extern unsigned parenthesis_level;

extern void out_character_measure(unsigned);
extern void out_extensible_piece(unsigned);
extern void out_ligature_command(unsigned);
extern void out_named_parameter(unsigned);
extern void out_named_mathsy_parameter(unsigned);
extern void out_named_mathex_parameter(unsigned);
extern void out_move_direction(unsigned);
extern void out_glue_order(unsigned);
extern void out_glue_kind(unsigned);
extern void out_ln(void);
extern void out_char(unsigned);
extern void out_num(unsigned);
extern void out_int(unsigned,unsigned);
extern void out_fix(fix);
extern void out_hex(unsigned char);
extern void out(const_string);
extern void right(void);
extern void left(void);
extern void print_xerox_face(int);
extern void out_rule_measure(unsigned);
extern void out_shrink_stretch(unsigned);
extern void out_type_direction(unsigned);
extern void out_accent_kind(unsigned);
