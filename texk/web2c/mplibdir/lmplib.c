/* lmplib.c
   
   Copyright 2006-2009 Taco Hoekwater <taco@luatex.org>

   This file is part of LuaTeX.

   LuaTeX is free software; you can redistribute it and/or modify it under
   the terms of the GNU Lesser General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your
   option) any later version.

   LuaTeX is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
   License for more details.

   You should have received a copy of the GNU Lesser General Public License along
   with LuaTeX; if not, see <http://www.gnu.org/licenses/>. */

#include <w2c/config.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h> /* temporary */

#ifndef pdfTeX
#  include <lua.h>
#  include <lauxlib.h>
#  include <lualib.h>
#else
#  include <../lua51/lua.h>
#  include <../lua51/lauxlib.h>
#  include <../lua51/lualib.h>
#endif

#include "mplib.h"
#include "mplibps.h"
#include "mplibsvg.h"

   /*@unused@*/ static const char _svn_version[] =
    "$Id: lmplib.c 1364 2008-07-04 16:09:46Z taco $ $URL: http://scm.foundry.supelec.fr/svn/luatex/trunk/src/texk/web2c/luatexdir/lua/lmplib.c $";

int luaopen_mplib(lua_State * L); /* forward */

/* metatable identifiers and tests */

#define MPLIB_METATABLE     "MPlib"
#define MPLIB_FIG_METATABLE "MPlib.fig"
#define MPLIB_GR_METATABLE  "MPlib.gr"

#define is_mp(L,b) (MP *)luaL_checkudata(L,b,MPLIB_METATABLE)
#define is_fig(L,b) (struct mp_edge_object **)luaL_checkudata(L,b,MPLIB_FIG_METATABLE)
#define is_gr_object(L,b) (struct mp_graphic_object **)luaL_checkudata(L,b,MPLIB_GR_METATABLE)

/* Lua string pre-hashing */

#define mplib_init_S(a) do {                                            \
    lua_pushliteral(L,#a);                                              \
    mplib_##a##_ptr = lua_tostring(L,-1);                       \
    mplib_##a##_index = luaL_ref (L,LUA_REGISTRYINDEX);                 \
  } while (0)

#define mplib_push_S(a) do {                                    \
    lua_rawgeti(L,LUA_REGISTRYINDEX,mplib_##a##_index);         \
  } while (0)

#define mplib_is_S(a,i) (mplib_##a##_ptr==lua_tostring(L,i))

#define mplib_make_S(a)                                                 \
  static int mplib_##a##_index = 0;                                     \
  static const char *mplib_##a##_ptr = NULL

static int mplib_type_Ses[mp_special_code + 1] = { 0 }; /* [0] is not used */

mplib_make_S(fill);
mplib_make_S(outline);
mplib_make_S(text);
mplib_make_S(special);
mplib_make_S(start_bounds);
mplib_make_S(stop_bounds);
mplib_make_S(start_clip);
mplib_make_S(stop_clip);

mplib_make_S(left_type);
mplib_make_S(right_type);
mplib_make_S(x_coord);
mplib_make_S(y_coord);
mplib_make_S(left_x);
mplib_make_S(left_y);
mplib_make_S(right_x);
mplib_make_S(right_y);

mplib_make_S(color);
mplib_make_S(dash);
mplib_make_S(depth);
mplib_make_S(dsize);
mplib_make_S(font);
mplib_make_S(height);
mplib_make_S(htap);
mplib_make_S(linecap);
mplib_make_S(linejoin);
mplib_make_S(miterlimit);
mplib_make_S(path);
mplib_make_S(pen);
mplib_make_S(postscript);
mplib_make_S(prescript);
mplib_make_S(transform);
mplib_make_S(type);
mplib_make_S(width);

static void mplib_init_Ses(lua_State * L)
{
    mplib_init_S(fill);
    mplib_init_S(outline);
    mplib_init_S(text);
    mplib_init_S(start_bounds);
    mplib_init_S(stop_bounds);
    mplib_init_S(start_clip);
    mplib_init_S(stop_clip);
    mplib_init_S(special);

    mplib_type_Ses[mp_fill_code] = mplib_fill_index;
    mplib_type_Ses[mp_stroked_code] = mplib_outline_index;
    mplib_type_Ses[mp_text_code] = mplib_text_index;
    mplib_type_Ses[mp_start_bounds_code] = mplib_start_bounds_index;
    mplib_type_Ses[mp_stop_bounds_code] = mplib_stop_bounds_index;
    mplib_type_Ses[mp_start_clip_code] = mplib_start_clip_index;
    mplib_type_Ses[mp_stop_clip_code] = mplib_stop_clip_index;
    mplib_type_Ses[mp_special_code] = mplib_special_index;

    mplib_init_S(left_type);
    mplib_init_S(right_type);
    mplib_init_S(x_coord);
    mplib_init_S(y_coord);
    mplib_init_S(left_x);
    mplib_init_S(left_y);
    mplib_init_S(right_x);
    mplib_init_S(right_y);

    mplib_init_S(color);
    mplib_init_S(dash);
    mplib_init_S(depth);
    mplib_init_S(dsize);
    mplib_init_S(font);
    mplib_init_S(height);
    mplib_init_S(htap);
    mplib_init_S(linecap);
    mplib_init_S(linejoin);
    mplib_init_S(miterlimit);
    mplib_init_S(path);
    mplib_init_S(pen);
    mplib_init_S(postscript);
    mplib_init_S(prescript);
    mplib_init_S(transform);
    mplib_init_S(type);
    mplib_init_S(width);
}


