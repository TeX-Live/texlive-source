/*
Copyright (c) 2004-2005 Han The Thanh, <thanh@pdftex.org>

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

$Id: avlstuff.c,v 1.12 2005/07/11 20:27:39 hahe Exp hahe $

*/

#include "mplib.h"
#include <kpathsea/c-vararg.h>
#include <kpathsea/c-proto.h>
#include "avl.h"

static const char perforce_id[] =
    "$Id: avlstuff.c,v 1.12 2005/07/11 20:27:39 hahe Exp hahe $";

/* memory management functions for avl */

void *avl_xmalloc (struct libavl_allocator *allocator, size_t size)
{
    assert (allocator != NULL && size > 0);
    return xmalloc (size);
}

void avl_xfree (struct libavl_allocator *allocator, void *block)
{
    assert (allocator != NULL && block != NULL);
    xfree (block);
}

struct libavl_allocator avl_xallocator = {
    avl_xmalloc,
    avl_xfree
};


/**********************************************************************/
