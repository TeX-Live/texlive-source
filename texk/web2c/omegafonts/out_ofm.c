/* out_ofm.c: Outputting to an OFM file.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

#include "cpascal.h"
#include "list_routines.h"
#include "header_routines.h"
#include "manifests.h"
#include "omfonts.h"
#include "char_routines.h"
#include "ligkern_routines.h"
#include "out_ofm.h"
#include "extra_routines.h"
#include "param_routines.h"
#include "dimen_routines.h"
#include "error_routines.h"
#include "parse_ofm.h"

static void compute_ofm_subsizes(void);
static void output_ofm_subsizes(void);

void
output_ofm_file(void)
{
    check_and_correct();
    if (omit_ofm) return;
    compute_ofm_extra_stuff();
    compute_ofm_character_info();
    compute_ofm_subsizes();
    output_ofm_subsizes();
    output_ofm_header();
    output_ofm_extra_stuff();
    output_ofm_character_info();
    output_ofm_dimension();
    output_ofm_ligkern();
    output_ofm_extensible();
    output_ofm_parameter();
}

static void
compute_ofm_subsizes(void)
{
    switch(ofm_level) {
        case OFM_TFM: {
            lh = header_max + 1;
            nw++; nh++; nd++; ni++;
            compute_ligkern_offset();
            lf = 6+lh+(ec-bc+1)+nw+nh+nd+ni+nl+lk_offset+nk+ne+np;
            break;
        }
        case OFM_LEVEL0: {
            lh = header_max + 1;
            nw++; nh++; nd++; ni++;
            compute_ligkern_offset();
            lf = 14+lh+2*(ec-bc+1)+nw+nh+nd+ni+2*(nl+lk_offset)+nk+2*ne+np;
            break;
        }
        case OFM_LEVEL1: {
            lh = header_max + 1;
            nw++; nh++; nd++; ni++;
            compute_ligkern_offset();
            words_per_entry = (12 + 2*npc) / 4;
            ncw = num_char_info * words_per_entry;
            lf = 29+lh+ncw+nw+nh+nd+ni+2*(nl+lk_offset)+nk+2*ne+np+
                 nki+nwi+nkf+nwf+nkm+nwm+nkr+nwr+nkg+nwg+nkp+nwp;
            nco = 29+lh+nki+nwi+nkf+nwf+nkm+nwm+nkr+nwr+nkg+nwg+nkp+nwp;
            break;
        }
        default: { internal_error_0("output_ofm_subfiles"); }
    }   
}

static void
output_ofm_subsizes(void)
{
    switch(ofm_level) {
        case OFM_TFM: {
            out_ofm_2(lf); out_ofm_2(lh); out_ofm_2(bc); out_ofm_2(ec);
            out_ofm_2(nw); out_ofm_2(nh); out_ofm_2(nd); out_ofm_2(ni);
            out_ofm_2(nl+lk_offset); out_ofm_2(nk);
            out_ofm_2(ne); out_ofm_2(np);
            break;
        }
        case OFM_LEVEL0: {
            out_ofm_4(0);
            out_ofm_4(lf); out_ofm_4(lh); out_ofm_4(bc); out_ofm_4(ec);
            out_ofm_4(nw); out_ofm_4(nh); out_ofm_4(nd); out_ofm_4(ni);
            out_ofm_4(nl+lk_offset); out_ofm_4(nk);
            out_ofm_4(ne); out_ofm_4(np); out_ofm_4(font_dir);
            break;
        }
        case OFM_LEVEL1: {
            out_ofm_4(1);
            out_ofm_4(lf); out_ofm_4(lh); out_ofm_4(bc); out_ofm_4(ec);
            out_ofm_4(nw); out_ofm_4(nh); out_ofm_4(nd); out_ofm_4(ni);
            out_ofm_4(nl+lk_offset); out_ofm_4(nk);
            out_ofm_4(ne); out_ofm_4(np); out_ofm_4(font_dir);
            out_ofm_4(nco); out_ofm_4(ncw); out_ofm_4(npc);
            out_ofm_4(nki); out_ofm_4(nwi);
            out_ofm_4(nkf); out_ofm_4(nwf);
            out_ofm_4(nkm); out_ofm_4(nwm);
            out_ofm_4(nkr); out_ofm_4(nwr);
            out_ofm_4(nkg); out_ofm_4(nwg);
            out_ofm_4(nkp); out_ofm_4(nwp);
            break;
        }
        default: { internal_error_0("output_ofm_subfiles"); }
    }
}

unsigned file_ofm_count = 0;

void
out_ofm(unsigned i)
{
    fputc(i,file_ofm);
    file_ofm_count++;
}

void
out_ofm_2(unsigned i)
{
 
    fputc((i>>8)&0xff,  file_ofm);
    fputc(i&0xff,       file_ofm);
    file_ofm_count += 2;
}

void
out_ofm_4(unsigned i)
{
    fputc((i>>24)&0xff, file_ofm);
    fputc((i>>16)&0xff, file_ofm);
    fputc((i>>8)&0xff,  file_ofm);
    fputc(i&0xff,       file_ofm);
    file_ofm_count += 4;
}

void
out_ofm_char(unsigned i)
{
    if (ofm_level == OFM_NOLEVEL) {
        internal_error_1("out_ofm_char.1 (ofm_level=%d)", ofm_level);
    }
    if (ofm_level == OFM_TFM) {
        if (i>=0x100) internal_error_1("out_ofm_char.2 (i=%x)", i);
        out_ofm(i);
    } else {
        if (i>=0x10000) internal_error_1("out_ofm_char.3 (i=%x)", i);
        out_ofm_2(i);
    }
}

#define dabs(x) ((x)>=0?(x):-(x))

void
out_ofm_scaled(fix fval)
{
    if (dabs((double)fval/(double)design_units) >= 16.0) {
        warning_2("The relative dimension %d is too large.\n"
                  "Must be less than 16*designsize = %d designunits",
                  fval/0x100000, design_units/0x10000);
        fval = 0;
    }
    if (design_units != UNITY) {
        fval = zround(((double)fval/(double)design_units) * 1048576.0);
    }
    if (fval < 0) {
        out_ofm(255); fval = fval + 0x1000000;
        if (fval <= 0) fval = 1;
    } else {
        out_ofm(0);
        if (fval >= 0x1000000) fval = 0xffffff;
    }
    out_ofm((fval >> 16) & 0xff);
    out_ofm((fval >> 8) & 0xff);
    out_ofm(fval & 0xff);
}
