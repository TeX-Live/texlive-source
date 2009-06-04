/**
*  $Id $
*  @desc Support interface for fontforge 20070607
*  @version  1.0
*  @author Taco Hoekwater
*/

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <locale.h>

#include "pfaedit.h"
#include "ustring.h"

extern char **gww_errors;
extern int gww_error_count;
extern void gwwv_errors_free (void);
extern struct ui_interface luaui_interface;

extern int readbinfile(FILE *f, unsigned char **b, int *s);

#define FONT_METATABLE "fontloader.splinefont"

#define LUA_OTF_VERSION "0.3"

static char *possub_type_enum[] = { 
  "null", "position", "pair",  "substitution", 
  "alternate", "multiple", "ligature", "lcaret",  
  "kerning", "vkerning", "anchors", "contextpos", 
  "contextsub", "chainpos", "chainsub","reversesub", 
  "max", "kernback", "vkernback", NULL };

#define LAST_POSSUB_TYPE_ENUM 18

#define eight_nulls() NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL

static char *asm_type_enum[] = { 
  "indic", "context", "lig", NULL, "simple", "insert", NULL, NULL,
  eight_nulls(),
  NULL, "kern" };

static char *otf_lookup_type_enum[] = { 
  "gsub_start", "gsub_single", "gsub_multiple", "gsub_alternate", 
  "gsub_ligature", "gsub_context",  "gsub_contextchain", NULL, 
  "gsub_reversecontextchain", NULL, NULL, NULL,  NULL, NULL, NULL, NULL, /*0x00F */
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(), NULL, NULL, NULL, NULL,  NULL, "morx_indic", "morx_context", "morx_insert", /* 0x0FF*/
  "gpos_start", "gpos_single", "gpos_pair", "gpos_cursive", 
  "gpos_mark2base", "gpos_mark2ligature", "gpos_mark2mark",  "gpos_context", 
  "gpos_contextchain", NULL, NULL, NULL,  NULL, NULL, NULL, NULL, /* 0x10F */
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(),eight_nulls(),
  eight_nulls(), NULL, NULL, NULL, NULL,  NULL, NULL, NULL, "kern_statemachine", /* 0x1FF*/
};


static char *anchor_type_enum[] = { 
   "mark", "basechar", "baselig", "basemark", "centry", "cexit", "max", NULL };

#define MAX_ANCHOR_TYPE 7

static char *anchorclass_type_enum[] = { 
  "mark", "mkmk", "curs", "mklg", NULL };

static char *glyph_class_enum[] = { 
  "automatic", "none" ,"base", "ligature","mark", "component", NULL };

static char *ttfnames_enum[ttf_namemax] = { 
    "copyright", "family", "subfamily", "uniqueid",
    "fullname", "version", "postscriptname", "trademark",
    "manufacturer", "designer", "descriptor", "venderurl",
    "designerurl", "license", "licenseurl", "idontknow",
    "preffamilyname", "prefmodifiers", "compatfull", "sampletext",
    "cidfindfontname", "wwsfamily", "wwssubfamily" };

static char *fpossub_format_enum [] = { 
   "glyphs", "class","coverage","reversecoverage" , NULL};

static char *tex_type_enum[4] = { "unset", "text", "math", "mathext"};

/* has an offset of 1, ui_none = 0. */
static char *uni_interp_enum[9] = {
  "unset", "none", "adobe", "greek", "japanese",
  "trad_chinese", "simp_chinese", "korean", "ams" };
	
#define check_isfont(L,b) (SplineFont **)luaL_checkudata(L,b,FONT_METATABLE)

void handle_generic_pst (lua_State *L, struct generic_pst *pst);  /* forward */
void handle_generic_fpst (lua_State *L, struct generic_fpst *fpst);  /* forward */
void handle_generic_asm (lua_State *L, struct generic_asm *sm) ;
void handle_kernclass (lua_State *L, struct kernclass *kerns);
void handle_anchorclass (lua_State *L, struct anchorclass *anchor);
void handle_splinefont(lua_State *L, struct splinefont *sf) ;


void
lua_ff_pushfont(lua_State *L, SplineFont *sf) {
  SplineFont **a;
  if (sf==NULL) {
    lua_pushnil(L);
  } else {
	a = lua_newuserdata(L,sizeof(SplineFont *));
	*a = sf;
    luaL_getmetatable(L,FONT_METATABLE);
    lua_setmetatable(L,-2);
  }
  return;
}


static int 
ff_open (lua_State *L) {
  SplineFont *sf;
  const char *fontname;
  FILE *l;
  char s[511];
  size_t len;
  int args,i ;
  int openflags = 1;
  fontname = luaL_checkstring(L,1);
  /* test fontname for existance */
  if ((l = fopen(fontname,"r"))) {
	fclose(l); 
  } else {
	lua_pushfstring(L,"font loading failed for %s (read error)\n", fontname);
	lua_error(L);
  }
  args = lua_gettop(L);
  if (args>=2 && lua_isstring(L,2)) {
    if (*(fontname+strlen(fontname))!=')') {
      /* possibly fails for embedded parens in the font name */
      snprintf(s,511,"%s(%s)", fontname, lua_tolstring(L,2,&len));
      if (len==0) {
        snprintf(s,511,"%s", fontname);
      }
    }
  } else {
	snprintf(s,511,"%s", fontname);
  }
  if (strlen(s)>0) {
    gww_error_count=0;
	sf = ReadSplineFont((char *)s,openflags);
	if (sf==NULL) {
	  lua_pushfstring(L,"font loading failed for %s\n", s);
	  if (gww_error_count>0) {
		for (i=0;i<gww_error_count;i++) {
		  lua_pushstring(L,gww_errors[i]);
          lua_concat(L,2);
		}
		gwwv_errors_free();
      }
	  lua_error(L);
	} else {
	  FVAppend(_FontViewCreate(sf));
	  lua_ff_pushfont(L,sf);
	  if (gww_error_count>0) {
		lua_newtable(L);
		for (i=0;i<gww_error_count;i++) {
		  lua_pushstring(L,gww_errors[i]);
		  lua_rawseti(L,-2,(i+1));
		}
		gwwv_errors_free();
	  } else {
		lua_pushnil(L);
	  }
	}
  } else {
	lua_pushfstring(L,"font loading failed: empty string given\n", fontname);
	lua_error(L);
  }
  return 2;
}


static int 
ff_close (lua_State *L) {
  SplineFont **sf;
  /*fputs("ff_close called",stderr);*/
  sf = check_isfont(L,1);
  if (*sf!=NULL) {
    SplineFontFree(*sf);
    *sf = NULL;
  }
  return 0;
}

static int 
ff_apply_featurefile (lua_State *L) {
  SplineFont **sf;
  char *fname;
  sf = check_isfont(L,1);
  fname = (char *)luaL_checkstring(L,2);
  SFApplyFeatureFilename(*sf,fname);
  return 0;
}

static int 
ff_apply_afmfile (lua_State *L) {
  SplineFont **sf;
  char *fname;
  sf = check_isfont(L,1);
  fname = (char *)luaL_checkstring(L,2);
  CheckAfmOfPostscript(*sf,fname,(*sf)->map);

  return 0;
}



static void 
dump_intfield (lua_State *L, char *name, long int field) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushnumber(L,field);
  lua_rawset(L,-3);
}

static void 
dump_realfield (lua_State *L, char *name, real field) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushnumber(L,field);
  lua_rawset(L,-3);
}


#define dump_cond_intfield(a,b,c) if ((c)!=0) { dump_intfield ((a),(b),(c)); }


static void 
dump_stringfield (lua_State *L, char *name, char *field) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushstring(L,field);
  lua_rawset(L,-3);
}

static void 
dump_char_ref (lua_State *L, struct splinechar *spchar) {
  lua_checkstack(L,2);
  lua_pushstring(L,"char");
  lua_pushstring(L,spchar->name);
  lua_rawset(L,-3);
}


static void 
dump_lstringfield (lua_State *L, char *name, char *field, int len) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushlstring(L,field,len);
  lua_rawset(L,-3);
}

static void 
dump_enumfield (lua_State *L, char *name, int fid, char **fields) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushstring(L,fields[fid]);
  lua_rawset(L,-3);
}

static void 
dump_floatfield (lua_State *L, char *name, double field) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushnumber(L,field);
  lua_rawset(L,-3);
}

static char tag_string [5] = {0};

static char *make_tag_string (unsigned int field) {
  tag_string[0] = (field&0xFF000000) >> 24;
  tag_string[1] = (field&0x00FF0000) >> 16;
  tag_string[2] = (field&0x0000FF00) >> 8;
  tag_string[3] = (field&0x000000FF);
  return (char *)tag_string;
}

static char featbuf[32] = {0};

static char *make_mactag_string (unsigned int field) {
  sprintf( featbuf, "<%d,%d>", field>>16, field&0xffff );
  return (char *)featbuf;
}


static void 
dump_tag (lua_State *L, char *name, unsigned int field) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushlstring(L,make_tag_string(field),4);
  lua_rawset(L,-3);
}

static void 
dump_mactag (lua_State *L, char *name, unsigned int field) {
  lua_checkstack(L,2);
  lua_pushstring(L,name);
  lua_pushstring(L,make_mactag_string(field));
  lua_rawset(L,-3);
}

void
dump_subtable_name (lua_State *L, char *name, struct lookup_subtable *s) {
  /* this is likely a backref */ 
  if (s==NULL)
    return;
  lua_checkstack(L,2);
  if (s->next == NULL) {
    dump_stringfield(L,name,s->subtable_name);
  } else {
    /* can this really happen ? */
    int i = 0;
    lua_newtable(L);
    while (s!=NULL) {
      lua_pushstring(L, s->subtable_name);
      lua_rawseti(L, -2, ++i);
      s = s->next;
    }
    lua_setfield(L, -2, name);
  }
}



#define NESTED_TABLE(a,b,c) {                                           \
    int k = 1;                                                          \
    next = b;															\
    while (next != NULL) {                                              \
      lua_checkstack(L,2);												\
      lua_pushnumber(L,k); k++;                                         \
      lua_createtable(L,0,c);                                           \
      a(L, next);                                                       \
      lua_rawset(L,-3);                                                 \
      next = next->next;                                                \
    } }

void
do_handle_scriptlanglist (lua_State *L, struct scriptlanglist *sl) {
  int k;
  dump_tag(L,"script",          sl->script);

  lua_checkstack(L,3);
  lua_newtable(L);
  for (k=0;k<MAX_LANG;k++) {
    if (sl->langs[k] != 0) {
      lua_pushnumber(L,(k+1));
      lua_pushstring(L,make_tag_string(sl->langs[k]));
      lua_rawset(L,-3);
    }
  }

  if (sl->lang_cnt>=MAX_LANG) {
    for (k=MAX_LANG;k<sl->lang_cnt;k++) {
      lua_pushnumber(L,(k+1));
      lua_pushstring(L,make_tag_string(sl->morelangs[k-MAX_LANG]));
      lua_rawset(L,-3);
    }
  }
  lua_setfield(L,-2,"langs");
} 

void
handle_scriptlanglist (lua_State *L, struct scriptlanglist *sll) {
  struct scriptlanglist *next;
  NESTED_TABLE(do_handle_scriptlanglist,sll,4);
}

void
do_handle_featurescriptlanglist (lua_State *L, struct featurescriptlanglist *features) {
  if (features->ismac) {
	dump_mactag (L,"tag",features->featuretag);
  } else {
	dump_tag (L,"tag",features->featuretag);
  }
  lua_newtable(L);
  handle_scriptlanglist(L, features->scripts);
  lua_setfield(L,-2,"scripts");
  dump_cond_intfield (L,"ismac",features->ismac);
} 

