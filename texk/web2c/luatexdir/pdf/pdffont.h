/* pdffont.h

   Copyright 2010 Taco Hoekwater <taco@luatex.org>

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

/* $Id: pdffont.h 3430 2010-02-11 21:42:02Z hhenkel $ */

#ifndef PDFFONT_H
#  define PDFFONT_H

extern void output_one_char(PDF pdf, internal_font_number ffi, int c);

extern void pdf_init_font(PDF pdf, internal_font_number f);
extern internal_font_number pdf_set_font(PDF pdf, internal_font_number f);

extern int pk_dpi;              /* PK pixel density value from \.{texmf.cnf} */

extern void copy_expand_params(internal_font_number k, internal_font_number f,
                               int e);
extern internal_font_number tfm_lookup(char *s, scaled fs, int e);
extern internal_font_number expand_font(internal_font_number f, int e);

extern void set_expand_params(internal_font_number f, boolean auto_expand,
                              int stretch_limit, int shrink_limit,
                              int font_step, int expand_ratio);

extern void read_expand_font(void);
extern void new_letterspaced_font(small_number a);
extern void make_font_copy(small_number a);

extern void pdf_include_chars(PDF);
extern void glyph_to_unicode(void);

#endif
