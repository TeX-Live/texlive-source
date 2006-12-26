/*
Copyright (c) 1996-2004 Han The Thanh, <thanh@pdftex.org>

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

$Id: writepng.c,v 1.2 2006/01/06 22:35:43 hahe Exp hahe $
*/

#include "ptexlib.h"
#include "image.h"

void read_png_info(integer img)
{
    double gamma;
    FILE *png_file = xfopen(img_name(img), FOPEN_RBIN_MODE);

    if ((png_ptr(img) = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                               NULL, NULL, NULL)) == NULL)
        pdftex_fail("libpng: png_create_read_struct() failed");
    if ((png_info(img) = png_create_info_struct(png_ptr(img))) == NULL)
        pdftex_fail("libpng: png_create_info_struct() failed");
    if (setjmp(png_ptr(img)->jmpbuf))
        pdftex_fail("libpng: internal error");
    png_init_io(png_ptr(img), png_file);
    png_read_info(png_ptr(img), png_info(img));
    /* simple transparency support */
    if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr(img));
    }
    /* alpha channel support  */
    if (fixedpdfminorversion < 4
        && png_ptr(img)->color_type | PNG_COLOR_MASK_ALPHA)
        png_set_strip_alpha(png_ptr(img));
    /* 16bit depth support */
    if (fixedpdfminorversion < 5)
        fixedimagehicolor = 0;
    if (png_info(img)->bit_depth == 16 && !fixedimagehicolor)
        png_set_strip_16(png_ptr(img));
    /* gamma support */
    if (fixedimageapplygamma) {
        if (png_get_gAMA(png_ptr(img), png_info(img), &gamma))
            png_set_gamma(png_ptr(img), (fixedgamma / 1000.0), gamma);
        else
            png_set_gamma(png_ptr(img), (fixedgamma / 1000.0),
                          (1000.0 / fixedimagegamma));
    }
    /* reset structure */
    png_read_update_info(png_ptr(img), png_info(img));
    /* resolution support */
    img_width(img) = png_info(img)->width;
    img_height(img) = png_info(img)->height;
    if (png_info(img)->valid & PNG_INFO_pHYs) {
        img_xres(img) =
            round(0.0254 *
                  png_get_x_pixels_per_meter(png_ptr(img), png_info(img)));
        img_yres(img) =
            round(0.0254 *
                  png_get_y_pixels_per_meter(png_ptr(img), png_info(img)));
    }
    switch (png_info(img)->color_type) {
    case PNG_COLOR_TYPE_PALETTE:
        img_color(img) = IMAGE_COLOR_C | IMAGE_COLOR_I;
        break;
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        img_color(img) = IMAGE_COLOR_B;
        break;
    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_RGB_ALPHA:
        img_color(img) = IMAGE_COLOR_C;
        break;
    default:
        pdftex_fail("unsupported type of color_type <%i>",
                    png_info(img)->color_type);
    }
}


#define write_gray_pixel_16(r)                           \
  if (j % 4 == 0||j % 4 == 1)  pdfbuf[pdfptr++]  = *r++; \
  else                        smask[smask_ptr++] = *r++

#define write_gray_pixel_8(r)                   \
    if (j % 2 == 0)  pdfbuf[pdfptr++]   = *r++; \
    else  	     smask[smask_ptr++] = *r++


#define write_rgb_pixel_16(r)                                 \
    if (!(j % 8 == 6||j % 8 == 7)) pdfbuf[pdfptr++]  = *r++;  \
    else                           smask[smask_ptr++] = *r++

#define write_rgb_pixel_8(r)                                 \
    if (j % 4 != 3)      pdfbuf[pdfptr++]  = *r++;           \
    else                 smask[smask_ptr++] = *r++

#define write_simple_pixel(r)    pdfbuf[pdfptr++] = *r++


#define write_noninterlaced(outmac)                    \
  for (i = 0; i < (int)png_info(img)->height; i++) {   \
    png_read_row(png_ptr(img), row, NULL);             \
    r = row;                                           \
    k = png_info(img)->rowbytes;                       \
    while(k > 0) {                                     \
	l = (k > pdfbufsize)? pdfbufsize : k;          \
		pdfroom(l);                            \
		for (j = 0; j < l; j++) {              \
		  outmac;	                       \
		}                                      \
		k -= l;                                \
	    }                                          \
        }

#define write_interlaced(outmac)                       \
  for (i = 0; i < (int)png_info(img)->height; i++) {   \
            row = rows[i];                             \
	    k = png_info(img)->rowbytes;               \
	    while(k > 0) {                             \
		l = (k > pdfbufsize)? pdfbufsize : k;  \
		pdfroom(l);                            \
		for (j = 0; j < l; j++) {              \
		  outmac;           	               \
		}                                      \
		k -= l;                                \
	    }                                          \
            xfree(rows[i]);                            \
        }


