/* writeimg.h

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

/* $Id: writeimg.h 2448 2009-06-08 07:43:50Z taco $ */

#ifndef WRITEIMG_H
#  define WRITEIMG_H

#  include "image.h"

boolean check_image_b(integer);
boolean check_image_c(integer);
boolean check_image_i(integer);
boolean is_pdf_image(integer);
boolean is_png_image(integer);
image_dict *new_image_dict(void);
image *new_image(void);
integer epdf_orig_x(integer i);
integer epdf_orig_y(integer i);
integer epdf_xsize(integer i);
integer epdf_ysize(integer i);
integer image_colordepth(integer img);
integer image_depth(integer);
integer image_height(integer);
integer image_index(integer);
integer image_objnum(integer);
integer image_pages(integer);
integer image_width(integer);
integer img_to_array(image *);
integer read_image(integer, integer, str_number, integer, str_number,
                   str_number, integer, integer, integer, integer);
void check_pdfstream_dict(image_dict *);
void dumpimagemeta(void);
void free_image_dict(image_dict * p);
void init_image_dict(image_dict *);
void init_image(image *);
void new_img_pdfstream_struct(image_dict *);
void out_image(integer, scaled, scaled);
void pdf_print_resname_prefix(void);
void read_img(image_dict *, integer, integer);
void scale_image(integer);
void scale_img(image *);
void set_image_dimensions(integer, integer, integer, integer);
void undumpimagemeta(integer, integer);
void update_image_procset(integer);
void write_image(integer);
void write_img(image_dict *);
void write_pdfstream(image_dict *);

#endif                          /* WRITEIMG_H */
