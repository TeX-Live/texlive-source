/*
Copyright (c) 1996-2002 Han The Thanh, <thanh@pdftex.org>

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

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/epdf.c#10 $
*/

#include "ptexlib.h"
#include <kpathsea/c-vararg.h>
#include <kpathsea/c-proto.h>

integer pdfbufmax = pdfbufsize;

extern void epdf_check_mem(void);

int is_subsetable(fm_entry *fm)
{
    return is_included(fm) && is_subsetted(fm);
}

int is_type1(fm_entry *fm)
{
    return is_t1fontfile(fm);
}

void mark_glyphs(fm_entry *fm, char *charset)
{
    char *new_charset = fm->charset;
    if (charset == NULL)
        return;
    if (new_charset == NULL)
        new_charset = xstrdup(charset);
    else {
        xretalloc(new_charset, strlen(new_charset)+strlen(charset)+1, char);
        strcat(new_charset, charset);
    }
    fm->charset = new_charset;
}

void embed_whole_font(fm_entry *fm)
{
    fm->all_glyphs = true;
}

integer get_fontfile(fm_entry *fm)
{
    return fm->ff_objnum;
}

integer get_fontname(fm_entry *fm)
{
    if (fm->fn_objnum == 0)
        fm->fn_objnum = pdfnewobjnum();
    return fm->fn_objnum;
}

void epdf_free(void)
{
    epdf_check_mem();
}
