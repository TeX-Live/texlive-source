/*372:*/
#line 7522 "hint.w"

#include "basetypes.h"
#include <string.h> 
#include <math.h> 
#include <zlib.h> 
#include "error.h"
#include "format.h"
#include "hrender.h"
#include "get.h"
#include "htex.h"
#include "hint.h"

/*82:*/
#line 1279 "hint.w"

#define HGET_STRING(S) S= (char*)hpos;\
 while(hpos<hend && *hpos!=0) { RNG("String character",*hpos,0x20,0x7E); hpos++;}\
 hpos++;
/*:82*//*83:*/
#line 1291 "hint.w"

#define HGET_XDIMEN(I,X) \
{ if((I)&b100) HGET32((X).w); else (X).w= 0;\
  if((I)&b010) (X).h= hget_float32();  else (X).h= 0.0;\
  if((I)&b001) (X).v= hget_float32(); else (X).v= 0.0;\
}
/*:83*//*87:*/
#line 1394 "hint.w"

#define HGET_STRETCH(F,O) { Stch _st;  HGET32(_st.u); (O)= _st.u&3;  _st.u&= ~3; (F)= (scaled)(_st.f*ONE); }
/*:87*//*89:*/
#line 1405 "hint.w"

#define HGET_GLYPH(I) \
{uint8_t f; uint32_t c;\
  if (I==1) c= HGET8;\
  else if (I==2) HGET16(c);\
  else if (I==3) HGET24(c);\
  else if (I==4) HGET32(c);\
  f= HGET8; REF_RNG(font_kind,f);\
  tail_append(new_character(f,c));\
}
/*:89*//*97:*/
#line 1487 "hint.w"

#define HGET_RULE(I)\
pointer p= new_rule();\
if ((I)&b100) HGET32(height(p)); else height(p)= null_flag;\
if ((I)&b010) HGET32(depth(p)); else depth(p)= null_flag;\
if ((I)&b001) HGET32(width(p)); else width(p)= null_flag;
/*:97*//*103:*/
#line 1549 "hint.w"

#define HGET_GLUE(I) \
  p=  get_node(glue_spec_size); \
  if((I)!=b111) { if ((I)&b100) HGET32(width(p)); else width(p)= 0; }\
  if((I)&b010) HGET_STRETCH(stretch(p),stretch_order(p)) else stretch(p)= 0, stretch_order(p)= normal;\
  if((I)&b001) HGET_STRETCH(shrink(p),shrink_order(p)) else shrink(p)= 0, shrink_order(p)= normal;\
  if(I==b111) width(p)= hget_xdimen_node();
/*:103*//*110:*/
#line 1669 "hint.w"

#define IS_LIST(X)  (KIND(X)==list_kind || \
        KIND(X)==text_kind  || KIND(X)==param_kind)
/*:110*//*115:*/
#line 1826 "hint.w"

#define HGET_BOX(I) \
p= new_null_box();\
HGET32(height(p));\
if ((I)&b001) HGET32(depth(p));\
HGET32(width(p));\
if ((I)&b010) HGET32(shift_amount(p));\
if ((I)&b100) {int8_t x; glue_set(p)= hget_float32();\
  x= HGET8;  glue_order(p)= x&0xF;\
  x= x>>4; glue_sign(p)= (x<0?shrinking:(x> 0?stretching:normal));}\
list_ptr(p)= hget_list_pointer();
/*:115*//*122:*/
#line 1950 "hint.w"

#define HGET_SET(I) \
 scaled x, st, sh; uint8_t sto, sho; \
 p= new_null_box();\
 HGET32(height(p)); if ((I)&b001) HGET32(depth(p)); HGET32(width(p));\
 if ((I)&b010) HGET32(shift_amount(p));\
 HGET_STRETCH(st,sto);   HGET_STRETCH(sh,sho);\
 if ((I)&b100) x=  hget_xdimen_node();  else x= hget_xdimen_ref(HGET8);\
 list_ptr(p)= hget_list_pointer();
/*:122*//*128:*/
#line 2093 "hint.w"

#define HGET_PACK(K,I) \
{ pointer p; scaled x, s= 0, d;  uint8_t m; \
 if ((I)&b001) m= additional; else m= exactly; \
 if ((I)&b010) HGET32(s);\
 if (K==vpack_kind) HGET32(d); \
 if ((I)&b100) x=  hget_xdimen_node();  else x= hget_xdimen_ref(HGET8);\
 p= hget_list_pointer(); \
 if (K==vpack_kind) p= vpackage(p,x,m,d); else p= hpack(p,x,m);\
 shift_amount(p)= s;\
 happend_to_vlist(p);}
/*:128*//*132:*/
#line 2160 "hint.w"

#define HGET_KERN(I) \
pointer p; scaled x; \
if (((I)&b011)==0) x= hget_dimen_ref(HGET8);\
else if (((I)&b011)==1) x= hget_xdimen_ref(HGET8);\
else if (((I)&b011)==2) HGET32(x);\
else if (((I)&b011)==3) x= hget_xdimen_node();\
p= new_kern(x);\
if ((I)&b100) subtype(p)= explicit;\
tail_append(p);
/*:132*//*136:*/
#line 2211 "hint.w"

#define HGET_LEADERS(I) \
{pointer p;\
 if ((I)&b100)p= hget_glue_node(); else {p= spec2glue(zero_glue); incr(glue_ref_count(zero_glue));} \
subtype(p)= a_leaders+((I)&b011)-1;\
if (KIND(*hpos)==rule_kind) leader_ptr(p)= hget_rule_node(); \
else if (KIND(*hpos)==hbox_kind) leader_ptr(p)= hget_hbox_node(); \
else  leader_ptr(p)= hget_vbox_node();\
tail_append(p);}
/*:136*//*140:*/
#line 2257 "hint.w"

#define HGET_BASELINE(I) \
  cur_list.bs_pos= hpos-1; \
  if((I)&b001) HGET32(cur_lsl); else cur_lsl= 0; \
  if((I)&b100) cur_bs= hget_glue_spec(); else cur_bs= zero_glue; \
  if((I)&b010) cur_ls= hget_glue_spec(); else cur_ls= zero_glue;
/*:140*//*145:*/
#line 2311 "hint.w"

#define HGET_LIG(I) \
{pointer p,q;uint8_t f;\
f= HGET8;\
if ((I)==7) q= hget_list_pointer(); else q= hget_text_list(I);\
if (q==null) QUIT("Ligature with empty list");\
p= new_ligature(f, character(q), link(q)); tail_append(p);\
link(q)= null; flush_node_list(q);\
}
/*:145*//*149:*/
#line 2363 "hint.w"

#define HGET_DISC(I)\
  pointer p= new_disc(); \
  if ((I)&b100) {uint8_t r; r= HGET8; set_replace_count(p,r); \
                 if ((r&0x80)==0) set_auto_disc(p); }\
  else  set_auto_disc(p); \
  if ((I)&b010) pre_break(p)= hget_list_pointer(); \
  if ((I)&b001) post_break(p)= hget_list_pointer();
/*:149*//*155:*/
#line 2436 "hint.w"

#define HGET_PAR(I) \
{ scaled x= 0;\
  ParamDef *q;\
  if ((I)==b100) q= hget_param_list_ref(HGET8);\
  if ((I)&b100) x= hget_xdimen_node(); else x= hget_xdimen_ref(HGET8);\
  if ((I)&b010) q= hget_param_list_node(); \
  else if ((I)!=b100) q= hget_param_list_ref(HGET8);\
  hget_paragraph(x,0,q);\
}
/*:155*//*172:*/
#line 2921 "hint.w"

#define HGET_MATH(I) \
{ ParamDef *q; pointer p= null, a= null;\
if ((I)&b100) q= hget_param_list_node(); else q= hget_param_list_ref(HGET8);\
if ((I)&b010) a= hget_hbox_node(); \
p= hget_list_pointer(); \
if ((I)&b001) a= hget_hbox_node();\
hset_param_list(q); hdisplay(p,a,((I)&b010)!=0); hrestore_param_list();\
}
/*:172*//*178:*/
#line 2990 "hint.w"

#define HGET_ADJUST(I) \
{ pointer p;\
  p= get_node(small_node_size); type(p)= adjust_node; subtype(p)= normal;\
  adjust_ptr(p)= hget_list_pointer(); \
  tail_append(p);\
}
/*:178*//*180:*/
#line 3006 "hint.w"

#define HGET_TABLE(I) \
if(I&b010) ; else ;\
if ((I)&b001) ; else ;\
if ((I)&b100) hget_xdimen_node(); else hget_xdimen_ref(HGET8);\
hget_list_pointer();  \
hget_list_pointer(); 
/*:180*//*185:*/
#line 3102 "hint.w"

#define HGET_STREAM(I) \
{ ParamDef *q;  pointer p;\
  p= get_node(ins_node_size); type(p)= ins_node;\
  subtype(p)= HGET8;RNG("Stream",subtype(p),1,254); \
  if ((I)&b010) q= hget_param_list_node(); else q= hget_param_list_ref(HGET8); \
  ins_ptr(p)= hget_list_pointer(); \
  if ((I)&b010) hset_stream_params(p,true,q); else hset_stream_params(p,false,q);\
  tail_append(p);}
/*:185*//*189:*/
#line 3134 "hint.w"

#define HGET_IMAGE(I) \
{ pointer p;\
  p= get_node(image_node_size);  type(p)= whatsit_node; subtype(p)= image_node;\
  HGET16(image_no(p));RNG("Section number",image_no(p),3,max_section_no);  \
  if (I&b010) {HGET32(image_width(p)); HGET32(image_height(p));} \
  else image_width(p)= image_height(p)= 0;\
  if (I&b001) {HGET_STRETCH(image_stretch(p),image_stretch_order(p)); \
               HGET_STRETCH(image_shrink(p),image_shrink_order(p));}\
  else { image_stretch(p)= image_shrink(p)= 0;image_stretch_order(p)= image_shrink_order(p)= normal;}\
  tail_append(p);}
/*:189*//*193:*/
#line 3176 "hint.w"

#define HGET_LINK(I) \
{ pointer p;\
  p= get_node(link_node_size);  type(p)= whatsit_node;\
  if (I&b010) subtype(p)= start_link_node; else subtype(p)= end_link_node;\
  if (I&b001) HGET16(label_ref(p)); else label_ref(p)= HGET8; \
  RNG("label",label_ref(p),0,max_ref[label_kind]);\
  label_has_name(p)= 0;\
  tail_append(p);}
/*:193*/
#line 7534 "hint.w"

/*84:*/
#line 1299 "hint.w"

#define HTEG_XDIMEN(I,X) \
  if((I)&b001) (X).v= hteg_float32(); else (X).v= 0.0;\
  if((I)&b010) (X).h= hteg_float32();  else (X).h= 0.0;\
  if((I)&b100) HTEG32((X).w); else (X).w= 0;\
/*:84*//*88:*/
#line 1397 "hint.w"

#define HTEG_STRETCH(F,O) { Stch _st;  HTEG32(_st.u); (O)= _st.u&3;  _st.u&= ~3; (F)= (scaled)(_st.f*ONE); }
/*:88*//*90:*/
#line 1417 "hint.w"

#define HTEG_GLYPH(I) \
{uint8_t f; uint32_t c;\
  f= HTEG8; REF_RNG(font_kind,f);\
  if (I==1) c= HTEG8;\
  else if (I==2) HTEG16(c);\
  else if (I==3) HTEG24(c);\
  else if (I==4) HTEG32(c);\
  tail_append(new_character(f,c));\
}
/*:90*//*98:*/
#line 1496 "hint.w"

#define HTEG_RULE(I)\
pointer p= new_rule();\
if ((I)&b001) HTEG32(width(p)); else width(p)= null_flag;\
if ((I)&b010) HTEG32(depth(p)); else depth(p)= null_flag;\
if ((I)&b100) HTEG32(height(p)); else height(p)= null_flag;
/*:98*//*105:*/
#line 1562 "hint.w"

#define HTEG_GLUE(I) \
  p=  get_node(glue_spec_size); \
  if(I==b111) width(p)= hget_xdimen_node();\
  if((I)&b001) HTEG_STRETCH(shrink(p),shrink_order(p)) else shrink(p)= 0, shrink_order(p)= normal;\
  if((I)&b010) HTEG_STRETCH(stretch(p),stretch_order(p)) else stretch(p)= 0, stretch_order(p)= normal;\
  if((I)!=b111) { if ((I)&b100) HGET32(width(p)); else width(p)= 0; }
/*:105*//*111:*/
#line 1673 "hint.w"

#define IS_LIST(X)  (KIND(X)==list_kind || \
        KIND(X)==text_kind  || KIND(X)==param_kind)
/*:111*//*116:*/
#line 1839 "hint.w"

#define HTEG_BOX(I) \
p= new_null_box();\
list_ptr(p)= hteg_list_pointer();\
if ((I)&b100) {int8_t x= HTEG8; glue_order(p)= x&0xF;\
   x= x>>4; glue_sign(p)= (x<0?shrinking:(x> 0?stretching:normal));\
   glue_set(p)= hteg_float32(); }\
if ((I)&b010) HTEG32(shift_amount(p));\
HTEG32(width(p));\
if ((I)&b001) HTEG32(depth(p));\
HTEG32(height(p));\
node_pos= hpos-hstart-1;
/*:116*//*123:*/
#line 1962 "hint.w"

#define HTEG_SET(I) \
  scaled x, st, sh; uint8_t sto, sho; \
  p= new_null_box();\
  list_ptr(p)= hteg_list_pointer();\
  if ((I)&b100) x= hteg_xdimen_node(); else x= hget_xdimen_ref(HTEG8);\
  HTEG_STRETCH(sh,sho);HTEG_STRETCH(st,sto);\
  if ((I)&b010) HTEG32(shift_amount(p)); \
  HTEG32(width(p));if ((I)&b001) HTEG32(depth(p));HTEG32(height(p)); \
  node_pos= hpos-hstart-1;
/*:123*//*129:*/
#line 2106 "hint.w"

#define HTEG_PACK(K,I) \
{ pointer p; scaled x, s, d;  uint8_t m; \
 p= hteg_list_pointer();\
 if ((I)&b100) x= hteg_xdimen_node();  else x= hget_xdimen_ref(HTEG8);\
 if (K==vpack_kind) HTEG32(d); \
 if ((I)&b010) HTEG32(s);\
 if ((I)&b001) m= additional; else m= exactly; \
 node_pos= hpos-hstart-1;\
 if (K==vpack_kind) p= vpackage(p,x,m,d); else p= hpack(p,x,m);\
 hprepend_to_vlist(p);}
/*:129*//*133:*/
#line 2173 "hint.w"

#define HTEG_KERN(I) \
pointer p; scaled x; \
if (((I)&b011)==0) x= hget_dimen_ref(HTEG8);\
else if (((I)&b011)==1) x= hget_xdimen_ref(HTEG8);\
else if (((I)&b011)==2) HTEG32(x);\
else if (((I)&b011)==3) x= hteg_xdimen_node();\
p= new_kern(x);\
if ((I)&b100) subtype(p)= explicit;\
tail_append(p);
/*:133*//*137:*/
#line 2222 "hint.w"

