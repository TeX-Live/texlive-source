/* Copyright (c) 2006 Taco Hoekwater <taco@elvenkind.com>

This file is part of MetaPost.

MetaPost is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

MetaPost is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MetaPost; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id $
*/

#include "mplib.h"

/*@unused@*/
static const char perforce_id[] =
    "$Id: writeenc.c,v 1.3 2005/12/26 14:20:13 hahe Exp hahe $";

void read_enc (enc_entry * e)
{
    assert (e != NULL);
    if (e->loaded)
        return;
    e->encname = NULL;
    load_enc (e->name, &e->encname, e->glyph_names);
    e->loaded = true;
}

/* write_enc is used to write either external encoding (given in map file) or
 * internal encoding (read from the font file); when glyph_names is NULL
 * the 2nd argument is a pointer to the encoding entry; otherwise the 3rd is 
 * the object number of the Encoding object
 */
void write_enc (char **glyph_names, enc_entry * e, integer eobjnum)
{
    int i;
    int s;
    int foffset;
    char **g;
    if (glyph_names == NULL) {
        assert (e != NULL);
        if (e->objnum != 0)     /* the encoding has been written already */
            return;
        e->objnum = 1;
        g = e->glyph_names;
    } else {
        g = glyph_names;
    }
    pdf_printf("\n%%%%BeginResource: encoding %s",e->encname);
    pdf_printf ("\n/%s [ ", e->encname);
    foffset = strlen(e->name)+3;
    for (i = 0; i < 256; i++) {
      s = strlen(g[i]);
      if (s+1+foffset>=80) {
	pdf_printf ("\n");
	foffset = 0;
      }
      foffset += s+2;
      pdf_printf ("/%s ", g[i]);
    }
    if (foffset>75)
      pdf_printf ("\n");
    pdf_puts ("] def\n");
    pdf_printf("%%%%EndResource");
}

/**********************************************************************/
/* All encoding entries go into AVL tree for fast search by name. */

struct avl_table *enc_tree = NULL;

/* AVL sort enc_entry into enc_tree by name */

static int comp_enc_entry (const void *pa, const void *pb, void *p)
{
    return strcmp (((const enc_entry *) pa)->name,
                   ((const enc_entry *) pb)->name);
}

enc_entry *add_enc (char *s)
{
    int i;
    enc_entry tmp, *p;
    void **aa;

    assert (s != NULL);
    if (enc_tree == NULL) {
        enc_tree = avl_create (comp_enc_entry, NULL, &avl_xallocator);
        assert (enc_tree != NULL);
    }
    tmp.name = s;
    p = (enc_entry *) avl_find (enc_tree, &tmp);
    if (p != NULL)              /* encoding already registered */
        return p;
    p = xtalloc (1, enc_entry);
    p->loaded = false;
    p->name = xstrdup (s);
    p->objnum = 0;
    p->tounicode = 0;
    p->glyph_names = xtalloc (256, char *);
    for (i = 0; i < 256; i++)
        p->glyph_names[i] = (char *) notdef;
    aa = avl_probe (enc_tree, p);
    assert (aa != NULL);
    return p;
}

/**********************************************************************/
/* cleaning up... */

static void destroy_enc_entry (void *pa, void *pb)
{
    enc_entry *p;
    int i;

    p = (enc_entry *) pa;
    xfree (p->name);
    if (p->glyph_names != NULL)
        for (i = 0; i < 256; i++)
            if (p->glyph_names[i] != notdef)
                xfree (p->glyph_names[i]);
    xfree (p->glyph_names);
    xfree (p);
}

void enc_free ()
{
    if (enc_tree != NULL)
        avl_destroy (enc_tree, destroy_enc_entry);
}

/**********************************************************************/
