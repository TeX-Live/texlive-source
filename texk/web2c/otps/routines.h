/* routines.h: Generating the finite state automaton

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

#ifdef HAVE_CONFIG_H
#include <config.h>
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

extern int yyparse();
extern int yylex();

extern int line_number;
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

typedef struct lcell_struct {struct lcell_struct *ptr;
        left val; } lcell;
typedef lcell *llist;

extern list cons();
extern list list1();
extern list list2();
extern list append();
extern list append1();

extern llist lcons();
extern llist llist1 P1H(left);
extern llist llist2();
extern llist lappend P2H(llist,llist);
extern llist lappend1 P2H(llist,left);

extern left WildCard();
extern left SingleLeft P1H(int);
extern left StringLeft P1H(char *);
extern left DoubleLeft P2H(int,int);
extern left ChoiceLeft P1H(llist);
extern left NotChoiceLeft P1H(llist);
extern left PlusLeft P2H(left,int);
extern left CompleteLeft P3H(left,int,int);
extern left BeginningLeft();
extern left EndLeft();
extern list gen_left();
extern void fill_in_left();
extern void out_left P1H(llist);
extern int no_lefts;

extern void store_alias P2H(string, left);
extern left lookup_alias P1H(string);

#define ARRAY_SIZE 10000
typedef struct {
	int length;
	char * str;
	int table[ARRAY_SIZE];
} table_type;

extern int no_tables;
extern int cur_table;
extern int room_for_tables;
extern table_type tables[];
extern void add_to_table P1H(int);

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
extern void add_to_state P1H(int);

extern void fill_in P1H(list);
extern void right_int();
extern void out_int P2H(int,int);
extern void out_right P2H(int,int);

extern void store_state P1H(string);
extern int lookup_state P1H(string);
extern void store_table P2H(string, int);
extern int lookup_table P1H(string);

typedef struct { char * str; left left_val; } alias_pair;

extern alias_pair aliases[];

extern int right_offset;
extern int input_bytes;
extern int output_bytes;

