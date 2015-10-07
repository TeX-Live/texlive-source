/* linebreak.h

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


#ifndef LINEBREAK_H
#  define LINEBREAK_H

#  define left_side 0
#  define right_side 1

extern halfword just_box;       /* the |hlist_node| for the last line of the new paragraph */

extern void line_break(boolean d, int line_break_context);

#  define inf_bad 10000         /* infinitely bad value */
#  define awful_bad 07777777777 /* more than a billion demerits */

extern void initialize_active(void);

extern void ext_do_line_break(int paragraph_dir,
                              int pretolerance,
                              int tracing_paragraphs,
                              int tolerance,
                              scaled emergency_stretch,
                              int looseness,
                              /*
                              int hyphen_penalty,
                              int ex_hyphen_penalty,
                              */
                              int adjust_spacing,
                              halfword par_shape_ptr,
                              int adj_demerits,
                              int protrude_chars,
                              int line_penalty,
                              int last_line_fit,
                              int double_hyphen_demerits,
                              int final_hyphen_demerits,
                              int hang_indent,
                              int hsize,
                              int hang_after,
                              halfword left_skip,
                              halfword right_skip,
                              halfword inter_line_penalties_ptr,
                              int inter_line_penalty,
                              int club_penalty,
                              halfword club_penalties_ptr,
                              halfword widow_penalties_ptr,
                              int widow_penalty,
                              int broken_penalty, halfword final_par_glue);

extern void get_linebreak_info(int *, int *);
extern halfword find_protchar_left(halfword l, boolean d);
extern halfword find_protchar_right(halfword l, halfword r);

/* skipable nodes at the margins during character protrusion */

#  define cp_skipable(a) ((! is_char_node((a))) &&                        \
                        ((type((a)) == ins_node)                        \
                         || (type((a)) == mark_node)                    \
                         || (type((a)) == adjust_node)                  \
                         || (type((a)) == penalty_node)                 \
                         || ((type((a)) == whatsit_node) &&             \
                             (subtype((a)) != pdf_refximage_node) &&    \
                             (subtype((a)) != pdf_refxform_node))       \
                         /* reference to an image or XObject form */    \
                         || ((type((a)) == disc_node) &&                \
                             (vlink_pre_break(a) == null) &&            \
                             (vlink_post_break(a) == null) &&           \
                             (vlink_no_break(a) == null))               \
                         /* an empty |disc_node| */                     \
                         || ((type((a)) == math_node) &&                \
                             (surround((a)) == 0))                      \
                         || ((type((a)) == kern_node) &&                \
                             ((width((a)) == 0) ||                      \
                              (subtype((a)) == normal)))                \
                         || ((type((a)) == glue_node) &&                \
                             (glue_ptr((a)) == zero_glue))              \
                         || ((type((a)) == hlist_node) &&               \
                             (width((a)) == 0) &&                       \
                             (height((a)) == 0) &&                      \
                             (depth((a)) == 0) &&                       \
                             (list_ptr((a)) == null))                   \
                         ))


#endif