/* Enumeration arrays to map MPlib enums to Lua strings */

static const char *interaction_options[] =
    { "unknown", "batch", "nonstop", "scroll", "errorstop", NULL };

static const char *mplib_filetype_names[] =
    { "term", "error", "mp", "log", "ps", "mem", "tfm", "map", "pfb", "enc", NULL };

static const char *knot_type_enum[] =
    { "endpoint", "explicit", "given", "curl", "open", "end_cycle" };

static const char *fill_fields[] =
    { "type", "path", "htap", "pen", "color", "linejoin", "miterlimit",
    "prescript", "postscript", NULL };

static const char *stroked_fields[] =
    { "type", "path", "pen", "color", "linejoin", "miterlimit", "linecap",
      "dash", "prescript", "postscript", NULL };

static const char *text_fields[] =
    { "type", "text", "dsize", "font", "color", "width", "height", "depth",
      "transform", "prescript", "postscript", NULL };

static const char *special_fields[] =
    { "type", "prescript", NULL };

static const char *start_bounds_fields[] =
    { "type", "path", NULL };

static const char *start_clip_fields[] = 
    { "type", "path", NULL };

static const char *stop_bounds_fields[] = 
    { "type", NULL };

static const char *stop_clip_fields[] = 
    { "type", NULL };

static const char *no_fields[] = 
    { NULL };


/* The list of supported MPlib options (not all make sense) */

typedef enum {
    P_ERROR_LINE, P_MAX_LINE, 
    P_MAIN_MEMORY, P_HASH_SIZE, P_PARAM_SIZE, P_IN_OPEN, P_RANDOM_SEED,
    P_INTERACTION, P_INI_VERSION, P_MEM_NAME, P_JOB_NAME, P_FIND_FILE, 
    P__SENTINEL } mplib_parm_idx;

typedef struct {
    const char *name;           /* parameter name */
    mplib_parm_idx idx;         /* parameter index */
} mplib_parm_struct;

static mplib_parm_struct mplib_parms[] = {
    {"error_line",        P_ERROR_LINE  },
    {"print_line",        P_MAX_LINE    },
    {"main_memory",       P_MAIN_MEMORY },
    {"hash_size",         P_HASH_SIZE   },
    {"param_size",        P_PARAM_SIZE  },
    {"max_in_open",       P_IN_OPEN     },
    {"random_seed",       P_RANDOM_SEED },
    {"interaction",       P_INTERACTION },
    {"ini_version",       P_INI_VERSION },
    {"mem_name",          P_MEM_NAME    },
    {"job_name",          P_JOB_NAME    },
    {"find_file",         P_FIND_FILE   },
    {NULL,                P__SENTINEL   }
};


/* Start by defining the needed callback routines for the library  */

static char *mplib_find_file(MP mp, const char *fname, const char *fmode, int ftype)
{
    lua_State *L = (lua_State *)mp_userdata(mp);
    lua_checkstack(L, 4);
    lua_getfield(L, LUA_REGISTRYINDEX, "mplib_file_finder");
    if (lua_isfunction(L, -1)) {
        char *s = NULL;
	const char *x = NULL;
        lua_pushstring(L, fname);
        lua_pushstring(L, fmode);
        if (ftype >= mp_filetype_text) {
          lua_pushnumber(L, (lua_Number)(ftype - mp_filetype_text));
        } else {
            lua_pushstring(L, mplib_filetype_names[ftype]);
        }
        if (lua_pcall(L, 3, 1, 0) != 0) {
            fprintf(stdout, "Error in mp.find_file: %s\n",
                    lua_tostring(L, -1));
            return NULL;
        }
        x = lua_tostring(L, -1);
        if (x != NULL)
            s = strdup(x);
        lua_pop(L, 1);          /* pop the string */
        return s;
    } else {
        lua_pop(L, 1);
    }
    if (fmode[0] != 'r' || (!access(fname, R_OK)) || ftype) {
        return strdup(fname);
    }
    return NULL;
}

static int mplib_find_file_function(lua_State * L)
{
    if (!(lua_isfunction(L, -1) || lua_isnil(L, -1))) {
        return 1;               /* error */
    }
    lua_pushstring(L, "mplib_file_finder");
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    return 0;
}

#define xfree(A) if ((A)!=NULL) { free((A)); A = NULL; }

