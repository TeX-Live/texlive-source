/* luatoken.c
   
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

#include "luatex-api.h"
#include <ptexlib.h>

#include "tokens.h"
#include "commands.h"

static const char _svn_version[] =
    "$Id: luatoken.c 2079 2009-03-22 10:15:03Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/lua/luatoken.c $";

command_item command_names[] = {
    {"relax", relax_cmd, NULL},
    {"left_brace", left_brace_cmd, NULL},
    {"right_brace", right_brace_cmd, NULL},
    {"math_shift", math_shift_cmd, NULL},
    {"tab_mark", tab_mark_cmd, NULL},
    {"car_ret", car_ret_cmd, NULL},
    {"mac_param", mac_param_cmd, NULL},
    {"sup_mark", sup_mark_cmd, NULL},
    {"sub_mark", sub_mark_cmd, NULL},
    {"endv", endv_cmd, NULL},
    {"spacer", spacer_cmd, NULL},
    {"letter", letter_cmd, NULL},
    {"other_char", other_char_cmd, NULL},
    {"par_end", par_end_cmd, NULL},
    {"stop", stop_cmd, NULL},
    {"delim_num", delim_num_cmd, NULL},
    {"char_num", char_num_cmd, NULL},
    {"math_char_num", math_char_num_cmd, NULL},
    {"mark", mark_cmd, NULL},
    {"xray", xray_cmd, NULL},
    {"make_box", make_box_cmd, NULL},
    {"hmove", hmove_cmd, NULL},
    {"vmove", vmove_cmd, NULL},
    {"un_hbox", un_hbox_cmd, NULL},
    {"un_vbox", un_vbox_cmd, NULL},
    {"remove_item", remove_item_cmd, NULL},
    {"hskip", hskip_cmd, NULL},
    {"vskip", vskip_cmd, NULL},
    {"mskip", mskip_cmd, NULL},
    {"kern", kern_cmd, NULL},
    {"mkern", mkern_cmd, NULL},
    {"leader_ship", leader_ship_cmd, NULL},
    {"halign", halign_cmd, NULL},
    {"valign", valign_cmd, NULL},
    {"no_align", no_align_cmd, NULL},
    {"vrule", vrule_cmd, NULL},
    {"hrule", hrule_cmd, NULL},
    {"insert", insert_cmd, NULL},
    {"vadjust", vadjust_cmd, NULL},
    {"ignore_spaces", ignore_spaces_cmd, NULL},
    {"after_assignment", after_assignment_cmd, NULL},
    {"after_group", after_group_cmd, NULL},
    {"break_penalty", break_penalty_cmd, NULL},
    {"start_par", start_par_cmd, NULL},
    {"ital_corr", ital_corr_cmd, NULL},
    {"accent", accent_cmd, NULL},
    {"math_accent", math_accent_cmd, NULL},
    {"discretionary", discretionary_cmd, NULL},
    {"eq_no", eq_no_cmd, NULL},
    {"left_right", left_right_cmd, NULL},
    {"math_comp", math_comp_cmd, NULL},
    {"limit_switch", limit_switch_cmd, NULL},
    {"above", above_cmd, NULL},
    {"math_style", math_style_cmd, NULL},
    {"math_choice", math_choice_cmd, NULL},
    {"non_script", non_script_cmd, NULL},
    {"vcenter", vcenter_cmd, NULL},
    {"case_shift", case_shift_cmd, NULL},
    {"message", message_cmd, NULL},
    {"extension", extension_cmd, NULL},
    {"in_stream", in_stream_cmd, NULL},
    {"begin_group", begin_group_cmd, NULL},
    {"end_group", end_group_cmd, NULL},
    {"omit", omit_cmd, NULL},
    {"ex_space", ex_space_cmd, NULL},
    {"no_boundary", no_boundary_cmd, NULL},
    {"radical", radical_cmd, NULL},
    {"end_cs_name", end_cs_name_cmd, NULL},
    {"char_ghost", char_ghost_cmd, NULL},
    {"assign_local_box", assign_local_box_cmd, NULL},
    {"char_given", char_given_cmd, NULL},
    {"math_given", math_given_cmd, NULL},
    {"omath_given", omath_given_cmd, NULL},
    {"xmath_given", xmath_given_cmd, NULL},
    {"last_item", last_item_cmd, NULL},
    {"toks_register", toks_register_cmd, NULL},
    {"assign_toks", assign_toks_cmd, NULL},
    {"assign_int", assign_int_cmd, NULL},
    {"assign_attr", assign_attr_cmd, NULL},
    {"assign_dimen", assign_dimen_cmd, NULL},
    {"assign_glue", assign_glue_cmd, NULL},
    {"assign_mu_glue", assign_mu_glue_cmd, NULL},
    {"assign_font_dimen", assign_font_dimen_cmd, NULL},
    {"assign_font_int", assign_font_int_cmd, NULL},
    {"set_aux", set_aux_cmd, NULL},
    {"set_prev_graf", set_prev_graf_cmd, NULL},
    {"set_page_dimen", set_page_dimen_cmd, NULL},
    {"set_page_int", set_page_int_cmd, NULL},
    {"set_box_dimen", set_box_dimen_cmd, NULL},
    {"set_tex_shape", set_tex_shape_cmd, NULL},
    {"set_etex_shape", set_etex_shape_cmd, NULL},
    {"def_char_code", def_char_code_cmd, NULL},
    {"def_del_code", def_del_code_cmd, NULL},
    {"extdef_math_code", extdef_math_code_cmd, NULL},
    {"extdef_del_code", extdef_del_code_cmd, NULL},
    {"def_family", def_family_cmd, NULL},
    {"set_math_param", set_math_param_cmd, NULL},
    {"set_font", set_font_cmd, NULL},
    {"def_font", def_font_cmd, NULL},
    {"register", register_cmd, NULL},
    {"assign_box_dir", assign_box_dir_cmd, NULL},
    {"assign_dir", assign_dir_cmd, NULL},
    {"advance", advance_cmd, NULL},
    {"multiply", multiply_cmd, NULL},
    {"divide", divide_cmd, NULL},
    {"prefix", prefix_cmd, NULL},
    {"let", let_cmd, NULL},
    {"shorthand_def", shorthand_def_cmd, NULL},
    {"read_to_cs", read_to_cs_cmd, NULL},
    {"def", def_cmd, NULL},
    {"set_box", set_box_cmd, NULL},
    {"hyph_data", hyph_data_cmd, NULL},
    {"set_interaction", set_interaction_cmd, NULL},
    {"letterspace_font", letterspace_font_cmd, NULL},
    {"pdf_copy_font", pdf_copy_font_cmd, NULL},
    {"set_ocp", set_ocp_cmd, NULL},
    {"def_ocp", def_ocp_cmd, NULL},
    {"set_ocp_list", set_ocp_list_cmd, NULL},
    {"def_ocp_list", def_ocp_list_cmd, NULL},
    {"clear_ocp_lists", clear_ocp_lists_cmd, NULL},
    {"push_ocp_list", push_ocp_list_cmd, NULL},
    {"pop_ocp_list", pop_ocp_list_cmd, NULL},
    {"ocp_list_op", ocp_list_op_cmd, NULL},
    {"ocp_trace_level", ocp_trace_level_cmd, NULL},
    {"undefined_cs", undefined_cs_cmd, NULL},
    {"expand_after", expand_after_cmd, NULL},
    {"no_expand", no_expand_cmd, NULL},
    {"input", input_cmd, NULL},
    {"if_test", if_test_cmd, NULL},
    {"fi_or_else", fi_or_else_cmd, NULL},
    {"cs_name", cs_name_cmd, NULL},
    {"convert", convert_cmd, NULL},
    {"the", the_cmd, NULL},
    {"top_bot_mark", top_bot_mark_cmd, NULL},
    {"call", call_cmd, NULL},
    {"long_call", long_call_cmd, NULL},
    {"outer_call", outer_call_cmd, NULL},
    {"long_outer_call", long_outer_call_cmd, NULL},
    {"end_template", end_template_cmd, NULL},
    {"dont_expand", dont_expand_cmd, NULL},
    {"glue_ref", glue_ref_cmd, NULL},
    {"shape_ref", shape_ref_cmd, NULL},
    {"box_ref", box_ref_cmd, NULL},
    {"data", data_cmd, NULL},
    {NULL, 0, NULL}
};


int get_command_id(char *s)
{
    int i;
    int cmd = -1;
    for (i = 0; command_names[i].cmd_name != NULL; i++) {
        if (strcmp(s, command_names[i].cmd_name) == 0)
            break;
    }
    if (command_names[i].cmd_name != NULL) {
        cmd = i;
    }
    return cmd;
}

static int get_cur_cmd(lua_State * L)
{
    int r = 0;
    int len = lua_objlen(L, -1);
    cur_cs = 0;
    if (len == 3 || len == 2) {
        r = 1;
        lua_rawgeti(L, -1, 1);
        cur_cmd = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        cur_chr = lua_tointeger(L, -1);
        if (len == 3) {
            lua_rawgeti(L, -3, 3);
            cur_cs = lua_tointeger(L, -1);
        }
        lua_pop(L, len);
        if (cur_cs == 0)
            cur_tok = (cur_cmd * string_offset) + cur_chr;
        else
            cur_tok = cs_token_flag + cur_cs;
    }
    return r;
}


static int token_from_lua(lua_State * L)
{
    int cmd, chr;
    int cs = 0;
    int len = lua_objlen(L, -1);
    if (len == 3 || len == 2) {
        lua_rawgeti(L, -1, 1);
        cmd = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        chr = lua_tointeger(L, -1);
        if (len == 3) {
            lua_rawgeti(L, -3, 3);
            cs = lua_tointeger(L, -1);
        }
        lua_pop(L, len);
        if (cs == 0) {
            return (cmd * string_offset) + chr;
        } else {
            return cs_token_flag + cs;
        }
    }
    return -1;
}

static int get_cur_cs(lua_State * L)
{
    char *s;
    unsigned j;
    size_t l;
    integer cs;
    int save_nncs;
    int ret;
    ret = 0;
    cur_cs = 0;
    lua_getfield(L, -1, "name");
    if (lua_isstring(L, -1)) {
        s = (char *) lua_tolstring(L, -1, &l);
        if (l > 0) {
            if ((int) (last + l) > buf_size)
                check_buffer_overflow(last + l);
            for (j = 0; j < l; j++) {
                buffer[last + 1 + j] = *s++;
            }
            save_nncs = no_new_control_sequence;
            no_new_control_sequence = false;
            cs = id_lookup((last + 1), l);
            cur_tok = cs_token_flag + cs;
            cur_cmd = zget_eq_type(cs);
            cur_chr = zget_equiv(cs);
            no_new_control_sequence = save_nncs;
            ret = 1;
        }
    }
    lua_pop(L, 1);
    return ret;
}


#define make_room(a)                                    \
  if ((i+a+1)>alloci) {                                 \
    ret = xrealloc(ret,alloci+64);                      \
    alloci = alloci + 64;                               \
  }


#define append_i_byte(a) ret[i++] = a

#define Print_char(a) make_room(1); append_i_byte(a)

#define Print_uchar(s) {                                           \
    make_room(4);                                                  \
    if (s<=0x7F) {                                                 \
      append_i_byte(s);                                            \
    } else if (s<=0x7FF) {                                         \
      append_i_byte(0xC0 + (s / 0x40));                            \
      append_i_byte(0x80 + (s % 0x40));                            \
    } else if (s<=0xFFFF) {                                        \
      append_i_byte(0xE0 + (s / 0x1000));                          \
      append_i_byte(0x80 + ((s % 0x1000) / 0x40));                 \
      append_i_byte(0x80 + ((s % 0x1000) % 0x40));                 \
    } else if (s>=0x110000) {                                      \
      append_i_byte(s-0x11000);                                    \
    } else {                                                       \
      append_i_byte(0xF0 + (s / 0x40000));                         \
      append_i_byte(0x80 + ((s % 0x40000) / 0x1000));              \
      append_i_byte(0x80 + (((s % 0x40000) % 0x1000) / 0x40));     \
      append_i_byte(0x80 + (((s % 0x40000) % 0x1000) % 0x40));     \
    } }


#define Print_esc(b) {                                          \
    char *v = b;                                                \
    if (e>0 && e<string_offset) {                               \
      Print_uchar (e); Print_uchar (e);                         \
    }                                                           \
    while (*v) { Print_char(*v); v++; }                         \
  }

#define single_letter(a) (length(a)==1)||                       \
  ((length(a)==4)&&(str_pool[str_start_macro(a)]>=0xF0))||      \
  ((length(a)==3)&&(str_pool[str_start_macro(a)]>=0xE0))||      \
  ((length(a)==2)&&(str_pool[str_start_macro(a)]>=0xC0))

#define is_active_cs(a) (length(a)>3 &&                               \
                         (str_pool[str_start_macro(a)]   == 0xEF) &&  \
                         (str_pool[str_start_macro(a)+1] == 0xBF) &&  \
                         (str_pool[str_start_macro(a)+2] == 0xBF))

#define is_cat_letter(a)                                                \
  (get_char_cat_code(pool_to_unichar(str_start_macro(a))) == 11)

static int null_cs = 0;
static int eqtb_size = 0;

/* 2,720,652 */
char *tokenlist_to_cstring(int pp, int inhibit_par, int *siz)
{
    register integer p, c, m;
    integer q;
    integer infop;
    char *s;
    int e;
    char *ret;
    int match_chr = '#';
    int n = '0';
    int alloci = 1024;
    int i = 0;
    p = pp;
    if (p == null || link(p) == null) {
        if (siz != NULL)
            *siz = 0;
        return NULL;
    }
    ret = xmalloc(alloci);
    p = link(p);                /* skip refcount */
    if (null_cs == 0) {
        null_cs = get_nullcs();
        eqtb_size = get_eqtb_size();
    }
    e = get_escape_char();
    while (p != null) {
        if (p < fix_mem_min || p > fix_mem_end) {
            Print_esc("CLOBBERED.");
            break;
        }
        infop = info(p);
        if (infop >= cs_token_flag) {
            if (!(inhibit_par && infop == par_token)) {
                q = infop - cs_token_flag;
                if (q < hash_base) {
                    if (q == null_cs) {
                        /* Print_esc("csname"); Print_esc("endcsname"); */
                    }
                } else if ((q >= static_undefined_control_sequence)
                           && ((q <= eqtb_size)
                               || (q > eqtb_size + hash_extra))) {
                    Print_esc("IMPOSSIBLE.");
                } else if ((zget_cs_text(q) < 0)
                           || (zget_cs_text(q) >= str_ptr)) {
                    Print_esc("NONEXISTENT.");
                } else {
                    str_number txt = zget_cs_text(q);
                    s = makecstring(txt);
                    if (is_active_cs(txt)) {
                        s = s + 3;
                        while (*s) {
                            Print_char(*s);
                            s++;
                        }
                    } else {
                        Print_uchar(e);
                        while (*s) {
                            Print_char(*s);
                            s++;
                        }
                        if ((!single_letter(zget_cs_text(q)))
                            || is_cat_letter(zget_cs_text(q))) {
                            Print_char(' ');
                        }
                    }
                }
            }
        } else {
            if (infop < 0) {
                Print_esc("BAD.");
            } else {
                m = infop >> string_offset_bits;
                c = infop & (string_offset - 1);
                switch (m) {
                case 10:
                case 11:
                case 12:
                case 1:
                case 2:
                case 3:
                case 4:
                case 7:
                case 8:
                    Print_uchar(c);
                    break;
                case 6:
                    /* Print_uchar(c); */
                    Print_uchar(c);
                    break;
                case 5:
                    Print_uchar(match_chr);
                    if (c <= 9) {
                        Print_char(c + '0');
                    } else {
                        Print_char('!');
                        xfree(ret);
                        return NULL;
                    }
                    break;
                case 13:
                    match_chr = c;
                    Print_uchar(c);
                    n++;
                    Print_char(n);
                    if (n > '9') {
                        xfree(ret);
                        return NULL;
                    }
                    break;
                case 14:
                    if (c == 0) {
                        Print_char('-');
                        Print_char('>');
                    }
                    break;
                default:
                    Print_esc("BAD.");
                    break;
                }
            }
        }
        p = link(p);
    }
    ret[i] = 0;
    if (siz != NULL)
        *siz = i;
    return ret;
}


