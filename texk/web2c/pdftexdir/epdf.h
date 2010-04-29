/*
Copyright (c) 1996-2008 Han The Thanh, <thanh@pdftex.org>

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
with pdfTeX; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

extern "C" {

#include <kpathsea/c-auto.h>

    extern char *xstrdup(const char *);

/* the following code is extremly ugly but needed for including web2c/config.h */

    typedef const char *const_string;   /* including kpathsea/types.h doesn't work on some systems */

#define KPATHSEA_CONFIG_H       /* avoid including other kpathsea header files */
    /* from web2c/config.h */

#ifdef CONFIG_H                 /* CONFIG_H has been defined by some xpdf */
#  undef CONFIG_H               /* header file */
#endif

#include <c-auto.h>       /* define SIZEOF_LONG */
#include <config.h>       /* define type integer */

#include <pdftexdir/ptexmac.h>

#include "openbsd-compat.h"

/* #-define pdfbufsize      pdfbufmax */

    extern float epdf_width;
    extern float epdf_height;
    extern float epdf_orig_x;
    extern float epdf_orig_y;
    extern float epdf_rotate;
    extern integer epdf_selected_page;
    extern integer epdf_num_pages;
    extern integer epdf_page_box;
    extern void *epdf_doc;
    extern void *epdf_xref;

    extern integer pdfboxspecmedia;
    extern integer pdfboxspeccrop;
    extern integer pdfboxspecbleed;
    extern integer pdfboxspectrim;
    extern integer pdfboxspecart;

    extern longinteger pdfstreamlength;
    extern longinteger pdfptr;
    extern integer poolptr;
    typedef unsigned char eightbits;
    extern eightbits *pdfbuf;
    extern integer pdfbufsize;
    extern integer pdfosmode;
    extern eightbits pdflastbyte;
    extern integer fixedinclusioncopyfont;

    extern char notdef[];

    extern struct fm_entry *lookup_fontmap(char *);
    extern integer get_fontfile(struct fm_entry *);
    extern integer get_fontname(struct fm_entry *);
    extern integer pdfnewobjnum(void);
    extern void pdfbeginstream(void);
    extern void pdfendobj(void);
    extern void pdfendstream(void);
    extern void pdfflush(void);
    extern void zpdfbegindict(integer, bool);
    extern void zpdfbeginobj(integer, bool);
    extern void zpdfcreateobj(integer, integer);
    extern void zpdfnewdict(integer, integer, bool);
    extern void zpdfosgetosbuf(integer);

#include "pdftex-common.h"
}
