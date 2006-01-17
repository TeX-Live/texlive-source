/*
Copyright (c) 1996-2002, 2005 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/writeimg.c#18 $
*/

#include "ptexlib.h"
#include "image.h"
#include <kpathsea/c-auto.h>
#include <kpathsea/c-memstr.h>

static const char perforce_id[] = 
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/writeimg.c#18 $";
    
#define bp2int(p)    round(p*(onehundredbp/100.0))

/* define image_ptr, image_array & image_limit */
define_array(image);   

float epdf_width;
float epdf_height;
float epdf_orig_x;
float epdf_orig_y;
integer epdf_selected_page;
integer epdf_num_pages;
integer epdf_page_box;
integer epdf_always_use_pdf_pagebox;
void *epdf_doc;

static integer new_image_entry(void)
{
    alloc_array(image, 1, SMALL_BUF_SIZE);
    image_ptr->image_type = IMAGE_TYPE_NONE;
    image_ptr->color_type = 0;
    image_ptr->num_pages = 0;
    image_ptr->x_res = 0;
    image_ptr->y_res = 0;
    image_ptr->width = 0;
    image_ptr->height = 0;
    image_ptr->colorspace_ref = 0;
    return image_ptr++ - image_array;
}

integer imagecolor(integer img)
{
    return img_color(img);
}

integer imagewidth(integer img)
{
    return img_width(img);
}

integer imageheight(integer img)
{
    return img_height(img);
}

integer imagexres(integer img)
{
    return img_xres(img);
}

integer imageyres(integer img)
{
    return img_yres(img);
}

boolean ispdfimage(integer img)
{
    return img_type(img) == IMAGE_TYPE_PDF;
}

boolean checkimageb(integer procset)
{
    return procset & IMAGE_COLOR_B;
}

boolean checkimagec(integer procset)
{
    return procset & IMAGE_COLOR_C;
}

boolean checkimagei(integer procset)
{
    return procset & IMAGE_COLOR_I;
}

void updateimageprocset(integer img)
{
    pdfimageprocset |= img_color(img);
}

integer epdforigx(integer img)
{
    return pdf_ptr(img)->orig_x;
}

integer epdforigy(integer img)
{
    return pdf_ptr(img)->orig_y;
}

integer imagepages(integer img)
{
    return img_pages(img);
}

integer imagecolordepth(integer img)
{
    switch (img_type(img)) {
    case IMAGE_TYPE_PNG:
        return png_info(img)->bit_depth;
    case IMAGE_TYPE_JPG:
        return jpg_ptr(img)->bits_per_component;
    case IMAGE_TYPE_PDF:
        return 0;
    default:
        pdftex_fail("unknown type of image");
    }
}

/*
  Patch ImageTypeDetection 2003/02/08 by Heiko Oberdiek.

  Function "readimage" performs some basic initializations.
  Then it looks at the file extension to determine the
  image type and calls specific code/functions.
    The main disadvantage is that standard file extensions
  have to be used, otherwise pdfTeX is not able to detect
  the correct image type.

  The patch now looks at the file header first regardless of
  the file extension. This is implemented in function
  "checktypebyheader". If this check fails, the traditional
  test of standard file extension is tried, done in function
  "checktypebyextension".

  Magic headers:

  * "PNG (Portable Network Graphics) Specification", Version 1.2
    (http://www.libpng.org/pub/png):

  |   3.1. PNG file signature
  |
  |      The first eight bytes of a PNG file always contain the following
  |      (decimal) values:
  |
  |         137 80 78 71 13 10 26 10

  Translation to C: "\x89PNG\r\n\x1A\n"

  * "JPEG File Interchange Format", Version 1.02:

  | o you can identify a JFIF file by looking for the following
  |   sequence: X'FF', SOI X'FF', APP0, <2 bytes to be skipped>,
  |   "JFIF", X'00'.

  Function "checktypebyheader" only looks at the first two bytes:
    "\xFF\xD8"

  * "PDF Reference", third edition:
    * The first line should contain "%PDF-1.0" until "%PDF-1.4"
      (section 3.4.1 "File Header").
    * The "implementation notes" say:

    | 3.4.1,  File Header
    |   12. Acrobat viewers require only that the header appear
    |       somewhere within the first 1024 bytes of the file.
    |   13. Acrobat viewers will also accept a header of the form
    |           %!PS-Adobe-N.n PDF-M.m

    The check in function "checktypebyheader" only implements
    the first issue. The implementation notes are not considered.
    Therefore files with garbage at start of file must have the
    standard extension.

    Functions "checktypebyheader" and "checktypebyextension":
    img_type(img) is set to IMAGE_TYPE_NONE by new_image_entry().
    Both functions tries to detect a type and set img_type(img).
    Thus a value other than IMAGE_TYPE_NONE indicates that a
    type is found.
*/