void tokenlist_to_lua(lua_State * L, int p)
{
    int cmd, chr, cs;
    int v;
    int i = 1;
    v = p;
    while (v != null && v < fix_mem_end) {
        i++;
        v = link(v);
    }
    i = 1;
    lua_createtable(L, i, 0);
    while (p != null && p < fix_mem_end) {
        if (info(p) >= cs_token_flag) {
            cs = info(p) - cs_token_flag;
            cmd = zget_eq_type(cs);
            chr = zget_equiv(cs);
            make_token_table(L, cmd, chr, cs);
        } else {
            cmd = info(p) / string_offset;
            chr = info(p) % string_offset;
            make_token_table(L, cmd, chr, 0);
        }
        lua_rawseti(L, -2, i++);
        p = link(p);
    }
}


void tokenlist_to_luastring(lua_State * L, int p)
{
    int l;
    char *s;
    s = tokenlist_to_cstring(p, 1, &l);
    lua_pushlstring(L, s, l);
}


int tokenlist_from_lua(lua_State * L)
{
    char *s;
    int tok;
    size_t i, j;
    halfword p, q, r;
    r = get_avail();
    info(r) = 0;                /* ref count */
    link(r) = null;
    p = r;
    if (lua_istable(L, -1)) {
        j = lua_objlen(L, -1);
        if (j > 0) {
            for (i = 1; i <= j; i++) {
                lua_rawgeti(L, -1, i);
                tok = token_from_lua(L);
                if (tok >= 0) {
                    store_new_token(tok);
                }
                lua_pop(L, 1);
            };
        }
        return r;
    } else if (lua_isstring(L, -1)) {
        s = (char *) lua_tolstring(L, -1, &j);
        for (i = 0; i < j; i++) {
            if (s[i] == 32) {
                tok = (10 * string_offset) + s[i];
            } else {
                tok = (12 * string_offset) + s[i];
            }
            store_new_token(tok);
        }
        return r;
    } else {
        free_avail(r);
        return null;
    }
}

