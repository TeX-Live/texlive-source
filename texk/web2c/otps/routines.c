/* routines.c: Generating the finite state automaton.

This file is part of Omega,
which is based on the web2c distribution of TeX.

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous
Copyright (C) 2005, 2006  Roozbeh Pournader

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

#include <kpathsea/config.h>
#include <kpathsea/types.h>
#include <kpathsea/c-std.h>
#include "routines.h"
#include "otp.h"

#define out_ptr (states[cur_state].length)
#define out_array (states[cur_state].instrs)

list left_false_holes;

int left_state;
int right_offset;
int input_bytes;
int output_bytes;

int no_states = 0;
int cur_state = 0;
int room_for_states = 0;
state_type states[OTP_MAXSTATES];

int no_tables = 0;
int cur_table = 0;
int room_for_tables = 0;
table_type tables[OTP_MAXTABLES];

int no_aliases = 0;
alias_pair aliases[OTP_MAXALIASES];

/* cons = [x] + L */
list
cons(int x, list L)
{
list temp;
temp = (list) malloc(sizeof(cell));
temp->val = x;
temp->ptr = L;
return temp;
}

/* list1 = [x] */
list
list1(int x)
{
list temp;
temp = (list) malloc(sizeof(cell));
temp->val = x;
temp->ptr = nil;
return temp;
}

/* list2 = [x, y] */
list
list2(int x, int y)
{
list temp, temp1;
temp = (list) malloc(sizeof(cell));
temp1 = (list) malloc(sizeof(cell));
temp->val = x;
temp->ptr = temp1;
temp1->val = y;
temp1->ptr = nil;
return temp;
}

/* append = copy(K) + L */
list
append(list K, list L)
{
if (K==nil) return L;
return cons(K->val, append(K->ptr, L));
}

/* append1 = copy(L) + [x] */
list
append1(list L, int x)
{
return (append(L,list1(x)));
}

/* lcons = [x] + L */
llist
lcons(left x, llist L)
{
llist temp;
temp = (llist) malloc(sizeof(lcell));
temp->val = x;
temp->ptr = L;
return temp;
}

/* llist1 = [x] */
llist
llist1(left x)
{
llist temp;
temp = (llist) malloc(sizeof(lcell));
temp->val = x;
temp->ptr = nil;
return temp;
}

/* llist2 = [x, y] */
llist
llist2(left x, left y)
{
llist temp, temp1;
temp = (llist) malloc(sizeof(lcell));
temp1 = (llist) malloc(sizeof(lcell));
temp->val = x;
temp->ptr = temp1;
temp1->val = y;
temp1->ptr = nil;
return temp;
}

/* lappend = copy(K) + L */
llist
lappend(llist K, llist L)
{
if (K==nil) return L;
return lcons(K->val, lappend(K->ptr, L));
}

/* lappend1 = copy(L) + [x] */
llist
lappend1(llist L, left x)
{
return (lappend(L,llist1(x)));
}

left
WildCard(void)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=WILDCARD;
return temp;
}

left
StringLeft(char *x)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=STRINGLEFT;
temp->valstr=x;
return temp;
}

left
SingleLeft(int x)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=SINGLELEFT;
temp->val1=x;
return temp;
}

left
DoubleLeft(int x, int y)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=DOUBLELEFT;
temp->val1=x;
temp->val2=y;
return temp;
}

left
ChoiceLeft(llist L)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=CHOICELEFT;
temp->more_lefts=L;
return temp;
}

left
NotChoiceLeft(llist L)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=NOTCHOICELEFT;
temp->more_lefts=L;
return temp;
}

left
PlusLeft(left l, int n)
{
left temp;
if (n == 0) { FATAL ("plusleft's argument must be non-zero"); }
temp = (left) malloc(sizeof(lft_cell));
temp->kind=PLUSLEFT;
temp->one_left=l;
temp->val1=n;
return temp;
}

left
CompleteLeft(left l, int n, int m)
{
left temp;
if (n == 0) { FATAL ("completeleft's first argument must be non-zero"); }
temp = (left) malloc(sizeof(lft_cell));
temp->kind=COMPLETELEFT;
temp->one_left=l;
temp->val1=n;
temp->val2=m;
return temp;
}

