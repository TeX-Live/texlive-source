
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


void
luatex_socketlua_open (lua_State *L) {
  if(luatex_socket_lua_open(L) ||
     luatex_ltn12_lua_open(L) ||
     luatex_mime_lua_open(L) ||
     luatex_url_lua_open(L) ||
     luatex_tp_lua_open(L) ||
     luatex_smtp_lua_open(L) ||
     luatex_http_lua_open(L) ||
     luatex_ftp_lua_open(L)) {
    fprintf(stderr,"FATAL error while preloading lua modules");
    exit(1);
  }
}
