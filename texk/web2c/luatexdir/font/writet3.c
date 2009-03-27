/* writet3.c
   
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

#include "ptexlib.h"
#include <kpathsea/tex-glyph.h>
#include <kpathsea/magstep.h>
#include <string.h>
#include "luatexfont.h"

static const char _svn_version[] =
    "$Id: writet3.c 1712 2009-01-02 10:54:55Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/font/writet3.c $";

#define T3_BUF_SIZE   1024

typedef char t3_line_entry;
define_array(t3_line);

FILE *t3_file;
static boolean t3_image_used;

static integer t3_char_procs[256];
static float t3_char_widths[256];
static int t3_glyph_num;
static float t3_font_scale;
static integer t3_b0, t3_b1, t3_b2, t3_b3;
static boolean is_pk_font;

/* not static because used by pkin.c  */
unsigned char *t3_buffer = NULL;
integer t3_size = 0;
integer t3_curbyte = 0;

#define t3_check_eof()                                     \
    if (t3_eof())                                          \
        pdftex_fail("unexpected end of file");


static void update_bbox(integer llx, integer lly, integer urx, integer ury,
                        boolean is_first_glyph)
{
    if (is_first_glyph) {
        t3_b0 = llx;
        t3_b1 = lly;
        t3_b2 = urx;
        t3_b3 = ury;
    } else {
        if (llx < t3_b0)
            t3_b0 = llx;
        if (lly < t3_b1)
            t3_b1 = lly;
        if (urx > t3_b2)
            t3_b2 = urx;
        if (ury > t3_b3)
            t3_b3 = ury;
    }
}

static integer get_pk_font_scale(internalfontnumber f)
{
    return
        divide_scaled(pk_scale_factor,
                      divide_scaled(pdf_font_size(f), one_hundred_bp,
                                    fixed_decimal_digits + 2), 0);
}

static integer pk_char_width(internalfontnumber f, scaled w)
{
    return
        divide_scaled(divide_scaled(w, pdf_font_size(f), 7),
                      get_pk_font_scale(f), 0);
}

scaled get_pk_char_width(internalfontnumber f, scaled w)
{
    return (get_pk_font_scale(f) / 100000.0) *
        (pk_char_width(f, w) / 100.0) * pdf_font_size(f);
}

