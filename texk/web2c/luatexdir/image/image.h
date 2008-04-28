/*
Copyright (c) 1996-2002 Han The Thanh, <thanh@pdftex.org>

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

$Id: image.h 1125 2008-03-25 19:36:52Z hhenkel $
*/

#ifndef IMAGE_H
#  define IMAGE_H

#  include <../libpng/png.h>

#  define JPG_UINT16      unsigned int
#  define JPG_UINT32      unsigned long
#  define JPG_UINT8       unsigned char

#  define IMAGE_COLOR_B   1
#  define IMAGE_COLOR_C   2
#  define IMAGE_COLOR_I   4

extern integer zround(double);  /* from zround.c */
#  define bp2int(p)       zround(p * (one_hundred_bp / 100.0))

#  define TYPE_IMG        "image"
#  define TYPE_IMG_DICT   "image-dict"

#  define scaled          integer

/**********************************************************************/

typedef struct {
    png_structp png_ptr;
    png_infop info_ptr;
} png_img_struct;

typedef struct {
    int color_space;            /* used color space. See JPG_ constants */
    JPG_UINT32 length;          /* length of file/data */
} jpg_img_struct;

#if 0
typedef struct {                /* currently unused */
} jb2_img_struct;
#endif

typedef enum { DICT_NEW,        /* fresh dictionary */
    DICT_FILESCANNED,           /* image file scanned */
    DICT_REFERED,               /* pdf_refximage_node in node list --> read-only dict */
    DICT_OUTIMG,                /* /Im* appears in pagestream */
    DICT_SCHEDULED,             /* image dict scheduled for writing */
    DICT_WRITTEN                /* image dict written to file */
} dict_state;

typedef enum { IMAGE_TYPE_NONE, IMAGE_TYPE_PDF, IMAGE_TYPE_PNG, IMAGE_TYPE_JPG,
    IMAGE_TYPE_JBIG2, IMAGE_TYPE_SENTINEL
} imgtype_e;

typedef enum { IMG_KEEPOPEN, IMG_CLOSEINBETWEEN } img_readtype_e;

typedef enum { PDF_BOX_SPEC_NONE, PDF_BOX_SPEC_MEDIA, PDF_BOX_SPEC_CROP,
    PDF_BOX_SPEC_BLEED, PDF_BOX_SPEC_TRIM, PDF_BOX_SPEC_ART,
    PDF_BOX_SPEC_SENTINEL
} pdfboxspec_e;

/**********************************************************************/

typedef struct {
    integer objnum;
    integer index;              /* /Im1, /Im2, ... */
    integer x_size;             /* dimensions in pixel counts as in JPG/PNG/JBIG2 file */
    integer y_size;
    integer x_orig;             /* origin in sp for PDF files */
    integer y_orig;
    integer x_res;              /* pixel resolution as in JPG/PNG/JBIG2 file */
    integer y_res;
    integer colorspace;         /* number of /ColorSpace object */
    integer total_pages;
    integer page_num;           /* requested page (by number) */
    char *pagename;             /* requested page (by name) */
    char *filename;             /* requested raw file name */
    char *filepath;             /* full file path after kpathsea */
    char *attr;                 /* additional image dict entries */
    FILE *file;
    imgtype_e image_type;
    int color_space;            /* used color space. See JPG_ constants */
    int color_depth;            /* color depth */
    pdfboxspec_e page_box_spec; /* PDF page box spec.: media/crop/bleed/trim/art */
    dict_state state;
    union {
        png_img_struct *png;
        jpg_img_struct *jpg;
        /* jb2_img_struct *jb2; */
    } img_struct;
} image_dict;

