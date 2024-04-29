/* omfonts.h: Main routine for ofm2opl, opl2ofm, ovf2ovp, ovp2ovf.

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

#define CHAR_CODE_NUM   0
#define CHAR_CODE_ASCII 1
#define NUM_CODE_HEX    0
#define NUM_CODE_OCTAL  1
#define TEXT_CODE_UPPER 0
#define TEXT_CODE_MIXED 1

extern FILE *file_ofm;
extern FILE *file_ovf;
extern FILE *file_output;
extern int verbose_option;
extern int char_format;
extern int num_format;
extern int text_format;
extern int omit_ofm;

extern unsigned length_ofm;
extern unsigned length_ovf;

extern unsigned char *ofm;
extern unsigned char *ovf;

extern void output_ovf_file(void);

