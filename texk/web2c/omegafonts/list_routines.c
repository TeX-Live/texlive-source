/* list_routines.c: Types used in this program.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

#include "cpascal.h"
#include "list_routines.h"

static list
list1 (void *x)
{
list temp;
temp = (list) xmalloc(sizeof(cell));
temp->contents = x;
temp->ptr = NULL;
return temp;
}

/* Attribute-value list function */

av_list
av_list1 (int attr, int val)
{
av_list temp;
temp = (av_list) xmalloc(sizeof(av_cell));
temp->attribute = attr;
temp->value = val;
temp->ptr = NULL;
return temp;
}

/* Index-value list function */

in_list
in_list1 (int val, in_list N)
{
in_list temp;
temp = (in_list) xmalloc(sizeof(in_cell));
temp->index = 0;
temp->value = val;
temp->actual = N;
temp->ptr = NULL;
return temp;
}

/* Attribute-value list function */

hash_list
hash_list1 (int x, int y, int new_class, int lig_z, hash_list N)
{
hash_list temp;
temp = (hash_list) xmalloc(sizeof(hash_cell));
temp->x = x;
temp->y = y;
temp->new_class = new_class;
temp->lig_z = lig_z;
temp->ptr = N;
return temp;
}


void
append_to_queue(queue *q, void *content)
{
    if (q->front == NULL) {
        q->front = list1(content);
        q->tail = q->front;
    } else {
        q->tail->ptr = list1(content);
        q->tail = q->tail->ptr;
    }
}
