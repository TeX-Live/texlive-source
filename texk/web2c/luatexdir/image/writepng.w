% writepng.w

% Copyright 1996-2006 Han The Thanh <thanh@@pdftex.org>
% Copyright 2006-2010 Taco Hoekwater <taco@@luatex.org>

% This file is part of LuaTeX.

% LuaTeX is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free
% Software Foundation; either version 2 of the License, or (at your
% option) any later version.

% LuaTeX is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.

% You should have received a copy of the GNU General Public License along
% with LuaTeX; if not, see <http://www.gnu.org/licenses/>. 

@ @c
static const char _svn_version[] =
    "$Id: writepng.w 4258 2011-05-11 05:36:13Z taco $ "
    "$URL: http://foundry.supelec.fr/svn/luatex/branches/0.70.x/source/texk/web2c/luatexdir/image/writepng.w $";

#include <assert.h>
#include "ptexlib.h"
#include "image/image.h"
#include "image/writepng.h"

@ @c
static int transparent_page_group = -1;

static void close_and_cleanup_png(image_dict * idict)
{
    assert(idict != NULL);
    assert(img_file(idict) != NULL);
    assert(img_filepath(idict) != NULL);
    xfclose(img_file(idict), img_filepath(idict));
    img_file(idict) = NULL;
    assert(img_png_ptr(idict) != NULL);
    png_destroy_read_struct(&(img_png_png_ptr(idict)),
                            &(img_png_info_ptr(idict)), NULL);
    xfree(img_png_ptr(idict));
}

@ @c
void read_png_info(PDF pdf, image_dict * idict, img_readtype_e readtype)
{
    png_structp png_p;
    png_infop info_p;
    assert(idict != NULL);
    assert(img_type(idict) == IMG_TYPE_PNG);
    img_totalpages(idict) = 1;
    img_pagenum(idict) = 1;
    img_xres(idict) = img_yres(idict) = 0;
    assert(img_file(idict) == NULL);
    img_file(idict) = xfopen(img_filepath(idict), FOPEN_RBIN_MODE);
    assert(img_png_ptr(idict) == NULL);
    img_png_ptr(idict) = xtalloc(1, png_img_struct);
    if ((png_p = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                        NULL, NULL, NULL)) == NULL)
        pdftex_fail("libpng: png_create_read_struct() failed");
    img_png_png_ptr(idict) = png_p;
    if ((info_p = png_create_info_struct(png_p)) == NULL)
        pdftex_fail("libpng: png_create_info_struct() failed");
    img_png_info_ptr(idict) = info_p;
    if (setjmp(png_jmpbuf(png_p)))
        pdftex_fail("libpng: internal error");
    png_init_io(png_p, img_file(idict));
    png_read_info(png_p, info_p);
    /* resolution support */
    img_xsize(idict) = (int) png_get_image_width(png_p, info_p);
    img_ysize(idict) = (int) png_get_image_height(png_p, info_p);
    if (png_get_valid(png_p, info_p, PNG_INFO_pHYs)) {
        img_xres(idict) =
            round(0.0254 * (double) png_get_x_pixels_per_meter(png_p, info_p));
        img_yres(idict) =
            round(0.0254 * (double) png_get_y_pixels_per_meter(png_p, info_p));
    }
    switch (png_get_color_type(png_p, info_p)) {
    case PNG_COLOR_TYPE_PALETTE:
        img_procset(idict) |= PROCSET_IMAGE_C | PROCSET_IMAGE_I;
        break;
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        img_procset(idict) |= PROCSET_IMAGE_B;
        break;
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        img_procset(idict) |= PROCSET_IMAGE_C;
        break;
    default:
        pdftex_fail("unsupported type of color_type <%i>",
                    (int) png_get_color_type(png_p, info_p));
    }
    img_colordepth(idict) = png_get_bit_depth(png_p, info_p);
    if (readtype == IMG_CLOSEINBETWEEN)
        close_and_cleanup_png(idict);
}

@ @c
#define write_gray_pixel_16(r)                           \
    if (j % 4 == 0||j % 4 == 1) pdf_quick_out(pdf,*r++); \
  else                        smask[smask_ptr++] = *r++

#define write_gray_pixel_8(r)                   \
    if (j % 2 == 0)  pdf_quick_out(pdf,*r++);   \
    else             smask[smask_ptr++] = *r++

