/* $Id: lpdflib.c 1013 2008-02-14 00:09:02Z oneiros $ */

#include "luatex-api.h"
#include <ptexlib.h>


static int 
findcurv (lua_State *L) {
  int j;
  j = get_cur_v();
  lua_pushnumber(L, j);
  return 1;
}

static int 
findcurh (lua_State *L) {
  int j;
  j = get_cur_h();
  lua_pushnumber(L, j);
  return 1;
}


typedef enum { set_origin, direct_page, direct_always } pdf_lit_mode;

int luapdfprint(lua_State * L)
{
    int n;
	unsigned i, len;
    const char *outputstr, *st;
    pdf_lit_mode literal_mode;
    n = lua_gettop(L);
    if (!lua_isstring(L, -1)) {
        lua_pushstring(L, "no string to print");
        lua_error(L);
    }
    literal_mode = set_origin;
    if (n == 2) {
        if (!lua_isstring(L, -2)) {
            lua_pushstring(L, "invalid argument for print literal mode");
            lua_error(L);
        } else {
            outputstr = (char *) lua_tostring(L, -2);
            if (strcmp(outputstr, "direct") == 0)
                literal_mode = direct_always;
            else if (strcmp(outputstr, "page") == 0)
                literal_mode = direct_page;
            else {
                lua_pushstring(L,
                               "invalid argument for print literal mode");
                lua_error(L);
            }
        }
    } else {
	    if (n != 1) {
            lua_pushstring(L, "invalid number of arguments");
            lua_error(L);
        }
    }
    switch (literal_mode) {
    case (set_origin):
        pdf_end_text();
        pdf_set_origin(cur_h, cur_v);
        break;
    case (direct_page):
        pdf_end_text();
        break;
    case (direct_always):
        pdf_end_string_nl();
        break;
    default:
        assert(0);
    }
    st = lua_tolstring(L, n,&len);
    for (i = 0; i < len; i++) {
	  if (i%16 == 0) 
        pdfroom(16);
	  pdf_buf[pdf_ptr++] = st[i];
    }
    return 0;
}

static int 
getpdf (lua_State *L) {
  char *st;
  if (lua_isstring(L,2)) {
    st = (char *)lua_tostring(L,2);
    if (st && *st) {
       if (*st == 'h')
	 return findcurh(L);
       else if (*st == 'v')
	 return findcurv(L);
    }
  }
  lua_pushnil(L);
  return 1;
}

static int 
setpdf (lua_State *L) {
  return (L==NULL ? 0 : 0); /* for -Wall */
}

static const struct luaL_reg pdflib[] = {
    {"print", luapdfprint},
    {NULL, NULL}                /* sentinel */
};


int 
luaopen_pdf (lua_State *L) {
  luaL_register(L, "pdf", pdflib);
  /* build meta table */
  luaL_newmetatable(L,"pdf_meta"); 
  lua_pushstring(L, "__index");
  lua_pushcfunction(L, getpdf); 
  /* do these later, NYI */
  if (0) {
    lua_settable(L, -3);
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, setpdf); 
  }
  lua_settable(L, -3);
  lua_setmetatable(L,-2); /* meta to itself */
  return 1;
}

