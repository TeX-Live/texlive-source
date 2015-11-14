% pdfrule.w
%
% Copyright 2010-2011 Taco Hoekwater <taco@@luatex.org>
%
% This file is part of LuaTeX.
%
% LuaTeX is free software; you can redistribute it and/or modify it under
% the terms of the GNU General Public License as published by the Free
% Software Foundation; either version 2 of the License, or (at your
% option) any later version.
%
% LuaTeX is distributed in the hope that it will be useful, but WITHOUT
% ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
% FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
% License for more details.
%
% You should have received a copy of the GNU General Public License along
% with LuaTeX; if not, see <http://www.gnu.org/licenses/>.

@ @c


#include "ptexlib.h"
#include "pdf/pdfpage.h"

@ @c
void pdf_place_rule(PDF pdf, halfword q, scaledpos size)
{
    pdfpos dim;
    pdfstructure *p = pdf->pstruct;
    scaledpos pos = pdf->posstruct->pos;
    /*  (void) q; */
    if (subtype(q) == box_rule) {
        pdf_place_form(pdf,q);
    } else if (subtype(q) == image_rule) {
        pdf_place_image(pdf,q);
    } else if (subtype(q) == empty_rule) {
        /* place nothing, only take space */
    } else {
        /* normal_rule or >= 100 being a leader rule */
        pdf_goto_pagemode(pdf);
        dim.h.m = i64round(size.h * p->k1);
        dim.h.e = p->pdf.h.e;
        dim.v.m = i64round(size.v * p->k1);
        dim.v.e = p->pdf.v.e;
        pdf_puts(pdf, "q\n");
        if (size.v <= one_bp) {
            pos.v += i32round(0.5 * size.v);
            pdf_set_pos_temp(pdf, pos);
            pdf_puts(pdf, "[]0 d 0 J ");
            print_pdffloat(pdf, dim.v);
            pdf_puts(pdf, " w 0 0 m ");
            print_pdffloat(pdf, dim.h);
            pdf_puts(pdf, " 0 l S\n");
        } else if (size.h <= one_bp) {
            pos.h += i32round(0.5 * size.h);
            pdf_set_pos_temp(pdf, pos);
            pdf_puts(pdf, "[]0 d 0 J ");
            print_pdffloat(pdf, dim.h);
            pdf_puts(pdf, " w 0 0 m 0 ");
            print_pdffloat(pdf, dim.v);
            pdf_puts(pdf, " l S\n");
        } else {
            pdf_set_pos_temp(pdf, pos);
            pdf_puts(pdf, "0 0 ");
            print_pdffloat(pdf, dim.h);
            pdf_out(pdf, ' ');
            print_pdffloat(pdf, dim.v);
            pdf_puts(pdf, " re f\n");
        }
        pdf_puts(pdf, "Q\n");
    }
}