#define HTEG_LEADERS(I) \
{pointer p,q;\
if (KIND(*(hpos-1))==rule_kind) q= hteg_rule_node(); \
else if (KIND(*(hpos-1))==hbox_kind) q= hteg_hbox_node(); \
else  q= hteg_vbox_node();\
if ((I)&b100) p= hteg_glue_node(); else {p= spec2glue(zero_glue); incr(glue_ref_count(zero_glue));} \
leader_ptr(p)= q;subtype(p)= a_leaders+((I)&b011)-1;\
tail_append(p);}
/*:137*//*141:*/
#line 2265 "hint.w"

#define HTEG_BASELINE(I) \
  if((I)&b010) cur_ls= hteg_glue_spec(); else cur_ls= zero_glue; \
  if((I)&b100) cur_bs= hteg_glue_spec(); else cur_bs= zero_glue; \
  if((I)&b001) HTEG32(cur_lsl); else cur_lsl= 0; \
  cur_list.bs_pos= hpos-1;
/*:141*//*146:*/
#line 2322 "hint.w"

#define HTEG_LIG(I) \
{pointer p,q;\
if ((I)==7) q= hteg_list_pointer();\
else {uint8_t *t= hpos; hpos= t-I; q= hget_text_list(I); hpos= t-I;}\
if (q==null) QUIT("Ligature with empty list");\
p= new_ligature(0, character(q), link(q)); tail_append(p);\
link(q)= null; flush_node_list(q);\
font(lig_char(p))= HTEG8;\
}
/*:146*//*150:*/
#line 2373 "hint.w"

#define HTEG_DISC(I)\
  pointer p= new_disc(); \
  if ((I)&b001) post_break(p)= hteg_list_pointer(); \
  if ((I)&b010) pre_break(p)= hteg_list_pointer(); \
  if ((I)&b100) {uint8_t r; r= HTEG8; set_replace_count(p,r); \
                 if ((r&0x80)==0) set_auto_disc(p); }\
  else  set_auto_disc(p);
/*:150*//*173:*/
#line 2932 "hint.w"

#define HTEG_MATH(I) \
{ ParamDef *q; pointer p= null, a= null;\
if ((I)&b001) a= hteg_hbox_node();\
p= hteg_list_pointer(); \
if ((I)&b010) a= hteg_hbox_node(); \
if ((I)&b100) q= hteg_param_list_node(); else q= hget_param_list_ref(HTEG8);\
hset_param_list(q); hdisplay(p,a,((I)&b010)!=0); hrestore_param_list();\
}
/*:173*//*181:*/
#line 3015 "hint.w"

#define HTEG_TABLE(I) \
if(I&b010) ; else ;\
if ((I)&b001) ; else ;\
hteg_list_pointer();   \
hteg_list_pointer();  \
if ((I)&b100) hteg_xdimen_node(); else hget_xdimen_ref(HTEG8);
/*:181*//*186:*/
#line 3113 "hint.w"

#define HTEG_STREAM(I) \
{pointer p= get_node(ins_node_size); type(p)= ins_node;\
 ins_ptr(p)= hteg_list_pointer();\
 if ((I)&b010) {ParamDef *q= hteg_param_list_node();  hset_stream_params(p,true,q);}\
 else {ParamDef *q= hget_param_list_ref(HTEG8);  hset_stream_params(p,false,q);}\
 subtype(p)= HTEG8;RNG("Stream",subtype(p),1,254);\
 tail_append(p);}
/*:186*//*190:*/
#line 3147 "hint.w"

#define HTEG_IMAGE(I) \
{ pointer p;\
  p= get_node(image_node_size);  type(p)= whatsit_node; subtype(p)= image_node;\
  if (I&b001) {HTEG_STRETCH(image_shrink(p),image_shrink_order(p));\
               HTEG_STRETCH(image_stretch(p),image_stretch_order(p)); }\
  else { image_stretch(p)= image_shrink(p)= 0;image_stretch_order(p)= image_shrink_order(p)= normal;}\
  if (I&b010) {HTEG32(image_height(p)); HTEG32(image_width(p)); } \
  else image_width(p)= image_height(p)= 0;\
  HTEG16(image_no(p));RNG("Section number",image_no(p),3,max_section_no);  \
  tail_append(p);}
/*:190*//*194:*/
#line 3187 "hint.w"

#define HTEG_LINK(I) \
{ pointer p;\
  p= get_node(link_node_size);  type(p)= whatsit_node;\
  if (I&b010) subtype(p)= start_link_node; else subtype(p)= end_link_node;\
  if (I&b001) HTEG16(label_ref(p)); else label_ref(p)= HTEG8; \
  RNG("label",label_ref(p),0,max_ref[label_kind]);\
  label_has_name(p)= 0;\
  tail_append(p);}
/*:194*/
#line 7535 "hint.w"


/*21:*/
#line 366 "hint.w"

typedef struct{pointer bs,ls;scaled lsl;}BaselineSkip;
/*:21*//*28:*/
#line 476 "hint.w"

typedef struct{
char*n;
uint16_t m,q;
scaled s;
pointer g;
pointer h;
pointer p[MAX_FONT_PARAMS+1];
}FontDef;
extern FontDef*font_def;
/*:28*//*37:*/
#line 581 "hint.w"

typedef struct{
uint8_t n,k;
int32_t v;
}Param;

typedef struct ParamDef{
struct ParamDef*next;
Param p;}ParamDef;
/*:37*/
#line 7537 "hint.w"



/*2:*/
#line 191 "hint.w"

pointer*pointer_def[32]= {NULL};
/*:2*//*6:*/
#line 240 "hint.w"

int32_t*integer_def;
/*:6*//*10:*/
#line 270 "hint.w"

scaled*dimen_def;
/*:10*//*14:*/
#line 300 "hint.w"

Xdimen*xdimen_def;
/*:14*//*22:*/
#line 370 "hint.w"

BaselineSkip*baseline_def= NULL;
/*:22*//*29:*/
#line 488 "hint.w"

FontDef*font_def;
/*:29*//*38:*/
#line 593 "hint.w"

ParamDef**param_def;
/*:38*//*45:*/
#line 733 "hint.w"

typedef struct{
uint8_t pg;
uint32_t f,t;
}RangeDef;
RangeDef*range_def;
/*:45*//*50:*/
#line 790 "hint.w"

Stream*streams;
/*:50*//*53:*/
#line 804 "hint.w"

typedef struct{
Xdimen x;
int f;
int p,n,r;
pointer b,a;
Xdimen w;
pointer g;
pointer h;
}StreamDef;
/*:53*//*57:*/
#line 877 "hint.w"

typedef struct{
char*n;
Dimen d;
pointer g;
uint8_t p;
uint32_t t;
Xdimen v,h;
StreamDef*s;
}PageDef;
PageDef*page_def;
PageDef*cur_page;
/*:57*//*65:*/
#line 1021 "hint.w"

hint_Outline*hint_outlines= NULL;
int outline_no= -1;
/*:65*//*162:*/
#line 2762 "hint.w"

static ParamDef*line_break_params= NULL;
/*:162*//*205:*/
#line 3487 "hint.w"

static scaled page_height;
static scaled top_so_far[8];
/*:205*//*220:*/
#line 3765 "hint.w"

static uint32_t map[0x10000];
/*:220*//*225:*/
#line 3829 "hint.w"

#define MAX_PAGE_POS (1<<3) 

uint64_t page_loc[MAX_PAGE_POS];
int cur_loc;
static int lo_loc,hi_loc;
/*:225*//*242:*/
#line 4221 "hint.w"

scaled hvsize,hhsize;
/*:242*//*244:*/
#line 4249 "hint.w"

int page_v,page_h,offset_v,offset_h;
/*:244*//*301:*/
#line 5523 "hint.w"

hint_Link*hint_links= NULL;
int max_link= -1;
/*:301*//*359:*/
#line 7223 "hint.w"

jmp_buf error_exit;
char herror_string[MAX_HERROR];
/*:359*/
#line 7540 "hint.w"

/*3:*/
#line 196 "hint.w"

static void hget_font_def(uint8_t a,uint8_t n);
static int32_t hget_integer_def(uint8_t a);
static scaled hget_dimen_def(uint8_t a);
static pointer hget_glue_def(uint8_t a);
static void hget_baseline_def(uint8_t a,uint8_t n);
static ParamDef*hget_param_list(uint8_t a);
static void hget_range_def(uint8_t a,uint8_t pg);
static void hget_page_def(uint8_t a,uint8_t n);
static void hget_outline_or_label_def(Info i,int n);
static void hget_font_metrics();
static pointer hget_definition(uint8_t a);
/*:3*//*27:*/
#line 457 "hint.w"

static pointer hprepend_to_vlist(pointer b);
/*:27*//*36:*/
#line 569 "hint.w"

static pointer hget_glue_spec(void);
static pointer hget_disc_node(void);
/*:36*//*104:*/
#line 1558 "hint.w"

static scaled hget_xdimen_node(void);
/*:104*//*117:*/
#line 1855 "hint.w"

static pointer hget_list_pointer(void);
static pointer hteg_list_pointer(void);
/*:117*//*124:*/
#line 1973 "hint.w"

static scaled hget_xdimen_node(void);
/*:124*/
#line 7541 "hint.w"

/*9:*/
#line 253 "hint.w"

static int32_t hget_integer_def(uint8_t a)
{if(INFO(a)==1){int8_t n= HGET8;return n;}
else if(INFO(a)==2){int16_t n;HGET16(n);return n;}
else if(INFO(a)==4){int32_t n;HGET32(n);return n;}
else TAGERR(a);
return 0;
}

static int32_t hget_integer_ref(uint8_t n)
{REF_RNG(int_kind,n);
return integer_def[n];
}
/*:9*//*17:*/
#line 311 "hint.w"

static scaled xdimen(Xdimen*x)
{return round(x->w+(double)x->h*(double)hhsize+(double)x->v*(double)hvsize);
}
static scaled hget_xdimen_ref(uint8_t n)
{REF_RNG(xdimen_kind,n);
return xdimen(xdimen_def+n);
}
/*:17*//*19:*/
#line 331 "hint.w"


static pointer hget_glue_ref(uint8_t n)
{REF_RNG(glue_kind,n);
return pointer_def[glue_kind][n];
}

static pointer hget_glue_def(uint8_t a)
{pointer p;
if(INFO(a)==b000)
{p= hget_glue_ref(HGET8);
add_glue_ref(p);
}
else
{HGET_GLUE(INFO(a));}
return p;
}

pointer hget_param_glue(uint8_t n)
{REF_RNG(glue_kind,n);
return new_glue(pointer_def[glue_kind][n]);
}
/*:19*//*32:*/
#line 501 "hint.w"

static void hget_font_def(uint8_t a,uint8_t n)
{char*t;
FontDef*f= font_def+n;
HGET_STRING(t);f->n= strdup(t);
DBG(DBGDEF,"Font %d: %s\n",n,t);
HGET32(f->s);RNG("Font size",f->s,1,0x7fffffff);
HGET16(f->m);RNG("Font metrics",f->m,3,max_section_no);
HGET16(f->q);RNG("Font glyphs",f->q,3,max_section_no);
f->g= hget_glue_spec();
f->h= hget_disc_node();
DBG(DBGDEF,"Start font parameters\n");
while(KIND(*hpos)!=font_kind)
{Kind k;
uint8_t n;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 516 "hint.w"

k= KIND(a);
n= HGET8;
DBG(DBGDEF,"Reading font parameter %d: %s\n",n,definition_name[k]);
if(k!=penalty_kind&&k!=kern_kind&&k!=ligature_kind&&
k!=disc_kind&&k!=glue_kind&&k!=language_kind&&k!=rule_kind&&k!=image_kind)
QUIT("Font parameter %d has invalid type %s",n,content_name[n]);
RNG("Font parameter",n,0,MAX_FONT_PARAMS);
f->p[n]= hget_definition(a);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 525 "hint.w"

}
DBG(DBGDEF,"End font definition\n");
}
/*:32*//*33:*/
#line 534 "hint.w"

static void hget_font_metrics(void)
{int i;
for(i= 0;i<=max_ref[font_kind];i++)
if(font_def[i].m!=0)
{int s;
hget_section(font_def[i].m);
s= font_def[i].s;
if(s==0)s= -1000;
read_font_info(i,font_def[i].n,s);
font_def[i].s= font_size[i];
}
}
/*:33*//*41:*/
#line 610 "hint.w"

static void free_param_list(ParamDef*p)
{while(p!=NULL)
{ParamDef*q= p;
p= p->next;
free(q);
}
}
/*:41*//*42:*/
#line 622 "hint.w"

static ParamDef*hget_param_list(uint8_t a)
{uint32_t s,t;
ParamDef*p= NULL;
uint8_t*list_start,*list_end;
list_start= hpos;
s= hget_list_size(INFO(a));
hget_size_boundary(INFO(a));
list_end= hpos+s;
if(list_end>=hend)
QUIT("list end after before stream end\n");
while(hpos<list_end)
{ParamDef*r;Param*q;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 635 "hint.w"

ALLOCATE(r,1,ParamDef);
q= &(r->p);
q->n= HGET8;
q->k= KIND(a);
DBG(DBGTAGS,"Defining %s %d\n",definition_name[KIND(a)],q->n);
if(KIND(a)==int_kind)q->v= hget_integer_def(a);
else if(KIND(a)==dimen_kind)q->v= hget_dimen_def(a);
else if(KIND(a)==glue_kind)q->v= hget_glue_def(a);
else TAGERR(a);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 645 "hint.w"

r->next= p;
p= r;
}
hget_size_boundary(INFO(a));
t= hget_list_size(INFO(a));
if(t!=s)
QUIT("List sizes at "SIZE_F" and "SIZE_F" do not match 0x%x != 0x%x",list_start-hstart,list_end-hstart,s,t);
return p;
}

ParamDef*hget_param_list_node(void)
{if(KIND(*hpos)!=param_kind)return NULL;
else
{ParamDef*p;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 660 "hint.w"

p= hget_param_list(a);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 662 "hint.w"

return p;
}
}

ParamDef*hget_param_list_ref(uint8_t n)
{REF_RNG(param_kind,n);
return param_def[n];
}
/*:42*//*43:*/
#line 684 "hint.w"

#define MAX_SAVE 100
#define SAVE_BOUNDARY 0xFF
static Param par_save[MAX_SAVE];
static int par_save_ptr= 0;

static void hset_param(uint8_t k,uint8_t n,int32_t v)
{Param*q;
if(par_save_ptr>=MAX_SAVE)QUIT("Parameter save stack overflow");
q= &(par_save[par_save_ptr++]);
q->k= k;
q->n= n;
if(q->k==int_kind)
{q->v= integer_def[q->n];integer_def[q->n]= v;}
else if(q->k==dimen_kind)
{q->v= dimen_def[q->n];dimen_def[q->n]= (scaled)v;}
else if(q->k==glue_kind)
{q->v= pointer_def[glue_kind][q->n];pointer_def[glue_kind][q->n]= (pointer)v;}
}

void hset_param_list(ParamDef*p)
{hset_param(SAVE_BOUNDARY,0,0);
while(p!=NULL)
{hset_param(p->p.k,p->p.n,p->p.v);
p= p->next;
}
}

void hrestore_param_list(void)
{
while(par_save_ptr> 0)
{Param*q;
q= &(par_save[--par_save_ptr]);
if(q->k==SAVE_BOUNDARY)return;
if(q->k==int_kind)
{integer_def[q->n]= q->v;}
else if(q->k==dimen_kind)
{dimen_def[q->n]= (scaled)q->v;}
else if(q->k==glue_kind)
{pointer_def[glue_kind][q->n]= (pointer)q->v;}
}
QUIT("Parameter save stack flow");
}
/*:43*//*48:*/
#line 748 "hint.w"

