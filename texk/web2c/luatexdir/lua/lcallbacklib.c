/* $Id: lcallbacklib.c 1013 2008-02-14 00:09:02Z oneiros $ */

#include "luatex-api.h"
#include <ptexlib.h>

extern int do_run_callback (int special, char *values, va_list vl);

int callback_set[total_callbacks] = {0};

static const char *const callbacknames[] = {
  "", /* empty on purpose */
  "find_write_file",
  "find_output_file",
  "find_image_file",
  "find_format_file",
  "find_read_file",      "open_read_file",
  "find_ocp_file",       "read_ocp_file",
  "find_vf_file",        "read_vf_file",
  "find_data_file",      "read_data_file",
  "find_font_file",      "read_font_file",
  "find_map_file",       "read_map_file",
  "find_enc_file",       "read_enc_file",
  "find_type1_file",     "read_type1_file",
  "find_truetype_file",  "read_truetype_file",
  "find_opentype_file",  "read_opentype_file",
  "find_sfd_file",       "read_sfd_file",
  "find_pk_file",        "read_pk_file",
  "show_error_hook",
  "process_input_buffer",
  "start_page_number",  "stop_page_number",
  "start_run",          "stop_run",
  "define_font",
  "token_filter",
  "pre_output_filter",
  "buildpage_filter",
  "hpack_filter",  "vpack_filter",
  "char_exists", 
  "hyphenate", 
  "ligaturing",
  "kerning",
  "pre_linebreak_filter", 
  "linebreak_filter", 
  "post_linebreak_filter", 
  NULL };

int callback_callbacks_id = 0;

void 
get_lua_boolean  (char *table, char *name, boolean *target) {
  int stacktop;
  stacktop = lua_gettop(Luas[0]);  
  luaL_checkstack(Luas[0],2,"out of stack space");
  lua_getglobal(Luas[0],table);
  if (lua_istable(Luas[0],-1)) {
    lua_getfield(Luas[0],-1,name);
    if (lua_isboolean(Luas[0],-1)) {
      *target = (lua_toboolean(Luas[0],-1));
    } else if (lua_isnumber(Luas[0],-1)) {
      *target = (lua_tonumber(Luas[0],-1)==0 ? 0 : 1);
    }
  }
  lua_settop(Luas[0],stacktop);
  return;
}

void 
get_saved_lua_boolean (int r, char *name, boolean *target) {
  int stacktop;
  stacktop = lua_gettop(Luas[0]);  
  luaL_checkstack(Luas[0],2,"out of stack space");
  lua_rawgeti(Luas[0],LUA_REGISTRYINDEX,r);
  if (lua_istable(Luas[0],-1)) {
    lua_getfield(Luas[0],-1,name);
    if (lua_isboolean(Luas[0],-1)) {
      *target = lua_toboolean(Luas[0],-1);
    } else if (lua_isnumber(Luas[0],-1)) {
      *target = (lua_tonumber(Luas[0],-1)==0 ? 0 : 1);
    } 
  }
  lua_settop(Luas[0],stacktop);
  return;
}

void 
get_lua_number (char *table, char *name, integer *target) {
  int stacktop;
  stacktop = lua_gettop(Luas[0]);  
  luaL_checkstack(Luas[0],2,"out of stack space");
  lua_getglobal(Luas[0],table);
  if (lua_istable(Luas[0],-1)) {
    lua_getfield(Luas[0],-1,name);
    if (lua_isnumber(Luas[0],-1)) {
      *target = lua_tonumber(Luas[0],-1);
    }
  }
  lua_settop(Luas[0],stacktop);
  return;
}

void 
get_saved_lua_number (int r, char *name, integer *target) {
  int stacktop;
  stacktop = lua_gettop(Luas[0]);  
  luaL_checkstack(Luas[0],2,"out of stack space");
  lua_rawgeti(Luas[0],LUA_REGISTRYINDEX,r);
  if (lua_istable(Luas[0],-1)) {
    lua_getfield(Luas[0],-1,name);
    if (lua_isnumber(Luas[0],-1)) {
      *target = lua_tonumber(Luas[0],-1);
    } 
  }
  lua_settop(Luas[0],stacktop);
  return;
}


