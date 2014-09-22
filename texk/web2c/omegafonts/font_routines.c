/* font_routines.c: Data structures for virtual font support

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (C) 1994--2001 John Plaice and Yannis Haralambous
Copyright (C) 2005, 2006 Roozbeh Pournader

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
#include "dvi.h"
#include "error_routines.h"
#include "list_routines.h"
#include "font_routines.h"
#include "char_routines.h"
#include "header_routines.h"
#include "print_routines.h"
#include "out_routines.h"
#include "omfonts.h"

#define BLOCK 16

font *font_table = NULL;
unsigned font_table_size = 0;
unsigned no_fonts = 0;
font *cur_font = NULL;
unsigned cur_font_index = 0;

unsigned vtitle_length = 0;
unsigned vf_check_sum = 0;
unsigned vf_design_size = 0;

void
font_table_init(void)
{
    if (font_table == NULL) {
        font_table_size = BLOCK;
        font_table = (font *) xmalloc(font_table_size * sizeof(font));
    }
    no_fonts = 0;
}

static void
font_no_incr(void)
{
    if (no_fonts == font_table_size) {
       font_table_size += BLOCK;
       font_table = (font *) xrealloc(font_table, font_table_size * sizeof(font));
    }
    no_fonts++;
}

static void
clear_map_font(int font_number)
{
    if (cur_font==NULL) internal_error_0("clear_map_font");
    cur_font->font_number = font_number;
    cur_font->font_name = NULL;
    cur_font->font_area = NULL;
    cur_font->font_at = design_units;
    cur_font->font_checksum = 0;
    cur_font->font_dsize = 10*UNITY;
    cur_font->font_at_defined = FALSE;
    cur_font->font_checksum_defined = FALSE;
    cur_font->font_dsize_defined = FALSE;
    cur_font->ovf_packet = NULL;
    cur_font->ovf_packet_length = 0;
}

static void append_command(unsigned, unsigned);
static void packet_table_init(void);
static void packet_table_end(void);

void
init_map_font(int font_number)
{
    unsigned i=0;

    while (i<no_fonts) {
        if (font_number == font_table[i].font_number) {
             warning_1("MAPFONT index (D %d) previously defined; "
              "old definition ignored", font_number);
            cur_font = &font_table[i];
            if (cur_font->font_area != NULL) 
                free(cur_font->font_area);
            if (cur_font->font_name != NULL) 
                free(cur_font->font_name);
            if (cur_font->ovf_packet != NULL) 
                free(cur_font->ovf_packet);
            clear_map_font(font_number);
            break;
        }
        i++;
    }
    if (i==no_fonts) {
        font_no_incr();
        cur_font = &font_table[i];
        clear_map_font(font_number);
    }
    packet_table_init();
    append_command(DVI_FNT_DEF_1, i);
    cur_font_index = i;
    cur_font = &font_table[i];
    cur_font->ovf_packet = cur_packet;
    cur_font->ovf_packet_length = packet_ptr;
    packet_table_end();

}

void
set_font_name(string name)
{
    if (cur_font==NULL) {
        internal_error_0("set_font_name");
    }
    if (cur_font->font_name != NULL) {
        warning_0("FONTNAME previously defined; old value ignored");
        free(cur_font->font_name);
        cur_font->font_name=NULL;
    }
    cur_font->font_name = name;
}

void
set_font_area(string area)
{
    if (cur_font==NULL) {
        internal_error_0("set_font_area");
    }
    if (cur_font->font_area != NULL) {
        warning_0("FONTAREA previously defined; old value ignored");
        free(cur_font->font_area);
        cur_font->font_area=NULL;
    }
    cur_font->font_area = area;
}

void
set_font_check_sum(unsigned cs)
{
    if (cur_font==NULL) {
        internal_error_0("set_font_check_sum");
    }
    if (cur_font->font_checksum_defined != FALSE) {
        warning_0("FONTCHECKSUM previously defined; old value ignored");
    }
    cur_font->font_checksum = cs;
    cur_font->font_checksum_defined = TRUE;
}

void
set_font_at(fix at)
{
    if (cur_font==NULL) {
        internal_error_0("set_font_at");
    }
    if (cur_font->font_at_defined != FALSE) {
        warning_0("FONTAT previously defined; old value ignored");
    }
    cur_font->font_at = at;
    cur_font->font_at_defined = TRUE;
}

void
set_font_design_size(fix ds)
{
    if (cur_font==NULL) {
        internal_error_0("set_font_design_size");
    }
    if (cur_font->font_dsize_defined != FALSE) {
        warning_0("FONTDSIZE previously defined; old value ignored");
    }
    cur_font->font_dsize = ds;
    cur_font->font_dsize_defined = TRUE;
}

string vtitle = NULL;

void
set_vtitle(string title)
{
    if (vtitle!=NULL) {
        warning_0("VTITLE previously defined; old value ignored");
        free(vtitle); vtitle = NULL;
    }
    vtitle = title;
}

unsigned packet_table_size = 0;
unsigned char *packet_table = NULL;
unsigned char *cur_packet = NULL;
unsigned packet_ptr = 0;

static void
packet_table_init(void)
{
    packet_table_size = BLOCK;
    packet_table = (unsigned char *) xmalloc(packet_table_size);
    packet_ptr = 0;
    cur_packet = packet_table;
}

static void
packet_ptr_incr(void)
{
    if (packet_ptr == packet_table_size) {
        packet_table_size += BLOCK;
        packet_table = (unsigned char *)
                       xrealloc(packet_table, packet_table_size);
        cur_packet = packet_table;
    }
    packet_ptr++;
}

static void
append_to_packet(unsigned val)
{
    packet_ptr_incr();
    packet_table[packet_ptr-1] = val & 0xff;
}

static void move_table_init(void);

void
init_map(void)
{
    move_table_init();
    packet_table_init();
}

void
end_map(void)
{
    current_character->ovf_packet = cur_packet;
    current_character->ovf_packet_length = packet_ptr;
    packet_table_end();
}

static void
append_command_2(unsigned cmd_0, unsigned max_n,
                 unsigned cmd_1, unsigned actual)
{
    if ((actual < 0) || (actual > 0x7fffffff))
        internal_error_1("append_command (actual=%d)", actual);
    if ((cmd_0 + actual) <= max_n)
        append_to_packet(cmd_0 + actual);
    else
        append_command(cmd_1, actual);
}

static void
append_command(unsigned cmd_1, unsigned actual)
{
    if ((cmd_1 != DVI_SET_1) && (cmd_1 != DVI_FNT_1) &&
        (cmd_1 != DVI_FNT_DEF_1) && (cmd_1 != DVI_XXX_1)) {
        internal_error_1("append_command (cmd_1=%d)", cmd_1);
    }
    if (actual < 0x100) {
        append_to_packet(cmd_1);
        append_to_packet(actual);
    } else if (actual < 0x10000) {
        append_to_packet(cmd_1 + 1);
        append_to_packet(actual >> 8);
        append_to_packet(actual & 0xff);
    } else if (actual < 0x1000000) {
        append_to_packet(cmd_1 + 2);
        append_to_packet(actual >> 16);
        append_to_packet((actual >> 8) & 0xff);
        append_to_packet(actual & 0xff);
    } else {
        append_to_packet(cmd_1 + 3);
/* BUG: Should deal with negative numbers */
        append_to_packet((actual >> 24) & 0xff);
        append_to_packet((actual >> 16) & 0xff);
        append_to_packet((actual >> 8) & 0xff);
        append_to_packet(actual & 0xff);
    }
    
}

