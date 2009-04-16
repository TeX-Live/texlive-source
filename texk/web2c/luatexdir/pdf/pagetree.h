/* pagetree.h

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

/* $Id: pagetree.h 2267 2009-04-12 18:23:21Z oneiros $ */

#ifndef PAGETREE_H
#  define PAGETREE_H
/**********************************************************************/

integer output_pages_tree();
integer pdf_do_page_divert(integer, integer);
void pdf_do_page_undivert(integer, integer);

/**********************************************************************/
#endif
