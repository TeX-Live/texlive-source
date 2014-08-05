/* dimen_routines.h: Data structures for holding widths, heights, etc.

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

extern int *dimen_tables[];

extern void set_character_measure(int, int);
extern void init_measures(void);
extern void build_dimen_tables(void);
extern void retrieve_dimen_tables(void);
extern void print_dimen_tables(void);
extern void set_accent(unsigned);
extern void output_ofm_dimension(void);

extern unsigned nw;
extern unsigned nh;
extern unsigned nd;
extern unsigned ni;