unsigned char *ovf_ptr=NULL;

int
ovf_get_arg(unsigned char **ptr, unsigned k, boolean is_signed)
{
    int a, b;

    if (k>4) internal_error_1("ovf_get_arg (k=%d)", k);
    a = b = (int) ((**ptr) & 0xff);
    (*ptr)++;
    if ((is_signed==TRUE) || (k==4)) { /* 4-byte numbers are signed */
       if (b > 0x7f) { a = a - 0x100; }
    }
    while (k>1) {
       b = (**ptr) & 0xff;
       (*ptr)++;
       a = 0x100 * a + b;
       k--;
    }
    return a;
}

static void
input_command(unsigned *cmd, int *actual)
{
    unsigned the_cmd = *ovf_ptr & 0xff;

    ovf_ptr++;
    if ((the_cmd>=DVI_SET_CHAR_0) && (the_cmd<=DVI_SET_CHAR_127)) {
       (*cmd)=DVI_SET_1;
       (*actual) = the_cmd;
    } else if ((the_cmd>=DVI_SET_1) && (the_cmd<=DVI_SET_4)) {
       (*cmd)=DVI_SET_1; 
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_SET_1 + 1, FALSE);
    } else if ((the_cmd>=DVI_FNT_NUM_0) && (the_cmd<=DVI_FNT_NUM_63)) {
       (*cmd)=DVI_FNT_1;
       (*actual) = the_cmd - DVI_FNT_NUM_0;
    } else if ((the_cmd>=DVI_FNT_1) && (the_cmd<=DVI_FNT_4)) {
       (*cmd)=DVI_FNT_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_FNT_1 + 1, FALSE);
    } else if ((the_cmd>=DVI_FNT_DEF_1) && (the_cmd<=DVI_FNT_DEF_4)) {
       (*cmd)=DVI_FNT_DEF_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_FNT_DEF_1 + 1, FALSE);
    } else if ((the_cmd>=DVI_XXX_1) && (the_cmd<=DVI_XXX_4)) {
       (*cmd)=DVI_XXX_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_XXX_1 + 1, FALSE);
    } else if ((the_cmd>=DVI_RIGHT_1) && (the_cmd<=DVI_RIGHT_4)) {
       (*cmd)=DVI_RIGHT_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_RIGHT_1 + 1, TRUE);
    } else if ((the_cmd>=DVI_DOWN_1) && (the_cmd<=DVI_DOWN_4)) {
       (*cmd)=DVI_DOWN_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_DOWN_1 + 1, TRUE);
    } else if ((the_cmd>=DVI_W_1) && (the_cmd<=DVI_W_4)) {
       (*cmd)=DVI_W_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_W_1 + 1, TRUE);
    } else if ((the_cmd>=DVI_X_1) && (the_cmd<=DVI_X_4)) {
       (*cmd)=DVI_X_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_X_1 + 1, TRUE);
    } else if ((the_cmd>=DVI_Y_1) && (the_cmd<=DVI_Y_4)) {
       (*cmd)=DVI_Y_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_Y_1 + 1, TRUE);
    } else if ((the_cmd>=DVI_Z_1) && (the_cmd<=DVI_Z_4)) {
       (*cmd)=DVI_Z_1;
       (*actual) = ovf_get_arg(&ovf_ptr, the_cmd - DVI_Z_1 + 1, TRUE);
    }
}


