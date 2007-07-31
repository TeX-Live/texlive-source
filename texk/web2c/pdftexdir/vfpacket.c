/*
Copyright (c) 1996-2007 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with pdfTeX; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

$Id: vfpacket.c 136 2007-06-24 21:30:09Z ms $
*/

#include "ptexlib.h"

typedef struct {
    char *dataptr;
    integer len;
} packet_entry;

/* define packet_ptr, packet_array & packet_limit */
define_array(packet);

typedef struct {
    char **data;
    int *len;
    int char_count;
} vf_entry;

/* define vf_ptr, vf_array & vf_limit */
define_array(vf);

static char *packet_data_ptr;

integer newvfpacket(internalfontnumber f)
{
    int i, n = fontec[f] - fontbc[f] + 1;
    alloc_array(vf, 1, SMALL_ARRAY_SIZE);
    vf_ptr->len = xtalloc(n, int);
    vf_ptr->data = xtalloc(n, char *);
    vf_ptr->char_count = n;
    for (i = 0; i < n; i++) {
        vf_ptr->data[i] = NULL;
        vf_ptr->len[i] = 0;
    }
    return vf_ptr++ - vf_array;
}

void storepacket(internalfontnumber f, eightbits c, strnumber s)
{
    int l = strstart[s + 1] - strstart[s];
    vf_array[vfpacketbase[f]].len[c - fontbc[f]] = l;
    vf_array[vfpacketbase[f]].data[c - fontbc[f]] = xtalloc(l, char);
    memcpy((void *) vf_array[vfpacketbase[f]].data[c - fontbc[f]],
           (void *) (strpool + strstart[s]), (unsigned) l);
}

void startpacket(internalfontnumber f, eightbits c)
{
    packet_data_ptr = vf_array[vfpacketbase[f]].data[c - fontbc[f]];
    vfpacketlength = vf_array[vfpacketbase[f]].len[c - fontbc[f]];
}

eightbits packetbyte()
{
    vfpacketlength--;
    return *packet_data_ptr++;
}

void pushpacketstate()
{
    alloc_array(packet, 1, SMALL_ARRAY_SIZE);
    packet_ptr->dataptr = packet_data_ptr;
    packet_ptr->len = vfpacketlength;
    packet_ptr++;
}

void poppacketstate()
{
    if (packet_ptr == packet_array)
        pdftex_fail("packet stack empty, impossible to pop");
    packet_ptr--;
    packet_data_ptr = packet_ptr->dataptr;
    vfpacketlength = packet_ptr->len;
}

void vf_free(void)
{
    vf_entry *v;
    char **p;
    if (vf_array != NULL) {
        for (v = vf_array; v < vf_ptr; v++) {
            xfree(v->len);
            for (p = v->data; p - v->data < v->char_count; p++)
                xfree(*p);
            xfree(v->data);
        }
        xfree(vf_array);
    }
    xfree(packet_array);
}

/* this function was copied/borrowed/stolen from dvipdfm code */
#define SIGNED_QUAD scaled
#define fixword integer
SIGNED_QUAD sqxfw(SIGNED_QUAD sq, fixword fw)
{
    int sign = 1;
    unsigned long a, b, c, d, ad, bd, bc, ac;
    unsigned long e, f, g, h, i, j, k;
    unsigned long result;
    /* Make positive. */
    if (sq < 0) {
        sign = -sign;
        sq = -sq;
    }
    if (fw < 0) {
        sign = -sign;
        fw = -fw;
    }
    a = ((unsigned long) sq) >> 16u;
    b = ((unsigned long) sq) & 0xffffu;
    c = ((unsigned long) fw) >> 16u;
    d = ((unsigned long) fw) & 0xffffu;
    ad = a * d;
    bd = b * d;
    bc = b * c;
    ac = a * c;
    e = bd >> 16u;
    f = ad >> 16u;
    g = ad & 0xffffu;
    h = bc >> 16u;
    i = bc & 0xffffu;
    j = ac >> 16u;
    k = ac & 0xffffu;
    result = (e + g + i + (1 << 3)) >> 4u;      /* 1<<3 is for rounding */
    result += (f + h + k) << 12u;
    result += j << 28u;
    return (sign > 0) ? result : -result;
}
