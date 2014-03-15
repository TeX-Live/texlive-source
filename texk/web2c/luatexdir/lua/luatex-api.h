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

/* $Id: luatex-api.h 4877 2014-03-14 01:26:05Z luigi $ */

#ifndef LUATEX_API_H
#  define LUATEX_API_H 1

#  include <stdlib.h>
#  include <stdio.h>
#  include <stdarg.h>
#  include "lua.h"
#  include "lauxlib.h"
#  include "lualib.h"
#ifdef LuajitTeX
#  include "luajit.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LoadS {
    char *s;
    size_t size;
} LoadS;

extern lua_State *Luas;

extern void make_table(lua_State * L, const char *tab, const char *mttab, const char *getfunc,
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
extern int luaopen_bit32(lua_State * L);

extern int luaopen_socket_core(lua_State * L);
extern int luaopen_mime_core(lua_State * L);
extern void luatex_socketlua_open(lua_State * L);

extern int luaopen_img(lua_State * L);
extern int l_new_image(lua_State * L);
extern int luaopen_epdf(lua_State * L);
extern int luaopen_pdfscanner(lua_State * L);
extern int luaopen_mplib(lua_State * L);

extern void open_oslibext(lua_State * L, int safer_option);
extern int open_iolibext(lua_State * L);
extern void open_strlibext(lua_State * L);
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
extern int luaopen_vf(lua_State * L);
extern int font_to_lua(lua_State * L, int f);
extern int font_from_lua(lua_State * L, int f); /* return is boolean */

extern int luaopen_token(lua_State * L);
extern void tokenlist_to_lua(lua_State * L, int p);
extern void tokenlist_to_luastring(lua_State * L, int p);
extern int tokenlist_from_lua(lua_State * L);

extern void lua_nodelib_push(lua_State * L);
extern int nodelib_getdir(lua_State * L, int n, int absolute_only);

extern int luaopen_node(lua_State * L);
extern void nodelist_to_lua(lua_State * L, int n);
extern int nodelist_from_lua(lua_State * L);

extern int dimen_to_number(lua_State * L, const char *s);

extern int get_command_id(const char *s);

extern void dump_luac_registers(void);

extern void undump_luac_registers(void);

extern int lua_only;
#ifdef LuajitTeX
extern int luajiton;
extern char *jithash_hashname ;
LUA_API int luajittex_choose_hash_function  ;
#endif


#ifdef LuajitTeX
#define LUAJITTEX_HASHCHARS 6 /* todo: It must be like that one on lj_str.c */
#else 
#define LUATEX_HASHCHARS 6  /* todo: It must be LUAI_HASHLIMIT! */
#endif 
extern unsigned char show_luahashchars ;

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

extern void preset_environment(lua_State * L, const parm_struct * p,
                               const char *s);

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

#ifdef __cplusplus
}
#endif

/*
    Same as in lnodelib.c, but with prefix G_ for now.
    These macros create and access pointers (indices) to keys which is faster. The
    shortcuts are created as part of the initialization.

*/

/*#define init_luaS_index(a) do {                         */
#define init_lua_key(a) do {                      \
    lua_pushliteral(Luas,#a);                             \
    luaS_##a##_ptr = lua_tostring(Luas,-1);               \
    luaS_##a##_index = luaL_ref (Luas,LUA_REGISTRYINDEX); \
} while (0)

  /*#define init_luaS_index_s(a,b) do {           */      
#define init_lua_key_alias(a,b) do {              \
    lua_pushliteral(Luas,b);                              \
    luaS_##a##_ptr = lua_tostring(Luas,-1);               \
    luaS_##a##_index = luaL_ref (Luas,LUA_REGISTRYINDEX); \
} while (0)

  /*#define make_luaS_index(a) */                       
#define make_lua_key(a)       \
    int luaS_##a##_index = 0;          \
    const char * luaS_##a##_ptr = NULL


/*#define luaS_ptr_eq(a,b) (a==luaS_##b##_ptr)*/
#define lua_key_eq(a,b) (a==luaS_##b##_ptr)