static boolean writepk(internalfontnumber f)
{
    kpse_glyph_file_type font_ret;
    integer llx, lly, urx, ury;
    integer cw, rw, i, j;
    halfword *row;
    char *name;
    char *ftemp = NULL;
    chardesc cd;
    boolean is_null_glyph, check_preamble;
    integer dpi;
    int callback_id = 0;
    int file_opened = 0;
    int mallocsize = 0;
    if (t3_buffer != NULL) {
        xfree(t3_buffer);
        t3_buffer = NULL;
    }
    t3_curbyte = 0;
    t3_size = 0;

    callback_id = callback_defined(find_pk_file_callback);

    if (callback_id > 0) {
        dpi = round(fixed_pk_resolution *
                    (((float) pdf_font_size(f)) / font_dsize(f)));
        /* <base>.dpi/<fontname>.<tdpi>pk */
        cur_file_name = font_name(f);
        mallocsize = strlen(cur_file_name) + 24 + 9;
        name = xmalloc(mallocsize);
        snprintf(name, mallocsize, "%ddpi/%s.%dpk", (int) fixed_pk_resolution,
                 cur_file_name, (int) dpi);
        if (run_callback(callback_id, "S->S", name, &ftemp)) {
            if (ftemp != NULL && strlen(ftemp)) {
                free(name);
                name = xstrdup(ftemp);
                free(ftemp);
            }
        }
    } else {
        dpi =
            kpse_magstep_fix(round
                             (fixed_pk_resolution *
                              (((float) pdf_font_size(f)) / font_dsize(f))),
                             fixed_pk_resolution, NULL);
        cur_file_name = font_name(f);
        name = kpse_find_pk(cur_file_name, (unsigned) dpi, &font_ret);
        if (name == NULL ||
            !FILESTRCASEEQ(cur_file_name, font_ret.name) ||
            !kpse_bitmap_tolerance((float) font_ret.dpi, (float) dpi)) {
            pdftex_fail("Font %s at %i not found", cur_file_name, (int) dpi);
        }
    }
    callback_id = callback_defined(read_pk_file_callback);
    if (callback_id > 0) {
        if (!
            (run_callback
             (callback_id, "S->bSd", name, &file_opened, &t3_buffer, &t3_size)
             && file_opened && t3_size > 0)) {
            pdftex_warn("Font %s at %i not found", cur_file_name, (int) dpi);
            cur_file_name = NULL;
            return false;
        }
    } else {
        t3_file = xfopen(name, FOPEN_RBIN_MODE);
        recorder_record_input(name);
        t3_read_file();
        t3_close();
    }
    t3_image_used = true;
    is_pk_font = true;
    if (tracefilenames)
        tex_printf(" <%s", (char *) name);
    cd.rastersize = 256;
    cd.raster = xtalloc(cd.rastersize, halfword);
    check_preamble = true;
    while (readchar(check_preamble, &cd) != 0) {
        check_preamble = false;
        if (!pdf_char_marked(f, cd.charcode))
            continue;
        t3_char_widths[cd.charcode] =
            pk_char_width(f, get_charwidth(f, cd.charcode));
        if (cd.cwidth < 1 || cd.cheight < 1) {
            cd.xescape = cd.cwidth = round(t3_char_widths[cd.charcode] / 100.0);
            cd.cheight = 1;
            cd.xoff = 0;
            cd.yoff = 0;
            is_null_glyph = true;
        } else
            is_null_glyph = false;
        llx = -cd.xoff;
        lly = cd.yoff - cd.cheight + 1;
        urx = cd.cwidth + llx + 1;
        ury = cd.cheight + lly;
        update_bbox(llx, lly, urx, ury, t3_glyph_num == 0);
        t3_glyph_num++;
        pdf_new_dict(0, 0, 0);
        t3_char_procs[cd.charcode] = obj_ptr;
        pdf_begin_stream();
        pdf_print_real(t3_char_widths[cd.charcode], 2);
        pdf_printf(" 0 %i %i %i %i d1\n",
                   (int) llx, (int) lly, (int) urx, (int) ury);
        if (is_null_glyph)
            goto end_stream;
        pdf_printf("q\n%i 0 0 %i %i %i cm\nBI\n", (int) cd.cwidth,
                   (int) cd.cheight, (int) llx, (int) lly);
        pdf_printf("/W %i\n/H %i\n", (int) cd.cwidth, (int) cd.cheight);
        pdf_puts("/IM true\n/BPC 1\n/D [1 0]\nID ");
        cw = (cd.cwidth + 7) / 8;
        rw = (cd.cwidth + 15) / 16;
        row = cd.raster;
        for (i = 0; i < cd.cheight; i++) {
            for (j = 0; j < rw - 1; j++) {
                pdfout(*row / 256);
                pdfout(*row % 256);
                row++;
            }
            pdfout(*row / 256);
            if (2 * rw == cw)
                pdfout(*row % 256);
            row++;
        }
        pdf_puts("\nEI\nQ\n");
      end_stream:
        pdf_end_stream();
    }
    xfree(cd.raster);
    cur_file_name = NULL;
    return true;
}

