/* ligkern_routines.c: The ligature/kerning table.

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
#include "manifests.h"
#include "list_routines.h"
#include "ligkern_routines.h"
#include "char_routines.h"
#include "out_routines.h"
#include "print_routines.h"
#include "error_routines.h"
#include "header_routines.h"
#include "out_ofm.h"

unsigned nk=0;
unsigned nl=0;

fix *kern_table;
av_list kern_list = NULL;
unsigned no_kerns = 0;

unsigned min_nl=0;
unsigned bchar = CHAR_BOUNDARY;
unsigned bchar_label = 0;
unsigned bchar_remainder;
unsigned lk_step_ended=FALSE;

four_entries lig_kern_table[100000];

void
set_boundary_character(unsigned c)
{
    /* What checks are required ? */
    bchar = c;
}

void
init_ligkern(void)
{
    lk_step_ended = FALSE;
    nl = 0;
    min_nl = 0;
}

void
set_label_command(unsigned c)
{
    if (c==CHAR_BOUNDARY) { /* BOUNDARYCHAR */
        bchar_label = nl;
    } else {
        check_char_tag(c);
	set_char_tag(c, TAG_LIG);
        if (nl==0) set_char_remainder(c, 0);
	else set_char_remainder(c, nl);
    }
    if (min_nl <= nl) { min_nl = nl+1; }
    lk_step_ended = FALSE;
    no_labels++;
}

void
set_stop_command(void)
{
    if (lk_step_ended == TRUE) {
        lig_kern_table[nl-1].entries[0] =
        lig_kern_table[nl-1].entries[0] / 256 * 256 + STOP_FLAG;
    } else {
        warning_0("STOP must follow LIG or KRN; ignored");
    }
    lk_step_ended = FALSE;
}

void
set_skip_command(unsigned val)
{
    if (lk_step_ended == TRUE) {
        if (val>128) {
            warning_0("Maximum SKIP amount is 127; ignored");
        } else {
            lig_kern_table[nl-1].entries[0] = val;
            if (min_nl <= (nl+val)) { min_nl = nl+val+1; }
        }
    } else {
        warning_0("SKIP must follow LIG or KRN; ignored");
    }
    lk_step_ended = FALSE;
}

void
set_ligature_command(unsigned lig, unsigned c, unsigned val)
{
    lig_kern_table[nl].entries[0] = 0;
    lig_kern_table[nl].entries[1] = c;
    lig_kern_table[nl].entries[2] = lig;
    lig_kern_table[nl].entries[3] = val;
    nl++;
    lk_step_ended = TRUE;

}

void
set_kerning_command(unsigned c, fix fval)
{
    unsigned k = set_new_kern(fval);

    lig_kern_table[nl].entries[0] = 0;
    lig_kern_table[nl].entries[1] = c;
    if (ofm_level==OFM_TFM) {
        lig_kern_table[nl].entries[2] = KERN_FLAG + (k/256);
        lig_kern_table[nl].entries[3] = k % 256;
    } else {
        lig_kern_table[nl].entries[2] = KERN_FLAG + (k/65536);
        lig_kern_table[nl].entries[3] = k % 65536;
    }
    nl++;
    lk_step_ended = TRUE;
}

void
set_c_label_command(unsigned new_class)
{
    fatal_error_0("CLABEL not currently supported");
}

void
set_c_kerning_command(unsigned new_class, fix fval)
{
    fatal_error_0("CKRN not currently supported");
}

void
set_c_glue_command(unsigned new_class, unsigned glue_index)
{
    fatal_error_0("CGLUE not currently supported");
}

void
set_c_penalty_command(unsigned new_class, unsigned pen_index)
{
    fatal_error_0("CPEN not currently supported");
}

void
set_c_penglue_command(unsigned new_class, unsigned pen_index, unsigned glue_index)
{
    fatal_error_0("CPENGLUE not currently supported");
}

int
set_new_kern(fix fval)
{
    unsigned index;
    av_list L1, L2;

    L1 = kern_list;
    if (L1 == NULL) {
        index = nk++;
        kern_list = av_list1(index, fval);
    } else {
        L2 = L1->ptr;
        while ((lval(L1) != fval) && (L2 != NULL)) {
            L1 = L2;
            L2 = L2->ptr;
        }
        if (fval == lval(L1)) {
            index = lattr(L1);
        } else {
            index = nk++;
            L2 = av_list1(index, fval);
            L1->ptr = L2;
        }
    }
    return index;
}


