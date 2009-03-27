/* primitive.h

   Copyright 2008-2009 Taco Hoekwater <taco@luatex.org>

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

/* $Id: primitive.h 2086 2009-03-22 15:32:08Z oneiros $ */

#ifndef LUATEX_PRIMITIVE_H
#  define LUATEX_PRIMITIVE_H 1

/* This enum is a list of origins for primitive commands */

typedef enum {
    tex_command = 1,
    etex_command = 2,
    aleph_command = 4,
    omega_command = 8,
    pdftex_command = 16,
    luatex_command = 32,
    core_command = 64,
} command_origin;

#  define eq_level(a) zeqtb[a].hh.u.B1
#  define eq_type(a)  zeqtb[a].hh.u.B0
#  define equiv(a)    zeqtb[a].hh.v.RH

#  define undefined_primitive 0
#  define prim_size 2100        /* maximum number of primitives */
#  define prim_prime 1777       /* about 85\pct! of |primitive_size| */

#  define hash_prime 55711      /* a prime number equal to about 85\pct! of |hash_size| */
#  define hash_base 2

extern void init_primitives(void);
extern void ini_init_primitives(void);

extern halfword compute_pool_hash(pool_pointer j, pool_pointer l,
                                  halfword prime_number);
extern pointer prim_lookup(str_number s);

extern boolean is_primitive(str_number csname);

extern quarterword get_prim_eq_type(integer p);
extern halfword get_prim_equiv(integer p);
extern str_number get_prim_text(integer p);
extern quarterword get_prim_origin(integer p);

extern void dump_primitives(void);
extern void undump_primitives(void);

#  define primitive_tex(a,b,c,d)    primitive((a),(b),(c),(d),tex_command)
#  define primitive_etex(a,b,c,d)   primitive((a),(b),(c),(d),etex_command)
#  define primitive_aleph(a,b,c,d)  primitive((a),(b),(c),(d),aleph_command)
#  define primitive_omega(a,b,c,d)  primitive((a),(b),(c),(d),omega_command)
#  define primitive_pdftex(a,b,c,d) primitive((a),(b),(c),(d),pdftex_command)
#  define primitive_luatex(a,b,c,d) primitive((a),(b),(c),(d),luatex_command)
#  define primitive_core(a,b,c,d)   primitive((a),(b),(c),(d),core_command)

extern void primitive(str_number ss, quarterword c, halfword o, halfword off,
                      int cmd_origin);
extern void primitive_def(char *s, size_t l, quarterword c, halfword o);
extern void print_cmd_chr(quarterword cmd, halfword chr_code);

extern pointer string_lookup(char *s, size_t l);
extern pointer id_lookup(integer j, integer l);


#endif                          /* LUATEX_PRIMITIVE_H */