static void
append_to_packet_fix(unsigned cmd, fix fval)
{
    unsigned k;
    unsigned negative=FALSE;
    int t;

    if (design_units != UNITY)
        fval = zround(((double)fval) / ((double)design_units) * 1048576.0);
    if (fval<0) {
        negative = TRUE;
        fval = -1 - fval;
    }
    if (cmd == 0) {
        k = 4; t = 0x1000000;
    } else {
        t = 0x7f; k = 1;
        while (fval>t) {
            t = (t<<8) | 0xff; k++;
        }
        append_to_packet(cmd+k-1); t = t/0x80 + 1;
    }
    do {
        if (negative == TRUE) {
            append_to_packet(0xff - fval/t);
            fval = (fval/t)*t + t-1-fval;
            negative = FALSE;
        } else {
            append_to_packet((fval/t) & 0xff);
        }
        k--; t = t >> 8; 
    } while (k != 0);
}

unsigned move_table_size = 0;
move *move_table = NULL;
move *cur_move = NULL;
unsigned move_ptr = 0;

static void
move_table_init(void)
{
    if (move_table == NULL) {
        move_table_size = BLOCK;
        move_table = (move *) xmalloc(move_table_size * sizeof(move));
     }
    move_ptr = 0;
    cur_move = move_table;
    cur_move->h = 0; cur_move->v = 0;
}

