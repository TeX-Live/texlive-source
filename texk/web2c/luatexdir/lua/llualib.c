
/* $Id: llualib.c 1169 2008-04-15 14:57:56Z oneiros $ */

#include "luatex-api.h"
#include <ptexlib.h>

#define LOAD_BUF_SIZE 256
#define UINT_MAX32 0xFFFFFFFF

typedef struct {
  unsigned char* buf;
  int size;
  int done;
  int alloc;
} bytecode;

static bytecode *lua_bytecode_registers = NULL;

int luabytecode_max = -1;
unsigned int luabytecode_bytes = 0;

void dump_luac_registers (void) {
  int k,n;
  bytecode b;
  dump_int(luabytecode_max);
  if (lua_bytecode_registers != NULL) {
    n = 0;
    for (k=0;k<=luabytecode_max;k++) {
      if (lua_bytecode_registers[k].size != 0)
	n++;
    }
    dump_int(n);
    for (k=0;k<=luabytecode_max;k++) {
      b = lua_bytecode_registers[k];
      if (b.size != 0) {
	dump_int(k);
	dump_int(b.size);
	do_zdump ((char *)b.buf,1,(b.size), DUMP_FILE);
      }
    }
  }
}

void undump_luac_registers (void) {
  int k,n;
  unsigned int i;
  bytecode b;
  undump_int(luabytecode_max);
  if (luabytecode_max>=0) {
	i = (luabytecode_max+1);
	if ((int)(UINT_MAX32/sizeof(bytecode)+1)<=i) {
	  lua_fatal_error(maketexstring("Corrupt format file"));
	}
    lua_bytecode_registers = xmalloc(i*sizeof(bytecode));
	luabytecode_bytes  = i*sizeof(bytecode);
    for (i=0;i<=(unsigned)luabytecode_max;i++) {
      lua_bytecode_registers[i].done = 0;
      lua_bytecode_registers[i].size = 0;
      lua_bytecode_registers[i].buf = NULL;
    }
    undump_int(n);
    for (i=0;i<(unsigned)n;i++) {
      undump_int(k);
      undump_int(b.size);
      b.buf=xmalloc(b.size);
	  luabytecode_bytes += b.size;
      memset(b.buf, 0, b.size);
      do_zundump ((char *)b.buf,1, b.size, DUMP_FILE);
      lua_bytecode_registers[k].size = b.size;
      lua_bytecode_registers[k].alloc = b.size;
      lua_bytecode_registers[k].buf = b.buf;
    }
  }
}

static void
bytecode_register_shadow_set (lua_State* L, int k){
  /* the stack holds the value to be set */
  lua_pushstring(L,"bytecode_shadow"); /* lua.bytecode_shadow */
  lua_rawget(L,LUA_REGISTRYINDEX);
  if (lua_istable(L, -1)) {
    lua_pushvalue(L,-2);
    lua_rawseti(L,-2,k);
  }
  lua_pop(L,1); /* pop table or nil */
  lua_pop(L,1); /* pop value */
}


static int
bytecode_register_shadow_get (lua_State* L, int k){
  /* the stack holds the value to be set */
  int ret = 0;
  lua_pushstring(L,"bytecode_shadow");
  lua_rawget(L,LUA_REGISTRYINDEX);
  if (lua_istable(L, -1)) {
    lua_rawgeti(L,-1,k);
    if (!lua_isnil(L,-1))
      ret = 1;
    lua_insert(L,-3); /* store the value or nil, deeper down  */
    lua_pop(L,1); /* pop the value or nil at top */
  }
  lua_pop(L,1); /* pop table or nil */
  return ret;
}


int writer(lua_State* L, const void* b, size_t size, void* B) {
  bytecode* buf = (bytecode*)B;
  if ((int)(buf->size + size) > buf->alloc) {
    buf->buf   = xrealloc(buf->buf,buf->alloc+size+LOAD_BUF_SIZE);
    buf->alloc = buf->alloc+size+LOAD_BUF_SIZE;
  }
  memcpy(buf->buf+buf->size, b, size);
  buf->size += size;
  luabytecode_bytes += size;
  return 0;
}

