/* extra_routines.h: Tables for glues, rules, penalties, etc.

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

extern void init_font_ivalue(unsigned);
extern void init_font_mvalue(unsigned);
extern void init_font_fvalue(unsigned);
extern void init_font_penalty(unsigned);
extern void init_font_rule(unsigned);
extern void init_font_glue(unsigned);


void init_font_ivalue_entry(unsigned);
void set_font_ivalue_definition(unsigned);
void init_font_mvalue_entry(unsigned);
void set_font_mvalue_definition(fix);
void init_font_fvalue_entry(unsigned);
void set_font_fvalue_definition(fix);
void init_font_penalty_entry(unsigned);
void set_font_penalty_definition(unsigned);
void init_font_rule_entry(unsigned);
void set_font_rule_measure(unsigned, fix);
void init_font_glue_entry(unsigned);
void set_font_glue_type(unsigned);
void set_font_glue_width(fix);
void set_font_glue_character(unsigned);
void set_font_glue_rule(unsigned, unsigned);
void set_font_glue_shrink_stretch(unsigned, fix, unsigned);

typedef struct rule_struct {
    fix rule_wd;
    fix rule_ht;
    fix rule_dp;
} rule;

typedef struct glue_struct {
    fix glue_width;
    fix glue_stretch;
    fix glue_shrink;
    unsigned glue_stretch_order;
    unsigned glue_shrink_order;
    unsigned glue_type;
    unsigned glue_arg_type;
    unsigned glue_arg1;
    unsigned glue_arg2;
} glue;

extern void set_character_ivalue(unsigned, unsigned);
extern void set_character_penalty(unsigned, unsigned);
extern void set_character_mvalue(unsigned, unsigned);
extern void set_character_fvalue(unsigned, unsigned);
extern void set_character_rule(unsigned, unsigned);
extern void set_character_glue(unsigned, unsigned);
extern void compute_ofm_extra_stuff(void);
extern void output_ofm_extra_stuff(void);

extern unsigned nki;
extern unsigned nwi;
extern unsigned nkp;
extern unsigned nwp;
extern unsigned nkm;
extern unsigned nwm;
extern unsigned nkf;
extern unsigned nwf;
extern unsigned nkr;
extern unsigned nwr;
extern unsigned nkg;
extern unsigned nwg;

