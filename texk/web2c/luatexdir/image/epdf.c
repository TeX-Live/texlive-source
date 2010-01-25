/* epdf.c

   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
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

#include "ptexlib.h"
#include "inc-epdf.h"

#include <string.h>

static const char _svn_version[] =
    "$Id: epdf.c 2599 2009-06-25 10:35:26Z taco $ "
    "$URL: http://foundry.supelec.fr/svn/luatex/tags/beta-0.40.6/source/texk/web2c/luatexdir/image/epdf.c $";

fd_entry *epdf_create_fontdescriptor(fm_entry * fm, int stemV)
{
    fd_entry *fd;
    if ((fd = lookup_fd_entry(fm->ff_name, fm->slant, fm->extend)) == NULL) {
        fm->in_use = true;
        fd = new_fd_entry();
        fd->fm = fm;
        register_fd_entry(fd);
        fd->fd_objnum = pdf_new_objnum();
        assert(fm->ps_name != NULL);
        fd->fontname = xstrdup(fm->ps_name);    /* just fallback */
        // stemV must be copied
        fd->font_dim[STEMV_CODE].val = stemV;
        fd->font_dim[STEMV_CODE].set = true;
        fd->gl_tree = avl_create(comp_string_entry, NULL, &avl_xallocator);
        assert(fd->gl_tree != NULL);
    }
    return fd;
}

/***********************************************************************
 * Mark glyphs used by embedded PDF file:
 * 1. build fontdescriptor, if not yet existing
 * 2. mark glyphs directly there
 *
 * Input charset from xpdf is a string of glyph names including
 * leading slashes, but without blanks between them, like: /a/b/c
***********************************************************************/

void epdf_mark_glyphs(fd_entry * fd, char *charset)
{
    char *p, *q, *s;
    char *glyph;
    void **aa;
    if (charset == NULL)
        return;
    assert(fd != NULL);
    for (s = charset + 1, q = charset + strlen(charset); s < q; s = p + 1) {
        for (p = s; *p != '\0' && *p != '/'; p++);
        *p = '\0';
        if ((char *) avl_find(fd->gl_tree, s) == NULL) {
            glyph = xstrdup(s);
            aa = avl_probe(fd->gl_tree, glyph);
            assert(aa != NULL);
        }
    }
}

void epdf_free(void)
{
    epdf_check_mem();
}
