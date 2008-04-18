/* $Id$ */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef pdfTeX
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#else
#include <../lua51/lua.h>
#include <../lua51/lauxlib.h>
#include <../lua51/lualib.h>
#endif

#include "mplib.h"
#include "mpmp.h"
#include "mppsout.h" /* for mp_edge_object */

#define MPLIB_METATABLE     "MPlib"
#define MPLIB_FIG_METATABLE "MPlib.fig"
#define MPLIB_GR_METATABLE  "MPlib.gr"

#define mplib_init_S(a) do {											\
    lua_pushliteral(L,#a);                                              \
	mplib_##a##_ptr = (char *)lua_tostring(L,-1);						\
    mplib_##a##_index = luaL_ref (L,LUA_REGISTRYINDEX);					\
  } while (0)

#define mplib_push_S(a) do {									\
    lua_rawgeti(L,LUA_REGISTRYINDEX,mplib_##a##_index);			\
  } while (0)

#define mplib_is_S(a,i) (mplib_##a##_ptr==(char *)lua_tostring(L,i))

#define mplib_make_S(a)													\
  static int mplib_##a##_index = 0;										\
  static char *mplib_##a##_ptr = NULL

static int mplib_type_Ses[mp_special_code+1] = {0}; /* [0] is not used */

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

