/* $Id: luatoken.c 1168 2008-04-15 13:43:34Z taco $ */

#include "luatex-api.h"
#include <ptexlib.h>

#include "tokens.h"

command_item command_names[] = 
  { { "relax", 0, NULL },
    { "left_brace", 0 , NULL },
    { "right_brace", 0 , NULL },
    { "math_shift", 0 , NULL },
    { "tab_mark", 0 , NULL },
    { "car_ret", 0 , NULL },
    { "mac_param", 0 , NULL },
    { "sup_mark", 0 , NULL },
    { "sub_mark", 0 , NULL },
    { "endv", 0 , NULL },
    { "spacer", 0 , NULL },
    { "letter", 0 , NULL },
    { "other_char", 0 , NULL },
    { "par_end", 0 , NULL },
    { "stop", 0 , NULL },
    { "delim_num", 0 , NULL  },
    { "char_num", 0 , NULL },
    { "math_char_num", 0 , NULL },
    { "mark", 0 , NULL  },
    { "xray", 0 , NULL },
    { "make_box", 0 , NULL },
    { "hmove", 0 , NULL },
    { "vmove", 0 , NULL },
    { "un_hbox", 0 , NULL },
    { "un_vbox", 0 , NULL },
    { "remove_item", 0 , NULL },
    { "hskip", 0 , NULL },
    { "vskip", 0 , NULL },
    { "mskip", 0 , NULL },
    { "kern", 0 , NULL },
    { "mkern", 0 , NULL },
    { "leader_ship", 0 , NULL },
    { "halign", 0 , NULL },
    { "valign", 0 , NULL },
    { "no_align", 0 , NULL  },
    { "vrule", 0 , NULL },
    { "hrule", 0 , NULL },
    { "insert", 0 , NULL },
    { "vadjust", 0 , NULL },
    { "ignore_spaces", 0 , NULL },
    { "after_assignment", 0 , NULL },
    { "after_group", 0 , NULL },
    { "break_penalty", 0 , NULL  },
    { "start_par", 0 , NULL },
    { "ital_corr", 0 , NULL },
    { "accent", 0 , NULL },
    { "math_accent", 0 , NULL },
    { "discretionary", 0 , NULL },
    { "eq_no", 0 , NULL },
    { "left_right", 0 , NULL },
    { "math_comp", 0 , NULL },
    { "limit_switch", 0 , NULL },
    { "above", 0 , NULL },
    { "math_style", 0 , NULL },
    { "math_choice", 0 , NULL },
    { "non_script", 0 , NULL },
    { "vcenter", 0 , NULL },
    { "case_shift", 0 , NULL },
    { "message", 0 , NULL },
    { "extension", 0 , NULL },
    { "in_stream", 0 , NULL },
    { "begin_group", 0 , NULL },
    { "end_group", 0 , NULL },
    { "omit", 0 , NULL },
    { "ex_space", 0 , NULL },
    { "no_boundary", 0 , NULL },
    { "radical", 0 , NULL },
    { "end_cs_name", 0 , NULL },
    { "char_ghost", 0 , NULL },
    { "assign_local_box", 0 , NULL },
    { "char_given", 0 , NULL },
    { "math_given", 0 , NULL },
    { "omath_given", 0 , NULL },
    { "last_item", 0 , NULL },
    { "toks_register", 0 , NULL },
    { "assign_toks", 0, NULL },
    { "assign_int", 0, NULL },
    { "assign_dimen", 0 , NULL },
    { "assign_glue", 0 ,NULL },
    { "assign_mu_glue", 0 , NULL },
    { "assign_font_dimen", 0 , NULL },
    { "assign_font_int", 0 , NULL },
    { "set_aux", 0 , NULL },
    { "set_prev_graf", 0 , NULL },
    { "set_page_dimen", 0 , NULL },
    { "set_page_int", 0 , NULL },
    { "set_box_dimen", 0 , NULL },
    { "set_shape", 0, NULL },
    { "def_code", 0 , NULL },
    { "extdef_code", 0 , NULL },
    { "def_family", 0 , NULL },
    { "set_font", 0 , NULL },
    { "def_font", 0 , NULL },
    { "register", 0 , NULL },
    { "assign_box_dir", 0 , NULL },
    { "assign_dir", 0 , NULL },
    { "advance", 0 , NULL },
    { "multiply", 0 , NULL },
    { "divide", 0 , NULL },
    { "prefix", 0 , NULL },
    { "let", 0 , NULL }, /* 100 */
    { "shorthand_def", 0 , NULL },
    { "read_to_cs", 0 , NULL },
    { "def", 0 , NULL },
    { "set_box", 0 , NULL },
    { "hyph_data", 0 , NULL },
    { "set_interaction", 0 , NULL },
    { "letterspace_font", 0 , NULL },
    { "set_ocp", 0 , NULL },
    { "def_ocp", 0 , NULL },
    { "set_ocp_list", 0 , NULL }, /* 110 */
    { "def_ocp_list", 0 , NULL },
    { "clear_ocp_lists", 0 , NULL },
    { "push_ocp_list", 0 , NULL },
    { "pop_ocp_list", 0 , NULL },
    { "ocp_list_op", 0 , NULL },
    { "ocp_trace_level", 0 , NULL},
    { "undefined_cs", 0 , NULL },
    { "expand_after", 0 , NULL },
    { "no_expand", 0 , NULL },
    { "input", 0 , NULL }, /* 120 */
    { "if_test", 0 , NULL },
    { "fi_or_else", 0 , NULL },
    { "cs_name", 0 , NULL },
    { "convert", 0 , NULL },
    { "the", 0 , NULL  },
    { "top_bot_mark", 0 , NULL },
    { "call", 0 , NULL },
    { "long_call", 0 , NULL },
    { "outer_call", 0 , NULL },
    { "long_outer_call", 0 , NULL }, /* 130 */
    { "end_template", 0 , NULL },
    { "dont_expand", 0, NULL },
    { "glue_ref", 0 , NULL },
    { "shape_ref", 0 , NULL },
    { "box_ref", 0 , NULL },
    { "data", 0 , NULL },
    {  NULL, 0, NULL } };


