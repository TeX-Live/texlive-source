/* dimen_routines.c: Data structures for holding widths, heights, etc.

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
#include "manifests.h"
#include "header_routines.h"
#include "dimen_routines.h"
#include "char_routines.h"
#include "out_routines.h"
#include "error_routines.h"
#include "out_ofm.h"
#include "parse_ofm.h"
#include "omfonts.h"

int *measure_max_entries;
int  TFM_measure_max_entries[] =
    {255,15,15,63, 0,0,0,0,
     0,0,0,0,      0,0,0,0,
     0,0,0,0,      0,0,0,0};
int  OFM0_measure_max_entries[] =
    {65535,255,255,255, 0,0,0,0,
     0,0,0,0,           0,0,0,0,
     0,0,0,0,           0,0,0,0};
int  OFM2_measure_max_entries[] =
    {255,255,255,255, 256,256,256,256,
     256,256,256,256, 256,256,256,0,
     256,256,256,256, 256,256,256,0};

in_list measure_list[C_MAX+1];
int measure_max[C_MAX+1];
int *dimen_tables[C_MAX+1];
unsigned char *measure_base[C_MAX+1];

unsigned nw=0;
unsigned nh=0;
unsigned nd=0;
unsigned ni=0;

void
init_measures(void)
{
    register int i;
    for (i=C_MIN; i<=C_MAX; i++) {
        measure_list[i] = in_list1(WEB_INFINITY, NULL);
        measure_max[i] = 0;
    }
}

void
set_character_measure(int index, int val)
{
    in_list L1, L2, *the_list;

    if ((index < C_MIN) || (index > C_MAX)) {
        internal_error_1("set_character_measure (index=%d)", index);
        return;
    }
    if ((val == 0) && (index > C_WD) && (index <= C_IC))
        return;
    the_list = measure_list+index;
    L1 = *the_list;
    if (L1 == NULL) {
	internal_error_0("set_character_measure (L1)");
    } else {
        L2 = L1->ptr;
        while ((L2 != NULL) && (lval(L2) <= val)) {
            L1 = L2;
            L2 = L2->ptr;
        }
        if (val < lval(L1)) {
            *the_list = in_list1(val, NULL);
            (*the_list)->ptr = L1;
            (*the_list)->actual = *the_list;
            measure_max[index]++;
            current_character->indices[index] = *the_list;
        } else if (val == lval(L1)) {
            current_character->indices[index] = L1;
        } else {
            L2 = in_list1(val, NULL);
            L2->ptr = L1->ptr;
            L2->actual = L2;
            L1->ptr = L2;
            measure_max[index]++;
            current_character->indices[index] = L2;
        }
    }
}

int next_d;
int excess;

static int
min_cover(int h, int d)
{
    in_list L = measure_list[h];
    int m=0; /* the current size of the cover being generated */
    int l;   /* the least element covered by the current interval */

    m = 0; next_d = WEB_INFINITY;
    while (lval(L) != WEB_INFINITY) {
        m++; l = lval(L);
        while (lval(L->ptr) <= (l+d)) L = L->ptr;
	L = L->ptr;
        if ((lval(L) - l) < next_d) next_d = lval(L) - l;
    }
    return m;
}

/* finds best way to round */

static int
shorten(int h, int m)
{
    int d=0; /* the current trial interval length */
    int k; /* the size of a minimum cover */
    int M = measure_max[h];

    if (M>m) {
        excess = M-m;
        k = min_cover(h,0); d = next_d;
        /* now the answer is at least d*/
        /* first we ascend rapidly until finding the range */
        do {
            d = d+d; k = min_cover(h,d);
        } while (k>m);
        /* now we run through the feasible steps */
        d = d/2; k=min_cover(h,d);
        while (k>m) {
            d = next_d; k = min_cover(h,d);
        }
    }
    return d;
}

/* reduces and indexes a list */

