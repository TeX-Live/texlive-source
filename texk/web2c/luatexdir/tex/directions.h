/* directions.h

   Copyright 2009 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */


#ifndef DIRECTIONS_H
#  define DIRECTIONS_H

#  define dir_T 0
#  define dir_L 1
#  define dir_B 2
#  define dir_R 3

#  define dir_TLT  0
#  define dir_TRT  4
#  define dir_LTL  9
#  define dir_RTT  24

extern const char *dir_strings[128];

                                   /* #  define dir_array_size  25 *//* |dir_RTT + 1| */

/* inv(primary) == tertiary */
/*
boolean _is_mirrored[dir_array_size];
_is_mirrored[dir_TLT] = 0;
_is_mirrored[dir_TRT] = 0;
_is_mirrored[dir_LTL] = 0;
_is_mirrored[dir_RTT] = 0;
*/

#  define is_mirrored(a) 0

/* secondary == tertiary */
/*
boolean _is_rotated[dir_array_size];
_is_rotated[dir_TLT] = 0;
_is_rotated[dir_TRT] = 0;
_is_rotated[dir_LTL] = 0;
_is_rotated[dir_RTT] = 1;
*/

#  define is_rotated(a) (a == dir_RTT)

/* secondary == secondary */
/*
boolean _textdir_parallel[dir_array_size][dir_array_size];
_textdir_parallel[dir_TLT][dir_TLT] = 1;
_textdir_parallel[dir_TLT][dir_TRT] = 1;
_textdir_parallel[dir_TLT][dir_LTL] = 0;
_textdir_parallel[dir_TLT][dir_RTT] = 0;
_textdir_parallel[dir_TRT][dir_TLT] = 1;
_textdir_parallel[dir_TRT][dir_TRT] = 1;
_textdir_parallel[dir_TRT][dir_LTL] = 0;
_textdir_parallel[dir_TRT][dir_RTT] = 0;
_textdir_parallel[dir_LTL][dir_TLT] = 0;
_textdir_parallel[dir_LTL][dir_TRT] = 0;
_textdir_parallel[dir_LTL][dir_LTL] = 1;
_textdir_parallel[dir_LTL][dir_RTT] = 1;
_textdir_parallel[dir_RTT][dir_TLT] = 0;
_textdir_parallel[dir_RTT][dir_TRT] = 0;
_textdir_parallel[dir_RTT][dir_LTL] = 1;
_textdir_parallel[dir_RTT][dir_RTT] = 1;
*/

#  define textdir_parallel(a,b)  (((a == dir_TLT || a == dir_TRT)&&(b == dir_TLT || b == dir_TRT)) || \
				((a == dir_LTL || a == dir_RTT)&&(b == dir_LTL || b == dir_RTT)))


/* primary == primary */
/*
boolean _pardir_parallel[dir_array_size][dir_array_size];
_pardir_parallel[dir_TLT][dir_TLT] = 1;
_pardir_parallel[dir_TLT][dir_TRT] = 1;
_pardir_parallel[dir_TLT][dir_LTL] = 0;
_pardir_parallel[dir_TLT][dir_RTT] = 0;
_pardir_parallel[dir_TRT][dir_TLT] = 1;
_pardir_parallel[dir_TRT][dir_TRT] = 1;
_pardir_parallel[dir_TRT][dir_LTL] = 0;
_pardir_parallel[dir_TRT][dir_RTT] = 0;
_pardir_parallel[dir_LTL][dir_TLT] = 0;
_pardir_parallel[dir_LTL][dir_TRT] = 0;
_pardir_parallel[dir_LTL][dir_LTL] = 1;
_pardir_parallel[dir_LTL][dir_RTT] = 1;
_pardir_parallel[dir_RTT][dir_TLT] = 0;
_pardir_parallel[dir_RTT][dir_TRT] = 0;
_pardir_parallel[dir_RTT][dir_LTL] = 1;
_pardir_parallel[dir_RTT][dir_RTT] = 1;
*/


