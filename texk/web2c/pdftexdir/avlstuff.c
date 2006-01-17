/*
Copyright (c) 2004 Han The Thanh, <thanh@pdftex.org>

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This file is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this file; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/avlstuff.c#5 $

formatted by indent -kr
*/

#include "ptexlib.h"
#include <kpathsea/c-vararg.h>
#include <kpathsea/c-proto.h>
#include "avl.h"

static const char perforce_id[] =
    "$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/avlstuff.c#5 $";

/* One AVL tree for each obj_type 0...pdfobjtypemax */

static struct avl_table *PdfObjTree[pdfobjtypemax + 1] =
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };


/* memory management functions for avl */

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


/* AVL sort objentry into avl_table[] */

int compare_info(const void *pa, const void *pb, void *param)
{
    integer a, b;
    int as, ae, bs, be, al, bl;

    a = ((const objentry *) pa)->int0;
    b = ((const objentry *) pb)->int0;
    if (a < 0 && b < 0) {        /* string comparison */
        as = strstart[-a];
        ae = strstart[-a + 1];        /* start of next string in pool */
        bs = strstart[-b];
        be = strstart[-b + 1];
        al = ae - as;
        bl = be - bs;
        if (al < bl)                /* compare first by string length */
            return -1;
        else if (al > bl)
            return 1;
        else
            for (; as < ae; as++, bs++) {
                if (strpool[as] < strpool[bs])
                    return -1;
                if (strpool[as] > strpool[bs])
                    return 1;
            }
        return 0;
    } else {                        /* integer comparison */
        if (a < b)
            return -1;
        else if (a > b)
            return 1;
        else
            return 0;
    }
}


void avlputobj(integer objptr, integer t)
{
    static void **pp;

    if (PdfObjTree[t] == NULL) {
        PdfObjTree[t] = avl_create(compare_info, NULL, &avl_xallocator);
        if (PdfObjTree[t] == NULL)
            pdftex_fail("avlstuff.c: avl_create() PdfObjTree failed");
    }
    pp = avl_probe(PdfObjTree[t], &(objtab[objptr]));
    if (pp == NULL)
        pdftex_fail("avlstuff.c: avl_probe() out of memory in insertion");
}


/* replacement for linear search pascal function "find_obj" */

integer avlfindobj(integer t, integer i, integer byname)
{
    static objentry *p;
    static objentry tmp;

    if (byname > 0)
        tmp.int0 = -i;
    else
        tmp.int0 = i;
    if (PdfObjTree[t] == NULL)
        return 0;
    p = (objentry *) avl_find(PdfObjTree[t], &tmp);
    if (p == NULL)
        return 0;
    else
        return (int) (p - (objentry *) objtab);
}

/* end of file avlstuff.c */