void
handle_featurescriptlanglist (lua_State *L, struct featurescriptlanglist *features) {
  struct featurescriptlanglist *next;
  NESTED_TABLE(do_handle_featurescriptlanglist,features,3);
}

void 
do_handle_lookup_subtable (lua_State *L, struct lookup_subtable *subtable) {

  dump_stringfield(L,"name",                 subtable->subtable_name); 
  dump_stringfield(L,"suffix",               subtable->suffix); 

  /* struct otlookup *lookup; */ /* this is the parent */

  /* dump_intfield   (L,"unused",               subtable->unused); */
  /* The next one is true if there is no fpst, false otherwise */
  /*
     dump_intfield      (L,"per_glyph_pst_or_kern",subtable->per_glyph_pst_or_kern); 
  */
  dump_cond_intfield (L,"anchor_classes",       subtable->anchor_classes); 
  dump_cond_intfield (L,"vertical_kerning",     subtable->vertical_kerning); 

  if (subtable->kc != NULL) {
    lua_newtable(L);
    handle_kernclass(L, subtable->kc);
    lua_setfield(L,-2, "kernclass");
  }

#if 0
  if (subtable->fpst != NULL) {
    /* lua_newtable(L); */
    handle_generic_fpst(L, subtable->fpst);
    /* lua_setfield(L,-2, "fpst"); */
  }
#endif

  if (subtable->sm != NULL) {
    lua_newtable(L);
    handle_generic_asm(L, subtable->sm);
    lua_setfield(L,-2, "sm");
  }
  /* int subtable_offset; */ /* used by OTF file generation */
  /* int32 *extra_subtables; */ /* used by OTF file generation */
}

void
handle_lookup_subtable (lua_State *L, struct lookup_subtable *subtable) {
  struct lookup_subtable *next;
  NESTED_TABLE(do_handle_lookup_subtable,subtable,2); 
}

void 
do_handle_lookup (lua_State *L, struct otlookup *lookup ) {

  dump_enumfield     (L,"type",             lookup->lookup_type, otf_lookup_type_enum); 

  lua_newtable(L);
  if (lookup->lookup_flags & pst_r2l) {
    lua_pushstring(L,"r2l");  lua_pushboolean(L,1);   lua_rawset(L,-3);
  }
  if (lookup->lookup_flags & pst_ignorebaseglyphs) {
    lua_pushstring(L,"ignorebaseglyphs");  lua_pushboolean(L,1);   lua_rawset(L,-3);
  }
  if (lookup->lookup_flags & pst_ignoreligatures) {
    lua_pushstring(L,"ignoreligatures");  lua_pushboolean(L,1);   lua_rawset(L,-3);
  }
  if (lookup->lookup_flags & pst_ignorecombiningmarks) {
    lua_pushstring(L,"ignorecombiningmarks");  lua_pushboolean(L,1);   lua_rawset(L,-3);
  }
  lua_setfield(L,-2,"flags");


  dump_stringfield   (L,"name",             lookup->lookup_name); 

  if (lookup->features != NULL) {
    lua_newtable(L);
    handle_featurescriptlanglist(L,lookup->features); 
    lua_setfield(L,-2,"features");
  }

  if (lookup->subtables != NULL) {
    lua_newtable(L);
    handle_lookup_subtable(L,lookup->subtables);
    lua_setfield(L,-2,"subtables");
  }

  /* dump_intfield   (L,"unused",           lookup->unused);  */
  /* dump_intfield   (L,"empty",            lookup->empty); */
  /* dump_intfield   (L,"store_in_afm",     lookup->store_in_afm); */
  /* dump_intfield   (L,"needs_extension",  lookup->needs_extension); */
  /* dump_intfield   (L,"temporary_kern",   lookup->temporary_kern); */
  /* dump_intfield   (L,"def_lang_checked", lookup->def_lang_checked); */
  /* dump_intfield   (L,"def_lang_found",   lookup->def_lang_found); */
  /* dump_intfield   (L,"ticked",           lookup->ticked); */
  /* dump_intfield   (L,"subcnt",           lookup->subcnt); */
  /* dump_intfield   (L,"lookup_index",     lookup->lookup_index); */ /* identical to array index */
  /* dump_intfield   (L,"lookup_offset",    lookup->lookup_offset); */
  /* dump_intfield   (L,"lookup_length",    lookup->lookup_length); */
  /* dump_stringfield(L,"tempname",         lookup->tempname); */

}

void
handle_lookup (lua_State *L, struct otlookup *lookup ) {
  struct otlookup *next;
  NESTED_TABLE(do_handle_lookup,lookup,18); /* 18 is a guess */
}

void
do_handle_kernpair (lua_State *L, struct kernpair *kp) {

  if (kp->sc != NULL)
    dump_char_ref(L, kp->sc); 
  dump_intfield(L,"off",          kp->off);
  /*  uint16 kcid;   */ /* temporary value */
  dump_subtable_name(L, "lookup", kp->subtable);
}

void
handle_kernpair (lua_State *L, struct kernpair *kp) {
  struct kernpair *next;
  NESTED_TABLE(do_handle_kernpair,kp,4);
}

void
handle_splinecharlist (lua_State *L, struct splinecharlist *scl) {

  struct splinecharlist *next = scl;
  int k = 1;
  lua_checkstack(L,10);
  while( next != NULL) {
    if (next->sc != NULL) {
      lua_pushnumber(L,k); k++;
      lua_pushstring(L,next->sc->name);
      lua_rawset(L,-3);
    }
    next = next->next;
  }
}



/* vs is the "variation selector" a unicode codepoint which modifieds */
/*  the code point before it. If vs is -1 then unienc is just an */
/*  alternate encoding (greek Alpha and latin A), but if vs is one */
/*  of unicode's variation selectors then this glyph is somehow a */
/*  variant shape. The specifics depend on the selector and script */
/*  fid is currently unused, but may, someday, be used to do ttcs */
/* NOTE: GlyphInfo displays vs==-1 as vs==0, and fixes things up */
void
handle_altuni (lua_State *L, struct altuni *au) {
  struct altuni *next = au;
  int k = 1;
  lua_checkstack(L,3);
  while( next != NULL) {
    lua_newtable(L);
    dump_intfield(L, "unicode", next->unienc);
    if (next->vs != -1)
      dump_intfield(L, "variant", next->vs);
    /* dump_intfield(L, "fid", next->fid); */
    lua_rawseti(L, -2, k++);
    next = next->next;
  }
}



#define interesting_vr(a) (((a)->xoff!=0) || ((a)->yoff!=0) || ((a)->h_adv_off!=0) || ((a)->v_adv_off!=0))

void handle_vr (lua_State *L, struct vr *pos) {

  dump_cond_intfield(L,"x", pos->xoff); 
  dump_cond_intfield(L,"y", pos->yoff); 
  dump_cond_intfield(L,"h", pos->h_adv_off); 
  dump_cond_intfield(L,"v", pos->v_adv_off);

}

void
do_handle_generic_pst (lua_State *L, struct generic_pst *pst) {
  int k;
  if (pst->type>LAST_POSSUB_TYPE_ENUM) {
    dump_tag(L,"type",  pst->type); 
  } else {
    dump_enumfield(L,"type",             pst->type, possub_type_enum); 
  }
  /*  unsigned int ticked: 1;*/
  /*  unsigned int temporary: 1;*/ /* Used in afm ligature closure */
  /*  struct lookup_subtable *subtable; */ /* handled by caller */

  lua_checkstack(L,4);
  lua_pushstring(L,"specification");
  lua_createtable(L,0,4);
  if (pst->type == pst_position) {
    handle_vr (L, &pst->u.pos);
  } else if (pst->type == pst_pair) {
    dump_stringfield(L,"paired",pst->u.pair.paired);
    if (pst->u.pair.vr != NULL) {
      lua_pushstring(L,"offsets");
      lua_createtable(L,2,0);
      if (interesting_vr(pst->u.pair.vr)) {
	lua_createtable(L,0,4);
	handle_vr (L, pst->u.pair.vr);
	lua_rawseti(L,-2,1);
      }
      if (interesting_vr(pst->u.pair.vr+1)) {
	lua_createtable(L,0,4);
	handle_vr (L, pst->u.pair.vr+1);
	lua_rawseti(L,-2,2);
      }
      lua_rawset(L,-3);
    }
  } else if (pst->type == pst_substitution) {
    dump_stringfield(L,"variant",pst->u.subs.variant);
  } else if (pst->type == pst_alternate) {
    dump_stringfield(L,"components",pst->u.mult.components);
  } else if (pst->type == pst_multiple) {
    dump_stringfield(L,"components",pst->u.alt.components);
  } else if (pst->type == pst_ligature) {
    dump_stringfield(L,"components",pst->u.lig.components);
    if (pst->u.lig.lig != NULL) {
      dump_char_ref(L,pst->u.lig.lig);
    }
  } else if (pst->type == pst_lcaret) {
    for (k=0;k<pst->u.lcaret.cnt;k++) {
      lua_pushnumber(L,(k+1));
      lua_pushnumber(L,pst->u.lcaret.carets[k]);
      lua_rawset(L,-3);
    }
  }
  lua_rawset(L,-3);
}


void
handle_generic_pst (lua_State *L, struct generic_pst *pst) {
  struct generic_pst *next;
  int k;
  int l = 1;
  next = pst; 
  /* most likely everything arrives in proper order. But to prevent
   * surprises, better do this is the proper way
   */
  while (next != NULL) {       
	if (next->subtable !=NULL && 
		next->subtable->subtable_name !=NULL) {
	  lua_checkstack(L,3); /* just in case */
	  lua_getfield(L,-1,next->subtable->subtable_name);
	  if (!lua_istable(L,-1)) {
		lua_pop(L,1);
		lua_newtable(L);
		lua_setfield(L,-2,next->subtable->subtable_name);
		lua_getfield(L,-1,next->subtable->subtable_name);
	  }
	  k = lua_objlen(L,-1) + 1; 
	  lua_pushnumber(L,k);
	  lua_createtable(L,0,4);    
	  do_handle_generic_pst(L, next);                
	  lua_rawset(L,-3);          
	  next = next->next;         
	  lua_pop(L,1); /* pop the subtable */
	} else {
	  /* Found a pst without subtable, or without subtable name */
	  lua_pushnumber(L,l); l++;
	  lua_createtable(L,0,4);    
	  do_handle_generic_pst(L, next);                
	  lua_rawset(L,-3);          
	  next = next->next;         
	}
  }
}

void
do_handle_liglist (lua_State *L, struct liglist *ligofme) {
  lua_checkstack(L,2);
  if(ligofme->lig != NULL) {
    lua_createtable(L,0,6);
    handle_generic_pst (L,ligofme->lig);
    lua_setfield(L,-2,"lig");    
  }
  dump_char_ref(L,ligofme->first);
  if (ligofme->components != NULL) {
    lua_newtable(L);
    handle_splinecharlist (L,ligofme->components);
    lua_setfield(L,-2,"components");    
  }
  dump_intfield(L,"ccnt",ligofme->ccnt); 
}

void
handle_liglist (lua_State *L, struct liglist *ligofme) {
  struct liglist *next;
  NESTED_TABLE(do_handle_liglist,ligofme,3);
}

