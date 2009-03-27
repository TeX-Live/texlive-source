/* pdfpage.c
   
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

/* $Id: pdfpage.c 2046 2009-03-17 20:06:05Z hhenkel $ */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "ptexlib.h"

static const char __svn_version[] =
    "$Id: pdfpage.c 2046 2009-03-17 20:06:05Z hhenkel $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/pdf/pdfpage.c $";

#define lround(a) (long) floor((a) + 0.5)
#define setpdffloat(a,b,c) {(a).m = (b); (a).e = (c);}
#define pdf2double(a) ((double) (a).m / exp10[(a).e])

/* eternal constants */
#define one_bp ((double) 65536 * (double) 72.27 / 72)   /* number of sp per 1bp */
#define e_tj 3                  /* must be 3; movements in []TJ are in fontsize/10^3 units */

/* definitions from luatex.web */
#define pdf_font_blink(a) font_tables[a]->_pdf_font_blink

pdfstructure *pstruct = NULL;

static long *exp10 = NULL;

/**********************************************************************/

static pdfstructure *new_pdfstructure()
{
    return xmalloc(sizeof(pdfstructure));
}

static void calc_k1(pdfstructure * p)
{
    p->k1 = exp10[p->pdf.h.e] / one_bp;
}

static void calc_k2(pdfstructure * p)
{
    p->tm[0].m =
        lround(pdf2double(p->hz) * pdf2double(p->ext) * exp10[p->tm[0].e]);
    p->k2 =
        exp10[e_tj +
              p->cw.e] / (exp10[p->pdf.h.e] * pdf2double(p->fs) *
                          pdf2double(p->tm[0]));
}

void pdf_page_init()
{
    pdfstructure *p;
    int i, decimal_digits = fixed_decimal_digits;
    if (exp10 == NULL) {
        exp10 = xmalloc(10 * sizeof(long));
        exp10[0] = 1;
        for (i = 1; i < 10; i++)
            exp10[i] = 10 * exp10[i - 1];
    }
    if (pstruct == NULL)
        pstruct = new_pdfstructure();
    p = pstruct;
    setpdffloat(p->pdf.h, 0, decimal_digits);
    setpdffloat(p->pdf.v, 0, decimal_digits);
    p->cw.e = 1;
    p->fs.e = decimal_digits + 2;       /* "+ 2" makes less corrections inside []TJ */
    setpdffloat(p->hz, 1000, 3);        /* m = 1000 = default = unexpanded, e must be 3 */
    setpdffloat(p->ext, 1000, 3);       /* m = 1000 = default = unextended, e must be 3 */
    /* for placement outside BT...ET */
    setpdffloat(p->cm[0], 1, 0);
    setpdffloat(p->cm[1], 0, 0);
    setpdffloat(p->cm[2], 0, 0);
    setpdffloat(p->cm[3], 1, 0);
    setpdffloat(p->cm[4], 0, decimal_digits);   /* horizontal movement on page */
    setpdffloat(p->cm[5], 0, decimal_digits);   /* vertical movement on page */
    /* for placement inside BT...ET */
    setpdffloat(p->tm[0], exp10[6], 6); /* mantissa holds HZ expand * ExtendFont */
    setpdffloat(p->tm[1], 0, 0);
    setpdffloat(p->tm[2], 0, 3);        /* mantissa holds SlantFont, 0 = default */
    setpdffloat(p->tm[3], 1, 0);
    setpdffloat(p->tm[4], 0, decimal_digits);   /* mantissa holds delta from pdf_bt_pos.h */
    setpdffloat(p->tm[5], 0, decimal_digits);   /* mantissa holds delta from pdf_bt_pos.v */
    /*  */
    p->f_cur = null_font;
    p->f_pdf = null_font;
    p->wmode = WMODE_H;
    p->mode = PMODE_PAGE;
    calc_k1(p);
}

/**********************************************************************/

#ifdef SYNCH_POS_WITH_CUR
/* some time this will be needed */
synch_pos_with_cur(scaledpos * pos, scaledpos * cur, scaledpos * box_pos)
{
    switch (dvi_direction) {
    case dir_TL_:
        pos->h = box_pos->h + cur->h;
        pos->v = box_pos->v - cur->v;
        break;
    case dir_TR_:
        pos->h = box_pos->h - cur->h;
        pos->v = box_pos->v - cur->v;
        break;
    case dir_BL_:
        pos->h = box_pos->h + cur->h;
        pos->v = box_pos->v + cur->v;
        break;
    case dir_BR_:
        pos->h = box_pos->h - cur->h;
        pos->v = box_pos->v + cur->v;
        break;
    case dir_LT_:
        pos->h = box_pos->h + cur->v;
        pos->v = box_pos->v - cur->h;
        break;
    case dir_RT_:
        pos->h = box_pos->h - cur->v;
        pos->v = box_pos->v - cur->h;
        break;
    case dir_LB_:
        pos->h = box_pos->h + cur->v;
        pos->v = box_pos->v + cur->h;
        break;
    case dir_RB_:
        pos->h = box_pos->h - cur->v;
        pos->v = box_pos->v + cur->h;
        break;
    default:;
    }
}
#endif