static void hget_range_def(uint8_t a,uint8_t pg)
{static uint8_t n= 0;
uint32_t f,t;
REF_RNG(page_kind,pg);
REF_RNG(range_kind,n);
if(INFO(a)&b100)
{if(INFO(a)&b001)HGET32(f);else HGET16(f);}
else f= 0;
if(INFO(a)&b010)
{if(INFO(a)&b001)HGET32(t);else HGET16(t);}
else t= HINT_NO_POS;
range_def[n].pg= pg;
range_def[n].f= f;
range_def[n].t= t;
DBG(DBGRANGE,"Range *%d from 0x%x\n",pg,f);
DBG(DBGRANGE,"Range *%d to 0x%x\n",pg,t);
n++;
}
#if 0

static uint8_t hget_page_ref(uint32_t pos)
{int i;
for(i= 1;i<=max_ref[range_kind];i++)
if(range_def[i].f<=pos&&pos<range_def[i].t)return range_def[i].pg;
return 0;
}
#endif
/*:48*//*54:*/
#line 821 "hint.w"

static void hget_xdimen_def_node(Xdimen*x);

static bool hget_stream_def(StreamDef*s)
{if(KIND(*hpos)!=stream_kind||!(INFO(*hpos)&b100))
return false;
else
{uint8_t n;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 829 "hint.w"

DBG(DBGDEF,"Defining stream %d at "SIZE_F"\n",*hpos,hpos-hstart-1);
n= HGET8;REF_RNG(stream_kind,n);
s= s+n;
if(n> 0)
{if(INFO(a)==b100)/*55:*/
#line 852 "hint.w"

{DBG(DBGDEF,"Defining normal stream %d at "SIZE_F"\n",*(hpos-1),hpos-hstart-2);
hget_xdimen_def_node(&(s->x));
HGET16(s->f);RNG("magnification factor",s->f,0,1000);
s->p= HGET8;if(s->p!=255)REF_RNG(stream_kind,s->p);
s->n= HGET8;if(s->n!=255)REF_RNG(stream_kind,s->n);
HGET16(s->r);RNG("split ratio",s->r,0,1000);
}
/*:55*/
#line 834 "hint.w"

else if(INFO(a)==b101)QUIT("first stream not yet implemented");
else if(INFO(a)==b110)QUIT("last stream not yet implemented");
else if(INFO(a)==b111)QUIT("top stream not yet implemented");
s->b= hget_list_pointer();
hget_xdimen_def_node(&(s->w));
s->g= hget_glue_spec();
s->a= hget_list_pointer();
s->h= hget_glue_spec();
}
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 844 "hint.w"

return true;
}
}
/*:54*//*61:*/
#line 921 "hint.w"


static void hset_cur_page(void)
{int i;
cur_page= &(page_def[0]);
for(i= 1;i<=max_ref[page_kind];i++)
if(page_def[i].p>=cur_page->p)
cur_page= &(page_def[i]);
}

static void hskip_list(void);
static void hget_page_def(uint8_t a,uint8_t i)
{char*n;
cur_page= &(page_def[i]);
ALLOCATE(cur_page->s,max_ref[stream_kind]+1,StreamDef);
HGET_STRING(n);cur_page->n= strdup(n);
cur_page->p= HGET8;
cur_page->g= hget_glue_spec();
HGET32(cur_page->d);
hget_xdimen_def_node(&(cur_page->v));
hget_xdimen_def_node(&(cur_page->h));
cur_page->t= hpos-hstart;
hskip_list();
while(hget_stream_def(cur_page->s))continue;
}
/*:61*//*72:*/
#line 1097 "hint.w"

static pointer hget_ligature_ref(uint8_t n)
{REF_RNG(ligature_kind,n);
return copy_node_list(pointer_def[ligature_kind][n]);
}

static pointer hget_rule_ref(uint8_t n)
{REF_RNG(rule_kind,n);
return copy_node_list(pointer_def[rule_kind][n]);
}

static pointer hget_image_ref(uint16_t n)
{REF_RNG(image_kind,n);
return copy_node_list(pointer_def[image_kind][n]);
}

static pointer hget_hyphen_ref(uint8_t n)
{REF_RNG(disc_kind,n);
return copy_node_list(pointer_def[disc_kind][n]);
}

static pointer hget_leaders_ref(uint8_t n)
{REF_RNG(leaders_kind,n);
return copy_node_list(pointer_def[leaders_kind][n]);
}




/*:72*//*85:*/
#line 1306 "hint.w"


static void hget_xdimen_def(Info i,Xdimen*x)
{switch(i)
{
case b000:
{int n= HGET8;
REF_RNG(xdimen_kind,n);
x->w= xdimen_def[n].w;
x->h= xdimen_def[n].h;
x->v= xdimen_def[n].v;
break;
}
case b001:HGET_XDIMEN(b001,*x);break;
case b010:HGET_XDIMEN(b010,*x);break;
case b011:HGET_XDIMEN(b011,*x);break;
case b100:HGET_XDIMEN(b100,*x);break;
case b101:HGET_XDIMEN(b101,*x);break;
case b110:HGET_XDIMEN(b110,*x);break;
case b111:HGET_XDIMEN(b111,*x);break;
default:
x->w= 0;x->h= x->v= 0.0;
}
}
static scaled hget_xdimen(Info i)
{Xdimen x;
hget_xdimen_def(i,&x);
return xdimen(&x);
}

static scaled hget_xdimen_node(void)
{scaled x= 0;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 1338 "hint.w"

if(KIND(a)==xdimen_kind)
x= hget_xdimen(INFO(a));
else
QUIT("Extent expected at 0x%x got %s",node_pos,NAME(a));
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1343 "hint.w"

return x;
}

static void hget_xdimen_def_node(Xdimen*x)
{/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 1348 "hint.w"

if(KIND(a)==xdimen_kind)
hget_xdimen_def(INFO(a),x);
else
QUIT("Extent expected at 0x%x got %s",node_pos,NAME(a));
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1353 "hint.w"

}


/*:85*//*86:*/
#line 1359 "hint.w"

scaled hteg_xdimen(uint8_t a)
{Xdimen x;
switch(a)
{
case TAG(xdimen_kind,b000):return hget_xdimen_ref(HTEG8);
case TAG(xdimen_kind,b001):HTEG_XDIMEN(b001,x);break;
case TAG(xdimen_kind,b010):HTEG_XDIMEN(b010,x);break;
case TAG(xdimen_kind,b011):HTEG_XDIMEN(b011,x);break;
case TAG(xdimen_kind,b100):HTEG_XDIMEN(b100,x);break;
case TAG(xdimen_kind,b101):HTEG_XDIMEN(b101,x);break;
case TAG(xdimen_kind,b110):HTEG_XDIMEN(b110,x);break;
case TAG(xdimen_kind,b111):HTEG_XDIMEN(b111,x);break;
default:
x.w= 0;x.h= x.v= 0.0;
QUIT("Extent expected got [%s,%d]",NAME(a),INFO(a));
}
return xdimen(&x);
}

scaled hteg_xdimen_node(void)
{scaled x= 0;
/*78:*/
#line 1222 "hint.w"

uint8_t a,z;
z= HTEG8,DBGTAG(z,hpos);
/*:78*/
#line 1381 "hint.w"

if(KIND(z)==xdimen_kind)
x= hteg_xdimen(z);
else
QUIT("Extent expected at 0x%x got %s",node_pos,NAME(z));
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 1386 "hint.w"

return x;
}
/*:86*//*102:*/
#line 1534 "hint.w"

static pointer hteg_rule_node(void)
{pointer q= null;
/*78:*/
#line 1222 "hint.w"

uint8_t a,z;
z= HTEG8,DBGTAG(z,hpos);
/*:78*/
#line 1537 "hint.w"

if(KIND(z)==rule_kind){HTEG_RULE(INFO(z));q= p;}
else QUIT("Rule expected at 0x%x got %s",node_pos,NAME(z));
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 1540 "hint.w"

return q;
}
/*:102*//*108:*/
#line 1603 "hint.w"

static pointer hget_glue_spec(void)
{pointer p= null;
uint8_t a,z;
if(hpos>=hend||KIND(*hpos)!=glue_kind)
{p= zero_glue;incr(glue_ref_count(p));}
else
{node_pos= hpos-hstart;
HGETTAG(a);
if(INFO(a)==b000)
{p= hget_glue_ref(HGET8);incr(glue_ref_count(p));}
else
{HGET_GLUE(INFO(a));}
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1616 "hint.w"

}
return p;
}

static pointer spec2glue(pointer q)
{pointer p;
p= get_node(small_node_size);type(p)= glue_node;subtype(p)= normal;
leader_ptr(p)= null;glue_ptr(p)= q;
return p;
}

static pointer hget_glue_node(void)
{return spec2glue(hget_glue_spec());
}
/*:108*//*109:*/
#line 1633 "hint.w"

static pointer hteg_glue_spec(void)
{pointer p= null;
uint8_t a,z;
if(hpos<=hstart)return null;
if(KIND(*(hpos-1))!=glue_kind)return null;
z= HTEG8,DBGTAG(z,hpos);
if(INFO(z)==b000)p= hget_glue_ref(HTEG8);
else
{HTEG_GLUE(INFO(z));}
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 1643 "hint.w"

return p;
}


static pointer hteg_glue_node(void)
{pointer p= hteg_glue_spec();
if(p!=null)return spec2glue(p);
else return new_glue(zero_glue);
}
/*:109*//*112:*/
#line 1678 "hint.w"

static pointer hget_node_list(uint32_t s)
{uint8_t*list_end= hpos+s;
pointer p;
push_nest();
cur_list.bs_pos= NULL;
while(hpos<list_end)
hget_content();
if(needs_bs)
QUIT("Unexpected trailing baseline node");
p= link(head);
pop_nest();
return p;
}

static pointer hget_text_list(uint32_t s);
static pointer hget_list_pointer(void)
{if(!IS_LIST(*hpos))return null;
else
{pointer p= null;
uint32_t s,t;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 1699 "hint.w"

s= hget_list_size(INFO(a));
hget_size_boundary(INFO(a));
if(KIND(a)==list_kind)
p= hget_node_list(s);
else if(KIND(a)==text_kind)
p= hget_text_list(s);
hget_size_boundary(INFO(a));
t= hget_list_size(INFO(a));
if(t!=s)
QUIT("List sizes at 0x%x and "SIZE_F" do not match 0x%x != 0x%x",node_pos+1,hpos-hstart-s-1,s,t);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1710 "hint.w"

return p;
}
}
/*:112*//*113:*/
#line 1720 "hint.w"


static void hskip_list()
{if(IS_LIST(*hpos))
{uint32_t s;
uint8_t a;
HGETTAG(a);
s= hget_list_size(INFO(a));
if(s> 0)
hpos= hpos+(1+s+1+(INFO(a)-1)+1);
else
hpos= hpos+1;
}
}

static void hskip_list_back()
{uint8_t z;
uint32_t s;
if(!IS_LIST(*(hpos-1)))return;
z= HTEG8;
s= hteg_list_size(INFO(z));
if(s> 0)
hpos= hpos-(1+s+1+(INFO(z)-1)+1);
else
hpos= hpos-1;
}

pointer hteg_list_pointer(void)
{uint8_t*list_start;
pointer p;
hskip_list_back();
list_start= hpos;
p= hget_list_pointer();
hpos= list_start;
return p;
}
/*:113*//*114:*/
#line 1765 "hint.w"

#if 0
static int32_t hteg_integer_def(uint8_t z)
{if(INFO(z)==1){int8_t n= HTEG8;return n;}
else if(INFO(z)==2){int16_t n;HTEG16(n);return n;}
else if(INFO(z)==4){int32_t n;HTEG32(n);return n;}
else TAGERR(z);
return 0;
}

static ParamDef*hteg_param_list(uint8_t z)
{uint32_t s,t;
ParamDef*p= NULL;
uint8_t*list_start,*list_end;
list_end= hpos;
s= hteg_list_size(INFO(z));
hteg_size_boundary(INFO(z));
list_start= hpos-s;
if(list_start<=hstart)
QUIT("list start before stream start\n");
while(list_start<hpos)
{ParamDef*r;Param*q;
/*78:*/
#line 1222 "hint.w"

uint8_t a,z;
z= HTEG8,DBGTAG(z,hpos);
/*:78*/
#line 1787 "hint.w"

ALLOCATE(r,1,ParamDef);
q= &(r->p);
q->k= KIND(z);
if(KIND(z)==int_kind)q->i= hteg_integer_def(a);
else if(KIND(a)==dimen_kind)HTEG32(q->d);
else if(KIND(a)==glue_kind){pointer p;HTEG_GLUE(INFO(z));q->g= p;}
else TAGERR(a);
q->n= HTEG8;
DBG(DBGTAGS,"Defining %s %d\n",definition_name[KIND(z)],q->n);
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 1797 "hint.w"

r->next= p;
p= r;
}
hteg_size_boundary(INFO(z));
t= hteg_list_size(INFO(z));
if(t!=s)
QUIT("List sizes at "SIZE_F" and "SIZE_F" do not match 0x%x != 0x%x",list_start-hstart,list_end-hstart,s,t);
return p;
}
#endif

static ParamDef*hteg_param_list_node(void)
{ParamDef*p;
uint8_t*list_start;
hskip_list_back();
list_start= hpos;
p= hget_param_list_node();
hpos= list_start;
return p;
}
/*:114*//*121:*/
#line 1924 "hint.w"

static pointer hteg_hbox_node(void)
{/*78:*/
#line 1222 "hint.w"

uint8_t a,z;
z= HTEG8,DBGTAG(z,hpos);
/*:78*/
#line 1926 "hint.w"

if(KIND(z)!=hbox_kind)QUIT("Hbox expected at 0x%x got %s",node_pos,NAME(z));
{pointer p;
HTEG_BOX(INFO(z));
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 1930 "hint.w"

return p;
}
}
static pointer hteg_vbox_node(void)
{/*78:*/
#line 1222 "hint.w"

uint8_t a,z;
z= HTEG8,DBGTAG(z,hpos);
/*:78*/
#line 1935 "hint.w"

if(KIND(z)!=vbox_kind)QUIT("Vbox expected at 0x%x got %s",node_pos,NAME(z));
{pointer p;
HTEG_BOX(INFO(z));
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 1939 "hint.w"

type(p)= vlist_node;
return p;
}
}

/*:121*//*127:*/
#line 2021 "hint.w"

static void hset(pointer p,
uint8_t sto,scaled st,uint8_t sho,scaled sh,scaled w)
{scaled x;
x= width(p);
width(p)= w;

x= w-x;
if(x==0)
{glue_sign(p)= normal;glue_order(p)= normal;
glue_set(p)= 0.0;
}
else if(x> 0)
{glue_order(p)= sto;glue_sign(p)= stretching;
if(st!=0)
glue_set(p)= (float32_t)(x/(double)st);
else
{glue_sign(p)= normal;
glue_set(p)= 0.0;
}
}
else
{glue_order(p)= sho;glue_sign(p)= shrinking;
if(sh!=0)
glue_set(p)= (float32_t)((-x)/(double)sh);
else
{glue_sign(p)= normal;
glue_set(p)= 0.0;
}
if((sh<-x)&&(sho==normal)&&(list_ptr(p)!=null))
glue_set(p)= 1.0;
}
}




