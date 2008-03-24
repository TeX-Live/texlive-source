
#include "luatex-api.h"
#include <ptexlib.h>

#include "nodes.h"

#define noVERBOSE

#define SAVE_REF 1

char *font_type_strings[]      = {"unknown","virtual","real", NULL};
char *font_format_strings[]    = {"unknown","type1","type3","truetype", "opentype", NULL};
char *font_embedding_strings[] = {"unknown","no","subset", "full", NULL};
char *ligature_type_strings[]  = {"=:", "=:|", "|=:", "|=:|", "", "=:|>", "|=:>", "|=:|>", "", "", "", "|=:|>>", NULL };

void
font_char_to_lua (lua_State *L, internalfontnumber f, charinfo *co) {
  int i;
  liginfo *l;
  kerninfo *ki;

  lua_createtable(L,0,10);

  lua_pushstring(L,"width");
  lua_pushnumber(L,get_charinfo_width(co));
  lua_rawset(L,-3);

  lua_pushstring(L,"height");
  lua_pushnumber(L,get_charinfo_height(co));
  lua_rawset(L,-3);

  lua_pushstring(L,"depth");
  lua_pushnumber(L,get_charinfo_depth(co));
  lua_rawset(L,-3);

  lua_pushstring(L,"italic");
  lua_pushnumber(L,get_charinfo_italic(co));
  lua_rawset(L,-3);

  if (get_charinfo_ef(co)!=0) {
	lua_pushstring(L,"expansion_factor");
	lua_pushnumber(L,get_charinfo_ef(co));
	lua_rawset(L,-3);
  }

  if (get_charinfo_lp(co)!=0) {
	lua_pushstring(L,"left_protruding");
	lua_pushnumber(L,get_charinfo_lp(co));
	lua_rawset(L,-3);
  }

  if (get_charinfo_lp(co)!=0) {
	lua_pushstring(L,"right_protruding");
	lua_pushnumber(L,get_charinfo_rp(co));
	lua_rawset(L,-3);
  }


  if (font_encodingbytes(f) == 2 ) {
        lua_pushstring(L,"index");
    lua_pushnumber(L,get_charinfo_index(co));
    lua_rawset(L,-3);
  }

  if (get_charinfo_name(co)!=NULL) {
        lua_pushstring(L,"name");
    lua_pushstring(L,get_charinfo_name(co));
    lua_rawset(L,-3);
  }

  if (get_charinfo_tounicode(co)!=NULL) {
        lua_pushstring(L,"tounicode");
    lua_pushstring(L,get_charinfo_tounicode(co));
    lua_rawset(L,-3);
  }

  if (get_charinfo_tag(co) == list_tag) {
        lua_pushstring(L,"next");
    lua_pushnumber(L,get_charinfo_remainder(co));
    lua_rawset(L,-3);
  }

  lua_pushstring(L,"used");
  lua_pushboolean(L,(get_charinfo_used(co) ? true : false));
  lua_rawset(L,-3);

  if (get_charinfo_tag(co) == ext_tag) {
        lua_pushstring(L,"extensible");
    lua_createtable(L,0,4);    
    lua_pushnumber(L,get_charinfo_extensible(co,EXT_TOP));
    lua_setfield(L,-2,"top");
    lua_pushnumber(L,get_charinfo_extensible(co,EXT_BOT));
    lua_setfield(L,-2,"bot");
    lua_pushnumber(L,get_charinfo_extensible(co,EXT_MID));
    lua_setfield(L,-2,"mid");
    lua_pushnumber(L,get_charinfo_extensible(co,EXT_REP));
    lua_setfield(L,-2,"rep");
    lua_rawset(L,-3);
  }
  ki = get_charinfo_kerns(co);
  if (ki != NULL) {
    lua_pushstring(L,"kerns");
    lua_createtable(L,10,1);
    for (i=0;!kern_end(ki[i]);i++) {
      if (kern_char(ki[i]) == right_boundarychar) {
                lua_pushstring(L,"right_boundary");
      } else {
                lua_pushnumber(L,kern_char(ki[i]));
      }
      lua_pushnumber(L,kern_kern(ki[i]));
      lua_rawset(L,-3);
    }
    lua_rawset(L,-3);
  }
  l = get_charinfo_ligatures(co);
  if (l!=NULL) {
        lua_pushstring(L,"ligatures");
    lua_createtable(L,10,1);
    for (i=0;!lig_end(l[i]);i++) {
      if (lig_char(l[i]) == right_boundarychar) {
                lua_pushstring(L,"right_boundary");
      } else {
                lua_pushnumber(L,lig_char(l[i]));
      }
      lua_createtable(L,0,2);
          lua_pushstring(L,"type");
      lua_pushnumber(L,lig_type(l[i]));
      lua_rawset(L,-3);
          lua_pushstring(L,"char");
      lua_pushnumber(L,lig_replacement(l[i]));
      lua_rawset(L,-3);
      lua_rawset(L,-3);
    }
    lua_rawset(L,-3);
  }
}

static void
write_lua_parameters (lua_State *L, int f) {
  int k;
  lua_newtable(L);
  for (k=1;k<=font_params(f);k++) {
    lua_pushnumber(L,font_param(f,k));
    switch (k) {
    case slant_code:         lua_setfield(L,-2,"slant");         break;
    case space_code:         lua_setfield(L,-2,"space");         break;
    case space_stretch_code: lua_setfield(L,-2,"space_stretch"); break;
    case space_shrink_code:  lua_setfield(L,-2,"space_shrink");  break;
    case x_height_code:      lua_setfield(L,-2,"x_height");      break;
    case quad_code:          lua_setfield(L,-2,"quad");          break;
    case extra_space_code:   lua_setfield(L,-2,"extra_space");   break;
    default:
      lua_rawseti(L,-2,k);
    }
  }
  lua_setfield(L,-2,"parameters");
}