int get_command_id (char *s) {
  int i;
  int cmd  = -1;
  for (i=0;command_names[i].cmd_name != NULL;i++) {
    if (strcmp(s,command_names[i].cmd_name) == 0) 
      break;
  }
  if (command_names[i].cmd_name!=NULL) {
    cmd = i;
  }
  return cmd;
}

static int
get_cur_cmd (lua_State *L) {
  int r = 0, len;
  cur_cs = 0;
  len = lua_objlen(L,-1);
  if (len==3 || len==2) {
    r = 1;
    lua_rawgeti(L,-1,1);
    cur_cmd = lua_tointeger(L,-1);
    lua_rawgeti(L,-2,2);
    cur_chr = lua_tointeger(L,-1);
	if (len==3) {
	  lua_rawgeti(L,-3,3);
	  cur_cs = lua_tointeger(L,-1);
	}
	lua_pop(L,len);
    if (cur_cs==0) 
      cur_tok=(cur_cmd*string_offset)+cur_chr; 
    else
      cur_tok=cs_token_flag+cur_cs; 
  }
  return r;
}


static int
token_from_lua (lua_State *L) {
  int cmd,chr;
  int cs = 0;  
  int len = lua_objlen(L,-1);
  if (len==3 || len==2) {
    lua_rawgeti(L,-1,1);
    cmd = lua_tointeger(L,-1);
    lua_rawgeti(L,-2,2);
    chr = lua_tointeger(L,-1);
	if (len==3) {
	  lua_rawgeti(L,-3,3);
	  cs = lua_tointeger(L,-1);
	}
	lua_pop(L,len);
    if (cs==0) {
      return (cmd*string_offset)+chr; 
    } else {
      return cs_token_flag+cs; 
    }
  }
  return -1;
}

static int
get_cur_cs (lua_State *L) {
  char *s;
  unsigned j;
  size_t l;
  integer cs;
  int save_nncs;
  int ret;
  ret = 0;
  cur_cs = 0;
  lua_getfield(L,-1,"name");
  if (lua_isstring(L,-1)) {
    s = (char *)lua_tolstring(L,-1,&l);
    if (l>0) {
      if ((int)(last+l)>buf_size)
        check_buffer_overflow(last+l);
      for (j=0;j<l;j++) {
	buffer[last+1+j]=*s++;
      }
      save_nncs = no_new_control_sequence;
      no_new_control_sequence = false;
      cs = id_lookup((last+1),l);
      cur_tok = cs_token_flag+cs;
      cur_cmd = zget_eq_type(cs);
      cur_chr = zget_equiv(cs);
      no_new_control_sequence = save_nncs;
      ret = 1;
    }
  }
  lua_pop(L,1);
  return ret;
}


#define append_i_byte(a) {				\
    if ((i+2)>alloci) {					\
      ret = xrealloc(ret,alloci+64);			\
      alloci = alloci + 64; }				\
    ret[i++] = a; }

