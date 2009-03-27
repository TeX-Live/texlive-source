/* lkpselib.c
   
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
#include <kpathsea/expand.h>
#include <kpathsea/variable.h>
#include <kpathsea/tex-glyph.h>
#include <kpathsea/readable.h>

static const char _svn_version[] =
    "$Id: lkpselib.c 2093 2009-03-23 11:44:13Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/lua/lkpselib.c $";

static const int filetypes[] = {
    kpse_gf_format,
    kpse_pk_format,
    kpse_any_glyph_format,
    kpse_tfm_format,
    kpse_afm_format,
    kpse_base_format,
    kpse_bib_format,
    kpse_bst_format,
    kpse_cnf_format,
    kpse_db_format,
    kpse_fmt_format,
    kpse_fontmap_format,
    kpse_mem_format,
    kpse_mf_format,
    kpse_mfpool_format,
    kpse_mft_format,
    kpse_mp_format,
    kpse_mppool_format,
    kpse_mpsupport_format,
    kpse_ocp_format,
    kpse_ofm_format,
    kpse_opl_format,
    kpse_otp_format,
    kpse_ovf_format,
    kpse_ovp_format,
    kpse_pict_format,
    kpse_tex_format,
    kpse_texdoc_format,
    kpse_texpool_format,
    kpse_texsource_format,
    kpse_tex_ps_header_format,
    kpse_troff_font_format,
    kpse_type1_format,
    kpse_vf_format,
    kpse_dvips_config_format,
    kpse_ist_format,
    kpse_truetype_format,
    kpse_type42_format,
    kpse_web2c_format,
    kpse_program_text_format,
    kpse_program_binary_format,
    kpse_miscfonts_format,
    kpse_web_format,
    kpse_cweb_format,
    kpse_enc_format,
    kpse_cmap_format,
    kpse_sfd_format,
    kpse_opentype_format,
    kpse_pdftex_config_format,
    kpse_lig_format,
    kpse_texmfscripts_format,
    kpse_lua_format
};

static const char *const filetypenames[] = {
    "gf",
    "pk",
    "bitmap font",
    "tfm",
    "afm",
    "base",
    "bib",
    "bst",
    "cnf",
    "ls-R",
    "fmt",
    "map",
    "mem",
    "mf",
    "mfpool",
    "mft",
    "mp",
    "mppool",
    "MetaPost support",
    "ocp",
    "ofm",
    "opl",
    "otp",
    "ovf",
    "ovp",
    "graphic/figure",
    "tex",
    "TeX system documentation",
    "texpool",
    "TeX system sources",
    "PostScript header",
    "Troff fonts",
    "type1 fonts",
    "vf",
    "dvips config",
    "ist",
    "truetype fonts",
    "type42 fonts",
    "web2c files",
    "other text files",
    "other binary files",
    "misc fonts",
    "web",
    "cweb",
    "enc files",
    "cmap files",
    "subfont definition files",
    "opentype fonts",
    "pdftex config",
    "lig files",
    "texmfscripts",
    "lua",
    NULL
};


#define KPATHSEA_METATABLE  "luatex_kpathsea"

/* set to 1 by the |program_name| function */

int program_name_set = 0;

#define TEST_PROGRAM_NAME_SET do {                                      \
    if (! program_name_set) {                                           \
      lua_pushstring(L, "Please call kpse.set_program_name() before using the library"); \
      return lua_error(L);                                              \
    }                                                                   \
  } while (0)

static int find_file(lua_State * L)
{
    int i;
    char *st;
    int ftype = kpse_tex_format;
    int mexist = 0;
    TEST_PROGRAM_NAME_SET;
    if (!lua_isstring(L, 1)) {
        lua_pushstring(L, "not a file name");
        lua_error(L);
    }
    st = (char *) lua_tostring(L, 1);
    i = lua_gettop(L);
    while (i > 1) {
        if (lua_isboolean(L, i)) {
            mexist = lua_toboolean(L, i);
        } else if (lua_isnumber(L, i)) {
            mexist = lua_tonumber(L, i);
        } else if (lua_isstring(L, i)) {
            int op = luaL_checkoption(L, i, NULL, filetypenames);
            ftype = filetypes[op];
        }
        i--;
    }
    if (ftype == kpse_pk_format ||
        ftype == kpse_gf_format || ftype == kpse_any_glyph_format) {
        /* ret.format, ret.name, ret.dpi */
        kpse_glyph_file_type ret;
        lua_pushstring(L, kpse_find_glyph(st, mexist, ftype, &ret));
    } else {
        if (mexist > 0)
            mexist = 1;
        if (mexist < 0)
            mexist = 0;
        lua_pushstring(L, kpse_find_file(st, ftype, mexist));
    }
    return 1;
}


