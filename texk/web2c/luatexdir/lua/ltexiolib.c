/* $Id: ltexiolib.c 1013 2008-02-14 00:09:02Z oneiros $ */

#include "luatex-api.h"
#include <ptexlib.h>

typedef void (*texio_printer) (strnumber s);

static char *loggable_info = NULL;

static int
do_texio_print (lua_State *L, texio_printer printfunction) {
  strnumber texs,u;
  char *s;
  char save_selector;
  int n,i;
  size_t k;
  u = 0;
  n = lua_gettop(L);
  if (n==0 || !lua_isstring(L, -1)) {
    lua_pushstring(L, "no string to print");
    lua_error(L);
  }
  save_selector = selector;
  i = 1;
  if (n>1) {
	s=(char *)lua_tostring(L, 1);
	if      (strcmp(s,"log") == 0)          { i++; selector = log_only;     }
	else if (strcmp(s,"term") == 0)         { i++; selector = term_only;    }
	else if (strcmp(s,"term and log") == 0) { i++; selector = term_and_log; }
  }
  if (selector!=log_only &&  selector!=term_only && selector != term_and_log) {
	normalize_selector(); /* sets selector */
  }
  /* just in case there is a string in progress */
  if (str_start[str_ptr-0x200000]<pool_ptr) 
    u=make_string();
  for (;i<=n;i++) {
	s = (char *)lua_tolstring(L, i, &k);
	texs = maketexlstring(s,k);
	printfunction(texs);
	flush_str(texs);
  }
  selector = save_selector;
  if (u!=0) str_ptr--;
  return 0; 
}

static void
do_texio_ini_print (lua_State *L, char *extra) {
  char *s;
  int i,n,l;
  n = lua_gettop(L);
  i = 1;
  l = 3;
  if (n>1) {
	s=(char *)lua_tostring(L, 1);
	if      (strcmp(s,"log") == 0)          { i++; l = 1; }
	else if (strcmp(s,"term") == 0)         { i++; l = 2; }
	else if (strcmp(s,"term and log") == 0) { i++; l = 3; }
  }
  for (;i<=n;i++) {
	if(lua_isstring(L, i)) {
	  s = (char *)lua_tostring(L, i);
	  if (l==2||l==3)
		fprintf(stdout,"%s%s", extra, s);
	  if (l==1||l==3) {
		if (loggable_info==NULL) {
		  loggable_info = strdup(s);
		} else {
		  char *v = concat3 (loggable_info,extra,s);
		  free(loggable_info);
		  loggable_info = v;
		}
	  }
	}
  }
}

static int
texio_print (lua_State *L) {
  if (ready_already!=314159 || pool_ptr==0 || job_name==0) {
	do_texio_ini_print(L,"");
	return 0;
  }
  return do_texio_print(L,zprint);
}

static int
texio_printnl (lua_State *L) {
  if (ready_already!=314159 || pool_ptr==0 || job_name==0) {
	do_texio_ini_print(L,"\n");
	return 0;
  }
  return do_texio_print(L,zprint_nl);
}

/* at the point this function is called, the selector is log_only */
void flush_loggable_info (void) {
  if (loggable_info!=NULL) {
	fprintf(log_file,"%s\n",loggable_info);
	free(loggable_info);
	loggable_info=NULL;
  }
}


static const struct luaL_reg texiolib [] = {
  {"write", texio_print},
  {"write_nl", texio_printnl},
  {NULL, NULL}  /* sentinel */
};

int
luaopen_texio (lua_State *L) {
  luaL_register(L,"texio",texiolib);
  return 1;
}