void vset(pointer p,uint8_t sto,scaled st,
uint8_t sho,scaled sh,scaled h)
{scaled x;
type(p)= vlist_node;
x= height(p);
height(p)= h;
x= h-x;
if(x==0)
{glue_sign(p)= normal;glue_order(p)= normal;
glue_set(p)= 0.0;
}
else if(x> 0)
{glue_order(p)= sto;glue_sign(p)= stretching;
if(st!=0)
glue_set(p)= (float32_t)(x/(double)st);
else
{glue_sign(p)= normal;
glue_set(p)= 0.0;
}
}
else
{glue_order(p)= sho;glue_sign(p)= shrinking;
if(sh!=0)
glue_set(p)= (float32_t)((-x)/(double)sh);
else
{glue_sign(p)= normal;
glue_set(p)= 0.0;
}
}
}
/*:127*//*144:*/
#line 2300 "hint.w"

static pointer hget_text_list(uint32_t s)
{pointer p= null;
pointer*pp= &p;
uint8_t*t= hpos+s;
while(hpos<t){*pp= new_character(0,hget_utf8());pp= &link(*pp);}
return p;
}
/*:144*//*153:*/
#line 2407 "hint.w"

static pointer hget_disc_node(void)
{/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 2409 "hint.w"

if(KIND(a)!=disc_kind||INFO(a)==b000)
QUIT("Hyphen expected at 0x%x got %s,%d",node_pos,NAME(a),INFO(a));
{
HGET_DISC(INFO(a));
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 2414 "hint.w"

return p;
}
}
/*:153*//*157:*/
#line 2482 "hint.w"

static void transplant_post_break_list(void)
{pointer r,q= link(head);
int t= replace_count(q);
pointer s= post_break(q);
r= q;
while(t> 0&&r!=null){r= link(r);t--;}
if(s!=null)
{while(link(s)!=null)s= link(s);
link(s)= link(r);link(r)= post_break(q);post_break(q)= null;
}
q= link(r);
if(r!=head)
{link(r)= null;flush_node_list(link(head));
link(head)= q;
}
}
static void transplant_pre_break_list(void)
{pointer q= tail;
set_replace_count(q,0);
link(q)= pre_break(q);
pre_break(q)= null;
while(link(q)!=null)q= link(q);
tail= q;
}


static void hprune_unwanted_nodes(void)
{pointer q,r= head;
while(true){q= link(r);
if(q==null)goto done;
if(is_char_node(q))goto done;
if(non_discardable(q))goto done;
if(type(q)==kern_node&&subtype(q)!=explicit)goto done;
r= q;
}
done:if(r!=head)
{link(r)= null;flush_node_list(link(head));
link(head)= q;
}
}
/*:157*//*158:*/
#line 2549 "hint.w"

pointer hget_paragraph_all(scaled x)
{
uint32_t s,t;
uint8_t a,z;
if(!IS_LIST(*hpos))return null;
HGETTAG(a);
s= hget_list_size(INFO(a));
hget_size_boundary(INFO(a));
if(KIND(a)==list_kind)
{uint8_t*list_end= hpos+s;
cur_list.hs_field= x;
push_nest();
cur_list.bs_pos= NULL;
#if 0
if(offset> 0)hpos= hstart+node_pos1+offset;
#endif
while(hpos<list_end)
{hget_content();
if(nest_ptr==1)
{pointer p= tail;
if(p!=head&&!is_char_node(p)&&
(type(p)==glue_node||type(p)==kern_node||type(p)==penalty_node
||type(p)==disc_node||type(p)==math_node))
store_map(p,node_pos,0);

}
}
hget_size_boundary(INFO(a));
t= hget_list_size(INFO(a));
if(t!=s)
QUIT("List sizes at 0x%x and "SIZE_F" do not match 0x%x != 0x%x",node_pos+1,hpos-hstart-s-1,s,t);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 2581 "hint.w"

#if 0
if(offset> 0&&link(head)!=null&&!is_char_node(link(head)))
{if(type(link(head))==disc_node)
transplant_post_break_list();
else
hprune_unwanted_nodes();
}
#endif
if(needs_bs)
QUIT("Unexpected trailing baseline node");
if(head==tail)
{pop_nest();
return null;
}
else
{pointer par_ptr= link(head);
pop_nest();
store_map(par_ptr,node_pos,0);
return par_ptr;
}
}
else
QUIT("Text in paragraph not yet implemented");
return null;
}
/*:158*//*159:*/
#line 2612 "hint.w"

pointer hget_paragraph_final(scaled x,uint8_t*from)
{
uint32_t s,t;
uint8_t a,z;
if(!IS_LIST(*hpos))return null;
HGETTAG(a);
s= hget_list_size(INFO(a));
hget_size_boundary(INFO(a));
if(KIND(a)==list_kind)
{uint8_t*list_end= hpos+s;
cur_list.hs_field= x;
push_nest();
cur_list.bs_pos= NULL;
hpos= from;
while(hpos<list_end)
{hget_content();
if(nest_ptr==1)
{pointer p= tail;
if(p!=head&&!is_char_node(p)&&
(type(p)==glue_node||type(p)==kern_node||type(p)==penalty_node||type(p)==disc_node||type(p)==math_node))
store_map(p,node_pos,0);
}
}
hget_size_boundary(INFO(a));
t= hget_list_size(INFO(a));
if(t!=s)
QUIT("List sizes at 0x%x and "SIZE_F" do not match 0x%x != 0x%x",node_pos+1,hpos-hstart-s-1,s,t);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 2640 "hint.w"

if(link(head)!=null&&!is_char_node(link(head)))
{if(type(link(head))==disc_node)
transplant_post_break_list();
else
hprune_unwanted_nodes();
}
if(needs_bs)
QUIT("Unexpected trailing baseline node");
if(head==tail)
{pop_nest();
return null;
}
else
{pointer par_ptr= link(head);
pop_nest();
store_map(par_ptr,node_pos,0);
return par_ptr;
}
}
else
QUIT("Text in paragraph not yet implemented");
return null;
}
/*:159*//*165:*/
#line 2779 "hint.w"

pointer hget_paragraph(scaled x,uint32_t offset,ParamDef*q)
{
pointer p,par_head;
ParamDef*save_lbp= line_break_params;
par_head= tail;
line_break_params= q;
if(offset==0)
{prev_graf= 0;
p= hget_paragraph_all(x);
}
else
{prev_graf= 3;
p= hget_paragraph_final(x,hstart+node_pos+offset);
}
if(p!=null)
line_break(hget_integer_ref(widow_penalty_no),p);
line_break_params= save_lbp;
return par_head;
}

void hget_par_node(uint32_t offset)
{scaled x= 0;
ParamDef*q;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 2803 "hint.w"

if(KIND(a)!=par_kind)
QUIT("Paragrap expected found tag [%s,%d] at "SIZE_F"\n",NAME(a),INFO(a),hpos-hstart);
node_pos= (hpos-hstart)-1;
if(INFO(a)==b100)q= hget_param_list_ref(HGET8);
if(INFO(a)&b100)x= hget_xdimen_node();else x= hget_xdimen_ref(HGET8);
if(INFO(a)&b010)q= hget_param_list_node();else q= hget_param_list_ref(HGET8);
hget_paragraph(x,offset,q);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 2811 "hint.w"

}
/*:165*//*168:*/
#line 2833 "hint.w"

void hteg_paragraph(Info i)
{scaled x= 0;
ParamDef*q= null;
pointer par_head;
uint8_t*bs_pos= cur_list.bs_pos;
scaled ph= prev_height;
uint8_t*list_start,*par_start;
hskip_list_back();
list_start= hpos;
if(INFO(i)&b010)q= hteg_param_list_node();
else if(INFO(i)!=b100)q= hget_param_list_ref(HTEG8);
if(INFO(i)&b100)x= hteg_xdimen_node();else x= hget_xdimen_ref(HTEG8);
if(INFO(i)==b100)q= hget_param_list_ref(HTEG8);
par_start= hpos;
node_pos= par_start-hstart-1;
hpos= list_start;
cur_list.bs_pos= NULL;
par_head= hget_paragraph(x,0,q);
/*169:*/
#line 2865 "hint.w"

{pointer p,r,par_tail;
p= null;
r= par_tail= link(par_head);

tail= par_head;
link(tail)= null;
while(r!=null)
{pointer q= link(r);
link(r)= p;
p= r;
r= q;
}
cur_list.bs_pos= bs_pos;
prev_height= ph;
hprepend_to_vlist(p);
tail= par_tail;
if(type(tail)==hlist_node||type(tail)==vlist_node)
prev_height= height(tail);
}
/*:169*/
#line 2852 "hint.w"

hpos= par_start;
}
/*:168*//*184:*/
#line 3080 "hint.w"

static void hset_stream_params(pointer p,bool f,ParamDef*q)
{ParamDef*r;
pointer s;
while(q!=null)
{r= q;
if(q->p.k==int_kind&&q->p.n==floating_penalty_no)float_cost(p)= q->p.v;
else if(q->p.k==dimen_kind&&q->p.n==split_max_depth_no)depth(p)= (scaled)q->p.v;
else if(q->p.k==glue_kind&&q->p.n==split_top_skip_no)
{split_top_ptr(p)= (pointer)q->p.v;if(!f)add_glue_ref(split_top_ptr(p));}
else QUIT("Unexpected parameter in stream");
q= q->next;
if(f)free(r);
}
s= vpack(ins_ptr(p),natural);
height(p)= height(s)+depth(s);
ins_ptr(p)= list_ptr(s);
list_ptr(s)= null;flush_node_list(s);
}
/*:184*//*224:*/
#line 3807 "hint.w"


uint64_t hlocation(pointer p)
{return PAGE_LOC(map[p],map[p+1]);
}
/*:224*//*227:*/
#line 3853 "hint.w"

#define NEXT_PAGE(X) (X= (X+1)&(MAX_PAGE_POS-1))
#define PREV_PAGE(X) (X= (X-1)&(MAX_PAGE_POS-1))

void hloc_clear(void)
{lo_loc= hi_loc= cur_loc;PREV_PAGE(lo_loc);NEXT_PAGE(hi_loc);
}

bool hloc_next(void)
{int i= cur_loc;
if(LOC_POS(page_loc[cur_loc])>=hend-hstart)
return false;
NEXT_PAGE(i);
if(i==hi_loc)
return false;
cur_loc= i;
return true;
}

bool hloc_prev(void)
{int i= cur_loc;
if(page_loc[cur_loc]==0)
return false;
PREV_PAGE(i);
if(i==lo_loc)
return false;
cur_loc= i;
return true;
}


/*:227*//*230:*/
#line 3915 "hint.w"


void hloc_set(uint64_t h)
{int i;
if(page_loc[cur_loc]==h)return;
for(i= lo_loc,NEXT_PAGE(i);i!=hi_loc;NEXT_PAGE(i))
if(page_loc[i]==h)
{cur_loc= i;return;}
page_loc[cur_loc]= h;
hloc_clear();
DBG(DBGPAGE,"loc_set: %d < %d < %d\n",lo_loc,cur_loc,hi_loc);
}
/*:230*//*231:*/
#line 3939 "hint.w"


void hloc_set_next(pointer p)
{int i= cur_loc;
uint64_t h= hlocation(p);

NEXT_PAGE(i);
if(i==hi_loc)
{if(hi_loc==lo_loc)
NEXT_PAGE(lo_loc);
NEXT_PAGE(hi_loc);
page_loc[i]= h;
}
else if(h!=page_loc[i])
{page_loc[i]= h;
NEXT_PAGE(i);
hi_loc= i;
}
DBG(DBGPAGE,"loc_set_next: %d < %d < %d\n",lo_loc,cur_loc,hi_loc);
}
/*:231*//*232:*/
#line 3975 "hint.w"

void hloc_set_prev(pointer p)
{int i= cur_loc;
uint64_t h= hlocation(p);
PREV_PAGE(i);
if(i==lo_loc)
{if(lo_loc==hi_loc)
PREV_PAGE(hi_loc);
PREV_PAGE(lo_loc);
page_loc[i]= h;
}
else if(h!=page_loc[i])
{page_loc[i]= h;
lo_loc= i;
PREV_PAGE(lo_loc);
}
hi_loc= cur_loc;
NEXT_PAGE(hi_loc);
cur_loc= i;
DBG(DBGPAGE,"loc_set_prev: %d < %d < %d\n",lo_loc,cur_loc,hi_loc);
}
/*:232*//*245:*/
#line 4259 "hint.w"

static void hset_margins(void)
{if(cur_page==&(page_def[0])){
offset_h= page_h/8-0x48000;
if(offset_h<0)offset_h= 0;
offset_v= page_v/8-0x48000;
if(offset_v<0)offset_v= 0;
if(offset_h> offset_v)offset_h= offset_v;
else offset_v= offset_h;
hhsize= page_h-2*offset_h;
hvsize= page_v-2*offset_v;
if(hhsize<=0)hhsize= page_h,offset_h= 0;
if(hvsize<=0)hvsize= page_v,offset_v= 0;
}
else
{hhsize= round((double)(page_h-cur_page->h.w)/(double)cur_page->h.h);
if(hhsize> page_h)hhsize= page_h;
hvsize= round((double)(page_v-cur_page->v.w)/(double)cur_page->v.v);
if(hvsize> page_v)hvsize= page_v;
}
}
/*:245*//*247:*/
#line 4289 "hint.w"

static void houtput_template0(void)
{pointer p,q,r;
if(streams[0].p==null)return;
p= streams[0].p;streams[0].p= null;
p= vpackage(p,hvsize,exactly,page_max_depth);
r= new_kern(offset_v);
link(r)= p;
q= new_null_box();
type(q)= vlist_node;
width(q)= width(p)+offset_h;
height(q)= height(p)+offset_v;depth(q)= depth(p);
list_ptr(q)= r;
shift_amount(p)+= offset_h;
streams[0].p= q;
}
/*:247*//*272:*/
#line 4932 "hint.w"

static int trv_string_size= 0;
static char trv_string[256];
#define TRV_UTF8(C) (trv_string[trv_string_size++]= (C))
static void trv_string_collect(uint32_t c)
{if(trv_string_size<256-5)
{if(c<0x80)
TRV_UTF8(c);
else if(c<0x800)
{TRV_UTF8(0xC0|(c>>6));TRV_UTF8(0x80|(c&0x3F));}
else if(c<0x10000)
{TRV_UTF8(0xE0|(c>>12));TRV_UTF8(0x80|((c>>6)&0x3F));TRV_UTF8(0x80|(c&0x3F));}
else if(c<0x200000)
{TRV_UTF8(0xF0|(c>>18));TRV_UTF8(0x80|((c>>12)&0x3F));
TRV_UTF8(0x80|((c>>6)&0x3F));TRV_UTF8(0x80|(c&0x3F));}
else
RNG("character code in outline",c,0,0x1FFFFF);
}
}

char*hlist_to_string(pointer p)
{trv_string_size= 0;
trv_init(trv_string_collect);
trv_hlist(p);
trv_string[trv_string_size]= 0;
return trv_string;
}
/*:272*//*369:*/
#line 7451 "hint.w"

static pointer leaks[1<<16]= {0};

static void leak_clear(void)
{
#ifdef DEBUG
int i;
for(i= 0;i<0x10000;i++)
leaks[i]= 0;
#endif
}