#define HEADER_JPG "\xFF\xD8"
#define HEADER_PNG "\x89PNG\r\n\x1A\n"
#define HEADER_PDF "%PDF-1."
#define MAX_HEADER (sizeof(HEADER_PNG)-1)
static void checktypebyheader(integer img)
{
    int i;
    FILE *file = NULL;
    char header[MAX_HEADER];

    if (img_type(img) != IMAGE_TYPE_NONE) /* nothing to do */
        return;

    /* read the header */
    file = xfopen(img_name(img), FOPEN_RBIN_MODE);
    for (i=0; i<MAX_HEADER; i++) {
        header[i] = xgetc(file);
        if (feof(file))
            pdftex_fail("reading image file failed");
    }
    xfclose(file, img_name(img));

    /* tests */
    if (strncmp(header, HEADER_JPG, sizeof(HEADER_JPG)-1) == 0)
        img_type(img) = IMAGE_TYPE_JPG;
    else if (strncmp(header, HEADER_PNG, sizeof(HEADER_PNG)-1) == 0)
        img_type(img) = IMAGE_TYPE_PNG;
    else if (strncmp(header, HEADER_PDF, sizeof(HEADER_PDF)-1) == 0)
        img_type(img) = IMAGE_TYPE_PDF;
}

static void checktypebyextension(integer img)
{
    char *image_suffix;

    if (img_type(img) != IMAGE_TYPE_NONE) /* nothing to do */
        return;
    /* tests */
    if ((image_suffix = strrchr(cur_file_name, '.')) == 0)
        img_type(img) = IMAGE_TYPE_NONE;
    else if (strcasecmp(image_suffix, ".pdf") == 0)
        img_type(img) = IMAGE_TYPE_PDF;
    else if (strcasecmp(image_suffix, ".png") == 0)
        img_type(img) = IMAGE_TYPE_PNG;
    else if (strcasecmp(image_suffix, ".jpg") == 0 ||
             strcasecmp(image_suffix, ".jpeg") == 0)
        img_type(img) = IMAGE_TYPE_JPG;
}

integer readimage(strnumber s, integer page_num, strnumber page_name, integer colorspace,
                  integer pdfversion, integer pdfforcepagebox,
                  integer pdfinclusionerrorlevel)
{
    char *dest = NULL;
    integer img = new_image_entry();
    img_colorspace_ref(img) = colorspace;

    /* need to allocate new string as makecstring's buffer is 
       already used by cur_file_name */
    if (page_name != 0)
      dest = xstrdup(makecstring(page_name));
    cur_file_name = makecfilename(s);
    img_name(img) = kpse_find_file(cur_file_name, kpse_tex_format, true);
    if (img_name(img) == NULL)
        pdftex_fail("cannot find image file");
    /* kpse_find_file perhaps changed the file name */
    cur_file_name = img_name(img);
    /* type checks */
    checktypebyheader(img);
    checktypebyextension(img);
    /* read image */
    switch (img_type(img)) {
    case IMAGE_TYPE_PDF:
        pdf_ptr(img) = xtalloc(1, pdf_image_struct);
        pdf_ptr(img)->page_box = pdflastpdfboxspec;
        pdf_ptr(img)->always_use_pdfpagebox = pdfforcepagebox;
        page_num = read_pdf_info(img_name(img), dest, page_num, 
                                     pdfversion, pdfforcepagebox,
                                     pdfinclusionerrorlevel);
        img_width(img) = bp2int(epdf_width);
        img_height(img) = bp2int(epdf_height);
        img_pages(img) = epdf_num_pages;
        pdf_ptr(img)->orig_x = bp2int(epdf_orig_x);
        pdf_ptr(img)->orig_y = bp2int(epdf_orig_y);
        pdf_ptr(img)->selected_page = page_num;
        pdf_ptr(img)->doc = epdf_doc;
        break;
    case IMAGE_TYPE_PNG:
        img_pages(img) = 1;
        read_png_info(img);
        break;
    case IMAGE_TYPE_JPG:
        jpg_ptr(img) = xtalloc(1, JPG_IMAGE_INFO);
        img_pages(img) = 1;
        read_jpg_info(img);
        break;
    default:
        pdftex_fail("unknown type of image");
    }
    xfree(dest);
    cur_file_name = NULL;
    return img;
}

void writeimage(integer img)
{
    cur_file_name = img_name(img);
    tex_printf(" <%s", img_name(img));
    switch (img_type(img)) {
    case IMAGE_TYPE_PNG:
        write_png(img);
        break;
    case IMAGE_TYPE_JPG:
        write_jpg(img);
        break;
    case IMAGE_TYPE_PDF:
        epdf_doc = pdf_ptr(img)->doc;
        epdf_selected_page = pdf_ptr(img)->selected_page;
        epdf_page_box = pdf_ptr(img)->page_box;
        epdf_always_use_pdf_pagebox = pdf_ptr(img)->always_use_pdfpagebox;
        write_epdf();
        break;
    default:
        pdftex_fail("unknown type of image");
    }
    tex_printf(">");
    cur_file_name = NULL;
}

void deleteimage(integer img)
{
    switch (img_type(img)) {
    case IMAGE_TYPE_PDF:
        epdf_doc = pdf_ptr(img)->doc;
        epdf_delete();
        break;
    case IMAGE_TYPE_PNG:
        xfclose(png_ptr(img)->io_ptr, cur_file_name);
        png_destroy_read_struct(&(png_ptr(img)), &(png_info(img)), NULL);
        break;
    case IMAGE_TYPE_JPG:
        xfclose(jpg_ptr(img)->file, cur_file_name);
        break;
    default:
        pdftex_fail("unknown type of image");
    }
    xfree(img_name(img));
    return;
}

void img_free() 
{
    xfree(image_array);
}