static void
packet_table_end(void)
{
    cur_packet = NULL;
    packet_ptr = 0;
    packet_table_size = 0;
}


static void
move_ptr_decr(void)
{
    if (move_ptr==0)
        internal_error_0("move_ptr_incr");
    move_ptr--;
    cur_move = &move_table[move_ptr];
}

static void
move_ptr_incr(void)
{
    move_ptr++;
    if (move_ptr == move_table_size) {
       move_table_size += BLOCK;
       move_table = (move *) xrealloc(move_table, move_table_size * sizeof(move));
    }
    cur_move = &move_table[move_ptr];
}

static unsigned
get_hex(unsigned char c)
{
    if ((c>='0') && (c<='9')) return(c-'0');
    if ((c<'A') || (c>'F')) {
        internal_error_1("get_hex (a=%c)", c);
    }
    return(c-'A'+10);
}

void
set_select_font(unsigned f)
{
    unsigned i;

    for (i = 0; i < no_fonts; i++)
        if (f == font_table[i].font_number) {
            append_command_2(DVI_FNT_NUM_0, DVI_FNT_NUM_63, DVI_FNT_1, i);
            return;
        }

    internal_error_1("undefined MAPFONT %d cannot be selected", f);
}

void
set_set_char(unsigned c)
{
    if (cur_font==NULL) {
        warning_1("Character (H %X) cannot be typeset in undefined font", c);
    } else {
        append_command_2(DVI_SET_CHAR_0, DVI_SET_CHAR_127, DVI_SET_1, c);
    }
}   

void
set_set_rule(fix height, fix width)
{
    append_to_packet(DVI_SET_RULE);
    append_to_packet_fix(0, height);
    append_to_packet_fix(0, width);
}

void
set_move(unsigned direction, fix fval)
{
    if (cur_move == NULL) {
        internal_error_0("set_move");
    }
    if (direction == M_LEFT) {
        fval = -fval;
        direction = M_RIGHT;
    } else if (direction == M_UP) {
        fval = -fval;
        direction = M_DOWN;
    }
    if (direction == M_RIGHT) { /* horizontal movement */
        if (cur_move->h == 0) {
            cur_move->wfix = fval; cur_move->h++;
            append_to_packet_fix(DVI_W_1, fval);
        } else if (fval == cur_move->wfix) {
            append_to_packet(DVI_W_0);
        } else if (cur_move->h == 1) {
            cur_move->xfix = fval; cur_move->h++;
            append_to_packet_fix(DVI_X_1, fval);
        } else if (fval == cur_move->xfix) {
            append_to_packet(DVI_X_0);
        } else {
            append_to_packet_fix(DVI_RIGHT_1, fval);
        }
    } else {                  /* vertical movement */
        if (cur_move->v == 0) {
            cur_move->yfix = fval; cur_move->v++;
            append_to_packet_fix(DVI_Y_1, fval);
        } else if (fval == cur_move->yfix) {
            append_to_packet(DVI_Y_0);
        } else if (cur_move->v == 1) {
            cur_move->zfix = fval; cur_move->v++;
            append_to_packet_fix(DVI_Z_1, fval);
        } else if (fval == cur_move->zfix) {
            append_to_packet(DVI_Z_0);
        } else {
            append_to_packet_fix(DVI_DOWN_1, fval);
        }
    }
}

void
set_push(void)
{
    append_to_packet(DVI_PUSH);
    move_ptr_incr();
    cur_move->h = 0;
    cur_move->v = 0;
}