#  define pardir_parallel(a,b) (((a == dir_TLT || a == dir_TRT)&&(b == dir_TLT || b == dir_TRT)) || \
			      ((a == dir_LTL || a == dir_RTT)&&(b == dir_LTL || b == dir_RTT)))


/* inv(primary) == primary */
/*
boolean _pardir_opposite[dir_array_size][dir_array_size];
_pardir_opposite[dir_TLT][dir_TLT] = 0;
_pardir_opposite[dir_TLT][dir_TRT] = 0;
_pardir_opposite[dir_TLT][dir_LTL] = 0;
_pardir_opposite[dir_TLT][dir_RTT] = 0;
_pardir_opposite[dir_TRT][dir_TLT] = 0;
_pardir_opposite[dir_TRT][dir_TRT] = 0;
_pardir_opposite[dir_TRT][dir_LTL] = 0;
_pardir_opposite[dir_TRT][dir_RTT] = 0;
_pardir_opposite[dir_LTL][dir_TLT] = 0;
_pardir_opposite[dir_LTL][dir_TRT] = 0;
_pardir_opposite[dir_LTL][dir_LTL] = 0;
_pardir_opposite[dir_LTL][dir_RTT] = 1;
_pardir_opposite[dir_RTT][dir_TLT] = 0;
_pardir_opposite[dir_RTT][dir_TRT] = 0;
_pardir_opposite[dir_RTT][dir_LTL] = 1;
_pardir_opposite[dir_RTT][dir_RTT] = 0;
*/

#  define pardir_opposite(a,b) ((a == dir_LTL && b == dir_RTT)||(a == dir_RTT && b == dir_LTL))


/* inv(secondary) == secondary */
/*
boolean _textdir_opposite[dir_array_size][dir_array_size];
_textdir_opposite[dir_TLT][dir_TLT] = 0;
_textdir_opposite[dir_TLT][dir_TRT] = 1;
_textdir_opposite[dir_TLT][dir_LTL] = 0;
_textdir_opposite[dir_TLT][dir_RTT] = 0;
_textdir_opposite[dir_TRT][dir_TLT] = 1;
_textdir_opposite[dir_TRT][dir_TRT] = 0;
_textdir_opposite[dir_TRT][dir_LTL] = 0;
_textdir_opposite[dir_TRT][dir_RTT] = 0;
_textdir_opposite[dir_LTL][dir_TLT] = 0;
_textdir_opposite[dir_LTL][dir_TRT] = 0;
_textdir_opposite[dir_LTL][dir_LTL] = 0;
_textdir_opposite[dir_LTL][dir_RTT] = 0;
_textdir_opposite[dir_RTT][dir_TLT] = 0;
_textdir_opposite[dir_RTT][dir_TRT] = 0;
_textdir_opposite[dir_RTT][dir_LTL] = 0;
_textdir_opposite[dir_RTT][dir_RTT] = 0;
*/

#  define textdir_opposite(a,b) ((a == dir_TLT && b == dir_TRT)||(a == dir_TRT && b == dir_TLT))


/* inv(tertiary) == tertiary */
/*
boolean _glyphdir_opposite[dir_array_size][dir_array_size];
_glyphdir_opposite[dir_TLT][dir_TLT] = 0;
_glyphdir_opposite[dir_TLT][dir_TRT] = 0;
_glyphdir_opposite[dir_TLT][dir_LTL] = 0;
_glyphdir_opposite[dir_TLT][dir_RTT] = 0;
_glyphdir_opposite[dir_TRT][dir_TLT] = 0;
_glyphdir_opposite[dir_TRT][dir_TRT] = 0;
_glyphdir_opposite[dir_TRT][dir_LTL] = 0;
_glyphdir_opposite[dir_TRT][dir_RTT] = 0;
_glyphdir_opposite[dir_LTL][dir_TLT] = 0;
_glyphdir_opposite[dir_LTL][dir_TRT] = 0;
_glyphdir_opposite[dir_LTL][dir_LTL] = 0;
_glyphdir_opposite[dir_LTL][dir_RTT] = 0;
_glyphdir_opposite[dir_RTT][dir_TLT] = 0;
_glyphdir_opposite[dir_RTT][dir_TRT] = 0;
_glyphdir_opposite[dir_RTT][dir_LTL] = 0;
_glyphdir_opposite[dir_RTT][dir_RTT] = 0;
*/

