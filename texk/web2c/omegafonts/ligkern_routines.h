/* ligkern_routines.h: The ligature/kerning table.

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

extern unsigned bchar;
extern unsigned bchar_label;
extern four_entries *lig_kern_table;
extern unsigned char *activity;

extern void set_label_command(unsigned);
extern void set_ligature_command(unsigned, unsigned, unsigned);
extern void set_kerning_command(unsigned, fix);
extern void set_stop_command(void);
extern void set_skip_command(unsigned);
extern void build_kern_table(void);
extern void set_boundary_character(unsigned);

extern void check_ligature_program(unsigned,unsigned);
extern void check_ligature_ends_properly(void);
extern void check_ligature_infinite_loops(void);
extern void doublecheck_ligatures(void);

extern void print_ligkern_table(void);
extern void init_ligkern(void);

#define PRIME           1009

#define KERN_FLAG 128
#define STOP_FLAG 128


extern void set_c_label_command(unsigned);
extern void set_c_kerning_command(unsigned, fix);
extern void set_c_glue_command(unsigned, unsigned);
extern void set_c_penalty_command(unsigned, unsigned);
extern void set_c_penglue_command(unsigned, unsigned, unsigned);

extern unsigned nl;
extern unsigned nk;

extern void output_ofm_ligkern(void);
extern void retrieve_ligkern_table(unsigned char *, unsigned char *);
extern void print_one_lig_kern_entry(four_entries *, boolean);
