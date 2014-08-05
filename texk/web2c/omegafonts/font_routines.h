/* font_routines.h: Data structures for virtual font support.

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

typedef struct font_struct {
    int font_number;
    unsigned font_checksum;
    double font_dsize;
    string font_name;
    string font_area;
    fix font_at;
    unsigned char *ovf_packet;
    unsigned ovf_packet_length;
    boolean font_at_defined;
    boolean font_dsize_defined;
    boolean font_checksum_defined;
} font;

typedef struct move_struct {
    fix wfix;
    fix xfix;
    fix yfix;
    fix zfix;
    unsigned h;
    unsigned v;
} move;

extern font *font_table;
extern unsigned no_fonts;
extern unsigned cur_font_index;
extern font *cur_font;

extern string vtitle;

extern unsigned packet_table_size;
extern unsigned char *packet_table;
extern unsigned char *cur_packet;
extern unsigned packet_ptr;

extern void init_map_font(int);
extern void set_font_name(string);
extern void set_font_area(string);
extern void set_font_check_sum(unsigned);
extern void set_font_at(fix);
extern void set_font_design_size(fix);

extern void set_vtitle(string );
extern void init_map(void);
extern void end_map(void);
extern void set_select_font(unsigned);
extern void set_set_char(unsigned);
extern void set_set_rule(fix, fix);
extern void set_move(unsigned, fix);
extern void set_push(void);
extern void set_pop(void);
extern void set_special(string );
extern void set_special_hex(string );

extern void font_table_init(void);

extern void out_ovf(unsigned);
extern void out_ovf_4(unsigned);

extern int ovf_get_arg(unsigned char **, unsigned,boolean);
extern void input_ovf_fonts(void);
extern void input_ovf_chars(void);
extern void input_ovf_file(void);
