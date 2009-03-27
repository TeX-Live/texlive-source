/* writepng.c
   
   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2008 Taco Hoekwater <taco@luatex.org>

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

#include <assert.h>
#include "ptexlib.h"
#include "image.h"

static const char _svn_version[] =
    "$Id: writepng.c 2029 2009-03-14 19:10:25Z oneiros $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/image/writepng.c $";

static int transparent_page_group = -1;

void close_and_cleanup_png(image_dict * idict)
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
    img_png_ptr(idict) = NULL;
}

void read_png_info(image_dict * idict, img_readtype_e readtype)
{
    double gamma;
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
    if (setjmp(png_p->jmpbuf))
        pdftex_fail("libpng: internal error");
    png_init_io(png_p, img_file(idict));
    png_read_info(png_p, info_p);
    /* simple transparency support */
    if (png_get_valid(png_p, info_p, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_p);
    }
    /* alpha channel support  */
    if (fixed_pdf_minor_version < 4 && png_p->color_type | PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png_p);
    /* 16bit depth support */
    if (fixed_pdf_minor_version < 5)
        fixed_image_hicolor = 0;
    if (info_p->bit_depth == 16 && !fixed_image_hicolor)
        png_set_strip_16(png_p);
    /* gamma support */
    if (fixed_image_apply_gamma) {
        if (png_get_gAMA(png_p, info_p, &gamma))
            png_set_gamma(png_p, (fixed_gamma / 1000.0), gamma);
        else
            png_set_gamma(png_p, (fixed_gamma / 1000.0),
                          (1000.0 / fixed_image_gamma));
    }
    /* reset structure */
    png_read_update_info(png_p, info_p);
    /* resolution support */
    img_xsize(idict) = info_p->width;
    img_ysize(idict) = info_p->height;
    if (info_p->valid & PNG_INFO_pHYs) {
        img_xres(idict) =
            round(0.0254 * png_get_x_pixels_per_meter(png_p, info_p));
        img_yres(idict) =
            round(0.0254 * png_get_y_pixels_per_meter(png_p, info_p));
    }
    switch (info_p->color_type) {
    case PNG_COLOR_TYPE_PALETTE:
        img_color(idict) = IMAGE_COLOR_C | IMAGE_COLOR_I;
        break;
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        img_color(idict) = IMAGE_COLOR_B;
        break;
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        img_color(idict) = IMAGE_COLOR_C;
        break;
    default:
        pdftex_fail("unsupported type of color_type <%i>", info_p->color_type);
    }
    img_colordepth(idict) = info_p->bit_depth;
    if (readtype == IMG_CLOSEINBETWEEN)
        close_and_cleanup_png(idict);
}

#define write_gray_pixel_16(r)                           \
  if (j % 4 == 0||j % 4 == 1) pdf_buf[pdf_ptr++] = *r++; \
  else                        smask[smask_ptr++] = *r++

#define write_gray_pixel_8(r)                   \
    if (j % 2 == 0)  pdf_buf[pdf_ptr++] = *r++; \
    else             smask[smask_ptr++] = *r++


#define write_rgb_pixel_16(r)                                  \
    if (!(j % 8 == 6||j % 8 == 7)) pdf_buf[pdf_ptr++]  = *r++; \
    else                           smask[smask_ptr++] = *r++

#define write_rgb_pixel_8(r)                   \
    if (j % 4 != 3) pdf_buf[pdf_ptr++] = *r++; \
    else            smask[smask_ptr++] = *r++

#define write_simple_pixel(r)    pdf_buf[pdf_ptr++] = *r++

#define write_noninterlaced(outmac)                      \
  for (i = 0; (unsigned) i < (int)info_p->height; i++) { \
    png_read_row(png_p, row, NULL);                      \
    r = row;                                             \
    k = info_p->rowbytes;                                \
    while(k > 0) {                                       \
        l = (k > pdf_buf_size)? pdf_buf_size : k;        \
                pdfroom(l);                              \
                for (j = 0; j < l; j++) {                \
                  outmac;                                \
                }                                        \
                k -= l;                                  \
            }                                            \
        }

#define write_interlaced(outmac)                         \
  for (i = 0; (unsigned) i < (int)info_p->height; i++) { \
            row = rows[i];                               \
            k = info_p->rowbytes;                        \
            while(k > 0) {                               \
                l = (k > pdf_buf_size)? pdf_buf_size : k;\
                pdfroom(l);                              \
                for (j = 0; j < l; j++) {                \
                  outmac;                                \
                }                                        \
                k -= l;                                  \
            }                                            \
            xfree(rows[i]);                              \
        }