void leak_in(pointer p,int s)
{
#ifdef DEBUG
if(0!=leaks[p])
fprintf(stderr,"ERROR leak in: p=%d, s in=%d, leaks[p]= %d != 0\n",p,s,leaks[p]);
leaks[p]= s;
#endif
}

void leak_out(pointer p,int s)
{
#ifdef DEBUG
if(s!=leaks[p])
fprintf(stderr,"ERROR: leak out: p=%d, s out=%d != %d = s in\n",p,s,leaks[p]);
leaks[p]= 0;
#endif
}

static void list_leaks(void)
{
#ifdef DEBUG
int i;
for(i= 0;i<0x10000;i++)
if(leaks[i]!=0)
fprintf(stderr,"ERROR:leak final: p=%d, s=%d\n",i,leaks[i]);
#endif
}
/*:369*/
#line 7542 "hint.w"

/*1:*/
#line 111 "hint.w"

void hget_def_node(void)
{Kind k;
int n;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 115 "hint.w"

k= KIND(a);
if(k==label_kind&&(INFO(a)&b001))HGET16(n);
else n= HGET8;
if(k!=range_kind)REF_RNG(k,n);
DBG(DBGTAGS,"Defining %s %d\n",definition_name[KIND(a)],n);
switch(KIND(a))
{case language_kind:{char*t;HGET_STRING(t);break;}
case font_kind:hget_font_def(a,n);break;
case int_kind:integer_def[n]= hget_integer_def(a);break;
case dimen_kind:dimen_def[n]= hget_dimen_def(a);break;
case xdimen_kind:hget_xdimen_def(INFO(a),&(xdimen_def[n]));break;
case baseline_kind:hget_baseline_def(a,n);break;
case glue_kind:pointer_def[glue_kind][n]= hget_glue_def(a);break;
case param_kind:param_def[n]= hget_param_list(a);break;
case range_kind:hget_range_def(a,n);break;
case page_kind:hget_page_def(a,n);break;
case label_kind:hget_outline_or_label_def(INFO(a),n);break;
default:pointer_def[KIND(a)][n]= hget_definition(a);break;
}
if(max_fixed[k]> max_default[k])
QUIT("Definitions for kind %s not supported",definition_name[k]);
if(n> max_ref[k]||n<=max_fixed[k])
QUIT("Definition %d for %s out of range [%d - %d]",
n,definition_name[k],max_fixed[k]+1,max_ref[k]);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 140 "hint.w"

}

pointer hset_glue(Glue*g)
{if(ZERO_GLUE(*g))
{add_glue_ref(zero_glue);
return zero_glue;
}
else
{pointer p= get_node(glue_spec_size);
width(p)= g->w.w;
stretch(p)= round(g->p.f*ONE);stretch_order(p)= g->p.o;
shrink(p)= round(g->m.f*ONE);shrink_order(p)= g->m.o;
return p;
}
}

void hset_default_definitions(void)
{int i;
for(i= 0;i<=MAX_INT_DEFAULT;i++)integer_def[i]= int_defaults[i];
for(i= 0;i<=MAX_DIMEN_DEFAULT;i++)dimen_def[i]= dimen_defaults[i];
for(i= 0;i<=MAX_XDIMEN_DEFAULT;i++)xdimen_def[i]= xdimen_defaults[i];
for(i= 0;i<=MAX_GLUE_DEFAULT;i++)pointer_def[glue_kind][i]= hset_glue(&(glue_defaults[i]));
for(i= 0;i<=MAX_BASELINE_DEFAULT;i++)
{baseline_def[i].bs= hset_glue(&(baseline_defaults[i].bs));
baseline_def[i].ls= hset_glue(&(baseline_defaults[i].ls));
baseline_def[i].lsl= baseline_defaults[i].lsl;
}
}

void free_definitions(void)
{/*5:*/
#line 226 "hint.w"

{int k;
for(k= 0;k<32;k++)
{free(pointer_def[k]);pointer_def[k]= NULL;}
}
/*:5*//*8:*/
#line 249 "hint.w"

free(integer_def);integer_def= NULL;
/*:8*//*12:*/
#line 278 "hint.w"

free(dimen_def);dimen_def= NULL;
/*:12*//*16:*/
#line 307 "hint.w"

free(xdimen_def);xdimen_def= NULL;
/*:16*//*24:*/
#line 378 "hint.w"

free(baseline_def);baseline_def= NULL;
/*:24*//*31:*/
#line 496 "hint.w"

free(font_def);font_def= NULL;
/*:31*//*40:*/
#line 601 "hint.w"

if(param_def!=NULL)
{int i;
for(i= 0;i<=max_ref[param_kind];i++)
free_param_list(param_def[i]);
}
free(param_def);param_def= NULL;
/*:40*//*47:*/
#line 744 "hint.w"

free(range_def);range_def= NULL;
/*:47*//*52:*/
#line 798 "hint.w"

free(streams);streams= NULL;
/*:52*//*60:*/
#line 911 "hint.w"

if(page_def!=NULL)
{int k;
for(k= 0;k<=max_ref[page_kind];k++)
{free(page_def[k].s);free(page_def[k].n);
}
free(page_def);page_def= NULL;cur_page= NULL;
}
/*:60*//*67:*/
#line 1033 "hint.w"

free(labels);labels= NULL;
{int k;
for(k= 0;k<=max_outline;k++)free(hint_outlines[k].title);
}
free(hint_outlines);hint_outlines= NULL;outline_no= -1;
max_outline= -1;
/*:67*/
#line 171 "hint.w"

}

void hget_definition_section(void)
{DBG(DBGDEF,"Definitions\n");
hget_section(1);
DBG(DBGDEF,"Reading list of maximum values\n");
free_definitions();
hget_max_definitions();
/*4:*/
#line 211 "hint.w"

{Kind k;
for(k= 0;k<32;k++)
{if(k==font_kind||k==int_kind||k==dimen_kind||k==xdimen_kind||
k==glue_kind||k==baseline_kind||k==range_kind||k==page_kind||k==param_kind||k==stream_kind||k==label_kind)
continue;
if(max_ref[k]>=0&&max_ref[k]<=256)
{DBG(DBGDEF,"Allocating definitions for %s (kind %d): %d entries of "SIZE_F" byte each\n",
definition_name[k],k,max_ref[k]+1,sizeof(pointer));
ALLOCATE(pointer_def[k],max_ref[k]+1,pointer);
}
}
}
/*:4*//*7:*/
#line 245 "hint.w"

ALLOCATE(integer_def,max_ref[int_kind]+1,int32_t);
/*:7*//*11:*/
#line 274 "hint.w"

ALLOCATE(dimen_def,max_ref[dimen_kind]+1,Dimen);
/*:11*//*15:*/
#line 304 "hint.w"

ALLOCATE(xdimen_def,max_ref[xdimen_kind]+1,Xdimen);
/*:15*//*20:*/
#line 355 "hint.w"

ALLOCATE(pointer_def[glue_kind],max_ref[glue_kind]+1,pointer);
/*:20*//*23:*/
#line 374 "hint.w"

ALLOCATE(baseline_def,max_ref[baseline_kind]+1,BaselineSkip);
/*:23*//*30:*/
#line 492 "hint.w"

ALLOCATE(font_def,max_ref[font_kind]+1,FontDef);
/*:30*//*39:*/
#line 597 "hint.w"

ALLOCATE(param_def,max_ref[param_kind]+1,ParamDef*);
/*:39*//*46:*/
#line 740 "hint.w"

ALLOCATE(range_def,max_ref[range_kind]+1,RangeDef);
/*:46*//*51:*/
#line 794 "hint.w"

ALLOCATE(streams,max_ref[stream_kind]+1,Stream);
/*:51*//*58:*/
#line 891 "hint.w"

ALLOCATE(page_def,max_ref[page_kind]+1,PageDef);
/*:58*//*66:*/
#line 1026 "hint.w"

if(max_ref[label_kind]>=0)
ALLOCATE(labels,max_ref[label_kind]+1,Label);
if(max_outline>=0)
ALLOCATE(hint_outlines,max_outline+1,hint_Outline);
/*:66*/
#line 180 "hint.w"

hset_default_definitions();
DBG(DBGDEF,"Reading list of definitions\n");
while(hpos<hend)
hget_def_node();
hget_font_metrics();
/*59:*/
#line 895 "hint.w"

page_def[0].d= max_depth;
page_def[0].g= top_skip;add_glue_ref(top_skip);
page_def[0].p= 0;
page_def[0].n= strdup("default");
page_def[0].v.w= -9*ONE;
page_def[0].v.h= 0.0;
page_def[0].v.v= 1.25;
page_def[0].h.w= -9*ONE;
page_def[0].h.h= 1.25;
page_def[0].h.v= 0.0;
page_def[0].t= 0;
ALLOCATE(page_def[0].s,max_ref[stream_kind]+1,StreamDef);
cur_page= &(page_def[0]);
/*:59*/
#line 186 "hint.w"

}
/*:1*//*13:*/
#line 282 "hint.w"

scaled hget_dimen_ref(uint8_t n)
{REF_RNG(dimen_kind,n);
return dimen_def[n];
}

static scaled hget_dimen_def(uint8_t a)
{if(INFO(a)==b000)
return hget_dimen_ref(HGET8);
else
{scaled d;HGET32(d);return d;}
}
/*:13*//*25:*/
#line 382 "hint.w"

static void hget_baseline_def(uint8_t a,uint8_t n)
{HGET_BASELINE(INFO(a));
baseline_def[n].bs= cur_bs;add_glue_ref(cur_bs);
baseline_def[n].ls= cur_ls;add_glue_ref(cur_ls);
baseline_def[n].lsl= cur_lsl;
}

void hget_baseline_ref(uint8_t n)
{REF_RNG(baseline_kind,n);
cur_bs= baseline_def[n].bs;
cur_ls= baseline_def[n].ls;
cur_lsl= baseline_def[n].lsl;
}

pointer happend_to_vlist(pointer b)
{scaled d;
pointer p= null;

if(needs_bs&&prev_depth> ignore_depth)
{d= width(cur_bs)-prev_depth-height(b);
if(d<cur_lsl)p= new_glue(cur_ls);
else{pointer q= new_spec(cur_bs);
width(q)= d;p= new_glue(q);glue_ref_count(q)= null;
}
link(tail)= p;tail= p;
if(nest_ptr==0)
store_map(p,cur_list.bs_pos-hstart,0);
}
link(tail)= b;tail= b;prev_depth= depth(b);
cur_list.bs_pos= NULL;
return p;
}
/*:25*//*26:*/
#line 436 "hint.w"

static pointer hprepend_to_vlist(pointer b)
{scaled d;
pointer p= null;

if(needs_bs&&prev_height> ignore_depth)
{d= width(cur_bs)-prev_height-depth(b);
if(d<cur_lsl)p= new_glue(cur_ls);
else{pointer q= new_spec(cur_bs);
width(q)= d;p= new_glue(q);glue_ref_count(q)= null;
}
link(tail)= p;tail= p;
if(nest_ptr==0)
store_map(p,cur_list.bs_pos-hstart,0);
}
link(tail)= b;tail= b;prev_height= height(b);
cur_list.bs_pos= NULL;
return p;
}
/*:26*//*68:*/
#line 1046 "hint.w"

void hget_outline_or_label_def(Info i,int n)
{if(i&b100)
/*70:*/
#line 1067 "hint.w"

{hint_Outline*t;
uint64_t pos;
uint8_t where;
outline_no++;
RNG("Outline",outline_no,0,max_outline);
t= hint_outlines+outline_no;
t->depth= HGET8;
t->p= hget_list_pointer();
t->title= strdup(hlist_to_string(t->p));
/*71:*/
#line 1083 "hint.w"

where= labels[n].where;
#if 1
pos= ((uint64_t)labels[n].pos<<32)+(labels[n].pos-labels[n].pos0);
#else
pos= ((uint64_t)labels[n].pos0<<32);
#endif
/*:71*/
#line 1077 "hint.w"

t->where= where;
t->pos= pos;
}
/*:70*/
#line 1049 "hint.w"

else
/*69:*/
#line 1055 "hint.w"

{Label*t= labels+n;
HGET32(t->pos);
t->where= HGET8;
if(t->where> LABEL_MID)t->where= LABEL_UNDEF;
if(i&b010)
{HGET32(t->pos0);t->f= HGET8;}
else t->pos0= t->pos;
DBG(DBGDEF,"Label 0x%x+0x%x where=%d font=%d\n",t->pos0,t->pos,t->where,t->f);
}
/*:69*/
#line 1051 "hint.w"

}
/*:68*//*73:*/
#line 1147 "hint.w"

static void hget_content_section()
{DBG(DBGDIR,"Reading Content Section\n");
hget_section(2);
}
/*:73*//*101:*/
#line 1523 "hint.w"

pointer hget_rule_node(void)
{pointer q= null;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 1526 "hint.w"

if(KIND(a)==rule_kind){HGET_RULE(INFO(a));q= p;}
else QUIT("Rule expected at 0x%x got %s",node_pos,NAME(a));
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1529 "hint.w"

return q;
}
/*:101*//*120:*/
#line 1899 "hint.w"

pointer hget_hbox_node(void)
{/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 1901 "hint.w"

if(KIND(a)!=hbox_kind)QUIT("Hbox expected at 0x%x got %s",node_pos,NAME(a));
{pointer p;
HGET_BOX(INFO(a));
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1905 "hint.w"

return p;
}
}


pointer hget_vbox_node(void)
{
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 1913 "hint.w"

if(KIND(a)!=vbox_kind)QUIT("Vbox expected at 0x%x got %s",node_pos,NAME(a));
{pointer p;
HGET_BOX(INFO(a));
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1917 "hint.w"

type(p)= vlist_node;
return p;
}
}
/*:120*//*160:*/
#line 2667 "hint.w"

pointer hget_paragraph_initial(scaled x,uint8_t*to)
{
uint8_t*node_start;
uint32_t s,t;
uint8_t a,z;
node_start= hpos;
HGETTAG(a);
if(!IS_LIST(a))
{hpos= node_start;return null;}
s= hget_list_size(INFO(a));
hget_size_boundary(INFO(a));
if(KIND(a)==list_kind)
{uint8_t*list_end= hpos+s;if(to> list_end){LOG("Value of to greater than list_end");to= list_end;}
cur_list.hs_field= x;
push_nest();
cur_list.bs_pos= NULL;
while(hpos<to)
{hget_content();
if(nest_ptr==1)
{pointer p= tail;
if(p!=head&&!is_char_node(p)&&
(type(p)==glue_node||type(p)==kern_node||type(p)==penalty_node||type(p)==disc_node||type(p)==math_node))
store_map(p,node_pos,0);
}
}
if(KIND(*to)==disc_kind)
{hget_content();
store_map(tail,node_pos,0);
transplant_pre_break_list();
}
hpos= list_end;
hget_size_boundary(INFO(a));
t= hget_list_size(INFO(a));
if(t!=s)
QUIT("List sizes at 0x%x and "SIZE_F" do not match 0x%x != 0x%x",node_pos+1,hpos-hstart-s-1,s,t);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 2703 "hint.w"

if(needs_bs)
QUIT("Unexpected trailing baseline node");
if(head==tail)
{pop_nest();
return null;
}
else
{pointer par_ptr= link(head);
/*161:*/
#line 2731 "hint.w"

if(is_char_node(tail))tail_append(new_penalty(inf_penalty))
else if(type(tail)!=glue_node)tail_append(new_penalty(inf_penalty))
else
{type(tail)= penalty_node;delete_glue_ref(glue_ptr(tail));
flush_node_list(leader_ptr(tail));penalty(tail)= inf_penalty;
}
tail_append(new_glue(zero_glue));
/*:161*/
#line 2712 "hint.w"

pop_nest();
store_map(par_ptr,node_pos,0);
return par_ptr;
}
}
else
QUIT("Text in paragraph not yet implemented");
return null;
}
/*:160*/
#line 7543 "hint.w"

