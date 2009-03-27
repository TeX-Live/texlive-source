
#include <stdlib.h>
#include "lua.h"
#include "lauxlib.h"

#include "socket_lua.c"
#include "ftp_lua.c"
#include "http_lua.c"
#include "smtp_lua.c"
#include "tp_lua.c"
#include "url_lua.c"
#include "ltn12_lua.c"
#include "mime_lua.c"


void luatex_socketlua_open (lua_State *L) {
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "loaded");
  if (!lua_istable(L,-1)) {
    lua_newtable(L);
    lua_setfield(L, -2, "loaded");
    lua_getfield(L, -1, "loaded");
  }
   if(!luatex_socket_lua_open(L)) {
    lua_setfield(L, -2, "socket");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  if(!luatex_ltn12_lua_open(L)) {
    lua_setfield(L, -2, "ltn12");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  if(!luatex_mime_lua_open(L)) {
    lua_setfield(L, -2, "mime");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  if(!luatex_url_lua_open(L)) {
    lua_setfield(L, -2, "socket.url");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  if(!luatex_tp_lua_open(L)) {
    lua_setfield(L, -2, "socket.tp");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  if(!luatex_smtp_lua_open(L)) {
    lua_setfield(L, -2, "socket.smtp");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  if(!luatex_http_lua_open(L)) {
    lua_setfield(L, -2, "socket.http");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  if(!luatex_ftp_lua_open(L)) {
    lua_setfield(L, -2, "socket.ftp");    
  } else {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
  lua_pop(L,2);
}
