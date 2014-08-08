/* list_routines.h: Types used in this program.

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

typedef int fix;

typedef struct cell_struct {
    struct cell_struct *ptr;
    void *contents;
} cell;
typedef cell *list;

typedef struct av_cell_struct {
    struct av_cell_struct *ptr;
    int attribute;
    int value;
} av_cell;
typedef av_cell *av_list;

typedef struct in_cell_struct {
    struct in_cell_struct *ptr;
    struct in_cell_struct *actual;
    int value;
    int index;
} in_cell;
typedef in_cell *in_list;

typedef struct hash_cell_struct {
    int x;
    int y;
    int new_class;
    int lig_z;
    struct hash_cell_struct *ptr;
} hash_cell;

typedef hash_cell *hash_list;

typedef struct queue_struct {
    struct cell_struct *front;
    struct cell_struct *tail;
} queue;

extern av_list av_list1(int,int);

extern in_list in_list1(int,in_list);

extern hash_list hash_list1(int,int,int,int,hash_list);

#define lattr(L) (L->attribute)
#define lval(L) (L->value)

typedef struct four_pieces_struct {
    unsigned int pieces[4];
} four_pieces;

typedef struct four_entries_struct {
    int entries[4];
} four_entries;

#define lb0(L) (((four_entries *)((L)->contents))->entries[0])
#define lb1(L) (((four_entries *)((L)->contents))->entries[1])
#define lb2(L) (((four_entries *)((L)->contents))->entries[2])
#define lb3(L) (((four_entries *)((L)->contents))->entries[3])


extern void append_to_queue(queue *, void *);