int
font_to_lua (lua_State *L, int f) {
  int k;
  charinfo *co;
  if (font_cache_id(f)) {
    /* fetch the table from the registry if  it was 
       saved there by font_from_lua() */ 
    lua_rawgeti(L,LUA_REGISTRYINDEX,font_cache_id(f));
    /* fontdimens can be changed from tex code */
    write_lua_parameters(L,f);
    return 1;
  }

  lua_newtable(L);
  lua_pushstring(L,font_name(f));
  lua_setfield(L,-2,"name");
  if(font_area(f)!=NULL) {
        lua_pushstring(L,font_area(f));
        lua_setfield(L,-2,"area");
  }
  if(font_filename(f)!=NULL) {
        lua_pushstring(L,font_filename(f));
        lua_setfield(L,-2,"filename");
  }
  if(font_fullname(f)!=NULL) {
        lua_pushstring(L,font_fullname(f));
        lua_setfield(L,-2,"fullname");
  }
  if(font_encodingname(f)!=NULL) {
        lua_pushstring(L,font_encodingname(f));
        lua_setfield(L,-2,"encodingname");
  }

  lua_pushboolean(L,(font_used(f) ? true : false));
  lua_setfield(L,-2,"used");

  
  lua_pushstring(L,font_type_strings[font_type(f)]);
  lua_setfield(L,-2,"type");
  lua_pushstring(L,font_format_strings[font_format(f)]);
  lua_setfield(L,-2,"format");
  lua_pushstring(L,font_embedding_strings[font_embedding(f)]);
  lua_setfield(L,-2,"embedding");
  
  lua_pushnumber(L,font_size(f));
  lua_setfield(L,-2,"size");
  lua_pushnumber(L,font_dsize(f));
  lua_setfield(L,-2,"designsize");
  lua_pushnumber(L,font_checksum(f));
  lua_setfield(L,-2,"checksum");
  lua_pushnumber(L,font_slant(f));
  lua_setfield(L,-2,"slant");
  lua_pushnumber(L,font_extend(f));
  lua_setfield(L,-2,"extend");
  lua_pushnumber(L,font_natural_dir(f));
  lua_setfield(L,-2,"direction");
  lua_pushnumber(L,font_encodingbytes(f));
  lua_setfield(L,-2,"encodingbytes");
  lua_pushnumber(L,font_tounicode(f));
  lua_setfield(L,-2,"tounicode");

  /* pdf parameters */
  /* skip the first four for now, that are very much interal */
  /*
  if (pdf_font_size(f) != 0) {
	lua_pushnumber(L,pdf_font_size(f));
	lua_setfield(L,-2,"pdf_size");
  }
  if (pdf_font_num(f) != 0) {
	lua_pushnumber(L,pdf_font_num(f));
	lua_setfield(L,-2,"pdf_num");
  }
  if (pdf_font_blink(f) != 0) {
	lua_pushnumber(L,pdf_font_blink(f));
	lua_setfield(L,-2,"pdf_blink");
  }
  if (pdf_font_elink(f) != 0) {
	lua_pushnumber(L,pdf_font_elink(f));
	lua_setfield(L,-2,"pdf_elink");
  }
  */
  /* the next one is read only */
  if (pdf_font_expand_ratio(f) != 0) {
	lua_pushnumber(L,pdf_font_expand_ratio(f));
	lua_setfield(L,-2,"expand_ratio");
  }
  if (pdf_font_shrink(f) != 0) {
	lua_pushnumber(L,pdf_font_shrink(f));
	lua_setfield(L,-2,"shrink");
  }
  if (pdf_font_stretch(f) != 0) {
	lua_pushnumber(L,pdf_font_stretch(f));
	lua_setfield(L,-2,"stretch");
  }
  if (pdf_font_step(f) != 0) {
	lua_pushnumber(L,pdf_font_step(f));
	lua_setfield(L,-2,"step");
  }
  if (pdf_font_auto_expand(f) != 0) {
	lua_pushboolean(L,pdf_font_auto_expand(f));
	lua_setfield(L,-2,"auto_expand");
  }
  if (pdf_font_attr(f) != 0) {
	lua_pushstring(L,makecstring(pdf_font_attr(f)));
	lua_setfield(L,-2,"attributes");
  }

  /* params */
  write_lua_parameters(L,f);
  
  /* chars */
  lua_createtable(L,font_tables[f]->charinfo_size,0); /* all characters */

  if (has_left_boundary(f)) {
    co = get_charinfo(f,left_boundarychar);
    font_char_to_lua(L,f,co);
    lua_setfield(L,-2,"left_boundary");
  }
  if (has_right_boundary(f)) {
    co = get_charinfo(f,right_boundarychar);
    font_char_to_lua(L,f,co);
    lua_setfield(L,-2,"right_boundary");
  }

  for (k=font_bc(f);k<=font_ec(f);k++) {
    if (char_exists(f,k)) {
      lua_pushnumber(L,k);
      co = get_charinfo(f,k);
      font_char_to_lua(L,f,co);
      lua_rawset(L,-3);
    }
  }
  lua_setfield(L,-2,"characters");
  return 1;
}

static int 
count_hash_items (lua_State *L, int name_index){
  int n = -1;
  lua_rawgeti(L,LUA_REGISTRYINDEX,name_index);
  lua_rawget(L,-2);
  if (!lua_isnil(L,-1)) {
    if (lua_istable(L,-1)) {
      n = 0;
      /* now find the number */
      lua_pushnil(L);  /* first key */
      while (lua_next(L, -2) != 0) {
	n++;
	lua_pop(L,1);
      }
    }
  }
  lua_pop(L,1);
  return n;
}

#define streq(a,b) (strcmp(a,b)==0)

#define append_packet(k) { cpackets[np++] = k; }

#define do_store_four(l) {                                                        \
    append_packet((l&0xFF000000)>>24);                                \
    append_packet((l&0x00FF0000)>>16);                                \
    append_packet((l&0x0000FF00)>>8);                                \
    append_packet((l&0x000000FF));  } 

/*
*/

#define lua_roundnumber(a,b) (int)floor((double)lua_tonumber(L,-1)+0.5)

static int
numeric_field (lua_State *L, char *name, int dflt) {
  int i = dflt;
  lua_pushstring(L,name);
  lua_rawget(L,-2);
  if (lua_isnumber(L,-1)) {        
    i = lua_roundnumber(L,-1);
  }
  lua_pop(L,1);
  return i;
}

static int
n_numeric_field (lua_State *L, int name_index, int dflt) {
  register int i = dflt;
  lua_rawgeti(L,LUA_REGISTRYINDEX, name_index); /* fetch the stringptr */
  lua_rawget(L,-2);
  if (lua_type(L,-1)==LUA_TNUMBER) {        
    i = lua_roundnumber(L,-1);
  }
  lua_pop(L,1);
  return i;
}


static int
enum_field (lua_State *L, char *name, int dflt, char **values) {
  int k;
  char *s;
  int i = dflt;
  lua_pushstring(L,name);
  lua_rawget(L,-2);
  if (lua_isnumber(L,-1)) {        
    i = lua_tonumber(L,-1);
  } else if (lua_isstring(L,-1)) {
    s = (char *)lua_tostring(L,-1);
    k = 0;
    while (values[k] != NULL) {
      if (strcmp(values[k],s) == 0) {
        i = k;
        break;
      }
      k++;
    }
  }
  lua_pop(L,1);
  return i;
}

static int
boolean_field (lua_State *L, char *name, int dflt) {
  int i = dflt;
  lua_pushstring(L,name);
  lua_rawget(L,-2);
  if (lua_isboolean(L,-1)) {        
    i = lua_toboolean(L,-1);
  }
  lua_pop(L,1);
  return i;
}

static int
n_boolean_field (lua_State *L, int name_index, int dflt) {
  int i = dflt;
  lua_rawgeti(L,LUA_REGISTRYINDEX, name_index); /* fetch the stringptr */
  lua_rawget(L,-2);
  if (lua_isboolean(L,-1)) {        
    i = lua_toboolean(L,-1);
  }
  lua_pop(L,1);
  return i;
}


static char *
string_field (lua_State *L, char *name, char *dflt) {
  char *i;
  lua_pushstring(L,name);
  lua_rawget(L,-2);
  if (lua_isstring(L,-1)) {        
    i = xstrdup(lua_tostring(L,-1));
  } else if (dflt==NULL) {
    i = NULL;
  } else {
    i = xstrdup(dflt);
  }
  lua_pop(L,1);
  return i;
}

static char *
n_string_field (lua_State *L, int name_index, char *dflt) {
  char *i;
  lua_rawgeti(L,LUA_REGISTRYINDEX, name_index); /* fetch the stringptr */
  lua_rawget(L,-2);
  if (lua_isstring(L,-1)) {        
    i = xstrdup(lua_tostring(L,-1));
  } else if (dflt==NULL) {
    i = NULL;
  } else {
    i = xstrdup(dflt);
  }
  lua_pop(L,1);
  return i;
}

#define init_luaS_index(a) do {					\
    lua_pushliteral(L,#a);					\
    luaS_##a##_ptr = (char *)lua_tostring(L,-1);		\
    luaS_##a##_index = luaL_ref (L,LUA_REGISTRYINDEX);		\
  } while (0)

#define make_luaS_index(a)			\
  static int luaS_##a##_index = 0;		\
  static char * luaS_##a##_ptr = NULL

#define luaS_index(a)	luaS_##a##_index

