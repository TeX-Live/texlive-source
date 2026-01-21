/* This is xdvipsk, an eXtended version of dvips(k) by Tomas Rokicki.

	Copyright (C) 2016 by VTeX Ltd (www.vtex.lt),
	the xdvipsk project team - Sigitas Tolusis and Arunas Povilaitis.

    Program original code copyright (C) 2007-2014 by Jin-Hwan Cho and 
	Shunsaku Hirata, the dvipdfmx project team.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#ifndef _CS_TYPE2_H_
#define _CS_TYPE2_H_

#include "cff_types.h"
#include "utarray.h"


typedef struct {
  double x;
  double y;
} cs_point;

typedef struct {
  long size;
  card8 *data;
} cs_type1subr;

extern void cs_start_conversion(int precision, boolean makeSubrs,
								UT_array *t1_glyphs, UT_array *t1_subrs);

extern long cs_convert_charstring (card8 *src, long srclen,
								   cff_index *gsubr, cff_index *subr,
								   double default_width, double nominal_width);

extern void cs_end_conversion();

#endif /* _CS_TYPE2_H_ */