void mplib_init_Ses(lua_State *L) {
  mplib_init_S(fill);
  mplib_init_S(outline);
  mplib_init_S(text);
  mplib_init_S(start_bounds);
  mplib_init_S(stop_bounds);
  mplib_init_S(start_clip);
  mplib_init_S(stop_clip);
  mplib_init_S(special);  

  mplib_type_Ses[mp_fill_code]         = mplib_fill_index;
  mplib_type_Ses[mp_stroked_code]      = mplib_outline_index;
  mplib_type_Ses[mp_text_code]         = mplib_text_index;
  mplib_type_Ses[mp_start_bounds_code] = mplib_start_bounds_index;
  mplib_type_Ses[mp_stop_bounds_code]  = mplib_stop_bounds_index;
  mplib_type_Ses[mp_start_clip_code]   = mplib_start_clip_index;
  mplib_type_Ses[mp_stop_clip_code]    = mplib_stop_clip_index;
  mplib_type_Ses[mp_special_code]      = mplib_special_index;

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




#define xfree(A) if ((A)!=NULL) { free((A)); A = NULL; }

#define is_mp(L,b) (MP *)luaL_checkudata(L,b,MPLIB_METATABLE)
#define is_fig(L,b) (struct mp_edge_object **)luaL_checkudata(L,b,MPLIB_FIG_METATABLE)
#define is_gr_object(L,b) (struct mp_graphic_object **)luaL_checkudata(L,b,MPLIB_GR_METATABLE)

/* Enumeration string arrays */

static const char *interaction_options[] = 
  { "unknown","batch","nonstop","scroll","errorstop", NULL};

static const char *mplib_filetype_names[] = 
  {"term", "error", "mp", "log", "ps", "mem", "tfm", "map", "pfb", "enc", NULL};

/* only "endpoint" and "explicit" actually happen in paths, 
   as well as "open" in elliptical pens */

static const char *knot_type_enum[]  = 
  { "endpoint", "explicit", "given", "curl", "open", "end_cycle"  };

/* object fields */

static const char *fill_fields[] = 
  { "type", "path", "htap", "pen", "color", "linejoin", "miterlimit", 
    "prescript", "postscript", NULL };

static const  char *stroked_fields[] = 
  { "type", "path", "pen", "color", "linejoin", "miterlimit", "linecap", "dash", 
    "prescript", "postscript", NULL };

static const char *text_fields[] = 
  { "type", "text", "dsize", "font", "color", "width", "height", "depth", "transform", 
    "prescript", "postscript", NULL };

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

typedef enum {  
  P_ERROR_LINE,  P_HALF_LINE,   P_MAX_LINE,    P_MAIN_MEMORY, 
  P_HASH_SIZE,   P_HASH_PRIME,  P_PARAM_SIZE,  P_IN_OPEN,     P_RANDOM_SEED, 
  P_INTERACTION, P_INI_VERSION, P_TROFF_MODE,  P_PRINT_NAMES, P_MEM_NAME, 
  P_JOB_NAME,    P_FIND_FILE,   P__SENTINEL
} mplib_parm_idx;

typedef struct {
    const char *name;           /* parameter name */
    mplib_parm_idx idx;         /* parameter index */
} mplib_parm_struct;

static mplib_parm_struct mplib_parms[] = {
  {"error_line",        P_ERROR_LINE   },
  {"half_error_line",   P_HALF_LINE    },
  {"max_print_line",    P_MAX_LINE     },
  {"main_memory",       P_MAIN_MEMORY  },
  {"hash_size",         P_HASH_SIZE    },
  {"hash_prime",        P_HASH_PRIME   },
  {"param_size",        P_PARAM_SIZE   },
  {"max_in_open",       P_IN_OPEN      },
  {"random_seed",       P_RANDOM_SEED  },
  {"interaction",       P_INTERACTION  },
  {"ini_version",       P_INI_VERSION  },
  {"troff_mode",        P_TROFF_MODE   },
  {"print_found_names", P_PRINT_NAMES  },
  {"mem_name",          P_MEM_NAME     },
  {"job_name",          P_JOB_NAME     },
  {"find_file",         P_FIND_FILE    },
  {NULL,                P__SENTINEL    }
};

typedef struct _FILE_ITEM {
  FILE *f;
} _FILE_ITEM ;

typedef struct _FILE_ITEM File;

#define make_stream_buf(A) char *A; size_t A##_size; size_t A##_used

#define free_stream_buf(A) xfree(mplib_data->A); mplib_data->A##_size = 0; mplib_data->A##_used = 0

typedef struct _MPLIB_INSTANCE_DATA {
  void *term_file_ptr;
  void *err_file_ptr;
  void *log_file_ptr;
  void *ps_file_ptr;
  make_stream_buf(term_out);
  make_stream_buf(error_out);
  make_stream_buf(log_out);
  make_stream_buf(ps_out);
  char *input_data;
  char *input_data_ptr;
  size_t input_data_len;
  struct mp_edge_object *edges ;
  lua_State *LL;
} _MPLIB_INSTANCE_DATA;

typedef struct _MPLIB_INSTANCE_DATA mplib_instance;

static mplib_instance *mplib_get_data (MP mp) {
  return (mplib_instance *)mp->userdata;
}

static mplib_instance *mplib_make_data (void) {
  mplib_instance *mplib_data = malloc(sizeof(mplib_instance));
  memset(mplib_data,0,sizeof(mplib_instance));
  return mplib_data ;
}


/* Start by defining all the callback routines for the library 
 * except |run_make_mpx| and |run_editor|.
 */


char *mplib_find_file (MP mp, const char *fname, const char *fmode, int ftype)  {
  mplib_instance *mplib_data = mplib_get_data(mp);
  lua_State *L = mplib_data->LL;
  lua_checkstack(L,4);
  lua_getfield(L,LUA_REGISTRYINDEX,"mplib_file_finder");
  if (lua_isfunction(L,-1)) {
    char *s = NULL, *x = NULL;
    lua_pushstring(L, fname);
    lua_pushstring(L, fmode);
    if (ftype >= mp_filetype_text) {
      lua_pushnumber(L, ftype-mp_filetype_text);
    } else {
      lua_pushstring(L, mplib_filetype_names[ftype]);
    }
    if(lua_pcall(L,3,1,0) != 0) {
      fprintf(stdout,"Error in mp.find_file: %s\n", (char *)lua_tostring(L,-1));
      return NULL;
    }
    x = (char *)lua_tostring(L,-1);
    if (x!=NULL)
      s = strdup(x);
	lua_pop(L,1); /* pop the string */
	return s;
  } else {
    lua_pop(L,1);
  }
  if (fmode[0] != 'r' || (! access (fname,R_OK)) || ftype) {  
     return strdup(fname);
  }
  return NULL;
}

static int 
mplib_find_file_function (lua_State *L) {
  if (! (lua_isfunction(L,-1)|| lua_isnil(L,-1) )) {
    return 1; /* error */
  }
  lua_pushstring(L, "mplib_file_finder");
  lua_pushvalue(L,-2);
  lua_rawset(L,LUA_REGISTRYINDEX);
  return 0;
}

void *mplib_open_file(MP mp, const char *fname, const char *fmode, int ftype)  {
  File *ff = malloc(sizeof (File));
  if (ff) {
    mplib_instance *mplib_data = mplib_get_data(mp);
    ff->f = NULL;
    if (ftype==mp_filetype_terminal) {
      if (fmode[0] == 'r') {
		ff->f = stdin;
      } else {
		xfree(mplib_data->term_file_ptr); 
		ff->f = malloc(1);
		mplib_data->term_file_ptr = ff->f;
      }
    } else if (ftype==mp_filetype_error) {
      xfree(mplib_data->err_file_ptr); 
      ff->f = malloc(1);
      mplib_data->err_file_ptr = ff->f;
    } else if (ftype == mp_filetype_log) {
      xfree(mplib_data->log_file_ptr); 
      ff->f = malloc(1);
      mplib_data->log_file_ptr = ff->f;
    } else if (ftype == mp_filetype_postscript) {
      xfree(mplib_data->ps_file_ptr); 
      ff->f = malloc(1);
      mplib_data->ps_file_ptr = ff->f;
    } else { 
      char realmode[3];
      char *f = mplib_find_file(mp, fname,fmode,ftype);
	  if (f==NULL)
		return NULL;
	  realmode[0] = *fmode;
	  realmode[1] = 'b';
	  realmode[2] = 0;
      ff->f = fopen(f, realmode);
      if ((fmode[0] == 'r') && (ff->f == NULL)) {
		free(ff);
		return NULL;  
      }
    }
    return ff;
  }
  return NULL;
}

static int 
mplib_get_char (void *f, mplib_instance *mplib_data) {
  int c;
  if (f==stdin && mplib_data->input_data != NULL) {
	if (mplib_data->input_data_len==0) {
	  if (mplib_data->input_data_ptr!=NULL)
		mplib_data->input_data_ptr = NULL;
	  else
		mplib_data->input_data = NULL;
	  c = EOF;
	} else {
	  mplib_data->input_data_len--;
	  c = *(mplib_data->input_data_ptr)++;
      }
  } else {
	c = fgetc(f);
  }
  return c;
}

static void
mplib_unget_char (void *f, mplib_instance *mplib_data, int c) {
  if (f==stdin && mplib_data->input_data_ptr != NULL) {
	mplib_data->input_data_len++;	
	mplib_data->input_data_ptr--;
  } else {
	ungetc(c,f);
  }
}


char *mplib_read_ascii_file (MP mp, void *ff, size_t *size) {
  char *s = NULL;
  if (ff!=NULL) {
	int c;
	size_t len = 0, lim = 128;
    mplib_instance *mplib_data = mplib_get_data(mp);
    FILE *f = ((File *)ff)->f;
    if (f==NULL)
      return NULL;
    *size = 0;
    c = mplib_get_char(f,mplib_data);
    if (c==EOF)
      return NULL;
    s = malloc(lim); 
    if (s==NULL) return NULL;
    while (c!=EOF && c!='\n' && c!='\r') { 
      if (len==lim) {
	s =realloc(s, (lim+(lim>>2)));
	if (s==NULL) return NULL;
	lim+=(lim>>2);
      }
      s[len++] = c;
      c = mplib_get_char(f,mplib_data);
    }
    if (c=='\r') {
      c = mplib_get_char(f,mplib_data);
      if (c!=EOF && c!='\n')
	mplib_unget_char(f,mplib_data,c);
    }
    s[len] = 0;
    *size = len;
  }
  return s;
}

#define APPEND_STRING(a,b) do {						\
    if ((mplib_data->a##_used+strlen(b))>=mplib_data->a##_size) {	\
      mplib_data->a##_size += 256+(mplib_data->a##_size)/5+strlen(b);	\
      mplib_data->a = realloc(mplib_data->a,mplib_data->a##_size);	\
    }									\
    (void)strcpy(mplib_data->a+mplib_data->a##_used,b);			\
    mplib_data->a##_used += strlen(b);					\
  } while (0)

void mplib_write_ascii_file (MP mp, void *ff, const char *s) {
  if (ff!=NULL) {
    void *f = ((File *)ff)->f;
    mplib_instance *mplib_data = mplib_get_data(mp);
    if (f!=NULL) {
      if (f==mplib_data->term_file_ptr) {
	APPEND_STRING(term_out,s);
      } else if (f==mplib_data->err_file_ptr) {
	APPEND_STRING(error_out,s);
      } else if (f==mplib_data->log_file_ptr) {
	APPEND_STRING(log_out,s);
      } else if (f==mplib_data->ps_file_ptr) {
        APPEND_STRING(ps_out,s);
      } else {
		fprintf((FILE *)f,"%s",s);
      }
    }
  }
}

void mplib_read_binary_file (MP mp, void *ff, void **data, size_t *size) {
  (void)mp;
  if (ff!=NULL) {
    size_t len = 0;
    FILE *f = ((File *)ff)->f;
    if (f!=NULL) 
      len = fread(*data,1,*size,f);
    *size = len;
  }
}

void mplib_write_binary_file (MP mp, void *ff, void *s, size_t size) {
  (void)mp;
  if (ff!=NULL) {
    FILE *f = ((File *)ff)->f;
    if (f!=NULL)
      fwrite(s,size,1,f);
  }
}


void mplib_close_file (MP mp, void *ff) {
  if (ff!=NULL) {
    mplib_instance *mplib_data = mplib_get_data(mp);
    void *f = ((File *)ff)->f;
    if (f != NULL && f != mplib_data->term_file_ptr && f != mplib_data->err_file_ptr
	&& f != mplib_data->log_file_ptr && f != mplib_data->ps_file_ptr) {
      fclose(f);
    }
    free(ff);
  }
}

int mplib_eof_file (MP mp, void *ff) {
  if (ff!=NULL) {
    mplib_instance *mplib_data = mplib_get_data(mp);
    FILE *f = ((File *)ff)->f;
    if (f==NULL)
      return 1;
    if (f==stdin && mplib_data->input_data != NULL) {	
      return (mplib_data->input_data_len==0);
    }
    return feof(f);
  }
  return 1;
}

void mplib_flush_file (MP mp, void *ff) {
  (void)mp;
  (void)ff;
  return ;
}

#define APPEND_TO_EDGES(a) do {							\
    if (mplib_data->edges==NULL) {						\
      mplib_data->edges = hh;							\
    } else {											\
      struct mp_edge_object *p = mplib_data->edges;		\
      while (p->_next!=NULL) { p = p->_next; }			\
      p->_next = hh;									\
    }													\
} while (0)

void mplib_shipout_backend (MP mp, int h) {
  struct mp_edge_object *hh = mp_gr_export(mp, h);
  if (hh) {
    mplib_instance *mplib_data = mplib_get_data(mp);
    APPEND_TO_EDGES(hh); 
  }
}


static void 
mplib_setup_file_ops(struct MP_options * options) {
  options->find_file         = mplib_find_file;
  options->open_file         = mplib_open_file;
  options->close_file        = mplib_close_file;
  options->eof_file          = mplib_eof_file;
  options->flush_file        = mplib_flush_file;
  options->write_ascii_file  = mplib_write_ascii_file;
  options->read_ascii_file   = mplib_read_ascii_file;
  options->write_binary_file = mplib_write_binary_file;
  options->read_binary_file  = mplib_read_binary_file;
  options->shipout_backend   = mplib_shipout_backend;
}

static int 
mplib_new (lua_State *L) {
  MP *mp_ptr;
  mp_ptr = lua_newuserdata(L, sizeof(MP *));
  if (mp_ptr) {
	int i;
	mplib_instance *mplib_data;
	struct MP_options * options; /* instance options */
    options = mp_options();
    mplib_setup_file_ops(options);
    mplib_data = mplib_make_data();
    mplib_data->LL = L;
    options->userdata = (void *)mplib_data;
    options->noninteractive = 1; /* required ! */
    options->print_found_names = 0;
    if (lua_type(L,1)==LUA_TTABLE) {
      for (i=0;mplib_parms[i].name!=NULL;i++) {
	lua_getfield(L,1,mplib_parms[i].name);
	if (lua_isnil(L,-1)) {
          lua_pop(L,1);
	  continue; /* skip unset */
	}
        switch(mplib_parms[i].idx) {
	case P_ERROR_LINE: 
	  options->error_line = lua_tointeger(L,-1);
          break;
	case P_HALF_LINE:   
	  options->half_error_line = lua_tointeger(L,-1);
          break;
	case P_MAX_LINE:
	  options->max_print_line = lua_tointeger(L,-1);
          break;
	case P_MAIN_MEMORY:
	  options->main_memory = lua_tointeger(L,-1);
          break;
	case P_HASH_SIZE:
	  options->hash_size = lua_tointeger(L,-1);
          break;
	case P_HASH_PRIME:
	  options->hash_prime = lua_tointeger(L,-1);
          break;
	case P_PARAM_SIZE:
	  options->param_size = lua_tointeger(L,-1);
          break;
	case P_IN_OPEN:
	  options->max_in_open = lua_tointeger(L,-1);
          break;
	case P_RANDOM_SEED:
	  options->random_seed = lua_tointeger(L,-1);
          break;
	case P_INTERACTION:
          options->interaction = luaL_checkoption(L,-1,"errorstopmode", interaction_options);
	  break;
	case P_INI_VERSION:
	  options->ini_version = lua_toboolean(L,-1);
          break;
	case P_TROFF_MODE:
	  options->troff_mode = lua_toboolean(L,-1);
          break;
	case P_PRINT_NAMES:
	  options->print_found_names = lua_toboolean(L,-1);
          break;
	  /*	  
	case P_COMMAND_LINE:
	  options->command_line = strdup((char *)lua_tostring(L,-1));
          break;
	  */
	case P_MEM_NAME:
	  options->mem_name = strdup((char *)lua_tostring(L,-1));
          break;
	case P_JOB_NAME:
	  options->job_name = strdup((char *)lua_tostring(L,-1));
          break;
	case P_FIND_FILE:  
	  if(mplib_find_file_function(L)) { /* error here */
	    fprintf(stdout,"Invalid arguments to mp.new({find_file=...})\n");
	  }
	  break;
        default:
	  break;
	}
        lua_pop(L,1);
      }
    }
    *mp_ptr = mp_new(options);
    xfree(options->command_line);
    xfree(options->mem_name);
    xfree(options->job_name);
    free(options);
    if (*mp_ptr) {
      luaL_getmetatable(L,MPLIB_METATABLE);
      lua_setmetatable(L,-2);
      return 1;
    }
  }
  lua_pushnil(L);
  return 1;
}

static int
mplib_collect (lua_State *L) {
  MP *mp_ptr = is_mp(L,1);
  xfree(*mp_ptr);
  return 0;
}

static int
mplib_tostring (lua_State *L) {
  MP *mp_ptr = is_mp(L,1);
  if (*mp_ptr!=NULL) {
    lua_pushfstring(L,"<MP %p>",*mp_ptr);
     return 1;
  }
  return 0;
}

static int 
mplib_wrapresults(lua_State *L, mplib_instance *mplib_data, int h) {
   lua_checkstack(L,5);
   lua_newtable(L);
   if (mplib_data->term_out != NULL) {
     lua_pushstring(L,mplib_data->term_out);
     lua_setfield(L,-2,"term");
     free_stream_buf(term_out);
   }
   if (mplib_data->error_out != NULL) {
     lua_pushstring(L,mplib_data->error_out);
     lua_setfield(L,-2,"error");
     free_stream_buf(error_out);
   } 
   if (mplib_data->log_out != NULL ) {
     lua_pushstring(L,mplib_data->log_out);
     lua_setfield(L,-2,"log");
     free_stream_buf(log_out);
   }
   if (mplib_data->edges != NULL ) {
     struct mp_edge_object **v;
     struct mp_edge_object *p = mplib_data->edges;
     int i = 1;
     lua_newtable(L);
     while (p!=NULL) { 
       v = lua_newuserdata (L, sizeof(struct mp_edge_object *));
       *v = p;
       luaL_getmetatable(L,MPLIB_FIG_METATABLE);
       lua_setmetatable(L,-2);
       lua_rawseti(L,-2,i); i++;
       p = p->_next;
     }
     lua_setfield(L,-2,"fig");
     mplib_data->edges = NULL;
   }
   lua_pushnumber(L,h);
   lua_setfield(L,-2,"status");
   return 1;
}

static int
mplib_execute (lua_State *L) {
  MP *mp_ptr = is_mp(L,1);
  if (*mp_ptr!=NULL && lua_isstring(L,2)) {
    int h;
    mplib_instance *mplib_data = mplib_get_data(*mp_ptr);
    mplib_data->input_data = (char *)lua_tolstring(L,2, &(mplib_data->input_data_len));
    mplib_data->input_data_ptr = mplib_data->input_data;
    if ((*mp_ptr)->run_state==0) {
      h = mp_initialize(*mp_ptr);
    }
    h = mp_execute(*mp_ptr);
    if (mplib_data->input_data_len!=0) {
      mplib_data->input_data = NULL;
	  mplib_data->input_data_ptr = NULL;
      mplib_data->input_data_len=0;
    }
    return mplib_wrapresults(L, mplib_data, h);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int
mplib_finish (lua_State *L) {
  MP *mp_ptr = is_mp(L,1);
  if (*mp_ptr!=NULL) {
    mplib_instance *mplib_data = mplib_get_data(*mp_ptr);
    int h = mp_finish(*mp_ptr);
    return mplib_wrapresults(L, mplib_data, h);
  } else {
    lua_pushnil(L);
  }
  return 1;
}

static int 
mplib_statistics (lua_State *L) {
  MP *mp_ptr = is_mp(L,1);
  if (*mp_ptr!=NULL) {
	lua_newtable(L);
	lua_pushnumber(L, mp_memory_usage (*mp_ptr));
	lua_setfield(L,-2,"main_memory");
	lua_pushnumber(L, mp_hash_usage (*mp_ptr));
	lua_setfield(L,-2,"hash_size");
	lua_pushnumber(L, mp_param_usage (*mp_ptr));
	lua_setfield(L,-2,"param_size");
	lua_pushnumber(L, mp_open_usage (*mp_ptr));
	lua_setfield(L,-2,"max_in_open");
  } else {
    lua_pushnil(L);
  }
  return 1;
}


/* figure methods */

static int
mplib_fig_collect (lua_State *L) {
  struct mp_edge_object **hh = is_fig(L,1);
  if (*hh!=NULL) {
    mp_gr_toss_objects (*hh);
    *hh=NULL;
  }
  return 0;
}

static int
mplib_fig_body (lua_State *L) {
  int i = 1;
  struct mp_graphic_object **v;
  struct mp_graphic_object *p;
  struct mp_edge_object **hh = is_fig(L,1);
  lua_newtable(L);
  p = (*hh)->body;
  while (p!=NULL) {
    v = lua_newuserdata (L, sizeof(struct mp_graphic_object *));
    *v = p;
    luaL_getmetatable(L,MPLIB_GR_METATABLE);
    lua_setmetatable(L,-2);
    lua_rawseti(L,-2,i); i++;
    p = p->_link_field;
  }
  (*hh)->body = NULL; /* prevent double free */
  return 1;
}

static int
mplib_fig_copy_body (lua_State *L) {
  int i = 1;
  struct mp_graphic_object **v;
  struct mp_graphic_object *p;
  struct mp_edge_object **hh = is_fig(L,1);
  lua_newtable(L);
  p = (*hh)->body;
  while (p!=NULL) {
    v = lua_newuserdata (L, sizeof(struct mp_graphic_object *));
    *v = mp_gr_copy_object((*hh)->_parent,p);
    luaL_getmetatable(L,MPLIB_GR_METATABLE);
    lua_setmetatable(L,-2);
    lua_rawseti(L,-2,i); i++;
    p = p->_link_field;
  }
  return 1;
}


static int
mplib_fig_tostring (lua_State *L) {
  struct mp_edge_object **hh = is_fig(L,1);
  lua_pushfstring(L,"<figure %p>",*hh);
  return 1;
}



static int 
mp_wrapped_shipout (struct mp_edge_object *hh, int prologues, int procset) {
  MP mp = hh->_parent;
  if (setjmp(mp->jump_buf)) {
    return 0;
  }
  mp_gr_ship_out(hh,prologues,procset);
  return 1;
}

static int
mplib_fig_postscript (lua_State *L) {
  struct mp_edge_object **hh = is_fig(L,1);
  int prologues = luaL_optnumber(L,2,-1);
  int procset = luaL_optnumber(L,3,-1);
  mplib_instance *mplib_data = mplib_get_data((*hh)->_parent);
  if (mplib_data->ps_out == NULL) {
    if (mp_wrapped_shipout(*hh,prologues, procset)) {
      if (mplib_data->ps_out!=NULL ) {
	lua_pushstring(L, mplib_data->ps_out);
	free_stream_buf(ps_out);
      } else {
	lua_pushnil(L);
      }
      return 1;
    } else {
      lua_pushnil(L);
      lua_pushstring(L,mplib_data->log_out);
      xfree(mplib_data->ps_out); 
      return 2;
    }
  }
  lua_pushnil(L);
  return 1;
}

static int
mplib_fig_filename (lua_State *L) {
  struct mp_edge_object **hh = is_fig(L,1);
  if (*hh!= NULL) { 
	char *s = (*hh)->_filename;
	lua_pushstring(L,s);
  } else {
	lua_pushnil(L);
  }
  return 1;
}


static int
mplib_fig_bb (lua_State *L) {
  struct mp_edge_object **hh = is_fig(L,1);
  lua_newtable(L);
  lua_pushnumber(L, (double)(*hh)->_minx/65536.0);
  lua_rawseti(L,-2,1);
  lua_pushnumber(L, (double)(*hh)->_miny/65536.0);
  lua_rawseti(L,-2,2);
  lua_pushnumber(L, (double)(*hh)->_maxx/65536.0);
  lua_rawseti(L,-2,3);
  lua_pushnumber(L, (double)(*hh)->_maxy/65536.0);
  lua_rawseti(L,-2,4);
  return 1;
}

/* object methods */

static int
mplib_gr_collect (lua_State *L) {
  struct mp_graphic_object **hh = is_gr_object(L,1);
  if (*hh!=NULL) {
    mp_gr_toss_object(*hh);
    *hh=NULL;
  }
  return 0;
}

static int
mplib_gr_tostring (lua_State *L) {
  struct mp_graphic_object **hh = is_gr_object(L,1);
  lua_pushfstring(L,"<object %p>",*hh);
  return 1;
}


static int
mplib_gr_fields (lua_State *L) {
  const char **fields;
  int i ;
  struct mp_graphic_object **hh = is_gr_object(L,1);
  if (*hh) {
    switch ((*hh)->_type_field) {
    case mp_fill_code:         fields = fill_fields;         break;
    case mp_stroked_code:      fields = stroked_fields;      break;
    case mp_text_code:         fields = text_fields;         break;
    case mp_special_code:      fields = special_fields;      break;
    case mp_start_clip_code:   fields = start_clip_fields;   break;
    case mp_start_bounds_code: fields = start_bounds_fields; break;
    case mp_stop_clip_code:    fields = stop_clip_fields;    break;
    case mp_stop_bounds_code:  fields = stop_bounds_fields;  break;
    default:                   fields = no_fields;
    }
    lua_newtable(L);
    for (i=0;fields[i]!=NULL;i++) {
      lua_pushstring(L,fields[i]);
      lua_rawseti(L,-2,(i+1));
    }
  } else {
    lua_pushnil(L);
  }
  return 1;
}


#define mplib_push_number(L,x) lua_pushnumber(L,(lua_Number)(x)/65536.0)

#define MPLIB_PATH 0
#define MPLIB_PEN 1

static void 
mplib_push_path (lua_State *L, struct mp_knot *h, int is_pen) {
  struct mp_knot *p; /* for scanning the path */
  int i=1;
  p=h;
  if (p!=NULL) {
    lua_newtable(L);
    do {  
      lua_createtable(L,0,6);
      if (!is_pen) {
        if (p->left_type_field != mp_explicit) {
		  mplib_push_S(left_type);
		  lua_pushstring(L,knot_type_enum[p->left_type_field]);
		  lua_rawset(L,-3);
		}
		if (p->right_type_field != mp_explicit) {
		  mplib_push_S(right_type);
		  lua_pushstring(L,knot_type_enum[p->right_type_field]);
		  lua_rawset(L,-3);
		}
	  }
      mplib_push_S(x_coord);
      mplib_push_number(L,p->x_coord_field);
      lua_rawset(L,-3);
      mplib_push_S(y_coord);
      mplib_push_number(L,p->y_coord_field);
      lua_rawset(L,-3);
      mplib_push_S(left_x);
      mplib_push_number(L,p->left_x_field);
      lua_rawset(L,-3);
      mplib_push_S(left_y);
      mplib_push_number(L,p->left_y_field);
      lua_rawset(L,-3);
      mplib_push_S(right_x);
      mplib_push_number(L,p->right_x_field);
      lua_rawset(L,-3);
      mplib_push_S(right_y);
      mplib_push_number(L,p->right_y_field);
      lua_rawset(L,-3);
      lua_rawseti(L,-2,i); i++;
      if ( p->right_type_field==mp_endpoint ) { 
	return;
      }
      p=p->next_field;
    } while (p!=h) ;
  } else {
    lua_pushnil(L);
  }
}

/* this assumes that the top of the stack is a table 
   or nil already in the case
 */
static void 
mplib_push_pentype (lua_State *L, struct mp_knot *h) {
  struct mp_knot *p; /* for scanning the path */
  p=h;
  if (p==NULL) {
	/* do nothing */
  } else if (p==p->next_field) {
	mplib_push_S(type);
	lua_pushstring(L,"elliptical");
	lua_rawset(L,-3);
  } else {
  }
}

#define set_color_objects(pq)				\
  object_color_model = pq->color_model_field;		\
  object_color_a = pq->color_field._a_val;		\
  object_color_b = pq->color_field._b_val;		\
  object_color_c = pq->color_field._c_val;		\
  object_color_d = pq->color_field._d_val; 


static void 
mplib_push_color (lua_State *L, struct mp_graphic_object *p ) {
  int object_color_model;
  int object_color_a, object_color_b, object_color_c, object_color_d ; 
  if (p!=NULL) {
    if (p->_type_field == mp_fill_code) {
      mp_fill_object *h = (mp_fill_object *)p;
      set_color_objects(h);
    } else if (p->_type_field == mp_stroked_code) {
      mp_stroked_object *h = (mp_stroked_object *)p;
      set_color_objects(h);
    } else {
      mp_text_object *h = (mp_text_object *)p;
      set_color_objects(h);
    }
    lua_newtable(L);
    if (object_color_model >= mp_grey_model) {
      mplib_push_number(L,object_color_a);
      lua_rawseti(L,-2,1);
      if (object_color_model >= mp_rgb_model) {
	mplib_push_number(L,object_color_b);
	lua_rawseti(L,-2,2);
	mplib_push_number(L,object_color_c);
	lua_rawseti(L,-2,3);
	if (object_color_model == mp_cmyk_model) {	
	  mplib_push_number(L,object_color_d);
	  lua_rawseti(L,-2,4);
	}
      }
    }
  } else {
    lua_pushnil(L);
  }
}

/* the dash scale is not exported, the field has no external value */
static void 
mplib_push_dash (lua_State *L, struct mp_stroked_object *h ) {
  mp_dash_object *d;
  double ds;
  if (h!=NULL && h->dash_p_field != NULL) {
    d  = h->dash_p_field;
    lua_newtable(L);
    mplib_push_number(L,d->offset_field);
    lua_setfield(L,-2,"offset");
    if (d->array_field!=NULL ) {
      int i = 0;
      lua_newtable(L);
      while (*(d->array_field+i) != -1) {
        ds = *(d->array_field+1) / 65536.0;
	lua_pushnumber(L, ds);
	i++;
	lua_rawseti(L,-2,i);
      }
      lua_setfield(L,-2,"dashes");
    }
  } else {
    lua_pushnil(L);
  }
}

static void 
mplib_push_transform (lua_State *L, struct mp_text_object *h ) {
  int i = 1;
  if (h!=NULL) {
    lua_createtable(L,6,0);
    mplib_push_number(L,h->tx_field);
    lua_rawseti(L,-2,i); i++;
    mplib_push_number(L,h->ty_field);
    lua_rawseti(L,-2,i); i++;
    mplib_push_number(L,h->txx_field);
    lua_rawseti(L,-2,i); i++;
    mplib_push_number(L,h->tyx_field);
    lua_rawseti(L,-2,i); i++;
    mplib_push_number(L,h->txy_field);
    lua_rawseti(L,-2,i); i++;
    mplib_push_number(L,h->tyy_field);
    lua_rawseti(L,-2,i); i++;
  } else {
    lua_pushnil(L);
  }
}

#define FIELD(A) (mplib_is_S(A,2))

static void 
mplib_fill_field (lua_State *L, struct mp_fill_object *h) {
  if (FIELD(path)) {
    mplib_push_path(L, h->path_p_field, MPLIB_PATH);
  } else if (FIELD(htap)) {
    mplib_push_path(L, h->htap_p_field, MPLIB_PATH);
  } else if (FIELD(pen)) {
    mplib_push_path(L, h->pen_p_field, MPLIB_PEN);
	mplib_push_pentype(L, h->pen_p_field);
  } else if (FIELD(color)) {
    mplib_push_color(L,(mp_graphic_object *)h);
  } else if (FIELD(linejoin)) {
    lua_pushnumber(L,h->ljoin_field);
  } else if (FIELD(miterlimit)) {
    mplib_push_number(L,h->miterlim_field);
  } else if (FIELD(prescript)) {
    lua_pushstring(L,h->pre_script_field);
  } else if (FIELD(postscript)) {
    lua_pushstring(L,h->post_script_field);
  } else {
    lua_pushnil(L);
  }
}

static void 
mplib_stroked_field (lua_State *L, struct mp_stroked_object *h) {
  if (FIELD(path)) {
    mplib_push_path(L, h->path_p_field, MPLIB_PATH);
  } else if (FIELD(pen)) {
    mplib_push_path(L, h->pen_p_field, MPLIB_PEN);
	mplib_push_pentype(L, h->pen_p_field);
  } else if (FIELD(color)) {
    mplib_push_color(L, (mp_graphic_object *)h);
  } else if (FIELD(dash)) {
    mplib_push_dash(L,h);
  } else if (FIELD(linecap)) {
    lua_pushnumber(L,h->lcap_field);
  } else if (FIELD(linejoin)) {
    lua_pushnumber(L,h->ljoin_field);
  } else if (FIELD(miterlimit)) {
    mplib_push_number(L,h->miterlim_field);
  } else if (FIELD(prescript)) {
    lua_pushstring(L,h->pre_script_field);
  } else if (FIELD(postscript)) {
    lua_pushstring(L,h->post_script_field);
  } else {
    lua_pushnil(L);
  }
}

static void 
mplib_text_field (lua_State *L, struct mp_text_object *h) {
  if (FIELD(text)) {
    lua_pushstring(L,h->text_p_field);
  } else if (FIELD(dsize)) {
    mplib_push_number(L,(h->font_dsize_field/16));
  } else if (FIELD(font)) {
    lua_pushstring(L,h->font_name_field);
  } else if (FIELD(color)) {
    mplib_push_color(L,(mp_graphic_object *)h);
  } else if (FIELD(width)) {
    mplib_push_number(L,h->width_field);
  } else if (FIELD(height)) {
    mplib_push_number(L,h->height_field);
  } else if (FIELD(depth)) {
    mplib_push_number(L,h->depth_field);
  } else if (FIELD(transform)) {
    mplib_push_transform(L,h);
  } else if (FIELD(prescript)) {
    lua_pushstring(L,h->pre_script_field);
  } else if (FIELD(postscript)) {
    lua_pushstring(L,h->post_script_field);
  } else {
    lua_pushnil(L);
  }
}

static void 
mplib_special_field (lua_State *L, struct mp_special_object *h) {
  if (FIELD(prescript)) {
    lua_pushstring(L,h->pre_script_field);
  } else {
    lua_pushnil(L);
  }
}

static void 
mplib_start_bounds_field (lua_State *L, struct mp_bounds_object *h) {
  if (FIELD(path)) {
    mplib_push_path(L,h->path_p_field, MPLIB_PATH);
  } else {
    lua_pushnil(L);
  }
}

static void 
mplib_start_clip_field (lua_State *L, struct mp_clip_object *h) {
  if (FIELD(path)) {
    mplib_push_path(L,h->path_p_field,  MPLIB_PATH);
  } else {
    lua_pushnil(L);
  }
}

static int
mplib_gr_index (lua_State *L) {
  struct mp_graphic_object **hh = is_gr_object(L,1);
  if (*hh) {
    struct mp_graphic_object *h = *hh;

    if (mplib_is_S(type,2)) {
      lua_rawgeti(L,LUA_REGISTRYINDEX,mplib_type_Ses[h->_type_field]);
    } else {
      switch (h->_type_field) {
      case mp_fill_code:         mplib_fill_field(L,(mp_fill_object *)h);           break;
      case mp_stroked_code:      mplib_stroked_field(L,(mp_stroked_object *)h);     break;
      case mp_text_code:         mplib_text_field(L,(mp_text_object *)h);           break;
      case mp_special_code:      mplib_special_field(L,(mp_special_object *)h);     break;
      case mp_start_clip_code:   mplib_start_clip_field(L,(mp_clip_object *)h);     break;
      case mp_start_bounds_code: mplib_start_bounds_field(L,(mp_bounds_object *)h); break;
	/* case mp_stop_clip_code:    */
	/* case mp_stop_bounds_code:  */
      default:                   lua_pushnil(L);
      }    
    }
  } else {
    lua_pushnil(L);
  }
  return 1;
}


static const struct luaL_reg mplib_meta[] = {
  {"__gc",               mplib_collect}, 
  {"__tostring",         mplib_tostring},
  {NULL, NULL}                /* sentinel */
};

static const struct luaL_reg mplib_fig_meta[] = {
  {"__gc",               mplib_fig_collect    },
  {"__tostring",         mplib_fig_tostring   },
  {"objects",            mplib_fig_body       },
  {"copy_objects",       mplib_fig_copy_body  },
  {"filename",           mplib_fig_filename   },
  {"postscript",         mplib_fig_postscript },
  {"boundingbox",        mplib_fig_bb         },
  {NULL, NULL}                /* sentinel */
};

static const struct luaL_reg mplib_gr_meta[] = {
  {"__gc",               mplib_gr_collect  },
  {"__tostring",         mplib_gr_tostring },
  {"__index",            mplib_gr_index    },
  {NULL, NULL}                /* sentinel */
};


static const struct luaL_reg mplib_d [] = {
  {"execute",            mplib_execute },
  {"finish",             mplib_finish },
  {"statistics",         mplib_statistics },
  {NULL, NULL}  /* sentinel */
};


static const struct luaL_reg mplib_m[] = {
  {"new",                 mplib_new            },
  {"fields",              mplib_gr_fields      },
  {NULL, NULL}                /* sentinel */
};


int 
luaopen_mp (lua_State *L) {
  mplib_init_Ses(L);

  luaL_newmetatable(L,MPLIB_GR_METATABLE);
  lua_pushvalue(L, -1); /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_register(L, NULL, mplib_gr_meta);  /* object meta methods */
  lua_pop(L,1);

  luaL_newmetatable(L,MPLIB_FIG_METATABLE);
  lua_pushvalue(L, -1); /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_register(L, NULL, mplib_fig_meta);  /* figure meta methods */
  lua_pop(L,1);

  luaL_newmetatable(L,MPLIB_METATABLE);
  lua_pushvalue(L, -1); /* push metatable */
  lua_setfield(L, -2, "__index"); /* metatable.__index = metatable */
  luaL_register(L, NULL, mplib_meta);  /* meta methods */
  luaL_register(L, NULL, mplib_d);  /* dict methods */
  luaL_register(L, "mplib", mplib_m); /* module functions */
  return 1;
}