#define luaS_ptr_eq(a,b) (a==luaS_##b##_ptr)

make_luaS_index(width);
make_luaS_index(height);
make_luaS_index(depth);
make_luaS_index(italic);
make_luaS_index(index);
make_luaS_index(left_protruding);
make_luaS_index(right_protruding);
make_luaS_index(expansion_factor);
make_luaS_index(top); 
make_luaS_index(bot); 
make_luaS_index(rep); 
make_luaS_index(mid);
make_luaS_index(next);
make_luaS_index(used);
make_luaS_index(name);
make_luaS_index(tounicode);
make_luaS_index(font);
make_luaS_index(char);
make_luaS_index(slot);
make_luaS_index(comment);
make_luaS_index(push);
make_luaS_index(pop);
make_luaS_index(rule);
make_luaS_index(right);
make_luaS_index(node);
make_luaS_index(down);
make_luaS_index(special);
make_luaS_index(slant);
make_luaS_index(space);
make_luaS_index(space_stretch);
make_luaS_index(space_shrink);
make_luaS_index(x_height);
make_luaS_index(quad);
make_luaS_index(extra_space);
make_luaS_index(left_boundary);
make_luaS_index(right_boundary);
make_luaS_index(kerns);
make_luaS_index(ligatures);
make_luaS_index(fonts);

void init_font_string_pointers (lua_State *L) {
  init_luaS_index(width);
  init_luaS_index(height);
  init_luaS_index(depth);
  init_luaS_index(italic);
  init_luaS_index(index);
  init_luaS_index(left_protruding);
  init_luaS_index(right_protruding);
  init_luaS_index(expansion_factor);
  init_luaS_index(top); 
  init_luaS_index(bot); 
  init_luaS_index(rep); 
  init_luaS_index(mid);
  init_luaS_index(next);
  init_luaS_index(used);
  init_luaS_index(name);
  init_luaS_index(tounicode);
  init_luaS_index(font);
  init_luaS_index(char);
  init_luaS_index(slot);
  init_luaS_index(comment);
  init_luaS_index(push);
  init_luaS_index(pop);
  init_luaS_index(rule);
  init_luaS_index(right);
  init_luaS_index(node);
  init_luaS_index(down);
  init_luaS_index(special);

  init_luaS_index(slant);
  init_luaS_index(space);
  init_luaS_index(space_stretch);
  init_luaS_index(space_shrink);
  init_luaS_index(x_height);
  init_luaS_index(quad);
  init_luaS_index(extra_space);

  init_luaS_index(left_boundary);
  init_luaS_index(right_boundary);
  init_luaS_index(kerns);
  init_luaS_index(ligatures);
  init_luaS_index(fonts);
}

static int
count_char_packet_bytes  (lua_State *L) {
  register int i; 
  register int l = 0;
  int ff = 0;
  for (i=1;i<=lua_objlen(L,-1);i++) {
    lua_rawgeti(L,-1,i);
    if (lua_istable(L,-1)) {
      lua_rawgeti(L,-1,1);
      if (lua_isstring(L,-1)) {
	char *s = (char *)lua_tostring(L,-1);
        if      (luaS_ptr_eq(s,font))    { l+= 5; ff =1; }
        else if (luaS_ptr_eq(s,char))    { if (ff==0) { l+=5;  } l += 5; ff = 1;        } 
        else if (luaS_ptr_eq(s,slot))    { l += 10; ff = 1;}
        else if (luaS_ptr_eq(s,comment)) { ;    } 
        else if (luaS_ptr_eq(s,push) || 
		 luaS_ptr_eq(s,pop))     { l++;  } 
        else if (luaS_ptr_eq(s,rule))    { l+=9; }
        else if (luaS_ptr_eq(s,right) ||
		 luaS_ptr_eq(s,node) ||
		 luaS_ptr_eq(s,down))    { l+=5; }
        else if (luaS_ptr_eq(s,special)) { 
	  size_t len;
          lua_rawgeti(L,-2,2);
          (void)lua_tolstring(L,-1,&len); 
          lua_pop(L,1);
          if (len>0) { l = l + 5 + len;  } 
        }
        else { fprintf(stdout,"unknown packet command %s!\n",s); }
      } else {
        fprintf(stdout,"no packet command!\n");
      }
      lua_pop(L,1); /* command name */
    }
    lua_pop(L,1); /* item */
  }
  return l;
}



scaled
sp_to_dvi (halfword sp, halfword atsize) {
  double result, mult;
  mult = (double)(atsize>>16);
  result = (sp << 4);
  return floor (result/mult );
}


static void
read_char_packets  (lua_State *L, integer *l_fonts, charinfo *co, int atsize) {
  int i, n, m;
  size_t l;
  int cmd;
  char *s;
  real_eight_bits *cpackets;
  int ff = 0;
  int np = 0;
  int max_f = 0;
  int pc = count_char_packet_bytes  (L);
  if (pc<=0)
    return;
  assert(l_fonts != NULL);
  assert(l_fonts[1] != 0);
  while (l_fonts[(max_f+1)]!=0) 
    max_f++;

  cpackets = xmalloc(pc+1);
  for (i=1;i<=lua_objlen(L,-1);i++) {
    lua_rawgeti(L,-1,i);
    if (lua_istable(L,-1)) {
      /* fetch the command code */
      lua_rawgeti(L,-1,1);
      if (lua_isstring(L,-1)) {
	s = (char *)lua_tostring(L,-1);
	cmd = 0;
	if (luaS_ptr_eq(s,font)) {  
	  cmd = packet_font_code;     
	} else if (luaS_ptr_eq(s,char)) { 
	  cmd = packet_char_code;     
	  if (ff==0) {
	    append_packet(packet_font_code);
	    ff = l_fonts[1];
	    do_store_four(ff);
	  }
	} else if (luaS_ptr_eq(s,slot)) { 
	  cmd = packet_nop_code;
	  lua_rawgeti(L,-2,2);  n = lua_tointeger(L,-1);
	  ff = (n>max_f ? l_fonts[1] : l_fonts[n]);
	  lua_rawgeti(L,-3,3);  n = lua_tointeger(L,-1);
	  lua_pop(L,2);
	  append_packet(packet_font_code);
	  do_store_four(ff);
	  append_packet(packet_char_code);
	  do_store_four(n);
	} 
	else if (luaS_ptr_eq(s,comment)) {  cmd = packet_nop_code;     } 
	else if (luaS_ptr_eq(s,node))    {  cmd = packet_node_code;    }
	else if (luaS_ptr_eq(s,push))    {  cmd = packet_push_code;    } 
	else if (luaS_ptr_eq(s,pop))     {  cmd = packet_pop_code;     } 
	else if (luaS_ptr_eq(s,rule))    {  cmd = packet_rule_code;    }
	else if (luaS_ptr_eq(s,right))   {  cmd = packet_right_code;   }
	else if (luaS_ptr_eq(s,down))    {  cmd = packet_down_code;    }
	else if (luaS_ptr_eq(s,special)) {  cmd = packet_special_code; } 
	
        switch(cmd) {
        case packet_push_code:
        case packet_pop_code:
          append_packet(cmd);
          break;
        case packet_font_code:
          append_packet(cmd);
          lua_rawgeti(L,-2,2);
          n = lua_tointeger(L,-1);
          ff = (n>max_f ? l_fonts[1] : l_fonts[n]);
          do_store_four(ff);
          lua_pop(L,1);
          break;
        case packet_node_code:
          append_packet(cmd);
          lua_rawgeti(L,-2,2);
          n = copy_node_list(nodelist_from_lua(L));
          do_store_four(n);
          lua_pop(L,1);
          break;
        case packet_char_code:
          append_packet(cmd);
          lua_rawgeti(L,-2,2);
          n = lua_tointeger(L,-1);
          do_store_four(n);
          lua_pop(L,1);
          break;
        case packet_right_code:
        case packet_down_code:
          append_packet(cmd);
          lua_rawgeti(L,-2,2);
          n = lua_tointeger(L,-1);
          do_store_four(sp_to_dvi(n,atsize));
          lua_pop(L,1);
          break;
        case packet_rule_code:
          append_packet(cmd);
          lua_rawgeti(L,-2,2);
          n = lua_tointeger(L,-1);
          do_store_four(sp_to_dvi(n,atsize));
          lua_rawgeti(L,-3,3);
          n = lua_tointeger(L,-1);
          do_store_four(sp_to_dvi(n,atsize));
          lua_pop(L,2);
          break;
        case packet_special_code:
          append_packet(cmd);
          lua_rawgeti(L,-2,2);
          s = (char *)lua_tolstring(L,-1,&l);
          if (l>0) {
            do_store_four(l);
            m = (int)l;
            while(m>0) {
              n = *s++;
	      m--;
              append_packet(n);
            }
          }
          lua_pop(L,1);
          break;
        case packet_nop_code:
          break;
        default:
          fprintf(stdout,"Unknown char packet code %s (char %d in font %s)\n",s,(int)c,font_name(f));
        }
      }
      lua_pop(L,1); /* command code */
    } else {
      fprintf(stdout,"Found a `commands' item that is not a table (char %d in font %s)\n",(int)c,font_name(f));
    }
    lua_pop(L,1); /* command table */
  }
  append_packet(packet_end_code);
  set_charinfo_packets(co,cpackets);
  return;
}


