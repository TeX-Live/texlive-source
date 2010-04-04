/* pdfshipout.h

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

/* $Id: pdfshipout.h 2977 2009-08-08 12:49:35Z hhenkel $ */

#ifndef PDFSHIPOUT_H
#  define PDFSHIPOUT_H

extern boolean is_shipping_page;        /* set to |shipping_page| when |ship_out| starts */
extern scaledpos shipbox_refpos;        /* for \gleaders */

extern void ship_out(PDF pdf, halfword p, boolean shipping_page);

#endif
