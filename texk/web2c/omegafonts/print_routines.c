/* print_routines.c: higher-level routines for printing OPL and OVP files.

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

#include "cpascal.h"
#include "list_routines.h"
#include "manifests.h"
#include "print_routines.h"
#include "header_routines.h"
#include "out_routines.h"
#include "error_routines.h"
#include "omfonts.h"

void
print_check_sum(void)
{
    left();
    out("CHECKSUM");
    out(" "); out_num(check_sum);
    right();
}

void
print_design_size(void)
{
    left();
    out("DESIGNSIZE");
    out(" "); out_fix(design_size);
    right();
}

void
print_coding_scheme(void)
{
    left();
    out("CODINGSCHEME");
    out(" "); out(coding_scheme);
    right();
}

void
print_family(void)
{
    left();
    out("FAMILY");
    out(" "); out(family);
    right();
}

void
print_face(void)
{
    left();
    out("FACE");
    out(" ");
    if ((face >= F_MIN) && (face <= F_MAX)) print_xerox_face(face);
    else out_num(face);
    right();
}

void
print_seven_bit_safe_flag(void)
{
    left();
    out("SEVENBITSAFEFLAG"); out(" ");
    if (seven_bit == 1) out("TRUE");
    else if (seven_bit == 0) out("FALSE");
    else internal_error_1("print_seven_bit_safe_flag (seven_bit=%d)", seven_bit);
    right();
    
}

void
print_font_dimension(void)
{
    left();
    out("FONTDIMEN"); out_ln();
}

void
print_boundary_char(unsigned val)
{
    left();
    out("BOUNDARYCHAR");
    out(" "); out_char(val); right();
}

void
print_ligature_table(void)
{
    left();
    out("LIGTABLE"); out_ln();
}

void
print_character(unsigned val)
{
    left();
    out("CHARACTER");
    out(" "); out_char(val); out_ln();
}

void
print_parameter(unsigned parameter, fix fval)
{
    left();
    if ((font_type==FT_MATHSY) &&
        (parameter>=P_MATHSY_MIN) &&
        (parameter<=P_MATHSY_MAX)) {
         out_named_mathsy_parameter(parameter);
    } else if ((font_type==FT_MATHEX) &&
        (parameter>=P_MATHEX_MIN) &&
        (parameter<=P_MATHEX_MAX)) {
         out_named_mathex_parameter(parameter);
    } else if (parameter<=P_MAX) {
         out_named_parameter(parameter);
    } else {
         out("PARAMETER"); out(" "); out_int(parameter,10);
    }
    out(" "); out_fix(fval);
    right();
}

void
print_label_command(unsigned val)
{
    left();
    out("LABEL"); out(" ");
    if (val == -1) out("BOUNDARYCHAR");
    else out_char(val);
    right();
}

void
print_ligature_command(unsigned command, unsigned val1, unsigned val2)
{
    left();
    out_ligature_command(command);
    out(" "); out_char(val1);
    out(" "); out_char(val2);
    right();
}

void
print_kerning_command(unsigned val, fix fval)
{
    left();
    out("KRN");
    out(" "); out_char(val);
    out(" "); out_fix(fval);
    right();
}

void
print_stop_command(void)
{
    left();
    out("STOP");
    right();
}

void
print_skip_command(unsigned val)
{
    left();
    out("SKIP");
    out(" "); out_int(val,10);
    right();
}

void
print_character_measure(unsigned measure, fix fval)
{
    left();
    out_character_measure(measure);
    out(" "); out_fix(fval);
    right();
}

void
print_next_larger(unsigned val)
{
    left();
    out("NEXTLARGER");
    out(" "); out_char(val);
    right();
}

void
print_var_character(void)
{
    left(); out("VARCHAR"); out_ln();
}

void
print_extensible_piece(unsigned piece, unsigned val)
{
    left();
    out_extensible_piece(piece);
    out(" "); out_char(val);
    right();
}

/* Primitives used in VF and OVF files */

void
print_vtitle(string sval)
{
    left();
    out("VTITLE");
    out(" "); out(sval);
    right();
}

void
print_map_font(unsigned val)
{
    left();
    out("MAPFONT");
    out(" "); out_int(val,10); out_ln();
}

void
print_font_name(string sval)
{
    left();
    out("FONTNAME");
    out(" "); out(sval);
    right();
}

void
print_font_area(string sval)
{
    left();
    out("FONTAREA");
    out(" "); out(sval);
    right();
}

void
print_font_check_sum(unsigned val)
{
    left();
    out("FONTCHECKSUM");
    out(" "); out_num(val);
    right();
}

void
print_font_at(fix fval)
{
    left();
    out("FONTAT");
    out(" "); out_fix(fval);
    right();
}

void
print_font_design_size(fix fval)
{
    left();
    out("FONTDSIZE");
    out(" "); out_fix(fval);
    right();
}

void
print_map(void)
{
    left(); out("MAP"); out_ln();
}

void
print_select_font(unsigned val)
{
    left();
    out("SELECTFONT");
    out(" "); out_int(val,10);
    right();
}

void
print_set_char(unsigned val)
{
    left();
    out("SETCHAR");
    out(" "); out_char(val);
    right();
}

void
print_put_char(unsigned val)
{
    left();
    out("PUTCHAR");
    out(" "); out_char(val);
    right();
}

void
print_set_rule(fix fval1, fix fval2)
{
    left();
    out("SETRULE");
    out(" "); out_fix(fval1);
    out(" "); out_fix(fval2);
    right();
}

void
print_put_rule(fix fval1, fix fval2)
{
    left();
    out("PUTRULE");
    out(" "); out_fix(fval1);
    out(" "); out_fix(fval2);
    right();
}

void
print_push(void)
{
    left(); out("PUSH"); right();
}

void
print_pop(void)
{
    left(); out("POP"); right();
}

void
print_move(unsigned direction, fix fval)
{
    left();
    out_move_direction(direction);
    out(" "); out_fix(fval);
    right();
}

void
print_special(string sval)
{
    left();
    out("SPECIAL");
    out(" "); out(sval);
    right();
}

void
print_special_hex(unsigned char *special, int k)
{
    left();
    out("SPECIALHEX ");
    for (; k > 0; k--, special++) {
        if ((k & 0x1f) == 0)
            out_ln();
        else if ((k & 0x03) == 0)
            out(" ");
        out_hex(*special);
    }
    right();
}

/* Primitives used in OPL and OVF files */

void
print_character_repeat(unsigned val1, unsigned val2)
{
    left();
    out("CHARREPEAT");
    out(" "); out_char(val1);
    out(" "); out_char(val2); out_ln();
}

void
print_ofm_level(unsigned val)
{
    left();
    out("OFMLEVEL");
    out(" "); out_int(val,16);
    right();
}

void
print_font_dir(unsigned val)
{
    left();
    out_type_direction(val);
    right();
}
