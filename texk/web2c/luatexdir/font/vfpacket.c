/* vfpacket.c
   
   Copyright 1996-2006 Han The Thanh <thanh@pdftex.org>
   Copyright 2006-2008 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

#include "ptexlib.h"

static const char _svn_version[] =
    "$Id: vfpacket.c 2048 2009-03-17 22:04:00Z hhenkel $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/font/vfpacket.c $";

/*
  The |do_vf_packet| procedure is called in order to interpret the
  character packet for a virtual character. Such a packet may contain
  the instruction to typeset a character from the same or an other
  virtual font; in such cases |do_vf_packet| calls itself
  recursively. The recursion level, i.e., the number of times this has
  happened, is kept in the global variable |packet_cur_s| and should
  not exceed |packet_max_recursion|.
*/

#define packet_max_recursion 100

typedef unsigned char packet_stack_index;       /* an index into the stack */

typedef struct packet_stack_record {
    scaled stack_h;
    scaled stack_v;
} packet_stack_record;


static packet_stack_index packet_cur_s = 0;     /* current recursion level */
static packet_stack_record packet_stack[packet_max_recursion];
static packet_stack_index packet_stack_ptr = 0; /* pointer into |packet_stack| */


/* Some macros for processing character packets. */

#define do_packet_byte() vf_packets[cur_packet_byte++]

#define packet_number(fw)  {              \
  fw = do_packet_byte();                  \
  fw = fw*256 + do_packet_byte();         \
  fw = fw*256 + do_packet_byte();         \
  fw = fw*256 + do_packet_byte(); }

#define packet_scaled(a,fs) { integer fw; \
  fw = do_packet_byte();                  \
  if (fw>127) fw = fw - 256;              \
  fw = fw*256 + do_packet_byte();         \
  fw = fw*256 + do_packet_byte();         \
  fw = fw*256 + do_packet_byte();         \
    a = sqxfw(fw, fs); }


/* count the number of bytes in a command packet */
int vf_packet_bytes(charinfo * co)
{
    real_eight_bits *vf_packets;
    integer cur_packet_byte;
    unsigned k;
    int cmd;

    vf_packets = get_charinfo_packets(co);
    if (vf_packets == NULL) {
        return 0;
    }
    cur_packet_byte = 0;
    while ((cmd = vf_packets[cur_packet_byte]) != packet_end_code) {
        cur_packet_byte++;
        switch (cmd) {
        case packet_char_code:
        case packet_font_code:
        case packet_right_code:
        case packet_down_code:
        case packet_node_code:
            cur_packet_byte += 4;
            break;
        case packet_push_code:
        case packet_pop_code:
            break;
        case packet_rule_code:
            cur_packet_byte += 8;
            break;
        case packet_special_code:
            packet_number(k);   /* +4 */
            cur_packet_byte += k;
            break;
        case packet_image_code:
            cur_packet_byte += 4;
            break;
        case packet_nop_code:
            break;
        default:
            pdf_error(maketexstring("vf"),
                      maketexstring("invalid DVI command (1)"));
        }
    };
    return (cur_packet_byte + 1);
}


/* typeset the \.{DVI} commands in the
   character packet for character |c| in current font |f| */

char *packet_command_names[] = {
    "char", "font", "pop", "push", "special", "image",
    "right", "down", "rule", "node", "nop", "end", NULL
};


void do_vf_packet(internal_font_number vf_f, integer c)
{
    internal_font_number lf;
    charinfo *co;
    scaledpos save_cur;
    real_eight_bits *vf_packets;
    integer cur_packet_byte;
    integer cmd, fs_f;
    scaled i;
    unsigned k;
    str_number s;

    packet_cur_s++;
    if (packet_cur_s >= packet_max_recursion)
        overflow_string("max level recursion of virtual fonts",
                        packet_max_recursion);
    save_cur = cur;

    lf = 0;                     /* for -Wall */
    co = get_charinfo(vf_f, c);
    vf_packets = get_charinfo_packets(co);
    if (vf_packets == NULL) {
        packet_cur_s--;
        return;
    }
    cur_packet_byte = 0;
    fs_f = font_size(vf_f);
    while ((cmd = vf_packets[cur_packet_byte]) != packet_end_code) {
        cur_packet_byte++;
        /*
           if (cmd>packet_end_code) {
           fprintf(stdout, "do_vf_packet(%i,%i) command code = illegal \n", vf_f,c);
           } else {
           fprintf(stdout, "do_vf_packet(%i,%i) command code = %s\n",vf_f, c, packet_command_names[cmd]);
           }
         */
        switch (cmd) {
        case packet_font_code:
            packet_number(lf);
            break;
        case packet_push_code:
            packet_stack[packet_stack_ptr].stack_h = cur.h;
            packet_stack[packet_stack_ptr].stack_v = cur.v;
            packet_stack_ptr++;
            break;
        case packet_pop_code:
            packet_stack_ptr--;
            cur.h = packet_stack[packet_stack_ptr].stack_h;
            cur.v = packet_stack[packet_stack_ptr].stack_v;
            break;
        case packet_char_code:
            packet_number(k);
            if (!char_exists(lf, k)) {
                char_warning(lf, k);
            } else {
                if (has_packet(lf, k))
                    do_vf_packet(lf, k);
                else {
                    pos = synch_p_with_c(cur);
                    pdf_place_glyph(lf, k);
                }
            }
            cur.h = cur.h + char_width(lf, k);
            break;
        case packet_rule_code:
            packet_scaled(rule_ht, fs_f);
            packet_scaled(rule_wd, fs_f);
            if ((rule_wd > 0) && (rule_ht > 0)) {
                pos = synch_p_with_c(cur);
                pdf_place_rule(pos.h, pos.v, rule_wd, rule_ht);
            }
            cur.h = cur.h + rule_wd;
            break;
        case packet_right_code:
            packet_scaled(i, fs_f);
            cur.h = cur.h + i;
            break;
        case packet_down_code:
            packet_scaled(i, fs_f);
            cur.v = cur.v + i;
            break;
        case packet_special_code:
            packet_number(k);
            string_room(k);
            while (k > 0) {
                k--;
                append_pool_char(do_packet_byte());
            }
            s = make_string();
            literal(s, scan_special, false);
            flush_str(s);
            break;
        case packet_image_code:
            packet_number(k);
            pos = synch_p_with_c(cur);
            vf_out_image(k);
            break;
        case packet_node_code:
            packet_number(k);
            temp_ptr = k;
            pdf_hlist_out();
            break;
        case packet_nop_code:
            break;
        default:
            pdf_error(maketexstring("vf"),
                      maketexstring("invalid DVI command (2)"));
        }
    };
    cur = save_cur;
    packet_cur_s--;
}