static int lua_kpathsea_find_file(lua_State * L)
{
    int i;
    int ftype = kpse_tex_format;
    int mexist = 0;
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    char *st = (char *) luaL_checkstring(L, 2);
    i = lua_gettop(L);
    while (i > 2) {
        if (lua_isboolean(L, i)) {
            mexist = lua_toboolean(L, i);
        } else if (lua_isnumber(L, i)) {
            mexist = lua_tonumber(L, i);
        } else if (lua_isstring(L, i)) {
            int op = luaL_checkoption(L, i, NULL, filetypenames);
            ftype = filetypes[op];
        }
        i--;
    }
    if (ftype == kpse_pk_format ||
        ftype == kpse_gf_format || ftype == kpse_any_glyph_format) {
        /* ret.format, ret.name, ret.dpi */
        kpse_glyph_file_type ret;
        lua_pushstring(L, kpathsea_find_glyph(*kp, st, mexist, ftype, &ret));
    } else {
        if (mexist > 0)
            mexist = 1;
        if (mexist < 0)
            mexist = 0;
        lua_pushstring(L, kpathsea_find_file(*kp, st, ftype, mexist));
    }
    return 1;

}

static int show_path(lua_State * L)
{
    int op = luaL_checkoption(L, -1, "tex", filetypenames);
    int user_format = filetypes[op];
    TEST_PROGRAM_NAME_SET;
    if (!kpse_format_info[user_format].type)    /* needed if arg was numeric */
        kpse_init_format(user_format);
    lua_pushstring(L, kpse_format_info[user_format].path);
    return 1;
}

static int lua_kpathsea_show_path(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    int op = luaL_checkoption(L, -1, "tex", filetypenames);
    int user_format = filetypes[op];
    if (!(*kp)->format_info[user_format].type)    /* needed if arg was numeric */
        kpathsea_init_format(*kp, user_format);
    lua_pushstring(L, (*kp)->format_info[user_format].path);
    return 1;
}

static int expand_path(lua_State * L)
{
    const char *st = luaL_checkstring(L, 1);
    TEST_PROGRAM_NAME_SET;
    lua_pushstring(L, kpse_path_expand(st));
    return 1;
}

static int lua_kpathsea_expand_path(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    const char *st = luaL_checkstring(L, 2);
    lua_pushstring(L, kpathsea_path_expand(*kp, st));
    return 1;
}

static int expand_braces(lua_State * L)
{
    const char *st = luaL_checkstring(L, 1);
    TEST_PROGRAM_NAME_SET;
    lua_pushstring(L, kpse_brace_expand(st));
    return 1;
}

static int lua_kpathsea_expand_braces(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    const char *st = luaL_checkstring(L, 2);
    lua_pushstring(L, kpathsea_brace_expand(*kp, st));
    return 1;
}


static int expand_var(lua_State * L)
{
    const char *st = luaL_checkstring(L, 1);
    TEST_PROGRAM_NAME_SET;
    lua_pushstring(L, kpse_var_expand(st));
    return 1;
}

static int lua_kpathsea_expand_var(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    const char *st = luaL_checkstring(L, 2);
    lua_pushstring(L, kpathsea_var_expand(*kp, st));
    return 1;
}


static int var_value(lua_State * L)
{
    const char *st = luaL_checkstring(L, 1);
    TEST_PROGRAM_NAME_SET;
    lua_pushstring(L, kpse_var_value(st));
    return 1;
}

static int lua_kpathsea_var_value(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    const char *st = luaL_checkstring(L, 2);
    lua_pushstring(L, kpathsea_var_value(*kp, st));
    return 1;
}


/* Engine support is a bit of a problem, because we do not want
 * to interfere with the normal format discovery of |luatex|.
 * Current approach: run |os.setenv()| if you have to.
 */

