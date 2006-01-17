/*
Copyright (c) 1996-2005 Han The Thanh, <thanh@pdftex.org>

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

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/writeenc.c#13 $

source code indentation by "indent -kr -nut"
*/

#include "ptexlib.h"
#include "avlstuff.h"

static const char perforce_id[] =
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/writeenc.c#13 $";

void read_enc(enc_entry * e)
{
    assert(e != NULL);
    if (e->loaded)
        return;
    load_enc(e->name, e->glyph_names);
    e->loaded = true;
}

/* write_enc is used to write either external encoding (given in map file) or
 * internal encoding (read from the font file); when glyph_names is NULL
 * the 2nd argument is a pointer to the encoding entry; otherwise the 3rd is 
 * the object number of the Encoding object
 */
void write_enc(char **glyph_names, enc_entry * e, integer eobjnum)
{
    boolean is_notdef;
    int i;
    char **g;
    if (glyph_names == NULL) {
        assert(e != NULL);
        if (e->objnum != 0)     /* the encoding has been written already */
            return;
        pdfnewdict(0, 0);
        e->objnum = objptr;
        g = e->glyph_names;
    } else {
        pdfbegindict(eobjnum);
        g = glyph_names;
    }
    pdf_printf("/Type /Encoding\n/Differences [ 0 /%s", g[0]);
    is_notdef = (g[0] == notdef);
    for (i = 1; i <= MAX_CHAR_CODE; i++) {
        if (g[i] == notdef) {
            if (!is_notdef) {
                pdf_printf(" %i/%s", i, notdef);
                is_notdef = true;
            }
        } else {
            if (is_notdef) {
                pdf_printf(" %i", i);
                is_notdef = false;
            }
            pdf_printf("/%s", g[i]);
        }
    }
    pdf_puts("]\n");
    pdfenddict();
}

/**********************************************************************/
/* All encoding entries go into AVL tree for fast search by name. */

struct avl_table *enc_tree = NULL;

/* AVL sort enc_entry into enc_tree by name */

static int comp_enc_entry(const void *pa, const void *pb, void *p)
{
    return strcmp(((const enc_entry *) pa)->name,
                  ((const enc_entry *) pb)->name);
}

enc_entry *add_enc(char *s)
{
    int i;
    enc_entry tmp, *p;
    void **aa;

    assert(s != NULL);
    if (enc_tree == NULL) {
        enc_tree = avl_create(comp_enc_entry, NULL, &avl_xallocator);
        assert(enc_tree != NULL);
    }
    tmp.name = s;
    p = (enc_entry *) avl_find(enc_tree, &tmp);
    if (p != NULL)              /* encoding already registered */
        return p;
    p = xtalloc(1, enc_entry);
    p->loaded = false;
    p->name = xstrdup(s);
    p->objnum = 0;
    p->glyph_names = xtalloc(MAX_CHAR_CODE + 1, char *);
    for (i = 0; i <= MAX_CHAR_CODE; i++)
        p->glyph_names[i] = (char *) notdef;
    aa = avl_probe(enc_tree, p);
    assert(aa != NULL);
    return p;
}

/**********************************************************************/
/* cleaning up... */

static void destroy_enc_entry(void *pa, void *pb)
{
    enc_entry *p;
    int i;

    p = (enc_entry *) pa;
    xfree(p->name);
    if (p->glyph_names != NULL)
        for (i = 0; i <= MAX_CHAR_CODE; i++)
            if (p->glyph_names[i] != notdef)
                xfree(p->glyph_names[i]);
    xfree(p->glyph_names);
    xfree(p);
}

void enc_free()
{
    if (enc_tree != NULL)
        avl_destroy(enc_tree, destroy_enc_entry);
}

/**********************************************************************/