left
BeginningLeft(void)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=BEGINNINGLEFT;
return temp;
}

left
EndLeft(void)
{
left temp;
temp = (left) malloc(sizeof(lft_cell));
temp->kind=ENDLEFT;
return temp;
}

list
gen_left(left arg)
{
int save_ptr, k;
list holes, false_holes, true_holes, backup_holes;
char *runner;
llist p;

switch(arg->kind) {
case WILDCARD:
	return nil;
case STRINGLEFT:
        runner=arg->valstr;
        holes=nil;
	while (*runner) {
		out_int(OTP_GOTO_NE, *runner);
		out_int(0, 0);
		holes=cons(out_ptr-1,holes);
		runner++;
		if (*runner) {
			out_int(OTP_GOTO_NO_ADVANCE, 0);
			holes = cons(out_ptr-1, holes);
		}
	}
	return holes;
case SINGLELEFT:
	out_int(OTP_GOTO_NE, arg->val1);
	out_int(0, 0);
	return list1(out_ptr-1);
case DOUBLELEFT:
	holes = nil;
	if (arg->val1 > 0) {
		out_int(OTP_GOTO_LT, arg->val1);
		out_int(0, 0);
		holes = list1(out_ptr-1);
	}
	if (arg->val2 < 0xFFFF) {
		out_int(OTP_GOTO_GT, arg->val2);
		out_int(0, 0);
		holes = cons(out_ptr-1, holes);
	}
	return holes;
case CHOICELEFT:
	true_holes=nil;
	false_holes=nil;
	p=arg->more_lefts;
	while (p!=nil) {
		false_holes = gen_left(p->val);
		if (p->ptr) {
			out_int(OTP_GOTO, 0);
			true_holes=cons(out_ptr-1, true_holes);
			fill_in(false_holes);
		}
		p=p->ptr;
	}
	fill_in(true_holes);
	return false_holes;
case NOTCHOICELEFT:
	true_holes=nil;
	p=arg->more_lefts;
	while (p!=nil) {
		false_holes = gen_left(p->val);
		out_int(OTP_GOTO, 0);
		true_holes=cons(out_ptr-1, true_holes);
		fill_in(false_holes);
		p=p->ptr;
	}
	return true_holes;
case PLUSLEFT:
	false_holes=nil;
	true_holes=nil;
	backup_holes=nil;
	k=1;
	while (k<arg->val1) {
		holes = gen_left(arg->one_left);
		false_holes = append(false_holes, holes);
		out_int(OTP_GOTO_NO_ADVANCE, 0);
		false_holes = cons(out_ptr-1, false_holes);
		k++;
	}
	holes=gen_left(arg->one_left);
	false_holes = append(false_holes, holes);
	save_ptr = out_ptr;
	out_int(OTP_GOTO_NO_ADVANCE, 0);
	true_holes=cons(out_ptr-1, true_holes);
	backup_holes=gen_left(arg->one_left);
	out_int(OTP_GOTO, save_ptr);
	fill_in(backup_holes);
	out_int(OTP_LEFT_BACKUP, 0);
	fill_in(true_holes);
	return false_holes;
case COMPLETELEFT:
	false_holes=nil;
	true_holes=nil;
	backup_holes=nil;
	k=1;
	if (arg->val1 > arg->val2) {
		return nil;
	}
	while (k<=arg->val1) {
		holes = gen_left(arg->one_left);
		false_holes = append(false_holes, holes);
		out_int(OTP_GOTO_NO_ADVANCE, 0);
		false_holes = cons(out_ptr-1, false_holes);
		k++;
	}
	while (k<arg->val2) {
		holes = gen_left(arg->one_left);
		true_holes = append(true_holes, holes);
		out_int(OTP_GOTO_NO_ADVANCE, 0);
		backup_holes = cons(out_ptr-1, backup_holes);
		k++;
	}
	holes = gen_left(arg->one_left);
	true_holes = append(true_holes, holes);
	out_int(OTP_GOTO, out_ptr+2);
	fill_in(true_holes);
	out_int(OTP_LEFT_BACKUP, 0);
	fill_in(backup_holes);
	return false_holes;
case BEGINNINGLEFT:
	out_int(OTP_GOTO_BEG, 0);
	true_holes=list1(out_ptr-1);
	out_int(OTP_GOTO, 0);
	false_holes=list1(out_ptr-1);
	fill_in(true_holes);
	return false_holes;
case ENDLEFT:
	out_int(OTP_GOTO_END, 0);
	true_holes=list1(out_ptr-1);
	out_int(OTP_GOTO, 0);
	false_holes=list1(out_ptr-1);
	fill_in(true_holes);
	return false_holes;
default:
	FATAL1 ("unrecognized left: %d", arg->kind);
}
}

