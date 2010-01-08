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

list
cons (void *x, list L)
{
list temp;
temp = (list) xmalloc(sizeof(cell));
temp->contents = x;
temp->ptr = L;
return temp;
}

list
list1 (void *x)
{
list temp;
temp = (list) xmalloc(sizeof(cell));
temp->contents = x;
temp->ptr = NULL;
return temp;
}

list
list2 (void *x, void *y)
{
list temp, temp1;
temp = (list) xmalloc(sizeof(cell));
temp1 = (list) xmalloc(sizeof(cell));
temp->contents = x;
temp->ptr = temp1;
temp1->contents = y;
temp1->ptr = NULL;
return temp;
}

list
append (list K, list L)
{
if (K==NULL) return L;
return cons(K->contents, append(K->ptr, L));
}

list
append1 (list L, void *x)
{
return (append(L,list1(x)));
}


/* Attribute-value list functions */

av_list
av_cons (int attr, int val, av_list L)
{
av_list temp;
temp = (av_list) xmalloc(sizeof(av_cell));
temp->attribute = attr;
temp->value = val;
temp->ptr = L;
return temp;
}

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

av_list
av_list2 (int attr, int val, int attr1, int val1)
{
av_list temp, temp1;
temp = (av_list) xmalloc(sizeof(av_cell));
temp1 = (av_list) xmalloc(sizeof(av_cell));
temp->attribute = attr;
temp->value = val;
temp->ptr = temp1;
temp1->attribute = attr1;
temp1->value = val1;
temp1->ptr = NULL;
return temp;
}

av_list
av_append (av_list K, av_list L)
{
if (K==NULL) return L;
return av_cons(K->attribute, K->value, av_append(K->ptr, L));
}

av_list
av_append1 (av_list L, int attr, int val)
{
return (av_append(L,av_list1(attr, val)));
}


/* Index-value list functions */

in_list
in_cons (int val, in_list N, in_list L)
{
in_list temp;
temp = (in_list) xmalloc(sizeof(in_cell));
temp->index = 0;
temp->value = val;
temp->actual = N;
temp->ptr = L;
return temp;
}

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

in_list
in_list2 (int val, in_list N, int val1, in_list N1)
{
in_list temp, temp1;
temp = (in_list) xmalloc(sizeof(in_cell));
temp1 = (in_list) xmalloc(sizeof(in_cell));
temp->index = 0;
temp->value = val;
temp->actual = N;
temp->ptr = temp1;
temp1->index = 0;
temp1->value = val1;
temp1->actual = N1;
temp1->ptr = NULL;
return temp;
}

in_list
in_append (in_list K, in_list L)
{
if (K==NULL) return L;
return in_cons(K->value, K->actual, in_append(K->ptr, L));
}

in_list
in_append1 (in_list L, int val, in_list N)
{
return (in_append(L,in_list1(val, N)));
}

/* Attribute-value list functions */

hash_list
hash_cons (int x, int y, int new_class, int lig_z, hash_list L)
{
hash_list temp;
temp = (hash_list) xmalloc(sizeof(hash_cell));
temp->x = x;
temp->y = y;
temp->new_class = new_class;
temp->lig_z = lig_z;
temp->ptr = L;
return temp;
}

hash_list
hash_list1 (int x, int y, int new_class, int lig_z)
{
hash_list temp;
temp = (hash_list) xmalloc(sizeof(hash_cell));
temp->x = x;
temp->y = y;
temp->new_class = new_class;
temp->lig_z = lig_z;
temp->ptr = NULL;
return temp;
}

hash_list
hash_list2(int x,int y,int new_class,int lig_z,int x1,int y1,int class1,int lig_z1)
{
hash_list temp, temp1;
temp = (hash_list) xmalloc(sizeof(hash_cell));
temp1 = (hash_list) xmalloc(sizeof(hash_cell));
temp->x = x;
temp->y = y;
temp->new_class = new_class;
temp->lig_z = lig_z;
temp->ptr = temp1;
temp1->x = x1;
temp1->y = y1;
temp1->new_class = class1;
temp1->lig_z = lig_z1;
temp1->ptr = NULL;
return temp;
}

hash_list
hash_append (hash_list K, hash_list L)
{
if (K==NULL) return L;
return hash_cons(K->x, K->y, K->new_class, K->lig_z, hash_append(K->ptr, L));
}

hash_list
hash_append1 (hash_list L, int x, int y, int new_class, int lig_z)
{
return (hash_append(L,hash_list1(x, y, new_class, lig_z)));
}

void
init_queue(queue *q)
{
    q->front = NULL;
    q->tail = NULL;
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