static int mplib_new(lua_State * L)
{
    MP *mp_ptr;
    mp_ptr = lua_newuserdata(L, sizeof(MP *));
    if (mp_ptr) {
        int i;
        struct MP_options *options = mp_options();
        options->userdata = (void *) L;
        options->noninteractive = 1;    /* required ! */
        options->find_file = mplib_find_file;
        options->print_found_names = 1;
        if (lua_type(L, 1) == LUA_TTABLE) {
            for (i = 0; mplib_parms[i].name != NULL; i++) {
                lua_getfield(L, 1, mplib_parms[i].name);
                if (lua_isnil(L, -1)) {
                    lua_pop(L, 1);
                    continue;   /* skip unset */
                }
                switch (mplib_parms[i].idx) {
                case P_ERROR_LINE:
                  options->error_line = (int)lua_tointeger(L, -1);
                    if (options->error_line<60) options->error_line =60;
                    if (options->error_line>250) options->error_line = 250;
                    options->half_error_line = (options->error_line/2)+10;
                    break;
                case P_MAX_LINE:
                    options->max_print_line = (int)lua_tointeger(L, -1);
                    if (options->max_print_line<60) options->max_print_line = 60;
                    break;
                case P_MAIN_MEMORY:
                    options->main_memory = (int)lua_tointeger(L, -1);
                    break;
                case P_HASH_SIZE:
                    options->hash_size = (unsigned)lua_tointeger(L, -1);
                    break;
                case P_PARAM_SIZE:
                    options->param_size = (int)lua_tointeger(L, -1);
                    break;
                case P_IN_OPEN:
                    options->max_in_open = (int)lua_tointeger(L, -1);
                    break;
                case P_RANDOM_SEED:
                  options->random_seed = (int)lua_tointeger(L, -1);
                    break;
                case P_INTERACTION:
                    options->interaction =
                        luaL_checkoption(L, -1, "errorstopmode",
                                         interaction_options);
                    break;
                case P_INI_VERSION:
                    options->ini_version = lua_toboolean(L, -1);
                    break;
                case P_MEM_NAME:
                    options->mem_name = strdup(lua_tostring(L, -1));
                    break;
                case P_JOB_NAME:
                    options->job_name = strdup(lua_tostring(L, -1));
                    break;
                case P_FIND_FILE:
                    if (mplib_find_file_function(L)) {  /* error here */
                        fprintf(stdout,
                                "Invalid arguments to mp.new({find_file=...})\n");
                    }
                    break;
                default:
                    break;
                }
                lua_pop(L, 1);
            }
        }
        *mp_ptr = mp_initialize(options);
        xfree(options->command_line);
        xfree(options->mem_name);
        free(options);
        if (*mp_ptr) {
            luaL_getmetatable(L, MPLIB_METATABLE);
            lua_setmetatable(L, -2);
            return 1;
        }
    }
    lua_pushnil(L);
    return 1;
}

static int mplib_collect(lua_State * L)
{
    MP *mp_ptr = is_mp(L, 1);
    if (*mp_ptr != NULL) {
      (void)mp_finish(*mp_ptr);
      *mp_ptr = NULL;
    }
    return 0;
}

static int mplib_tostring(lua_State * L)
{
    MP *mp_ptr = is_mp(L, 1);
    if (*mp_ptr != NULL) {
      (void)lua_pushfstring(L, "<MP %p>", *mp_ptr);
        return 1;
    }
    return 0;
}

static int mplib_wrapresults(lua_State * L, mp_run_data *res, int status)
{
    lua_checkstack(L, 5);
    lua_newtable(L);
    if (res->term_out.size != 0) {
        lua_pushstring(L, res->term_out.data);
        lua_setfield(L, -2, "term");
    }
    if (res->error_out.size != 0) {
        lua_pushstring(L, res->error_out.data);
        lua_setfield(L, -2, "error");
    }
    if (res->log_out.size != 0) {
        lua_pushstring(L, res->log_out.data);
        lua_setfield(L, -2, "log");
    }
    if (res->edges != NULL) {
        struct mp_edge_object **v;
        struct mp_edge_object *p = res->edges;
        int i = 1;
        lua_newtable(L);
        while (p != NULL) {
            v = lua_newuserdata(L, sizeof(struct mp_edge_object *));
            *v = p;
            luaL_getmetatable(L, MPLIB_FIG_METATABLE);
            lua_setmetatable(L, -2);
            lua_rawseti(L, -2, i);
            i++;
            p = p->next;
        }
        lua_setfield(L, -2, "fig");
        res->edges = NULL;
    }
    lua_pushnumber(L, (lua_Number)status);
    lua_setfield(L, -2, "status");
    return 1;
}