#  define glyphdir_opposite(a,b) 0


/* primary == primary */
/*
boolean _pardir_eq[dir_array_size][dir_array_size];
_pardir_eq[dir_TLT][dir_TLT] = 1;
_pardir_eq[dir_TLT][dir_TRT] = 1;
_pardir_eq[dir_TLT][dir_LTL] = 0;
_pardir_eq[dir_TLT][dir_RTT] = 0;
_pardir_eq[dir_TRT][dir_TLT] = 1;
_pardir_eq[dir_TRT][dir_TRT] = 1;
_pardir_eq[dir_TRT][dir_LTL] = 0;
_pardir_eq[dir_TRT][dir_RTT] = 0;
_pardir_eq[dir_LTL][dir_TLT] = 0;
_pardir_eq[dir_LTL][dir_TRT] = 0;
_pardir_eq[dir_LTL][dir_LTL] = 1;
_pardir_eq[dir_LTL][dir_RTT] = 0;
_pardir_eq[dir_RTT][dir_TLT] = 0;
_pardir_eq[dir_RTT][dir_TRT] = 0;
_pardir_eq[dir_RTT][dir_LTL] = 0;
_pardir_eq[dir_RTT][dir_RTT] = 1;
*/

#  define pardir_eq(a,b) (((a == dir_TLT || a == dir_TRT)&&(b == dir_TLT || b == dir_TRT))|| \
			(a == dir_LTL && b == dir_LTL) ||		\
			(a == dir_RTT && b == dir_RTT))

/* secondary == secondary */
/*
boolean _textdir_eq[dir_array_size][dir_array_size];
_textdir_eq[dir_TLT][dir_TLT] = 1;
_textdir_eq[dir_TLT][dir_TRT] = 0;
_textdir_eq[dir_TLT][dir_LTL] = 0;
_textdir_eq[dir_TLT][dir_RTT] = 0;
_textdir_eq[dir_TRT][dir_TLT] = 0;
_textdir_eq[dir_TRT][dir_TRT] = 1;
_textdir_eq[dir_TRT][dir_LTL] = 0;
_textdir_eq[dir_TRT][dir_RTT] = 0;
_textdir_eq[dir_LTL][dir_TLT] = 0;
_textdir_eq[dir_LTL][dir_TRT] = 0;
_textdir_eq[dir_LTL][dir_LTL] = 1;
_textdir_eq[dir_LTL][dir_RTT] = 1;
_textdir_eq[dir_RTT][dir_TLT] = 0;
_textdir_eq[dir_RTT][dir_TRT] = 0;
_textdir_eq[dir_RTT][dir_LTL] = 1;
_textdir_eq[dir_RTT][dir_RTT] = 1;
*/

#  define textdir_eq(a,b) ((a == dir_TLT && b == dir_TLT) || \
			 (a == dir_TRT && b == dir_TRT) || \
			 (a == dir_LTL && (b == dir_LTL || b == dir_RTT)) || \
			 (a == dir_RTT && (b == dir_LTL || b == dir_RTT))


/* tertiary == tertiary */
/*
boolean _glyphdir_eq[dir_array_size][dir_array_size];
_glyphdir_eq[dir_TLT][dir_TLT] = 1;
_glyphdir_eq[dir_TLT][dir_TRT] = 1;
_glyphdir_eq[dir_TLT][dir_LTL] = 0;
_glyphdir_eq[dir_TLT][dir_RTT] = 1;
_glyphdir_eq[dir_TRT][dir_TLT] = 1;
_glyphdir_eq[dir_TRT][dir_TRT] = 1;
_glyphdir_eq[dir_TRT][dir_LTL] = 0;
_glyphdir_eq[dir_TRT][dir_RTT] = 1;
_glyphdir_eq[dir_LTL][dir_TLT] = 0;
_glyphdir_eq[dir_LTL][dir_TRT] = 0;
_glyphdir_eq[dir_LTL][dir_LTL] = 1;
_glyphdir_eq[dir_LTL][dir_RTT] = 0;
_glyphdir_eq[dir_RTT][dir_TLT] = 1;
_glyphdir_eq[dir_RTT][dir_TRT] = 1;
_glyphdir_eq[dir_RTT][dir_LTL] = 0;
_glyphdir_eq[dir_RTT][dir_RTT] = 1;
*/