/**********************************************************************/

boolean calc_pdfpos(pdfstructure * p, scaledpos * pos)
{
    scaledpos new;
    int move_pdfpos = FALSE;
    switch (p->mode) {
    case PMODE_PAGE:
        new.h = lround(pos->h * p->k1);
        new.v = lround(pos->v * p->k1);
        p->cm[4].m = new.h - p->pdf.h.m;        /* cm is concatenated */
        p->cm[5].m = new.v - p->pdf.v.m;
        if (abs(p->cm[4].m) >= 1 || abs(p->cm[5].m) >= 1)
            move_pdfpos = TRUE;
        break;
    case PMODE_TEXT:
        new.h = lround(pos->h * p->k1);
        new.v = lround(pos->v * p->k1);
        p->tm[4].m = new.h - p->pdf_bt_pos.h.m; /* Tm replaces */
        p->tm[5].m = new.v - p->pdf_bt_pos.v.m;
        if (abs(p->tm[4].m) >= 1 || abs(p->tm[5].m) >= 1)
            move_pdfpos = TRUE;
        break;
    case PMODE_CHAR:
    case PMODE_CHARARRAY:
        switch (p->wmode) {
        case WMODE_H:
            new.h = lround((pos->h * p->k1 - p->pdf_tj_pos.h.m) * p->k2);
            new.v = lround(pos->v * p->k1);
            p->tj_delta.m =
                -lround((new.h - p->cw.m) / exp10[p->cw.e - p->tj_delta.e]);
            p->tm[5].m = new.v - p->pdf.v.m;    /* p->tm[4] is meaningless */
            if (abs(p->tj_delta.m) >= 1 || abs(p->tm[5].m) >= 1)
                move_pdfpos = TRUE;
            break;
        case WMODE_V:
            new.h = lround(pos->h * p->k1);
            new.v = lround((p->pdf_tj_pos.v.m - pos->v * p->k1) * p->k2);
            p->tm[4].m = new.h - p->pdf.h.m;    /* p->tm[5] is meaningless */
            p->tj_delta.m =
                -lround((new.v - p->cw.m) / exp10[p->cw.e - p->tj_delta.e]);
            if (abs(p->tj_delta.m) >= 1 || abs(p->tm[4].m) >= 1)
                move_pdfpos = TRUE;
            break;
        default:
            assert(0);
        }
        break;
    default:
        assert(0);
    }
    return move_pdfpos;
}

/**********************************************************************/

void print_pdffloat(pdffloat * f)
{
    char a[24];
    int e = f->e, i, j;
    long l, m = f->m;
    if (m < 0) {
        pdf_printf("-");
        m *= -1;
    }
    l = m / exp10[e];
    pdf_print_int(l);
    l = m % exp10[e];
    if (l != 0) {
        pdf_printf(".");
        j = snprintf(a, 23, "%ld", l + exp10[e]);
        assert(j < 23);
        for (i = e; i > 0; i--) {
            if (a[i] != '0')
                break;
            a[i] = '\0';
        }
        pdf_printf("%s", a + 1);
    }
}

static void print_pdf_matrix(pdffloat * tm)
{
    int i;
    for (i = 0; i < 5; i++) {
        print_pdffloat(tm + i);
        pdf_printf(" ");
    }
    print_pdffloat(tm + i);
}

void pdf_print_cm(pdffloat * cm)
{
    print_pdf_matrix(cm);
    pdf_printf(" cm\n");
}

static void pdf_print_tm(pdffloat * tm)
{
    print_pdf_matrix(tm);
    pdf_printf(" Tm ");
}

static void set_pos(pdfstructure * p, scaledpos * pos)
{
    boolean move;
    assert(is_pagemode(p));
    move = calc_pdfpos(p, pos);
    if (move == TRUE) {
        pdf_print_cm(p->cm);
        p->pdf.h.m += p->cm[4].m;
        p->pdf.v.m += p->cm[5].m;
    }
}

