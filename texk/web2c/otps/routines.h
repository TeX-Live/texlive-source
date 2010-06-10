/* routines.h: Generating the finite state automaton

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous
Copyright (C) 2005  Roozbeh Pournader

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

#ifdef HAVE_CONFIG_H
#include <w2c/config.h>
#endif
#ifdef KPATHSEA
#include <kpathsea/config.h>
#include <kpathsea/c-memstr.h>
#include <kpathsea/c-std.h>
#else /* !KPATHSEA */
#include <stdio.h>
#include <string.h>
#ifdef __STDC__
#include <stdlib.h>
#else
extern void exit();
#endif
#endif /* KPATHSEA */

#define nil 0

#define WILDCARD 0
#define STRINGLEFT 1
#define SINGLELEFT 2
#define DOUBLELEFT 3
#define CHOICELEFT 4
#define NOTCHOICELEFT 5
#define PLUSLEFT 6
#define COMPLETELEFT 7
#define BEGINNINGLEFT 8
#define ENDLEFT 9

extern int yyparse(void);
extern int yylex(void);
extern int yywrap(void);

extern int line_number;

/* linked list of ints */
typedef struct cell_struct {struct cell_struct *ptr;
        int val; } cell;
typedef cell *list;

typedef struct left_cell {
	int kind;
	int val1, val2;
	char *valstr;
        struct lcell_struct *more_lefts;
	struct left_cell *one_left;
} lft_cell;
typedef lft_cell *left;

/* linked list of lefts */
typedef struct lcell_struct {struct lcell_struct *ptr;
        left val; } lcell;
typedef lcell *llist;

extern list cons(int x, list L);
extern list list1(int x);
extern list list2(int x, int y);
extern list append(list K, list L);
extern list append1(list L, int x);

extern llist lcons(left x, llist L);
extern llist llist1(left x);
extern llist llist2(left x, left y);
extern llist lappend(llist K, llist L);
extern llist lappend1(llist L, left x);

extern left WildCard(void);
extern left StringLeft(char *x);
extern left SingleLeft(int x);
extern left DoubleLeft(int x, int y);
extern left ChoiceLeft(llist L);
extern left NotChoiceLeft(llist L);
extern left PlusLeft(left l, int n);
extern left CompleteLeft(left l, int n, int m);
extern left BeginningLeft(void);
extern left EndLeft(void);
extern list gen_left(left arg);

extern int no_lefts;
extern void store_alias(string str, left l);
extern left lookup_alias(string str);
extern void out_left(llist L);
extern void fill_in_left(void);

#define ARRAY_SIZE 50000
typedef struct {
	int length;
	char * str;
	int table[ARRAY_SIZE];
} table_type;

extern int no_tables;
extern int cur_table;
extern int room_for_tables;
extern table_type tables[];
extern void add_to_table(int x);

typedef struct {
	int length;
	char * str;
	int no_exprs;
	int instrs[ARRAY_SIZE];
} state_type;

extern int no_states;
extern int cur_state;
extern int room_for_states;
extern state_type states[];
extern void add_to_state(int x);

extern void fill_in(list L);
extern void out_int(int instr, int val);
extern void out_right(int instr, int val);

extern void store_state(const_string str);
extern int lookup_state(string str);
extern void store_table(string str, int len);
extern int lookup_table(string str);

typedef struct { char * str; left left_val; } alias_pair;

extern alias_pair aliases[];

extern int right_offset;
extern int input_bytes;
extern int output_bytes;