void
do_handle_anchorpoint (lua_State *L, struct anchorpoint *anchor) {

  if (anchor->anchor==NULL) {
    return;
  }
  if (anchor->type>=0 && anchor->type <= MAX_ANCHOR_TYPE ) {
    lua_pushstring(L,anchor_type_enum[anchor->type]);
  } else {
    lua_pushstring(L,"Anchorpoint has an unknown type!");
    lua_error(L);
  }
  /* unsigned int selected: 1; */
  /* unsigned int ticked: 1; */

  lua_rawget(L,-2);
  if (!lua_istable(L,-1)) {
    /* create the table first */
    lua_pop(L,1);
    lua_pushstring(L,anchor_type_enum[anchor->type]);
    lua_pushvalue(L,-1);
    lua_newtable(L);
    lua_rawset(L,-4);
    lua_rawget(L,-2);
  }
  /* now the 'type' table is top of stack */
  if (anchor->type==at_baselig) {
    lua_pushstring(L,anchor->anchor->name);
    lua_rawget(L,-2);
    if (!lua_istable(L,-1)) {
      /* create the table first */
      lua_pop(L,1);
      lua_pushstring(L,anchor->anchor->name);
      lua_pushvalue(L,-1);
      lua_newtable(L);
      lua_rawset(L,-4);
      lua_rawget(L,-2);
    }
    lua_newtable(L);
    dump_intfield(L,"x",      anchor->me.x);
    dump_intfield(L,"y",      anchor->me.y);
    if (anchor->has_ttf_pt)
      dump_intfield(L,"ttf_pt_index",  anchor->ttf_pt_index);
    dump_intfield(L,"lig_index",     anchor->lig_index); 
    lua_rawseti(L,-2,(anchor->lig_index+1));  
    lua_pop(L,1);  
  } else {
    lua_pushstring(L,anchor->anchor->name);
    lua_newtable(L);
    dump_intfield(L,"x",      anchor->me.x);
    dump_intfield(L,"y",      anchor->me.y);
    if (anchor->has_ttf_pt)
      dump_intfield(L,"ttf_pt_index",  anchor->ttf_pt_index);
    dump_intfield(L,"lig_index",     anchor->lig_index);     
    lua_rawset(L,-3);  
  }
  lua_pop(L,1);
}

void
handle_anchorpoint (lua_State *L, struct anchorpoint *anchor) {
  struct anchorpoint *next;
  next = anchor;
  while (next != NULL) {
    do_handle_anchorpoint(L, next);
    next = next->next;
  } 
}

void
handle_glyphvariants (lua_State *L, struct glyphvariants *vars) {
  int i ;
  dump_stringfield(L, "variants", vars->variants);
  dump_intfield   (L, "italic_correction", vars->italic_correction);
  lua_newtable(L);
  for (i=0; i< vars->part_cnt; i++) {
    lua_newtable(L);
    dump_stringfield(L, "component", vars->parts[i].component);
    dump_intfield(L, "extender", vars->parts[i].is_extender);
    dump_intfield(L, "start", vars->parts[i].startConnectorLength);
    dump_intfield(L, "end", vars->parts[i].endConnectorLength);
    dump_intfield(L, "advance", vars->parts[i].fullAdvance);
    lua_rawseti(L,-2,(i+1));
  }
  lua_setfield(L, -2, "parts");
}

void handle_mathkernvertex (lua_State *L, struct mathkernvertex *mkv) {
  int i;
  for (i=0; i<mkv->cnt;i++) {
    lua_newtable(L);
    dump_intfield(L, "height", mkv->mkd[i].height );
    dump_intfield(L, "kern",   mkv->mkd[i].kern );
    lua_rawseti(L,-2, (i+1));
  }
}

void handle_mathkern (lua_State *L, struct mathkern *mk) {
  lua_newtable(L);
  handle_mathkernvertex(L, &(mk->top_right));
  lua_setfield(L, -2, "top_right");
  lua_newtable(L);
  handle_mathkernvertex(L, &(mk->top_left));
  lua_setfield(L, -2, "top_left");
  lua_newtable(L);
  handle_mathkernvertex(L, &(mk->bottom_right));
  lua_setfield(L, -2, "bottom_right");
  lua_newtable(L);
  handle_mathkernvertex(L, &(mk->bottom_left));
  lua_setfield(L, -2, "bottom_left");
}



void 
handle_splinechar (lua_State *L,struct splinechar *glyph, int hasvmetrics) {
  DBounds bb;
  if (glyph->xmax==0 && glyph->ymax==0 && glyph->xmin==0 && glyph->ymin==0) {
    SplineCharFindBounds(glyph,&bb);
    glyph->xmin = bb.minx;
    glyph->ymin = bb.miny;
    glyph->xmax = bb.maxx;
    glyph->ymax = bb.maxy;
  }
  dump_stringfield(L,"name",        glyph->name);
  dump_intfield(L,"unicode",     glyph->unicodeenc);
  lua_createtable(L,4,0);
  lua_pushnumber(L,1);  lua_pushnumber(L,glyph->xmin); lua_rawset(L,-3);
  lua_pushnumber(L,2);  lua_pushnumber(L,glyph->ymin); lua_rawset(L,-3);
  lua_pushnumber(L,3);  lua_pushnumber(L,glyph->xmax); lua_rawset(L,-3);
  lua_pushnumber(L,4);  lua_pushnumber(L,glyph->ymax); lua_rawset(L,-3);
  lua_setfield(L,-2,"boundingbox");
  /*dump_intfield(L,"orig_pos",       glyph->orig_pos);*/
  if (hasvmetrics)
    dump_intfield(L,"vwidth",         glyph->vwidth);
  dump_intfield(L,"width",          glyph->width);

  if (glyph->lsidebearing != glyph->xmin) {
    dump_cond_intfield(L,"lsidebearing",   glyph->lsidebearing); 
  }
  /* dump_intfield(L,"ttf_glyph",   glyph->ttf_glyph);  */
  
  /* Layer layers[2];	*/	/* TH Not used */
  /*  int layer_cnt;    */	/* TH Not used */
  /*  StemInfo *hstem;  */	/* TH Not used */
  /*  StemInfo *vstem;	*/	/* TH Not used */
  /*  DStemInfo *dstem;	*/	/* TH Not used */
 
  /* MinimumDistance *md; */    /* TH Not used */
  /* struct charviewbase *views; */ /* TH Not used */
  /* struct charinfo *charinfo;  */ /* TH ? (charinfo.c) */
  /* struct splinefont *parent;  */  /* TH Not used */

  if (glyph->glyph_class>0) {
    dump_enumfield(L,"class",              glyph->glyph_class, glyph_class_enum);  
  }
  /* TH: internal fontforge stuff
     dump_intfield(L,"changed",                  glyph->changed); 
     dump_intfield(L,"changedsincelasthinted",   glyph->changedsincelasthinted); 
     dump_intfield(L,"manualhints",              glyph->manualhints); 
     dump_intfield(L,"ticked",                   glyph->ticked);
     dump_intfield(L,"changed_since_autosave",   glyph->changed_since_autosave); 
     dump_intfield(L,"widthset",                 glyph->widthset); 
     dump_intfield(L,"vconflicts",               glyph->vconflicts); 
     dump_intfield(L,"hconflicts",               glyph->hconflicts); 
     dump_intfield(L,"searcherdummy",            glyph->searcherdummy); 
     dump_intfield(L,"changed_since_search",     glyph->changed_since_search); 
     dump_intfield(L,"wasopen",                  glyph->wasopen); 
     dump_intfield(L,"namechanged",              glyph->namechanged); 
     dump_intfield(L,"blended",                  glyph->blended); 
     dump_intfield(L,"ticked2",                  glyph->ticked2);
     dump_intfield(L,"unused_so_far",            glyph->unused_so_far); 
     dump_intfield(L,"numberpointsbackards",     glyph->numberpointsbackards);  
     dump_intfield(L,"instructions_out_of_date", glyph->instructions_out_of_date);  
     dump_intfield(L,"complained_about_ptnums",  glyph->complained_about_ptnums);
    unsigned int vs_open: 1;
    unsigned int unlink_rm_ovrlp_save_undo: 1;
    unsigned int inspiro: 1;
    unsigned int lig_caret_cnt_fixed: 1;


  uint8 *ttf_instrs;
  int16 ttf_instrs_len;
  int16 countermask_cnt;
  HintMask *countermasks;
  */

 if (glyph->kerns != NULL) {
   lua_newtable(L);
   handle_kernpair(L,glyph->kerns);
   lua_setfield(L,-2,"kerns");
 }
 if (glyph->vkerns != NULL) {
   lua_newtable(L);
   handle_kernpair(L,glyph->vkerns);
   lua_setfield(L,-2,"vkerns");
 }

 if (glyph->dependents != NULL) {
   lua_newtable(L);
   handle_splinecharlist(L,glyph->dependents);
   lua_setfield(L,-2,"dependents");
   
 }
 if (glyph->possub != NULL) {
   lua_newtable(L);
   handle_generic_pst(L,glyph->possub);
   lua_setfield(L,-2,"lookups");
 }

 if (glyph->ligofme != NULL) {
   lua_newtable(L);
   handle_liglist(L,glyph->ligofme);
   lua_setfield(L,-2,"ligatures");
 }

 if (glyph->comment != NULL)
   dump_stringfield(L,"comment",              glyph->comment);

 /* Color color;  */  /* dont care */

 if (glyph->anchor != NULL) {
   lua_newtable(L);
   handle_anchorpoint(L,glyph->anchor);
   lua_setfield(L,-2,"anchors");
 }

 if (glyph->altuni != NULL) {
   lua_newtable(L);
   handle_altuni(L, glyph->altuni);
   lua_setfield(L,-2,"altuni");   
 }

  if (glyph->tex_height != TEX_UNDEF)
    dump_intfield(L,"tex_height",              glyph->tex_height);  
  if (glyph->tex_depth != TEX_UNDEF)
    dump_intfield(L,"tex_depth",               glyph->tex_depth);  
  
  dump_cond_intfield(L, "is_extended_shape", glyph->is_extended_shape);
  if (glyph->italic_correction != TEX_UNDEF)
     dump_intfield(L, "italic_correction", glyph->italic_correction);
  if (glyph->top_accent_horiz != TEX_UNDEF)
     dump_intfield(L, "top_accent",  glyph->top_accent_horiz);

  if (glyph->vert_variants != NULL) {
    lua_newtable(L);
    handle_glyphvariants(L, glyph->vert_variants);
    lua_setfield(L,-2,"vert_variants");
  }
  if (glyph->horiz_variants != NULL) {
    lua_newtable(L);
    handle_glyphvariants(L, glyph->horiz_variants);
    lua_setfield(L,-2,"horiz_variants");
  }
  if (glyph->mathkern != NULL) {
    lua_newtable(L);
    handle_mathkern(L, glyph->mathkern);
    lua_setfield(L,-2,"mathkern");
  }
}

char *panose_values_0[] = { "Any", "No Fit", "Text and Display", "Script", "Decorative", "Pictorial" };

char *panose_values_1[] = { "Any", "No Fit", "Cove", "Obtuse Cove", "Square Cove", "Obtuse Square Cove",
			    "Square", "Thin", "Bone", "Exaggerated", "Triangle", "Normal Sans",
			    "Obtuse Sans", "Perp Sans", "Flared", "Rounded" } ;

char *panose_values_2[] = { "Any", "No Fit", "Very Light", "Light", "Thin", "Book",
			    "Medium", "Demi", "Bold", "Heavy", "Black", "Nord" } ;

char *panose_values_3[] = { "Any", "No Fit", "Old Style", "Modern", "Even Width",
			    "Expanded", "Condensed", "Very Expanded", "Very Condensed", "Monospaced" };

char *panose_values_4[] = { "Any", "No Fit", "None", "Very Low", "Low", "Medium Low",
			    "Medium", "Medium High", "High", "Very High" };