static void
read_lua_cidinfo (lua_State *L, int f) {
  int i;
  char *s;
  lua_getfield(L,-1,"cidinfo");
  if (lua_istable(L,-1)) {        
    i = numeric_field(L,"version",0);
    set_font_cidversion(f,i);    
    i = numeric_field(L,"supplement",0);
    set_font_cidsupplement(f,i);
    s = string_field(L,"registry","Adobe"); /* Adobe-Identity-0 */
    set_font_cidregistry(f,s);
    s = string_field(L,"ordering","Identity");
    set_font_cidordering(f,s);
  }
  lua_pop(L,1);
}


static void
read_lua_parameters (lua_State *L, int f) {
  int i, n;
  char *s;
  lua_getfield(L,-1,"parameters");
  if (lua_istable(L,-1)) {        
    /* the number of parameters is the max(IntegerKeys(L)),7) */
    n = 7;
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
      if (lua_isnumber(L,-2)) {
        i = lua_tonumber(L,-2);
        if (i > n)  n = i;
      }
      lua_pop(L,1); /* pop value */
    }
    if (n>7) set_font_params(f,n);
    /* sometimes it is handy to have all integer keys */
    for (i=1;i<=7;i++) {
      lua_rawgeti(L,-1,i);
      if (lua_isnumber(L,-1)) {
        n = lua_roundnumber(L,-1);
        set_font_param(f,i, n);
      }
      lua_pop(L,1); 
    }
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
      if (lua_isnumber(L,-2)) {
        i = lua_tointeger(L,-2);
        if (i>=8) {
          n = (lua_isnumber(L,-1) ? lua_roundnumber(L,-1) : 0);
          set_font_param(f,i, n);
        }
      } else if (lua_isstring(L,-2)) {
        s = (char *)lua_tostring(L,-2);
        n = (lua_isnumber(L,-1) ? lua_roundnumber(L,-1) : 0);
        if       (luaS_ptr_eq(s,slant))         {  set_font_param(f,slant_code,n); }
        else if  (luaS_ptr_eq(s,space))         {  set_font_param(f,space_code,n); }
        else if  (luaS_ptr_eq(s,space_stretch)) {  set_font_param(f,space_stretch_code,n); }
        else if  (luaS_ptr_eq(s,space_shrink))  {  set_font_param(f,space_shrink_code,n); }
        else if  (luaS_ptr_eq(s,x_height))      {  set_font_param(f,x_height_code,n); }
        else if  (luaS_ptr_eq(s,quad))          {  set_font_param(f,quad_code,n); }
        else if  (luaS_ptr_eq(s,extra_space))   {  set_font_param(f,extra_space_code,n); }
      }
      lua_pop(L,1); 
    }
  }
  lua_pop(L,1);

}
 
void
font_char_from_lua (lua_State *L, internal_font_number f, integer i, integer *l_fonts) {
  int k,r,t;
  charinfo *co;
  kerninfo *ckerns;
  liginfo *cligs;
  scaled j;
  char *s;
  int nl = 0; /* number of ligature table items */
  int nk = 0; /* number of kern table items */
  int ctr = 0;
  int atsize = font_size(f);
  if (lua_istable(L,-1)) {
    co = get_charinfo(f,i); 
    set_charinfo_tag       (co,0);
    j = n_numeric_field(L,luaS_width_index,0);        set_charinfo_width (co,j);
    j = n_numeric_field(L,luaS_height_index,0);       set_charinfo_height (co,j);
    j = n_numeric_field(L,luaS_depth_index,0);        set_charinfo_depth (co,j);
    j = n_numeric_field(L,luaS_italic_index,0);       set_charinfo_italic (co,j);              
    j = n_numeric_field(L,luaS_index_index,0);        set_charinfo_index(co,j);
    j = n_numeric_field(L,luaS_expansion_factor_index,0);  set_charinfo_ef(co,j);
    j = n_numeric_field(L,luaS_left_protruding_index,0);   set_charinfo_lp(co,j);
    j = n_numeric_field(L,luaS_right_protruding_index,0);  set_charinfo_rp(co,j);
    k = n_boolean_field(L,luaS_used_index,0);         set_charinfo_used(co,k);
    s = n_string_field (L,luaS_name_index,NULL);      set_charinfo_name(co,s);
    s = n_string_field (L,luaS_tounicode_index,NULL); set_charinfo_tounicode(co,s);
    k = n_numeric_field(L,luaS_next_index,-1); 
    if (k>=0) {
      set_charinfo_tag       (co,list_tag);
      set_charinfo_remainder (co,k);
    }
    lua_getfield(L,-1,"extensible");
    if (lua_istable(L,-1)){ 
      int top, bot,mid, rep;
      top = n_numeric_field(L,luaS_top_index,0);
      bot = n_numeric_field(L,luaS_bot_index,0);
      mid = n_numeric_field(L,luaS_mid_index,0);
      rep = n_numeric_field(L,luaS_rep_index,0);
      if (top != 0 || bot != 0 || mid != 0 || rep != 0) {
        set_charinfo_tag       (co,ext_tag);
        set_charinfo_extensible (co,top,bot,mid,rep);
      } else {
        pdftex_warn("lua-loaded font %s char [%d] has an invalid extensible field!",font_name(f),(int)i);
      }
    }
    lua_pop(L,1);
      
    nk = count_hash_items(L,luaS_index(kerns));
    if (nk>0) {
      ckerns = xcalloc((nk+1),sizeof(kerninfo));
      lua_rawgeti(L,LUA_REGISTRYINDEX,luaS_index(kerns));
      lua_rawget(L,-2);
      if (lua_istable(L,-1)) {  /* there are kerns */
        ctr = 0;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
          k = non_boundarychar;
          if (lua_isnumber(L,-2)) {
            k = lua_tonumber(L,-2); /* adjacent char */
            if (k<0)
              k = non_boundarychar;
          } else if (lua_isstring(L,-2)) {
            s = (char *)lua_tostring(L,-2);
            if (luaS_ptr_eq(s,right_boundary)) {
              k = right_boundarychar;
              if (!has_right_boundary(f))
                set_right_boundary(f,get_charinfo(f,right_boundarychar));
            }
          }
          j = lua_roundnumber(L,-1); /* movement */
          if (k!=non_boundarychar) {
            set_kern_item(ckerns[ctr],k,j);
            ctr++;
          } else {
            pdftex_warn("lua-loaded font %s char [%d] has an invalid kern field!",font_name(f),(int)i);
          }
          lua_pop(L,1);
        }
        /* guard against empty tables */
        if (ctr>0) {
          set_kern_item(ckerns[ctr],end_kern,0);
          set_charinfo_kerns(co,ckerns);
        } else {
          pdftex_warn("lua-loaded font %s char [%d] has an invalid kerns field!",font_name(f),(int)i);
        }
      }
      lua_pop(L,1);
    }
      
    /* packet commands */
    lua_getfield(L,-1,"commands");
    if (lua_istable(L,-1)){ 
      lua_pushnil(L);  /* first key */
      if (lua_next(L, -2) != 0) {
	lua_pop(L,2);
	read_char_packets(L,(integer *)l_fonts,co,atsize);
      }
    }
    lua_pop(L,1);

    /* ligatures */
    nl = count_hash_items(L,luaS_index(ligatures));
      
    if (nl>0) {
      cligs = xcalloc((nl+1),sizeof(liginfo));
      lua_rawgeti(L,LUA_REGISTRYINDEX,luaS_index(ligatures));
      lua_rawget(L,-2);
      if (lua_istable(L,-1)){/* do ligs */
        ctr = 0;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
          k = non_boundarychar;
          if (lua_isnumber(L,-2)) {
            k = lua_tonumber(L,-2); /* adjacent char */
            if (k<0) {
              k = non_boundarychar;
            }
          } else if (lua_isstring(L,-2)) {
            s = (char *)lua_tostring(L,-2);
            if (luaS_ptr_eq(s,right_boundary)) {
              k = right_boundarychar;
              if (!has_right_boundary(f))
                set_right_boundary(f,get_charinfo(f,right_boundarychar));
            }
          }
          r = -1;
          if (lua_istable(L,-1)) { 
            r = n_numeric_field(L,luaS_char_index,-1); /* ligature */
          }
          if (r != -1 && k != non_boundarychar) {                
            t = enum_field(L,"type",0,ligature_type_strings);
            set_ligature_item(cligs[ctr],(t*2)+1,k,r);
            ctr++;
          } else {
            pdftex_warn("lua-loaded font %s char [%d] has an invalid ligature field!",font_name(f),(int)i);
          }
          lua_pop(L,1); /* iterator value */
        }                  
        /* guard against empty tables */
        if (ctr>0) {
          set_ligature_item(cligs[ctr],0,end_ligature,0);
          set_charinfo_ligatures(co,cligs);
        } else {
          pdftex_warn("lua-loaded font %s char [%d] has an invalid ligatures field!",font_name(f),(int)i);
        }
      }
      lua_pop(L,1); /* ligatures table */
    }
  }
}