void writet3(int objnum, internalfontnumber f)
{

    int i;
    integer wptr, eptr, cptr;
    int first_char, last_char;
    integer pk_font_scale;
    boolean is_notdef;


    t3_glyph_num = 0;
    t3_image_used = false;
    for (i = 0; i < 256; i++) {
        t3_char_procs[i] = 0;
        t3_char_widths[i] = 0;
    }
    pack_file_name(tex_font_name(f), get_nullstr(), maketexlstring(".pgc", 4));
    cur_file_name = makecstring(make_name_string());
    is_pk_font = false;

    if (t3_buffer != NULL) {
        xfree(t3_buffer);
        t3_buffer = NULL;
    }
    t3_curbyte = 0;
    t3_size = 0;
    if (!writepk(f)) {
        cur_file_name = NULL;
        return;
    }
    for (i = font_bc(f); i <= font_ec(f); i++)
        if (pdf_char_marked(f, i))
            break;
    first_char = i;
    for (i = font_ec(f); i > first_char; i--)
        if (pdf_char_marked(f, i))
            break;
    last_char = i;
    pdf_begin_dict(objnum, 1);  /* Type 3 font dictionary */
    pdf_puts("/Type /Font\n/Subtype /Type3\n");
    pdf_printf("/Name /F%i\n", (int) f);
    if (pdf_font_attr(f) != get_nullstr()) {
        pdf_print(pdf_font_attr(f));
        pdf_puts("\n");
    }
    if (is_pk_font) {
        pk_font_scale = get_pk_font_scale(f);
        pdf_puts("/FontMatrix [");
        pdf_print_real(pk_font_scale, 5);
        pdf_puts(" 0 0 ");
        pdf_print_real(pk_font_scale, 5);
        pdf_puts(" 0 0]\n");
    } else
        pdf_printf("/FontMatrix [%g 0 0 %g 0 0]\n",
                   (double) t3_font_scale, (double) t3_font_scale);
    pdf_printf("/%s [ %i %i %i %i ]\n",
               font_key[FONTBBOX1_CODE].pdfname,
               (int) t3_b0, (int) t3_b1, (int) t3_b2, (int) t3_b3);
    pdf_printf("/Resources << /ProcSet [ /PDF %s] >>\n",
               t3_image_used ? "/ImageB " : "");
    pdf_printf("/FirstChar %i\n/LastChar %i\n", first_char, last_char);
    wptr = pdf_new_objnum();
    eptr = pdf_new_objnum();
    cptr = pdf_new_objnum();
    pdf_printf("/Widths %i 0 R\n/Encoding %i 0 R\n/CharProcs %i 0 R\n",
               (int) wptr, (int) eptr, (int) cptr);
    pdf_end_dict();
    pdf_begin_obj(wptr, 1);     /* chars width array */
    pdf_puts("[");
    if (is_pk_font)
        for (i = first_char; i <= last_char; i++) {
            pdf_print_real(t3_char_widths[i], 2);
            pdf_puts(" ");
    } else
        for (i = first_char; i <= last_char; i++)
            pdf_printf("%i ", (int) t3_char_widths[i]);
    pdf_puts("]\n");
    pdf_end_obj();
    pdf_begin_dict(eptr, 1);    /* encoding dictionary */
    pdf_printf("/Type /Encoding\n/Differences [%i", first_char);
    if (t3_char_procs[first_char] == 0) {
        pdf_printf("/%s", notdef);
        is_notdef = true;
    } else {
        pdf_printf("/a%i", first_char);
        is_notdef = false;
    }
    for (i = first_char + 1; i <= last_char; i++) {
        if (t3_char_procs[i] == 0) {
            if (!is_notdef) {
                pdf_printf(" %i/%s", i, notdef);
                is_notdef = true;
            }
        } else {
            if (is_notdef) {
                pdf_printf(" %i", i);
                is_notdef = false;
            }
            pdf_printf("/a%i", i);
        }
    }
    pdf_puts("]\n");
    pdf_end_dict();
    pdf_begin_dict(cptr, 1);    /* CharProcs dictionary */
    for (i = first_char; i <= last_char; i++)
        if (t3_char_procs[i] != 0)
            pdf_printf("/a%i %i 0 R\n", (int) i, (int) t3_char_procs[i]);
    pdf_end_dict();
    if (tracefilenames)
        tex_printf(">");
    cur_file_name = NULL;
}
