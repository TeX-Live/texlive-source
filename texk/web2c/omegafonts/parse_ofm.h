/* parse_ofm.h: Checking an existent OFM file

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

extern unsigned char_base,width_base,height_base,depth_base,italic_base;
extern unsigned npc,nco,ncw,lf;

extern void parse_ofm(boolean);
extern void ofm_organize(void);
extern void ofm_read_simple(void);
extern void ofm_read_rest(void);

extern void eval_two_bytes(unsigned *);
extern void eval_four_bytes(unsigned *);

extern unsigned width_index(unsigned);
extern unsigned height_index(unsigned);
extern unsigned depth_index(unsigned);
extern unsigned italic_index(unsigned);
extern unsigned tag(unsigned);
extern void set_no_tag(unsigned);
extern boolean ctag(unsigned);
extern void set_no_ctag(unsigned);
extern unsigned no_repeats(unsigned);
extern unsigned char_param(unsigned, unsigned);
extern unsigned rremainder(unsigned);
extern unsigned lig_step(unsigned);
extern unsigned exten(unsigned);
extern unsigned l_skip_byte(unsigned);
extern void set_l_skip_byte(unsigned, unsigned);
extern unsigned l_next_char(unsigned);
extern void set_l_next_char(unsigned, unsigned);
extern unsigned l_op_byte(unsigned);
extern void set_l_op_byte(unsigned, unsigned);
extern unsigned l_remainder(unsigned);
extern void set_l_remainder(unsigned, unsigned);

extern unsigned ofm_get_4(unsigned);
extern unsigned ofm_get_1(unsigned);