void write_png_palette(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    integer palette_objnum = 0;
    pdfcreateobj(0, 0);
    palette_objnum = objptr;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_printf("[/Indexed /DeviceRGB %i %i 0 R]\n",
                   (int) (png_info(img)->num_palette - 1),
                   (int) palette_objnum);
    }
    pdfbeginstream();
    if (png_info(img)->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(png_info(img)->rowbytes, png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_info(img)->height * png_info(img)->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_info(img)->height, png_bytep);
        for (i = 0; (unsigned) i < png_info(img)->height; i++)
            rows[i] = xtalloc(png_info(img)->rowbytes, png_byte);
        png_read_image(png_ptr(img), rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdfendstream();
    if (palette_objnum > 0) {
        pdfbegindict(palette_objnum, 0);
        pdfbeginstream();
        for (i = 0; (unsigned) i < png_info(img)->num_palette; i++) {
            pdfroom(3);
            pdfbuf[pdfptr++] = png_info(img)->palette[i].red;
            pdfbuf[pdfptr++] = png_info(img)->palette[i].green;
            pdfbuf[pdfptr++] = png_info(img)->palette[i].blue;
        }
        pdfendstream();
    }
}

void write_png_gray(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceGray\n");
    }
    pdfbeginstream();
    if (png_info(img)->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(png_info(img)->rowbytes, png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_info(img)->height * png_info(img)->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_info(img)->height, png_bytep);
        for (i = 0; (unsigned) i < png_info(img)->height; i++)
            rows[i] = xtalloc(png_info(img)->rowbytes, png_byte);
        png_read_image(png_ptr(img), rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdfendstream();
}



void write_png_gray_alpha(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    integer smask_objnum = 0;
    png_bytep smask;
    integer smask_ptr = 0;
    integer smask_size = 0;
    int bitdepth;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceGray\n");
    }
    pdfcreateobj(0, 0);
    smask_objnum = objptr;
    pdf_printf("/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (png_info(img)->rowbytes / 2) * png_info(img)->height;
    smask = xtalloc(smask_size, png_byte);
    pdfbeginstream();
    if (png_info(img)->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(png_info(img)->rowbytes, png_byte);
        if ((png_info(img)->bit_depth == 16) && fixedimagehicolor) {
            write_noninterlaced(write_gray_pixel_16(r));
        } else {
            write_noninterlaced(write_gray_pixel_8(r));
        }
        xfree(row);
    } else {
        if (png_info(img)->height * png_info(img)->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_info(img)->height, png_bytep);
        for (i = 0; (unsigned) i < png_info(img)->height; i++)
            rows[i] = xtalloc(png_info(img)->rowbytes, png_byte);
        png_read_image(png_ptr(img), rows);
        if ((png_info(img)->bit_depth == 16) && fixedimagehicolor) {
            write_interlaced(write_gray_pixel_16(row));
        } else {
            write_interlaced(write_gray_pixel_8(row));
        }
        xfree(rows);
    }
    pdfendstream();
    pdfflush();
    /* now write the Smask object */
    if (smask_objnum > 0) {
        bitdepth = (int) png_info(img)->bit_depth;
        pdfbegindict(smask_objnum, 0);
        pdf_puts("/Type /XObject\n/Subtype /Image\n");
        pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
                   (int) png_info(img)->width,
                   (int) png_info(img)->height,
                   (bitdepth == 16 ? 8 : bitdepth));
        pdf_puts("/ColorSpace /DeviceGray\n");
        pdfbeginstream();
        for (i = 0; i < smask_size; i++) {
            if (i % 8 == 0)
                pdfroom(8);
            pdfbuf[pdfptr++] = smask[i];
            if (bitdepth == 16)
                i++;
        }
        xfree(smask);
        pdfendstream();
    }
}

void write_png_rgb(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceRGB\n");
    }
    pdfbeginstream();
    if (png_info(img)->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(png_info(img)->rowbytes, png_byte);
        write_noninterlaced(write_simple_pixel(r));
        xfree(row);
    } else {
        if (png_info(img)->height * png_info(img)->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_info(img)->height, png_bytep);
        for (i = 0; (unsigned) i < png_info(img)->height; i++)
            rows[i] = xtalloc(png_info(img)->rowbytes, png_byte);
        png_read_image(png_ptr(img), rows);
        write_interlaced(write_simple_pixel(row));
        xfree(rows);
    }
    pdfendstream();
}

