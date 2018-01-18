/* out_routines.c: Low-level routines for outputting OPL and OVP files.

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
#include "list_routines.h"
#include "manifests.h"
#include "omfonts.h"
#include "out_routines.h"
#include "error_routines.h"
#include "header_routines.h"
#include "char_routines.h"

/* Character measures */

const_string character_measures[] =
     { "CHARWD",      "CHARHT",         "CHARDP",        "CHARIC",
       "SECWD",       "SECHT",          "SECDP",         "SECIC",
       "PRIMTOPAXIS", "PRIMTOPAXISBIS", "PRIMBOTAXIS",   "PRIMBOTAXISBIS",
       "PRIMMIDHOR",  "PRIMMIDVERT",    "PRIMBASESLANT", NULL,
       "SECTOPAXIS",  "SECTOPAXISBIS",  "SECBOTAXIS",    "SECBOTAXISBIS",
       "SECMIDHOR",   "SECMIDVERT",     "SECBASESLANT",  NULL};

void
out_character_measure(unsigned measure)
{
    if ((measure < C_MIN) || (measure > C_MAX) ||
        (character_measures[measure]==NULL)) {
	internal_error_1("out_character_measure (measure=%d)", measure);
    }
    out(character_measures[measure]);
}

/* Extensible pieces */

const_string extensible_pieces[] =
     { "TOP", "MID", "BOT", "REP", NULL };

void
out_extensible_piece(unsigned piece)
{
    if ((piece < E_MIN) || (piece > E_MAX)) {
	internal_error_1("out_extensible_piece (piece=%d)", piece);
    }
    out(extensible_pieces[piece]);
}

/* Ligature commands */

const_string ligature_commands[] =
     { "LIG", "LIG/", "/LIG", "/LIG/", NULL,
       "LIG/>", "/LIG>", "/LIG/>", NULL, NULL,
       NULL, "/LIG/>>", NULL };

void
out_ligature_command(unsigned command)
{
    if ((command < L_MIN) || (command > L_MAX) ||
        (ligature_commands[command]==NULL)) {
	internal_error_1("out_ligature_command (command=%d)", command);
    }
    out(ligature_commands[command]);
}

/* Xerox faces */

const_string xerox_faces[] =
    { "MRR", "MIR", "BRR", "BIR", "LRR", "LIR",
      "MRC", "MIC", "BRC", "BIC", "LRC", "LIC",
      "MRE", "MIE", "BRE", "BIE", "LRE", "LIE", NULL };

void
print_xerox_face(int face)
{
    if ((face < F_MIN) || (face > F_MAX)) {
	internal_error_1("print_xerox_face (face=%d)", face);
    }
    out("F "); out(xerox_faces[face]);
}

/* Named parameters */

const_string named_parameters[] =
    { NULL,      "SLANT",      "SPACE",  "STRETCH", "SHRINK",    "XHEIGHT",
      "QUAD",    "EXTRASPACE", NULL };

void
out_named_parameter(unsigned parameter)
{
    if ((parameter < P_MIN) || (parameter > P_MAX)) {
	internal_error_1("out_named_parameter (parameter = %d)", parameter);
    }
    out(named_parameters[parameter]);
}

/* Named math symbol parameters */

const_string named_mathsy_parameters[] =
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      "NUM1",   "NUM2",    "NUM3",      "DENOM1",
      "DENOM2",  "SUP1",       "SUP2",   "SUP3",    "SUB1",      "SUB2",
      "SUPDROP", "SUBDROP",    "DELIM1", "DELIM2",  "AXISHEIGHT", NULL };

void
out_named_mathsy_parameter(unsigned parameter)
{
    if ((parameter < P_MATHSY_MIN) || (parameter > P_MATHSY_MAX) ||
        (named_mathsy_parameters[parameter]==NULL)) {
	internal_error_1("out_named_mathsy_parameter (parameter=%d)",parameter);
    }
    out(named_mathsy_parameters[parameter]);
}

/* Named math extension parameters */

const_string named_mathex_parameters[] =
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
      "DEFAULTRULETHICKNESS", "BIGOPSPACING1",
      "BIGOPSPACING2",        "BIGOPSPACING3",
      "BIGOPSPACING4",        "BIGOPSPACING5", NULL };

void
out_named_mathex_parameter(unsigned parameter)
{
    if ((parameter < P_MATHEX_MIN) || (parameter > P_MATHEX_MAX) ||
        (named_mathex_parameters[parameter]==NULL)) {
	internal_error_1("out_named_mathex_parameter (parameter=%d)",parameter);
    }
    out(named_mathex_parameters[parameter]);
}

/* Typesetting directions */

const_string type_directions[] =
     { "FONTDIR", "NATURALFONTDIR", NULL };

const_string actual_directions[] =
     { "TL", "LT", "TR", "RT", "LB", "BL", "BR", "RB", NULL };

void
out_type_direction(unsigned direction)
{
    if ((direction < DIR_MIN) || (direction > DIR_MAX)) {
	internal_error_1("out_type_direction (direction=%d)", direction);
    }
    out(type_directions[direction/8]); out(" ");
    out(actual_directions[direction%8]);
}

/* Kinds of accents */

