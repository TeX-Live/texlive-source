/*
Copyright 1996-2014 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ptexlib.h"
#include "image.h"

#define JPG_GRAY  1             /* Gray color space, use /DeviceGray  */
#define JPG_RGB   3             /* RGB color space, use /DeviceRGB    */
#define JPG_CMYK  4             /* CMYK color space, use /DeviceCMYK  */

typedef enum {                  /* JPEG marker codes                    */
    M_SOF0 = 0xc0,              /* baseline DCT                         */
    M_SOF1 = 0xc1,              /* extended sequential DCT              */
    M_SOF2 = 0xc2,              /* progressive DCT                      */
    M_SOF3 = 0xc3,              /* lossless (sequential)                */

    M_SOF5 = 0xc5,              /* differential sequential DCT          */
    M_SOF6 = 0xc6,              /* differential progressive DCT         */
    M_SOF7 = 0xc7,              /* differential lossless                */

    M_JPG = 0xc8,               /* JPEG extensions                      */
    M_SOF9 = 0xc9,              /* extended sequential DCT              */
    M_SOF10 = 0xca,             /* progressive DCT                      */
    M_SOF11 = 0xcb,             /* lossless (sequential)                */

    M_SOF13 = 0xcd,             /* differential sequential DCT          */
    M_SOF14 = 0xce,             /* differential progressive DCT         */
    M_SOF15 = 0xcf,             /* differential lossless                */

    M_DHT = 0xc4,               /* define Huffman tables                */

    M_DAC = 0xcc,               /* define arithmetic conditioning table */

    M_RST0 = 0xd0,              /* restart                              */
    M_RST1 = 0xd1,              /* restart                              */
    M_RST2 = 0xd2,              /* restart                              */
    M_RST3 = 0xd3,              /* restart                              */
    M_RST4 = 0xd4,              /* restart                              */
    M_RST5 = 0xd5,              /* restart                              */
    M_RST6 = 0xd6,              /* restart                              */
    M_RST7 = 0xd7,              /* restart                              */

    M_SOI = 0xd8,               /* start of image                       */
    M_EOI = 0xd9,               /* end of image                         */
    M_SOS = 0xda,               /* start of scan                        */
    M_DQT = 0xdb,               /* define quantization tables           */
    M_DNL = 0xdc,               /* define number of lines               */
    M_DRI = 0xdd,               /* define restart interval              */
    M_DHP = 0xde,               /* define hierarchical progression      */
    M_EXP = 0xdf,               /* expand reference image(s)            */

    M_APP0 = 0xe0,              /* application marker, used for JFIF    */
    M_APP1 = 0xe1,              /* application marker                   */
    M_APP2 = 0xe2,              /* application marker                   */
    M_APP3 = 0xe3,              /* application marker                   */
    M_APP4 = 0xe4,              /* application marker                   */
    M_APP5 = 0xe5,              /* application marker                   */
    M_APP6 = 0xe6,              /* application marker                   */
    M_APP7 = 0xe7,              /* application marker                   */
    M_APP8 = 0xe8,              /* application marker                   */
    M_APP9 = 0xe9,              /* application marker                   */
    M_APP10 = 0xea,             /* application marker                   */
    M_APP11 = 0xeb,             /* application marker                   */
    M_APP12 = 0xec,             /* application marker                   */
    M_APP13 = 0xed,             /* application marker                   */
    M_APP14 = 0xee,             /* application marker, used by Adobe    */
    M_APP15 = 0xef,             /* application marker                   */

    M_JPG0 = 0xf0,              /* reserved for JPEG extensions         */
    M_JPG13 = 0xfd,             /* reserved for JPEG extensions         */
    M_COM = 0xfe,               /* comment                              */

    M_TEM = 0x01,               /* temporary use                        */

    M_ERROR = 0x100             /* dummy marker, internal use only      */
} JPEG_MARKER;

static JPG_UINT16 read2bytes(FILE * f)
{
    int c = xgetc(f);
    return (c << 8) + xgetc(f);
}