static void set_pos_temp(pdfstructure * p, scaledpos * pos)
{
    boolean move;
    assert(is_pagemode(p));
    move = calc_pdfpos(p, pos);
    if (move == TRUE)
        pdf_print_cm(p->cm);
}

void pdf_set_pos(scaled h, scaled v)
{
    scaledpos pos;
    pos.h = h;
    pos.v = v;
    set_pos(pstruct, &pos);
}

void pdf_set_pos_temp(scaled h, scaled v)
{
    scaledpos pos;
    pos.h = h;
    pos.v = v;
    set_pos_temp(pstruct, &pos);
}

/**********************************************************************/

static long pdf_char_width(pdfstructure * p, internal_font_number f, int i)
{
    /* use exactly this formula also for calculating the /Width array values */
    return
        lround((double) char_width(f, i) / font_size(f) *
               exp10[e_tj + p->cw.e]);
}

void pdf_print_charwidth(internal_font_number f, int i)
{
    pdffloat cw;
    assert(pdf_font_blink(f) == null_font);     /* must use unexpanded font! */
    cw.m = pdf_char_width(pstruct, f, i);
    cw.e = pstruct->cw.e;
    print_pdffloat(&cw);
}

/**********************************************************************/

static void begin_text(pdfstructure * p)
{
    assert(is_pagemode(p));
    p->pdf_bt_pos = p->pdf;
    pdf_printf("BT\n");
    p->mode = PMODE_TEXT;
}

static void end_text(pdfstructure * p)
{
    assert(is_textmode(p));
    pdf_printf("ET\n");
    p->pdf = p->pdf_bt_pos;
    p->mode = PMODE_PAGE;
}

static void begin_charmode(pdfstructure * p)
{
    assert(is_chararraymode(p));
    pdf_printf("(");
    p->mode = PMODE_CHAR;
}

static void end_charmode(pdfstructure * p)
{
    assert(is_charmode(p));
    pdf_printf(")");
    p->mode = PMODE_CHARARRAY;
}

static void begin_chararray(pdfstructure * p)
{
    assert(is_textmode(p));
    p->pdf_tj_pos = p->pdf;
    p->cw.m = 0;
    pdf_printf("[");
    p->mode = PMODE_CHARARRAY;
}

static void end_chararray(pdfstructure * p)
{
    assert(is_chararraymode(p));
    pdf_printf("]TJ\n");
    p->pdf = p->pdf_tj_pos;
    p->mode = PMODE_TEXT;
}

void pdf_end_string_nl()
{
    if (is_charmode(pstruct))
        end_charmode(pstruct);
    if (is_chararraymode(pstruct))
        end_chararray(pstruct);
}

/**********************************************************************/

static void goto_pagemode(pdfstructure * p)
{
    if (!is_pagemode(p)) {
        if (is_charmode(p))
            end_charmode(p);
        if (is_chararraymode(p))
            end_chararray(p);
        if (is_textmode(p))
            end_text(p);
        assert(is_pagemode(p));
    }
}

void pdf_goto_pagemode()
{
    goto_pagemode(pstruct);
}

static void goto_textmode(pdfstructure * p)
{
    scaledpos origin = {
        0, 0
    };
    if (!is_textmode(p)) {
        if (is_pagemode(p)) {
            set_pos(p, &origin);        /* reset to page origin */
            begin_text(p);
        } else {
            if (is_charmode(p))
                end_charmode(p);
            if (is_chararraymode(p))
                end_chararray(p);
        }
        assert(is_textmode(p));
    }
}

void pos_finish(pdfstructure * p)
{
    goto_pagemode(p);
}

/**********************************************************************/

static void place_rule(pdfstructure * p, scaledpos * pos, scaled wd, scaled ht)
{
    pdfpos dim;
    scaledpos tmppos = *pos;
    goto_pagemode(p);
    dim.h.m = lround(wd * p->k1);
    dim.h.e = p->pdf.h.e;
    dim.v.m = lround(ht * p->k1);
    dim.v.e = p->pdf.v.e;
    pdf_printf("q\n");
    if (ht <= one_bp) {
        tmppos.v += lround(0.5 * ht);
        set_pos_temp(p, &tmppos);
        pdf_printf("[]0 d 0 J ");
        print_pdffloat(&(dim.v));
        pdf_printf(" w 0 0 m ");
        print_pdffloat(&(dim.h));
        pdf_printf(" 0 l S\n");
    } else if (wd <= one_bp) {
        tmppos.h += lround(0.5 * wd);
        set_pos_temp(p, &tmppos);
        pdf_printf("[]0 d 0 J ");
        print_pdffloat(&(dim.h));
        pdf_printf(" w 0 0 m 0 ");
        print_pdffloat(&(dim.v));
        pdf_printf(" l S\n");
    } else {
        set_pos_temp(p, &tmppos);
        pdf_printf("0 0 ");
        print_pdffloat(&(dim.h));
        pdf_printf(" ");
        print_pdffloat(&(dim.v));
        pdf_printf(" re f\n");
    }
    pdf_printf("Q\n");
}