const_string accent_kinds[] =
     { NULL, "TOPACCENT", "MIDACCENT", "BOTACCENT", NULL };

void
out_accent_kind(unsigned kind)
{
    if ((kind < ACC_TOP) || (kind > ACC_BOT)) {
	internal_error_1("out_accent_kind (kind=%d)", kind);
    }
    out(accent_kinds[kind]);
}

/* Move directions */

const_string move_directions[] =
     { "MOVERIGHT", "MOVELEFT", "MOVEUP", "MOVEDOWN", NULL };

void
out_move_direction(unsigned direction)
{
    if ((direction < M_MIN) || (direction > M_MAX)) {
	internal_error_1("out_move_direction (direction=%d)", direction);
    }
    out(move_directions[direction]);
}

/* Rule measures */

const_string rule_measures[] =
     { "RULEWD", "RULEHT", "RULEDP", NULL };

void
out_rule_measure(unsigned measure)
{
    if ((measure < RULE_MIN) || (measure > RULE_MAX)) {
	internal_error_1("out_rule_measure (measure=%d)", measure);
    }
    out(rule_measures[measure]);
}

/* Glue shrink or stretch */

const_string glue_shrink_stretch[] =
     { "GLUESHRINK", "GLUESTRETCH", NULL };

void
out_shrink_stretch(unsigned shrink_stretch)
{
    if ((shrink_stretch < GLUE_MIN) || (shrink_stretch > GLUE_MAX)) {
	internal_error_1("out_shrink_stretch (shrink_stretch=%d)",
                         shrink_stretch);
    }
    out(glue_shrink_stretch[shrink_stretch]);
}

/* Glue orders */

const_string glue_orders[] =
     { "UNIT", "FI", "FIL", "FILL", "FILLL", NULL };

void
out_glue_order(unsigned order)
{
    if ((order < O_MIN) || (order > O_MAX)) {
	internal_error_1("out_glue_order (order=%d)", order);
    }
    out(glue_orders[order]);
}

/* Glue kinds */

const_string glue_kinds[] =
     { "NORMAL", "ALEADERS", "CLEADERS", "ALEADERS", "XLEADERS", NULL };

void
out_glue_kind(unsigned kind)
{
    if ((kind < K_MIN) || (kind > K_MAX)) {
	internal_error_1("out_glue_kind (kind=%d)", kind);
    }
    out(glue_kinds[kind]);
}

unsigned parenthesis_level=0;
unsigned digits[12];

/* Basic routines used by everyone */

void
out_ln(void)
{
    unsigned i;
    out("\n");
    for (i = 0; i < parenthesis_level; i++) out("   ");
}

void
left(void)
{
    parenthesis_level++; out("(");
}

void
right(void)
{
    if (parenthesis_level==0) internal_error_0("right");
    parenthesis_level--; out(")"); out_ln();
}

void
out_char(unsigned val)
{
        if (char_format==CHAR_CODE_NUM) out_num(val);
        else if (font_type!=FT_VANILLA) out_num(val);
        else if (((val>='0') && (val<='9')) ||
                 ((val>='A') && (val<='Z')) ||
                 ((val>='a') && (val<='z')))
	    fprintf(file_output, "C %c", val);
        else out_num(val);
}

void
out_num(unsigned val)
{
        if (num_format==NUM_CODE_OCTAL) out_int(val, 8);
        else out_int(val, 16);
}

static void out_digits(unsigned);

void
out_int(unsigned val, unsigned base)
{
	register unsigned j=0, acc=val;
	if (val < 0) internal_error_1("out_int (val=%d)", val);
	if (base==16)      out("H ");
	else if (base==10) out("D ");
	else if (base==8)  out("O ");
	else internal_error_1("out_int (base=%d)", base);
	do {
		digits[j] = acc % base;
		j++;
		acc = acc / base;
	} while (acc > 0);
/*
        if ((base==16) && (ec>=0x100)) {
            while (j<4) { digits[j] = 0; j++; }
        }
*/
	out_digits(j);
}

void
out_fix(fix fval)
{
	register int a = (fval & 0xfff00000) >> 20, f = fval & 0xfffff, j=0;
	register int delta;

	out("R ");
	if (a>0x7ff) {
		out("-"); a = 0x1000 - a;
		if (f>0) {
			f = 0x100000 - f; a--;
		}
	}
	do {
		digits[j] = a % 10;
		j++;
		a = a / 10;
	} while (a > 0);
	out_digits(j);
	out(".");
	f = 10*f + 5; delta = 10;
	do {
		if (delta>0x100000) f = f + 0x80000 - (delta / 2);
		fprintf(file_output, "%c", (f / 0x100000) + '0');
		f = 10 * (f % 0x100000); delta = delta*10;
	} while (f>delta);
}

void
out_hex(unsigned char c)
{
	fprintf(file_output, "%02X", c);
}

static void
out_digits(unsigned counter)
{
	register unsigned j=counter, c;
	while (j>0) {
		c = digits[--j];
		if (c<10) fprintf(file_output, "%c", c+'0');
		else fprintf(file_output, "%c", c+'A'-10);
	}
}

void
out(const_string sval)
{
	fprintf(file_output, "%s", sval);
}