#define Print_char(a) append_i_byte(a)

#define Print_uchar(s) {					\
  if (s<=0x7F) {						\
    Print_char(s);						\
  } else if (s<=0x7FF) {					\
    Print_char(0xC0 + (s / 0x40));				\
    Print_char(0x80 + (s % 0x40));				\
  } else if (s<=0xFFFF) {					\
    Print_char(0xE0 + (s / 0x1000));				\
    Print_char(0x80 + ((s % 0x1000) / 0x40));			\
    Print_char(0x80 + ((s % 0x1000) % 0x40));			\
  } else if (s>=0x10FF00) {					\
    Print_char(s-0x10FF00);					\
  } else {							\
    Print_char(0xF0 + (s / 0x40000));				\
    Print_char(0x80 + ((s % 0x40000) / 0x1000));		\
    Print_char(0x80 + (((s % 0x40000) % 0x1000) / 0x40));	\
    Print_char(0x80 + (((s % 0x40000) % 0x1000) % 0x40));	\
  } }


#define Print_esc(b) { if (e>0 && e<string_offset) { Print_uchar (e); Print_uchar (e); }	\
    { char *v = b; while (*v) { Print_char(*v); v++; } } }

#define single_letter(a) (length(a)==1)||			\
  ((length(a)==4)&&(str_pool[str_start_macro(a)]>=0xF0))||	\
  ((length(a)==3)&&(str_pool[str_start_macro(a)]>=0xE0))||	\
  ((length(a)==2)&&(str_pool[str_start_macro(a)]>=0xC0))

#define is_cat_letter(a)						\
  (get_char_cat_code(pool_to_unichar(str_start_macro(a))) == 11)

static int active_base = 0;
static int hash_base = 0;
static int eqtb_size = 0;
static int null_cs = 0;
static int undefined_control_sequence;

char * 
tokenlist_to_cstring ( int p , int inhibit_par, int *siz) {
  integer m, c  ;
  integer q;
  char *s;
  int e;
  char *ret=NULL;
  int match_chr = '#';
  int n = '0';
  int alloci = 0;
  int i = 0;
  if (p==null || link(p)==null) {
    if (siz!=NULL)
      *siz = 0;
    return NULL;
  }
  p = link(p); /* skip refcount */
  if (active_base==0) {
	active_base = get_active_base();
	hash_base = get_hash_base();
	null_cs = get_nullcs();
	eqtb_size = get_eqtb_size();
	undefined_control_sequence = get_undefined_control_sequence();
  }
  e = get_escape_char();
  while ( p != null ) {      
    if (p < fix_mem_min || p > fix_mem_end )  {
      Print_esc ("CLOBBERED.") ;
      break;
    } 
    if (info(p)>=cs_token_flag) {
      if ( ! (inhibit_par && info(p)==par_token) ) {
		q = info(p) - cs_token_flag;
		if (q<hash_base) {
		  if (q==null_cs) {
			/* Print_esc("csname"); Print_esc("endcsname"); */
		  } else {
			if (q<active_base) {
			  Print_esc("IMPOSSIBLE.");
			} else {
			  Print_uchar(q-active_base);
			}
		  }
		} else if ((q>=undefined_control_sequence)&&((q<=eqtb_size)||(q>eqtb_size+hash_extra))) {
		  Print_esc("IMPOSSIBLE.");
		} else if ((zget_cs_text(q)<0)||(zget_cs_text(q)>=str_ptr)) {
		  Print_esc("NONEXISTENT.");
		} else {
		  Print_uchar (e); 	
		  s = makecstring(zget_cs_text(q));
		  while (*s) { Print_char(*s); s++; }
		  if ((! single_letter(zget_cs_text(q))) || is_cat_letter(zget_cs_text(q))) { 
			Print_char(' ');
		  }
		}
      }
    } else {
      m=info(p) / string_offset; 
      c=info(p) % string_offset;
      if ( info(p) < 0 ) {
		Print_esc ( "BAD.") ;
      } else { 
		switch ( m ) {
		case 6 : /* falls through */
		  Print_uchar ( c ) ;
		case 1 : 
		case 2 : 
		case 3 : 
		case 4 : 
		case 7 : 
		case 8 : 
		case 10 : 
		case 11 : 
		case 12 : 
		  Print_uchar ( c ) ;
		  break ;
		case 5 : 
		  Print_uchar ( match_chr ) ;
		  if ( c <= 9 ) {
			Print_char ( c + '0') ;
		  } else {
			Print_char ( '!' ) ;
			return NULL;
		  } 
		  break ;
		case 13 : 
		  match_chr = c ;
		  Print_uchar ( c ) ;
		  incr ( n ) ;
		  Print_char ( n ) ;
		  if ( n > '9' ) 
			return NULL;
		  break ;
		case 14 : 
		  if ( c == 0 ) {
			Print_char ('-');
			Print_char ('>') ;
		  }
		  break ;
		default: 
		  Print_esc ( "BAD.") ;
		  break ;
		} 
      } 
    } 
    p = link(p);
  } 
  ret[i]=0;
  if (siz!=NULL)
    *siz = i;
  return ret;
}


