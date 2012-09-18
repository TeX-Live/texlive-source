/* luatex-api.h

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

/* $Id: luatex-api.h 4001 2010-11-28 15:49:46Z taco $ */

#ifndef LUATEX_API_H
#  define LUATEX_API_H 1

#  include <stdlib.h>
#  include <stdio.h>
#  include <stdarg.h>
#  include "lua51/lua.h"
#  include "lua51/lauxlib.h"
#  include "lua51/lualib.h"

typedef struct LoadS {
    char *s;
    size_t size;
} LoadS;

extern lua_State *Luas;

extern void make_table(lua_State * L, const char *tab, const char *getfunc,
                       const char *setfunc);

extern int luac_main(int argc, char *argv[]);

extern int luaopen_tex(lua_State * L);

extern int luaopen_pdf(lua_State * L);

#  define LUA_TEXFILEHANDLE               "TEXFILE*"

extern int luaopen_texio(lua_State * L);

extern int luaopen_lang(lua_State * L);

extern lua_State *luatex_error(lua_State * L, int fatal);

extern int luaopen_unicode(lua_State * L);
extern int luaopen_zip(lua_State * L);
extern int luaopen_lfs(lua_State * L);
extern int luaopen_lpeg(lua_State * L);
extern int luaopen_md5(lua_State * L);
extern int luatex_md5_lua_open(lua_State * L);

extern int luaopen_zlib(lua_State * L);
extern int luaopen_gzip(lua_State * L);
extern int luaopen_ff(lua_State * L);
extern int luaopen_profiler(lua_State * L);

extern int luaopen_socket_core(lua_State * L);
extern int luaopen_mime_core(lua_State * L);
extern void luatex_socketlua_open(lua_State * L);

extern int luaopen_img(lua_State * L);
extern int l_new_image(lua_State * L);
extern int luaopen_epdf(lua_State * L);
extern int luaopen_mplib(lua_State * L);

extern void open_oslibext(lua_State * L, int safer_option);
extern void open_lfslibext(lua_State * L);

extern void initfilecallbackids(int max);
extern void setinputfilecallbackid(int n, int i);
extern void setreadfilecallbackid(int n, int i);
extern int getinputfilecallbackid(int n);
extern int getreadfilecallbackid(int n);

extern void lua_initialize(int ac, char **av);

extern int luaopen_kpse(lua_State * L);

extern int luaopen_callback(lua_State * L);

extern int luaopen_lua(lua_State * L, char *fname);

extern int luaopen_stats(lua_State * L);

extern int luaopen_font(lua_State * L);
extern int font_to_lua(lua_State * L, int f);
extern int font_from_lua(lua_State * L, int f); /* return is boolean */

extern int luaopen_token(lua_State * L);
extern void tokenlist_to_lua(lua_State * L, int p);
extern void tokenlist_to_luastring(lua_State * L, int p);
extern int tokenlist_from_lua(lua_State * L);

extern void lua_nodelib_push(lua_State * L);
extern int nodelib_getdir(lua_State * L, int n);

extern int luaopen_node(lua_State * L);
extern void nodelist_to_lua(lua_State * L, int n);
extern int nodelist_from_lua(lua_State * L);

extern int dimen_to_number(lua_State * L, const char *s);

extern int get_command_id(const char *s);

extern void dump_luac_registers(void);

extern void undump_luac_registers(void);

extern int lua_only;

extern void unhide_lua_table(lua_State * lua, const char *name, int r);
extern int hide_lua_table(lua_State * lua, const char *name);

extern void unhide_lua_value(lua_State * lua, const char *name,
                             const char *item, int r);
extern int hide_lua_value(lua_State * lua, const char *name, const char *item);

typedef struct command_item_ {
    const char *cmd_name;
    int command_offset;
    const char **commands;
} command_item;

extern command_item command_names[];
extern int callback_callbacks_id;

extern void luainterpreter(void);

extern int luabytecode_max;
extern unsigned int luabytecode_bytes;
extern int luastate_bytes;

extern int callback_count;
extern int saved_callback_count;

extern const char *ptexbanner;

/* luastuff.h */

typedef struct {
    const char *name;           /* parameter name */
    int idx;                    /* index within img_parms array */
} parm_struct;

extern void preset_environment(lua_State * L, const parm_struct * p);

extern char *startup_filename;
extern int safer_option;
extern int nosocket_option;

extern char *last_source_name;
extern int last_lineno;

extern int program_name_set;    /* in lkpselib.c */

/* for topenin() */
extern char **argv;
extern int argc;

extern int loader_C_luatex(lua_State * L, const char *name,
                           const char *filename);
extern int loader_Call_luatex(lua_State * L, const char *name,
                              const char *filename);

extern void init_tex_table(lua_State * L);

extern int tex_table_id;
extern int pdf_table_id;
extern int token_table_id;
extern int node_table_id;
extern int main_initialize(void);

extern int do_run_callback(int special, const char *values, va_list vl);
extern int lua_traceback(lua_State * L);

extern int luainit;

extern char *luanames[];

extern int ff_get_ttc_index(char *ffname, char *psname);        /* luafontloader/src/luafflib.c */
extern int ff_createcff(char *, unsigned char **, int *);       /* luafontloader/src/luafflib.c */

extern char *FindResourceTtfFont(char *filename, char *fontname);       /* luafontloader/fontforge/fontforge/macbinary.c */

extern char charsetstr[];       /* from mpdir/psout.w */

#ifndef WIN32
extern char **environ;
#endif

extern int luac_main(int argc, char *argv[]);   /* texluac.w */

#endif                          /* LUATEX_API_H */