void
build_kern_table(void)
{
    av_list L1 = kern_list, L2;
    unsigned i = 0;

    kern_table = (fix *) xmalloc((nk+1)*sizeof(int));
    while (L1 != NULL) {
        kern_table[i] = lval(L1);
        L2 = L1->ptr;
        free(L1); L1 = L2;
        i++;
    }
}

void
print_kerns(void)
{
    unsigned i;

    if (nk>0) {
        left(); out("COMMENT"); out_ln();
        for (i=0; i<nk; i++) {
            left(); out("KERN_ENTRY");
            out(" "); out_int(i,10);
            out(" "); out_fix(kern_table[i]);
            right();
        }
        right();
    }
}

void
print_ligkern_table(void)
{
    unsigned i;

    sort_ptr = 1;
    if (nl>0) {
        left(); out("LIGTABLE"); out_ln();
        for (i=0; i<nl; i++) {
            while ((sort_ptr<=label_ptr) && (i==label_table[sort_ptr].rr)) {
                print_label_command(label_table[sort_ptr].cc);
                sort_ptr++;
            }
            print_one_lig_kern_entry(lig_kern_table+i, TRUE);
        }
        right();
    }
}

void
print_one_lig_kern_entry(four_entries *lentry, boolean show_stop)
{
    if (lentry->entries[2] >= KERN_FLAG) {
        if (ofm_level==OFM_TFM) {
        print_kerning_command(lentry->entries[1],
        kern_table[256*(lentry->entries[2]-KERN_FLAG)+lentry->entries[3]]);
        } else {
        print_kerning_command(lentry->entries[1],
        kern_table[65536*(lentry->entries[2]-KERN_FLAG)+lentry->entries[3]]);
        }
    } else {
        print_ligature_command(lentry->entries[2],
                               lentry->entries[1],
                               lentry->entries[3]);
    }
    if ((show_stop == TRUE) && (lentry->entries[0] > 0)) {
        if (lentry->entries[0] >= STOP_FLAG) {
            print_stop_command();
        } else {
            print_skip_command(0);
        }
    }
}

hash_list hash_table[PRIME];

unsigned x_lig_cycle;
unsigned y_lig_cycle = CHAR_BOUNDARY;

int
l_f(hash_list h, unsigned x, unsigned y)
{
    switch(h->new_class) {
         case LIG_SIMPLE: {break;}
         case LIG_LEFT_Z: {
             h->new_class = LIG_PENDING;
             h->lig_z = l_eval(h->lig_z, y);
             h->new_class = LIG_SIMPLE;
             break;
         }
         case LIG_RIGHT_Z: {
             h->new_class = LIG_PENDING;
             h->lig_z = l_eval(x, h->lig_z);
             h->new_class = LIG_SIMPLE;
             break;
         }
         case LIG_BOTH_Z: {
             h->new_class = LIG_PENDING;
             h->lig_z = l_eval(l_eval(x,h->lig_z), y);
             h->new_class = LIG_SIMPLE;
             break;
         }
         case LIG_PENDING: {
             x_lig_cycle = x;
             y_lig_cycle = y;
             h->lig_z = CHAR_ERROR;
             h->new_class = LIG_SIMPLE;
             break;
         }
         default: {
             internal_error_1("f (new_class=%d)", h->new_class);
         }
    }
    return (h->lig_z);
}

int
l_eval(unsigned x, unsigned y)
{
    hash_list h;

    if ((x==CHAR_ERROR) || (y==CHAR_ERROR)) return CHAR_ERROR;
    h = l_hash_lookup(x, y);
    if (h == NULL) return y;
    return l_f(h, x, y);
}

queue hash_entries;