void
tokenlist_to_lua(lua_State *L, int p) {
  int cmd,chr,cs;
  int v;
  int i = 1;
  v = p;
  while (v!=null && v < fix_mem_end) {   i++;    v = link(v);  }
  i = 1;
  lua_createtable(L,i,0);
  while (p!=null&& p < fix_mem_end) {
    if (info(p)>=cs_token_flag) {
      cs=info(p)-cs_token_flag;
      cmd = zget_eq_type(cs);
      chr = zget_equiv(cs);
      make_token_table(L,cmd,chr,cs);
    } else {
      cmd=info(p) / string_offset; 
      chr=info(p) % string_offset;
      make_token_table(L,cmd,chr,0);
    }
    lua_rawseti(L,-2,i++);
    p = link(p);
  }
}


void
tokenlist_to_luastring(lua_State *L, int p) {
  int l;
  char *s;
  s = tokenlist_to_cstring(p,1,&l);
  lua_pushlstring(L,s,l);
}


int
tokenlist_from_lua(lua_State *L) {
  char *s;
  int tok;
  size_t i,j;
  halfword p,q,r;
  r = get_avail();
  info(r)=0; /* ref count */
  link(r)=null;
  p = r;
  if (lua_istable(L,-1)) {
    j = lua_objlen(L,-1);
    if (j>0) {
      for (i=1;i<=j;i++) {
	lua_rawgeti(L,-1,i);
	tok = token_from_lua(L);
	if (tok>=0) {
	  store_new_token(tok);
	}
	lua_pop(L,1);
      };
    }
    return r;
  } else if (lua_isstring(L,-1)) {
    s = (char *)lua_tolstring(L,-1,&j);
    for (i=0;i<j;i++) {
      if (s[i] == 32) {
	tok = (10*string_offset)+s[i]; 	  
      } else {
	tok = (12*string_offset)+s[i]; 	  
      }
      store_new_token(tok);
    }
    return r;
  } else {
    free_avail(r);
    return null;
  }
}

void
do_get_token_lua (integer callback_id) {
  lua_State *L = Luas[0];

  lua_rawgeti(L,LUA_REGISTRYINDEX,callback_callbacks_id);
  while (1) {
    lua_rawgeti(L,-1, callback_id);
    if (!lua_isfunction(L,-1)) {
      lua_pop(L,2); /* the not-a-function callback and the container */
      get_next();
      return;
    }
    if (lua_pcall(L,0,1,0) != 0) { /* no arg, 1 result */
      fprintf(stdout,"error: %s\n",lua_tostring(L,-1));
      lua_pop(L,2);
      error();
      return;
    }
    if (lua_istable(L,-1)) {
      lua_rawgeti(L,-1,1);
      if (lua_istable(L,-1)) {
	integer p,q,r;
	int i,j;
	lua_pop(L,1); 
	/* build a token list */
	r = get_avail(); 
	p = r;
	j = lua_objlen(L,-1);
	if (j>0) {
	  for (i=1;i<=j;i++) {
	    lua_rawgeti(L,-1,i);
	    if (get_cur_cmd(L) || get_cur_cs(L)) {
	      store_new_token(cur_tok);
	    }
	    lua_pop(L,1);
	  }
	}
	if (p!=r) {
	  p = link(r);
	  free_avail(r);
	  begin_token_list(p, inserted);
	  cur_input.nofilter_field=true;
	  get_next();
	  lua_pop(L,1);
	  break;	
	} else {
	  fprintf(stdout,"error: illegal or empty token list returned\n");
	  lua_pop(L,2);
	  error();
	  return;
	}
      } else {
	lua_pop(L,1);
	if (get_cur_cmd(L) || get_cur_cs(L)) {
	  lua_pop(L,1);
	  break;	
	} else {
	  lua_pop(L,2);
	  continue; 
	}
      }
    } else {
      lua_pop(L,1);  
      continue;	
    }
  }
  lua_pop(L,1); /* callback container */
  return; 
}