void
set_pop(void)
{
    append_to_packet(DVI_POP);
    move_ptr_decr();
}

void
set_special(string special_string)
{
    unsigned len = strlen(special_string);
    unsigned i=0;

    append_command(DVI_XXX_1, len);
    for (i=0; i<len; i++) {
        append_to_packet(special_string[i]);
    }
}

void
set_special_hex(string special_hex_string)
{
    unsigned len = strlen(special_hex_string);
    unsigned i=0;

    append_command(DVI_XXX_1, len/2);
    for (i=0; i<len; i+=2) {
        append_to_packet(get_hex(special_hex_string[i])*16+
                         get_hex(special_hex_string[i+1]));
    }
}

unsigned file_ovf_count = 0;

void
out_ovf(unsigned i)
{
    fputc(i,file_ovf);
    file_ovf_count++;
}

void
out_ovf_4(unsigned i)
{
    fputc(i>>24,        file_ovf);
    fputc((i>>16)&0xff, file_ovf);
    fputc((i>>8)&0xff,  file_ovf);
    fputc(i&0xff,       file_ovf);
    file_ovf_count += 4;
}

static void
output_ovf_fonts(void)
{
    unsigned i, j, k1, k2;

    for (i=0; i<no_fonts; i++) {
        cur_font = font_table+i;
        for (j=0; j<cur_font->ovf_packet_length; j++)
            out_ovf(cur_font->ovf_packet[j]);
        out_ovf_4(cur_font->font_checksum);
        out_ovf_4(cur_font->font_at);
        out_ovf_4((unsigned int)cur_font->font_dsize);
        if (cur_font->font_area != NULL) {
            k2 = strlen(cur_font->font_area);
            out_ovf(k2);
        } else {
            k2 = 0; out_ovf(0);
        }
        if (cur_font->font_name != NULL) {
            k1 = strlen(cur_font->font_name);
            out_ovf(k1);
        } else {
            k1 = 4; out_ovf(4);
        }
        if (cur_font->font_area != NULL) {
            for (j=0; j<k2; j++)
                out_ovf(cur_font->font_area[j]);
        }
        if (cur_font->font_name != NULL) {
            for (j=0; j<k1; j++)
                out_ovf(cur_font->font_name[j]);
        } else {
            out_ovf('N'); out_ovf('U'); out_ovf('L'); out_ovf('L');
        }
    }
}

void
output_ovf_file(void)
{
    unsigned i, k;

    out_ovf(DVI_PRE); out_ovf(VF_ID);
    /* Check for vtitle==NULL by Joel Riou <joel.riou@normalesup.org> */
    k = vtitle ? strlen(vtitle) : 0; out_ovf(k);
    for (i=0; i<k; i++) out_ovf(vtitle[i]);
    out_ovf_4(check_sum); out_ovf_4(design_size);
    output_ovf_fonts();
    output_ovf_chars();
    do {
        out_ovf(DVI_POST);
    } while ((file_ovf_count % 4) != 0);
}

static void
in_ovf_4(int *value)
{
    *value = (((*ovf_ptr)    & 0xff) << 24) |
             (((*(ovf_ptr+1)) & 0xff) << 16) |
             (((*(ovf_ptr+2)) & 0xff) <<  8) |
              ((*(ovf_ptr+3)) & 0xff);
    ovf_ptr += 4;
}

static void
in_ovf_unsigned_4(unsigned *value)
{
    *value = (((*ovf_ptr)    & 0xff) << 24) |
             (((*(ovf_ptr+1)) & 0xff) << 16) |
             (((*(ovf_ptr+2)) & 0xff) <<  8) |
              ((*(ovf_ptr+3)) & 0xff);
    ovf_ptr += 4;
}

static void
in_ovf_3(int *value)
{
    *value = (((*ovf_ptr) & 0xff) << 16) |
             (((*(ovf_ptr+1)) & 0xff) <<  8) |
              ((*(ovf_ptr+2)) & 0xff);
    ovf_ptr += 3;
}