void do_get_token_lua(integer callback_id)
{
    lua_State *L = Luas;
    while (1) {
        if (!get_callback(L, callback_id)) {
            get_next();
            lua_pop(L, 2);      /* the not-a-function callback and the container */
            break;
        }
        if (lua_pcall(L, 0, 1, 0) != 0) {       /* no arg, 1 result */
            tex_error((char *) lua_tostring(L, -1), NULL);
            lua_pop(L, 2);      /* container and result */
            break;
        }
        if (lua_istable(L, -1)) {
            lua_rawgeti(L, -1, 1);
            if (lua_istable(L, -1)) {   /* container, result, result[1] */
                integer p, q, r;
                int i, j;
                lua_pop(L, 1);  /* container, result */
                /* build a token list */
                r = get_avail();
                p = r;
                j = lua_objlen(L, -1);
                if (j > 0) {
                    for (i = 1; i <= j; i++) {
                        lua_rawgeti(L, -1, i);
                        if (get_cur_cmd(L) || get_cur_cs(L)) {
                            store_new_token(cur_tok);
                        }
                        lua_pop(L, 1);
                    }
                }
                if (p != r) {
                    p = link(r);
                    free_avail(r);
                    begin_token_list(p, inserted);
                    cur_input.nofilter_field = true;
                    get_next();
                } else {
                    tex_error("error: illegal or empty token list returned",
                              NULL);
                }
                lua_pop(L, 2);
                break;
            } else {            /* container, result, whatever */
                lua_pop(L, 1);  /* container, result */
                if (get_cur_cmd(L) || get_cur_cs(L)) {
                    lua_pop(L, 2);
                    break;
                } else {
                    lua_pop(L, 2);
                    continue;
                }
            }
        } else {
            lua_pop(L, 2);      /* container, result */
        }
    }
    return;
}