char *panose_values_5[] = { "Any", "No Fit", "Gradual/Diagonal", "Gradual/Transitional","Gradual/Vertical", 
			    "Gradual/Horizontal", "Rapid/Vertical",  "Rapid/Horizontal",  "Instant/Vertical" };

char *panose_values_6[] = {"Any","No Fit","Straight Arms/Horizontal","Straight Arms/Wedge","Straight Arms/Vertical",
			   "Straight Arms/Single Serif","Straight Arms/Double Serif","Non-Straight Arms/Horizontal",
			   "Non-Straight Arms/Wedge","Non-Straight Arms/Vertical","Non-Straight Arms/Single Serif",
			   "Non-Straight Arms/Double Serif" };

char *panose_values_7[] = { "Any", "No Fit","Normal/Contact","Normal/Weighted","Normal/Boxed","Normal/Flattened",
			    "Normal/Rounded","Normal/Off Center","Normal/Square","Oblique/Contact","Oblique/Weighted",
			    "Oblique/Boxed","Oblique/Flattened","Oblique/Rounded","Oblique/Off Center","Oblique/Square" };

char *panose_values_8[] = { "Any","No Fit","Standard/Trimmed","Standard/Pointed","Standard/Serifed","High/Trimmed",
			    "High/Pointed","High/Serifed","Constant/Trimmed","Constant/Pointed","Constant/Serifed",
			    "Low/Trimmed","Low/Pointed","Low/Serifed"};

char *panose_values_9[] = { "Any","No Fit", "Constant/Small",  "Constant/Standard",
			    "Constant/Large", "Ducking/Small", "Ducking/Standard", "Ducking/Large" };


void 
handle_pfminfo (lua_State *L, struct pfminfo pfm) {

  dump_intfield (L, "pfmset",            pfm.pfmset);
  dump_intfield (L, "winascent_add",     pfm.winascent_add);
  dump_intfield (L, "windescent_add",    pfm.windescent_add);
  dump_intfield (L, "hheadascent_add",   pfm.hheadascent_add);
  dump_intfield (L, "hheaddescent_add",  pfm.hheaddescent_add);
  dump_intfield (L, "typoascent_add",    pfm.typoascent_add);
  dump_intfield (L, "typodescent_add",   pfm.typodescent_add);
  dump_intfield (L, "subsuper_set",      pfm.subsuper_set);
  dump_intfield (L, "panose_set",        pfm.panose_set);
  dump_intfield (L, "hheadset",          pfm.hheadset);
  dump_intfield (L, "vheadset",          pfm.vheadset);
  dump_intfield (L, "pfmfamily",         pfm.pfmfamily);
  dump_intfield (L, "weight",            pfm.weight);
  dump_intfield (L, "width",             pfm.width);
  dump_intfield (L, "avgwidth",          pfm.avgwidth);
  dump_intfield (L, "firstchar",         pfm.firstchar);
  dump_intfield (L, "lastchar",          pfm.lastchar);
  lua_createtable(L,0,10);
  dump_enumfield(L,"familytype",      pfm.panose[0], panose_values_0);
  dump_enumfield(L,"serifstyle",      pfm.panose[1], panose_values_1);
  dump_enumfield(L,"weight",          pfm.panose[2], panose_values_2);
  dump_enumfield(L,"proportion",      pfm.panose[3], panose_values_3);
  dump_enumfield(L,"contrast",        pfm.panose[4], panose_values_4);
  dump_enumfield(L,"strokevariation", pfm.panose[5], panose_values_5);
  dump_enumfield(L,"armstyle",        pfm.panose[6], panose_values_6);
  dump_enumfield(L,"letterform",      pfm.panose[7], panose_values_7);
  dump_enumfield(L,"midline",         pfm.panose[8], panose_values_8);
  dump_enumfield(L,"xheight",         pfm.panose[9], panose_values_9);
  lua_setfield  (L,-2,"panose");

  dump_intfield (L, "fstype",            pfm.fstype);
  dump_intfield (L, "linegap",           pfm.linegap);
  dump_intfield (L, "vlinegap",          pfm.vlinegap);
  dump_intfield (L, "hhead_ascent",      pfm.hhead_ascent);
  dump_intfield (L, "hhead_descent",     pfm.hhead_descent);
  dump_intfield (L, "hhead_descent",     pfm.hhead_descent);
  dump_intfield (L, "os2_typoascent",     pfm.os2_typoascent  );
  dump_intfield (L, "os2_typodescent",    pfm.os2_typodescent );
  dump_intfield (L, "os2_typolinegap",    pfm.os2_typolinegap );
  dump_intfield (L, "os2_winascent",      pfm.os2_winascent	  );
  dump_intfield (L, "os2_windescent",     pfm.os2_windescent  );
  dump_intfield (L, "os2_subxsize",       pfm.os2_subxsize	  );
  dump_intfield (L, "os2_subysize",       pfm.os2_subysize	  );
  dump_intfield (L, "os2_subxoff",        pfm.os2_subxoff	  );
  dump_intfield (L, "os2_subyoff",        pfm.os2_subyoff	  );
  dump_intfield (L, "os2_supxsize",       pfm.os2_supxsize	  );
  dump_intfield (L, "os2_supysize",       pfm.os2_supysize	  );
  dump_intfield (L, "os2_supxoff",        pfm.os2_supxoff	  );
  dump_intfield (L, "os2_supyoff",        pfm.os2_supyoff	  );
  dump_intfield (L, "os2_strikeysize",    pfm.os2_strikeysize );
  dump_intfield (L, "os2_strikeypos",     pfm.os2_strikeypos  );
  dump_lstringfield (L, "os2_vendor",    pfm.os2_vendor, 4);
  dump_intfield (L, "os2_family_class",   pfm.os2_family_class);
  dump_intfield (L, "os2_xheight",        pfm.os2_xheight);
  dump_intfield (L, "os2_capheight",      pfm.os2_capheight);
  dump_intfield (L, "os2_defaultchar",    pfm.os2_defaultchar);
  dump_intfield (L, "os2_breakchar",      pfm.os2_breakchar);
  if (pfm.hascodepages) {
    lua_newtable(L);
    lua_pushnumber(L, pfm.codepages[0]);  lua_rawseti(L,-2,1);
    lua_pushnumber(L, pfm.codepages[1]);  lua_rawseti(L,-2,2);
    lua_setfield(L,-2,"codepages");
  }
  if (pfm.hasunicoderanges) {
    lua_newtable(L);
    lua_pushnumber(L, pfm.unicoderanges[0]); lua_rawseti(L,-2,1);
    lua_pushnumber(L, pfm.unicoderanges[1]); lua_rawseti(L,-2,2);
    lua_pushnumber(L, pfm.unicoderanges[2]); lua_rawseti(L,-2,3);
    lua_pushnumber(L, pfm.unicoderanges[3]); lua_rawseti(L,-2,4);
    lua_setfield(L,-2,"unicoderanges");
  }
}


void 
do_handle_enc (lua_State *L, struct enc *enc) { 
  int i;
  
  dump_stringfield(L,"enc_name", enc->enc_name);
  dump_intfield  (L,"char_cnt", enc->char_cnt);

  lua_checkstack(L,4);
  if (enc->char_cnt && enc->unicode != NULL) {
	lua_createtable(L,enc->char_cnt,1);
	for (i=0;i<enc->char_cnt;i++) {
	  lua_pushnumber(L,i);
	  lua_pushnumber(L,enc->unicode[i]);
	  lua_rawset(L,-3);
	}
	lua_setfield(L,-2,"unicode");
  }

  if (enc->char_cnt && enc->psnames != NULL) {
	lua_createtable(L,enc->char_cnt,1);
	for (i=0;i<enc->char_cnt;i++) {
	  lua_pushnumber(L,i);
	  lua_pushstring(L,enc->psnames[i]);
	  lua_rawset(L,-3);
	}
	lua_setfield(L,-2,"psnames");
  }
  dump_intfield  (L,"builtin",            enc->builtin         );
  dump_intfield  (L,"hidden",             enc->hidden         );
  dump_intfield  (L,"only_1byte",         enc->only_1byte     );
  dump_intfield  (L,"has_1byte",          enc->has_1byte      );
  dump_intfield  (L,"has_2byte",          enc->has_2byte      );
  dump_cond_intfield  (L,"is_unicodebmp",      enc->is_unicodebmp  );
  dump_cond_intfield  (L,"is_unicodefull",     enc->is_unicodefull );
  dump_cond_intfield  (L,"is_custom",          enc->is_custom      );
  dump_cond_intfield  (L,"is_original",        enc->is_original    );  
  dump_cond_intfield  (L,"is_compact",         enc->is_compact     );
  dump_cond_intfield  (L,"is_japanese",        enc->is_japanese    );  
  dump_cond_intfield  (L,"is_korean",          enc->is_korean      );
  dump_cond_intfield  (L,"is_tradchinese",     enc->is_tradchinese );
  dump_cond_intfield  (L,"is_simplechinese",   enc->is_simplechinese);

  if (enc->iso_2022_escape_len > 0) {
	dump_lstringfield (L,"iso_2022_escape", enc->iso_2022_escape, enc->iso_2022_escape_len);
  }
  dump_intfield (L,"low_page", enc->low_page);
  dump_intfield(L,"high_page", enc->high_page);

  dump_stringfield(L,"iconv_name", enc->iconv_name);

  dump_intfield  (L,"char_max", enc->char_max);
}
      
void 
handle_enc (lua_State *L, struct enc *enc) {
  struct enc *next;
  NESTED_TABLE(do_handle_enc,enc,24);
}

void 
handle_encmap (lua_State *L, struct encmap *map, int notdef_loc) {
  int i;
  dump_intfield(L,"enccount", map->enccount) ;
  dump_intfield(L,"encmax",   map->encmax) ;
  dump_intfield(L,"backmax",  map->backmax) ;
  /*dump_intfield(L,"ticked",   map->ticked) ;*/
  if (map->remap != NULL) {
    lua_newtable(L);
    dump_intfield(L,"firstenc", map->remap->firstenc) ;
    dump_intfield(L,"lastenc",  map->remap->lastenc) ;
    dump_intfield(L,"infont",   map->remap->infont) ;
    lua_setfield(L,-2,"remap");
  }
  lua_checkstack(L,4);
  if (map->encmax > 0 && map->map != NULL) {
    lua_createtable(L,map->encmax,1);
    for (i=0;i<map->encmax;i++) {
      if (map->map[i]!=-1) {
        int l = map->map[i];
        lua_pushnumber(L,i);
        if (l<notdef_loc)
          lua_pushnumber(L,(l+1));
        else
          lua_pushnumber(L,l);
		lua_rawset(L,-3);
      }
    }
    lua_setfield(L,-2,"map");
  }

  if (map->backmax > 0 && map->backmap != NULL) {
    lua_newtable(L);
    for (i=0;i<map->backmax;i++) {
      if (map->backmap[i]!=-1) { /* TODO: check this, because valgrind sometimes says
                                    "Conditional jump or move depends on uninitialised value(s)"
                                    needs a test file.
                                 */ 
        if (i<notdef_loc)
          lua_pushnumber(L,(i+1));
        else
          lua_pushnumber(L,i);
		lua_pushnumber(L,map->backmap[i]);
		lua_rawset(L,-3);
      }
    }
    lua_setfield(L,-2,"backmap");
  }

  if (map->enc != NULL) {
    lua_newtable(L);
    handle_enc(L,map->enc);
    lua_setfield(L,-2,"enc");
  }
}