static void
set_indices(int h, int d)
{
    in_list L1;     /* the current node of interest */
    in_list L2;     /* trails one step behind L1 */
    int l;          /* least value in the current interval */
    int lprime;     /* middle value for current interval */
    int m;          /* index number of nodes in the current interval */

/* How to store the information for each character ? */
    L1 = measure_list[h]; m = 0;
    while (lval(L1) != WEB_INFINITY) {
        L2 = L1;
        m++; l = lval(L1);
        while (lval(L1->ptr) <= (l+d)) {
            L1 = L1->ptr; excess--;
            if (excess==0) d = 0;
        }
        lprime = l + (lval(L1)-l) / 2;
	lval(L1) = lprime;
	L1->index = m;
	while (L2 != L1) {
	   lval(L2) = lprime;
	   L2->actual = L1;
	   L2->index = m;
	   L2 = L2->ptr;
	}
        L1 = L1->ptr;
    }
    measure_max[h] = m;
}

void
build_dimen_tables(void)
{
    int delta;
    in_list L1;
    int i,j;

    switch(ofm_level) {
      case OFM_TFM:
        measure_max_entries = TFM_measure_max_entries;
        break;
      case OFM_LEVEL0: case OFM_LEVEL1:
        measure_max_entries = OFM0_measure_max_entries;
        break;
      default:
        measure_max_entries = OFM2_measure_max_entries;
    }
    for (i=C_MIN; i<=C_MAX; i++) {
        if (measure_max_entries[i] != 0) {
            delta=shorten(i,measure_max_entries[i]);
            set_indices(i,delta);
            dimen_tables[i] = (int *) xmalloc((measure_max[i]+1)*sizeof(int));
            L1 = measure_list[i];
            j=0;
            while (lval(L1) != WEB_INFINITY) {
                L1 = L1->actual;
                dimen_tables[i][j] = lval(L1);
                L1 = L1->ptr;
                j++;
            }
        }
    }
    nw = measure_max[C_WD];
    nh = measure_max[C_HT];
    nd = measure_max[C_DP];
    ni = measure_max[C_IC];
}

void
retrieve_dimen_tables(void)
{
    unsigned i, j;
    unsigned char *k;

    measure_max[C_WD] = nw;
    measure_max[C_HT] = nh;
    measure_max[C_DP] = nd;
    measure_max[C_IC] = ni;
    measure_base[C_WD] = ofm+width_base*4;
    measure_base[C_HT] = ofm+height_base*4;
    measure_base[C_DP] = ofm+depth_base*4;
    measure_base[C_IC] = ofm+italic_base*4;
    for (i=C_IC+1; i<=C_MAX; i++) {
        measure_max[i] = 0;
        dimen_tables[i] = NULL;
    }
    for (i=C_MIN; i<=C_MAX; i++) {
        dimen_tables[i] = (int *) xmalloc((measure_max[i]+1)*sizeof(int));
        for (j=0; j<measure_max[i]; j++) {
            k = measure_base[i]+4*j;
            dimen_tables[i][j] = (((*k)&0xff) << 24) |
                                 (((*(k+1))&0xff) << 16) |
                                 (((*(k+2))&0xff) << 8) |
                                 ((*(k+3))&0xff);
        }
    }
}

void
print_dimen_tables(void)
{
    int i, j;

    for (i=C_MIN; i<=C_MAX; i++) {
        if (measure_max[i] != 0) {
            left(); out("COMMENT"); out_ln();
            for (j=0; j<measure_max[i]; j++) {
                left();
                out_character_measure(i); out("_ENTRY");
                out(" "); out_int(j,10);
                out(" "); out_fix(dimen_tables[i][j]);
                right();
            }
            right();
        }
    }
}

void
set_accent(unsigned kind)
{
    if ((kind<ACC_TOP) || (kind>ACC_BOT)) {
        warning_0("bad ACCENT value; ignored");
        kind = ACC_NONE;
    }
    current_character->accent = kind;
}

void
output_ofm_dimension(void)
{
    int i, j;

    for (i=C_WD; i<=C_IC; i++) {
        out_ofm_4(0);
        for (j=0; j<measure_max[i]; j++) {
            out_ofm_4(dimen_tables[i][j]);
        }
    }
}