const char* reader(lua_State* L, void* ud, size_t* size) {
  bytecode* buf = (bytecode*)ud;
  if (buf->done == buf->size) {
    *size = 0;
    buf->done = 0;
    return NULL;
  }
  *size = buf->size;
  buf->done = buf->size;
  return (const char*)buf->buf;
}

int get_bytecode (lua_State *L) {
  int k;
  k = (int)luaL_checkinteger(L,-1);
  if (k<0) {
    lua_pushnil(L);
  } else if (!bytecode_register_shadow_get(L,k)) {
    if (k<=luabytecode_max && lua_bytecode_registers[k].buf != NULL) {
      if(lua_load(L,reader,(void *)(lua_bytecode_registers+k),"bytecode")) {
	lua_error(L);
	lua_pushnil(L);
      } else {
	lua_pushvalue(L,-1);
	bytecode_register_shadow_set(L,k);
      }
    } else {
      lua_pushnil(L);
    }
  }
  return 1;
}

int set_bytecode (lua_State *L) {
  int k, ltype;
  unsigned int i;
  k = (int)luaL_checkinteger(L,-2);
  i = k+1;
  if ((int)(UINT_MAX32/sizeof(bytecode)+1)<i) {
    lua_pushstring(L, "value too large");
    lua_error(L);
  }
  if (k<0) {
    lua_pushstring(L, "negative values not allowed");
    lua_error(L);
  }
  ltype = lua_type(L,-1);
  if (ltype != LUA_TFUNCTION && ltype != LUA_TNIL){
    lua_pushstring(L, "unsupported type");
    lua_error(L);
  }
  if (k>luabytecode_max) {
	i = sizeof(bytecode)*(k+1);
    lua_bytecode_registers = xrealloc(lua_bytecode_registers,i);
	if (luabytecode_max==-1) {
	  luabytecode_bytes += sizeof(bytecode)*(k+1);
	} else {
	  luabytecode_bytes += sizeof(bytecode)*(k+1-luabytecode_max);
	}
    for (i=(luabytecode_max+1);i<=(unsigned)k;i++) {
      lua_bytecode_registers[i].buf=NULL;
      lua_bytecode_registers[i].size=0;
      lua_bytecode_registers[i].done=0;
    }
    luabytecode_max = k;
  }
  if (lua_bytecode_registers[k].buf != NULL) {
    xfree(lua_bytecode_registers[k].buf);
	luabytecode_bytes -= lua_bytecode_registers[k].size;
    lua_bytecode_registers[k].buf = NULL;
    lua_bytecode_registers[k].size=0;
    lua_bytecode_registers[k].done=0;
    lua_pushnil(L);
    bytecode_register_shadow_set(L,k);
  }
  if (ltype == LUA_TFUNCTION) {
    lua_bytecode_registers[k].buf=xmalloc(LOAD_BUF_SIZE);
    lua_bytecode_registers[k].alloc = LOAD_BUF_SIZE;
    memset(lua_bytecode_registers[k].buf, 0, LOAD_BUF_SIZE);
    lua_dump(L,writer,(void *)(lua_bytecode_registers+k));
  }
  lua_pop(L,1);
  return 0;
}

static const struct luaL_reg lualib [] = {
  {"getbytecode", get_bytecode},
  {"setbytecode", set_bytecode},
  {NULL, NULL}  /* sentinel */
};

int luaopen_lua (lua_State *L, int n, char *fname) 
{
  luaL_register(L, "lua", lualib);
  make_table(L,"bytecode","getbytecode","setbytecode");
  lua_newtable(L);
  lua_setfield(L, LUA_REGISTRYINDEX, "bytecode_shadow");
  lua_pushinteger(L, n);
  lua_setfield(L, -2, "id");
  lua_pushstring(L, "0.1");
  lua_setfield(L, -2, "version");
  if (fname == NULL) {
	lua_pushnil(L);
  } else {
	lua_pushstring(L, fname);
  }
  lua_setfield(L, -2, "startupfile");
  return 1;
}