static void
handle_psdict (lua_State *L, struct psdict *private) {
  int k;
  if (private->keys != NULL && private->values != NULL) {
	for (k=0;k<private->next;k++) {
	  lua_pushstring(L,private->keys[k]);
	  lua_pushstring(L,private->values[k]);
	  lua_rawset(L,-3);
	}
  }
}

void 
do_handle_ttflangname (lua_State *L, struct ttflangname *names) {
  int k;
  dump_stringfield(L,"lang", (char *)MSLangString(names->lang)) ;
  lua_checkstack(L,4);
  lua_createtable(L,0,ttf_namemax);
  for (k=0;k<ttf_namemax;k++) {
	lua_pushstring(L,ttfnames_enum[k]);
	lua_pushstring(L,names->names[k]);
	lua_rawset(L,-3);
  }
  lua_setfield(L, -2 , "names");
}


void 
handle_ttflangname (lua_State *L, struct ttflangname *names) {
  struct ttflangname *next;
  NESTED_TABLE(do_handle_ttflangname,names,2);
}


void
do_handle_anchorclass (lua_State *L, struct anchorclass *anchor) {

  dump_stringfield(L,"name",           anchor->name);
  dump_subtable_name (L, "lookup", anchor->subtable);
  dump_enumfield(L,"type",              anchor->type, anchorclass_type_enum);
  /*   uint8 has_base; */
  /*  uint8 processed, has_mark, matches, ac_num; */
  /*  uint8 ticked; */
}

void
handle_anchorclass (lua_State *L, struct anchorclass *anchor) {
  struct anchorclass *next;
  NESTED_TABLE(do_handle_anchorclass,anchor,10);
}

void
do_handle_ttf_table  (lua_State *L, struct ttf_table *ttf_tab) {

  dump_tag(L,"tag",               ttf_tab->tag);
  dump_intfield(L,"len",          ttf_tab->len);
  dump_intfield(L,"maxlen",       ttf_tab->maxlen);
  dump_lstringfield(L,"data", (char *)ttf_tab->data, ttf_tab->len);
}

void
handle_ttf_table  (lua_State *L, struct ttf_table *ttf_tab) {
  struct ttf_table *next;
  NESTED_TABLE(do_handle_ttf_table,ttf_tab,4);
}

void
do_handle_kernclass (lua_State *L, struct kernclass *kerns) {
  int k;
  
  /*
   * dump_intfield(L,"first_cnt",       kerns->first_cnt);
   * dump_intfield(L,"second_cnt",      kerns->second_cnt);
   */
  lua_checkstack(L,4);
  lua_createtable(L,kerns->first_cnt,1);
  for (k=0;k<kerns->first_cnt;k++) {
    lua_pushnumber(L,(k+1));
    lua_pushstring(L,kerns->firsts[k]);
    lua_rawset(L,-3);
  }
  lua_setfield(L,-2,"firsts");

  lua_createtable(L,kerns->second_cnt,1);
  for (k=0;k<kerns->second_cnt;k++) {
    lua_pushnumber(L,(k+1));
	lua_pushstring(L,kerns->seconds[k]);
	lua_rawset(L,-3);
  }
  lua_setfield(L,-2,"seconds");

  dump_subtable_name(L, "lookup", kerns->subtable);
  /*dump_intfield(L,"kcid", kerns->kcid); *//* probably not needed */

  lua_createtable(L,kerns->second_cnt*kerns->first_cnt,1);
  for (k=0;k<(kerns->second_cnt*kerns->first_cnt);k++) {
    if (kerns->offsets[k]!=0) {
      lua_pushnumber(L,(k+1));
      lua_pushnumber(L,kerns->offsets[k]);
      lua_rawset(L,-3);
    }
  }
  lua_setfield(L,-2,"offsets");

}

void
handle_kernclass (lua_State *L, struct kernclass *kerns) {
  struct kernclass *next;
  NESTED_TABLE(do_handle_kernclass,kerns,8);
}


#define DUMP_NUMBER_ARRAY(s,cnt,item) {					\
    if (cnt>0 && item != NULL) {						\
      int kk;											\
      lua_newtable(L);									\
      for (kk=0;kk<cnt;kk++) {							\
		lua_pushnumber(L,(kk+1));						\
		lua_pushnumber(L,item[kk]);						\
		lua_rawset(L,-3); }								\
      lua_setfield(L,-2,s); } }


#define DUMP_STRING_ARRAY(s,cnt,item) {				\
    if (cnt>0 && item!=NULL) {						\
      int kk;										\
      lua_newtable(L);								\
      for (kk=0;kk<cnt;kk++) {						\
		lua_pushnumber(L,(kk+1));					\
		lua_pushstring(L,item[kk]);					\
		lua_rawset(L,-3); }							\
      lua_setfield(L,-2,s); } }

#define DUMP_EXACT_STRING_ARRAY(s,cnt,item) {		\
    if (cnt>0 && item!=NULL) {						\
      int kk;										\
      lua_newtable(L);								\
      for (kk=0;kk<cnt;kk++) {						\
		lua_pushnumber(L,(kk));						\
		lua_pushstring(L,item[kk]);					\
		lua_rawset(L,-3); }							\
      lua_setfield(L,-2,s); } }


void handle_fpst_rule (lua_State *L, struct fpst_rule *rule, int format) {
  int k;


  if (format == pst_glyphs) {

    lua_newtable(L);
    dump_stringfield(L,"names",rule->u.glyph.names);
    dump_stringfield(L,"back",rule->u.glyph.back);
    dump_stringfield(L,"fore",rule->u.glyph.fore);
    lua_setfield(L,-2,fpossub_format_enum[format]);

  } else if (format == pst_class) {
  
    lua_newtable(L);
    DUMP_NUMBER_ARRAY("current", rule->u.class.ncnt,rule->u.class.nclasses);
    DUMP_NUMBER_ARRAY("before", rule->u.class.bcnt,rule->u.class.bclasses);
    DUMP_NUMBER_ARRAY("after", rule->u.class.fcnt,rule->u.class.fclasses);
#if 0
    DUMP_NUMBER_ARRAY("allclasses", 0,rule->u.class.allclasses);
#endif
    lua_setfield(L,-2,fpossub_format_enum[format]);

  } else if (format == pst_coverage) {

    lua_newtable(L);
    DUMP_STRING_ARRAY("current", rule->u.coverage.ncnt,rule->u.coverage.ncovers);
    DUMP_STRING_ARRAY("before", rule->u.coverage.bcnt,rule->u.coverage.bcovers);
    DUMP_STRING_ARRAY("after", rule->u.coverage.fcnt,rule->u.coverage.fcovers);
    lua_setfield(L,-2,fpossub_format_enum[format]);

  } else if (format == pst_reversecoverage) {

    lua_newtable(L);
    DUMP_STRING_ARRAY("current", rule->u.rcoverage.always1,rule->u.rcoverage.ncovers);
    DUMP_STRING_ARRAY("before", rule->u.rcoverage.bcnt,rule->u.rcoverage.bcovers);
    DUMP_STRING_ARRAY("after", rule->u.rcoverage.fcnt,rule->u.rcoverage.fcovers);
    dump_stringfield(L,"replacements", rule->u.rcoverage.replacements);
    lua_setfield(L,-2,fpossub_format_enum[format]);
  } else {
    fprintf(stderr,"handle_fpst_rule(): Unknown rule format: %d\n",format);
  }
  
  if (rule->lookup_cnt>0) {
    lua_newtable(L);
    for (k=0;k<rule->lookup_cnt;k++) {
      lua_pushnumber(L,(rule->lookups[k].seq+1)); 
      if (rule->lookups[k].lookup!=NULL) {
	lua_pushstring(L,rule->lookups[k].lookup->lookup_name);
      } else {
	lua_pushnil(L);
      }
      lua_rawset(L,-3);
    }
    lua_setfield(L,-2,"lookups");
  } else {
    /*fprintf(stderr,"handle_fpst_rule(): No lookups?\n");*/
  }
}

void 
do_handle_generic_fpst(lua_State *L, struct generic_fpst *fpst) {
  int k;

  if (fpst->type>LAST_POSSUB_TYPE_ENUM) {
    dump_intfield(L,"type", fpst->type);
  } else {
    dump_enumfield(L,"type", fpst->type, possub_type_enum);
  }
  dump_enumfield(L,"format", fpst->format, fpossub_format_enum);

  if (fpst->format==pst_class) {
	DUMP_EXACT_STRING_ARRAY("current_class",fpst->nccnt,fpst->nclass);
	DUMP_EXACT_STRING_ARRAY("before_class",fpst->bccnt,fpst->bclass);
	DUMP_EXACT_STRING_ARRAY("after_class",fpst->fccnt,fpst->fclass);
  } else {
	DUMP_STRING_ARRAY("current_class",fpst->nccnt,fpst->nclass);
	DUMP_STRING_ARRAY("before_class",fpst->bccnt,fpst->bclass);
	DUMP_STRING_ARRAY("after_class",fpst->fccnt,fpst->fclass);
  }

  lua_checkstack(L,4);
  if (fpst->rule_cnt>0) {
    lua_createtable(L,fpst->rule_cnt,1);
    for (k=0;k<fpst->rule_cnt;k++) {
      lua_pushnumber(L,(k+1));
      lua_newtable(L);
      handle_fpst_rule(L,&(fpst->rules[k]),fpst->format);
      lua_rawset(L,-3);
    }
    lua_setfield(L,-2,"rules");
  }
  /*dump_intfield (L,"ticked", fpst->ticked);*/
}

void 
handle_generic_fpst(lua_State *L, struct generic_fpst *fpst) {
  struct generic_fpst *next;
  int k = 1;                        
  lua_checkstack(L,3);	      
  if (fpst->subtable != NULL && 
      fpst->subtable->subtable_name != NULL) {
    lua_pushstring(L,fpst->subtable->subtable_name);
  } else {
    lua_pushnumber(L,k); k++;
  }
  lua_createtable(L,0,10);          
  do_handle_generic_fpst(L,fpst);   
  lua_rawset(L,-3); 
  next = fpst->next;                                                       
  while (next != NULL) {                                                   
    lua_checkstack(L,3);                                                   
  if (next->subtable != NULL &&                                            
      next->subtable->subtable_name != NULL) {                             
    lua_pushstring(L,next->subtable->subtable_name);                       
  } else {                                                                 
    lua_pushnumber(L,k); k++;                                              
  }                                                                        
    lua_createtable(L,0,10);                                               
    do_handle_generic_fpst(L, next);                                       
    lua_rawset(L,-3);                                                      
    next = next->next;                                                     
  } 
}

void
do_handle_otfname (lua_State *L, struct otfname *oname) {
  dump_intfield(L,"lang",        oname->lang);
  dump_stringfield(L,"name",     oname->name);
}

void
handle_otfname (lua_State *L, struct otfname *oname) {
  struct otfname *next;
  NESTED_TABLE(do_handle_otfname,oname,2);
}

void 
do_handle_macname (lua_State *L, struct macname *featname) {
  dump_intfield(L,"enc",         featname->enc);
  dump_intfield(L,"lang",        featname->lang);
  dump_stringfield(L,"name",     featname->name);
}

void 
handle_macname (lua_State *L, struct macname *featname) {
  struct macname *next;
  NESTED_TABLE(do_handle_macname,featname,3);
}

void 
do_handle_macsetting (lua_State *L, struct macsetting *settings) {
  dump_intfield(L,"setting",            settings->setting);
  dump_intfield(L,"strid",              settings->strid);
  dump_intfield(L,"initially_enabled",  settings->initially_enabled);
  if (settings->setname != NULL) {
    lua_newtable(L);
    handle_macname(L,settings->setname);
    lua_setfield(L,-2,"setname");
  }
}

