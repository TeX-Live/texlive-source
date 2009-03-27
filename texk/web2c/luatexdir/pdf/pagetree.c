/* pagetree.c
   
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

/* $Id: pagetree.c 2031 2009-03-16 10:30:47Z taco $ */

#include "ptexlib.h"

#include "openbsd-compat.h"
#ifdef HAVE_ASPRINTF            /* asprintf is not defined in openbsd-compat.h, but in stdio.h */
#  include <stdio.h>
#endif

#include "sys/types.h"
#ifndef __MINGW32__
#  include "sysexits.h"
#else
#  define EX_SOFTWARE 70
#endif

static const char __svn_version[] = "$Id: pagetree.c 2031 2009-03-16 10:30:47Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/pdf/pagetree.c $";

/**********************************************************************/
/* Page diversions */

#ifdef DEBUG
#  define PAGES_TREE_KIDSMAX 3
#else
#  define PAGES_TREE_KIDSMAX 6
#endif

static struct avl_table *divert_list_tree = NULL;

typedef struct pages_entry_ {
    integer objnum;             /* object number of this /Pages object */
    integer number_of_pages;    /* total number of all pages below */
    integer number_of_kids;     /* number of direct kid objects */
    integer kids[PAGES_TREE_KIDSMAX];   /* array of kid object numbers */
    struct pages_entry_ *next;
} pages_entry;

typedef struct divert_list_entry_ {
    integer divnum;
    pages_entry *first;
    pages_entry *last;
} divert_list_entry;

static int comp_divert_list_entry(const void *pa, const void *pb, void *p)
{
    if (((const divert_list_entry *) pa)->divnum >
        ((const divert_list_entry *) pb)->divnum)
        return 1;
    if (((const divert_list_entry *) pa)->divnum <
        ((const divert_list_entry *) pb)->divnum)
        return -1;
    return 0;
}

static pages_entry *new_pages_entry()
{
    pages_entry *p;
    int i;
    p = xtalloc(1, pages_entry);
    p->number_of_pages = p->number_of_kids = 0;
    for (i = 0; i < PAGES_TREE_KIDSMAX; i++)
        p->kids[i] = 0;
    p->next = NULL;
    pdf_create_obj(0, 0);
    p->objnum = obj_ptr;
    return p;
}

static divert_list_entry *new_divert_list_entry()
{
    divert_list_entry *d;
    d = xtalloc(1, divert_list_entry);
    d->first = d->last = NULL;
    return d;
}

static void ensure_list_tree()
{
    if (divert_list_tree == NULL) {
        divert_list_tree =
            avl_create(comp_divert_list_entry, NULL, &avl_xallocator);
        assert(divert_list_tree != NULL);
    }
}

static divert_list_entry *get_divert_list(integer divnum)
{
    divert_list_entry *d, tmp;
    void **aa;
    tmp.divnum = divnum;
    d = (divert_list_entry *) avl_find(divert_list_tree, &tmp);
    if (d == NULL) {
        d = new_divert_list_entry();
        d->divnum = divnum;
        aa = avl_probe(divert_list_tree, d);
        assert(aa != NULL);
    }
    return d;
}

/* pdf_do_page_divert() returns the current /Parent object number */

integer pdf_do_page_divert(integer objnum, integer divnum)
{
    divert_list_entry *d;
    pages_entry *p;
#ifdef DEBUG
    pages_entry *q;
    struct avl_traverser t;
    int i;
#endif
    // initialize the tree
    ensure_list_tree();
    // make sure we have a list for this diversion
    d = get_divert_list(divnum);
    if (d->first == NULL || d->last->number_of_kids == PAGES_TREE_KIDSMAX) {
        // append a new pages_entry
        p = new_pages_entry();
        if (d->first == NULL)
            d->first = p;
        else
            d->last->next = p;
        d->last = p;
    }
    p = d->last;
    p->kids[p->number_of_kids++] = objnum;
    p->number_of_pages++;
#ifdef DEBUG
    printf("\n");
    avl_t_init(&t, divert_list_tree);
    for (d = avl_t_first(&t, divert_list_tree); d != NULL; d = avl_t_next(&t)) {
        printf("===== D-LIST %d: ", d->divnum);
        for (q = d->first; q != NULL; q = q->next) {
            printf("P=%d NK=%d (", q->objnum, q->number_of_kids);
            for (i = 0; i < q->number_of_kids; i++)
                printf("%d ", q->kids[i]);
            printf(") ");
        }
        printf("\n");
    }
    printf("\n");
#endif
    return p->objnum;
}

