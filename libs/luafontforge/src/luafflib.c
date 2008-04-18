/**
*  $Id $
*  @desc Support interface for fontforge 20070607
*  @version  1.0
*  @author Taco Hoekwater
*/

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>

#include "pfaedit.h"

extern void  LoadPrefs(void);

extern char **gww_errors;
extern int gww_error_count;
extern void gwwv_errors_free (void);

#define FONT_METATABLE "fontforge.splinefont"

#define LUA_OTF_VERSION "0.2"

char *possub_type_enum[] = { 
  "null", "position", "pair",  "substitution", 
  "alternate", "multiple", "ligature", "lcaret",  
  "kerning", "vkerning", "anchors", "contextpos", 
  "contextsub", "chainpos", "chainsub","reversesub", 
   "max", "kernback", "vkernback"};

#define LAST_POSSUB_TYPE_ENUM 18

#define eight_nulls() NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL

char *otf_lookup_type_enum[] = { 
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


char *anchor_type_enum[] = { "mark", "basechar", "baselig", "basemark", "centry", "cexit", "max", NULL };
#define MAX_ANCHOR_TYPE 7
char *anchorclass_type_enum[] = { "mark", "mkmk", "curs", "mklg", NULL };
char *glyph_class_enum[] = { "automatic", "none" ,"base", "ligature","mark", "component", NULL };


#define check_isfont(L,b) (SplineFont **)luaL_checkudata(L,b,FONT_METATABLE)

void handle_generic_pst (lua_State *L, struct generic_pst *pst);  /* forward */
void handle_generic_fpst (lua_State *L, struct generic_fpst *fpst);  /* forward */
void handle_kernclass (lua_State *L, struct kernclass *kerns);
void handle_anchorclass (lua_State *L, struct anchorclass *anchor);


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
  if (args>=2) {
	if (lua_isstring(L,2)) {
	  if (*(fontname+strlen(fontname))!=')') {
		/* possibly fails for embedded parens in the font name */
		snprintf(s,511,"%s(%s)", fontname, lua_tolstring(L,2,&len));
		if (len==0) {
		  snprintf(s,511,"%s", fontname);
		}
	  }
	}
  } else {
	snprintf(s,511,"%s", fontname);
  }
  if (strlen(s)>0) {
	sf = ReadSplineFont((char *)s,openflags);
	if (sf==NULL) {
	  lua_pushfstring(L,"font loading failed for %s\n", fontname);
	  lua_error(L);
	} else {
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

  /* generic_asm *sm; */ /* TODO, Apple state machine */

  /* int subtable_offset; */ /* used by OTF file generation */
  /* int32 *extra_subtables; */ /* used by OTF file generation */
}

/* subtables have to be dumped as an array because the ordering
 *  has to be preserved 
 */

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
  /* dump_intfield   (L,"def_lang_checked", lookup->def_lang_checked); */
  /* dump_intfield   (L,"def_lang_found",   lookup->def_lang_found); */
  /* dump_intfield   (L,"subcnt",           lookup->subcnt); */
  /* dump_intfield   (L,"index",     lookup->lookup_index); */ /* identical to array index */
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
  if (kp->subtable!=NULL) {
    dump_stringfield(L,"lookup",kp->subtable->subtable_name);
  }
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
      
  /*dump_intfield(L,"macfeature",        pst->macfeature); */

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

/*
if glyph.anchor then
            local t = { }
            for k,v in ipairs(glyph.anchor) do
                if not t[v.type] then t[v.type] = { } end
                t[v.type][v.anchor] = v
                v.anchor = nil
                v.type = nil
            end
            glyph.anchor = t
        end
*/

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
    /* dump_intfield(L,"lig_index",     anchor->lig_index); */
    lua_rawseti(L,-2,(anchor->lig_index+1));  
    lua_pop(L,1);  
  } else {
    lua_pushstring(L,anchor->anchor->name);
    lua_newtable(L);
    dump_intfield(L,"x",      anchor->me.x);
    dump_intfield(L,"y",      anchor->me.y);
  
    if (anchor->has_ttf_pt)
      dump_intfield(L,"ttf_pt_index",  anchor->ttf_pt_index);
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
handle_splinechar (lua_State *L,struct splinechar *glyph, int hasvmetrics) {
  
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

  dump_cond_intfield(L,"lsidebearing",   glyph->lsidebearing); 
  
  /* Layer layers[2];	*/	/* TH Not used */
  /*  int layer_cnt;    */	/* TH Not used */
  /*  StemInfo *hstem;  */	/* TH Not used */
  /*  StemInfo *vstem;	*/	/* TH Not used */
  /*  DStemInfo *dstem;	*/	/* TH Not used */
 
  /* MinimumDistance *md; */    /* TH Not used */
  /* struct charview *views; */ /* TH Not used */
  /* struct charinfo *charinfo;  */ /* TH ? (charinfo.c) */

  /*  struct splinefont *parent; */  /* TH Not used */

  /*

  */
  if (glyph->glyph_class>0) {
    dump_enumfield(L,"class",              glyph->glyph_class, glyph_class_enum);  
  }
  /* TH: internal fontforge stuff
     dump_intfield(L,"ticked",                   glyph->ticked);
     dump_intfield(L,"widthset",                 glyph->widthset); 
     dump_intfield(L,"ttf_glyph",                glyph->ttf_glyph); 
     dump_intfield(L,"changed",                  glyph->changed); 
     dump_intfield(L,"changedsincelasthinted",   glyph->changedsincelasthinted); 
     dump_intfield(L,"manualhints",              glyph->manualhints); 
     dump_intfield(L,"changed_since_autosave",   glyph->changed_since_autosave); 
     dump_intfield(L,"vconflicts",               glyph->vconflicts); 
     dump_intfield(L,"hconflicts",               glyph->hconflicts); 
     dump_intfield(L,"anyflexes",                glyph->anyflexes); 
     dump_intfield(L,"searcherdummy",            glyph->searcherdummy); 
     dump_intfield(L,"changed_since_search",     glyph->changed_since_search); 
     dump_intfield(L,"wasopen",                  glyph->wasopen); 
     dump_intfield(L,"namechanged",              glyph->namechanged); 
     dump_intfield(L,"blended",                  glyph->blended); 
     dump_intfield(L,"unused_so_far",            glyph->unused_so_far); 
     dump_intfield(L,"numberpointsbackards",     glyph->numberpointsbackards);  
     dump_intfield(L,"instructions_out_of_date", glyph->instructions_out_of_date);  
     dump_intfield(L,"complained_about_ptnums",  glyph->complained_about_ptnums);

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
 
  if (glyph->tex_height != TEX_UNDEF)
    dump_intfield(L,"tex_height",              glyph->tex_height);  
  if (glyph->tex_depth != TEX_UNDEF)
    dump_intfield(L,"tex_depth",               glyph->tex_depth);  
  if (glyph->tex_sub_pos != TEX_UNDEF)
    dump_intfield(L,"tex_sub_pos",             glyph->tex_sub_pos);  
  if (glyph->tex_super_pos != TEX_UNDEF)
    dump_intfield(L,"tex_super_pos",           glyph->tex_super_pos);  
    
  /*  struct altuni { struct altuni *next; int unienc; } *altuni; */
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

  /* TH: iconv internal information, ignorable */
  /* iconv_t *tounicode; */
  /* iconv_t *fromunicode; */
  /* int (*tounicode_func)(int); */
  /* int (*fromunicode_func)(int); */

  /*dump_intfield  (L,"is_temporary", enc->is_temporary);*/
  dump_intfield  (L,"char_max", enc->char_max);

}
      
void 
handle_enc (lua_State *L, struct enc *enc) {
  struct enc *next;
  NESTED_TABLE(do_handle_enc,enc,24);
}

void 
handle_encmap (lua_State *L, struct encmap *map) {
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
		lua_pushnumber(L,i);
		lua_pushnumber(L,map->map[i]);
		lua_rawset(L,-3);
      }
    }
    lua_setfield(L,-2,"map");
  }

  if (map->backmax > 0 && map->backmap != NULL) {
    lua_newtable(L);
    for (i=0;i<map->backmax;i++) {
      if (map->backmap[i]!=-1) {
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

char *ttfnames_enum[ttf_namemax] = { "copyright", "family", "subfamily", "uniqueid",
    "fullname", "version", "postscriptname", "trademark",
    "manufacturer", "designer", "descriptor", "venderurl",
    "designerurl", "license", "licenseurl", "idontknow",
    "preffamilyname", "prefmodifiers", "compatfull", "sampletext",
    "cidfindfontname"};

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
  if (anchor->subtable!=NULL) {
    dump_stringfield(L,"lookup",anchor->subtable->subtable_name);
  }
  dump_enumfield(L,"type",              anchor->type, anchorclass_type_enum);
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

  if (kerns->subtable != NULL) {
    dump_stringfield(L,"lookup", kerns->subtable->subtable_name);
  }
  /*dump_intfield(L,"kcid",            kerns->kcid); *//* probably not needed */

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



static char *fpossub_format_enum [] = { "glyphs", "class","coverage","reversecoverage"};

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

char *tex_type_enum[4] = { "unset", "text", "math", "mathext"};

/* has an offset of 1, ui_none = 0. */
char *uni_interp_enum[9] = {
  "unset", "none", "adobe", "greek", "japanese",
  "trad_chinese", "simp_chinese", "korean", "ams" };
	

void
handle_splinefont(lua_State *L, struct splinefont *sf) {
  int k,l;

  dump_stringfield(L,"table_version",   LUA_OTF_VERSION);
  dump_stringfield(L,"fontname",        sf->fontname);
  dump_stringfield(L,"fullname",        sf->fullname);
  dump_stringfield(L,"familyname",      sf->familyname);
  dump_stringfield(L,"weight",          sf->weight);
  dump_stringfield(L,"copyright",       sf->copyright);
  dump_stringfield(L,"filename",        sf->filename);
  dump_stringfield(L,"defbasefilename", sf->defbasefilename);
  dump_stringfield(L,"version",         sf->version);
  dump_floatfield (L,"italicangle",     sf->italicangle);
  dump_floatfield (L,"upos",            sf->upos);
  dump_floatfield (L,"uwidth",          sf->uwidth);
  dump_intfield   (L,"units_per_em",    sf->units_per_em);
  dump_intfield   (L,"ascent",          sf->ascent);
  dump_intfield   (L,"descent",         sf->descent);
  dump_intfield   (L,"vertical_origin", sf->vertical_origin);
  dump_intfield   (L,"uniqueid",        sf->uniqueid);
  dump_intfield   (L,"glyphcnt",        sf->glyphcnt);
  dump_intfield   (L,"glyphmax",        sf->glyphmax);

  if (sf->possub != NULL) {
    lua_newtable(L);
    handle_generic_fpst(L,sf->possub);
    lua_setfield(L,-2,"lookups");
  }

  lua_checkstack(L,4);
  lua_createtable(L,sf->glyphcnt,0);
  /* here is a bit of hackery for .notdef's that appear in the middle
  of a type1 font. This situation should be handled nicer, because the
  trick assumes a specific way of handling cff fonts, and that will
  fail. The current code will at least make sure that the intended use
  works, for now. */
  l = 0;
  for (k=0;k<sf->glyphcnt;k++,l++) {
    lua_pushnumber(L,l);
    lua_createtable(L,0,12);
	if (sf->glyphs[k]) {
	  if (k>0 && strcmp(sf->glyphs[k]->name,".notdef") == 0) {
		l--;
	  } else {
		handle_splinechar(L,sf->glyphs[k], sf->hasvmetrics);
	  }
	}
    lua_rawset(L,-3);
  }
  lua_setfield(L,-2,"glyphs");

  dump_intfield(L,"changed",                   sf->changed);
  dump_intfield(L,"hasvmetrics",               sf->hasvmetrics);
  dump_intfield(L,"order2",                    sf->order2);
  dump_intfield(L,"strokedfont",               sf->strokedfont);
  dump_intfield(L,"weight_width_slope_only",   sf->weight_width_slope_only);
  dump_intfield(L,"head_optimized_for_cleartype",sf->head_optimized_for_cleartype);
  dump_enumfield(L,"uni_interp",               (sf->uni_interp+1), uni_interp_enum);
  
  if (sf->map != NULL ) {
    lua_newtable(L);
    handle_encmap(L,sf->map);
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

  /* always the parent of a subfont, so why bother? */
  /*
	if (sf->cidmaster != NULL) {
      lua_pushstring(L, sf->cidmaster->origname);
      lua_setfield(L,-2,"cidmaster");
    }
  */
  
  dump_stringfield(L,"comments",    sf->comments);
  
  if (sf->ttf_tables != NULL) {
    lua_newtable(L);
    handle_ttf_table(L,sf->ttf_tables);
    lua_setfield(L,-2,"ttf_tables");
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

  /* Todo: a set of Apple-related items */
  /*
    ASM *sm;
    if (sf->features != NULL) {
      lua_newtable(L);
      handle_macfeat(L,sf->features);
      lua_setfield(L,-2,"features");
    }
    struct mmset *mm;
  */ 
  
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
  /*dump_intfield(L,"mark_class_cnt",     sf->mark_class_cnt);*/
  
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
  /* Grid-fitting And Scan-conversion Procedures: not needed */

  /*
    dump_intfield(L,"gasp_version",     sf->gasp_version);
    dump_intfield(L,"gasp_cnt",         sf->gasp_cnt);
    dump_intfield(L,"gasp",         (int)sf->gasp);
  */

  /* uint8 sfd_version;	*/		/* Used only when reading in an sfd file */
  /* struct gfi_data *fontinfo; */ /* TH: looks like this is screen-related */
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
	  i = 1;
	  lua_newtable(L);
	  while (sf) {
		do_ff_info(L, sf);
		lua_rawseti(L,-2,i);
		i++;
		SplineFontFree(sf);
		sf = sf->next;
	  }
	} else {
	  do_ff_info(L, sf);
	  SplineFontFree(sf);
	}	  
  }
  return 1;
}

/* possible flags:

   ttf_flag_shortps = 1, 
   ttf_flag_nohints = 2,
   ttf_flag_applemode=4,
   ttf_flag_pfed_comments=8, 
   ttf_flag_pfed_colors=0x10,
   ttf_flag_otmode=0x20,
   ttf_flag_glyphmap=0x40,
   ttf_flag_TeXtable=0x80,
   ttf_flag_ofm=0x100,
   ttf_flag_oldkern=0x200,	
   ttf_flag_brokensize=0x400	

   ps_flag_nohintsubs = 0x10000, 
   ps_flag_noflex=0x20000,
   ps_flag_nohints = 0x40000, 
   ps_flag_restrict256=0x80000,
   ps_flag_afm = 0x100000, 
   ps_flag_pfm = 0x200000,
   ps_flag_tfm = 0x400000,
   ps_flag_round = 0x800000,
   ps_flag_nocffsugar = 0x1000000,
   ps_flag_identitycidmap = 0x2000000,
   ps_flag_afmwithmarks = 0x4000000,
   ps_flag_noseac = 0x8000000,


*/

extern int readbinfile(FILE *f, unsigned char **b, int *s);

static void ff_do_cff (SplineFont *sf, char *filename, unsigned char **buf, int *bufsiz) {
  FILE *f;
  int32 *bsizes = NULL;
  int flags = ps_flag_nocffsugar + ps_flag_nohints;
  EncMap *map;

  map = EncMap1to1(sf->glyphcnt);

  if(WriteTTFFont(filename, sf, ff_cff, bsizes, bf_none, flags, map)) {
    /* success */
    f = fopen(filename,"rb");
    readbinfile(f , buf, bufsiz);
    /*fprintf(stdout,"\n%s => CFF, size: %d\n", sf->filename, *bufsiz);*/
    fclose(f);
    return;
  } 
  /* errors */
  /*fprintf(stdout,"\n%s => CFF, failed\n", sf->filename);*/

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


static int ff_make_cff (lua_State *L) {
  SplineFont **sf;
  char *s;
  unsigned char *buf = NULL;
  int bufsiz = 0;
  sf = check_isfont(L,1);
  s = (char *)luaL_checkstring(L,2);
  ff_do_cff (*sf, s, &buf, &bufsiz);
  return 0;
}


static struct luaL_reg fflib[] = {
  {"open", ff_open},
  {"info", ff_info},
  {"close", ff_close},
  {"apply_afmfile", ff_apply_afmfile},
  {"apply_featurefile", ff_apply_featurefile},
  {"to_table", ff_make_table},
  {"to_cff", ff_make_cff},
  {NULL, NULL}
};

static const struct luaL_reg fflib_m [] = {
  {"__gc", ff_close }, /* doesnt work yet! */
  {NULL, NULL}  /* sentinel */
};


int luaopen_ff (lua_State *L) {
  LoadPrefs();
  luaL_newmetatable(L,FONT_METATABLE);
  luaL_register(L, NULL, fflib_m);
  luaL_openlib(L, "fontforge", fflib, 0);
  return 1;
}