void 
get_lua_string (char *table, char *name, char **target) {
  int stacktop;
  stacktop = lua_gettop(Luas[0]);  
  luaL_checkstack(Luas[0],2,"out of stack space");
  lua_getglobal(Luas[0],table);
  if (lua_istable(Luas[0],-1)) {
    lua_getfield(Luas[0],-1,name);
    if (lua_isstring(Luas[0],-1)) {
      *target = (char *)lua_tostring(Luas[0],-1);
    }
  }
  lua_settop(Luas[0],stacktop);
  return;
}

void 
get_saved_lua_string (int r, char *name, char **target) {
  int stacktop;
  stacktop = lua_gettop(Luas[0]);  
  luaL_checkstack(Luas[0],2,"out of stack space");
  lua_rawgeti(Luas[0],LUA_REGISTRYINDEX,r);
  if (lua_istable(Luas[0],-1)) {
    lua_getfield(Luas[0],-1,name);
    if (lua_isstring(Luas[0],-1)) {
      *target = (char *)lua_tostring(Luas[0],-1);
    } 
  }
  lua_settop(Luas[0],stacktop);
  return;
}


#define CALLBACK_BOOLEAN        'b'
#define CALLBACK_INTEGER        'd'
#define CALLBACK_LINE           'l'
#define CALLBACK_STRNUMBER      's'
#define CALLBACK_STRING         'S'
#define CALLBACK_CHARNUM        'c'

int 
run_saved_callback (int r, char *name, char *values, ...) {
  va_list args;
  int ret = 0;
  lua_State *L = Luas[0];
  int stacktop = lua_gettop(L);  
  va_start(args,values);
  luaL_checkstack(L,2,"out of stack space");
  lua_rawgeti(L,LUA_REGISTRYINDEX,r);
  lua_pushstring(L,name);
  lua_rawget(L,-2);
  if (lua_isfunction(L,-1)) {
    ret = do_run_callback(2,values,args);
  }
  va_end(args);
  lua_settop(L,stacktop);
  return ret;
}


int
run_and_save_callback (int i, char *values, ...) {
  va_list args;
  int ret = 0;
  lua_State *L = Luas[0];
  int stacktop = lua_gettop(L);
  va_start(args,values);
  luaL_checkstack(L,2,"out of stack space");
  lua_rawgeti(L,LUA_REGISTRYINDEX,callback_callbacks_id);
  lua_rawgeti(L,-1,i);
  if (lua_isfunction(L,-1)) {
    ret = do_run_callback(1,values,args);
  }
  va_end(args);
  if (ret>0) {
    ret = luaL_ref(L,LUA_REGISTRYINDEX);
  }
  lua_settop(L,stacktop);
  return ret;
}


int
run_callback (int i, char *values, ...) {
  va_list args;
  int ret = 0;
  lua_State *L = Luas[0];
  int stacktop = lua_gettop(L);
  va_start(args,values);
  luaL_checkstack(L,2,"out of stack space");
  lua_rawgeti(L,LUA_REGISTRYINDEX,callback_callbacks_id);
  lua_rawgeti(L,-1, i);
  if (lua_isfunction(L,-1)) {
    ret = do_run_callback(0,values,args);
  }
  va_end(args);
  lua_settop(L,stacktop);
  return ret;
}