void write_png_rgb_alpha(integer img)
{
    int i, j, k, l;
    png_bytep row, r, *rows;
    integer smask_objnum = 0;
    png_bytep smask;
    integer smask_ptr = 0;
    integer smask_size = 0;
    int bitdepth;
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        pdf_puts("/DeviceRGB\n");
    }
    pdfcreateobj(0, 0);
    smask_objnum = objptr;
    pdf_printf("/SMask %i 0 R\n", (int) smask_objnum);
    smask_size = (png_info(img)->rowbytes / 2) * png_info(img)->height;
    smask = xtalloc(smask_size, png_byte);
    pdfbeginstream();
    if (png_info(img)->interlace_type == PNG_INTERLACE_NONE) {
        row = xtalloc(png_info(img)->rowbytes, png_byte);
        if ((png_info(img)->bit_depth == 16) && fixedimagehicolor) {
            write_noninterlaced(write_rgb_pixel_16(r));
        } else {
            write_noninterlaced(write_rgb_pixel_8(r));
        }
        xfree(row);
    } else {
        if (png_info(img)->height * png_info(img)->rowbytes >= 10240000L)
            pdftex_warn
                ("large interlaced PNG might cause out of memory (use non-interlaced PNG to fix this)");
        rows = xtalloc(png_info(img)->height, png_bytep);
        for (i = 0; (unsigned) i < png_info(img)->height; i++)
            rows[i] = xtalloc(png_info(img)->rowbytes, png_byte);
        png_read_image(png_ptr(img), rows);
        if ((png_info(img)->bit_depth == 16) && fixedimagehicolor) {
            write_interlaced(write_rgb_pixel_16(row));
        } else {
            write_interlaced(write_rgb_pixel_8(row));
        }
        xfree(rows);
    }
    pdfendstream();
    pdfflush();
    /* now write the Smask object */
    if (smask_objnum > 0) {
        bitdepth = (int) png_info(img)->bit_depth;
        pdfbegindict(smask_objnum, 0);
        pdf_puts("/Type /XObject\n/Subtype /Image\n");
        pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
                   (int) png_info(img)->width,
                   (int) png_info(img)->height,
                   (bitdepth == 16 ? 8 : bitdepth));
        pdf_puts("/ColorSpace /DeviceGray\n");
        pdfbeginstream();
        for (i = 0; i < smask_size; i++) {
            if (i % 8 == 0)
                pdfroom(8);
            pdfbuf[pdfptr++] = smask[i];
            if (bitdepth == 16)
                i++;
        }
        xfree(smask);
        pdfendstream();
    }
}


/**********************************************************************/
/*
 *
 * The |copy_png| function is from Hartmut Henkel. The goal is to use
 * pdf's native FlateDecode support if that is possible.
 *
 * Only a subset of the png files allows this, but when possible it
 * greatly improves inclusion speed.
 *
 */

/* Code cheerfully gleaned from Thomas Merz' PDFlib, file p_png.c "SPNG - Simple PNG" */

static int spng_getint(FILE * fp)
{
    unsigned char buf[4];
    if (fread(buf, 1, 4, fp) != 4)
        pdftex_fail("writepng: reading chunk type failed");
    return ((((((int) buf[0] << 8) + buf[1]) << 8) + buf[2]) << 8) + buf[3];
}

#define SPNG_CHUNK_IDAT 0x49444154
#define SPNG_CHUNK_IEND 0x49454E44

void copy_png(integer img)
{
    FILE *fp = (FILE *) png_ptr(img)->io_ptr;
    int i, len, type, streamlength = 0;
    boolean endflag = false;
    int idat = 0;               /* flag to check continuous IDAT chunks sequence */
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
    pdf_printf("/Length %d\n", streamlength);
    pdf_printf("/Filter /FlateDecode\n");
    pdf_printf("/DecodeParms << ");
    pdf_printf("/Colors %d ", png_info(img)->color_type == 2 ? 3 : 1);
    pdf_printf("/Columns %d ", (int) png_info(img)->width);
    pdf_printf("/BitsPerComponent %i ", (int) png_info(img)->bit_depth);
    pdf_printf("/Predictor %d ", 10);   /* actual predictor defined on line basis */
    pdf_printf(">>\n>>\nstream\n");
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
                i = (len > pdfbufsize) ? pdfbufsize : len;
                pdfroom(i);
                fread(&pdfbuf[pdfptr], 1, i, fp);
                pdfptr += i;
                len -= i;
            }
            if (fseek(fp, 4, SEEK_CUR) != 0)
                pdftex_fail("writepng: fseek in PNG file failed");
            break;
        case SPNG_CHUNK_IEND:  /* done */
            pdfendstream();
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

