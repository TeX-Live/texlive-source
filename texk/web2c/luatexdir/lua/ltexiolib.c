/* ltexiolib.c
   
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

static const char _svn_version[] =
    "$Id: ltexiolib.c 2027 2009-03-14 18:47:32Z oneiros $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/lua/ltexiolib.c $";

typedef void (*texio_printer) (strnumber s);

static char *loggable_info = NULL;

static boolean get_selector_value(lua_State * L, int i, char *l)
{
    boolean r = false;
    if (lua_isstring(L, i)) {
        char *s = (char *) lua_tostring(L, i);
        if (strcmp(s, "log") == 0) {
            *l = log_only;
            r = true;
        } else if (strcmp(s, "term") == 0) {
            *l = term_only;
            r = true;
        } else if (strcmp(s, "term and log") == 0) {
            *l = term_and_log;
            r = true;
        }
    } else {
        lua_pushstring(L, "first argument is not a string");
        lua_error(L);
    }
    return r;
}

static int do_texio_print(lua_State * L, texio_printer printfunction)
{
    strnumber texs;
    char *s;
    size_t k;
    int i = 1;
    strnumber u = 0;
    char save_selector = selector;
    int n = lua_gettop(L);
    if (n == 0 || !lua_isstring(L, -1)) {
        lua_pushstring(L, "no string to print");
        lua_error(L);
    }
    if (n > 1) {
        if (get_selector_value(L, i, &selector))
            i++;
    }
    if (selector != log_only && selector != term_only
        && selector != term_and_log) {
        normalize_selector();   /* sets selector */
    }
    /* just in case there is a string in progress */
    if (str_start[str_ptr - 0x200000] < pool_ptr)
        u = make_string();
    for (; i <= n; i++) {
        if (lua_isstring(L, i)) {
            s = (char *) lua_tolstring(L, i, &k);
            texs = maketexlstring(s, k);
            printfunction(texs);
            flush_str(texs);
        } else {
            lua_pushstring(L, "argument is not a string");
            lua_error(L);
        }
    }
    selector = save_selector;
    if (u != 0)
        str_ptr--;
    return 0;
}

static void do_texio_ini_print(lua_State * L, char *extra)
{
    char *s;
    int i = 1;
    char l = term_and_log;
    int n = lua_gettop(L);
    if (n > 1) {
        if (get_selector_value(L, i, &l))
            i++;
    }
    for (; i <= n; i++) {
        if (lua_isstring(L, i)) {
            s = (char *) lua_tostring(L, i);
            if (l == term_and_log || l == term_only)
                fprintf(stdout, "%s%s", extra, s);
            if (l == log_only || l == term_and_log) {
                if (loggable_info == NULL) {
                    loggable_info = strdup(s);
                } else {
                    char *v = concat3(loggable_info, extra, s);
                    free(loggable_info);
                    loggable_info = v;
                }
            }
        }
    }
}

static int texio_print(lua_State * L)
{
    if (ready_already != 314159 || pool_ptr == 0 || job_name == 0) {
        do_texio_ini_print(L, "");
        return 0;
    }
    return do_texio_print(L, zprint);
}

static int texio_printnl(lua_State * L)
{
    if (ready_already != 314159 || pool_ptr == 0 || job_name == 0) {
        do_texio_ini_print(L, "\n");
        return 0;
    }
    return do_texio_print(L, zprint_nl);
}

/* at the point this function is called, the selector is log_only */
void flush_loggable_info(void)
{
    if (loggable_info != NULL) {
        fprintf(log_file, "%s\n", loggable_info);
        free(loggable_info);
        loggable_info = NULL;
    }
}


static const struct luaL_reg texiolib[] = {
    {"write", texio_print},
    {"write_nl", texio_printnl},
    {NULL, NULL}                /* sentinel */
};

int luaopen_texio(lua_State * L)
{
    luaL_register(L, "texio", texiolib);
    return 1;
}
