/*  $Header$

    This is dvipdfm, a DVI to PDF translator.
    Copyright (C) 1998, 1999 by Mark A. Wicks

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    The author may be contacted via the e-mail address

	mwicks@kettering.edu
*/

#include <stdio.h>
#include <stdlib.h>
#include <kpathsea/config.h>
#include <kpathsea/c-ctype.h>
#include "system.h"
#include "config.h"
#include "mem.h"
#include "pdfobj.h"

#ifdef HAVE_LIBPNG
#include <png.h>

#define TMP "/tmp"

static unsigned char sigbytes[4];
int check_for_png (FILE *png_file) 
{
  rewind (png_file);
  if (fread (sigbytes, 1, sizeof(sigbytes), png_file) !=
      sizeof(sigbytes) ||
      (!png_check_sig (sigbytes, sizeof(sigbytes))))
    return 0;
  else
    return 1;
}

pdf_obj *start_png_image (FILE *file, char *res_name)
{
  pdf_obj *result = NULL, *dict = NULL;
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned long width, height;
  unsigned bit_depth, color_type;
  rewind (file);
  if (!(png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING,    
					  NULL, NULL, NULL)) ||
      !(info_ptr = png_create_info_struct (png_ptr))) {
    fprintf (stderr, "\n\nLibpng failed to initialize\n");
    if (png_ptr)
      png_destroy_read_struct(&png_ptr, NULL, NULL);
    return NULL;
  }
  png_init_io (png_ptr, file);
  /*  png_set_sig_bytes (png_ptr, 0); */
  /* Read PNG header */
  png_read_info (png_ptr, info_ptr);
  {
    png_color_16 default_background;
    png_color_16p file_background;

    default_background.red=255; default_background.green=255;
    default_background.blue=255; default_background.gray=0;
    default_background.index = 0;

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    /* Convert paletted images to true color */
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
      png_set_expand(png_ptr);
    }
    /* Limit image component depth to 8 bits */
    if (bit_depth == 16) {
      png_set_strip_16 (png_ptr);
    }
    if (png_get_bKGD(png_ptr, info_ptr, &file_background)) {
      png_set_background(png_ptr, file_background,
			 PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    } else {
      png_set_background(png_ptr, &default_background,
			 PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
    }
  }
  { /* Read the image in raw RGB format */
    int i, rowbytes, pdf_bit_depth;
    png_bytep *rows;
    png_read_update_info(png_ptr, info_ptr);
    rows = NEW (height, png_bytep);
    rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    for (i=0; i<height; i++) {
      rows[i] = NEW (rowbytes, png_byte);
    }
    png_read_image(png_ptr, rows);
    result = pdf_new_stream(STREAM_COMPRESS);
    dict = pdf_stream_dict(result);
    pdf_add_dict (dict, pdf_new_name ("Width"),
		  pdf_new_number(width));
    pdf_add_dict (dict, pdf_new_name ("Height"),
		  pdf_new_number(height));
    if (color_type == PNG_COLOR_TYPE_GRAY) {
      pdf_bit_depth = bit_depth;
    } else {
      pdf_bit_depth = 8;
    }
    pdf_add_dict (dict, pdf_new_name ("BitsPerComponent"),
		  pdf_new_number(pdf_bit_depth));
    if (color_type == PNG_COLOR_TYPE_GRAY ||
	color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
      pdf_add_dict (dict, pdf_new_name ("ColorSpace"),
		    pdf_new_name ("DeviceGray"));
    } else{
      pdf_add_dict (dict, pdf_new_name ("ColorSpace"),
		    pdf_new_name ("DeviceRGB"));
    }
    for (i=0; i<height; i++) {
      pdf_add_stream (result, (char *) rows[i], rowbytes);
      RELEASE (rows[i]);
    }
    RELEASE (rows);
  }
  { /* Cleanup  */
    if (info_ptr)
      png_destroy_info_struct(png_ptr, &info_ptr);
    if (png_ptr)
      png_destroy_read_struct(&png_ptr, NULL, NULL);
  }
  return result;
}

#endif /* HAVE_LIBPNG */

