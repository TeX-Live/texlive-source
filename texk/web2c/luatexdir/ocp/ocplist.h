/* ocplist.h
   
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

/* $Id: ocplist.h 3302 2009-12-25 12:49:14Z oneiros $ */

#ifndef OCPLIST_H

#  define OCPLIST_H 1

#  define ocp_list_base 0       /* smallest internal ocp list number; must not be less
                                   than |min_quarterword| */
#  define number_ocp_lists 32768
#  define null_ocp_list ocp_list_base

typedef int internal_ocp_list_number;
typedef int ocp_list_index;     /* index into |ocp_list_info| */
typedef int ocp_lstack_index;   /* index into |ocp_lstack_info| */

extern memory_word *ocp_list_info;
extern memory_word *ocp_lstack_info;
extern internal_ocp_list_number ocp_list_ptr;
extern ocp_list_index *ocp_list_list;

#  define ocp_list_lnext(A) ocp_list_info[(A)].hh.u.B0
#  define ocp_list_lstack(A) ocp_list_info[(A)].hh.u.B1
#  define ocp_list_lstack_no(A) ocp_list_info[(A)+1].cint
#  define ocp_lstack_lnext(A) ocp_lstack_info[(A)].hh.u.B0
#  define ocp_lstack_ocp(A) ocp_lstack_info[(A)].hh.u.B1
#  define make_null_ocp_list() make_ocp_list_node(0,ocp_maxint,0)
#  define is_null_ocp_list(A) (ocp_list_lstack_no(A)==ocp_maxint)

#  define make_null_ocp_lstack() 0
#  define is_null_ocp_lstack(A) ((A)==0)

typedef enum {
    add_before_op = 1,
    add_after_op,
    remove_before_op,
    remove_after_op
} ocplist_ops;


extern void initialize_init_ocplists(void);
extern void initialize_ocplist_arrays(int ocp_list_size);

extern ocp_list_index make_ocp_list_node(ocp_lstack_index llstack,
                                         scaled llstack_no,
                                         ocp_list_index llnext);

extern ocp_lstack_index make_ocp_lstack_node(internal_ocp_number locp,
                                             ocp_lstack_index llnext);

extern ocp_lstack_index copy_ocp_lstack(ocp_lstack_index llstack);

extern ocp_list_index copy_ocp_list(ocp_list_index list);

extern ocp_list_index ocp_ensure_lstack(ocp_list_index list, scaled llstack_no);

extern void ocp_apply_add(ocp_list_index list_entry,
                          boolean lbefore, internal_ocp_number locp);

extern void ocp_apply_remove(ocp_list_index list_entry, boolean lbefore);

extern void print_ocp_lstack(ocp_lstack_index lstack_entry);
extern void print_ocp_list(ocp_list_index list_entry);
extern ocp_list_index scan_ocp_list(void);
extern internal_ocp_list_number read_ocp_list(void);
extern void scan_ocp_list_ident(void);

extern void new_ocp_list(small_number a);

extern void dump_ocplist_info(void);
extern void undump_ocplist_info(void);

#endif
