/* $Id: lfontlib.c 1171 2008-04-16 13:56:53Z taco $ */

#include "luatex-api.h"
#include <ptexlib.h>
#include "nodes.h"

/* this function is in vfovf.c for the moment */
extern int make_vf_table(lua_State *L, char *name, scaled s);

static int 
font_read_tfm (lua_State *L) {
  internalfontnumber f;
  scaled s;
  int k;
  char *cnom;
  if(lua_isstring(L, 1)) {
    cnom = (char *)lua_tostring(L, 1);
    if(lua_isnumber(L, 2)) {
      s = (integer)lua_tonumber(L,2);
	  if (strlen(cnom)) {
		f = new_font();
		if (read_tfm_info(f, cnom, "", s)) {
		  k =  font_to_lua(L,f);
		  delete_font(f);
		  return k;
		} else {
		  delete_font(f);
		  lua_pushstring(L, "font loading failed");
		  lua_error(L);
		}
      } else {
		lua_pushstring(L, "expected tfm name as first argument");
		lua_error(L);
      }
    } else {
      lua_pushstring(L, "expected an integer size as second argument");
      lua_error(L);
    }
  } else {
    lua_pushstring(L, "expected tfm name as first argument");
    lua_error(L);
  }
  return 2; /* not reached */
}


static int 
font_read_vf (lua_State *L) {
  scaled s;
  char *cnom;
  if(lua_isstring(L, 1)) {
    cnom = (char *)lua_tostring(L, 1);
	if (strlen(cnom)) {
	  if(lua_isnumber(L, 2)) {
		s = lua_tonumber(L,2);
		return make_vf_table(L,cnom,s);
	  } else {
		lua_pushstring(L, "expected an integer size as second argument");
		lua_error(L);
		return 2;
	  }
	}
  }
  lua_pushstring(L, "expected vf name as first argument"); 
  lua_error(L);
  return 2; /* not reached */
}

static int 
tex_current_font (lua_State *L) {
  int i;
  i = (int)luaL_optinteger(L,1,0);
  if (i>0) {
	if (is_valid_font(i)) {
	  zset_cur_font(i);
	  return 0;
	} else {
	  lua_pushstring(L, "expected a valid font id");
	  lua_error(L);
	  return 2; /* not reached */
	}
  } else {
	lua_pushnumber(L,get_cur_font());
	return 1;
  }
}

static int 
tex_max_font (lua_State *L) {
  lua_pushnumber(L,max_font_id());
  return 1;
}


static int 
tex_each_font_next (lua_State *L) {
  int i,m; /* id */
  m = lua_tonumber(L,1);
  i = lua_tonumber(L,2);
  i++;
  while(i<=m && !is_valid_font(i))
    i++; 
  if (i>m) {
    lua_pushnil(L);
    return 1;
  } else {
    lua_pushnumber(L, i);
    font_to_lua (L, i);
    return 2;
  }
}

static int 
tex_each_font (lua_State *L) {
  lua_pushcclosure(L, tex_each_font_next, 0);
  lua_pushnumber(L, max_font_id());
  lua_pushnumber(L, 0);
  return 3;
}

static int 
frozenfont (lua_State *L) {
  int i;
  i = (int)luaL_checkinteger(L,1);
  if (i) {
    if (is_valid_font(i)) {
      if (font_touched(i) || font_used(i)) {
	lua_pushboolean(L,1);
      } else {
	lua_pushboolean(L,0);
      }
    } else {
      lua_pushnil(L);
    }
    return 1;	
  } else {
    lua_pushstring(L, "expected an integer argument"); 
    lua_error(L);
  }
  return 0; /* not reached */
}


static int 
setfont (lua_State *L) {
  int i;
  i = (int)luaL_checkinteger(L,-2);
  if (i) {
    luaL_checktype(L,-1,LUA_TTABLE);
    if (is_valid_font(i)) {
      if (! (font_touched(i) || font_used(i))) {
	font_from_lua (L,i) ;
      } else {
	lua_pushstring(L, "that font has been accessed already, changing it is forbidden");
	lua_error(L);
      }
    } else {
      lua_pushstring(L, "that integer id is not a valid font");
      lua_error(L);
    }
  }
  return 0;
}


static int 
deffont (lua_State *L) {
  int i;
  luaL_checktype(L,-1,LUA_TTABLE);

  i = new_font();
  if(font_from_lua (L,i)) {
    lua_pushnumber(L,i);
    return 1;
  } else {
    lua_pop(L,1); /* pop the broken table */
    delete_font(i);
    lua_pushstring(L, "font creation failed");
    lua_error(L);
  }  
  return 0; /* not reached */
}

/* this returns the expected (!) next fontid. */
static int 
nextfontid (lua_State *L) {
  int i = new_font();
  lua_pushnumber(L,i);
  delete_font(i);
  return 1;
}


static int 
getfont (lua_State *L) {
  int i;
  i = (int)luaL_checkinteger(L,-1);
  if (i && is_valid_font(i) &&  font_to_lua (L, i))
    return 1;
  lua_pushnil(L);
  return 1;
}


static const struct luaL_reg fontlib [] = {
  {"read_tfm",      font_read_tfm},
  {"read_vf",       font_read_vf},
  {"current",       tex_current_font},
  {"max",           tex_max_font},
  {"each",          tex_each_font},
  {"getfont",       getfont},
  {"setfont",       setfont},
  {"define",        deffont},
  {"nextid",        nextfontid},
  {"frozen",        frozenfont},
  {NULL, NULL}  /* sentinel */
};

int luaopen_font (lua_State *L) 
{
  luaL_register(L, "font", fontlib);
  make_table(L,"fonts","getfont","setfont");
  return 1;
}
