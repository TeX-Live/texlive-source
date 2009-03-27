/* writetype0.c
   
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
#include "writettf.h"
#include "writecff.h"

static const char _svn_version[] =
    "$Id: writetype0.c 1783 2009-01-18 12:51:01Z oneiros $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/font/writetype0.c $";

void writetype0(fd_entry * fd)
{
    int callback_id;
    int file_opened = 0;
    char *ftemp = NULL;
    long i;
    dirtab_entry *tab;
    cff_font *cff;

    dir_tab = NULL;
    glyph_tab = NULL;

    fd_cur = fd;                /* fd_cur is global inside writettf.c */
    assert(fd_cur->fm != NULL);
    assert(is_opentype(fd_cur->fm));
    assert(is_included(fd_cur->fm));

    set_cur_file_name(fd_cur->fm->ff_name);
    ttf_curbyte = 0;
    ttf_size = 0;
    callback_id = callback_defined(find_opentype_file_callback);
    if (callback_id > 0) {
        if (run_callback
            (callback_id, "S->S", (char *) (nameoffile + 1), &ftemp)) {
            if (ftemp != NULL && strlen(ftemp)) {
                free(nameoffile);
                namelength = strlen(ftemp);
                nameoffile = xmalloc(namelength + 2);
                strcpy((char *) (nameoffile + 1), ftemp);
                free(ftemp);
            }
        }
    }
    callback_id = callback_defined(read_opentype_file_callback);
    if (callback_id > 0) {
        if (run_callback(callback_id, "S->bSd", (char *) (nameoffile + 1),
                         &file_opened, &ttf_buffer, &ttf_size) &&
            file_opened && ttf_size > 0) {
        } else {
            pdftex_fail("cannot open OpenType font file for reading");
        }
    } else {
        if (!otf_open()) {
            pdftex_fail("cannot open OpenType font file for reading");
        }
        ttf_read_file();
        ttf_close();
    }

    cur_file_name = (char *) nameoffile + 1;
    fd_cur->ff_found = true;

    if (tracefilenames) {
        if (is_subsetted(fd_cur->fm))
            tex_printf("<%s", cur_file_name);
        else
            tex_printf("<<%s", cur_file_name);
    }
    ttf_read_tabdir();
    /* read font parameters */
    if (ttf_name_lookup("head", false) != NULL)
        ttf_read_head();
    if (ttf_name_lookup("hhea", false) != NULL)
        ttf_read_hhea();
    if (ttf_name_lookup("PCLT", false) != NULL)
        ttf_read_pclt();
    if (ttf_name_lookup("post", false) != NULL)
        ttf_read_post();

    /* copy font file */
    tab = ttf_seek_tab("CFF ", 0);

    /* TODO the next 0 is a subfont index */
    cff = read_cff(ttf_buffer + ttf_curbyte, tab->length, 0);
    if (cff != NULL) {
        if (cff_is_cidfont(cff)) {
            write_cid_cff(cff, fd_cur);
            /*
               for (i = tab->length; i > 0; i--)
               fb_putchar (ttf_getnum(1));
             */
        } else {
            write_cff(cff, fd_cur);
        }
    } else {
        /* not understood, just do a copy */
        for (i = tab->length; i > 0; i--)
            fb_putchar(ttf_getnum(1));
    }
    xfree(dir_tab);
    xfree(ttf_buffer);
    if (tracefilenames) {
        if (is_subsetted(fd_cur->fm))
            tex_printf(">");
        else
            tex_printf(">>");
    }
    cur_file_name = NULL;
}