/* The caller has to fix the state of the lua stack when there is an error! */


int
font_from_lua (lua_State *L, int f) {
  int i,n,r,t;
  int s_top; /* lua stack top */
  int bc; /* first char index */
  int ec; /* last char index */
  char *s;
  integer *l_fonts = NULL;
  /* the table is at stack index -1 */

  if (luaS_width_index==0)
    init_font_string_pointers(L);

  s = string_field(L,"area","");                 set_font_area(f,s);
  s = string_field(L,"filename",NULL);           set_font_filename(f,s);
  s = string_field(L,"encodingname",NULL);       set_font_encodingname(f,s);

  s = string_field(L,"name",NULL);               set_font_name(f,s);
  s = string_field(L,"fullname",font_name(f));   set_font_fullname(f,s);

  if (s==NULL) {
    pdftex_fail("lua-loaded font [%d] has no name!",f);
    return false;
  }

  i = numeric_field(L,"designsize",655360);      set_font_dsize(f,i);
  i = numeric_field(L,"size",font_dsize(f));     set_font_size(f,i);
  i = numeric_field(L,"checksum",0);             set_font_checksum(f,i);
  i = numeric_field(L,"direction",0);            set_font_natural_dir(f,i);
  i = numeric_field(L,"encodingbytes",0);        set_font_encodingbytes(f,i);
  i = numeric_field(L,"tounicode",0);            set_font_tounicode(f,i);

  i = numeric_field(L,"extend",0);
  if (i<-2000) i = -2000; if (i>2000) i = 2000; if (i==1000) i = 0;
  set_font_extend(f,i);
  i = numeric_field(L,"slant",0);
  if (i<-1000) i = -1000; if (i>1000) i = 1000;
  set_font_slant(f,i);

  i = numeric_field(L,"hyphenchar",get_default_hyphen_char()); set_hyphen_char(f,i);
  i = numeric_field(L,"skewchar",get_default_skew_char());     set_skew_char(f,i);
  i = boolean_field(L,"used",0);                 set_font_used(f,i);

  s = string_field (L,"attributes",NULL);          
  if (s!=NULL && strlen(s)>0) {
	i = maketexstring(s);
    set_pdf_font_attr(f,i);
  }

  i = enum_field(L,"type",     unknown_font_type,font_type_strings);      set_font_type(f,i);
  i = enum_field(L,"format",   unknown_format,   font_format_strings);    set_font_format(f,i);
  i = enum_field(L,"embedding",unknown_embedding,font_embedding_strings); set_font_embedding(f,i);
  if (font_encodingbytes(f)==0 && 
      (font_format(f)==opentype_format || font_format(f)==truetype_format)) {
    set_font_encodingbytes(f,2);
  }

  /* now fetch the base fonts, if needed */
  n = count_hash_items(L,luaS_index(fonts));
  if (n>0) {
    l_fonts = xmalloc((n+2)*sizeof(integer));
    memset (l_fonts,0,(n+2)*sizeof(integer));
    lua_rawgeti(L,LUA_REGISTRYINDEX,luaS_index(fonts));
    lua_rawget(L,-2);
    for (i=1;i<=n;i++) {
      lua_rawgeti(L,-1,i);
      if (lua_istable(L,-1)) {
	lua_getfield(L,-1,"id");
	if (lua_isnumber(L,-1)) {
	  l_fonts[i] = lua_tonumber(L,-1);
	  lua_pop(L,2); /* pop id  and entry */
	  continue; 
	}
	lua_pop(L,1); /* pop id */
      };
      s = NULL;
      if (lua_istable(L,-1)) {
	lua_getfield(L,-1,"name");
	if (lua_isstring(L,-1)) {
	  s = (char *)lua_tostring(L,-1);
	}
	lua_pop(L,1); /* pop name */
      }
      if (s!= NULL) {
	lua_getfield(L,-1,"size");
	t = (lua_isnumber(L,-1) ? lua_roundnumber(L,-1) : -1000);
	lua_pop(L,1);
        
	/* TODO: the stack is messed up, otherwise this 
	 * explicit resizing would not be needed 
	 */
	s_top = lua_gettop(L);
	l_fonts[i] = find_font_id(s,"",t);
	lua_settop(L,s_top);
      } else {
	pdftex_fail("Invalid local font in font %s!\n", font_name(f));
      }
      lua_pop(L,1); /* pop list entry */
    }
    lua_pop(L,1); /* pop list entry */
  } else {
    if(font_type(f) == virtual_font_type) {
      pdftex_fail("Invalid local fonts in font %s!\n", font_name(f));
    } else {
      l_fonts = xmalloc(3*sizeof(integer));
      l_fonts[0] = 0;
      l_fonts[1] = f;
      l_fonts[2] = 0;
    }
  }

  /* parameters */
  read_lua_parameters(L,f);
  read_lua_cidinfo(L,f);

  /* characters */
  lua_getfield(L,-1,"characters");
  if (lua_istable(L,-1)) {        
    /* find the array size values */
    ec = 0; bc = -1; 
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
      if (lua_isnumber(L,-2)) {
        i = lua_tointeger(L,-2);
        if (i>=0) {
          if (lua_istable(L,-1)) {
            if (i>ec) ec = i;
            if (bc<0) bc = i;
            if (bc>=0 && i<bc) bc = i;
          }
        }
      }
      lua_pop(L, 1);
    }

    if (bc != -1) {
	  /* fprintf(stdout,"defined a font at %d with %d-%d\n",f,bc,ec); */
      set_font_bc(f,bc);
      set_font_ec(f,ec);
      lua_pushnil(L);  /* first key */
      while (lua_next(L, -2) != 0) {
        if (lua_isnumber(L,-2)) {
          i = lua_tonumber(L,-2);
          if (i>=0) {
            font_char_from_lua(L,f, i, l_fonts);
          }
        } else if (lua_isstring(L,-2)) {
          s = (char *)lua_tostring(L,-2);
          if (luaS_ptr_eq(s,left_boundary)) {
            font_char_from_lua(L,f, left_boundarychar, l_fonts);
          } else if (luaS_ptr_eq(s,right_boundary)) {
            font_char_from_lua(L,f, right_boundarychar, l_fonts);
          }
        }
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
      
	  /* handle font expansion last: the |copy_font| routine is called eventually, 
		 and that needs to know |bc| and |ec|. */
	  if (font_type(f)!=virtual_font_type) {
		int fstep = numeric_field(L,"step",0);
		if (fstep<0) fstep = 0; 
		if (fstep>100) fstep = 100;
		if (fstep!=0) {
		  int fshrink = numeric_field(L,"shrink",0);
		  int fstretch = numeric_field(L,"stretch",0);
		  int fexpand = boolean_field(L,"auto_expand",0);
		  if (fshrink<0)     fshrink = 0; 
		  if (fshrink>500)   fshrink = 500;
		  fshrink -=         (fshrink % fstep);
		  if (fshrink<0)     fshrink = 0; 
		  if (fstretch<0)    fstretch = 0; 
		  if (fstretch>1000) fstretch = 1000;
		  fstretch -=        (fstretch % fstep);
		  if (fstretch<0)    fstretch = 0; 
		  set_expand_params(f, fexpand, fstretch, fshrink, fstep, 0);
		}
	  }

    } else { /* jikes, no characters */
      pdftex_warn("lua-loaded font [%d] (%s) has no characters!",f, font_name(f));
    }

#if SAVE_REF
    r = luaL_ref(Luas[0],LUA_REGISTRYINDEX);    /* pops the table */
    set_font_cache_id(f,r);
#else
    lua_pop(Luas[0],1);
#endif
  } else { /* jikes, no characters */
    pdftex_warn("lua-loaded font [%d] (%s) has no character table!",f, font_name(f));
  }

  if (l_fonts!=NULL) 
    free(l_fonts);
  return true;
}