void read_jpg_info(integer img)
{
    int i, units = 0;
    unsigned char jpg_id[] = "JFIF";
    img_xres(img) = img_yres(img) = 0;
    jpg_ptr(img)->file = xfopen(img_name(img), FOPEN_RBIN_MODE);
    /* no LFS needed, as JPEG is limited to <4GiB */
    xfseek(jpg_ptr(img)->file, 0, SEEK_END, cur_file_name);
    jpg_ptr(img)->length = xftell(jpg_ptr(img)->file, cur_file_name);
    xfseek(jpg_ptr(img)->file, 0, SEEK_SET, cur_file_name);
    if (read2bytes(jpg_ptr(img)->file) != 0xFFD8)
        pdftex_fail("reading JPEG image failed (no JPEG header found)");
    /* currently only true JFIF files allow extracting img_xres and img_yres */
    if (read2bytes(jpg_ptr(img)->file) == 0xFFE0) {     /* check for JFIF */
        (void) read2bytes(jpg_ptr(img)->file);
        for (i = 0; i < 5; i++) {
            if (xgetc(jpg_ptr(img)->file) != jpg_id[i])
                break;
        }
        if (i == 5) {           /* it's JFIF */
            read2bytes(jpg_ptr(img)->file);
            units = xgetc(jpg_ptr(img)->file);
            img_xres(img) = read2bytes(jpg_ptr(img)->file);
            img_yres(img) = read2bytes(jpg_ptr(img)->file);
            switch (units) {
            case 1:
                break;          /* pixels per inch */
            case 2:
                img_xres(img) *= 2.54;
                img_yres(img) *= 2.54;
                break;          /* pixels per cm */
            default:
                img_xres(img) = img_yres(img) = 0;
                break;
            }
        }
        /* if either xres or yres is 0 but the other isn't, set it to the value of the other */
        if ((img_xres(img) == 0) && (img_yres(img) != 0)) {
            img_xres(img) = img_yres(img);
        }
        if ((img_yres(img) == 0) && (img_xres(img) != 0)) {
            img_yres(img) = img_xres(img);
        }
    }
    xfseek(jpg_ptr(img)->file, 0, SEEK_SET, cur_file_name);
    while (1) {
        if (feof(jpg_ptr(img)->file))
            pdftex_fail("reading JPEG image failed (premature file end)");
        if (fgetc(jpg_ptr(img)->file) != 0xFF)
            pdftex_fail("reading JPEG image failed (no marker found)");
        switch (xgetc(jpg_ptr(img)->file)) {
        case M_SOF5:
        case M_SOF6:
        case M_SOF7:
        case M_SOF9:
        case M_SOF10:
        case M_SOF11:
        case M_SOF13:
        case M_SOF14:
        case M_SOF15:
            pdftex_fail("unsupported type of compression");
        case M_SOF2:
            if (fixedpdfminorversion <= 2)
                pdftex_fail("cannot use progressive DCT with PDF-1.2");
        case M_SOF0:
        case M_SOF1:
        case M_SOF3:
            (void) read2bytes(jpg_ptr(img)->file);      /* read segment length  */
            jpg_ptr(img)->bits_per_component = xgetc(jpg_ptr(img)->file);
            img_height(img) = read2bytes(jpg_ptr(img)->file);
            img_width(img) = read2bytes(jpg_ptr(img)->file);
            jpg_ptr(img)->color_space = xgetc(jpg_ptr(img)->file);
            xfseek(jpg_ptr(img)->file, 0, SEEK_SET, cur_file_name);
            switch (jpg_ptr(img)->color_space) {
            case JPG_GRAY:
                img_color(img) = IMAGE_COLOR_B;
                break;
            case JPG_RGB:
                img_color(img) = IMAGE_COLOR_C;
                break;
            case JPG_CMYK:
                img_color(img) = IMAGE_COLOR_C;
                break;
            default:
                pdftex_fail("Unsupported color space %i",
                            (int) jpg_ptr(img)->color_space);
            }
            return;
        case M_SOI:            /* ignore markers without parameters */
        case M_EOI:
        case M_TEM:
        case M_RST0:
        case M_RST1:
        case M_RST2:
        case M_RST3:
        case M_RST4:
        case M_RST5:
        case M_RST6:
        case M_RST7:
            break;
        default:               /* skip variable length markers */
            xfseek(jpg_ptr(img)->file, read2bytes(jpg_ptr(img)->file) - 2,
                   SEEK_CUR, cur_file_name);
            break;
        }
    }
}

void write_jpg(integer img)
{
    long unsigned l;
    FILE *f;
    pdf_puts("/Type /XObject\n/Subtype /Image\n");
    pdf_printf("/Width %i\n/Height %i\n/BitsPerComponent %i\n/Length %i\n",
               (int) img_width(img),
               (int) img_height(img),
               (int) jpg_ptr(img)->bits_per_component,
               (int) jpg_ptr(img)->length);
    pdf_puts("/ColorSpace ");
    if (img_colorspace_ref(img) != 0) {
        pdf_printf("%i 0 R\n", (int) img_colorspace_ref(img));
    } else {
        switch (jpg_ptr(img)->color_space) {
        case JPG_GRAY:
            pdf_puts("/DeviceGray\n");
            break;
        case JPG_RGB:
            pdf_puts("/DeviceRGB\n");
            break;
        case JPG_CMYK:
            pdf_puts("/DeviceCMYK\n/Decode [1 0 1 0 1 0 1 0]\n");
            break;
        default:
            pdftex_fail("Unsupported color space %i",
                        (int) jpg_ptr(img)->color_space);
        }
    }
    pdf_puts("/Filter /DCTDecode\n>>\nstream\n");
    for (l = jpg_ptr(img)->length, f = jpg_ptr(img)->file; l > 0; l--)
        pdfout(xgetc(f));
    pdfendstream();
}