void write_png(integer img)
{

    double gamma, checked_gamma;
    int i;
    integer palette_objnum = 0;
    if (fixedpdfminorversion < 5)
        fixedimagehicolor = 0;

    pdf_puts("/Type /XObject\n/Subtype /Image\n");
    pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n",
               (int) png_info(img)->width,
               (int) png_info(img)->height, (int) png_info(img)->bit_depth);
    pdf_puts("/ColorSpace ");
    checked_gamma = 1.0;
    if (fixedimageapplygamma) {
        if (png_get_gAMA(png_ptr(img), png_info(img), &gamma)) {
            checked_gamma = (fixedgamma / 1000.0) * gamma;
        } else {
            checked_gamma = (fixedgamma / 1000.0) * (1000.0 / fixedimagegamma);
        }
    }
    /* the switching between |png_info| and |png_ptr| queries has been trial and error.
     */
    if (fixedpdfminorversion > 1 && png_info(img)->interlace_type == PNG_INTERLACE_NONE && (png_ptr(img)->transformations == 0 || png_ptr(img)->transformations == 0x2000)      /* gamma */
        &&!(png_ptr(img)->color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
            png_ptr(img)->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        && (fixedimagehicolor || (png_ptr(img)->bit_depth <= 8))
        && (checked_gamma <= 1.01 && checked_gamma > 0.99)
        ) {
        if (img_colorspace_ref(img) != 0) {
            pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
        } else {
            switch (png_info(img)->color_type) {
            case PNG_COLOR_TYPE_PALETTE:
                pdfcreateobj(0, 0);
                palette_objnum = objptr;
                pdf_printf("[/Indexed /DeviceRGB %i %i 0 R]\n",
                           (int) (png_info(img)->num_palette - 1),
                           (int) palette_objnum);
                break;
            case PNG_COLOR_TYPE_GRAY:
                pdf_puts("/DeviceGray\n");
                break;
            default:           /* RGB */
                pdf_puts("/DeviceRGB\n");
            };
        }
        tex_printf(" (PNG copy)");
        copy_png(img);
        if (palette_objnum > 0) {
            pdfbegindict(palette_objnum, 0);
            pdfbeginstream();
            for (i = 0; i < png_info(img)->num_palette; i++) {
                pdfroom(3);
                pdfbuf[pdfptr++] = png_info(img)->palette[i].red;
                pdfbuf[pdfptr++] = png_info(img)->palette[i].green;
                pdfbuf[pdfptr++] = png_info(img)->palette[i].blue;
            }
            pdfendstream();
        }
    } else {
        if (0) {
            tex_printf(" PNG copy skipped because: ");
            if (fixedimageapplygamma &&
                (checked_gamma > 1.01 || checked_gamma < 0.99))
                tex_printf("gamma delta=%lf ", checked_gamma);
            if (png_ptr(img)->transformations != PNG_TRANSFORM_IDENTITY)
                tex_printf("transform=%lu",
                           (long) png_ptr(img)->transformations);
            if ((png_info(img)->color_type != PNG_COLOR_TYPE_GRAY)
                && (png_info(img)->color_type != PNG_COLOR_TYPE_RGB)
                && (png_info(img)->color_type != PNG_COLOR_TYPE_PALETTE))
                tex_printf("colortype ");
            if (fixedpdfminorversion <= 1)
                tex_printf("version=%d ", (int) fixedpdfminorversion);
            if (png_info(img)->interlace_type != PNG_INTERLACE_NONE)
                tex_printf("interlaced ");
            if (png_info(img)->bit_depth > 8)
                tex_printf("bitdepth=%d ", png_info(img)->bit_depth);
            if (png_get_valid(png_ptr(img), png_info(img), PNG_INFO_tRNS))
                tex_printf("simple transparancy ");
        }
        switch (png_info(img)->color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            write_png_palette(img);
            break;
        case PNG_COLOR_TYPE_GRAY:
            write_png_gray(img);
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            if (fixedpdfminorversion >= 4)
                write_png_gray_alpha(img);
            else
                write_png_gray(img);
            break;
        case PNG_COLOR_TYPE_RGB:
            write_png_rgb(img);
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            if (fixedpdfminorversion >= 4)
                write_png_rgb_alpha(img);
            else
                write_png_rgb(img);
            break;
        default:
            pdftex_fail("unsupported type of color_type <%i>",
                        png_info(img)->color_type);
        }
    }
    pdfflush();
}