/* ====================================================================
 *
 * L I G A T U R I N G
 *
 * ==================================================================== */


#define assert_disc(a) \
  assert(pre_break(a)!=null); /* expect head_node */ \
  assert(type(pre_break(a))==nesting_node);				\
  assert((vlink_pre_break(a)==null && tlink_pre_break(a)==null) || tail_of_list(vlink_pre_break(a))==tlink_pre_break(a)); \
  assert(post_break(a)!=null); /* expect head_node */ \
  assert(type(post_break(a))==nesting_node);					  \
  assert((vlink_post_break(a)==null && tlink_post_break(a)==null) || tail_of_list(vlink_post_break(a))==tlink_post_break(a)); \
  assert(no_break(a)!=null); /* expect head_node */	  \
  assert(type(no_break(a))==nesting_node);					  \
  assert((vlink_no_break(a)==null && tlink_no_break(a)==null) || tail_of_list(vlink_no_break(a))==tlink_no_break(a)); 

static void 
nesting_append (halfword nest, halfword newn) {
  halfword tail = tlink(nest);
  assert(alink(nest)==null);
  assert(vlink(newn)==null);
  assert(alink(newn)==null);
  if (tail==null) {
    assert(vlink(nest)==null);
    couple_nodes(nest,newn);
  } else {
    assert(vlink(tail)==null);
    assert(tail_of_list(vlink(nest))==tail);
    couple_nodes(tail,newn);
  }
  tlink(nest) = newn;
}


static void 
nesting_prepend (halfword nest, halfword newn) {
  halfword head = vlink(nest);
  assert(alink(nest)==null);
  assert(vlink(newn)==null);
  assert(alink(newn)==null);
  couple_nodes(nest,newn);
  if (head==null) {
    assert(tlink(nest)==null);
    tlink(nest) = newn;
  } else {
    assert(alink(head)==nest);
    assert(tail_of_list(head)==tlink(nest));
    couple_nodes(newn,head);
  }
}


static int
test_ligature( liginfo *lig, halfword left, halfword right ) {
  if (type(left)!=glyph_node)
	return 0;
  assert(type(right)==glyph_node);
  if (font(left)!=font(right)) return 0;
  if (is_ghost(left) || is_ghost(right)) return 0;
  *lig = get_ligature(font(left),character(left),character(right));
  if (is_valid_ligature(*lig)) {
	return 1;
  }
  return 0;
}


static int
try_ligature(halfword *frst, halfword fwd) {
  halfword cur = *frst;
  liginfo lig;
  if (test_ligature(&lig,cur,fwd)) {
    int move_after = (lig_type(lig) & 0x0C)>>2;
    int keep_right = ((lig_type(lig) & 0x01) != 0);
    int keep_left  = ((lig_type(lig) & 0x02) != 0);
    halfword newgl = raw_glyph_node();
	font(newgl)      = font(cur);
	character(newgl) = lig_replacement(lig);
    set_is_ligature(newgl);

    /* below might not be correct in contrived border case.
     * but we use it only for debugging, so ... */ 
    if (character(cur)<0) {
      set_is_leftboundary(newgl);
	} 
	if (character(fwd)<0) {
      set_is_rightboundary(newgl);
	} 
	if (character(cur)<0) {
	  if (character(fwd)<0) {
		build_attribute_list(newgl);
	  } else {
		add_node_attr_ref(node_attr(fwd));
		node_attr(newgl) = node_attr(fwd);
	  }
	} else {
		add_node_attr_ref(node_attr(cur));
      node_attr(newgl) = node_attr(cur);
	}

    /* TODO/FIXME if this ligature is consists of another ligature
     * we should add it's lig_ptr to the new glyphs lig_ptr (and
     * cleanup the no longer needed node) LOW PRIORITY */
    /* left side */
    if (keep_left) {
      halfword new_first = copy_node(cur);
      lig_ptr(newgl) = new_first;
      couple_nodes(cur,newgl);
      if (move_after) {
        move_after--;
        cur = newgl;
      }
    } else {
      halfword prev = alink(cur);
      uncouple_node(cur);
      lig_ptr(newgl) = cur;
      assert(prev!=null);
      couple_nodes(prev,newgl);
      cur = newgl; /* as cur has disappeared */
    }
    /* right side */
    if (keep_right) {
      halfword new_second = copy_node(fwd);
      /* correct, because we _know_ lig_ptr points to _one_ node */
      couple_nodes(lig_ptr(newgl),new_second);
      couple_nodes(newgl,fwd);
      if (move_after) {
        move_after--;
        cur = fwd;
      }
    } else {
      halfword next = vlink(fwd);
      uncouple_node(fwd);
      /* correct, because we _know_ lig_ptr points to _one_ node */
      couple_nodes(lig_ptr(newgl),fwd);
      if (next!=null) {couple_nodes(newgl,next);}
    }

    /* check and return */
    /* assert(move_after==0);*/
    *frst = cur;
    return 1;
  }
  return 0;
}


