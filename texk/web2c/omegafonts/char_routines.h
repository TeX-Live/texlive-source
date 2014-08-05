/* char_routines.h: Data structures for character information

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

typedef struct char_entry_struct {
    in_list indices[C_MAX+1];
    unsigned index_indices[C_MAX+1];
    unsigned c;
    unsigned copies;
    unsigned tag;
    unsigned remainder;
    unsigned defined;
    unsigned accent;
    unsigned *extens;
    unsigned ovf_packet_length;
    unsigned char *ovf_packet;
} char_entry;

extern char_entry *current_character;
extern char_entry *current_secondary_character;

extern unsigned num_char_info, words_per_entry;

extern void print_characters(boolean);
extern void init_character(unsigned, char_entry *);
extern void copy_characters(unsigned, unsigned);
extern void init_planes(void);

extern void ensure_existence(unsigned);
extern void check_char_tag(unsigned);
extern void set_char_tag(unsigned,unsigned);
extern void set_char_remainder(unsigned,unsigned);
extern void set_next_larger(unsigned);
extern void init_var_character(void);

typedef struct label_entry_struct {
    int rr;
    int cc;
} label_entry;

extern unsigned no_labels;
extern label_entry *label_table;
extern int label_ptr, sort_ptr;
extern int lk_offset;
extern boolean extra_loc_needed;


extern void check_and_correct(void);

extern void adjust_labels(boolean);
extern void print_labels(void);
extern void set_extensible_piece(unsigned,unsigned);
extern void check_existence_and_safety(unsigned,unsigned,const_string,const_string);
extern void clear_ligature_entries(void);
extern void print_extens(void);
extern void retrieve_exten_table(unsigned char *);
extern void doublecheck_existence(unsigned, const_string,const_string);
extern void output_ovf_chars(void);

extern unsigned bc;
extern unsigned ec;
extern unsigned ne;

extern void compute_ligkern_offset(void);
extern void output_ofm_extensible(void);
extern void compute_ofm_character_info(void);
extern void output_ofm_character_info(void);