#  define glyphdir_eq(a,b) ((a != dir_LTL && b != dir_LTL) || \
			  (a == dir_LTL && b == dir_LTL))


/* primary == secondary */
/*
boolean _partextdir_eq[dir_array_size][dir_array_size];
_partextdir_eq[dir_TLT][dir_TLT] = 0;
_partextdir_eq[dir_TLT][dir_TRT] = 0;
_partextdir_eq[dir_TLT][dir_LTL] = 1;
_partextdir_eq[dir_TLT][dir_RTT] = 1;
_partextdir_eq[dir_TRT][dir_TLT] = 0;
_partextdir_eq[dir_TRT][dir_TRT] = 0;
_partextdir_eq[dir_TRT][dir_LTL] = 1;
_partextdir_eq[dir_TRT][dir_RTT] = 1;
_partextdir_eq[dir_LTL][dir_TLT] = 1;
_partextdir_eq[dir_LTL][dir_TRT] = 0;
_partextdir_eq[dir_LTL][dir_LTL] = 0;
_partextdir_eq[dir_LTL][dir_RTT] = 0;
_partextdir_eq[dir_RTT][dir_TLT] = 0;
_partextdir_eq[dir_RTT][dir_TRT] = 1;
_partextdir_eq[dir_RTT][dir_LTL] = 0;
_partextdir_eq[dir_RTT][dir_RTT] = 0;
*/

#  define partextdir_eq(a,b) (((a == dir_TLT || a == dir_TRT)&&(b == dir_LTL || b == dir_RTT)) || \
			    (a == dir_LTL && b == dir_TLT) ||		\
			    (a == dir_RTT && b == dir_TRT))


/* secondary != tertiary */
/*
boolean _textglyphdir_orthogonal[dir_array_size]
_textglyphdir_orthogonal[dir_TLT] = 1;
_textglyphdir_orthogonal[dir_TRT] = 1;
_textglyphdir_orthogonal[dir_LTL] = 1;
_textglyphdir_orthogonal[dir_RTT] = 0;
*/

#  define textglyphdir_orthogonal(a) (a != dir_RTT)

/* secondary == L */
/*
boolean _textdir_is[dir_array_size];
_textdir_is[dir_TLT] = 1;
_textdir_is[dir_TRT] = 1;
_textdir_is[dir_LTL] = 1;
_textdir_is[dir_RTT] = 0;
*/

#  define textdir_is_L(a) (a == dir_TLT)

#  define push_dir(a,b)				\
   { halfword dir_tmp=new_dir((a));             \
       vlink(dir_tmp)=b;                        \
       b=dir_tmp;                               \
   }

#  define push_dir_node(a,b)		    \
   { halfword dir_tmp=copy_node((a));	    \
       vlink(dir_tmp)=b;                    \
       b=dir_tmp;                           \
   }

#  define pop_dir_node(b)                 \
   { halfword dir_tmp=b;                  \
       b=vlink(dir_tmp);                  \
       flush_node(dir_tmp);               \
   }

extern halfword dir_ptr;

extern halfword text_dir_ptr;

extern void initialize_directions(void);
extern halfword new_dir(int s);

extern const char *string_dir(int d);
extern void print_dir(int d);

extern void scan_direction(void);

extern halfword do_push_dir_node(halfword p, halfword a);
extern halfword do_pop_dir_node(halfword p);

scaled pack_width(int curdir, int pdir, halfword p, boolean isglyph);
scaled_whd pack_width_height_depth(int curdir, int pdir, halfword p,
                                   boolean isglyph);

void update_text_dir_ptr(int val);

#endif