integer *packet_local_fonts(internal_font_number f, integer * num)
{
    int c, cmd, cur_packet_byte, lf, k, l, i;
    integer localfonts[256] = { 0 };
    integer *lfs;
    charinfo *co;

    real_eight_bits *vf_packets;
    k = 0;
    for (c = font_bc(f); c <= font_ec(f); c++) {
        if (char_exists(f, c)) {
            co = get_charinfo(f, c);
            vf_packets = get_charinfo_packets(co);
            if (vf_packets == NULL)
                continue;
            cur_packet_byte = 0;
            while ((cmd = vf_packets[cur_packet_byte]) != packet_end_code) {
                cur_packet_byte++;
                switch (cmd) {
                case packet_font_code:
                    packet_number(lf);
                    for (l = 0; l < k; l++) {
                        if (localfonts[l] == lf) {
                            break;
                        }
                    }
                    if (l == k) {
                        localfonts[k++] = lf;
                    }
                    break;
                case packet_push_code:
                case packet_pop_code:
                case packet_nop_code:
                    break;
                case packet_char_code:
                case packet_right_code:
                case packet_down_code:
                case packet_node_code:
                    cur_packet_byte += 4;
                    break;
                case packet_rule_code:
                    cur_packet_byte += 8;
                    break;
                case packet_special_code:
                    packet_number(i);
                    while (i-- > 0)
                        (void) do_packet_byte();
                    break;
                case packet_image_code:
                    cur_packet_byte += 4;
                    break;
                default:
                    pdf_error(maketexstring("vf"),
                              maketexstring("invalid DVI command (3)"));
                }
            }
        }
    }
    *num = k;
    if (k > 0) {
        lfs = xmalloc(k * sizeof(integer));
        memcpy(lfs, localfonts, k * sizeof(integer));
        return lfs;
    }
    return NULL;
}


void
replace_packet_fonts(internal_font_number f, integer * old_fontid,
                     integer * new_fontid, int count)
{
    int c, cmd, cur_packet_byte, lf, k, l;
    charinfo *co;
    real_eight_bits *vf_packets;

    k = 0;
    for (c = font_bc(f); c <= font_ec(f); c++) {
        if (char_exists(f, c)) {
            co = get_charinfo(f, c);
            vf_packets = get_charinfo_packets(co);
            if (vf_packets == NULL)
                continue;
            cur_packet_byte = 0;
            while ((cmd = vf_packets[cur_packet_byte]) != packet_end_code) {
                cur_packet_byte++;
                switch (cmd) {
                case packet_font_code:
                    packet_number(lf);
                    for (l = 0; l < count; l++) {
                        if (old_fontid[l] == lf) {
                            break;
                        }
                    }
                    if (l < count) {
                        k = new_fontid[l];
                        vf_packets[(cur_packet_byte - 4)] =
                            (k & 0xFF000000) >> 24;
                        vf_packets[(cur_packet_byte - 3)] =
                            (k & 0x00FF0000) >> 16;
                        vf_packets[(cur_packet_byte - 2)] =
                            (k & 0x0000FF00) >> 8;
                        vf_packets[(cur_packet_byte - 1)] = (k & 0x000000FF);
                    }
                    break;
                case packet_push_code:
                case packet_pop_code:
                case packet_nop_code:
                    break;
                case packet_char_code:
                case packet_right_code:
                case packet_down_code:
                case packet_node_code:
                    cur_packet_byte += 4;
                    break;
                case packet_rule_code:
                    cur_packet_byte += 8;
                    break;
                case packet_special_code:
                    packet_number(k);
                    while (k-- > 0)
                        (void) do_packet_byte();
                    break;
                case packet_image_code:
                    cur_packet_byte += 4;
                    break;
                default:
                    pdf_error(maketexstring("vf"),
                              maketexstring("invalid DVI command (4)"));
                }
            }
        }
    }
}

/* this function was copied/borrowed/stolen from dvipdfm code */

scaled sqxfw(scaled sq, integer fw)
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