void 
handle_macsetting (lua_State *L, struct macsetting *settings) {
  struct macsetting *next;
  NESTED_TABLE(do_handle_macsetting,settings,4);
}


void 
do_handle_macfeat (lua_State *L, struct macfeat *features) {

  dump_intfield(L,"feature",         features->feature);
  dump_intfield(L,"ismutex",         features->ismutex);
  dump_intfield(L,"default_setting", features->default_setting);
  dump_intfield(L,"strid",           features->strid);

  if (features->featname != NULL) {
    lua_newtable(L);
    handle_macname(L,features->featname);
    lua_setfield(L,-2,"featname");
  }

  if (features->settings != NULL) {
    lua_newtable(L);
    handle_macsetting(L,features->settings);
    lua_setfield(L,-2,"settings");
  }
}

void 
handle_macfeat (lua_State *L, struct macfeat *features) {
  struct macfeat *next;
  NESTED_TABLE(do_handle_macfeat,features,6);
}


/* asm_state flags:
 Indic:
	0x8000	mark current glyph as first in rearrangement
	0x4000	don't advance to next glyph
	0x2000	mark current glyph as last
	0x000f	verb
		0 = no change		8 = AxCD => CDxA
		1 = Ax => xA		9 = AxCD => DCxA
		2 = xD => Dx		a = ABxD => DxAB
		3 = AxD => DxA		b = ABxD => DxBA
		4 = ABx => xAB		c = ABxCD => CDxAB
		5 = ABx => xBA		d = ABxCD => CDxBA
		6 = xCD => CDx		e = ABxCD => DCxAB
		7 = xCD => DCx		f = ABxCD => DCxBA
 Contextual:
	0x8000	mark current glyph
	0x4000	don't advance to next glyph
 Insert:
	0x8000	mark current glyph
	0x4000	don't advance to next glyph
	0x2000	current is Kashida like
	0x1000	mark is Kashida like
	0x0800	current insert before
	0x0400	mark insert before
	0x03e0	count of chars to be inserted at current (31 max)
	0x001f	count of chars to be inserted at mark (31 max)
 Kern:
	0x8000	add current glyph to kerning stack
	0x4000	don't advance to next glyph
	0x3fff	value offset
*/
/* to be tested */
void 
do_handle_generic_asm (lua_State *L, struct generic_asm *sm) {
  int i, k;

  dump_enumfield  (L,"type",  sm->type, asm_type_enum); 
  /* backref */
  dump_subtable_name(L, "lookup", sm->subtable);
  /* uint8 ticked; */
  lua_newtable(L);
  if (sm->flags & asm_vert) {
    lua_pushstring(L,"vert");  lua_pushboolean(L,1);   lua_rawset(L,-3);
  }
  if (sm->flags & asm_descending) {
    lua_pushstring(L,"descending");  lua_pushboolean(L,1);   lua_rawset(L,-3);
  }
  if (sm->flags & asm_always) {
    lua_pushstring(L,"always");  lua_pushboolean(L,1);   lua_rawset(L,-3);
  }
  lua_setfield(L,-2,"flags");

  if (sm->class_cnt >0) {
    lua_newtable(L);
    for (i=0; i<sm->class_cnt; i++) {
      if (sm->classes[i]!=NULL) {
        lua_pushstring(L,sm->classes[i]);
        lua_rawseti(L,-2,(i+1));
      }
    }
    lua_setfield(L,-2,"classes");
  }
  if (sm->state_cnt >0) {
    lua_newtable(L);
    for (i=0; i<(sm->class_cnt * sm->state_cnt); i++) {
       struct asm_state as = sm->state[i];
       dump_intfield(L,"next", as.next_state);
       dump_intfield(L,"flags", as.flags);
       if (sm->type==asm_context) {
         lua_newtable(L);
         if (as.u.context.mark_lookup!=NULL)
           dump_stringfield(L,"mark", as.u.context.mark_lookup->lookup_name); /* backref */
         if (as.u.context.cur_lookup!=NULL)
           dump_stringfield(L,"cur",  as.u.context.cur_lookup->lookup_name); /* backref */
         lua_setfield(L,-2,"context");
       } else if  (sm->type==asm_insert) {
         lua_newtable(L);
         lua_pushstring(L, as.u.insert.mark_ins);
         lua_setfield(L, -2, "mark");
         lua_pushstring(L, as.u.insert.cur_ins);
         lua_setfield(L, -2, "cur");
         lua_setfield(L,-2,"insert");
       } else if  (sm->type==asm_kern) {
         lua_newtable(L);
         for (k=0;k<as.u.kern.kcnt;k++) {
           lua_pushnumber(L,as.u.kern.kerns[k]);
           lua_rawseti(L,-2, (k+1));
         }
         lua_setfield(L,-2,"kerns");
       }
    }
    lua_setfield(L,-2,"states");
  }
}

void 
handle_generic_asm (lua_State *L, struct generic_asm *sm) {
  struct generic_asm *next;
  NESTED_TABLE(do_handle_generic_asm,sm,6);
}




void handle_MATH (lua_State *L, struct MATH *MATH) {
    dump_intfield(L,"ScriptPercentScaleDown",MATH->ScriptPercentScaleDown);
    dump_intfield(L,"ScriptScriptPercentScaleDown",MATH->ScriptScriptPercentScaleDown);
    dump_intfield(L,"DelimitedSubFormulaMinHeight",MATH->DelimitedSubFormulaMinHeight);
    dump_intfield(L,"DisplayOperatorMinHeight",MATH->DisplayOperatorMinHeight);
    dump_intfield(L,"MathLeading",MATH->MathLeading);
    dump_intfield(L,"AxisHeight",MATH->AxisHeight);
    dump_intfield(L,"AccentBaseHeight",MATH->AccentBaseHeight);
    dump_intfield(L,"FlattenedAccentBaseHeight",MATH->FlattenedAccentBaseHeight);
    dump_intfield(L,"SubscriptShiftDown",MATH->SubscriptShiftDown);
    dump_intfield(L,"SubscriptTopMax",MATH->SubscriptTopMax);
    dump_intfield(L,"SubscriptBaselineDropMin",MATH->SubscriptBaselineDropMin);
    dump_intfield(L,"SuperscriptShiftUp",MATH->SuperscriptShiftUp);
    dump_intfield(L,"SuperscriptShiftUpCramped",MATH->SuperscriptShiftUpCramped);
    dump_intfield(L,"SuperscriptBottomMin",MATH->SuperscriptBottomMin);
    dump_intfield(L,"SuperscriptBaselineDropMax",MATH->SuperscriptBaselineDropMax);
    dump_intfield(L,"SubSuperscriptGapMin",MATH->SubSuperscriptGapMin);
    dump_intfield(L,"SuperscriptBottomMaxWithSubscript",MATH->SuperscriptBottomMaxWithSubscript);
    dump_intfield(L,"SpaceAfterScript",MATH->SpaceAfterScript);
    dump_intfield(L,"UpperLimitGapMin",MATH->UpperLimitGapMin);
    dump_intfield(L,"UpperLimitBaselineRiseMin",MATH->UpperLimitBaselineRiseMin);
    dump_intfield(L,"LowerLimitGapMin",MATH->LowerLimitGapMin);
    dump_intfield(L,"LowerLimitBaselineDropMin",MATH->LowerLimitBaselineDropMin);
    dump_intfield(L,"StackTopShiftUp",MATH->StackTopShiftUp);
    dump_intfield(L,"StackTopDisplayStyleShiftUp",MATH->StackTopDisplayStyleShiftUp);
    dump_intfield(L,"StackBottomShiftDown",MATH->StackBottomShiftDown);
    dump_intfield(L,"StackBottomDisplayStyleShiftDown",MATH->StackBottomDisplayStyleShiftDown);
    dump_intfield(L,"StackGapMin",MATH->StackGapMin);
    dump_intfield(L,"StackDisplayStyleGapMin",MATH->StackDisplayStyleGapMin);
    dump_intfield(L,"StretchStackTopShiftUp",MATH->StretchStackTopShiftUp);
    dump_intfield(L,"StretchStackBottomShiftDown",MATH->StretchStackBottomShiftDown);
    dump_intfield(L,"StretchStackGapAboveMin",MATH->StretchStackGapAboveMin);
    dump_intfield(L,"StretchStackGapBelowMin",MATH->StretchStackGapBelowMin);
    dump_intfield(L,"FractionNumeratorShiftUp",MATH->FractionNumeratorShiftUp);
    dump_intfield(L,"FractionNumeratorDisplayStyleShiftUp",MATH->FractionNumeratorDisplayStyleShiftUp);
    dump_intfield(L,"FractionDenominatorShiftDown",MATH->FractionDenominatorShiftDown);
    dump_intfield(L,"FractionDenominatorDisplayStyleShiftDown",MATH->FractionDenominatorDisplayStyleShiftDown);
    dump_intfield(L,"FractionNumeratorGapMin",MATH->FractionNumeratorGapMin);
    dump_intfield(L,"FractionNumeratorDisplayStyleGapMin",MATH->FractionNumeratorDisplayStyleGapMin);
    dump_intfield(L,"FractionRuleThickness",MATH->FractionRuleThickness);
    dump_intfield(L,"FractionDenominatorGapMin",MATH->FractionDenominatorGapMin);
    dump_intfield(L,"FractionDenominatorDisplayStyleGapMin",MATH->FractionDenominatorDisplayStyleGapMin);
    dump_intfield(L,"SkewedFractionHorizontalGap",MATH->SkewedFractionHorizontalGap);
    dump_intfield(L,"SkewedFractionVerticalGap",MATH->SkewedFractionVerticalGap);
    dump_intfield(L,"OverbarVerticalGap",MATH->OverbarVerticalGap);
    dump_intfield(L,"OverbarRuleThickness",MATH->OverbarRuleThickness);
    dump_intfield(L,"OverbarExtraAscender",MATH->OverbarExtraAscender);
    dump_intfield(L,"UnderbarVerticalGap",MATH->UnderbarVerticalGap);
    dump_intfield(L,"UnderbarRuleThickness",MATH->UnderbarRuleThickness);
    dump_intfield(L,"UnderbarExtraDescender",MATH->UnderbarExtraDescender);
    dump_intfield(L,"RadicalVerticalGap",MATH->RadicalVerticalGap);
    dump_intfield(L,"RadicalDisplayStyleVerticalGap",MATH->RadicalDisplayStyleVerticalGap);
    dump_intfield(L,"RadicalRuleThickness",MATH->RadicalRuleThickness);
    dump_intfield(L,"RadicalExtraAscender",MATH->RadicalExtraAscender);
    dump_intfield(L,"RadicalKernBeforeDegree",MATH->RadicalKernBeforeDegree);
    dump_intfield(L,"RadicalKernAfterDegree",MATH->RadicalKernAfterDegree);
    dump_intfield(L,"RadicalDegreeBottomRaisePercent",MATH->RadicalDegreeBottomRaisePercent);
    dump_intfield(L,"MinConnectorOverlap",MATH->MinConnectorOverlap);	
}

/* the handling of BASE is untested, no font */
void handle_baselangextent (lua_State *L, struct baselangextent *ble);

void
do_handle_baselangextent (lua_State *L, struct baselangextent *ble) {
   dump_tag(L,"tag",ble->lang);	
   dump_intfield(L,"ascent",ble->ascent);	
   dump_intfield(L,"descent",ble->descent);	
   lua_newtable(L);
   handle_baselangextent(L, ble->features);
   lua_setfield(L,-2,"features");
}