#define write_rgb_pixel_16(r)                                  \
    if (!(j % 8 == 6||j % 8 == 7)) pdf_quick_out(pdf,*r++);    \
    else                           smask[smask_ptr++] = *r++

#define write_rgb_pixel_8(r)                   \
    if (j % 4 != 3) pdf_quick_out(pdf,*r++);   \
    else            smask[smask_ptr++] = *r++

#define write_simple_pixel(r)    pdf_quick_out(pdf,*r++)

#define write_noninterlaced(outmac)                      \
    for (i = 0; i < (int)png_get_image_height (png_p, info_p); i++) {          \
    png_read_row(png_p, row, NULL);                      \
    r = row;                                             \
    k = (int)png_get_rowbytes(png_p, info_p);				 \
    while(k > 0) {                                       \
        l = (k > pdf->buf_size)? pdf->buf_size : k;      \
                pdf_room(pdf,l);                         \
                for (j = 0; j < l; j++) {                \
                  outmac;                                \
                }                                        \
                k -= l;                                  \
            }                                            \
        }

#define write_interlaced(outmac)                         \
    for (i = 0; i < (int)png_get_image_height (png_p, info_p); i++) {          \
            row = rows[i];                               \
            k = (int)png_get_rowbytes(png_p, info_p);			 \
            while(k > 0) {                               \
                l = (k > pdf->buf_size)?pdf->buf_size: k;\
                pdf_room(pdf,l);                         \
                for (j = 0; j < l; j++) {                \
                  outmac;                                \
                }                                        \
                k -= l;                                  \
            }                                            \
            xfree(rows[i]);                              \
        }