void pdf_place_rule(scaled h, scaled v, scaled wd, scaled ht)
{
    scaledpos pos;
    pos.h = h;
    pos.v = v;
    place_rule(pstruct, &pos, wd, ht);
}

/**********************************************************************/

static void setup_fontparameters(pdfstructure * p, internal_font_number f)
{
    p->f_cur = f;
    p->f_pdf = pdf_set_font(f);
    p->tj_delta.e = p->cw.e - 1;        /* "- 1" makes less corrections inside []TJ */
    /* no need to be more precise than TeX (1sp) */
    while (p->tj_delta.e > 0
           && (double) font_size(f) / exp10[p->tj_delta.e + e_tj] < 0.5)
        p->tj_delta.e--;        /* happens for very tiny fonts */
    assert(p->cw.e >= p->tj_delta.e);   /* else we would need, e. g., exp10[-1] */
    p->fs.m = lround(font_size(f) / one_bp * exp10[p->fs.e]);
    p->hz.m = pdf_font_expand_ratio(f) + exp10[p->hz.e];
    calc_k2(p);
}

static void set_textmatrix(pdfstructure * p, scaledpos * pos)
{
    boolean move;
    assert(is_textmode(p));
    move = calc_pdfpos(p, pos);
    if (move == TRUE) {
        pdf_print_tm(p->tm);
        p->pdf.h.m = p->pdf_bt_pos.h.m + p->tm[4].m;    /* Tm replaces */
        p->pdf.v.m = p->pdf_bt_pos.v.m + p->tm[5].m;
    }
}

static void set_font(pdfstructure * p)
{
    pdf_printf("/F%d", p->f_pdf);
    pdf_print_resname_prefix();
    pdf_printf(" ");
    print_pdffloat(&(p->fs));
    pdf_printf(" Tf ");
}

/**********************************************************************/

static void
place_glyph(pdfstructure * p, scaledpos * pos, internal_font_number f,
            integer c)
{
    int move;
    if (f != p->f_cur || is_textmode(p) || is_pagemode(p)) {
        goto_textmode(p);
        if (f != p->f_cur)
            setup_fontparameters(p, f);
        set_font(p);
        set_textmatrix(p, pos);
        begin_chararray(p);
    }
    assert(is_charmode(p) || is_chararraymode(p));
    move = calc_pdfpos(p, pos);
    if (move == TRUE) {
        if ((p->wmode == WMODE_H && abs(p->tm[5].m) >= 1)
            || (p->wmode == WMODE_V && abs(p->tm[4].m) >= 1)
            || abs(p->tj_delta.m) >= 1000000) {
            goto_textmode(p);
            set_textmatrix(p, pos);
            begin_chararray(p);
            move = calc_pdfpos(p, pos);
        }
        assert((p->wmode == WMODE_H && p->tm[5].m == 0)
               || (p->wmode == WMODE_V && p->tm[4].m == 0));
        if (move == TRUE) {
            if (is_charmode(p))
                end_charmode(p);
            assert(abs(p->tj_delta.m) >= 1);
            print_pdffloat(&(p->tj_delta));
            p->cw.m -= p->tj_delta.m * exp10[p->cw.e - p->tj_delta.e];
        }
    }
    if (is_chararraymode(p))
        begin_charmode(p);
    pdf_print_char(f, c);       /* this also does pdf_mark_char() */
    p->cw.m += pdf_char_width(p, p->f_pdf, c);  /* aka adv_char_width() */
}

void pdf_place_glyph(internal_font_number f, integer c)
{
    if (char_exists(f, c))
        place_glyph(pstruct, &pos, f, c);
}

/**********************************************************************/

static void place_form(pdfstructure * p, scaledpos * pos, integer i)
{
    goto_pagemode(p);
    pdf_printf("q\n");
    set_pos_temp(p, pos);
    pdf_printf("/Fm%d", i);
    pdf_print_resname_prefix();
    pdf_printf(" Do\nQ\n");
}

void pdf_place_form(scaled h, scaled v, integer i)
{
    scaledpos pos;
    pos.h = h;
    pos.v = v;
    place_form(pstruct, &pos, i);
}
