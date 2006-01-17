/* dvi.h: All of the DVI commands

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

/* VF commands */

#define VF_ID		202 /* the second byte of a VF file */
#define VF_LONG_CHAR	242 /* long format for character packets */

/* DVI commands */

#define DVI_SET_CHAR_0    0 /* typeset character 0 and move right */
#define DVI_SET_CHAR_127 127 /* ... to 127 */
#define DVI_SET_1        128 /* typeset a char (1 byte)  and move right */
#define DVI_SET_2        129 /* typeset a char (2 bytes) and move right */
#define DVI_SET_3        130 /* typeset a char (3 bytes) and move right */
#define DVI_SET_4        131 /* typeset a char (4 bytes) and move right */
#define DVI_SET_RULE     132 /* typeset a rule and move right */
#define DVI_PUT_1        133 /* typeset a char (1 byte)  and stay put */
#define DVI_PUT_2        134 /* typeset a char (2 bytes) and stay put */
#define DVI_PUT_3        135 /* typeset a char (3 bytes) and stay put */
#define DVI_PUT_4        136 /* typeset a char (4 bytes) and stay put */
#define DVI_PUT_RULE     137 /* typeset a rule and stay put */
#define DVI_NOP          138 /* no-op */
#define DVI_BOP          139 /* beginning of page */
#define DVI_EOP          140 /* end of page */
#define DVI_PUSH         141 /* save the current positions */
#define DVI_POP          142 /* restore previous positions */
#define DVI_RIGHT_1      143 /* move right (1 byte)  */
#define DVI_RIGHT_2      144 /* move right (2 bytes) */
#define DVI_RIGHT_3      145 /* move right (3 bytes) */
#define DVI_RIGHT_4      146 /* move right (4 bytes) */
#define DVI_W_0          147 /* move right by |w| */
#define DVI_W_1          148 /* move right and set |w| (1 byte)  */
#define DVI_W_2          149 /* move right and set |w| (2 bytes) */
#define DVI_W_3          150 /* move right and set |w| (3 bytes) */
#define DVI_W_4          151 /* move right and set |w| (4 bytes) */
#define DVI_X_0          152 /* move right by |x| */
#define DVI_X_1          153 /* move right and set |x| (1 byte)  */
#define DVI_X_2          154 /* move right and set |x| (2 bytes) */
#define DVI_X_3          155 /* move right and set |x| (3 bytes) */
#define DVI_X_4          156 /* move right and set |x| (4 bytes) */
#define DVI_DOWN_1       157 /* move down (1 byte) */
#define DVI_DOWN_2       158 /* move down (2 bytes) */
#define DVI_DOWN_3       159 /* move down (3 bytes) */
#define DVI_DOWN_4       160 /* move down (4 bytes) */
#define DVI_Y_0          161 /* move down by |y| */
#define DVI_Y_1          162 /* move down and set |y| (1 byte)  */
#define DVI_Y_2          163 /* move down and set |y| (2 bytes) */
#define DVI_Y_3          164 /* move down and set |y| (3 bytes) */
#define DVI_Y_4          165 /* move down and set |y| (4 bytes) */
#define DVI_Z_0          166 /* move down by |z| */
#define DVI_Z_1          167 /* move down and set |z| (1 byte)  */
#define DVI_Z_2          168 /* move down and set |z| (2 bytes) */
#define DVI_Z_3          169 /* move down and set |z| (3 bytes) */
#define DVI_Z_4          170 /* move down and set |z| (4 bytes) */
#define DVI_FNT_NUM_0    171 /* set current font to 0 */
#define DVI_FNT_NUM_63   234 /* ... to 234 */
#define DVI_FNT_1        235 /* set current font (1 byte)  */
#define DVI_FNT_2        236 /* set current font (2 bytes) */
#define DVI_FNT_3        237 /* set current font (3 bytes) */
#define DVI_FNT_4        238 /* set current font (4 bytes) */
#define DVI_XXX_1        239 /* extension to DVI primitives (1 byte)  */
#define DVI_XXX_2        240 /* extension to DVI primitives (2 bytes) */
#define DVI_XXX_3        241 /* extension to DVI primitives (3 bytes) */
#define DVI_XXX_4        242 /* extension to DVI primitives (4 bytes) */
#define DVI_FNT_DEF_1    243 /* declare font (1 byte)  */
#define DVI_FNT_DEF_2    244 /* declare font (2 bytes) */
#define DVI_FNT_DEF_3    245 /* declare font (3 bytes) */
#define DVI_FNT_DEF_4    246 /* declare font (4 bytes) */
#define DVI_PRE          247 /* preamble */
#define DVI_POST         248 /* postamble beginning */
#define DVI_POST_POST    249 /* postamble ending */

