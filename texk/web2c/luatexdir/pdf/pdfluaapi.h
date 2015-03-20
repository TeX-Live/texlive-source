/* pdfluaapi.h

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


#ifndef PDFLUAAPI_H
#  define PDFLUAAPI_H

extern int new_pdflua(void);
extern void pdflua_begin_page(PDF pdf);
extern void pdflua_end_page(PDF pdf, int annots, int beads);
extern void pdflua_output_pages_tree(PDF pdf);

#endif                          /* PDFLUAAPI_H */