int
l_hash_input(unsigned p, unsigned c)
{

    four_entries *entry = lig_kern_table+p;
    unsigned y = entry->entries[1];
    unsigned t = entry->entries[2];
    unsigned cc = LIG_SIMPLE;
    unsigned zz = entry->entries[3];
    unsigned key;
    hash_list L1, L2;

    if (t >= KERN_FLAG) zz = y;
    else {
        switch(t) {
             case L_0:
             case L_Ax:   { break; }
             case L_Bx:
             case L_ABxx: { zz = y; break; }
             case L_B:
             case L_ABx:  { cc = LIG_LEFT_Z;  break; }
             case L_A:    { cc = LIG_RIGHT_Z; break; }
             case L_AB:   { cc = LIG_BOTH_Z;  break; }
             default: {
                 internal_error_1("l_hash_input (case=%d)", t);
             }
        }
    }
    key = (c & 0x7fff)*(y & 0x7fff) % PRIME;
    if (hash_table[key] == NULL) {
        hash_table[key] = hash_list1(c,y,cc,zz);
        append_to_queue(&hash_entries, hash_table[key]);
    } else {
        L1 = hash_table[key];
        L2 = L1->ptr;
        while ((L2 != NULL) &&
               ((L2->x <= c) || ((L2->x == c) && (L2->y <= y)))) {
            L1 = L2;
            L2 = L2->ptr;
        }
        if (L2 == NULL) {
            L2 = hash_list1(c,y,cc,zz);
            append_to_queue(&hash_entries, L2);
            L2->ptr = L1->ptr;
            L1->ptr = L2;
        } else if ((c < L2->x) || ((c == L2->x) && (y < L2->y))) {
            hash_table[key] = hash_list1(c,y,cc,zz);
            hash_table[key]->ptr = L1;
            append_to_queue(&hash_entries, hash_table[key]);
        } else if ((c == L2->x) && (y < L2->y)) {
            return FALSE; /* unused ligature command */
        } else {
            L2 = hash_list1(c,y,cc,zz);
            append_to_queue(&hash_entries, L2);
            L2->ptr = L1->ptr;
            L1->ptr = L2;
        }
    }
    return TRUE;
}

hash_list
l_hash_lookup(unsigned x, unsigned y)
{
    unsigned key = (x & 0x7fff)*(y & 0x7fff) % PRIME;
    hash_list L = hash_table[key];

    if (L==NULL) return NULL;
    while (L->x < x) L = L->ptr;
    if (L->x != x) return NULL;
    while ((L->x == x) && (L->y < y)) L = L->ptr;
    if (L->y != y) return NULL;
    return L;
}

void
check_ligature_ends_properly(void)
{
    if (nl>0) {
        if (bchar_label != 0) {
            /* make room for it; the actual label will be stored later */
            nl++;
        }
        while (min_nl > nl) {
            nl++;
        }
        if (lig_kern_table[nl].entries[0] == 0) {
            lig_kern_table[nl].entries[0] = STOP_FLAG;
        }
    }
}

void
check_ligature_program(unsigned c, unsigned lab)
{
    unsigned lig_ptr = lab;
    four_entries *entry;

    while (lig_ptr < nl) {
        entry = lig_kern_table+lig_ptr;
        if (l_hash_input(lig_ptr,c)) {
            if (entry->entries[2] < KERN_FLAG) {
                if (entry->entries[1] != bchar) {
                    check_existence_and_safety(c, entry->entries[1],
                        ligature_commands[entry->entries[2]],
                        "%s character examined by (H %X)");
                    if (entry->entries[1] > 0xffff) {
                        fprintf(stderr, "%s character (H %X) examined by (H %X) "
                            "exceeds ffff\n", ligature_commands[entry->entries[2]], entry->entries[1], c);
                        exit(2);
                    }
                }
                if (entry->entries[3] >= 128)
/* Needs fixing */
                    if ((c < 128) || (c == -1))
                        if ((entry->entries[1] < 128) ||
                            (entry->entries[1] == bchar))
                            seven_bit_calculated = 0;
                check_existence_and_safety(c, entry->entries[3],
                    ligature_commands[entry->entries[2]],
                    "%s character generated by (H %X)");
                if (entry->entries[3] >0xffff) {
                    fprintf(stderr, "%s character (H %X) generated by (H %X) "
                        "exceeds ffff\n", ligature_commands[entry->entries[2]], entry->entries[3], c);
                    exit(2);
                }
            } else {
                check_existence_and_safety(c, entry->entries[1],
                    "KRN", "%s character examined by (H %X)");
                if (entry->entries[1] > 0xffff) {
                    fprintf(stderr, "KRN character (H %X) examined by (H %X) "
                        "exceeds ffff\n", entry->entries[1], c);
                    exit(2);
                }
            }
        }
        if (entry->entries[0] >= STOP_FLAG) lig_ptr = nl;
        else lig_ptr = lig_ptr + 1 + entry->entries[0];
    }
}

