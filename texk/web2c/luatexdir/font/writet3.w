% writet3.w

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
    "$Id: writet3.w 3882 2010-09-13 22:29:18Z hhenkel $ "
"$URL: http://foundry.supelec.fr/svn/luatex/tags/beta-0.66.0/source/texk/web2c/luatexdir/font/writet3.w $";

#include "ptexlib.h"
#include <kpathsea/tex-glyph.h>
#include <kpathsea/magstep.h>
#include <string.h>

#define T3_BUF_SIZE   1024

typedef char t3_line_entry;
define_array(t3_line);

FILE *t3_file;
static boolean t3_image_used;

static int t3_char_procs[256];
static float t3_char_widths[256];
static int t3_glyph_num;
static float t3_font_scale;
static int t3_b0, t3_b1, t3_b2, t3_b3;
static boolean is_pk_font;

/* not static because used by pkin.c  */
unsigned char *t3_buffer = NULL;
int t3_size = 0;
int t3_curbyte = 0;

#define t3_check_eof()                                     \
    if (t3_eof())                                          \
        pdftex_fail("unexpected end of file");

@
@c
static void update_bbox(int llx, int lly, int urx, int ury,
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

static int get_pk_font_scale(internal_font_number f, int precision,
                             int scale_factor)
{
    return
        divide_scaled(scale_factor,
                      divide_scaled(font_size(f), one_hundred_bp,
                                    precision + 2), 0);
}

static int pk_char_width(internal_font_number f, scaled w, int precision,
                         int scale_factor)
{
    return
        divide_scaled(divide_scaled(w, font_size(f), 7),
                      get_pk_font_scale(f, precision, scale_factor), 0);
}

@
@c
static boolean writepk(PDF pdf, internal_font_number f)
{
    kpse_glyph_file_type font_ret;
    int llx, lly, urx, ury;
    int cw, rw, i, j;
    halfword *row;
    char *name;
    char *ftemp = NULL;
    chardesc cd;
    boolean is_null_glyph, check_preamble;
    int dpi;
    int callback_id = 0;
    int file_opened = 0;
    unsigned mallocsize = 0;
    xfree(t3_buffer);
    t3_curbyte = 0;
    t3_size = 0;

    callback_id = callback_defined(find_pk_file_callback);

    if (callback_id > 0) {
        dpi = round((float) pdf->pk_resolution *
                    (((float) font_size(f)) / (float) font_dsize(f)));
        /* <base>.dpi/<fontname>.<tdpi>pk */
        cur_file_name = font_name(f);
        mallocsize = (unsigned) (strlen(cur_file_name) + 24 + 9);
        name = xmalloc(mallocsize);
        snprintf(name, (size_t) mallocsize, "%ddpi/%s.%dpk",
                 (int) pdf->pk_resolution, cur_file_name, (int) dpi);
        if (run_callback(callback_id, "S->S", name, &ftemp)) {
            if (ftemp != NULL && strlen(ftemp)) {
                free(name);
                name = xstrdup(ftemp);
                free(ftemp);
            }
        }
    } else {
        dpi = (int)
            kpse_magstep_fix((unsigned) round
                             ((float) pdf->pk_resolution *
                              ((float) font_size(f) / (float) font_dsize(f))),
                             (unsigned) pdf->pk_resolution, NULL);
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
    cd.raster = xtalloc((unsigned long) cd.rastersize, halfword);
    check_preamble = true;
    while (readchar(check_preamble, &cd) != 0) {
        check_preamble = false;
        if (!pdf_char_marked(f, cd.charcode))
            continue;
        t3_char_widths[cd.charcode] = (float)
            pk_char_width(f, get_charwidth(f, cd.charcode),
                          pdf->decimal_digits, pdf->pk_scale_factor);
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
        t3_char_procs[cd.charcode] = pdf_new_dict(pdf, obj_type_others, 0, 0);
        pdf_begin_stream(pdf);
        pdf_print_real(pdf, (int) t3_char_widths[cd.charcode], 2);
        pdf_printf(pdf, " 0 %i %i %i %i d1\n",
                   (int) llx, (int) lly, (int) urx, (int) ury);
        if (is_null_glyph)
            goto end_stream;
        pdf_printf(pdf, "q\n%i 0 0 %i %i %i cm\nBI\n", (int) cd.cwidth,
                   (int) cd.cheight, (int) llx, (int) lly);
        pdf_printf(pdf, "/W %i\n/H %i\n", (int) cd.cwidth, (int) cd.cheight);
        pdf_puts(pdf, "/IM true\n/BPC 1\n/D [1 0]\nID ");
        cw = (cd.cwidth + 7) / 8;
        rw = (cd.cwidth + 15) / 16;
        row = cd.raster;
        for (i = 0; i < cd.cheight; i++) {
            for (j = 0; j < rw - 1; j++) {
                pdf_out(pdf, (unsigned char) (*row / 256));
                pdf_out(pdf, (unsigned char) (*row % 256));
                row++;
            }
            pdf_out(pdf, (unsigned char) (*row / 256));
            if (2 * rw == cw)
                pdf_out(pdf, (unsigned char) (*row % 256));
            row++;
        }
        pdf_puts(pdf, "\nEI\nQ\n");
      end_stream:
        pdf_end_stream(pdf);
    }
    xfree(cd.raster);
    cur_file_name = NULL;
    return true;
}

@
@c
void writet3(PDF pdf, internal_font_number f)
{

    int i;
    int wptr, eptr, cptr;
    int first_char, last_char;
    int pk_font_scale;
    boolean is_notdef;

    t3_glyph_num = 0;
    t3_image_used = false;
    for (i = 0; i < 256; i++) {
        t3_char_procs[i] = 0;
        t3_char_widths[i] = 0;
    }
    is_pk_font = false;

    xfree(t3_buffer);
    t3_curbyte = 0;
    t3_size = 0;
    if (!writepk(pdf, f))
        return;
    for (i = font_bc(f); i <= font_ec(f); i++)
        if (pdf_char_marked(f, i))
            break;
    first_char = i;
    for (i = font_ec(f); i > first_char; i--)
        if (pdf_char_marked(f, i))
            break;
    last_char = i;
    pdf_begin_dict(pdf, pdf_font_num(f), 1);    /* Type 3 font dictionary */
    pdf_puts(pdf, "/Type /Font\n/Subtype /Type3\n");
    pdf_printf(pdf, "/Name /F%i\n", (int) f);
    if (pdf_font_attr(f) != get_nullstr() && pdf_font_attr(f) != 0) {
        pdf_print(pdf, pdf_font_attr(f));
        pdf_puts(pdf, "\n");
    }
    if (is_pk_font) {
        pk_font_scale =
            get_pk_font_scale(f, pdf->decimal_digits, pdf->pk_scale_factor);
        pdf_puts(pdf, "/FontMatrix [");
        pdf_print_real(pdf, pk_font_scale, 5);
        pdf_puts(pdf, " 0 0 ");
        pdf_print_real(pdf, pk_font_scale, 5);
        pdf_puts(pdf, " 0 0]\n");
    } else
        pdf_printf(pdf, "/FontMatrix [%g 0 0 %g 0 0]\n",
                   (double) t3_font_scale, (double) t3_font_scale);
    pdf_printf(pdf, "/%s [ %i %i %i %i ]\n",
               font_key[FONTBBOX1_CODE].pdfname,
               (int) t3_b0, (int) t3_b1, (int) t3_b2, (int) t3_b3);
    pdf_printf(pdf, "/Resources << /ProcSet [ /PDF %s] >>\n",
               t3_image_used ? "/ImageB " : "");
    pdf_printf(pdf, "/FirstChar %i\n/LastChar %i\n", first_char, last_char);
    wptr = pdf_new_objnum(pdf);
    eptr = pdf_new_objnum(pdf);
    cptr = pdf_new_objnum(pdf);
    pdf_printf(pdf, "/Widths %i 0 R\n/Encoding %i 0 R\n/CharProcs %i 0 R\n",
               (int) wptr, (int) eptr, (int) cptr);
    pdf_end_dict(pdf);
    pdf_begin_obj(pdf, wptr, 1);        /* chars width array */
    pdf_puts(pdf, "[");
    if (is_pk_font)
        for (i = first_char; i <= last_char; i++) {
            pdf_print_real(pdf, (int) t3_char_widths[i], 2);
            pdf_puts(pdf, " ");
    } else
        for (i = first_char; i <= last_char; i++)
            pdf_printf(pdf, "%i ", (int) t3_char_widths[i]);
    pdf_puts(pdf, "]\n");
    pdf_end_obj(pdf);
    pdf_begin_dict(pdf, eptr, 1);       /* encoding dictionary */
    pdf_printf(pdf, "/Type /Encoding\n/Differences [%i", first_char);
    if (t3_char_procs[first_char] == 0) {
        pdf_printf(pdf, "/%s", notdef);
        is_notdef = true;
    } else {
        pdf_printf(pdf, "/a%i", first_char);
        is_notdef = false;
    }
    for (i = first_char + 1; i <= last_char; i++) {
        if (t3_char_procs[i] == 0) {
            if (!is_notdef) {
                pdf_printf(pdf, " %i/%s", i, notdef);
                is_notdef = true;
            }
        } else {
            if (is_notdef) {
                pdf_printf(pdf, " %i", i);
                is_notdef = false;
            }
            pdf_printf(pdf, "/a%i", i);
        }
    }
    pdf_puts(pdf, "]\n");
    pdf_end_dict(pdf);
    pdf_begin_dict(pdf, cptr, 1);       /* CharProcs dictionary */
    for (i = first_char; i <= last_char; i++)
        if (t3_char_procs[i] != 0)
            pdf_printf(pdf, "/a%i %i 0 R\n", (int) i, (int) t3_char_procs[i]);
    pdf_end_dict(pdf);
    if (tracefilenames)
        tex_printf(">");
    cur_file_name = NULL;
}