static void
in_ovf(int *value)
{
    *value = (*ovf_ptr) & 0xff;
    ovf_ptr ++;
}

void
input_ovf_file(void)
{
    ovf_ptr = ovf;
    if ((*ovf_ptr & 0xff) != DVI_PRE)
	fatal_error_0("Not a VF or OVF file; first byte should be 247");
    ovf_ptr++;
    if ((*ovf_ptr & 0xff) != VF_ID)
	fatal_error_0("Not a VF or OVF file; second byte should be 202");
    ovf_ptr++;
    vtitle_length = *ovf_ptr & 0xff;
    ovf_ptr++;
    vtitle = (char *) xmalloc(vtitle_length+1);
    strncpy(vtitle, (const char *)ovf_ptr, vtitle_length);
    vtitle[vtitle_length] = '\0';
    ovf_ptr += vtitle_length;
    print_vtitle(vtitle);
    in_ovf_unsigned_4(&vf_check_sum);
    in_ovf_unsigned_4(&vf_design_size);
}


void
input_ovf_fonts(void)
{
    int cur_font_area_length;
    int cur_font_name_length;
    char *cur_font_name;
    char *cur_font_area;
    unsigned cur_font_check_sum;
    int cur_font_at;
    int cur_font_dsize;
    unsigned cmd;

    font_table_init();
    while (((*ovf_ptr & 0xff) >= DVI_FNT_DEF_1) &&
           ((*ovf_ptr & 0xff) <= DVI_FNT_DEF_4)) {
      cur_font_index = no_fonts;
      cur_font = &(font_table[cur_font_index]);
      font_no_incr();
      input_command(&cmd, &(cur_font->font_number));
      print_map_font(cur_font->font_number);
      in_ovf_unsigned_4(&cur_font_check_sum);
      cur_font_at = ovf_get_arg(&ovf_ptr, 4, FALSE);
      cur_font_dsize = ovf_get_arg(&ovf_ptr, 4, FALSE);
      in_ovf(&cur_font_area_length);
      in_ovf(&cur_font_name_length);
      cur_font_area = NULL;
      if (cur_font_area_length != 0) {
          cur_font_area = (char *)xmalloc(cur_font_area_length+1);
          strncpy(cur_font_area, (const char *)ovf_ptr, cur_font_area_length);
          cur_font_area[cur_font_area_length] = '\0';
          ovf_ptr += cur_font_area_length;
          print_font_area(cur_font_area);
      }
      cur_font_name = NULL;
      if (cur_font_name_length != 0) {
          cur_font_name = (char *)xmalloc(cur_font_name_length+1);
          strncpy(cur_font_name, (const char *)ovf_ptr, cur_font_name_length);
          cur_font_name[cur_font_name_length] = '\0';
          ovf_ptr += cur_font_name_length;
          print_font_name(cur_font_name);
      }
      print_font_check_sum(cur_font_check_sum);
      print_font_at(cur_font_at);
      print_font_design_size(cur_font_dsize);
      right();
    }
}

void
input_ovf_chars(void)
{
    int pl,cc,wd;

    while ((*ovf_ptr & 0xff) <= VF_LONG_CHAR) {
        if ((*ovf_ptr & 0xff) == VF_LONG_CHAR) {
            ovf_ptr++;
            in_ovf_4(&pl);
            in_ovf_4(&cc);
            in_ovf_4(&wd);
        } else {
            in_ovf(&pl);
            in_ovf(&cc);
            in_ovf_3(&wd);
        }
        ensure_existence(cc);
        current_secondary_character->ovf_packet_length = pl;
        current_secondary_character->ovf_packet = ovf_ptr;
        ovf_ptr += pl;
    }
    while (ovf_ptr < (ovf+length_ovf)) {
        if ((*ovf_ptr & 0xff) != DVI_POST)
            fatal_error_0("Not a VF or OVF file; final bytes should be 248");
        ovf_ptr++;
    }
}
