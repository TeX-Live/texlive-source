/* avlstuff.c

   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
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
#include <kpathsea/c-vararg.h>
#include <kpathsea/c-proto.h>
#include "avl.h"

static const char __svn_version[] =
    "$Id: avlstuff.c 2073 2009-03-21 10:06:50Z hhenkel $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/utils/avlstuff.c $";

static struct avl_table **PdfObjTree = NULL;

/**********************************************************************/
/* memory management functions for AVL */

void *avl_xmalloc(struct libavl_allocator *allocator, size_t size)
{
    assert(allocator != NULL && size > 0);
    return xmalloc(size);
}

void avl_xfree(struct libavl_allocator *allocator, void *block)
{
    assert(allocator != NULL && block != NULL);
    xfree(block);
}

struct libavl_allocator avl_xallocator = {
    avl_xmalloc,
    avl_xfree
};

/**********************************************************************/
/* general AVL comparison functions */

int comp_int_entry(const void *pa, const void *pb, void *p)
{
    cmp_return(*(const int *) pa, *(const int *) pb);
    return 0;
}

int comp_string_entry(const void *pa, const void *pb, void *p)
{
    return strcmp((const char *) pa, (const char *) pb);
}

/**********************************************************************/
/* One AVL tree for each obj_type 0...pdf_objtype_max */

typedef struct oentry_ {
    integer int0;
    integer objptr;
} oentry;

/* AVL sort oentry into avl_table[] */

int compare_info(const void *pa, const void *pb, void *param)
{
    integer a, b;
    int as, ae, bs, be, al, bl;

    a = ((const oentry *) pa)->int0;
    b = ((const oentry *) pb)->int0;
    if (a < 0 && b < 0) {       /* string comparison */
        if (a <= 2097152 && b <= 2097152) {
            a += 2097152;
            b += 2097152;
            as = str_start[-a];
            ae = str_start[-a + 1];     /* start of next string in pool */
            bs = str_start[-b];
            be = str_start[-b + 1];
            al = ae - as;
            bl = be - bs;
            if (al < bl)        /* compare first by string length */
                return -1;
            if (al > bl)
                return 1;
            for (; as < ae; as++, bs++) {
                if (str_pool[as] < str_pool[bs])
                    return -1;
                if (str_pool[as] > str_pool[bs])
                    return 1;
            }
        } else {
            pdftex_fail
                ("avlstuff.c: compare_items() for single characters: NI");
        }
    } else {                    /* integer comparison */
        if (a < b)
            return -1;
        if (a > b)
            return 1;
    }
    return 0;
}

void avl_put_obj(integer objptr, integer t)
{
    static void **pp;
    static oentry *oe;
    int i;

    if (PdfObjTree == NULL) {
        PdfObjTree = xtalloc(pdf_objtype_max + 1, struct avl_table *);
        for (i = 0; i <= pdf_objtype_max; i++)
            PdfObjTree[i] = NULL;
    }
    if (PdfObjTree[t] == NULL) {
        PdfObjTree[t] = avl_create(compare_info, NULL, &avl_xallocator);
        if (PdfObjTree[t] == NULL)
            pdftex_fail("avlstuff.c: avl_create() PdfObjTree failed");
    }
    oe = xtalloc(1, oentry);
    oe->int0 = obj_tab[objptr].int0;
    oe->objptr = objptr;        /* allows to relocate objtab */
    pp = avl_probe(PdfObjTree[t], oe);
    if (pp == NULL)
        pdftex_fail("avlstuff.c: avl_probe() out of memory in insertion");
}

/* replacement for linear search pascal function "find_obj()" */

integer avl_find_obj(integer t, integer i, integer byname)
{
    static oentry *p;
    static oentry tmp;

    if (byname > 0)
        tmp.int0 = -i;
    else
        tmp.int0 = i;
    if (PdfObjTree == NULL || PdfObjTree[t] == NULL)
        return 0;
    p = (oentry *) avl_find(PdfObjTree[t], &tmp);
    if (p == NULL)
        return 0;
    return p->objptr;
}

/**********************************************************************/

struct avl_table *mf_tree = NULL;

typedef struct {
    char *tfm_name;
    internalfontnumber fontnum;
} mf_entry;

/**********************************************************************/
/* cleaning up... */

static void destroy_oentry(void *pa, void *pb)
{
    oentry *p = (oentry *) pa;
    xfree(p);
}

void PdfObjTree_free()
{
    int i;

    if (PdfObjTree == NULL)
        return;
    for (i = 0; i <= pdf_objtype_max; i++) {
        if (PdfObjTree[i] != NULL)
            avl_destroy(PdfObjTree[i], destroy_oentry);
    }
    xfree(PdfObjTree);
    PdfObjTree = NULL;
}

/**********************************************************************/