/*80:*/
#line 1241 "hint.w"

static void hteg_node(uint8_t z)
{switch(z)
{
/*92:*/
#line 1435 "hint.w"

case TAG(glyph_kind,1):HTEG_GLYPH(1);break;
case TAG(glyph_kind,2):HTEG_GLYPH(2);break;
case TAG(glyph_kind,3):HTEG_GLYPH(3);break;
case TAG(glyph_kind,4):HTEG_GLYPH(4);break;
/*:92*//*94:*/
#line 1450 "hint.w"

case TAG(penalty_kind,0):tail_append(new_penalty(hget_integer_ref(HTEG8)));break;
case TAG(penalty_kind,1):{tail_append(new_penalty(HTEG8));}break;
case TAG(penalty_kind,2):{int16_t n;HTEG16(n);RNG("Penalty",n,-20000,+20000);tail_append(new_penalty(n));}break;
/*:94*//*96:*/
#line 1470 "hint.w"

case TAG(language_kind,b000):(void)HTEG8;
case TAG(language_kind,1):
case TAG(language_kind,2):
case TAG(language_kind,3):
case TAG(language_kind,4):
case TAG(language_kind,5):
case TAG(language_kind,6):
case TAG(language_kind,7):break;
/*:96*//*100:*/
#line 1513 "hint.w"

case TAG(rule_kind,b000):tail_append(hget_rule_ref(HTEG8));prev_height= ignore_depth;break;
case TAG(rule_kind,b011):{HTEG_RULE(b011);tail_append(p);prev_height= ignore_depth;}break;
case TAG(rule_kind,b101):{HTEG_RULE(b101);tail_append(p);prev_height= ignore_depth;}break;
case TAG(rule_kind,b001):{HTEG_RULE(b001);tail_append(p);prev_height= ignore_depth;}break;
case TAG(rule_kind,b110):{HTEG_RULE(b110);tail_append(p);prev_height= ignore_depth;}break;
case TAG(rule_kind,b111):{HTEG_RULE(b111);tail_append(p);prev_height= ignore_depth;}break;
/*:100*//*107:*/
#line 1584 "hint.w"

case TAG(glue_kind,b000):tail_append(new_glue(hget_glue_ref(HTEG8)));break;
case TAG(glue_kind,b001):{pointer p;HTEG_GLUE(b001);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b010):{pointer p;HTEG_GLUE(b010);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b011):{pointer p;HTEG_GLUE(b011);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b100):{pointer p;HTEG_GLUE(b100);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b101):{pointer p;HTEG_GLUE(b101);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b110):{pointer p;HTEG_GLUE(b110);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b111):{pointer p;HTEG_GLUE(b111);tail_append(spec2glue(p));}break;
/*:107*//*119:*/
#line 1880 "hint.w"

case TAG(hbox_kind,b000):{pointer p;HTEG_BOX(b000);hprepend_to_vlist(p);}break;
case TAG(hbox_kind,b001):{pointer p;HTEG_BOX(b001);hprepend_to_vlist(p);}break;
case TAG(hbox_kind,b010):{pointer p;HTEG_BOX(b010);hprepend_to_vlist(p);}break;
case TAG(hbox_kind,b011):{pointer p;HTEG_BOX(b011);hprepend_to_vlist(p);}break;
case TAG(hbox_kind,b100):{pointer p;HTEG_BOX(b100);hprepend_to_vlist(p);}break;
case TAG(hbox_kind,b101):{pointer p;HTEG_BOX(b101);hprepend_to_vlist(p);}break;
case TAG(hbox_kind,b110):{pointer p;HTEG_BOX(b110);hprepend_to_vlist(p);}break;
case TAG(hbox_kind,b111):{pointer p;HTEG_BOX(b111);hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b000):{pointer p;HTEG_BOX(b000);type(p)= vlist_node;hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b001):{pointer p;HTEG_BOX(b001);type(p)= vlist_node;hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b010):{pointer p;HTEG_BOX(b010);type(p)= vlist_node;hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b011):{pointer p;HTEG_BOX(b011);type(p)= vlist_node;hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b100):{pointer p;HTEG_BOX(b100);type(p)= vlist_node;hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b101):{pointer p;HTEG_BOX(b101);type(p)= vlist_node;hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b110):{pointer p;HTEG_BOX(b110);type(p)= vlist_node;hprepend_to_vlist(p);}break;
case TAG(vbox_kind,b111):{pointer p;HTEG_BOX(b111);type(p)= vlist_node;hprepend_to_vlist(p);}break;
/*:119*//*126:*/
#line 1998 "hint.w"

case TAG(hset_kind,b000):{pointer p;HTEG_SET(b000);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(hset_kind,b001):{pointer p;HTEG_SET(b001);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(hset_kind,b010):{pointer p;HTEG_SET(b010);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(hset_kind,b011):{pointer p;HTEG_SET(b011);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(hset_kind,b100):{pointer p;HTEG_SET(b100);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(hset_kind,b101):{pointer p;HTEG_SET(b101);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(hset_kind,b110):{pointer p;HTEG_SET(b110);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(hset_kind,b111):{pointer p;HTEG_SET(b111);hset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;

case TAG(vset_kind,b000):{pointer p;HTEG_SET(b000);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(vset_kind,b001):{pointer p;HTEG_SET(b001);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(vset_kind,b010):{pointer p;HTEG_SET(b010);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(vset_kind,b011):{pointer p;HTEG_SET(b011);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(vset_kind,b100):{pointer p;HTEG_SET(b100);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(vset_kind,b101):{pointer p;HTEG_SET(b101);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(vset_kind,b110):{pointer p;HTEG_SET(b110);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
case TAG(vset_kind,b111):{pointer p;HTEG_SET(b111);vset(p,sto,st,sho,sh,x);hprepend_to_vlist(p);}break;
/*:126*//*131:*/
#line 2138 "hint.w"

case TAG(hpack_kind,b000):HTEG_PACK(hpack_kind,b000);break;
case TAG(hpack_kind,b010):HTEG_PACK(hpack_kind,b010);break;
case TAG(hpack_kind,b100):HTEG_PACK(hpack_kind,b100);break;
case TAG(hpack_kind,b110):HTEG_PACK(hpack_kind,b110);break;
case TAG(hpack_kind,b001):HTEG_PACK(hpack_kind,b001);break;
case TAG(hpack_kind,b011):HTEG_PACK(hpack_kind,b011);break;
case TAG(hpack_kind,b101):HTEG_PACK(hpack_kind,b101);break;
case TAG(hpack_kind,b111):HTEG_PACK(hpack_kind,b111);break;

case TAG(vpack_kind,b000):HTEG_PACK(vpack_kind,b000);break;
case TAG(vpack_kind,b010):HTEG_PACK(vpack_kind,b010);break;
case TAG(vpack_kind,b100):HTEG_PACK(vpack_kind,b100);break;
case TAG(vpack_kind,b110):HTEG_PACK(vpack_kind,b110);break;
case TAG(vpack_kind,b001):HTEG_PACK(vpack_kind,b001);break;
case TAG(vpack_kind,b011):HTEG_PACK(vpack_kind,b011);break;
case TAG(vpack_kind,b101):HTEG_PACK(vpack_kind,b101);break;
case TAG(vpack_kind,b111):HTEG_PACK(vpack_kind,b111);break;
/*:131*//*135:*/
#line 2197 "hint.w"

case TAG(kern_kind,b000):{HTEG_KERN(b000);}break;
case TAG(kern_kind,b001):{HTEG_KERN(b001);}break;
case TAG(kern_kind,b010):{HTEG_KERN(b010);}break;
case TAG(kern_kind,b011):{HTEG_KERN(b011);}break;
case TAG(kern_kind,b100):{HTEG_KERN(b100);}break;
case TAG(kern_kind,b101):{HTEG_KERN(b101);}break;
case TAG(kern_kind,b110):{HTEG_KERN(b110);}break;
case TAG(kern_kind,b111):{HTEG_KERN(b111);}break;
/*:135*//*139:*/
#line 2244 "hint.w"

case TAG(leaders_kind,0):tail_append(hget_leaders_ref(HTEG8));break;
case TAG(leaders_kind,1):HTEG_LEADERS(1);break;
case TAG(leaders_kind,2):HTEG_LEADERS(2);break;
case TAG(leaders_kind,3):HTEG_LEADERS(3);break;
case TAG(leaders_kind,b100|1):HTEG_LEADERS(b100|1);break;
case TAG(leaders_kind,b100|2):HTEG_LEADERS(b100|2);break;
case TAG(leaders_kind,b100|3):HTEG_LEADERS(b100|3);break;
/*:139*//*143:*/
#line 2285 "hint.w"

case TAG(baseline_kind,b000):{hget_baseline_ref(HTEG8);cur_list.bs_pos= hpos-1;}break;
case TAG(baseline_kind,b010):{HTEG_BASELINE(b010);}break;
case TAG(baseline_kind,b011):{HTEG_BASELINE(b011);}break;
case TAG(baseline_kind,b100):{HTEG_BASELINE(b100);}break;
case TAG(baseline_kind,b101):{HTEG_BASELINE(b101);}break;
case TAG(baseline_kind,b110):{HTEG_BASELINE(b110);}break;
case TAG(baseline_kind,b111):{HTEG_BASELINE(b111);}break;
/*:143*//*148:*/
#line 2346 "hint.w"

case TAG(ligature_kind,0):tail_append(hget_ligature_ref(HTEG8));break;
case TAG(ligature_kind,1):HTEG_LIG(1);break;
case TAG(ligature_kind,2):HTEG_LIG(2);break;
case TAG(ligature_kind,3):HTEG_LIG(3);break;
case TAG(ligature_kind,4):HTEG_LIG(4);break;
case TAG(ligature_kind,5):HTEG_LIG(5);break;
case TAG(ligature_kind,6):HTEG_LIG(6);break;
case TAG(ligature_kind,7):HTEG_LIG(7);break;
/*:148*//*152:*/
#line 2395 "hint.w"

case TAG(disc_kind,b000):tail_append(hget_hyphen_ref(HTEG8));break;
case TAG(disc_kind,b001):{HTEG_DISC(b001);tail_append(p);}break;
case TAG(disc_kind,b010):{HTEG_DISC(b010);tail_append(p);}break;
case TAG(disc_kind,b011):{HTEG_DISC(b011);tail_append(p);}break;
case TAG(disc_kind,b100):{HTEG_DISC(b100);tail_append(p);}break;
case TAG(disc_kind,b101):{HTEG_DISC(b101);tail_append(p);}break;
case TAG(disc_kind,b110):{HTEG_DISC(b110);tail_append(p);}break;
case TAG(disc_kind,b111):{HTEG_DISC(b111);tail_append(p);}break;
/*:152*//*167:*/
#line 2820 "hint.w"

case TAG(par_kind,b000):hteg_paragraph(b000);break;
case TAG(par_kind,b010):hteg_paragraph(b010);break;
case TAG(par_kind,b100):hteg_paragraph(b100);break;
case TAG(par_kind,b110):hteg_paragraph(b110);break;
/*:167*//*175:*/
#line 2953 "hint.w"

case TAG(math_kind,b000):HTEG_MATH(b000);break;
case TAG(math_kind,b001):HTEG_MATH(b001);break;
case TAG(math_kind,b010):HTEG_MATH(b010);break;
case TAG(math_kind,b100):HTEG_MATH(b100);break;
case TAG(math_kind,b101):HTEG_MATH(b101);break;
case TAG(math_kind,b110):HTEG_MATH(b110);break;
/*:175*//*177:*/
#line 2977 "hint.w"

case TAG(math_kind,b111):tail_append(new_math(0,before));break;
case TAG(math_kind,b011):tail_append(new_math(0,after));break;
/*:177*//*183:*/
#line 3047 "hint.w"

case TAG(table_kind,b000):HTEG_TABLE(b000);break;
case TAG(table_kind,b001):HTEG_TABLE(b001);break;
case TAG(table_kind,b010):HTEG_TABLE(b010);break;
case TAG(table_kind,b011):HTEG_TABLE(b011);break;
case TAG(table_kind,b100):HTEG_TABLE(b100);break;
case TAG(table_kind,b101):HTEG_TABLE(b101);break;
case TAG(table_kind,b110):HTEG_TABLE(b110);break;
case TAG(table_kind,b111):HTEG_TABLE(b111);break;

case TAG(item_kind,b000):hteg_list_pointer();break;
case TAG(item_kind,b001):hteg_content();break;
case TAG(item_kind,b010):hteg_content();break;
case TAG(item_kind,b011):hteg_content();break;
case TAG(item_kind,b100):hteg_content();break;
case TAG(item_kind,b101):hteg_content();break;
case TAG(item_kind,b110):hteg_content();break;
case TAG(item_kind,b111):hteg_content();(void)HTEG8;break;
/*:183*//*188:*/
#line 3128 "hint.w"

case TAG(stream_kind,b000):HTEG_STREAM(b000);break;
case TAG(stream_kind,b010):HTEG_STREAM(b010);break;
/*:188*//*192:*/
#line 3167 "hint.w"

case TAG(image_kind,b000):hget_image_ref(HTEG8);break;
case TAG(image_kind,b100):HTEG_IMAGE(b100);break;
case TAG(image_kind,b101):HTEG_IMAGE(b101);break;
case TAG(image_kind,b110):HTEG_IMAGE(b110);break;
case TAG(image_kind,b111):HTEG_IMAGE(b111);break;
/*:192*//*196:*/
#line 3204 "hint.w"

case TAG(link_kind,b000):HTEG_LINK(b000);break;
case TAG(link_kind,b001):HTEG_LINK(b001);break;
case TAG(link_kind,b010):HTEG_LINK(b010);break;
case TAG(link_kind,b011):HTEG_LINK(b011);break;
/*:196*/
#line 1245 "hint.w"

default:
TAGERR(z);
}
}

void hteg_content(void)
{/*78:*/
#line 1222 "hint.w"

uint8_t a,z;
z= HTEG8,DBGTAG(z,hpos);
/*:78*/
#line 1252 "hint.w"

node_pos= hpos-hstart;
hteg_node(z);
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 1255 "hint.w"

node_pos= hpos-hstart;
if(nest_ptr==0&&tail!=head
#if 0
&&(type(tail)==penalty_node||type(tail)==glue_node
||type(tail)==hlist_node||type(tail)==vlist_node
||type(tail)==kern_node)
#endif
)
store_map(tail,node_pos,0);
}
/*:80*//*154:*/
#line 2420 "hint.w"

pointer hteg_disc_node(void)
{/*78:*/
#line 1222 "hint.w"

uint8_t a,z;
z= HTEG8,DBGTAG(z,hpos);
/*:78*/
#line 2422 "hint.w"

if(KIND(z)!=disc_kind||INFO(z)==b000)
QUIT("Hyphen expected at 0x%x got %s,%d",node_pos,NAME(z),INFO(z));
{
HTEG_DISC(INFO(z));
/*79:*/
#line 1227 "hint.w"

a= HTEG8,DBGTAG(a,hpos);
if(z!=a)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at "SIZE_F" to 0x%x\n",
NAME(a),INFO(a),NAME(z),INFO(z),hpos-hstart,node_pos);

/*:79*/
#line 2427 "hint.w"

return p;
}
}
/*:154*//*170:*/
#line 2889 "hint.w"

void hteg_par_node(uint32_t offset)
{scaled x= 0;
ParamDef*save_lbp= line_break_params;
pointer p;
pointer par_head= tail;
uint8_t*bs_pos= cur_list.bs_pos;
scaled ph= prev_height;
/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 2897 "hint.w"

node_pos= (hpos-hstart)-1;
if(INFO(a)&b100)x= hget_xdimen_node();else x= hget_xdimen_ref(HGET8);
if(INFO(a)&b010)line_break_params= hget_param_list_node();else line_break_params= hget_param_list_ref(HGET8);
prev_graf= 0;
p= hget_paragraph_initial(x,hstart+node_pos+offset);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 2903 "hint.w"

cur_list.bs_pos= NULL;
if(p!=null)
line_break(hget_integer_ref(widow_penalty_no),p);
if(par_head!=tail)
/*169:*/
#line 2865 "hint.w"

{pointer p,r,par_tail;
p= null;
r= par_tail= link(par_head);

tail= par_head;
link(tail)= null;
while(r!=null)
{pointer q= link(r);
link(r)= p;
p= r;
r= q;
}
cur_list.bs_pos= bs_pos;
prev_height= ph;
hprepend_to_vlist(p);
tail= par_tail;
if(type(tail)==hlist_node||type(tail)==vlist_node)
prev_height= height(tail);
}
/*:169*/
#line 2908 "hint.w"

hpos= hstart+node_pos;
line_break_params= save_lbp;
}

/*:170*/
#line 7544 "hint.w"


/*18:*/
#line 323 "hint.w"

void print_xdimen(int i)
{}
/*:18*//*34:*/
#line 552 "hint.w"

uint16_t hglyph_section(uint8_t f)
{return font_def[f].q;
}

int32_t font_at_size(uint8_t f)
{return font_def[f].s;
}
/*:34*//*62:*/
#line 949 "hint.w"

static void hinsert_stream(uint8_t n)
{REF_RNG(stream_kind,n);
if(streams[n].p==null)return;
DBG(DBGPAGE,"Filling in stream %d\n",n);
if(n> 0&&cur_page->s[n].b!=0)
{pointer p= copy_node_list(cur_page->s[n].b);
link(tail)= p;
while(link(p)!=null)p= link(p);
tail= p;
DBG(DBGPAGE,"Filling in before list %d\n",n);
}
link(tail)= streams[n].p;
tail= streams[n].t;
if(tail==null)QUIT("Tail of nonempty stream %d is null\n",n);
streams[n].p= streams[n].t= null;
DBG(DBGPAGE,"Filling in content list %d\n",n);
if(n> 0&&cur_page->s[n].a!=0)
{pointer p= copy_node_list(cur_page->s[n].a);
link(tail)= p;
while(link(p)!=null)p= link(p);
tail= p;
DBG(DBGPAGE,"Filling in after list %d\n",n);
}
}
/*:62*//*63:*/
#line 979 "hint.w"

void hfill_page_template(void)
{pointer p;
#if 0
print_str("\npage_head:\n");
show_box(link(page_head));
print_str("\nstream 0:\n");
show_box(streams[0].p);
print_str("\nstream 1:\n");
show_box(streams[1].p);
print_str("\ncontributions:\n");
show_box(link(contrib_head));
#endif
if(cur_page->t!=0)
{
uint8_t*spos= hpos,*sstart= hstart,*send= hend;
hget_section(1);
hpos= hpos+cur_page->t;
p= hget_list_pointer();
hpos= spos,hstart= sstart,hend= send;
if(streams[0].p!=null)flush_node_list(streams[0].p);
streams[0].p= streams[0].t= null;
streams[0].p= vpackage(p,page_v,exactly,page_max_depth);
}
else
houtput_template0();
hmark_page();
}
/*:63*//*76:*/
#line 1181 "hint.w"


static void hget_node(uint8_t a)
{switch(a)
{
/*56:*/
#line 870 "hint.w"

case TAG(stream_kind,b100):hinsert_stream(HGET8);break;
/*:56*//*91:*/
#line 1429 "hint.w"

case TAG(glyph_kind,1):HGET_GLYPH(1);break;
case TAG(glyph_kind,2):HGET_GLYPH(2);break;
case TAG(glyph_kind,3):HGET_GLYPH(3);break;
case TAG(glyph_kind,4):HGET_GLYPH(4);break;
/*:91*//*93:*/
#line 1444 "hint.w"

case TAG(penalty_kind,0):tail_append(new_penalty(hget_integer_ref(HGET8)));break;
case TAG(penalty_kind,1):{tail_append(new_penalty(HGET8));}break;
case TAG(penalty_kind,2):{int16_t n;HGET16(n);RNG("Penalty",n,-20000,+20000);tail_append(new_penalty(n));}break;
/*:93*//*95:*/
#line 1458 "hint.w"

case TAG(language_kind,b000):(void)HGET8;
case TAG(language_kind,1):
case TAG(language_kind,2):
case TAG(language_kind,3):
case TAG(language_kind,4):
case TAG(language_kind,5):
case TAG(language_kind,6):
case TAG(language_kind,7):break;
/*:95*//*99:*/
#line 1504 "hint.w"

case TAG(rule_kind,b000):tail_append(hget_rule_ref(HGET8));prev_depth= ignore_depth;break;
case TAG(rule_kind,b011):{HGET_RULE(b011);tail_append(p);prev_depth= ignore_depth;}break;
case TAG(rule_kind,b101):{HGET_RULE(b101);tail_append(p);prev_depth= ignore_depth;}break;
case TAG(rule_kind,b001):{HGET_RULE(b001);tail_append(p);prev_depth= ignore_depth;}break;
case TAG(rule_kind,b110):{HGET_RULE(b110);tail_append(p);prev_depth= ignore_depth;}break;
case TAG(rule_kind,b111):{HGET_RULE(b111);tail_append(p);prev_depth= ignore_depth;}break;
/*:99*//*106:*/
#line 1572 "hint.w"

case TAG(glue_kind,b000):tail_append(new_glue(hget_glue_ref(HGET8)));break;
case TAG(glue_kind,b001):{pointer p;HGET_GLUE(b001);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b010):{pointer p;HGET_GLUE(b010);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b011):{pointer p;HGET_GLUE(b011);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b100):{pointer p;HGET_GLUE(b100);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b101):{pointer p;HGET_GLUE(b101);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b110):{pointer p;HGET_GLUE(b110);tail_append(spec2glue(p));}break;
case TAG(glue_kind,b111):{pointer p;HGET_GLUE(b111);tail_append(spec2glue(p));}break;
/*:106*//*118:*/
#line 1861 "hint.w"

case TAG(hbox_kind,b000):{pointer p;HGET_BOX(b000);happend_to_vlist(p);}break;
case TAG(hbox_kind,b001):{pointer p;HGET_BOX(b001);happend_to_vlist(p);}break;
case TAG(hbox_kind,b010):{pointer p;HGET_BOX(b010);happend_to_vlist(p);}break;
case TAG(hbox_kind,b011):{pointer p;HGET_BOX(b011);happend_to_vlist(p);}break;
case TAG(hbox_kind,b100):{pointer p;HGET_BOX(b100);happend_to_vlist(p);}break;
case TAG(hbox_kind,b101):{pointer p;HGET_BOX(b101);happend_to_vlist(p);}break;
case TAG(hbox_kind,b110):{pointer p;HGET_BOX(b110);happend_to_vlist(p);}break;
case TAG(hbox_kind,b111):{pointer p;HGET_BOX(b111);happend_to_vlist(p);}break;
case TAG(vbox_kind,b000):{pointer p;HGET_BOX(b000);type(p)= vlist_node;happend_to_vlist(p);}break;
case TAG(vbox_kind,b001):{pointer p;HGET_BOX(b001);type(p)= vlist_node;happend_to_vlist(p);}break;
case TAG(vbox_kind,b010):{pointer p;HGET_BOX(b010);type(p)= vlist_node;happend_to_vlist(p);}break;
case TAG(vbox_kind,b011):{pointer p;HGET_BOX(b011);type(p)= vlist_node;happend_to_vlist(p);}break;
case TAG(vbox_kind,b100):{pointer p;HGET_BOX(b100);type(p)= vlist_node;happend_to_vlist(p);}break;
case TAG(vbox_kind,b101):{pointer p;HGET_BOX(b101);type(p)= vlist_node;happend_to_vlist(p);}break;
case TAG(vbox_kind,b110):{pointer p;HGET_BOX(b110);type(p)= vlist_node;happend_to_vlist(p);}break;
case TAG(vbox_kind,b111):{pointer p;HGET_BOX(b111);type(p)= vlist_node;happend_to_vlist(p);}break;
/*:118*//*125:*/
#line 1977 "hint.w"

case TAG(hset_kind,b000):{pointer p;HGET_SET(b000);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(hset_kind,b001):{pointer p;HGET_SET(b001);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(hset_kind,b010):{pointer p;HGET_SET(b010);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(hset_kind,b011):{pointer p;HGET_SET(b011);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(hset_kind,b100):{pointer p;HGET_SET(b100);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(hset_kind,b101):{pointer p;HGET_SET(b101);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(hset_kind,b110):{pointer p;HGET_SET(b110);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(hset_kind,b111):{pointer p;HGET_SET(b111);hset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;

case TAG(vset_kind,b000):{pointer p;HGET_SET(b000);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(vset_kind,b001):{pointer p;HGET_SET(b001);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(vset_kind,b010):{pointer p;HGET_SET(b010);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(vset_kind,b011):{pointer p;HGET_SET(b011);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(vset_kind,b100):{pointer p;HGET_SET(b100);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(vset_kind,b101):{pointer p;HGET_SET(b101);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(vset_kind,b110):{pointer p;HGET_SET(b110);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
case TAG(vset_kind,b111):{pointer p;HGET_SET(b111);vset(p,sto,st,sho,sh,x);happend_to_vlist(p);}break;
/*:125*//*130:*/
#line 2119 "hint.w"

case TAG(hpack_kind,b000):HGET_PACK(hpack_kind,b000);break;
case TAG(hpack_kind,b010):HGET_PACK(hpack_kind,b010);break;
case TAG(hpack_kind,b100):HGET_PACK(hpack_kind,b100);break;
case TAG(hpack_kind,b110):HGET_PACK(hpack_kind,b110);break;
case TAG(hpack_kind,b001):HGET_PACK(hpack_kind,b001);break;
case TAG(hpack_kind,b011):HGET_PACK(hpack_kind,b011);break;
case TAG(hpack_kind,b101):HGET_PACK(hpack_kind,b101);break;
case TAG(hpack_kind,b111):HGET_PACK(hpack_kind,b111);break;

case TAG(vpack_kind,b000):HGET_PACK(vpack_kind,b000);break;
case TAG(vpack_kind,b010):HGET_PACK(vpack_kind,b010);break;
case TAG(vpack_kind,b100):HGET_PACK(vpack_kind,b100);break;
case TAG(vpack_kind,b110):HGET_PACK(vpack_kind,b110);break;
case TAG(vpack_kind,b001):HGET_PACK(vpack_kind,b001);break;
case TAG(vpack_kind,b011):HGET_PACK(vpack_kind,b011);break;
case TAG(vpack_kind,b101):HGET_PACK(vpack_kind,b101);break;
case TAG(vpack_kind,b111):HGET_PACK(vpack_kind,b111);break;
/*:130*//*134:*/
#line 2186 "hint.w"

case TAG(kern_kind,b000):{HGET_KERN(b000);}break;
case TAG(kern_kind,b001):{HGET_KERN(b001);}break;
case TAG(kern_kind,b010):{HGET_KERN(b010);}break;
case TAG(kern_kind,b011):{HGET_KERN(b011);}break;
case TAG(kern_kind,b100):{HGET_KERN(b100);}break;
case TAG(kern_kind,b101):{HGET_KERN(b101);}break;
case TAG(kern_kind,b110):{HGET_KERN(b110);}break;
case TAG(kern_kind,b111):{HGET_KERN(b111);}break;
/*:134*//*138:*/
#line 2235 "hint.w"

case TAG(leaders_kind,0):tail_append(hget_leaders_ref(HGET8));break;
case TAG(leaders_kind,1):HGET_LEADERS(1);break;
case TAG(leaders_kind,2):HGET_LEADERS(2);break;
case TAG(leaders_kind,3):HGET_LEADERS(3);break;
case TAG(leaders_kind,b100|1):HGET_LEADERS(b100|1);break;
case TAG(leaders_kind,b100|2):HGET_LEADERS(b100|2);break;
case TAG(leaders_kind,b100|3):HGET_LEADERS(b100|3);break;
/*:138*//*142:*/
#line 2275 "hint.w"

case TAG(baseline_kind,b000):{cur_list.bs_pos= hpos-1;hget_baseline_ref(HGET8);}break;
case TAG(baseline_kind,b010):{HGET_BASELINE(b010);}break;
case TAG(baseline_kind,b011):{HGET_BASELINE(b011);}break;
case TAG(baseline_kind,b100):{HGET_BASELINE(b100);}break;
case TAG(baseline_kind,b101):{HGET_BASELINE(b101);}break;
case TAG(baseline_kind,b110):{HGET_BASELINE(b110);}break;
case TAG(baseline_kind,b111):{HGET_BASELINE(b111);}break;
/*:142*//*147:*/
#line 2335 "hint.w"

case TAG(ligature_kind,0):tail_append(hget_ligature_ref(HGET8));break;
case TAG(ligature_kind,1):HGET_LIG(1);break;
case TAG(ligature_kind,2):HGET_LIG(2);break;
case TAG(ligature_kind,3):HGET_LIG(3);break;
case TAG(ligature_kind,4):HGET_LIG(4);break;
case TAG(ligature_kind,5):HGET_LIG(5);break;
case TAG(ligature_kind,6):HGET_LIG(6);break;
case TAG(ligature_kind,7):HGET_LIG(7);break;
/*:147*//*151:*/
#line 2385 "hint.w"

case TAG(disc_kind,b000):tail_append(hget_hyphen_ref(HGET8));break;
case TAG(disc_kind,b001):{HGET_DISC(b001);tail_append(p);}break;
case TAG(disc_kind,b010):{HGET_DISC(b010);tail_append(p);}break;
case TAG(disc_kind,b011):{HGET_DISC(b011);tail_append(p);}break;
case TAG(disc_kind,b100):{HGET_DISC(b100);tail_append(p);}break;
case TAG(disc_kind,b101):{HGET_DISC(b101);tail_append(p);}break;
case TAG(disc_kind,b110):{HGET_DISC(b110);tail_append(p);}break;
case TAG(disc_kind,b111):{HGET_DISC(b111);tail_append(p);}break;
/*:151*//*156:*/
#line 2448 "hint.w"

case TAG(par_kind,b000):HGET_PAR(b000);break;
case TAG(par_kind,b010):HGET_PAR(b010);break;
case TAG(par_kind,b100):HGET_PAR(b100);break;
case TAG(par_kind,b110):HGET_PAR(b110);break;
/*:156*//*174:*/
#line 2944 "hint.w"

case TAG(math_kind,b000):HGET_MATH(b000);break;
case TAG(math_kind,b001):HGET_MATH(b001);break;
case TAG(math_kind,b010):HGET_MATH(b010);break;
case TAG(math_kind,b100):HGET_MATH(b100);break;
case TAG(math_kind,b101):HGET_MATH(b101);break;
case TAG(math_kind,b110):HGET_MATH(b110);break;
/*:174*//*176:*/
#line 2973 "hint.w"

case TAG(math_kind,b111):tail_append(new_math(0,before));break;
case TAG(math_kind,b011):tail_append(new_math(0,after));break;
/*:176*//*179:*/
#line 2999 "hint.w"

case TAG(adjust_kind,1):HGET_ADJUST(1);break;
/*:179*//*182:*/
#line 3027 "hint.w"

case TAG(table_kind,b000):HGET_TABLE(b000);break;
case TAG(table_kind,b001):HGET_TABLE(b001);break;
case TAG(table_kind,b010):HGET_TABLE(b010);break;
case TAG(table_kind,b011):HGET_TABLE(b011);break;
case TAG(table_kind,b100):HGET_TABLE(b100);break;
case TAG(table_kind,b101):HGET_TABLE(b101);break;
case TAG(table_kind,b110):HGET_TABLE(b110);break;
case TAG(table_kind,b111):HGET_TABLE(b111);break;

case TAG(item_kind,b000):hget_list_pointer();break;
case TAG(item_kind,b001):hget_content();break;
case TAG(item_kind,b010):hget_content();break;
case TAG(item_kind,b011):hget_content();break;
case TAG(item_kind,b100):hget_content();break;
case TAG(item_kind,b101):hget_content();break;
case TAG(item_kind,b110):hget_content();break;
case TAG(item_kind,b111):(void)HGET8;hget_content();break;
/*:182*//*187:*/
#line 3123 "hint.w"

case TAG(stream_kind,b000):HGET_STREAM(b000);break;
case TAG(stream_kind,b010):HGET_STREAM(b010);break;
/*:187*//*191:*/
#line 3160 "hint.w"

case TAG(image_kind,b000):hget_image_ref(HGET8);break;
case TAG(image_kind,b100):HGET_IMAGE(b100);break;
case TAG(image_kind,b101):HGET_IMAGE(b101);break;
case TAG(image_kind,b110):HGET_IMAGE(b110);break;
case TAG(image_kind,b111):HGET_IMAGE(b111);break;
/*:191*//*195:*/
#line 3198 "hint.w"

case TAG(link_kind,b000):HGET_LINK(b000);break;
case TAG(link_kind,b001):HGET_LINK(b001);break;
case TAG(link_kind,b010):HGET_LINK(b010);break;
case TAG(link_kind,b011):HGET_LINK(b011);break;
/*:195*/
#line 1186 "hint.w"

default:
TAGERR(a);
}
}

void hget_content(void)
{/*74:*/
#line 1161 "hint.w"

uint8_t a,z;
HGETTAG(a);
/*:74*/
#line 1193 "hint.w"

node_pos= (hpos-hstart)-1;
hget_node(a);
/*75:*/
#line 1166 "hint.w"

HGETTAG(z);
if(a!=z)
QUIT("Tag mismatch [%s,%d]!=[%s,%d] at 0x%x to "SIZE_F"\n",
NAME(a),INFO(a),NAME(z),INFO(z),node_pos,hpos-hstart-1);
/*:75*/
#line 1196 "hint.w"

if(nest_ptr==0&&tail!=head&&(type(tail)==penalty_node||type(tail)==glue_node||type(tail)==kern_node))
store_map(tail,node_pos,0);
}

static pointer hget_definition(uint8_t a)
{pointer p;
if(link(head)!=null)QUIT("Calling get_node with nonempty curent list");
hget_node(a);
p= link(head);
if(p!=null&&link(p)!=null)QUIT("get_node returns multiple nodes");
link(head)= null;
tail= head;
return p;
}
/*:76*//*163:*/
#line 2769 "hint.w"

void set_line_break_params(void)
{hset_param_list(line_break_params);
}
/*:163*//*199:*/
#line 3331 "hint.w"

pointer skip(uint8_t n)
{return cur_page->s[n].g;}
pointer*box_ptr(uint8_t n)
{return&streams[n].p;}
int count(uint8_t n)
{return cur_page->s[n].f;}
scaled dimen(uint8_t n)
{return xdimen(&cur_page->s[n].x);}

/*:199*//*200:*/
#line 3356 "hint.w"

void hpage_init(void)
{int i;
if(streams==NULL||cur_page==NULL)return;
for(i= 0;i<=max_ref[stream_kind];i++)
if(streams[i].p!=null)
{flush_node_list(streams[i].p);
streams[i].p= streams[i].t= null;
}
page_contents= empty;page_tail= page_head;link(page_head)= null;
hset_cur_page();
hset_margins();
page_depth= page_height= 0;
memset(top_so_far,0,sizeof(top_so_far));
max_depth= cur_page->d;
if(top_skip!=cur_page->g)
{delete_glue_ref(top_skip);
top_skip= cur_page->g;
add_glue_ref(top_skip);
}
}
/*:200*//*202:*/
#line 3387 "hint.w"

void hflush_contribution_list(void)
{if(link(contrib_head)!=null)
{flush_node_list(link(contrib_head));
link(contrib_head)= null;tail= contrib_head;
}
}
/*:202*//*204:*/
#line 3431 "hint.w"

static bool hbuild_page_up(void)
{
static scaled page_top_height;
pointer p;
pointer q,r;
int b,c;
int pi= 0;
if(link(contrib_head)==null)return false;
do{p= link(contrib_head);
/*209:*/
#line 3553 "hint.w"

switch(type(p)){
case hlist_node:case vlist_node:case rule_node:
/*206:*/
#line 3497 "hint.w"

if(page_contents<box_there)
{if(page_contents==empty)freeze_page_specs(box_there);
else page_contents= box_there;
if(depth(p)> page_max_depth)
page_total= depth(p)-page_max_depth;
depth(p)= 0;
/*207:*/
#line 3523 "hint.w"

{page_top_height= width(top_skip);
page_total= page_total+page_top_height;
}
/*:207*/
#line 3504 "hint.w"

}
/*208:*/
#line 3532 "hint.w"

{int i;
for(i= 1;i<=6;i++)
{page_so_far[i]+= top_so_far[i];
top_so_far[i]= 0;
}
}
/*:208*/
#line 3506 "hint.w"

page_total+= page_height+depth(p);
if(height(p)> page_top_height)
{page_total= page_total+height(p)-page_top_height;
page_height= page_top_height;
}
else
page_height= height(p);
/*:206*/
#line 3556 "hint.w"
goto contribute;
case whatsit_node:goto contribute;
case glue_node:/*212:*/
#line 3591 "hint.w"

if(link(p)==null)return false;
/*213:*/
#line 3598 "hint.w"

#define top_shrink top_so_far[6]
#define top_total top_so_far[1]

{pointer q= glue_ptr(p);
top_so_far[2+stretch_order(q)]+= stretch(q);
if((shrink_order(q)!=normal)&&(shrink(q)!=0))
DBG(DBGTEX,"Infinite glue shrinkage found on current page");
top_shrink+= shrink(q);
top_total+= width(q);
}
/*:213*/
#line 3593 "hint.w"

if(page_contents==empty||!precedes_break(link(p)))goto contribute;
pi= 0;
/*:212*/
#line 3558 "hint.w"
break;
case kern_node:/*214:*/
#line 3614 "hint.w"

top_total+= width(p);
if(page_contents==empty||
link(page_head)==null||
type(link(page_head))!=glue_node)
goto contribute;
pi= 0;
/*:214*/
#line 3559 "hint.w"
break;
case penalty_node:if(page_contents==empty)goto done1;else pi= penalty(p);break;
case ins_node:happend_insertion(p);goto contribute;
default:DBG(DBGTEX,"Unexpected node type %d in build_page_up ignored\n",type(p));
}
/*217:*/
#line 3648 "hint.w"

if(pi<inf_penalty)
{/*215:*/
#line 3628 "hint.w"

/*216:*/
#line 3639 "hint.w"

if(page_total<page_goal)
{if((page_so_far[3]!=0)||(page_so_far[4]!=0)||(page_so_far[5]!=0))b= 0;
else b= badness(page_goal-page_total,page_so_far[2]);
}
else if(page_total-page_goal> page_shrink)b= awful_bad;
else b= badness(page_total-page_goal,page_shrink)
/*:216*/
#line 3629 "hint.w"
;
if(b<awful_bad)
{if(pi<=eject_penalty)c= pi;
else if(b<inf_bad)c= b+pi+insert_penalties;
else c= deplorable;
}
else c= b;
if(insert_penalties>=10000)c= awful_bad;
/*:215*/
#line 3650 "hint.w"

if(c<=least_page_cost)
{best_page_break= p;best_size= page_goal;
least_page_cost= c;
r= link(page_ins_head);
while(r!=page_ins_head)
{best_ins_ptr(r)= last_ins_ptr(r);
r= link(r);
}
}
if((c==awful_bad)||(pi<=eject_penalty))
{
/*218:*/
#line 3678 "hint.w"

if(p!=best_page_break)
{while(link(page_head)!=best_page_break)
{q= link(page_head);
link(page_head)= link(q);
link(q)= null;
link(q)= link(head);
link(head)= q;
}
}
/*:218*/
#line 3662 "hint.w"

/*219:*/
#line 3696 "hint.w"

hloc_set_prev(link(page_head));
while(true){
q= link(page_head);
if(q==null)return false;
else if(q==best_page_break)
break;
else if(type(q)==penalty_node||type(q)==glue_node||type(q)==kern_node)
{link(page_head)= link(q);link(q)= null;flush_node_list(q);}
else break;
}
temp_ptr= new_spec(top_skip);
q= new_glue(temp_ptr);glue_ref_count(temp_ptr)= null;
if(width(temp_ptr)> page_height)width(temp_ptr)= width(temp_ptr)-page_height;
else width(temp_ptr)= 0;
link(q)= link(page_head);
link(page_head)= q;
best_page_break= null;
/*:219*/
#line 3663 "hint.w"

hpack_page();
hfill_page_template();
return true;
}
}
/*:217*/
#line 3564 "hint.w"

contribute:
/*210:*/
#line 3571 "hint.w"

link(contrib_head)= link(p);
link(p)= link(page_head);
if(link(page_head)==null)page_tail= p;
link(page_head)= p;
goto done;
/*:210*/
#line 3566 "hint.w"

done1:/*211:*/
#line 3579 "hint.w"

link(contrib_head)= link(p);link(p)= null;flush_node_list(p);
/*:211*/
#line 3567 "hint.w"

done:
/*:209*/
#line 3441 "hint.w"
;
}while(link(contrib_head)!=null);
tail= contrib_head;
return false;
}
/*:204*//*221:*/
#line 3772 "hint.w"

static void clear_map(void)
{memset(map,0,sizeof(map));
}
/*:221*//*222:*/
#line 3785 "hint.w"

void store_map(pointer p,uint32_t pos0,uint32_t offset)
{map[p]= pos0;
map[p+1]= offset;
}

uint32_t hposition(pointer p)
{return map[p];
}
/*:222*//*229:*/
#line 3895 "hint.w"

void hloc_init(void)
{cur_loc= 0;
hloc_clear();
page_loc[cur_loc]= 0;
DBG(DBGPAGE,"loc_init: %d < %d < %d\n",lo_loc,cur_loc,hi_loc);
}
/*:229*//*234:*/
#line 4031 "hint.w"

int hint_begin(void)
{if(!hint_map())return 0;
hpos= hstart= hin_addr;
hend= hstart+hin_size;
hint_clear_fonts(true);
hflush_contribution_list();hpage_init();
flush_node_list(link(page_head));
free_definitions();
mem_init();
list_init();
hclear_dir();
hclear_fonts();
hget_banner();
if(!hcheck_banner("hint"))
{hint_unmap();return 0;}
hget_directory();
hget_definition_section();
hget_content_section();
leak_clear();
clear_map();
hloc_init();
return 1;
}

void hint_end(void)
{if(hin_addr==NULL)return;
hint_unmap();
hin_addr= hpos= hstart= hend= NULL;
hflush_contribution_list();hpage_init();
flush_node_list(link(page_head));
free_definitions();
list_leaks();
hclear_dir();
}
/*:234*//*238:*/
#line 4134 "hint.w"

bool hint_forward(void)
{hpage_init();
if(hbuild_page())return true;
while(hpos<hend)
{hget_content();
if(hbuild_page())return true;
}
while(!flush_pages(hend-hstart))
if(hbuild_page())return true;
return false;
}
/*:238*//*239:*/
#line 4175 "hint.w"

bool hint_backward(void)
{hpage_init();
if(hbuild_page_up())return true;
while(hpos> hstart)
{hteg_content();
if(hbuild_page_up())return true;
}
while(!flush_pages(0x0))
if(hbuild_page_up())return true;
return false;
}
/*:239*//*241:*/
#line 4203 "hint.w"

bool flush_pages(uint32_t pos)
{pointer p= link(head);
while(p!=null&&
(type(p)==penalty_node||type(p)==glue_node||type(p)==kern_node))
p= link(p);
if(p==null&&link(page_head)==null)return true;
tail_append(new_null_box());
width(tail)= hhsize;
tail_append(hget_param_glue(fill_skip_no));
store_map(tail,pos,0);
tail_append(new_penalty(-010000000000));
store_map(tail,pos,0);
return false;
}
/*:241*//*263:*/
#line 4716 "hint.w"

int hint_get_outline_max(void)
{return max_outline;}
/*:263*//*268:*/
#line 4796 "hint.w"

hint_Outline*hint_get_outlines(void)
{return hint_outlines;
}
/*:268*//*270:*/
#line 4855 "hint.w"

static bool trv_ignore= false;
static bool trv_skip_space= false;
static void(*trv_stream)(uint32_t c);
void trv_init(void(*f)(uint32_t c))
{trv_ignore= false;trv_skip_space= false;trv_stream= f;}

static void trv_char(uint32_t c)
{if(c==0x20)trv_skip_space= true;
else
{if(trv_skip_space)
{trv_skip_space= false;trv_stream(0x20);}
trv_stream(c);
}
}

void trv_hlist(pointer p)
{while(p!=null)
{if(is_char_node(p))
{if(!trv_ignore)trv_char(character(p));
}
else switch(type(p))
{case hlist_node:if(list_ptr(p)!=null)trv_hlist(list_ptr(p));break;
case vlist_node:if(list_ptr(p)!=null)trv_vlist(list_ptr(p));break;
case ligature_node:
if(!trv_ignore)
{pointer q= lig_ptr(p);
while(q!=null)
{trv_char(character(q));
q= link(q);
}
}
break;
case glue_node:
if(!trv_ignore)trv_skip_space= true;
break;
case whatsit_node:
if(subtype(p)==ignore_node)
{if(ignore_info(p)==1)
{trv_hlist(ignore_list(p));
trv_ignore= true;
}
else
trv_ignore= false;
}
break;
default:break;
}
p= link(p);
}
}

void trv_vlist(pointer p)
{while(p!=null)
{switch(type(p))
{case hlist_node:if(list_ptr(p)!=null)trv_hlist(list_ptr(p));
if(!trv_ignore)trv_skip_space= true;
break;
case vlist_node:if(list_ptr(p)!=null)trv_vlist(list_ptr(p));break;
default:break;
}
p= link(p);
}
}
/*:270*/
#line 7546 "hint.w"


/*:372*/