@ @c
static void write_png_palette(PDF pdf, image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    int palette_objnum = 0;
    png_colorp palette;
    int num_palette;

    png_get_PLTE(png_p, info_p, &palette, &num_palette);

    if (img_colorspace(idict) != 0) {
        pdf_printf(pdf, "%i 0 R\n", (int) img_colorspace(idict));
    } else {
        palette_objnum = pdf_create_obj(pdf, obj_type_others, 0);
        pdf_printf(pdf, "[/Indexed /DeviceRGB %i %i 0 R]\n",
                   (int) (num_palette - 1), (int) palette_objnum);
    }
    pdf_begin_stream(pdf);
    if (png_get_interlace_type(png_p, info_p) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_get_image_height (png_p, info_p) * png_get_rowbytes(png_p, info_p) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height (png_p, info_p), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height (png_p, info_p); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        png_read_image(png_p, rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdf_end_stream(pdf);
    if (palette_objnum > 0) {
        pdf_begin_dict(pdf, palette_objnum, 0);
        pdf_begin_stream(pdf);
        for (i = 0; i < num_palette; i++) {
            pdf_room(pdf, 3);
            pdf_quick_out(pdf, palette[i].red);
            pdf_quick_out(pdf, palette[i].green);
            pdf_quick_out(pdf, palette[i].blue);
        }
        pdf_end_stream(pdf);
    }
}

@ @c
static void write_png_gray(PDF pdf, image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    if (img_colorspace(idict) != 0) {
        pdf_printf(pdf, "%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts(pdf, "/DeviceGray\n");
    }
    pdf_begin_stream(pdf);
    if (png_get_interlace_type(png_p, info_p) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_get_image_height (png_p, info_p) * png_get_rowbytes(png_p, info_p) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height (png_p, info_p), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height (png_p, info_p); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        png_read_image(png_p, rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdf_end_stream(pdf);
}

@ @c
static void write_png_gray_alpha(PDF pdf, image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    int smask_objnum = 0;
    png_bytep smask;
    int smask_ptr = 0;
    int smask_size = 0;
    png_byte bitdepth;
    if (img_colorspace(idict) != 0) {
        pdf_printf(pdf, "%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts(pdf, "/DeviceGray\n");
    }
    smask_objnum = pdf_create_obj(pdf, obj_type_others, 0);
    pdf_printf(pdf, "/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (int) ((png_get_rowbytes(png_p, info_p) / 2) * png_get_image_height (png_p, info_p));
    smask = xtalloc((unsigned) smask_size, png_byte);
    pdf_begin_stream(pdf);
    if (png_get_interlace_type(png_p, info_p) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        if ((png_get_bit_depth(png_p, info_p) == 16) && (pdf->image_hicolor != 0)) {
            write_noninterlaced(write_gray_pixel_16(r));
        } else {
            write_noninterlaced(write_gray_pixel_8(r));
        }
        xfree(row);
    } else {
        if (png_get_image_height (png_p, info_p) * png_get_rowbytes(png_p, info_p) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height (png_p, info_p), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height (png_p, info_p); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        png_read_image(png_p, rows);
        if ((png_get_bit_depth(png_p, info_p) == 16) && (pdf->image_hicolor != 0)) {
            write_interlaced(write_gray_pixel_16(row));
        } else {
            write_interlaced(write_gray_pixel_8(row));
        }
        xfree(rows);
    }
    pdf_end_stream(pdf);
    pdf_flush(pdf);
    /* now write the Smask object */
    bitdepth = png_get_bit_depth (png_p, info_p);
    pdf_begin_dict(pdf, smask_objnum, 0);
    pdf_puts(pdf, "/Type /XObject\n/Subtype /Image\n");
    if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
        pdf_printf(pdf, "%s\n", img_attr(idict));
    pdf_printf(pdf, "/Width %i\n/Height %i\n/BitsPerComponent %i\n",
               (int) png_get_image_width (png_p, info_p),
               (int) png_get_image_height (png_p, info_p),
               (bitdepth == 16 ? 8 : bitdepth));
    pdf_puts(pdf, "/ColorSpace /DeviceGray\n");
    pdf_begin_stream(pdf);
    for (i = 0; i < smask_size; i++) {
        if (i % 8 == 0)
            pdf_room(pdf, 8);
        pdf_quick_out(pdf, smask[i]);
        if (bitdepth == 16)
            i++;
    }
    pdf_end_stream(pdf);
    xfree(smask);
}

@ @c
static void write_png_rgb(PDF pdf, image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    if (img_colorspace(idict) != 0) {
        pdf_printf(pdf, "%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts(pdf, "/DeviceRGB\n");
    }
    pdf_begin_stream(pdf);
    if (png_get_interlace_type(png_p, info_p) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_get_image_height (png_p, info_p) * png_get_rowbytes(png_p, info_p) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height (png_p, info_p), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height (png_p, info_p); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        png_read_image(png_p, rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdf_end_stream(pdf);
}

@ @c
static void write_png_rgb_alpha(PDF pdf, image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    int smask_objnum = 0;
    png_bytep smask;
    int smask_ptr = 0;
    int smask_size = 0;
    png_byte bitdepth;
    if (img_colorspace(idict) != 0) {
        pdf_printf(pdf, "%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts(pdf, "/DeviceRGB\n");
    }
    smask_objnum = pdf_create_obj(pdf, obj_type_others, 0);
    pdf_printf(pdf, "/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (int) ((png_get_rowbytes (png_p, info_p) / 4) * png_get_image_height (png_p, info_p));
    smask = xtalloc((unsigned) smask_size, png_byte);
    pdf_begin_stream(pdf);
    if (png_get_interlace_type(png_p, info_p) == PNG_INTERLACE_NONE) {
        row = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        if ((png_get_bit_depth(png_p, info_p) == 16) && (pdf->image_hicolor != 0)) {
            write_noninterlaced(write_rgb_pixel_16(r));
        } else {
            write_noninterlaced(write_rgb_pixel_8(r));
        }
        xfree(row);
    } else {
        if (png_get_image_height (png_p, info_p) * png_get_rowbytes(png_p, info_p) >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_get_image_height (png_p, info_p), png_bytep);
        for (i = 0; (unsigned) i < png_get_image_height (png_p, info_p); i++)
            rows[i] = xtalloc(png_get_rowbytes(png_p, info_p), png_byte);
        png_read_image(png_p, rows);
        if ((png_get_bit_depth(png_p, info_p) == 16) && (pdf->image_hicolor != 0)) {
            write_interlaced(write_rgb_pixel_16(row));
        } else {
            write_interlaced(write_rgb_pixel_8(row));
        }
        xfree(rows);
    }
    pdf_end_stream(pdf);
    pdf_flush(pdf);
    /* now write the Smask object */
    if (smask_objnum > 0) {
        bitdepth = png_get_bit_depth (png_p, info_p);
        pdf_begin_dict(pdf, smask_objnum, 0);
        pdf_puts(pdf, "/Type /XObject\n/Subtype /Image\n");
        if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
            pdf_printf(pdf, "%s\n", img_attr(idict));
        pdf_printf(pdf, "/Width %i\n/Height %i\n/BitsPerComponent %i\n",
                   (int) png_get_image_width (png_p, info_p),
                   (int) png_get_image_height (png_p, info_p),
                   (bitdepth == 16 ? 8 : bitdepth));
        pdf_puts(pdf, "/ColorSpace /DeviceGray\n");
        pdf_begin_stream(pdf);
        for (i = 0; i < smask_size; i++) {
            if (i % 8 == 0)
                pdf_room(pdf, 8);
            pdf_quick_out(pdf, smask[i]);
            if (bitdepth == 16)
                i++;
        }
        xfree(smask);
        pdf_end_stream(pdf);
    }
}

@ The |copy_png| function is from Hartmut Henkel. The goal is to use
pdf's native FlateDecode support if that is possible.

Only a subset of the png files allows this, but when possible it
greatly improves inclusion speed.

Code cheerfully gleaned from Thomas Merz' PDFlib,
file |p_png.c| "SPNG - Simple PNG"


@c
static int spng_getint(FILE * fp)
{
    unsigned char buf[4];
    if (fread(buf, 1, 4, fp) != 4)
        pdftex_fail("writepng: reading chunk type failed");
    return ((((((int) buf[0] << 8) + buf[1]) << 8) + buf[2]) << 8) + buf[3];
}

#define SPNG_CHUNK_IDAT 0x49444154
#define SPNG_CHUNK_IEND 0x49454E44

static void copy_png(PDF pdf, image_dict * idict)
{
    png_structp png_p;
    png_infop info_p;
    FILE *fp;
    int i, len, type, streamlength = 0;
    boolean endflag = false;
    int idat = 0;               /* flag to check continuous IDAT chunks sequence */
    assert(idict != NULL);
    png_p = img_png_png_ptr(idict);
    info_p = img_png_info_ptr(idict);
    fp = (FILE *) png_get_io_ptr(png_p);
    /* 1st pass to find overall stream /Length */
    if (fseek(fp, 8, SEEK_SET) != 0)
        pdftex_fail("writepng: fseek in PNG file failed");
    do {
        len = spng_getint(fp);
        type = spng_getint(fp);
        switch (type) {
        case SPNG_CHUNK_IEND:
            endflag = true;
            break;
        case SPNG_CHUNK_IDAT:
            streamlength += len;
        default:
            if (fseek(fp, len + 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
        }
    } while (endflag == false);
    pdf_printf(pdf, "/Length %d\n"
               "/Filter/FlateDecode\n"
               "/DecodeParms<<"
               "/Colors %d"
               "/Columns %u"
               "/BitsPerComponent %i"
               "/Predictor 10>>\n>>\nstream\n", streamlength,
               png_get_color_type (png_p, info_p) == 2 ? 3 : 1,
               png_get_image_width (png_p, info_p), 
               png_get_bit_depth (png_p, info_p));
    /* 2nd pass to copy data */
    endflag = false;
    if (fseek(fp, 8, SEEK_SET) != 0)
        pdftex_fail("writepng: fseek in PNG file failed");
    do {
        len = spng_getint(fp);
        type = spng_getint(fp);
        switch (type) {
        case SPNG_CHUNK_IDAT:  /* do copy */
            if (idat == 2)
                pdftex_fail("writepng: IDAT chunk sequence broken");
            idat = 1;
            while (len > 0) {
                i = (len > pdf->buf_size) ? pdf->buf_size : len;
                pdf_room(pdf, i);
                fread(&pdf->buf[pdf->ptr], 1, (size_t) i, fp);
                pdf->ptr += i;
                len -= i;
            }
            if (fseek(fp, 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
            break;
        case SPNG_CHUNK_IEND:  /* done */
            pdf_end_stream(pdf);
            endflag = true;
            break;
        default:
            if (idat == 1)
                idat = 2;
            if (fseek(fp, len + 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
        }
    } while (endflag == false);
}

@ @c
static void reopen_png(PDF pdf, image_dict * idict)
{
    int width, height, xres, yres;
    width = img_xsize(idict);   /* do consistency check */
    height = img_ysize(idict);
    xres = img_xres(idict);
    yres = img_yres(idict);
    read_png_info(pdf, idict, IMG_KEEPOPEN);
    if (width != img_xsize(idict) || height != img_ysize(idict)
        || xres != img_xres(idict) || yres != img_yres(idict))
        pdftex_fail("writepng: image dimensions have changed");
}

@ @c
static boolean last_png_needs_page_group;

void write_png(PDF pdf, image_dict * idict)
{
    boolean png_copy = true;
    double gamma = 0.0;
    png_fixed_point int_file_gamma = 0;
#ifndef PNG_FP_1
   /* for libpng < 1.5.0 */
#define PNG_FP_1    100000
#endif
    int i;
    int palette_objnum = 0;
    png_structp png_p;
    png_infop info_p;
    assert(idict != NULL);
    last_png_needs_page_group = false;
    if (img_file(idict) == NULL)
        reopen_png(pdf, idict);
    assert(img_png_ptr(idict) != NULL);
    png_p = img_png_png_ptr(idict);
    info_p = img_png_info_ptr(idict);
    if (pdf->minor_version < 5)
        pdf->image_hicolor = 0;
    pdf_puts(pdf, "/Type /XObject\n/Subtype /Image\n");
    if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
        pdf_printf(pdf, "%s\n", img_attr(idict));
    /* simple transparency support */
    if (png_get_valid(png_p, info_p, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_p);
        png_copy = false;
    }
    /* alpha channel support */
    if (pdf->minor_version < 4
        && png_get_color_type(png_p, info_p) | PNG_COLOR_MASK_ALPHA) {
        png_set_strip_alpha(png_p);
        png_copy = false;
    }
    /* 16 bit depth support */
    if (pdf->minor_version < 5)
        pdf->image_hicolor = 0;
    if ((png_get_bit_depth(png_p, info_p) == 16) && (pdf->image_hicolor == 0)) {
        png_set_strip_16(png_p);
        png_copy = false;
    }
    /* gamma support */
    if (png_get_valid(png_p, info_p, PNG_INFO_gAMA)) {
        png_get_gAMA(png_p, info_p, &gamma);
        png_get_gAMA_fixed(png_p, info_p, &int_file_gamma);
    }
    if (pdf->image_apply_gamma) {
        if (png_get_valid(png_p, info_p, PNG_INFO_gAMA))
            png_set_gamma(png_p, (pdf->gamma / 1000.0), gamma);
        else
            png_set_gamma(png_p, (pdf->gamma / 1000.0),
                          (1000.0 / pdf->image_gamma));
        png_copy = false;
    }
    /* reset structure */
    (void) png_set_interlace_handling(png_p);
    png_read_update_info(png_p, info_p);

    pdf_printf(pdf, "/Width %i\n/Height %i\n/BitsPerComponent %i\n",
               (int) png_get_image_width(png_p, info_p),
               (int) png_get_image_height(png_p, info_p),
               (int) png_get_bit_depth(png_p, info_p));
    pdf_puts(pdf, "/ColorSpace ");
    if (png_copy && pdf->minor_version > 1
        && png_get_interlace_type(png_p, info_p) == PNG_INTERLACE_NONE
        && (png_get_color_type(png_p, info_p) == PNG_COLOR_TYPE_GRAY
            || png_get_color_type(png_p, info_p) == PNG_COLOR_TYPE_RGB)
        && !pdf->image_apply_gamma
        && (!png_get_valid(png_p, info_p, PNG_INFO_gAMA)
            || int_file_gamma == PNG_FP_1)
        && !png_get_valid(png_p, info_p, PNG_INFO_cHRM)
        && !png_get_valid(png_p, info_p, PNG_INFO_iCCP)
        && !png_get_valid(png_p, info_p, PNG_INFO_sBIT)
        && !png_get_valid(png_p, info_p, PNG_INFO_sRGB)
        && !png_get_valid(png_p, info_p, PNG_INFO_bKGD)
        && !png_get_valid(png_p, info_p, PNG_INFO_hIST)
        && !png_get_valid(png_p, info_p, PNG_INFO_tRNS)
        && !png_get_valid(png_p, info_p, PNG_INFO_sPLT)
        ) {
        /* Copy PNG */
        png_colorp palette;
        int num_palette;

        png_get_PLTE(png_p, info_p, &palette, &num_palette);
        if (img_colorspace(idict) != 0) {
            pdf_printf(pdf, "%i 0 R\n", (int) img_colorspace(idict));
        } else {
            switch (png_get_color_type(png_p, info_p)) {
            case PNG_COLOR_TYPE_PALETTE:
                palette_objnum = pdf_create_obj(pdf, obj_type_others, 0);
                pdf_printf(pdf, "[/Indexed /DeviceRGB %i %i 0 R]\n",
                           num_palette - 1, (int) palette_objnum);
                break;
            case PNG_COLOR_TYPE_GRAY:
                pdf_puts(pdf, "/DeviceGray\n");
                break;
            default:           /* RGB */
                pdf_puts(pdf, "/DeviceRGB\n");
            };
        }
        if (tracefilenames)
            tex_printf(" (PNG copy)");
        copy_png(pdf, idict);
        if (palette_objnum > 0) {
            pdf_begin_dict(pdf, palette_objnum, 0);
            pdf_begin_stream(pdf);
            for (i = 0; i < num_palette; i++) {
                pdf_room(pdf, 3);
                pdf_quick_out(pdf, palette[i].red);
                pdf_quick_out(pdf, palette[i].green);
                pdf_quick_out(pdf, palette[i].blue);
            }
            pdf_end_stream(pdf);
        }
    } else {
        if (0) {
            tex_printf(" *** PNG copy skipped because: ");
            if (!png_copy)
                tex_printf("!png_copy ");
            if (!(pdf->minor_version > 1))
                tex_printf("minorversion=%d ", pdf->minor_version);
            if (!(png_get_interlace_type(png_p, info_p) == PNG_INTERLACE_NONE))
                tex_printf("interlaced ");
            if (!((png_get_color_type(png_p, info_p) == PNG_COLOR_TYPE_GRAY)
                  || (png_get_color_type(png_p, info_p) == PNG_COLOR_TYPE_RGB)))
                tex_printf("colortype ");
            if (pdf->image_apply_gamma)
                tex_printf("apply gamma ");
            if (!(!png_get_valid(png_p, info_p, PNG_INFO_gAMA)
                  || int_file_gamma == PNG_FP_1))
                tex_printf("gamma ");
            if (png_get_valid(png_p, info_p, PNG_INFO_cHRM))
                tex_printf("cHRM ");
            if (png_get_valid(png_p, info_p, PNG_INFO_iCCP))
                tex_printf("iCCP ");
            if (png_get_valid(png_p, info_p, PNG_INFO_sBIT))
                tex_printf("sBIT ");
            if (png_get_valid(png_p, info_p, PNG_INFO_sRGB))
                tex_printf("sRGB ");
            if (png_get_valid(png_p, info_p, PNG_INFO_bKGD))
                tex_printf("bKGD ");
            if (png_get_valid(png_p, info_p, PNG_INFO_hIST))
                tex_printf("hIST ");
            if (png_get_valid(png_p, info_p, PNG_INFO_tRNS))
                tex_printf("tRNS ");
            if (png_get_valid(png_p, info_p, PNG_INFO_sPLT))
                tex_printf("sPLT ");
        }
        switch (png_get_color_type(png_p, info_p)) {
        case PNG_COLOR_TYPE_PALETTE:
            write_png_palette(pdf, idict);
            break;
        case PNG_COLOR_TYPE_GRAY:
            write_png_gray(pdf, idict);
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (pdf->minor_version >= 4) {
                write_png_gray_alpha(pdf, idict);
                last_png_needs_page_group = true;
            } else
                write_png_gray(pdf, idict);
            break;
        case PNG_COLOR_TYPE_RGB:
            write_png_rgb(pdf, idict);
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            if (pdf->minor_version >= 4) {
                write_png_rgb_alpha(pdf, idict);
                last_png_needs_page_group = true;
            } else
                write_png_rgb(pdf, idict);
            break;
        default:
            pdftex_fail("unsupported type of color_type <%i>",
                        png_get_color_type(png_p, info_p));
        }
    }
    pdf_flush(pdf);
    close_and_cleanup_png(idict);
}

@ @c
static boolean transparent_page_group_was_written = false;

@ Called after the xobject generated by |write_png| has been finished; used to
write out additional objects
@c
void write_additional_png_objects(PDF pdf)
{
    (void) pdf;
    (void) transparent_page_group;
    (void) transparent_page_group_was_written;
    return; /* this interferes with current macro-based usage and cannot be configured */
#if 0
    if (last_png_needs_page_group) {
        if (!transparent_page_group_was_written && transparent_page_group > 1) {
            /* create new group object */
            transparent_page_group_was_written = true;
            pdf_begin_obj(pdf, transparent_page_group, 2);
            if (pdf->compress_level == 0) {
                pdf_puts(pdf, "%PTEX Group needed for transparent pngs\n");
            }
            pdf_puts
                (pdf,
                 "<</Type/Group /S/Transparency /CS/DeviceRGB /I true /K true>>\n");
            pdf_end_obj(pdf);
        }
    }
#endif
}