#define luaS_index(a)    luaS_##a##_index
#define lua_key_index(a) luaS_##a##_index
#define lua_key(a) luaS_##a##_ptr
#define use_lua_key(a)  \
  extern int luaS_##a##_index ;          \
  extern const char * luaS_##a##_ptr 


#define lua_key_rawgeti(a) \
  lua_rawgeti(L, LUA_REGISTRYINDEX, luaS_##a##_index);\
  lua_rawget(L, -2)


#define lua_roundnumber(a,b) (int)floor((double)lua_tonumber(L,-1)+0.5)
extern int lua_numeric_field_by_index(lua_State *, int , int);
 

#endif                          /* LUATEX_API_H */




/*                                                 */  
/* These keys have to available to different files */
/*                                                 */ 


use_lua_key(LTL);
use_lua_key(MathConstants);
use_lua_key(RTT);
use_lua_key(TLT);
use_lua_key(TRT);
use_lua_key(accent);
use_lua_key(action);
use_lua_key(action_id);
use_lua_key(action_type);
use_lua_key(additional);
use_lua_key(adjust_head);
use_lua_key(advance);
use_lua_key(aleph);
use_lua_key(align_head);
use_lua_key(area);
use_lua_key(attr);
use_lua_key(attributes);
use_lua_key(auto_expand);
use_lua_key(best_ins_ptr);
use_lua_key(best_page_break);
use_lua_key(best_size);
use_lua_key(bot);
use_lua_key(bot_accent);
use_lua_key(bottom_left);
use_lua_key(bottom_right);
use_lua_key(box_left);
use_lua_key(box_left_width);
use_lua_key(box_right);
use_lua_key(box_right_width);
use_lua_key(broken_ins);
use_lua_key(broken_ptr);
use_lua_key(cache);
use_lua_key(cal_expand_ratio);
use_lua_key(char);
use_lua_key(characters);
use_lua_key(checksum);
use_lua_key(cidinfo);
use_lua_key(class);
use_lua_key(command);
use_lua_key(commands);
use_lua_key(comment);
use_lua_key(components);
use_lua_key(contrib_head);
use_lua_key(core);
use_lua_key(cost);
use_lua_key(count);
use_lua_key(data);
use_lua_key(degree);
use_lua_key(delim);
use_lua_key(delimptr);
use_lua_key(denom);
use_lua_key(depth);
use_lua_key(designsize);
use_lua_key(dest_id);
use_lua_key(dest_type);
use_lua_key(dir);
use_lua_key(dir_h);
use_lua_key(direction);
use_lua_key(dirs);
use_lua_key(display);
use_lua_key(down);
use_lua_key(dvi_ptr);
use_lua_key(embedding);
use_lua_key(encodingbytes);
use_lua_key(encodingname);
use_lua_key(end);
use_lua_key(etex);
use_lua_key(exactly);
use_lua_key(expansion_factor);
use_lua_key(ext);
use_lua_key(extend);
use_lua_key(extender);
use_lua_key(extensible);
use_lua_key(extra_space);
use_lua_key(fam);
use_lua_key(fast);
use_lua_key(file);
use_lua_key(filename);
use_lua_key(font);
use_lua_key(fonts);
use_lua_key(format);
use_lua_key(fullname);
use_lua_key(global);
use_lua_key(glue_order);
use_lua_key(glue_set);
use_lua_key(glue_sign);
use_lua_key(glyph);
use_lua_key(head);
use_lua_key(height);
use_lua_key(hold_head);
use_lua_key(horiz_variants);
use_lua_key(hyphenchar);
use_lua_key(id);
use_lua_key(image);
use_lua_key(index);
use_lua_key(italic);
use_lua_key(kern);
use_lua_key(kerns);
use_lua_key(lang);
use_lua_key(large_char);
use_lua_key(large_fam);
use_lua_key(last_ins_ptr);
use_lua_key(leader);
use_lua_key(least_page_cost);
use_lua_key(left);
use_lua_key(left_boundary);
use_lua_key(left_protruding);
use_lua_key(level);
use_lua_key(ligatures);
use_lua_key(link_attr);
use_lua_key(list);
use_lua_key(log);
use_lua_key(lua);
use_lua_key(luatex);
use_lua_key(luatex_node);
use_lua_key(mLTL);
use_lua_key(mRTT);
use_lua_key(mTLT);
use_lua_key(mTRT);
use_lua_key(mark);
use_lua_key(mathdir);
use_lua_key(mathkern);
use_lua_key(mathstyle);
use_lua_key(mid);
use_lua_key(mode);
use_lua_key(modeline);
use_lua_key(name);
use_lua_key(named_id);
use_lua_key(new_window);
use_lua_key(next);
use_lua_key(no);
use_lua_key(noad);
use_lua_key(node);
use_lua_key(node_properties);
use_lua_key(node_properties_indirect);
use_lua_key(nomath);
use_lua_key(nop);
use_lua_key(nucleus);
use_lua_key(num);
use_lua_key(number);
use_lua_key(objnum);
use_lua_key(omega);
use_lua_key(ordering);
use_lua_key(pLTL);
use_lua_key(pRTT);
use_lua_key(pTLT);
use_lua_key(pTRT);
use_lua_key(page_head);
use_lua_key(page_ins_head);
use_lua_key(parameters);
use_lua_key(pdftex);
use_lua_key(pen_broken);
use_lua_key(pen_inter);
use_lua_key(penalty);
use_lua_key(pop);
use_lua_key(post);
use_lua_key(pre);
use_lua_key(pre_adjust_head);
use_lua_key(prev);
use_lua_key(prevdepth);
use_lua_key(prevgraf);
use_lua_key(psname);
use_lua_key(ptr);
use_lua_key(push);
use_lua_key(quad);
use_lua_key(ref_count);
use_lua_key(reg);
use_lua_key(registry);
use_lua_key(renew);
use_lua_key(rep);
use_lua_key(replace);
use_lua_key(right);
use_lua_key(right_boundary);
use_lua_key(right_protruding);
use_lua_key(rule);
use_lua_key(scale);
use_lua_key(script);
use_lua_key(scriptscript);
use_lua_key(shift);
use_lua_key(shrink);
use_lua_key(shrink_order);
use_lua_key(size);
use_lua_key(skewchar);
use_lua_key(slant);
use_lua_key(slot);
use_lua_key(small_char);
use_lua_key(small_fam);
use_lua_key(space);
use_lua_key(space_shrink);
use_lua_key(space_stretch);
use_lua_key(spacefactor);
use_lua_key(spec);
use_lua_key(special);
use_lua_key(stack);
use_lua_key(start);
use_lua_key(step);
use_lua_key(stream);
use_lua_key(stretch);
use_lua_key(stretch_order);
use_lua_key(string);
use_lua_key(style);
use_lua_key(sub);
use_lua_key(subst_ex_font);
use_lua_key(subtype);
use_lua_key(sup);
use_lua_key(supplement);
use_lua_key(surround);
use_lua_key(tail);
use_lua_key(temp_head);
use_lua_key(term);
use_lua_key(term_and_log);
use_lua_key(tex);
use_lua_key(text);
use_lua_key(thread_attr);
use_lua_key(thread_id);
use_lua_key(top);
use_lua_key(top_accent);
use_lua_key(top_left);
use_lua_key(top_right);
use_lua_key(tounicode);
use_lua_key(transform);
use_lua_key(type);
use_lua_key(uchyph);
use_lua_key(umath);
use_lua_key(units_per_em);
use_lua_key(used);
use_lua_key(user_id);
use_lua_key(value);
use_lua_key(version);
use_lua_key(vert_variants);
use_lua_key(width);
use_lua_key(writable);
use_lua_key(x_height);
use_lua_key(xoffset);
use_lua_key(xyz_zoom);
use_lua_key(yoffset);

use_lua_key(immediate);
use_lua_key(compresslevel);
use_lua_key(objcompression);
use_lua_key(direct);
use_lua_key(page);
use_lua_key(streamfile);
use_lua_key(annot);
use_lua_key(lua_functions);



