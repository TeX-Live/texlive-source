/* pdfaction.h

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


#ifndef PDFACTION_H
#  define PDFACTION_H

/* pdf action spec */

#  define pdf_action_size 4

typedef enum {
    pdf_action_page = 0,
    pdf_action_goto,
    pdf_action_thread,
    pdf_action_user
} pdf_action_type;

typedef enum {
    pdf_window_notset,
    pdf_window_new,
    pdf_window_nonew,
} pdf_window_type;


#  define pdf_action_type(a)        type((a) + 1)       /* enum pdf_action_type */
#  define pdf_action_named_id(a)    subtype((a) + 1)    /* boolean */
#  define pdf_action_id(a)          vlink((a) + 1)      /* number or toks */
#  define pdf_action_file(a)        vinfo((a) + 2)      /* toks */
#  define pdf_action_new_window(a)  vlink((a) + 2)      /* enum pdf_window_type */
#  define pdf_action_tokens(a)      vinfo((a) + 3)      /* toks */
#  define pdf_action_refcount(a)    vlink((a) + 3)      /* number */

/* increase count of references to this action. this is used to speed up copy_node() */

#  define add_action_ref(a) pdf_action_refcount((a))++

/* decrease count of references to this
   action; free it if there is no reference to this action*/

#  define delete_action_ref(a) {                                        \
        if (pdf_action_refcount(a) == null) {                           \
            delete_action_node(a);                                      \
        } else {                                                        \
            pdf_action_refcount(a)--;                                   \
        }                                                               \
    }


#  define set_pdf_action_type(A,B) pdf_action_type(A)=B
#  define set_pdf_action_tokens(A,B) pdf_action_tokens(A)=B
#  define set_pdf_action_file(A,B) pdf_action_file(A)=B
#  define set_pdf_action_id(A,B) pdf_action_id(A)=B
#  define set_pdf_action_named_id(A,B) pdf_action_named_id(A)=B
#  define set_pdf_action_new_window(A,B) pdf_action_new_window(A)=B

extern halfword scan_action(PDF pdf);
extern void write_action(PDF pdf, halfword p);
extern void delete_action_node(halfword a);

#endif
