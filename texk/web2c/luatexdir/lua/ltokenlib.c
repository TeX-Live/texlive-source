/* ltokenlib.c
   
   Copyright 2006-2012 Taco Hoekwater <taco@luatex.org>

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
#include "lua/luatex-api.h"


#define  is_valid_token(L,i)  (lua_istable(L,i) && lua_rawlen(L,i)==3)
#define  get_token_cmd(L,i)  lua_rawgeti(L,i,1)
#define  get_token_chr(L,i)  lua_rawgeti(L,i,2)
#define  get_token_cs(L,i)   lua_rawgeti(L,i,3)
#define  is_active_string(s) (strlen((char *)s)>3 && *s==0xEF && *(s+1)==0xBF && *(s+2)==0xBF)


static unsigned char *get_cs_text(int cs)
{
    if (cs == null_cs)
        return (unsigned char *) xstrdup("\\csname\\endcsname");
    else if ((cs_text(cs) < 0) || (cs_text(cs) >= str_ptr))
        return (unsigned char *) xstrdup("");
    else
        return (unsigned char *) makecstring(cs_text(cs));
}


static int test_expandable(lua_State * L)
{
    int cmd = -1;
    if (is_valid_token(L, -1)) {
        get_token_cmd(L, -1);
        if (lua_isnumber(L, -1)) {
            cmd = (int) lua_tointeger(L, -1);
        } else if (lua_isstring(L, -1)) {
            cmd = get_command_id(lua_tostring(L, -1));
        }
        if (cmd > max_command_cmd) {
            lua_pushboolean(L, 1);
        } else {
            lua_pushboolean(L, 0);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}


static int test_protected(lua_State * L)
{
    int chr = -1;
    if (is_valid_token(L, -1)) {
        get_token_chr(L, -1);
        if (lua_isnumber(L, -1)) {
            chr = (int) lua_tointeger(L, -1);
        } else if (lua_isstring(L, -1)) {
            chr = get_command_id(lua_tostring(L, -1));
        }
        if (token_info(token_link(chr)) == protected_token) {
            lua_pushboolean(L, 1);
        } else {
            lua_pushboolean(L, 0);
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int test_activechar(lua_State * L)
{
    if (is_valid_token(L, -1)) {
        unsigned char *s;
        int cs = 0;
        get_token_cs(L, -1);
        if (lua_isnumber(L, -1)) {
            cs = (int) lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
        if (cs != 0 && ((s = get_cs_text(cs)) != (unsigned char *) NULL)) {
            if (is_active_string(s)) {
                free(s);
                lua_pushboolean(L, 1);
                return 1;
            }
            free(s);
        }
    }
    lua_pushboolean(L, 0);
    return 1;
}


static int run_get_command_name(lua_State * L)
{
    int cs;
    if (is_valid_token(L, -1)) {
        get_token_cmd(L, -1);
        if (lua_isnumber(L, -1)) {
            cs = (int) lua_tointeger(L, -1);
            lua_pushstring(L, command_names[cs].cmd_name);
        } else {
            lua_pushstring(L, "");
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}


static int run_get_csname_name(lua_State * L)
{
    int cs /*, cmd*/;
    unsigned char *s;
    if (is_valid_token(L, -1)) {
        get_token_cmd(L, -1);
        /*
        if (lua_isnumber(L, -1)) {
            cmd = (int) lua_tointeger(L, -1);
        }
        */
        lua_pop(L, 1);
        cs = 0;
        get_token_cs(L, -1);
        if (lua_isnumber(L, -1)) {
            cs = (int) lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        if (cs != 0 && ((s = get_cs_text(cs)) != (unsigned char *) NULL)) {
            if (is_active_string(s))
                lua_pushstring(L, (char *) (s + 3));
            else
                lua_pushstring(L, (char *) s);
        } else {
            lua_pushstring(L, "");
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int run_get_command_id(lua_State * L)
{
    int cs = -1;
    if (lua_isstring(L, -1)) {
        cs = get_command_id(lua_tostring(L, -1));
    }
    lua_pushnumber(L, cs);
    return 1;
}


static int run_get_csname_id(lua_State * L)
{
    const char *s;
    size_t k, cs = 0;
    if (lua_isstring(L, -1)) {
        s = lua_tolstring(L, -1, &k);
        cs = (size_t) string_lookup(s, k);
    }
    lua_pushnumber(L, (lua_Number) cs);
    return 1;
}


void make_token_table(lua_State * L, int cmd, int chr, int cs)
{
    lua_createtable(L, 3, 0);
    lua_pushnumber(L, cmd);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L, chr);
    lua_rawseti(L, -2, 2);
    lua_pushnumber(L, cs);
    lua_rawseti(L, -2, 3);
}

static int run_get_next(lua_State * L)
{
    int save_nncs;
    save_nncs = no_new_control_sequence;
    no_new_control_sequence = 0;
    get_next();
    no_new_control_sequence = save_nncs;
    make_token_table(L, cur_cmd, cur_chr, cur_cs);
    return 1;
}

static int run_expand(lua_State * L)
{
    (void) L;
    expand();
    return 0;
}


static int run_lookup(lua_State * L)
{
    const char *s;
    size_t l;
    int cs, cmd, chr;
    int save_nncs;
    if (lua_isstring(L, -1)) {
        s = lua_tolstring(L, -1, &l);
        if (l > 0) {
            save_nncs = no_new_control_sequence;
            no_new_control_sequence = true;
            cs = id_lookup((last + 1), (int) l);        /* cleans up the lookup buffer */
            cs = string_lookup(s, l);
            cmd = eq_type(cs);
            chr = equiv(cs);
            make_token_table(L, cmd, chr, cs);
            no_new_control_sequence = save_nncs;
            return 1;
        }
    }
    lua_newtable(L);
    return 1;
}

static int run_build(lua_State * L)
{
    int cmd, chr, cs;
    if (lua_isnumber(L, 1)) {
        cs = 0;
        chr = (int) lua_tointeger(L, 1);
        cmd = (int) luaL_optinteger(L, 2, get_char_cat_code(chr));
        if (cmd == 0 || cmd == 9 || cmd == 14 || cmd == 15) {
            fprintf(stdout,
                    "\n\nluatex error: not a good token.\nCatcode %i can not be returned, so I replaced it by 12 (other)",
                    (int) cmd);
            error();
            cmd = 12;
        }
        if (cmd == 13) {
            cs = active_to_cs(chr, false);
            cmd = eq_type(cs);
            chr = equiv(cs);
        }
        make_token_table(L, cmd, chr, cs);
        return 1;
    } else {
        return run_lookup(L);
    }
}


static const struct luaL_Reg tokenlib[] = {
    {"get_next", run_get_next},
    {"expand", run_expand},
    {"lookup", run_lookup},
    {"create", run_build},
    {"is_expandable", test_expandable},
    {"is_activechar", test_activechar},
    {"is_protected", test_protected},
    {"csname_id", run_get_csname_id},
    {"csname_name", run_get_csname_name},
    {"command_name", run_get_command_name},
    {"command_id", run_get_command_id},
    {NULL, NULL}                /* sentinel */
};

int luaopen_token(lua_State * L)
{
    luaL_register(L, "token", tokenlib);
    return 1;
}
