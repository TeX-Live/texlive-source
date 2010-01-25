/* pdfpage.h

   Copyright 2009 Taco Hoekwater <taco@luatex.org>

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

/* $Id: pdfpage.h 2448 2009-06-08 07:43:50Z taco $ */

#ifndef PDFPAGE_H
#  define PDFPAGE_H

typedef struct {
    long m;                     /* mantissa (significand) */
    int e;                      /* exponent * -1 */
} pdffloat;

typedef struct {
    pdffloat h;
    pdffloat v;
} pdfpos;

typedef enum { PMODE_NONE, PMODE_PAGE, PMODE_TEXT, PMODE_CHARARRAY,
    PMODE_CHAR
} pos_mode;

typedef enum { WMODE_H, WMODE_V } writing_mode; /* []TJ runs horizontal or vertical */

#  define is_pagemode(p)      ((p)->mode == PMODE_PAGE)
#  define is_textmode(p)      ((p)->mode == PMODE_TEXT)
#  define is_chararraymode(p) ((p)->mode == PMODE_CHARARRAY)
#  define is_charmode(p)      ((p)->mode == PMODE_CHAR)

#  define setpdffloat(a,b,c) {(a).m = (b); (a).e = (c);}

#  ifdef hz
/* AIX 4.3 defines hz as 100 in system headers */
#    undef hz
#  endif

typedef struct {
    pdfpos pdf;                 /* pos. on page (PDF page raster) */
    pdfpos pdf_bt_pos;          /* pos. at begin of BT-ET group (PDF page raster) */
    pdfpos pdf_tj_pos;          /* pos. at begin of TJ array (PDF page raster) */
    pdffloat cw;                /* pos. within [(..)..]TJ array (glyph raster);
                                   cw.e = fractional digits in /Widths array */
    pdffloat tj_delta;          /* rel. movement in [(..)..]TJ array (glyph raster) */
    pdffloat fs;                /* font size in PDF units */
    pdffloat hz;                /* HZ expansion factor */
    pdffloat ext;               /* ExtendFont factor */
    pdffloat cm[6];             /* cm array */
    pdffloat tm[6];             /* Tm array */
    double k1;                  /* conv. factor from TeX sp to PDF page raster */
    double k2;                  /* conv. factor from PDF page raster to TJ array raster */
    internal_font_number f_cur; /* TeX font number */
    internal_font_number f_pdf; /* /F* font number, of unexpanded base font! */
    writing_mode wmode;         /* PDF writing mode WMode (horizontal/vertical) */
    pos_mode mode;              /* current positioning mode */
} pdfstructure;

extern pdfstructure *pstruct;

/**********************************************************************/

boolean calc_pdfpos(pdfstructure * p, scaledpos * pos);
void pdf_end_string_nl(void);
void pdf_goto_pagemode(void);
void pdf_page_init(void);
void pdf_place_form(scaled h, scaled v, integer i);
void pdf_place_glyph(internal_font_number f, integer c);
void pdf_place_rule(scaled h, scaled v, scaled wd, scaled ht);
void pdf_print_charwidth(internal_font_number f, int i);
void pdf_print_cm(pdffloat * cm);
void pdf_set_pos(scaled h, scaled v);
void pos_finish(pdfstructure * p);
void print_pdffloat(pdffloat * f);

#endif