extern int luainit;

static int set_program_name(lua_State * L)
{
    const char *exe_name = luaL_checkstring(L, 1);
    const char *prog_name = luaL_optstring(L, 2, exe_name);
    if (!program_name_set) {
        kpse_set_program_name(exe_name, prog_name);
        program_name_set = 1;
    } else {
        kpse_reset_program_name(prog_name);
    }
    /* fix up the texconfig entry */
    lua_checkstack(L, 3);
    lua_getglobal(L, "texconfig");
    if (lua_istable(L, -1)) {
        lua_pushstring(L, "kpse_init");
        lua_pushboolean(L, 0);
        lua_rawset(L, -3);
    }
    lua_pop(L, 1);
    return 0;
}

static int init_prog(lua_State * L)
{
    const char *prefix = luaL_checkstring(L, 1);
    unsigned dpi = luaL_checkinteger(L, 2);
    const char *mode = luaL_checkstring(L, 3);
    const char *fallback = luaL_optstring(L, 4, NULL);
    TEST_PROGRAM_NAME_SET;
    kpse_init_prog(prefix, dpi, mode, fallback);
    return 0;
}

static int lua_kpathsea_init_prog(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    const char *prefix = luaL_checkstring(L, 2);
    unsigned dpi = luaL_checkinteger(L, 3);
    const char *mode = luaL_checkstring(L, 4);
    const char *fallback = luaL_optstring(L, 5, NULL);
    kpathsea_init_prog(*kp,prefix, dpi, mode, fallback);
    return 0;
}

static int readable_file(lua_State * L)
{
    const char *name = luaL_checkstring(L, 1);
    TEST_PROGRAM_NAME_SET;
    lua_pushstring(L, (char *) kpse_readable_file(name));
    return 1;
}

static int lua_kpathsea_readable_file(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    const char *name = luaL_checkstring(L, 2);
    lua_pushstring(L, (char *) kpathsea_readable_file(*kp,name));
    return 1;
}


static int lua_kpathsea_finish(lua_State * L)
{
    kpathsea *kp = (kpathsea *)luaL_checkudata(L, 1, KPATHSEA_METATABLE);
    kpathsea_finish(*kp);
    return 0;
}

static int lua_kpathsea_new(lua_State * L)
{
    kpathsea kpse = NULL;
    kpathsea *kp = NULL;
    char *argv = (char *)luaL_checkstring(L,1);
    char *liar = (char *)luaL_optstring(L,2,argv);
    kpse = kpathsea_new();
    kpathsea_set_program_name(kpse, argv, liar);
    kp = (kpathsea *)lua_newuserdata(L, sizeof(kpathsea *));
    *kp = kpse;
    luaL_getmetatable(L, KPATHSEA_METATABLE);
    lua_setmetatable(L, -2);    
    return 1;
}

static const struct luaL_reg kpselib_m[] = {
    {"__gc",          lua_kpathsea_finish        },
    {"init_prog",     lua_kpathsea_init_prog     },
    {"readable_file", lua_kpathsea_readable_file },
    {"find_file",     lua_kpathsea_find_file     },
    {"expand_path",   lua_kpathsea_expand_path   },
    {"expand_var",    lua_kpathsea_expand_var    },
    {"expand_braces", lua_kpathsea_expand_braces },
    {"var_value",     lua_kpathsea_var_value     },
    {"show_path",     lua_kpathsea_show_path     },
    {NULL, NULL}                /* sentinel */
};

static const struct luaL_reg kpselib_l[] = {
    {"new",              lua_kpathsea_new },
    {"set_program_name", set_program_name },
    {"init_prog",        init_prog        },
    {"readable_file",    readable_file    },
    {"find_file",        find_file        },
    {"expand_path",      expand_path      },
    {"expand_var",       expand_var       },
    {"expand_braces",    expand_braces    },
    {"var_value",        var_value        },
    {"show_path",        show_path        },
    {NULL, NULL}                /* sentinel */
};

int luaopen_kpse(lua_State * L)
{
    luaL_newmetatable(L, KPATHSEA_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2,"__index");
    luaL_register(L, NULL, kpselib_m);
    luaL_register(L, "kpse", kpselib_l);
    return 1;
}