void
store_alias(string str, left l)
{
int i;
for (i=0; i<no_aliases; i++) {
    if (!strcmp(str,aliases[i].str)) {
        FATAL1 ("alias %s already defined", str);
    }
}
aliases[i].str=xstrdup(str);
aliases[i].left_val=l;
no_aliases++;
}

left
lookup_alias(string str)
{
int i;
for (i=0; i<no_aliases; i++) {
    if (!strcmp(str,aliases[i].str)) {
        return aliases[i].left_val;
    }
}
FATAL1 ("alias %s not defined", str);
}

void
out_left(llist L)
{
llist p;
list holes;
if ((states[cur_state].no_exprs)==1) {
	out_int(OTP_LEFT_START, 0);
} else {
	out_int(OTP_LEFT_RETURN, 0);
}
p=L;
left_false_holes=nil;
while (p!=nil) {
	holes = gen_left(p->val);
	if ((p->ptr != nil) &&
            ((p->val)->kind !=BEGINNINGLEFT) &&
            (((p->ptr)->val)->kind !=ENDLEFT)) {
		out_int(OTP_GOTO_NO_ADVANCE, 0);
		left_false_holes = cons(out_ptr-1,left_false_holes);
	}
	left_false_holes = append(left_false_holes, holes);
	p=p->ptr;
}
}

void
fill_in_left(void)
{
	out_int(OTP_STOP, 0);
	fill_in(left_false_holes);
}

void
fill_in(list L) 
{
list p;
p=L;
while (p!=0) {
    out_array[p->val] = out_array[p->val] + out_ptr;
    p=p->ptr;
}
}

void
out_right(int instr, int val)
{
out_int(instr+right_offset, val);
}

void
out_int(int instr, int val)
{
if (val>=(1<<24)) {
    FATAL1 ("Argument (%d) of instruction too big", val);
}
add_to_state((instr<<24)+val);
}

void
store_state(const_string str)
{
int i;
for (i=0; i<no_states; i++) {
   if (!strcmp(str,states[i].str)) {
      FATAL1 ("state %s already defined", str);
   }
}
states[i].str=xstrdup(str);
states[i].length=0;
states[i].no_exprs=0;
cur_state=i;
no_states++;
}

int
lookup_state(string str)
{
int i;
for (i=0; i<no_states; i++) {
    if (!strcmp(str,states[i].str)) {
        return i;
    }
}
FATAL1 ("state %s not defined", str);
}

void
add_to_state(int x)
{
int len;
len = states[cur_state].length;
if (len > ARRAY_SIZE) {
	char * str = states[cur_state].str;
        FATAL2 ("%s state has too many instructions: %d", str, len);
}
(states[cur_state].instrs)[len] = x;
states[cur_state].length = len+1;
}

void
store_table(string str, int len)
{
int i;
for (i=0; i<no_tables; i++) {
    if (!strcmp(str,tables[i].str)) {
        FATAL1 ("table %s already defined", str);
    }
}
tables[i].str=xstrdup(str);
tables[i].length=0;
cur_table=i;
no_tables++;
}

void
add_to_table(int x)
{
int len;
len = tables[cur_table].length;
(tables[cur_table].table)[len] = x;
tables[cur_table].length = len+1;
}

int
lookup_table(string str)
{
int i;
for (i=0; i<no_tables; i++) {
    if (!strcmp(str,tables[i].str)) {
        return i;
    }
}
FATAL1 ("table %s not defined", str);
}