int
do_run_callback (int special, char *values, va_list vl) {
  int ret, len;
  int narg,nres;
  char *s;
  char cs;
  int *bufloc;
  char *ss = NULL;
  int retval = 0;
  lua_State *L = Luas[0];
  if (special==2) { /* copy the enclosing table */
    luaL_checkstack(L,1,"out of stack space");
    lua_pushvalue(L,-2);
  }
  for (narg = 0; *values; narg++) {
    luaL_checkstack(L,1,"out of stack space");
    switch (*values++) {
    case CALLBACK_CHARNUM: /* an ascii char! */ 
	  cs = (char)va_arg(vl, int);
	  lua_pushlstring(L,&cs, 1);
      break;
    case CALLBACK_STRING: /* C string */ 
	  s = va_arg(vl, char *);
      lua_pushstring(L, s);
      break;
    case CALLBACK_INTEGER: /* int */ 
      lua_pushnumber(L, va_arg(vl, int));
      break;
    case CALLBACK_STRNUMBER: /* TeX string */ 
      s = makeclstring(va_arg(vl, int),&len);
      lua_pushlstring(L, s,len);
      break;
    case CALLBACK_BOOLEAN: /* boolean */ 
      lua_pushboolean(L, va_arg(vl, int));
      break;
    case CALLBACK_LINE: /* a buffer section, with implied start */ 
      lua_pushlstring(L, (char *)(buffer+first), va_arg(vl, int));
      break;
    case '-': 
      narg--;
      break;
    case '>': 
      goto ENDARGS;
    default :
      ;
    }
  }
 ENDARGS:
  nres = strlen(values);
  if (special==1) {
    nres++;
  }
  if (special==2) {
    narg++;
  }
  if(lua_pcall(L,narg,nres,0) != 0) {
    /* Can't be more precise here, could be called before 
	 * TeX initialization is complete 
	 */
    fprintf(stderr,"This went wrong: %s\n",lua_tostring(L,-1));
	error();
    return 0;
  };
  if (nres==0) {
    return 1;
  }
  nres = -nres;
  while (*values) {
    int b;
    switch (*values++) {
    case CALLBACK_BOOLEAN: 
      if (!lua_isboolean(L,nres)) {
		fprintf(stderr,"Expected a boolean, not: %s\n", lua_typename(L,lua_type(L,nres)));
		goto EXIT;
      }
      b = lua_toboolean(L,nres);
      *va_arg(vl, boolean *) = (boolean)b;
      break;
    case CALLBACK_INTEGER: 
      if (!lua_isnumber(L,nres)) {
		fprintf(stderr,"Expected a number, not: %s\n", lua_typename(L,lua_type(L,nres)));
		goto EXIT;
      }
      b = lua_tonumber(L,nres);
      *va_arg(vl, int *) = b;
      break;
    case CALLBACK_LINE:  /* TeX line */
      if (!lua_isstring(L,nres)) {
		if (!lua_isnil(L,nres))
		  fprintf(stderr,"Expected a string for (l), not: %s\n", 
				  lua_typename(L,lua_type(L,nres))); 
		goto EXIT;
      }
      s = (char *)lua_tolstring(L,nres, (size_t *)&len);
      if (s!=NULL) { /* |len| can be zero */
		bufloc = va_arg(vl, int *);
		ret = *bufloc;
		check_buffer_overflow (ret+len);
		while (len--) {
		  buffer[(*bufloc)++] = *s++;
		}
		while ((*bufloc)-1>ret && buffer[(*bufloc)-1] == ' ')
		  (*bufloc)--;
      } else {
		bufloc = 0;
	  }
      break;
    case CALLBACK_STRNUMBER:  /* TeX string */
      if (!lua_isstring(L,nres)) {
		if (!lua_isnil(L,nres)) {
		  fprintf(stderr,"Expected a string for (s), not: %s\n", 
				  lua_typename(L,lua_type(L,nres)));
		  goto EXIT;
		}
      }
	  s = (char *)lua_tolstring(L,nres,(size_t *)&len);
      if (s==NULL) /* |len| can be zero */
		*va_arg(vl, int *) = 0;
      else {
		*va_arg(vl, int *) = maketexlstring(s,len);
      }
      break;
    case CALLBACK_STRING:  /* C string aka buffer */
      if (!lua_isstring(L,nres)) {
		if (!lua_isnil(L,nres)) {
		  fprintf(stderr,"Expected a string for (S), not: %s\n", 
				  lua_typename(L,lua_type(L,nres)));
		  goto EXIT;
		}
      }
	  s = (char *)lua_tolstring(L,nres,(size_t *)&len);

      if (s==NULL) /* |len| can be zero */
		*va_arg(vl, int *) = 0;
      else {
		ss = xmalloc(len+1);
        (void)memcpy(ss,s,(len+1));
		*va_arg(vl, char **) = ss;
      }
      break;
    default: 
      fprintf(stdout,"invalid return value type");
      goto EXIT;
    }
    nres++;
  }
  retval = 1;
 EXIT:
  return retval;
}