void write_png_palette(image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    integer palette_objnum = 0;
    if (img_colorspace(idict) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_create_obj(0, 0);
        palette_objnum = obj_ptr;
        pdf_printf("[/Indexed /DeviceRGB %i %i 0 R]\n",
                   (int) (info_p->num_palette - 1), (int) palette_objnum);
    }
    pdf_begin_stream();
    if (info_p->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(info_p->rowbytes, png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (info_p->height * info_p->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(info_p->height, png_bytep);
        for (i = 0; (unsigned) i < info_p->height; i++)
            rows[i] = xtalloc(info_p->rowbytes, png_byte);
        png_read_image(png_p, rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdf_end_stream();
    if (palette_objnum > 0) {
        pdf_begin_dict(palette_objnum, 0);
        pdf_begin_stream();
        for (i = 0; (unsigned) i < info_p->num_palette; i++) {
            pdfroom(3);
            pdf_buf[pdf_ptr++] = info_p->palette[i].red;
            pdf_buf[pdf_ptr++] = info_p->palette[i].green;
            pdf_buf[pdf_ptr++] = info_p->palette[i].blue;
        }
        pdf_end_stream();
    }
}

void write_png_gray(image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    if (img_colorspace(idict) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts("/DeviceGray\n");
    }
    pdf_begin_stream();
    if (info_p->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(info_p->rowbytes, png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (info_p->height * info_p->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(info_p->height, png_bytep);
        for (i = 0; (unsigned) i < info_p->height; i++)
            rows[i] = xtalloc(info_p->rowbytes, png_byte);
        png_read_image(png_p, rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdf_end_stream();
}

void write_png_gray_alpha(image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    integer smask_objnum = 0;
    png_bytep smask;
    integer smask_ptr = 0;
    integer smask_size = 0;
    int bitdepth;
    if (img_colorspace(idict) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts("/DeviceGray\n");
    }
    pdf_create_obj(0, 0);
    smask_objnum = obj_ptr;
    pdf_printf("/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (info_p->rowbytes / 2) * info_p->height;
    smask = xtalloc(smask_size, png_byte);
    pdf_begin_stream();
    if (info_p->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(info_p->rowbytes, png_byte);
        if ((info_p->bit_depth == 16) && fixed_image_hicolor) {
            write_noninterlaced(write_gray_pixel_16(r));
        } else {
            write_noninterlaced(write_gray_pixel_8(r));
        }
        xfree(row);
    } else {
        if (info_p->height * info_p->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(info_p->height, png_bytep);
        for (i = 0; (unsigned) i < info_p->height; i++)
            rows[i] = xtalloc(info_p->rowbytes, png_byte);
        png_read_image(png_p, rows);
        if ((info_p->bit_depth == 16) && fixed_image_hicolor) {
            write_interlaced(write_gray_pixel_16(row));
        } else {
            write_interlaced(write_gray_pixel_8(row));
        }
        xfree(rows);
    }
    pdf_end_stream();
    pdf_flush();
    /* now write the Smask object */
    bitdepth = (int) info_p->bit_depth;
    pdf_begin_dict(smask_objnum, 0);
    pdf_puts("/Type /XObject\n/Subtype /Image\n");
    if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
        pdf_printf("%s\n", img_attr(idict));
    pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
               (int) info_p->width,
               (int) info_p->height, (bitdepth == 16 ? 8 : bitdepth));
    pdf_puts("/ColorSpace /DeviceGray\n");
    pdf_begin_stream();
    for (i = 0; i < smask_size; i++) {
        if (i % 8 == 0)
            pdfroom(8);
        pdf_buf[pdf_ptr++] = smask[i];
        if (bitdepth == 16)
            i++;
    }
    pdf_end_stream();
    xfree(smask);
}

void write_png_rgb(image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    if (img_colorspace(idict) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts("/DeviceRGB\n");
    }
    pdf_begin_stream();
    if (info_p->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(info_p->rowbytes, png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (info_p->height * info_p->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(info_p->height, png_bytep);
        for (i = 0; (unsigned) i < info_p->height; i++)
            rows[i] = xtalloc(info_p->rowbytes, png_byte);
        png_read_image(png_p, rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdf_end_stream();
}

void write_png_rgb_alpha(image_dict * idict)
{
    int i, j, k, l;
    png_structp png_p = img_png_png_ptr(idict);
    png_infop info_p = img_png_info_ptr(idict);
    png_bytep row, r, *rows;
    integer smask_objnum = 0;
    png_bytep smask;
    integer smask_ptr = 0;
    integer smask_size = 0;
    int bitdepth;
    if (img_colorspace(idict) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace(idict));
    } else {
        pdf_puts("/DeviceRGB\n");
    }
    pdf_create_obj(0, 0);
    smask_objnum = obj_ptr;
    pdf_printf("/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (info_p->rowbytes / 2) * info_p->height;
    smask = xtalloc(smask_size, png_byte);
    pdf_begin_stream();
    if (info_p->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(info_p->rowbytes, png_byte);
        if ((info_p->bit_depth == 16) && fixed_image_hicolor) {
            write_noninterlaced(write_rgb_pixel_16(r));
        } else {
            write_noninterlaced(write_rgb_pixel_8(r));
        }
        xfree(row);
    } else {
        if (info_p->height * info_p->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(info_p->height, png_bytep);
        for (i = 0; (unsigned) i < info_p->height; i++)
            rows[i] = xtalloc(info_p->rowbytes, png_byte);
        png_read_image(png_p, rows);
        if ((info_p->bit_depth == 16) && fixed_image_hicolor) {
            write_interlaced(write_rgb_pixel_16(row));
        } else {
            write_interlaced(write_rgb_pixel_8(row));
        }
        xfree(rows);
    }
    pdf_end_stream();
    pdf_flush();
    /* now write the Smask object */
    if (smask_objnum > 0) {
        bitdepth = (int) info_p->bit_depth;
        pdf_begin_dict(smask_objnum, 0);
        pdf_puts("/Type /XObject\n/Subtype /Image\n");
        if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
            pdf_printf("%s\n", img_attr(idict));
        pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
                   (int) info_p->width,
                   (int) info_p->height, (bitdepth == 16 ? 8 : bitdepth));
        pdf_puts("/ColorSpace /DeviceGray\n");
        pdf_begin_stream();
        for (i = 0; i < smask_size; i++) {
            if (i % 8 == 0)
                pdfroom(8);
            pdf_buf[pdf_ptr++] = smask[i];
            if (bitdepth == 16)
                i++;
        }
        xfree(smask);
        pdf_end_stream();
    }
}

/**********************************************************************/
/*
 * The |copy_png| function is from Hartmut Henkel. The goal is to use 
 * pdf's native FlateDecode support if that is possible.
 *
 * Only a subset of the png files allows this, but when possible it
 * greatly improves inclusion speed. 
 *
 * Code cheerfully gleaned from Thomas Merz' PDFlib,
 * file p_png.c "SPNG - Simple PNG"
 */

static int spng_getint(FILE * fp)
{
    unsigned char buf[4];
    if (fread(buf, 1, 4, fp) != 4)
        pdftex_fail("writepng: reading chunk type failed");
    return ((((((int) buf[0] << 8) + buf[1]) << 8) + buf[2]) << 8) + buf[3];
}

#define SPNG_CHUNK_IDAT 0x49444154
#define SPNG_CHUNK_IEND 0x49454E44

void copy_png(image_dict * idict)
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
    fp = (FILE *) png_p->io_ptr;
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
    pdf_printf("/Length %d\n"
               "/Filter/FlateDecode\n"
               "/DecodeParms<<"
               "/Colors %d"
               "/Columns %d"
               "/BitsPerComponent %i"
               "/Predictor 10>>\n>>\nstream\n", streamlength,
               info_p->color_type == 2 ? 3 : 1,
               (int) info_p->width, info_p->bit_depth);
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
                i = (len > pdf_buf_size) ? pdf_buf_size : len;
                pdfroom(i);
                fread(&pdf_buf[pdf_ptr], 1, i, fp);
                pdf_ptr += i;
                len -= i;
            }
            if (fseek(fp, 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
            break;
        case SPNG_CHUNK_IEND:  /* done */
            pdf_end_stream();
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

void reopen_png(image_dict * idict)
{
    integer width, height, xres, yres;
    width = img_xsize(idict);   /* do consistency check */
    height = img_ysize(idict);
    xres = img_xres(idict);
    yres = img_yres(idict);
    read_png_info(idict, IMG_KEEPOPEN);
    if (width != img_xsize(idict) || height != img_ysize(idict)
        || xres != img_xres(idict) || yres != img_yres(idict))
        pdftex_fail("writepng: image dimensions have changed");
}

static boolean last_png_needs_page_group;

void write_png(image_dict * idict)
{
    double gamma, checked_gamma;
    int i;
    integer palette_objnum = 0;
    png_structp png_p;
    png_infop info_p;
    assert(idict != NULL);
    last_png_needs_page_group = false;
    if (img_file(idict) == NULL)
        reopen_png(idict);
    assert(img_png_ptr(idict) != NULL);
    png_p = img_png_png_ptr(idict);
    info_p = img_png_info_ptr(idict);
    if (fixed_pdf_minor_version < 5)
        fixed_image_hicolor = 0;
    pdf_puts("/Type /XObject\n/Subtype /Image\n");
    if (img_attr(idict) != NULL && strlen(img_attr(idict)) > 0)
        pdf_printf("%s\n", img_attr(idict));
    pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
               (int) info_p->width,
               (int) info_p->height, (int) info_p->bit_depth);
    pdf_puts("/ColorSpace ");
    checked_gamma = 1.0;
    if (fixed_image_apply_gamma) {
        if (png_get_gAMA(png_p, info_p, &gamma)) {
            checked_gamma = (fixed_gamma / 1000.0) * gamma;
        } else {
            checked_gamma =
                (fixed_gamma / 1000.0) * (1000.0 / fixed_image_gamma);
        }
    }
    /* the switching between |info_p| and |png_p| queries has been trial and error. 
     */
    if (fixed_pdf_minor_version > 1 && info_p->interlace_type == PNG_INTERLACE_NONE && (png_p->transformations == 0 || png_p->transformations == 0x2000)        /* gamma */
        &&!(png_p->color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
            png_p->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        && (fixed_image_hicolor || (png_p->bit_depth <= 8))
        && (checked_gamma <= 1.01 && checked_gamma > 0.99)
        ) {
        if (img_colorspace(idict) != 0) {
            pdf_printf("%i 0 R\n", (int) img_colorspace(idict));
        } else {
            switch (info_p->color_type) {
            case PNG_COLOR_TYPE_PALETTE:
                pdf_create_obj(0, 0);
                palette_objnum = obj_ptr;
                pdf_printf("[/Indexed /DeviceRGB %i %i 0 R]\n",
                           (int) (info_p->num_palette - 1),
                           (int) palette_objnum);
                break;
            case PNG_COLOR_TYPE_GRAY:
                pdf_puts("/DeviceGray\n");
                break;
            default:           /* RGB */
                pdf_puts("/DeviceRGB\n");
            };
        }
        if (tracefilenames)
            tex_printf(" (PNG copy)");
        copy_png(idict);
        if (palette_objnum > 0) {
            pdf_begin_dict(palette_objnum, 0);
            pdf_begin_stream();
            for (i = 0; (unsigned) i < info_p->num_palette; i++) {
                pdfroom(3);
                pdf_buf[pdf_ptr++] = info_p->palette[i].red;
                pdf_buf[pdf_ptr++] = info_p->palette[i].green;
                pdf_buf[pdf_ptr++] = info_p->palette[i].blue;
            }
            pdf_end_stream();
        }
    } else {
        if (0) {
            tex_printf(" PNG copy skipped because: ");
            if (fixed_image_apply_gamma &&
                (checked_gamma > 1.01 || checked_gamma < 0.99))
                tex_printf("gamma delta=%lf ", checked_gamma);
            if (png_p->transformations != PNG_TRANSFORM_IDENTITY)
                tex_printf("transform=%lu", (long) png_p->transformations);
            if ((info_p->color_type != PNG_COLOR_TYPE_GRAY)
                && (info_p->color_type != PNG_COLOR_TYPE_RGB)
                && (info_p->color_type != PNG_COLOR_TYPE_PALETTE))
                tex_printf("colortype ");
            if (fixed_pdf_minor_version <= 1)
                tex_printf("version=%d ", (int) fixed_pdf_minor_version);
            if (info_p->interlace_type != PNG_INTERLACE_NONE)
                tex_printf("interlaced ");
            if (info_p->bit_depth > 8)
                tex_printf("bitdepth=%d ", info_p->bit_depth);
            if (png_get_valid(png_p, info_p, PNG_INFO_tRNS))
                tex_printf("simple transparancy ");
        }
        switch (info_p->color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            write_png_palette(idict);
            break;
        case PNG_COLOR_TYPE_GRAY:
            write_png_gray(idict);
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (fixed_pdf_minor_version >= 4) {
                write_png_gray_alpha(idict);
                last_png_needs_page_group = true;
            } else
                write_png_gray(idict);
            break;
        case PNG_COLOR_TYPE_RGB:
            write_png_rgb(idict);
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            if (fixed_pdf_minor_version >= 4) {
                write_png_rgb_alpha(idict);
                last_png_needs_page_group = true;
            } else
                write_png_rgb(idict);
            break;
        default:
            pdftex_fail("unsupported type of color_type <%i>",
                        info_p->color_type);
        }
    }
    pdf_flush();
    close_and_cleanup_png(idict);
}



static boolean transparent_page_group_was_written = false;

/* Called after the xobject generated by write_png has been finished; used to
 * write out additional objects */
void write_additional_png_objects(void)
{
    return;                     /* this interferes with current macro-based usage and cannot be configured */
    if (last_png_needs_page_group) {
        if (!transparent_page_group_was_written && transparent_page_group > 1) {
            /* create new group object */
            transparent_page_group_was_written = true;
            pdf_begin_obj(transparent_page_group, 2);
            if (get_pdf_compress_level() == 0) {
                pdf_puts("%PTEX Group needed for transparent pngs\n");
            }
            pdf_puts
                ("<</Type/Group /S/Transparency /CS/DeviceRGB /I true /K true>>\n");
            pdf_end_obj();
        }
    }
}