void handle_baselangextent (lua_State *L, struct baselangextent *ble) {
  struct baselangextent *next;
  NESTED_TABLE(do_handle_baselangextent,ble,4);
}


void handle_base  (lua_State *L, struct Base *Base) {
  int i;
  struct basescript *next = Base->scripts;
  lua_newtable(L);
  for ( i=0; i<Base->baseline_cnt; i++ ) {
    lua_pushstring(L,make_tag_string(Base->baseline_tags[i]));
    lua_rawseti(L,-2,(i+1));
  }
  lua_setfield(L,-2,"tags");
  if (next != NULL) {
    lua_newtable(L);
    while (next != NULL) {
      lua_pushstring(L,make_tag_string(next->script));
      lua_newtable(L);
      dump_intfield(L, "default_baseline", (next->def_baseline+1)) ;
      lua_newtable(L);
      for ( i=0; i<Base->baseline_cnt; i++ ) {
        lua_pushnumber(L, next->baseline_pos[i]) ;
        lua_rawseti(L,-2, (i+1));
      }
      lua_setfield(L, -2, "baseline");
      lua_newtable(L);
      handle_baselangextent(L, next->langs);
      lua_setfield(L, -2, "lang");
      lua_rawset(L,-3);
      next = next->next;
    }
    lua_setfield(L,-2,"scripts");
  }
}


void
handle_axismap (lua_State *L, struct axismap *am) {
  int i;
  lua_checkstack(L,3);
  lua_newtable(L);
  for (i=0;i<am->points;i++) {
    lua_pushnumber(L, am->blends[i] )  ;
    lua_rawseti(L,-2, (i+1));     
  }
  lua_setfield(L,-2,"blends");
  lua_newtable(L);
  for (i=0;i<am->points;i++) {
    lua_pushnumber(L, am->designs[i] )  ;
    lua_rawseti(L,-2, (i+1));     
  }
  lua_setfield(L,-2,"designs");
  dump_realfield(L, "min", am->min)  ;
  dump_realfield(L, "def", am->def)  ;
  dump_realfield(L, "max", am->max)  ;
  if (am->axisnames != NULL ){
    lua_newtable(L);
    handle_macname(L, am->axisnames);
    lua_setfield(L,-2,"axisnames");
  }
} 


void
handle_mmset (lua_State *L, struct mmset *mm) {
  int i, k;
  lua_newtable(L);
  for (i=0;i<mm->axis_count;i++) {
    lua_pushstring(L, mm->axes[i]) ;
    lua_rawseti(L,-2, (i+1));
  }
  lua_setfield(L,-2,"axes");

  dump_intfield(L,"instance_count",mm->instance_count);	   
  /* SplineFont *normal; */ /* this is the parent */
  if (mm->instance_count>0) {
    lua_newtable(L);
    for (i=0;i<mm->instance_count*mm->axis_count;i++) {
      lua_pushnumber(L, mm->positions[i]) ;
      lua_rawseti(L,-2, (i+1));
    }
    lua_setfield(L,-2,"positions");

    /* better not to do this */
#if 0
    {
      struct mmset *mmsave ;
      lua_newtable(L);
      for (i=0;i<mm->instance_count;i++) {
        lua_checkstack(L,20);
        lua_createtable(L,0,60);
        mmsave = mm->instances[i]->mm;
        mm->instances[i]->mm = NULL;
        handle_splinefont(L, mm->instances[i]);
        mm->instances[i]->mm = mmsave;
        lua_rawseti(L,-2, (i+1));
      }
      lua_setfield(L,-2,"instances");
    }
#endif

    lua_newtable(L);
    for (i=0;i<mm->instance_count;i++) {
      lua_pushnumber(L, mm->defweights[i]) ;
      lua_rawseti(L,-2, (i+1));
    }
    lua_setfield(L,-2,"defweights");
  }
  
  if (mm->axismaps != NULL) {
    lua_newtable(L);
    for (i=0;i<mm->axis_count;i++) {
      lua_newtable(L);
      handle_axismap(L, &(mm->axismaps[i]));
      lua_rawseti(L, -2, (i+1));
    }
    lua_setfield(L,-2,"axismaps"); 
  }
  dump_stringfield(L,"cdv",mm->cdv);	
  dump_stringfield(L,"ndv",mm->ndv);	
  dump_intfield(L,"named_instance_count",mm->named_instance_count);	

  if (mm->named_instance_count>0) {
      lua_newtable(L);
      for (i=0; i<mm->named_instance_count;i++) {
        struct named_instance *ni = &(mm->named_instances[i]);
        lua_newtable(L);

        lua_newtable(L);
        for (k=0;k<=mm->axis_count;k++) {
          lua_pushnumber(L,ni->coords[k]);
          lua_rawseti(L,-2,(k+1));
        }
        lua_setfield(L,-2,"coords");

        lua_newtable(L);
        handle_macname(L, ni->names);
        lua_setfield(L,-2,"names");

        lua_rawseti(L,-2,(i+1));
       }
      lua_setfield(L,-2,"named_instances");
  }
  /* unsigned int changed: 1; */
  dump_intfield(L,"apple",mm->apple);	
}