void
destroy_saved_callback (int i) {
  luaL_unref(Luas[0],LUA_REGISTRYINDEX,i);
}

static int callback_register (lua_State *L) {
  int cb;
  char *s;
  if (!lua_isstring(L,1) || 
      ((!lua_isfunction(L,2)) && !lua_isnil(L,2))) {
    lua_pushnil(L);
    lua_pushstring(L,"Invalid arguments to callback.register.");
    return 2;
  }
  s = (char *)lua_tostring(L,1);
  for(cb=0;cb<total_callbacks;cb++) {
    if (strcmp(callbacknames[cb],s)==0) 
      break;
  }
  if (cb==total_callbacks) {
    lua_pushnil(L);
    lua_pushstring(L,"No such callback exists.");
    return 2;
  }
  if (lua_isfunction(L,2)) {
    callback_set[cb]=cb;
  } else {
    callback_set[cb]=0;
  }
  luaL_checkstack(L,2,"out of stack space");
  lua_rawgeti(L,LUA_REGISTRYINDEX,callback_callbacks_id); /* push the table */
  lua_pushvalue(L,2);    /* the function or nil */
  lua_rawseti(L,-2,cb);
  lua_rawseti(L,LUA_REGISTRYINDEX,callback_callbacks_id);
  lua_pushnumber(L,cb);
  return 1;
}

static int callback_find (lua_State *L) {
  int cb;
  char * s;
  if (!lua_isstring(L,1)) {
    lua_pushnil(L);
    lua_pushstring(L,"Invalid arguments to callback.find.");
    return 2;
  }
  s = (char *)lua_tostring(L,1);
  for(cb=0;cb<total_callbacks;cb++) {
    if (strcmp(callbacknames[cb],s)==0) 
      break;
  }
  if (cb==total_callbacks) {
    lua_pushnil(L);
    lua_pushstring(L,"No such callback exists.");
    return 2;
  }
  luaL_checkstack(L,2,"out of stack space");
  lua_rawgeti(L,LUA_REGISTRYINDEX,callback_callbacks_id); /* push the table */
  lua_rawgeti(L,-1,cb);
  return 1;
}


static int callback_listf (lua_State *L) {
  int i;
  luaL_checkstack(L,3,"out of stack space");
  lua_newtable(L);
  for (i=1; callbacknames[i]; i++) {
    lua_pushstring(L,callbacknames[i]);
    if (callback_defined(i)) {
      lua_pushboolean(L,1);
    } else {
      lua_pushboolean(L,0);
    }
    lua_rawset(L,-3);
  }
  return 1;
}

static const struct luaL_reg callbacklib [] = {
  {"find",    callback_find},
  {"register",callback_register},
  {"list",    callback_listf},
  {NULL, NULL}  /* sentinel */
};

int luaopen_callback (lua_State *L) 
{
  luaL_register(L, "callback", callbacklib);
  luaL_checkstack(L,1,"out of stack space");
  lua_newtable(L);
  callback_callbacks_id = luaL_ref(L,LUA_REGISTRYINDEX);
  return 1;
}