static void movelist(divert_list_entry * d, divert_list_entry * dto)
{
    if (d != NULL && d->first != NULL && d->divnum != dto->divnum) {    /* no undivert of empty list or into self */
        if (dto->first == NULL)
            dto->first = d->first;
        else
            dto->last->next = d->first;
        dto->last = d->last;
        d->first = d->last = NULL;      /* one could as well remove this divert_list_entry */
    }
}

/* undivert from diversion <divnum> into diversion <curdivnum> */

void pdf_do_page_undivert(integer divnum, integer curdivnum)
{
    divert_list_entry *d, *dto, tmp;
    struct avl_traverser t;
#ifdef DEBUG
    pages_entry *p;
    int i;
#endif
    // initialize the tree
    ensure_list_tree();
    // find the diversion <curdivnum> list where diversion <divnum> should go
    dto = get_divert_list(curdivnum);
    if (divnum == 0) {          /* 0 = special case: undivert _all_ lists */
        avl_t_init(&t, divert_list_tree);
        for (d = avl_t_first(&t, divert_list_tree); d != NULL;
             d = avl_t_next(&t))
            movelist(d, dto);
    } else {
        tmp.divnum = divnum;
        d = (divert_list_entry *) avl_find(divert_list_tree, &tmp);
        movelist(d, dto);
    }
#ifdef DEBUG
    printf("\n");
    avl_t_init(&t, divert_list_tree);
    for (d = avl_t_first(&t, divert_list_tree); d != NULL; d = avl_t_next(&t)) {
        printf("===== U-LIST %d: ", d->divnum);
        for (p = d->first; p != NULL; p = p->next) {
            printf("P=%d NK=%d (", p->objnum, p->number_of_kids);
            for (i = 0; i < p->number_of_kids; i++)
                printf("%d ", p->kids[i]);
            printf(") ");
        }
        printf("\n");
    }
    printf("\n");
#endif
}

/* write a /Pages object */

static void write_pages(pages_entry * p, int parent)
{
    int i;
    assert(p != NULL);
    pdf_begin_dict(p->objnum, 1);
    pdf_printf("/Type /Pages\n");
    if (parent == 0)            /* it's root */
        print_pdf_pages_attr();
    else
        pdf_printf("/Parent %d 0 R\n", parent);
    pdf_printf("/Count %d\n/Kids [", (int) p->number_of_pages);
    for (i = 0; i < p->number_of_kids; i++)
        pdf_printf("%d 0 R ", (int) p->kids[i]);
    remove_last_space();
    pdf_printf("]\n");
    pdf_end_dict();
}

/* loop over all /Pages objects, output them, create their parents,
 * recursing bottom up, return the /Pages root object number */

static integer output_pages_list(pages_entry * pe)
{
    pages_entry *p, *q, *r;
    assert(pe != NULL);
    if (pe->next == NULL) {     /* everything fits into one pages_entry */
        write_pages(pe, 0);     /* --> /Pages root found */
        return pe->objnum;
    }
    q = r = new_pages_entry();  /* one level higher needed */
    for (p = pe; p != NULL; p = p->next) {
        if (q->number_of_kids == PAGES_TREE_KIDSMAX) {
            q->next = new_pages_entry();
            q = q->next;
        }
        q->kids[q->number_of_kids++] = p->objnum;
        q->number_of_pages += p->number_of_pages;
        write_pages(p, q->objnum);
    }
    return output_pages_list(r);        /* recurse through next higher level */
}

integer output_pages_tree()
{
    divert_list_entry *d;
    pdf_do_page_undivert(0, 0); /* concatenate all diversions into diversion 0 */
    d = get_divert_list(0);     /* get diversion 0 */
    return output_pages_list(d->first);
}