/* there shouldn't be any ligatures here - we only add them at the end of
 * xxx_break in a DISC-1 - DISC-2 situation and we stop processing DISC-1
 * (we continue with DISC-1's post_ and no_break */
static halfword
handle_lig_nest(halfword root, halfword cur) {
  if (cur==null) return root;
  while (vlink(cur)!=null) {
    halfword fwd = vlink(cur);
    if (type(cur)==glyph_node && type(fwd)==glyph_node &&
        font(cur)==font(fwd)  && try_ligature(&cur,fwd)) continue;
    cur  = vlink(cur);
    assert(vlink(alink(cur))==cur);
  }
  tlink(root) = cur;
  return root;
}


static halfword 
handle_lig_word(halfword cur) {
  halfword right=null;
      
  if (type(cur)==whatsit_node && subtype(cur)==cancel_boundary_node) {
    halfword prev= alink(cur);
    halfword fwd = vlink(cur);
/*  uncouple_node(cur); */ /* not needed, it is freed */
    flush_node(cur);
    if (fwd==null) {
      vlink(prev) = fwd;
      return prev;
    }
    couple_nodes(prev,fwd);
    if (type(fwd)!=glyph_node) return prev;
    cur = fwd;
  } else if (has_left_boundary(font(cur))) {
    halfword prev= alink(cur);
    halfword p  = new_glyph(font(cur),left_boundarychar);
    couple_nodes(prev,p);
    couple_nodes(p,cur);
    cur         = p;
  }
  if (has_right_boundary(font(cur))) {
    right = new_glyph(font(cur),right_boundarychar);
  }

  while (1) {
    /* A glyph followed by ... */
    if (type(cur)==glyph_node) {
      halfword fwd = vlink(cur);
      if (fwd==null) { /* last character of paragraph */
        if (right==null) break;
        couple_nodes(cur,right);
        right = null;
        continue;
      }
      assert(alink(fwd)==cur);
      if (type(fwd)==glyph_node) { /* GLYPH - GLYPH */
        if (font(cur)!=font(fwd)) break;
        if (try_ligature(&cur,fwd)) continue;
      } else if (type(fwd)==disc_node) { /* GLYPH - DISC */

        /* if  a{bx}{}{y} and a+b=>B convert to {Bx}{}{ay} */
        halfword pre = vlink_pre_break(fwd);
        halfword nob = vlink_no_break(fwd);
        liginfo lig;
        assert_disc(fwd);
        /* Check on: a{b?}{?}{?} and a+b=>B : {B?}{?}{a?}*/
        /* Check on: a{?}{?}{b?} and a+b=>B : {a?}{?}{B?} */
        if ( (pre!=null && type(pre)==glyph_node && test_ligature(&lig,cur,pre))
          || (nob!=null && type(nob)==glyph_node && test_ligature(&lig,cur,nob))) {
          /* move cur from before disc, to skipped part */
          halfword prev = alink(cur);
          assert(vlink(prev)==cur);
          uncouple_node(cur);
          couple_nodes(prev,fwd);
          nesting_prepend(no_break(fwd),cur);
          /* now ligature the pre_break */
          nesting_prepend(pre_break(fwd),copy_node(cur));
          /* As we have removed cur, we need to start again ... */
          cur = prev;
        } 
        /* Check on: a{?}{?}{}b and a+b=>B : {a?}{?b}{B}*/
        halfword next = vlink(fwd);
        if (nob==null && next != null && type(next)==glyph_node
          && test_ligature(&lig,cur,next)) {
          /* move cur from before disc to no_break part */
          halfword prev = alink(cur);
          assert(alink(next)==fwd);
          assert(vlink(prev)==cur);
          uncouple_node(cur);
          couple_nodes(prev,fwd);
          couple_nodes(no_break(fwd),cur); /* we _know_ it's empty */
          /* now copy cur the pre_break */
          nesting_prepend(pre_break(fwd),copy_node(cur));
          /* move next from after disc to no_break part */
          halfword tail = vlink(next);
          uncouple_node(next);
          try_couple_nodes(fwd,tail);
          couple_nodes(cur,next); /* we _know_ this works */
          tlink(no_break(fwd)) = next; /* and make sure the list is correct */
          /* now copy next to the post_break */
          nesting_append(post_break(fwd),copy_node(next));
          /* As we have removed cur, we need to start again ... */
          cur = prev;
        }
        /* we are finished with the pre_break */
        handle_lig_nest(pre_break(fwd),vlink_pre_break(fwd));
      } else if (type(fwd)==whatsit_node && subtype(fwd)==cancel_boundary_node) {
        halfword next = vlink(fwd);
        try_couple_nodes(cur,next);
        flush_node(fwd);
        if (right!=null) {
          flush_node(right); /* Shame, didn't need it */
	  /*        right = null; */ /* no need, we're going to leave the loop anyway */
        }
        break;
      } else { /* fwd is something unknown */
        if (right==null) break;
        couple_nodes(cur,right);
        couple_nodes(right,fwd);
        right = null;
        continue;
      }
    /* A discretionary followed by ... */
    } else if (type(cur)==disc_node) {
      assert_disc(cur);
      /* If {?}{x}{?} or {?}{?}{y} then ... */
      if (vlink_no_break(cur)!=null || vlink_post_break(cur)!=null) {
        halfword fwd;
        halfword lists[511]; /* max 8 levels */

        int i, max_depth=0;
        lists[max_depth++] = handle_lig_nest(post_break(cur),vlink_post_break(cur));
        lists[max_depth++] = handle_lig_nest(no_break(cur),vlink_no_break(cur));
        while (1) {
          if ((fwd = vlink(cur))==null) return cur;
          if ( type(fwd)==glyph_node) {
            for (i=0; i<max_depth; i++) {
              liginfo lig;
              halfword tail = tlink(lists[i]);
              if ( tail!=null && test_ligature(&lig,tail,fwd))
                goto add_glyph_to_all;
            }
            /* if we get here, nothing had a ligature, so we stop */
            break;
add_glyph_to_all:
            for (i=0; i<max_depth; i++) {
              halfword copy = copy_node(fwd);
              halfword tail = tlink(lists[i]);
              nesting_append(lists[i],copy);
              if (tail==null) continue; /* first character - never a ligature */
              handle_lig_nest(lists[i],tail);
            }
            halfword next = vlink(fwd);
            uncouple_node(fwd);
            try_couple_nodes(cur,next);
            flush_node(fwd);
          } else if ( type(fwd)==disc_node) {
            /* MAGIC WARNING
             * A disc followed by a disc can have different kernings
             * depending on which path is choosen, and it is impossible to
             * store the possible kernings: fe {}{A}{W} {V}{}{A}
             * So we _always_ add discs so only a simple path remains. */
            int m=max_depth; /* as max_depth changes in this loop */
            for (i=0; i<m; i++) {
              halfword copy = copy_node(fwd);
              halfword tail = tlink(lists[i]);
              if (tail!=null) {
                halfword prev = alink(tail);
				assert(alink(tail)!=null);
                uncouple_node(tail);
                vlink(prev) = null;
                if (prev==lists[i]) {
                  tlink(prev) = null;
                } else {
                  tlink(lists[i]) = prev;
                }
                nesting_prepend(pre_break(copy),tail);
                nesting_prepend(no_break(copy),copy_node(tail));
              }
              nesting_append(lists[i],copy);
              handle_lig_nest(pre_break(copy),vlink_pre_break(copy));
              assert(max_depth<256);
              lists[max_depth++] = handle_lig_nest(no_break(copy),vlink_no_break(copy));
              lists[i]           = handle_lig_nest(post_break(copy),vlink_post_break(copy));
            }
            halfword next = vlink(fwd);
            uncouple_node(fwd);
            try_couple_nodes(cur,next);
            flush_node(fwd);
          } else {
            return cur;
          }
        }
      }
    } else { /* NO GLYPH NOR DISC */
      /* fprintf(stdout,"This is a %d node\n",type(cur));*/
      /* assert(0);*/ /* TODO howcome there can be a glue here? */
      return cur;
    }
    /* step-to-next-node */
    {
      halfword prev = cur;
      cur  = vlink(cur);
      assert(cur!=null);
      assert(alink(cur)==prev);
      /*  alink(cur) = prev;*/
    }
  }

  return cur;
}