#  define img_objnum(N)         ((N)->objnum)
#  define img_index(N)          ((N)->index)
#  define img_xsize(N)          ((N)->x_size)
#  define img_ysize(N)          ((N)->y_size)
#  define img_xorig(N)          ((N)->x_orig)
#  define img_yorig(N)          ((N)->y_orig)
#  define img_xres(N)           ((N)->x_res)
#  define img_yres(N)           ((N)->y_res)
#  define img_colorspace(N)     ((N)->colorspace)
#  define img_totalpages(N)     ((N)->total_pages)
#  define img_pagenum(N)        ((N)->page_num)
#  define img_pagename(N)       ((N)->pagename)
#  define img_filename(N)       ((N)->filename)
#  define img_filepath(N)       ((N)->filepath)
#  define img_attr(N)           ((N)->attr)
#  define img_file(N)           ((N)->file)
#  define img_type(N)           ((N)->image_type)
#  define img_color(N)          ((N)->color_space)
#  define img_colordepth(N)     ((N)->color_depth)
#  define img_pagebox(N)        ((N)->page_box_spec)
#  define img_state(N)          ((N)->state)

#  define img_png_ptr(N)        ((N)->img_struct.png)
#  define img_png_png_ptr(N)    ((N)->img_struct.png->png_ptr)
#  define img_png_info_ptr(N)   ((N)->img_struct.png->info_ptr)

#  define img_jpg_ptr(N)        ((N)->img_struct.jpg)
#  define img_jpg_color(N)      ((N)->img_struct.jpg->color_space)

#  define img_jb2_ptr(N)        ((N)->img_struct.jb2)

/**********************************************************************/

typedef struct {
    integer width;              /* requested/actual TeX dimensions */
    integer height;
    integer depth;
    integer transform;
    integer flags;
    image_dict *dict;
    int dict_ref;               /* luaL_ref() reference */
} image;

#  define img_width(N)          ((N)->width)
#  define img_height(N)         ((N)->height)
#  define img_depth(N)          ((N)->depth)
#  define img_transform(N)      ((N)->transform)
#  define img_flags(N)          ((N)->flags)
#  define img_dict(N)           ((N)->dict)
#  define img_dictref(N)        ((N)->dict_ref)

#  define F_FLAG_SCALED         0x01
#  define F_FLAG_REFERED        0x02

#  define img_flags(N)          ((N)->flags)
#  define img_set_scaled(N)     (img_flags(N) |= F_FLAG_SCALED)
#  define img_set_refered(N)    (img_flags(N) |= F_FLAG_REFERED)
#  define img_unset_scaled(N)   (img_flags(N) &= ~F_FLAG_SCALED)
#  define img_unset_refered(N)  (img_flags(N) &= ~F_FLAG_REFERED)
#  define img_is_scaled(N)      ((img_flags(N) & F_FLAG_SCALED) != 0)
#  define img_is_refered(N)     ((img_flags(N) & F_FLAG_REFERED) != 0)

#  define set_wd_running(N)     (img_width(N) = null_flag)
#  define set_ht_running(N)     (img_height(N) = null_flag)
#  define set_dp_running(N)     (img_depth(N) = null_flag)
#  define is_wd_running(N)      (img_width(N) == null_flag)
#  define is_ht_running(N)      (img_height(N) == null_flag)
#  define is_dp_running(N)      (img_depth(N) == null_flag)

/**********************************************************************/

/* writeimg.c */

image *new_image();
image_dict *new_image_dict();
void init_image(image *);
void init_image_dict(image_dict *);
void scale_img(image *);
integer img_to_array(image *);
void delete_image(image *);
void free_image_dict(image_dict * p);
void read_img(image_dict *, integer, integer);

/* writepng.c */

#  ifndef boolean               /* TODO: from where to get the original definition? */
#    define boolean int
#  endif

void read_png_info(image_dict *, img_readtype_e);
void read_jpg_info(image_dict *, img_readtype_e);
void read_jbig2_info(image_dict *);
void read_pdf_info(image_dict *, integer, integer);
void write_img(image_dict *);
void write_png(image_dict *);
void write_jpg(image_dict *);
void write_jbig2(image_dict *);
void write_epdf(image_dict *);

/* pdftoepdf.cc */

void unrefPdfDocument(char *);

#endif
