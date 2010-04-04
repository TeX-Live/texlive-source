/* readocp.h
   
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

/* $Id: readocp.h 3127 2009-11-11 10:14:11Z taco $ */

#ifndef READOCP_H

#  define READOCP_H 1

#  define ocp_info(A,B) ocp_tables[(A)][(B)]

typedef enum {
    offset_ocp_file_size = 0,
    offset_ocp_name,
    offset_ocp_area,
    offset_ocp_external,
    offset_ocp_external_arg,
    offset_ocp_input,
    offset_ocp_output,
    offset_ocp_no_tables,
    offset_ocp_no_states,
    offset_ocp_table_base,
    offset_ocp_state_base,
    offset_ocp_info             /* 11 */
} ocp_offsets;

#  define ocp_file_size(A) ocp_info(A,offset_ocp_file_size)
#  define ocp_name(A) ocp_info(A,offset_ocp_name)
#  define ocp_area(A) ocp_info(A,offset_ocp_area)
#  define ocp_external(A) ocp_info(A,offset_ocp_external)
#  define ocp_external_arg(A) ocp_info(A,offset_ocp_external_arg)
#  define ocp_input(A) ocp_info(A,offset_ocp_input)
#  define ocp_output(A) ocp_info(A,offset_ocp_output)
#  define ocp_no_tables(A) ocp_info(A,offset_ocp_no_tables)
#  define ocp_no_states(A) ocp_info(A,offset_ocp_no_states)
#  define ocp_table_base(A) ocp_info(A,offset_ocp_table_base)
#  define ocp_state_base(A) ocp_info(A,offset_ocp_state_base)

extern internal_ocp_number read_ocp_info(pointer u, char *nom,
                                         char *aire, char *ext,
                                         boolean external_ocp);

extern void init_null_ocp(str_number a, str_number n);

#endif