void
check_ligature_infinite_loops(void)
{
    list entry = hash_entries.front;
    hash_list tt;

    while (entry != NULL) {
        tt = (hash_list) entry->contents;
        if (tt->new_class > LIG_SIMPLE)
             l_f(tt, tt->x, tt->y);
        entry = entry->ptr;
    }
    if (y_lig_cycle != 0x80000000) {
        if (x_lig_cycle == CHAR_BOUNDARY) {
            warning_1("Infinite ligature loop starting with boundary and %d",
                      y_lig_cycle);
        } else {
            warning_2("Infinite ligature loop starting with %d and %d",
                      x_lig_cycle, y_lig_cycle);
        }
        clear_ligature_entries();
        nl = 0; bchar = CHAR_BOUNDARY; bchar_label = 0;
    }
}

void
doublecheck_ligatures(void)
{
    unsigned i;

    if (nl>0) {
        for (i=0; i<nl; i++) {
            if (lig_kern_table[i].entries[2] < KERN_FLAG) {
                if (lig_kern_table[i].entries[0] != CHAR_BOUNDARY) {
                    doublecheck_existence(
                        lig_kern_table[i].entries[1],
                        ligature_commands[lig_kern_table[i].entries[2]],
                        "Unused %s step refers to nonexistent character (H %X)");
                    doublecheck_existence(
                        lig_kern_table[i].entries[3],
                        ligature_commands[lig_kern_table[i].entries[2]],
                        "Unused %s step refers to nonexistent character (H %X)");
                }
            } else {
                doublecheck_existence(
                    lig_kern_table[i].entries[1],
                    "KRN",
                    "Unused %s step refers to nonexistent character (H %X)");
            }
        }
    }
}

void
output_ofm_ligkern(void)
{
    unsigned i;
    four_entries *entry;

    if (ofm_level == OFM_TFM) {
        for (i=0; i<nl; i++) {
            entry = lig_kern_table+i;
            out_ofm(entry->entries[0] & 0xff);
            out_ofm(entry->entries[1] & 0xff);
            out_ofm(entry->entries[2] & 0xff);
            out_ofm(entry->entries[3] & 0xff);
        }
    } else {
        for (i=0; i<nl; i++) {
            entry = lig_kern_table+i;
            out_ofm_2(entry->entries[0] & 0xffff);
            out_ofm_2(entry->entries[1] & 0xffff);
            out_ofm_2(entry->entries[2] & 0xffff);
            out_ofm_2(entry->entries[3] & 0xffff);
        }
    }
    for (i=0; i<nk; i++) {
        out_ofm_4(kern_table[i]);
    }
}

void
retrieve_ligkern_table(unsigned char *ofm_lig_table,
                       unsigned char *ofm_kern_table)
{
    unsigned i;
    four_entries *entry;
    unsigned char *table_entry;

    if (ofm_level == OFM_TFM) {
        for (i=0; i<nl; i++) {
            entry = lig_kern_table+i;
            table_entry = ofm_lig_table+(4*i);
            entry->entries[0] = (*table_entry) & 0xff;
            entry->entries[1] = (*(table_entry+1)) & 0xff;
            entry->entries[2] = (*(table_entry+2)) & 0xff;
            entry->entries[3] = (*(table_entry+3)) & 0xff;
        }
    } else {
        for (i=0; i<nl; i++) {
            entry = lig_kern_table+i;
            table_entry = ofm_lig_table+(8*i);
            entry->entries[0] = (((*table_entry) & 0xff) * 0x100)
                                + ((*(table_entry+1)) & 0xff);
            entry->entries[1] = (((*(table_entry+2)) & 0xff) * 0x100)
                                + ((*(table_entry+3)) & 0xff);
            entry->entries[2] = (((*(table_entry+4)) & 0xff) * 0x100)
                                + ((*(table_entry+5)) & 0xff);
            entry->entries[3] = (((*(table_entry+6)) & 0xff) * 0x100)
                                + ((*(table_entry+7)) & 0xff);
        }
    }
    kern_table = (fix *) xmalloc((nk+1)*sizeof(int));
    for (i=0; i<nk; i++) {
        table_entry = ofm_kern_table+(4*i);
        kern_table[i] = (((*table_entry) & 0xff) << 24) |
                        (((*(table_entry+1)) & 0xff) << 16) |
                        (((*(table_entry+2)) & 0xff) << 8) |
                        ((*(table_entry+3)) & 0xff);
    }
}
