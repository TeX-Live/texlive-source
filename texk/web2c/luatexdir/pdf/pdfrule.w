% pdfrule.w
% 
% Copyright 2010 Taco Hoekwater <taco@@luatex.org>

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
    "$Id: pdfrule.w 3584 2010-04-02 17:45:55Z hhenkel $"
    "$URL: http://foundry.supelec.fr/svn/luatex/tags/beta-0.66.0/source/texk/web2c/luatexdir/pdf/pdfrule.w $";

#include "ptexlib.h"

@ @c
#include "pdf/pdfpage.h"

#define lround(a) (long) floor((a) + 0.5)

@ @c
void pdf_place_rule(PDF pdf, halfword q, scaledpos size)
{
    pdfpos dim;
    pdfstructure *p = pdf->pstruct;
    scaledpos pos = pdf->posstruct->pos;
    (void) q;
    pdf_goto_pagemode(pdf);
    dim.h.m = lround(size.h * p->k1);
    dim.h.e = p->pdf.h.e;
    dim.v.m = lround(size.v * p->k1);
    dim.v.e = p->pdf.v.e;
    pdf_printf(pdf, "q\n");
    if (size.v <= one_bp) {
        pos.v += (int) lround(0.5 * size.v);
        pdf_set_pos_temp(pdf, pos);
        pdf_printf(pdf, "[]0 d 0 J ");
        print_pdffloat(pdf, dim.v);
        pdf_printf(pdf, " w 0 0 m ");
        print_pdffloat(pdf, dim.h);
        pdf_printf(pdf, " 0 l S\n");
    } else if (size.h <= one_bp) {
        pos.h += (int) lround(0.5 * size.h);
        pdf_set_pos_temp(pdf, pos);
        pdf_printf(pdf, "[]0 d 0 J ");
        print_pdffloat(pdf, dim.h);
        pdf_printf(pdf, " w 0 0 m 0 ");
        print_pdffloat(pdf, dim.v);
        pdf_printf(pdf, " l S\n");
    } else {
        pdf_set_pos_temp(pdf, pos);
        pdf_printf(pdf, "0 0 ");
        print_pdffloat(pdf, dim.h);
        pdf_printf(pdf, " ");
        print_pdffloat(pdf, dim.v);
        pdf_printf(pdf, " re f\n");
    }
    pdf_printf(pdf, "Q\n");
}
