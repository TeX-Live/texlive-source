/* managed-sa.h
   
   Copyright 2006-2008 Taco Hoekwater <taco@luatex.org>

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

/* $Id$ */

#ifndef MANAGED_SA_H
#  define MANAGED_SA_H 1

/* the next two sets of three had better match up exactly, but using bare numbers
  is easier on the C compiler */

#  define HIGHPART 128
#  define MIDPART 128
#  define LOWPART 128

#  define HIGHPART_PART(a) (((a)>>14)&127)
#  define MIDPART_PART(a)  (((a)>>7)&127)
#  define LOWPART_PART(a)  ((a)&127)

#  define Mxmalloc_array(a,b)  xmalloc((b)*sizeof(a))
#  define Mxrealloc_array(a,b,c)  xrealloc((a),(c)*sizeof(b))
#  define Mxfree(a) free(a); a=NULL

typedef unsigned int sa_tree_item;

typedef struct {
    int code;
    int level;
    sa_tree_item value;
} sa_stack_item;


typedef struct {
    int stack_size;             /* initial stack size   */
    int stack_step;             /* increment stack step */
    int stack_ptr;              /* current stack point  */
    int dflt;                   /* default item value   */
    sa_tree_item ***tree;       /* item tree head       */
    sa_stack_item *stack;       /* stack tree head      */
} sa_tree_head;

typedef sa_tree_head *sa_tree;

extern sa_tree_item get_sa_item(const sa_tree head, const integer n);
extern void set_sa_item(sa_tree head, integer n, sa_tree_item v, integer gl);

extern sa_tree new_sa_tree(integer size, sa_tree_item dflt);

extern sa_tree copy_sa_tree(sa_tree head);
extern void destroy_sa_tree(sa_tree head);

extern void dump_sa_tree(sa_tree a);
extern sa_tree undump_sa_tree(void);

extern void restore_sa_stack(sa_tree a, integer gl);
extern void clear_sa_stack(sa_tree a);

#endif