void
handle_splinefont(lua_State *L, struct splinefont *sf) {
  int k;
  int fix_notdef = 0;
  int l = -1;

  dump_stringfield(L,"table_version",   LUA_OTF_VERSION);
  dump_stringfield(L,"fontname",        sf->fontname);
  dump_stringfield(L,"fullname",        sf->fullname);
  dump_stringfield(L,"familyname",      sf->familyname);
  dump_stringfield(L,"weight",          sf->weight);
  dump_stringfield(L,"copyright",       sf->copyright);
  dump_stringfield(L,"filename",        sf->filename);
  /* dump_stringfield(L,"defbasefilename", sf->defbasefilename); */
  dump_stringfield(L,"version",         sf->version);
  dump_floatfield (L,"italicangle",     sf->italicangle);
  dump_floatfield (L,"upos",            sf->upos);
  dump_floatfield (L,"uwidth",          sf->uwidth);
  dump_intfield   (L,"ascent",          sf->ascent);
  dump_intfield   (L,"descent",         sf->descent);
  dump_intfield   (L,"uniqueid",        sf->uniqueid);
  dump_intfield   (L,"glyphcnt",        sf->glyphcnt);
  dump_intfield   (L,"glyphmax",        sf->glyphmax);
  dump_intfield   (L,"units_per_em",    sf->units_per_em);

  if (sf->possub != NULL) {
    lua_newtable(L);
    handle_generic_fpst(L,sf->possub);
    lua_setfield(L,-2,"lookups");
  }

  lua_checkstack(L,4);
  lua_createtable(L,sf->glyphcnt,0);

  /* This after-the-fact type discovery is not brilliant,
     I should really add a 'format' key in the structure */     
  if ((sf->origname != NULL) &&
      (strmatch(sf->origname+strlen(sf->origname)-4, ".pfa")==0 ||
       strmatch(sf->origname+strlen(sf->origname)-4, ".pfb")==0)) {
    fix_notdef = 1;
  }

  if (fix_notdef) {
    /* some code to ensure that the .notdef ends up in slot 0 
       (this will actually be enforced by the CFF writer) */
    for (k=0;k<sf->glyphcnt;k++) {
      if (sf->glyphs[k]) {
        if (strcmp(sf->glyphs[k]->name,".notdef") == 0) {
          l = k;
        }
      }
    }
    if (l==-1) { /* fake a .notdef at the end */
      l = sf->glyphcnt;
    }
    for (k=0;k<l;k++) {
      lua_pushnumber(L,(k+1));
      lua_createtable(L,0,12);
      if (sf->glyphs[k]) {
        handle_splinechar(L,sf->glyphs[k], sf->hasvmetrics);
      }
      lua_rawset(L,-3);
    }
    if (sf->glyphs != NULL && l<sf->glyphcnt) {
      lua_pushnumber(L,0);
      lua_createtable(L,0,12);
      if (sf->glyphs[l]) {
        handle_splinechar(L,sf->glyphs[l], sf->hasvmetrics);
      }
      lua_rawset(L,-3);
    }
  }
  if ((l+1)<sf->glyphcnt) {
    for (k=(l+1);k<sf->glyphcnt;k++) {
      lua_pushnumber(L,k);
      lua_createtable(L,0,12);
      if (sf->glyphs[k]) {
        handle_splinechar(L,sf->glyphs[k], sf->hasvmetrics);
      }
      lua_rawset(L,-3);
    }
  }
  lua_setfield(L,-2,"glyphs");

  /* dump_intfield(L,"changed",                   sf->changed); */
  dump_intfield(L,"hasvmetrics",               sf->hasvmetrics);
  dump_intfield(L,"onlybitmaps",               sf->onlybitmaps);
  dump_intfield(L,"serifcheck",                sf->serifcheck);
  dump_intfield(L,"isserif",                   sf->isserif);
  dump_intfield(L,"issans",                    sf->issans);
  dump_intfield(L,"encodingchanged",           sf->encodingchanged);
  dump_intfield(L,"strokedfont",               sf->strokedfont);
  dump_intfield(L,"use_typo_metrics",          sf->use_typo_metrics);
  dump_intfield(L,"weight_width_slope_only",   sf->weight_width_slope_only);
  dump_intfield(L,"head_optimized_for_cleartype",sf->head_optimized_for_cleartype);

  dump_enumfield(L,"uni_interp",               (sf->uni_interp+1), uni_interp_enum);
  
  if (sf->map != NULL ) {
    lua_newtable(L);
    handle_encmap(L,sf->map, l);
    lua_setfield(L,-2,"map");
  }

  dump_stringfield(L,"origname",        sf->origname); /* new */

  if (sf->private != NULL) {
    lua_newtable(L);
    handle_psdict(L, sf->private);
    lua_setfield(L,-2,"private");
  }
  
  dump_stringfield(L,"xuid",    sf->xuid);
  
  lua_createtable(L,0,40);
  handle_pfminfo(L,sf->pfminfo);
  lua_setfield(L,-2,"pfminfo");
  
  if (sf->names != NULL) {
    lua_newtable(L);
    handle_ttflangname(L,sf->names);
    lua_setfield(L,-2,"names");
  }
  
  lua_createtable(L,0,4);
  dump_stringfield(L,"registry",    sf->cidregistry);
  dump_stringfield(L,"ordering",    sf->ordering);
  dump_intfield   (L,"version",     sf->cidversion);
  dump_intfield   (L,"supplement",  sf->supplement);
  lua_setfield(L,-2,"cidinfo");
  
  /* SplineFont *cidmaster */ /* parent in a subfont */
  if (sf->subfontcnt>0) {
    lua_createtable(L,sf->subfontcnt,0);
    for (k=0;k<sf->subfontcnt;k++) {
	  lua_checkstack(L,10);
	  lua_newtable(L);
      handle_splinefont(L,sf->subfonts[k]);
      lua_rawseti(L,-2,(k+1));
    }
    lua_setfield(L,-2,"subfonts");
  }

  dump_stringfield(L,"comments",    sf->comments);
  dump_stringfield(L,"fontlog",    sf->fontlog);

  if (sf->cvt_names != NULL) {
    lua_newtable(L);
    for (k=0; sf->cvt_names[k]!=END_CVT_NAMES; ++k) {
      lua_pushstring(L, sf->cvt_names[k]);
      lua_rawseti(L,-2,(k+1));
    }
    lua_setfield(L,-2,"cvt_names");
  }

  if (sf->ttf_tables != NULL) {
    lua_newtable(L);
    handle_ttf_table(L,sf->ttf_tables);
    lua_setfield(L,-2,"ttf_tables");
  }

  if (sf->ttf_tab_saved != NULL) {
    lua_newtable(L);
    handle_ttf_table(L,sf->ttf_tab_saved);
    lua_setfield(L,-2,"ttf_tab_saved");
  }

  if (sf->texdata.type != tex_unset) {
    lua_newtable(L);
    dump_enumfield(L,"type",  sf->texdata.type, tex_type_enum);
    lua_newtable(L);
    for (k=0;k<22;k++) {
      lua_pushnumber(L,k);
      lua_pushnumber(L,sf->texdata.params[k]);
      lua_rawset(L,-3);
    }
    lua_setfield(L,-2,"params");
    lua_setfield(L,-2,"texdata");
  }
  if (sf->anchor != NULL) {
    lua_newtable(L);
    handle_anchorclass(L,sf->anchor);
    lua_setfield(L,-2,"anchor_classes");
  }
  if (sf->kerns != NULL) { 
    lua_newtable(L);
    handle_kernclass(L,sf->kerns);
    lua_setfield(L,-2,"kerns");
  }
  if (sf->vkerns != NULL) {
    lua_newtable(L);
    handle_kernclass(L,sf->vkerns);
    lua_setfield(L,-2,"vkerns");
  }
  if (sf->gsub_lookups != NULL) {
    lua_newtable(L);
    handle_lookup(L,sf->gsub_lookups);
    lua_setfield(L,-2,"gsub");
  }
  if (sf->gpos_lookups != NULL) {
    lua_newtable(L);
    handle_lookup(L,sf->gpos_lookups);
    lua_setfield(L,-2,"gpos");
  }

  if (sf->sm != NULL) {
    lua_newtable(L);
    handle_generic_asm(L,sf->sm);
    lua_setfield(L,-2,"sm");
  }
  if (sf->features != NULL) {
    lua_newtable(L);
    handle_macfeat(L,sf->features);
    lua_setfield(L,-2,"features");
  }  
  if (sf->mm != NULL) {
    lua_newtable(L);
    handle_mmset (L,sf->mm);
    lua_setfield(L,-2,"mm");
  }
  dump_stringfield(L,"chosenname",    sf->chosenname);
  
  dump_intfield(L,"macstyle",    sf->macstyle);
  dump_stringfield(L,"fondname",    sf->fondname);
  
  dump_intfield(L,"design_size",     sf->design_size);
  dump_intfield(L,"fontstyle_id",     sf->fontstyle_id);
  
  if (sf->fontstyle_name != NULL) {
    lua_newtable(L);
    handle_otfname(L,sf->fontstyle_name);
    lua_setfield(L,-2,"fontstyle_name");
  }
   
  dump_intfield(L,"design_range_bottom",sf->design_range_bottom);
  dump_intfield(L,"design_range_top",   sf->design_range_top);
  dump_floatfield(L,"strokewidth",      sf->strokewidth);
  
  if (sf->mark_class_cnt>0) {
    lua_newtable(L);
    for ( k=0; k<sf->mark_class_cnt; ++k ) {
      lua_pushnumber(L,(k+1));
      lua_pushstring(L,sf->mark_classes[k]);
      lua_rawset(L,-3);
    }
    lua_setfield(L,-1,"mark_classes");

    lua_newtable(L);
    for ( k=0; k<sf->mark_class_cnt; ++k ) {
      lua_pushnumber(L,(k+1));
      lua_pushstring(L,sf->mark_class_names[k]);
      lua_rawset(L,-3);
    }
    lua_setfield(L,-1,"mark_class_names");
  }
  
  dump_intfield(L,"creationtime",     sf->creationtime);
  dump_intfield(L,"modificationtime", sf->modificationtime);

  dump_intfield(L,"os2_version",      sf->os2_version);
  dump_intfield(L,"sfd_version",     sf->sfd_version);

  if (sf->MATH != NULL) {
    lua_newtable(L);
    handle_MATH(L, sf->MATH);
    lua_setfield(L,-2,"math");
  }
  
  if (sf->loadvalidation_state != 0) {
    int val, st;
    lua_newtable(L);
    val = 1;
    st = sf->loadvalidation_state;
    if (st & lvs_bad_ps_fontname) { 
       lua_pushliteral(L, "bad_ps_fontname"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_glyph_table) { 
       lua_pushliteral(L, "bad_glyph_table"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_cff_table) { 
       lua_pushliteral(L, "bad_cff_table"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_metrics_table) { 
       lua_pushliteral(L, "bad_metrics_table"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_cmap_table) { 
       lua_pushliteral(L, "bad_cmap_table"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_bitmaps_table) { 
       lua_pushliteral(L, "bad_bitmaps_table"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_gx_table) { 
       lua_pushliteral(L, "bad_gx_table"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_ot_table) { 
       lua_pushliteral(L, "bad_ot_table"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_os2_version) { 
       lua_pushliteral(L, "bad_os2_version"); lua_rawseti(L,-2,val++); 
    }
    if (st & lvs_bad_sfnt_header) { 
       lua_pushliteral(L, "bad_sfnt_header"); lua_rawseti(L,-2,val++); 
    }
    lua_setfield(L,-2,"validation_state");
  }

  if (sf->horiz_base != NULL) {
    lua_newtable(L);
    handle_base(L, sf->horiz_base);
    lua_setfield(L,-2,"horiz_base");
  }
  if (sf->vert_base != NULL) {
    lua_newtable(L);
    handle_base(L, sf->vert_base);
    lua_setfield(L,-2,"vert_base");
  }
  dump_intfield(L,"extrema_bound",     sf->extrema_bound);
}

int 
ff_make_table (lua_State *L) {
  SplineFont *sf;
  sf = *(check_isfont(L,1));
  if (sf == NULL) {
    lua_pushboolean(L,0);
  } else {
    lua_createtable(L,0,60);
    handle_splinefont(L,sf);
  }
  return 1;
}

void do_ff_info (lua_State *L, SplineFont *sf) {
  lua_newtable(L);
  dump_stringfield(L,"familyname",      sf->familyname);
  dump_stringfield(L,"fontname",        sf->fontname);
  dump_stringfield(L,"fullname",        sf->fullname);
  dump_intfield   (L,"italicangle",     sf->italicangle);
  dump_stringfield(L,"version",         sf->version);
  dump_stringfield(L,"weight",          sf->weight);

}

static int 
ff_info (lua_State *L) {
  SplineFont *sf;
  FILE *l;
  int i;
  const char *fontname;
  int openflags = 1;
  fontname = luaL_checkstring(L,1);
  if (!strlen(fontname)) {
  	lua_pushfstring(L,"font loading failed: empty string given\n", fontname);
	lua_error(L);
	return 1;
  } 
  /* test fontname for existance */
  if ((l = fopen(fontname,"r"))) {
	fclose(l); 
  } else {
	lua_pushfstring(L,"font loading failed for %s (read error)\n", fontname);
	lua_error(L);
  }
  sf = ReadSplineFontInfo((char *)fontname,openflags);
  if (sf==NULL) {
    lua_pushfstring(L,"font loading failed for %s\n", fontname);
    lua_error(L);
  } else {
	if (sf->next != NULL) {
      SplineFont *sf_next;
	  i = 1;
	  lua_newtable(L);
	  while (sf) {
		do_ff_info(L, sf);
		lua_rawseti(L,-2,i);
		i++;
        sf_next = sf->next;
		SplineFontFree(sf);
		sf = sf_next;
	  }
	} else {
	  do_ff_info(L, sf);
	  SplineFontFree(sf);
	}	  
  }
  return 1;
}

static void ff_do_cff (SplineFont *sf, char *filename, unsigned char **buf, int *bufsiz) {
  FILE *f;
  int32 *bsizes = NULL;
  int flags = ps_flag_nocffsugar + ps_flag_nohints;
  EncMap *map;

  map = EncMap1to1(sf->glyphcnt);

  if(WriteTTFFont(filename, sf, ff_cff, bsizes, bf_none, flags, map, ly_fore)) {
    /* success */
    f = fopen(filename,"rb");
    readbinfile(f , buf, bufsiz);
    /*fprintf(stdout,"\n%s => CFF, size: %d\n", sf->filename, *bufsiz);*/
    fclose(f);
    return;
  } 
  /* errors */
  fprintf(stdout,"\n%s => CFF, failed\n", sf->filename);

}

/* exported for writecff.c */

int ff_createcff (char *file, unsigned char **buf, int *bufsiz) {
  SplineFont *sf;
  int k ;
  char s[] = "tempfile.cff";
  int openflags = 1;
  int notdefpos = 0;
  sf =  ReadSplineFont(file,openflags);
  if (sf) {
    /* this is not the best way. nicer to have no temp file at all */
    ff_do_cff(sf, s, buf,bufsiz);
	for (k=0;k<sf->glyphcnt;k++) {
	  if (sf->glyphs[k] && strcmp(sf->glyphs[k]->name,".notdef")==0) {
		notdefpos=k;
		break;
	  }
	}
    remove(s);
	SplineFontFree(sf);
  }
  return notdefpos;
}

int ff_get_ttc_index(char *ffname, char*psname) {
  SplineFont *sf;
  int i = 0;
  int openflags = 1;
  int index = 0;

  sf = ReadSplineFontInfo((char *)ffname,openflags);
  if (sf==NULL) {
    perror("font loading failed unexpectedly\n");
    exit(EXIT_FAILURE);
  } 
  while (sf != NULL) {
	if (strcmp(sf->fontname,psname)==0) {
	  index = i;
	  break;
	}
	i++;
	sf = sf->next;
  }
  return index;
}

static int warning_given = 0;

static int 
ffold_info (lua_State *L) {
    if (warning_given<5) {
        fprintf (stderr, "Warning: The 'fontforge' table has been renamed to 'fontloader'.\n");
        fprintf (stderr, "  Please update your source file(s) accordingly.\n");
        warning_given ++;
    }
    return ff_info(L);
}

static int 
ffold_open (lua_State *L) {
    fprintf (stderr, "Warning: The 'fontforge' table has been renamed to 'fontloader'.\n");
    fprintf (stderr, "  Please update your source file(s) accordingly.\n");
    return ff_open(L);
}


static struct luaL_reg fflib[] = {
  {"open", ffold_open},
  {"info", ffold_info},
  {"close", ff_close},
  {"apply_afmfile", ff_apply_afmfile},
  {"apply_featurefile", ff_apply_featurefile},
  {"to_table", ff_make_table},
  {NULL, NULL}
};

static struct luaL_reg fllib[] = {
  {"open", ff_open},
  {"info", ff_info},
  {"close", ff_close},
  {"apply_afmfile", ff_apply_afmfile},
  {"apply_featurefile", ff_apply_featurefile},
  {"to_table", ff_make_table},
  {NULL, NULL}
};

static const struct luaL_reg fflib_m [] = {
  {"__gc", ff_close }, /* doesnt work yet! */
  {NULL, NULL}  /* sentinel */
};

extern char *SaveTablesPref;
extern char *coord_sep ;

int luaopen_ff (lua_State *L) {
  InitSimpleStuff();
  setlocale(LC_ALL,"C"); /* undo whatever InitSimpleStuff has caused */
  coord_sep = ",";
  FF_SetUiInterface(&luaui_interface);
  default_encoding = FindOrMakeEncoding("ISO8859-1");
  SaveTablesPref = "VORG,JSTF,acnt,bsln,fdsc,fmtx,hsty,just,trak,Zapf,LINO";
  luaL_newmetatable(L,FONT_METATABLE);
  luaL_register(L, NULL, fflib_m);
  luaL_openlib(L, "fontforge", fflib, 0);
  luaL_openlib(L, "fontloader", fllib, 0);
  return 1;
}

