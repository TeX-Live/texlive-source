/* runocp.h
   
   Copyright 2006-2009 Taco Hoekwater <taco@luatex.org>

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

/* $Id: runocp.h 3302 2009-12-25 12:49:14Z oneiros $ */

#ifndef RUNOCP_H

#  define RUNOCP_H 1

#  define active_mem_size 50000 /* number of words of |active_info| for active ocps */

typedef int active_index;

extern memory_word active_info[(active_mem_size + 1)];
extern active_index active_min_ptr;
extern active_index active_max_ptr;
extern active_index active_real;

#  define active_ocp(A)       active_info[(A)].hh.u.B0
#  define active_counter(A)   active_info[(A)].hh.u.B1
#  define active_lstack_no(A) active_info[(A)+1].cint

extern boolean is_last_ocp(scaled llstack_no, int counter);

extern void print_active_ocps(void);
extern void add_ocp_stack(int min_index, scaled min_value);
extern void active_compile(void);

extern void run_ocp(void);
extern void run_otp(void);

extern void do_push_ocp_list(small_number a);
extern void do_pop_ocp_list(small_number a);
extern void do_clear_ocp_lists(small_number a);

extern void dump_active_ocp_info(void);
extern void undump_active_ocp_info(void);

extern void initialize_ocp_buffers(int ocp_buf_size, int ocp_stack_size);

/* for ocplist.h */
/* typedef int ocp_list_index ; */

#endif
