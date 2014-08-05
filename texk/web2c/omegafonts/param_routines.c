/* param_routines.c: Data structures for parameter lists.

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
#include "param_routines.h"
#include "print_routines.h"
#include "out_routines.h"
#include "error_routines.h"
#include "out_ofm.h"

#define PARAM_MIN 1

av_list param_list = NULL;
int param_max = PARAM_MIN-1;
int param_started = FALSE;

unsigned np=0;

void
init_parameters(void)
{
    if (param_started==TRUE)
    warning_0("FONTDIMEN previously defined;  all old parameters ignored");
    if (param_list!=NULL) {
        av_list L1 = param_list, L2;
        while (L1 != NULL) {
	    L2 = L1->ptr;
	    free(L1);
	    L1 = L2;
        }
    }
    param_list = NULL;
    param_max = PARAM_MIN-1;
    np=param_max;
    param_started = TRUE;
}

void
set_param_word(int index, int val)
{
    av_list L1, L2;

    if (index < PARAM_MIN) {
        warning_0("PARAMETER index must be at least 1; ignored");
        return;
    }
    L1 = param_list;
    if (L1 == NULL) {
        param_list = av_list1(index, val);
        param_max = index;
        np=param_max;
    } else {
        L2 = L1->ptr;
        while ((L2 != NULL) && (lattr(L2) <= index)) {
            L1 = L2;
            L2 = L2->ptr;
        }
        if (index < lattr(L1)) {
            param_list = av_list1(index, val);
            param_list->ptr = L1;
	} else if (index == lattr(L1)) {
        warning_1("PARAMETER index (%d) previously defined; "
                  "old value ignored", index);
            lval(L1)  = val;
        } else {
            if (L2==NULL) {param_max=index; np=param_max;}
            L2 = av_list1(index, val);
            L2->ptr = L1->ptr;
            L1->ptr = L2;
        }
    }
}

void
retrieve_parameters(unsigned char *table)
{
    int value;
    unsigned i;
    unsigned np_prime = np;

    param_list = NULL;
    for (i=1; i<=np_prime; i++) {
        value = ((table[4*i] & 0xff) << 24) |
                ((table[4*i+1] & 0xff) << 16) |
                ((table[4*i+2] & 0xff) << 8) |
                (table[4*i+3] & 0xff);
        if ((i<=7) | (value != 0)) {
           set_param_word(i, value);
        }
    }
    np = np_prime;
}

void
print_parameters(void)
{
    av_list L = param_list;

    if (L == NULL)
        return;

    print_font_dimension();
    while (L != NULL) {
	print_parameter(lattr(L), lval(L));
	L = L->ptr;
    }
    right();
}

static void
output_ofm_one_parameter(unsigned i, fix param)
{
    if (i==P_SLANT) out_ofm_4(param);
    else out_ofm_scaled(param);
}

void
output_ofm_parameter(void)
{
    unsigned i=1, j;

    av_list L = param_list;

    while(L != NULL) {
       j=lattr(L);
       while (i<j) {
           output_ofm_one_parameter(i, 0);
           i++;
       }
       output_ofm_one_parameter(i, lval(L));
       L = L->ptr; i++;
    }
}
