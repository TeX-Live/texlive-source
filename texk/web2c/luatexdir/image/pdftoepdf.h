/* pdftoepdf.h

   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
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

/* $Id: pdftoepdf.h 2331 2009-04-18 16:39:50Z hhenkel $ */

#ifndef PDFTOEPDF_H
#  define PDFTOEPDF_H

#  include "image.h"

void read_pdf_info(image_dict *, integer, integer);
void unrefPdfDocument(char *);
void write_additional_epdf_objects(void);
void write_epdf(image_dict *);
void epdf_check_mem(void);

/* epdf.c --- this should go in an own header file */
extern integer get_fontfile_num(int);
extern integer get_fontname_num(int);
extern void epdf_free(void);

#endif                          /* PDFTOEPDF_H */
