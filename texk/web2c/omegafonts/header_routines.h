/* header_routines.h: Headers of font metric files.

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

extern string header;

extern unsigned check_sum;
extern boolean check_sum_specified;

extern fix design_size;
extern boolean design_size_specified;

extern fix design_units;
extern boolean design_units_specified;

extern string coding_scheme;
extern boolean coding_scheme_specified;

extern string family;
extern boolean family_specified;

extern unsigned face;
extern boolean face_specified;

extern unsigned ofm_level;
extern boolean ofm_level_specified;

extern unsigned font_dir;
extern boolean font_dir_specified;

extern boolean seven_bit;
extern boolean seven_bit_specified;
extern boolean seven_bit_calculated;

extern unsigned font_type;

extern unsigned header_max;
extern unsigned lh;

extern void set_header_word(unsigned, unsigned);

extern void set_check_sum(unsigned);

extern void set_design_size(fix);

extern void set_design_units(fix);

extern void set_coding_scheme(string );

extern void set_family(string );

extern void set_face(unsigned);

extern void set_seven_bit_safe_flag(unsigned);
extern void calculate_seven_bit_safe_flag(void);

extern void init_header(void);
extern void retrieve_header(void);

extern void output_ofm_header(void);

extern void set_ofm_level(unsigned);
extern void set_font_dir(unsigned);