/* return = new tail, head should be a dummy */

halfword 
handle_ligaturing(halfword head, halfword tail) {
  halfword save_tail ; /* trick to allow explicit node==null tests */
  halfword cur, prev;

  if (vlink(head)==null)
    return tail;
  save_tail = vlink(tail);
  vlink(tail) = null;

  /* if (fix_node_lists) */
  fix_node_list(head);

  prev = head;
  cur = vlink(prev);

  while (cur!=null) {
    if ( type(cur)==glyph_node ||
        (type(cur)==whatsit_node && subtype(cur)==cancel_boundary_node)) {
      cur = handle_lig_word(cur);
    }
    prev = cur;
    cur = vlink(cur);
    assert(cur==null||alink(cur) == prev);
  }
  if (valid_node(save_tail)) {
    try_couple_nodes(prev,save_tail);
  }
  return prev;
}


/* ====================================================================
 *
 * K E R N I N G
 *
 * ==================================================================== */

static void
add_kern_before(halfword left, halfword right) {
  if ((!is_rightghost(right)) && 
      font(left)==font(right) && 
      has_kern(font(left),character(left))) {
    int k = get_kern(font(left),character(left),character(right));
    if (k!=0) {
      halfword kern = new_kern(k);
      halfword prev = alink(right);
      assert(vlink(prev)==right);
      couple_nodes(prev,kern);
      couple_nodes(kern,right);
    }
  }
}


static void
add_kern_after(halfword left, halfword right, halfword aft) {
  if ((!is_rightghost(right)) && 
      font(left)==font(right) && 
      has_kern(font(left),character(left))) {
    int k = get_kern(font(left),character(left),character(right));
    if (k!=0) {
      halfword kern = new_kern(k);
      halfword next = vlink(aft);
      assert(next==null||alink(next)==aft);
      couple_nodes(aft,kern);
      try_couple_nodes(kern,next);
    }
  }
}


static void
do_handle_kerning (halfword root, halfword init_left, halfword init_right) {
  halfword cur = vlink(root);
  halfword left = null;
  assert(init_left==null || type(init_left)==glyph_node);
  assert(init_right==null || type(init_right)==glyph_node);
  if (cur==null) {
    if (init_left!=null && init_right!=null) {
      add_kern_after(init_left,init_right,root);
      tlink(root)=vlink(root);
    }
    return;
  }
  if (type(cur)==glyph_node) {
    set_is_glyph(cur);
    if (init_left!=null) 
      add_kern_before(init_left,cur);
    left = cur;
  }
  while ((cur=vlink(cur))!=null) {
    if (type(cur)==glyph_node) {
      set_is_glyph(cur);
      if (left != null) { 
        add_kern_before(left,cur);
        if (character(left)<0 || is_ghost(left)) {
          halfword prev = alink(left);
          couple_nodes(prev,cur);
          flush_node(left);
        }
      }
      left = cur;
    } else {
      if (type(cur)==disc_node) {
        halfword right= type(vlink(cur))==glyph_node ? vlink(cur) : null;
        do_handle_kerning(pre_break(cur),left,null);
        do_handle_kerning(post_break(cur),null,right);
        do_handle_kerning(no_break(cur),left,right);
      }
      if (left != null) {
        if (character(left)<0 || is_ghost(left)) {
          halfword prev = alink(left);
          couple_nodes(prev,cur);
          flush_node(left);
        }
        left = null;
      }
    }
  }
  if (left!=null) {
    if (init_right!=null)
      add_kern_after(left,init_right,left);
    if (character(left)<0 || is_ghost(left)) {
      halfword prev = alink(left);
      halfword next = vlink(left);
      if (next!=null) {
        couple_nodes(prev,next);
        tlink(root) = next;
        assert(vlink(next)==null);
        assert(type(next)==kern_node);
      } else if (prev!=root) {
        vlink(prev) = null;
        tlink(root) = prev;
      } else {
        vlink(root) = null;
        tlink(root) = null;
      }
      flush_node(left);
    }
  }
}


halfword
handle_kerning (halfword head, halfword tail) {
  halfword save_link;
  save_link = vlink(tail);
  vlink(tail) = null;
  tlink(head) = tail;
  do_handle_kerning(head,null,null);
  tail = tlink(head);
  if (valid_node(save_link)) {
    try_couple_nodes(tail,save_link);  
  }
  return tail;
}

/* ====================================================================
 *
 * L I G A T U R I N G and K E R N I N G   :   L U A - I N T E R F A C E
 *
 * ==================================================================== */

static halfword
run_lua_ligkern_callback (halfword head, halfword tail, int callback_id){
  lua_State *L = Luas[0];
  lua_rawgeti(L,LUA_REGISTRYINDEX,callback_callbacks_id);
  lua_rawgeti(L,-1, callback_id);
  if (!lua_isfunction(L,-1)) {
    lua_pop(L,2);
    return tail;
  }
  nodelist_to_lua(L,head);
  nodelist_to_lua(L,tail);
  if (lua_pcall(L,2,1,0) != 0) {
    fprintf(stdout,"error: %s\n",lua_tostring(L,-1));
    lua_pop(L,2);
    lua_error(L);
    return tail;
  } 
  tail = nodelist_from_lua(L);
  if (fix_node_lists) 
    fix_node_list(head);
  lua_pop(L,2);
  return tail;
}


halfword 
new_ligkern(halfword head, halfword tail) {
  int callback_id = 0;

  assert(head!=null);
  if (vlink(head)==null)
    return tail;

  callback_id = callback_defined(ligaturing_callback);
  if (callback_id>0) {
    tail = run_lua_ligkern_callback(head,tail,callback_id);
    if (tail==null) tail = tail_of_list(head);
  } else {
    tail = handle_ligaturing(head,tail);
  }	
  
  callback_id = callback_defined(kerning_callback);
  if (callback_id>0) {
    tail = run_lua_ligkern_callback(head,tail,callback_id);
    if (tail==null) tail  = tail_of_list(head);
  } else {
    halfword nest = new_node(nesting_node,1);
    halfword cur  = vlink(head);
    halfword aft = vlink(tail);
    couple_nodes(nest,cur);
    tlink(nest)=tail;
    vlink(tail)=null;
    do_handle_kerning(nest,null,null);
    couple_nodes(head,vlink(nest));
    tail = tlink(nest);
    try_couple_nodes(tail,aft);
    flush_node(nest);
  }
  return tail;
}