static int mplib_execute(lua_State * L)
{
    MP *mp_ptr; 
    if (lua_gettop(L)!=2) {
        lua_pushnil(L);
	return 1;
    }
    mp_ptr = is_mp(L, 1);
    if (*mp_ptr != NULL && lua_isstring(L, 2)) {
        size_t l;
        char *s = xstrdup(lua_tolstring(L, 2, &l));
        int h = mp_execute(*mp_ptr, s, l);
        mp_run_data *res = mp_rundata(*mp_ptr);
	xfree(s);
        return mplib_wrapresults(L, res, h);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mplib_finish(lua_State * L)
{
    MP *mp_ptr = is_mp(L, 1);
    if (*mp_ptr != NULL) {
      int i;
      int h = mp_execute(*mp_ptr,NULL,0);
      mp_run_data *res = mp_rundata(*mp_ptr);
      i = mplib_wrapresults(L, res, h);
      (void)mp_finish(*mp_ptr);
       *mp_ptr = NULL;
       return i;
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mplib_char_dimension(lua_State * L, int t)
{
  MP *mp_ptr = is_mp(L, 1);
  if (*mp_ptr != NULL) {
    char *fname = xstrdup(luaL_checkstring(L,2));
    int charnum = (int)luaL_checkinteger(L,3);
    if (charnum<0 || charnum>255) {
      lua_pushnumber(L, (lua_Number)0);
    } else {
      lua_pushnumber(L,(lua_Number)mp_get_char_dimension(*mp_ptr,fname,charnum,t));
    }
    xfree(fname);
  } else {
    lua_pushnumber(L, (lua_Number)0);
  }
  return 1;
}

static int mplib_charwidth(lua_State * L) 
{
  return mplib_char_dimension(L, 'w');
}

static int mplib_chardepth(lua_State * L) 
{
  return mplib_char_dimension(L, 'd');
}

static int mplib_charheight(lua_State * L) 
{
  return mplib_char_dimension(L, 'h');
}

static int mplib_version(lua_State * L)
{
  char *s = mp_metapost_version();
  lua_pushstring(L, s);
  free(s);
  return 1;
}

static int mplib_statistics(lua_State * L)
{
    MP *mp_ptr = is_mp(L, 1);
    if (*mp_ptr != NULL) {
        lua_newtable(L);
        lua_pushnumber(L, (lua_Number)mp_memory_usage(*mp_ptr));
        lua_setfield(L, -2, "main_memory");
        lua_pushnumber(L, (lua_Number)mp_hash_usage(*mp_ptr));
        lua_setfield(L, -2, "hash_size");
        lua_pushnumber(L, (lua_Number)mp_param_usage(*mp_ptr));
        lua_setfield(L, -2, "param_size");
        lua_pushnumber(L, (lua_Number)mp_open_usage(*mp_ptr));
        lua_setfield(L, -2, "max_in_open");
    } else {
        lua_pushnil(L);
    }
    return 1;
}


/* figure methods */

static int mplib_fig_collect(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    if (*hh != NULL) {
        mp_gr_toss_objects(*hh);
        *hh = NULL;
    }
    return 0;
}

static int mplib_fig_body(lua_State * L)
{
    int i = 1;
    struct mp_graphic_object **v;
    struct mp_graphic_object *p;
    struct mp_edge_object **hh = is_fig(L, 1);
    lua_newtable(L);
    p = (*hh)->body;
    while (p != NULL) {
        v = lua_newuserdata(L, sizeof(struct mp_graphic_object *));
        *v = p;
        luaL_getmetatable(L, MPLIB_GR_METATABLE);
        lua_setmetatable(L, -2);
        lua_rawseti(L, -2, i);
        i++;
        p = p->next;
    }
    (*hh)->body = NULL;         /* prevent double free */
    return 1;
}

static int mplib_fig_copy_body(lua_State * L)
{
    int i = 1;
    struct mp_graphic_object **v;
    struct mp_graphic_object *p;
    struct mp_edge_object **hh = is_fig(L, 1);
    lua_newtable(L);
    p = (*hh)->body;
    while (p != NULL) {
        v = lua_newuserdata(L, sizeof(struct mp_graphic_object *));
        *v = mp_gr_copy_object((*hh)->parent, p);
        luaL_getmetatable(L, MPLIB_GR_METATABLE);
        lua_setmetatable(L, -2);
        lua_rawseti(L, -2, i);
        i++;
        p = p->next;
    }
    return 1;
}


static int mplib_fig_tostring(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    (void)lua_pushfstring(L, "<figure %p>", *hh);
    return 1;
}

static int mplib_fig_postscript(lua_State * L)
{
    mp_run_data *res;
    struct mp_edge_object **hh = is_fig(L, 1);
    int prologues = (int)luaL_optnumber(L, 2, (lua_Number)-1);
    int procset = (int)luaL_optnumber(L, 3, (lua_Number)-1);
    if (mp_ps_ship_out(*hh, prologues, procset) 
        && (res = mp_rundata((*hh)->parent))
        && (res->ps_out.size != 0)) {
        lua_pushstring(L, res->ps_out.data);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mplib_fig_svg(lua_State * L)
{
    mp_run_data *res;
    struct mp_edge_object **hh = is_fig(L, 1);
    int prologues = (int)luaL_optnumber(L, 2, (lua_Number)-1);
    if (mp_svg_ship_out(*hh, prologues) 
        && (res = mp_rundata((*hh)->parent))
        && (res->ps_out.size != 0)) {
        lua_pushstring(L, res->ps_out.data);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mplib_fig_filename(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    if (*hh != NULL) {
        char *s = (*hh)->filename;
        lua_pushstring(L, s);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int mplib_fig_width(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    if (*hh != NULL) {
      lua_pushnumber(L, (double) (*hh)->width / 65536.0);
    } else {
      lua_pushnil(L);
    }
    return 1;
}

static int mplib_fig_height(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    if (*hh != NULL) {
      lua_pushnumber(L, (double) (*hh)->height / 65536.0);
    } else {
      lua_pushnil(L);
    }
    return 1;
}

static int mplib_fig_depth(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    if (*hh != NULL) {
      lua_pushnumber(L, (double) (*hh)->depth / 65536.0);
    } else {
      lua_pushnil(L);
    }
    return 1;
}

static int mplib_fig_italcorr(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    if (*hh != NULL) {
      lua_pushnumber(L, (double) (*hh)->ital_corr / 65536.0);
    } else {
      lua_pushnil(L);
    }
    return 1;
}

static int mplib_fig_charcode(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    if (*hh != NULL) {
      lua_pushnumber(L, (lua_Number)(*hh)->charcode);
    } else {
      lua_pushnil(L);
    }
    return 1;
}



static int mplib_fig_bb(lua_State * L)
{
    struct mp_edge_object **hh = is_fig(L, 1);
    lua_newtable(L);
    lua_pushnumber(L, (double) (*hh)->minx / 65536.0);
    lua_rawseti(L, -2, 1);
    lua_pushnumber(L, (double) (*hh)->miny / 65536.0);
    lua_rawseti(L, -2, 2);
    lua_pushnumber(L, (double) (*hh)->maxx / 65536.0);
    lua_rawseti(L, -2, 3);
    lua_pushnumber(L, (double) (*hh)->maxy / 65536.0);
    lua_rawseti(L, -2, 4);
    return 1;
}

/* object methods */

static int mplib_gr_collect(lua_State * L)
{
    struct mp_graphic_object **hh = is_gr_object(L, 1);
    if (*hh != NULL) {
        mp_gr_toss_object(*hh);
        *hh = NULL;
    }
    return 0;
}

static int mplib_gr_tostring(lua_State * L)
{
    struct mp_graphic_object **hh = is_gr_object(L, 1);
    (void)lua_pushfstring(L, "<object %p>", *hh);
    return 1;
}

#define pyth(a,b) (sqrt((a)*(a) + (b)*(b)))

#define aspect_bound   (10.0/65536.0)
#define aspect_default (1.0/65536.0)

static double eps  = 0.0001;

static double coord_range_x (mp_knot *h, double dz) {
  double z;
  double zlo = 0.0, zhi = 0.0;
  mp_knot *f = h; 
  while (h != NULL) {
    z = (double)h->x_coord;
    if (z < zlo) zlo = z; else if (z > zhi) zhi = z;
    z = (double)h->right_x;
    if (z < zlo) zlo = z; else if (z > zhi) zhi = z;
    z = (double)h->left_x;
    if (z < zlo) zlo = z; else if (z > zhi) zhi = z;
    h = h->next;
    if (h==f)
      break;
  }
  return (zhi - zlo <= dz ? aspect_bound : aspect_default);
}

static double coord_range_y (mp_knot *h, double dz) {
  double z;
  double zlo = 0.0, zhi = 0.0;
  mp_knot *f = h; 
  while (h != NULL) {
    z = (double)h->y_coord;
    if (z < zlo) zlo = z; else if (z > zhi) zhi = z;
    z = (double)h->right_y;
    if (z < zlo) zlo = z; else if (z > zhi) zhi = z;
    z = (double)h->left_y;
    if (z < zlo) zlo = z; else if (z > zhi) zhi = z;
    h = h->next;
    if (h==f)
      break;
  }
  return (zhi - zlo <= dz ? aspect_bound : aspect_default);
}


static int mplib_gr_peninfo(lua_State * L) {
    double x_coord, y_coord, left_x, left_y, right_x, right_y;
    double wx, wy;
    double rx = 1.0, sx = 0.0, sy = 0.0, ry = 1.0, tx = 0.0, ty = 0.0;
    double divider = 1.0;
    double width = 1.0;
    mp_knot *p = NULL, *path = NULL;
    struct mp_graphic_object **hh = is_gr_object(L, -1);
    if (!*hh) {
      lua_pushnil(L);
      return 1;
    }
    if ((*hh)->type == mp_fill_code) {
      p    = ((mp_fill_object *)(*hh))->pen_p;
      path = ((mp_fill_object *)(*hh))->path_p;
    } else if ((*hh)->type == mp_stroked_code) {
      p    = ((mp_stroked_object *)(*hh))->pen_p;
      path = ((mp_stroked_object *)(*hh))->path_p;
    }
    if (p==NULL || path == NULL) {
      lua_pushnil(L);
      return 1;
    }
    x_coord = p->x_coord/65536.0;
    y_coord = p->y_coord/65536.0;
    left_x = p->left_x/65536.0;
    left_y = p->left_y/65536.0;
    right_x = p->right_x/65536.0;
    right_y = p->right_y/65536.0;
    if ((right_x == x_coord) && (left_y == y_coord)) {
      wx = fabs(left_x  - x_coord);
      wy = fabs(right_y - y_coord);
    } else {
      wx = pyth(left_x - x_coord, right_x - x_coord);
      wy = pyth(left_y - y_coord, right_y - y_coord);
    }
    if ((wy/coord_range_x(path, wx)) >= (wx/coord_range_y(path, wy)))
      width = wy;
    else
      width = wx;
    tx = x_coord; 
    ty = y_coord;
    sx = left_x - tx; 
    rx = left_y - ty; 
    ry = right_x - tx; 
    sy = right_y - ty;
    if (width !=1.0) {
      if (width == 0.0) {
        sx = 1.0; sy = 1.0;
      } else {
        rx/=width; ry/=width; sx/=width; sy/=width;
      }
    }
    if (fabs(sx) < eps) sx = eps;
    if (fabs(sy) < eps) sy = eps;
    divider = sx*sy - rx*ry;
    lua_newtable(L);
    lua_pushnumber(L,width); lua_setfield(L,-2,"width");
    lua_pushnumber(L,rx); lua_setfield(L,-2,"rx");
    lua_pushnumber(L,sx); lua_setfield(L,-2,"sx");
    lua_pushnumber(L,sy); lua_setfield(L,-2,"sy");
    lua_pushnumber(L,ry); lua_setfield(L,-2,"ry");
    lua_pushnumber(L,tx); lua_setfield(L,-2,"tx");
    lua_pushnumber(L,ty); lua_setfield(L,-2,"ty");
    return 1;
}


static int mplib_gr_fields(lua_State * L)
{
    const char **fields;
    int i;
    struct mp_graphic_object **hh = is_gr_object(L, 1);
    if (*hh) {
        switch ((*hh)->type) {
        case mp_fill_code:
            fields = fill_fields;
            break;
        case mp_stroked_code:
            fields = stroked_fields;
            break;
        case mp_text_code:
            fields = text_fields;
            break;
        case mp_special_code:
            fields = special_fields;
            break;
        case mp_start_clip_code:
            fields = start_clip_fields;
            break;
        case mp_start_bounds_code:
            fields = start_bounds_fields;
            break;
        case mp_stop_clip_code:
            fields = stop_clip_fields;
            break;
        case mp_stop_bounds_code:
            fields = stop_bounds_fields;
            break;
        default:
            fields = no_fields;
        }
        lua_newtable(L);
        for (i = 0; fields[i] != NULL; i++) {
            lua_pushstring(L, fields[i]);
            lua_rawseti(L, -2, (i + 1));
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}


#define mplib_push_number(L,x) lua_pushnumber(L,(lua_Number)(x)/65536.0)

#define MPLIB_PATH 0
#define MPLIB_PEN 1

static void mplib_push_path(lua_State * L, struct mp_knot *h, int is_pen)
{
    struct mp_knot *p;          /* for scanning the path */
    int i = 1;
    p = h;
    if (p != NULL) {
        lua_newtable(L);
        do {
            lua_createtable(L, 0, 6);
            if (!is_pen) {
                if (p->left_type != mp_explicit) {
                    mplib_push_S(left_type);
                    lua_pushstring(L, knot_type_enum[p->left_type]);
                    lua_rawset(L, -3);
                }
                if (p->right_type != mp_explicit) {
                    mplib_push_S(right_type);
                    lua_pushstring(L, knot_type_enum[p->right_type]);
                    lua_rawset(L, -3);
                }
            }
            mplib_push_S(x_coord);
            mplib_push_number(L, p->x_coord);
            lua_rawset(L, -3);
            mplib_push_S(y_coord);
            mplib_push_number(L, p->y_coord);
            lua_rawset(L, -3);
            mplib_push_S(left_x);
            mplib_push_number(L, p->left_x);
            lua_rawset(L, -3);
            mplib_push_S(left_y);
            mplib_push_number(L, p->left_y);
            lua_rawset(L, -3);
            mplib_push_S(right_x);
            mplib_push_number(L, p->right_x);
            lua_rawset(L, -3);
            mplib_push_S(right_y);
            mplib_push_number(L, p->right_y);
            lua_rawset(L, -3);
            lua_rawseti(L, -2, i);
            i++;
            if (p->right_type == mp_endpoint) {
                return;
            }
            p = p->next;
        } while (p != h);
    } else {
        lua_pushnil(L);
    }
}

/* this assumes that the top of the stack is a table 
   or nil already in the case
 */
static void mplib_push_pentype(lua_State * L, struct mp_knot *h)
{
    struct mp_knot *p;          /* for scanning the path */
    p = h;
    if (p == NULL) {
        /* do nothing */
    } else if (p == p->next) {
        mplib_push_S(type);
        lua_pushstring(L, "elliptical");
        lua_rawset(L, -3);
    } else {
    }
}

#define set_color_objects(pq)                           \
  object_color_model = pq->color_model;           \
  object_color_a = pq->color.a_val;              \
  object_color_b = pq->color.b_val;              \
  object_color_c = pq->color.c_val;              \
  object_color_d = pq->color.d_val;


static void mplib_push_color(lua_State * L, struct mp_graphic_object *p)
{
    int object_color_model;
    int object_color_a, object_color_b, object_color_c, object_color_d;
    if (p != NULL) {
        if (p->type == mp_fill_code) {
            mp_fill_object *h = (mp_fill_object *) p;
            set_color_objects(h);
        } else if (p->type == mp_stroked_code) {
            mp_stroked_object *h = (mp_stroked_object *) p;
            set_color_objects(h);
        } else {
            mp_text_object *h = (mp_text_object *) p;
            set_color_objects(h);
        }
        lua_newtable(L);
        if (object_color_model >= mp_grey_model) {
            mplib_push_number(L, object_color_a);
            lua_rawseti(L, -2, 1);
            if (object_color_model >= mp_rgb_model) {
                mplib_push_number(L, object_color_b);
                lua_rawseti(L, -2, 2);
                mplib_push_number(L, object_color_c);
                lua_rawseti(L, -2, 3);
                if (object_color_model == mp_cmyk_model) {
                    mplib_push_number(L, object_color_d);
                    lua_rawseti(L, -2, 4);
                }
            }
        }
    } else {
        lua_pushnil(L);
    }
}

/* the dash scale is not exported, the field has no external value */
static void mplib_push_dash(lua_State * L, struct mp_stroked_object *h)
{
    mp_dash_object *d;
    double ds;
    if (h != NULL && h->dash_p != NULL) {
        d = h->dash_p;
        lua_newtable(L);
        mplib_push_number(L, d->offset);
        lua_setfield(L, -2, "offset");
        if (d->array != NULL) {
            int i = 0;
            lua_newtable(L);
            while (*(d->array + i) != -1) {
                ds = *(d->array + i) / 65536.0;
                lua_pushnumber(L, ds);
                i++;
                lua_rawseti(L, -2, i);
            }
            lua_setfield(L, -2, "dashes");
        }
    } else {
        lua_pushnil(L);
    }
}

static void mplib_push_transform(lua_State * L, struct mp_text_object *h)
{
    int i = 1;
    if (h != NULL) {
        lua_createtable(L, 6, 0);
        mplib_push_number(L, h->tx);
        lua_rawseti(L, -2, i);
        i++;
        mplib_push_number(L, h->ty);
        lua_rawseti(L, -2, i);
        i++;
        mplib_push_number(L, h->txx);
        lua_rawseti(L, -2, i);
        i++;
        mplib_push_number(L, h->tyx);
        lua_rawseti(L, -2, i);
        i++;
        mplib_push_number(L, h->txy);
        lua_rawseti(L, -2, i);
        i++;
        mplib_push_number(L, h->tyy);
        lua_rawseti(L, -2, i);
        i++;
    } else {
        lua_pushnil(L);
    }
}

#define FIELD(A) (mplib_is_S(A,2))

static void mplib_fill(lua_State * L, struct mp_fill_object *h)
{
    if (FIELD(path)) {
        mplib_push_path(L, h->path_p, MPLIB_PATH);
    } else if (FIELD(htap)) {
        mplib_push_path(L, h->htap_p, MPLIB_PATH);
    } else if (FIELD(pen)) {
        mplib_push_path(L, h->pen_p, MPLIB_PEN);
        mplib_push_pentype(L, h->pen_p);
    } else if (FIELD(color)) {
        mplib_push_color(L, (mp_graphic_object *) h);
    } else if (FIELD(linejoin)) {
      lua_pushnumber(L, (lua_Number)h->ljoin);
    } else if (FIELD(miterlimit)) {
        mplib_push_number(L, h->miterlim);
    } else if (FIELD(prescript)) {
        lua_pushstring(L, h->pre_script);
    } else if (FIELD(postscript)) {
        lua_pushstring(L, h->post_script);
    } else {
        lua_pushnil(L);
    }
}

static void mplib_stroked(lua_State * L, struct mp_stroked_object *h)
{
    if (FIELD(path)) {
        mplib_push_path(L, h->path_p, MPLIB_PATH);
    } else if (FIELD(pen)) {
        mplib_push_path(L, h->pen_p, MPLIB_PEN);
        mplib_push_pentype(L, h->pen_p);
    } else if (FIELD(color)) {
        mplib_push_color(L, (mp_graphic_object *) h);
    } else if (FIELD(dash)) {
        mplib_push_dash(L, h);
    } else if (FIELD(linecap)) {
        lua_pushnumber(L, (lua_Number)h->lcap);
    } else if (FIELD(linejoin)) {
      lua_pushnumber(L, (lua_Number)h->ljoin);
    } else if (FIELD(miterlimit)) {
        mplib_push_number(L, h->miterlim);
    } else if (FIELD(prescript)) {
        lua_pushstring(L, h->pre_script);
    } else if (FIELD(postscript)) {
        lua_pushstring(L, h->post_script);
    } else {
        lua_pushnil(L);
    }
}

static void mplib_text(lua_State * L, struct mp_text_object *h)
{
    if (FIELD(text)) {
        lua_pushstring(L, h->text_p);
    } else if (FIELD(dsize)) {
        mplib_push_number(L, (h->font_dsize / 16));
    } else if (FIELD(font)) {
        lua_pushstring(L, h->font_name);
    } else if (FIELD(color)) {
        mplib_push_color(L, (mp_graphic_object *) h);
    } else if (FIELD(width)) {
        mplib_push_number(L, h->width);
    } else if (FIELD(height)) {
        mplib_push_number(L, h->height);
    } else if (FIELD(depth)) {
        mplib_push_number(L, h->depth);
    } else if (FIELD(transform)) {
        mplib_push_transform(L, h);
    } else if (FIELD(prescript)) {
        lua_pushstring(L, h->pre_script);
    } else if (FIELD(postscript)) {
        lua_pushstring(L, h->post_script);
    } else {
        lua_pushnil(L);
    }
}

static void mplib_special(lua_State * L, struct mp_special_object *h)
{
    if (FIELD(prescript)) {
        lua_pushstring(L, h->pre_script);
    } else {
        lua_pushnil(L);
    }
}

static void mplib_start_bounds(lua_State * L, struct mp_bounds_object *h)
{
    if (FIELD(path)) {
        mplib_push_path(L, h->path_p, MPLIB_PATH);
    } else {
        lua_pushnil(L);
    }
}

static void mplib_start_clip(lua_State * L, struct mp_clip_object *h)
{
    if (FIELD(path)) {
        mplib_push_path(L, h->path_p, MPLIB_PATH);
    } else {
        lua_pushnil(L);
    }
}

static int mplib_gr_index(lua_State * L)
{
    struct mp_graphic_object **hh = is_gr_object(L, 1);
    if (*hh) {
        struct mp_graphic_object *h = *hh;

        if (mplib_is_S(type, 2)) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, mplib_type_Ses[h->type]);
        } else {
            switch (h->type) {
            case mp_fill_code:
                mplib_fill(L, (mp_fill_object *) h);
                break;
            case mp_stroked_code:
                mplib_stroked(L, (mp_stroked_object *) h);
                break;
            case mp_text_code:
                mplib_text(L, (mp_text_object *) h);
                break;
            case mp_special_code:
                mplib_special(L, (mp_special_object *) h);
                break;
            case mp_start_clip_code:
                mplib_start_clip(L, (mp_clip_object *) h);
                break;
            case mp_start_bounds_code:
                mplib_start_bounds(L, (mp_bounds_object *) h);
                break;
            case mp_stop_clip_code:
            case mp_stop_bounds_code:
            default:
                lua_pushnil(L);
            }
        }
    } else {
        lua_pushnil(L);
    }
    return 1;
}


static const struct luaL_reg mplib_meta[] = {
    {"__gc", mplib_collect},
    {"__tostring", mplib_tostring},
    {NULL, NULL}                /* sentinel */
};

static const struct luaL_reg mplib_fig_meta[] = {
    {"__gc",         mplib_fig_collect},
    {"__tostring",   mplib_fig_tostring},
    {"objects",      mplib_fig_body},
    {"copy_objects", mplib_fig_copy_body},
    {"filename",     mplib_fig_filename},
    {"postscript",   mplib_fig_postscript},
    {"svg",          mplib_fig_svg},
    {"boundingbox",  mplib_fig_bb},
    {"width",        mplib_fig_width},
    {"height",       mplib_fig_height},
    {"depth",        mplib_fig_depth},
    {"italcorr",     mplib_fig_italcorr},
    {"charcode",     mplib_fig_charcode},
    {NULL, NULL}                /* sentinel */
};

static const struct luaL_reg mplib_gr_meta[] = {
    {"__gc", mplib_gr_collect},
    {"__tostring", mplib_gr_tostring},
    {"__index", mplib_gr_index},
    {NULL, NULL}                /* sentinel */
};

static const struct luaL_reg mplib_d[] = {
    {"execute", mplib_execute},
    {"finish", mplib_finish},
    {"char_width", mplib_charwidth},
    {"char_height", mplib_charheight},
    {"char_depth", mplib_chardepth},
    {"statistics", mplib_statistics},
    {NULL, NULL}                /* sentinel */
};


static const struct luaL_reg mplib_m[] = {
    {"new", mplib_new},
    {"version",    mplib_version},
    {"fields", mplib_gr_fields},
    {"pen_info", mplib_gr_peninfo},
    {NULL, NULL}                /* sentinel */
};


int luaopen_mplib(lua_State * L)
{
    mplib_init_Ses(L);

    luaL_newmetatable(L, MPLIB_GR_METATABLE);
    lua_pushvalue(L, -1);       /* push metatable */
    lua_setfield(L, -2, "__index");     /* metatable.__index = metatable */
    luaL_register(L, NULL, mplib_gr_meta);      /* object meta methods */
    lua_pop(L, 1);

    luaL_newmetatable(L, MPLIB_FIG_METATABLE);
    lua_pushvalue(L, -1);       /* push metatable */
    lua_setfield(L, -2, "__index");     /* metatable.__index = metatable */
    luaL_register(L, NULL, mplib_fig_meta);     /* figure meta methods */
    lua_pop(L, 1);

    luaL_newmetatable(L, MPLIB_METATABLE);
    lua_pushvalue(L, -1);       /* push metatable */
    lua_setfield(L, -2, "__index");     /* metatable.__index = metatable */
    luaL_register(L, NULL, mplib_meta); /* meta methods */
    luaL_register(L, NULL, mplib_d);    /* dict methods */
    luaL_register(L, "mplib", mplib_m); /* module functions */
    return 1;
}
