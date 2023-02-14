	/*526:*/
	#line 10759 "format.w"

#ifndef _HFORMAT_H_
#define _HFORMAT_H_
	/*366:*/
	#line 8063 "format.w"

#define REF_RNG(K,N) if ((int)(N)>max_ref[K]) QUIT("Reference %d to %s out of range [0 - %d]",\
  (N),definition_name[K],max_ref[K])
	/*:366*/	/*367:*/
	#line 8077 "format.w"

#define MAX_REF(K) ((K)==label_kind?0xFFFF:0xFF)
	/*:367*/	/*443:*/
	#line 9552 "format.w"

#ifdef WIN32
#define SIZE_F "0x%x"
#else
#define SIZE_F "0x%zx"
#endif
#ifdef DEBUG
#define DBG(FLAGS,...) ((debugflags & (FLAGS))?LOG(__VA_ARGS__):0)
#else
#define DBG(FLAGS,...) (void)0
#endif
#define DBGTAG(A,P) DBG(DBGTAGS,"tag [%s,%d] at " SIZE_F "\n",NAME(A),INFO(A),(P)-hstart)

#define RNG(S,N,A,Z) \
  if ((int)(N)<(int)(A)||(int)(N)>(int)(Z)) QUIT(S " %d out of range [%d - %d]",N,A,Z)

#define TAGERR(A) QUIT("Unknown tag [%s,%d] at " SIZE_F "\n",NAME(A),INFO(A),hpos-hstart)
	/*:443*/
	#line 10762 "format.w"

	/*430:*/
	#line 9291 "format.w"

#define DBGNONE     0x0
#define DBGBASIC    0x1
#define DBGTAGS     0x2
#define DBGNODE     0x4
#define DBGDEF      0x8
#define DBGDIR      0x10
#define DBGRANGE    0x20
#define DBGFLOAT    0x40
#define DBGCOMPRESS 0x80
#define DBGBUFFER   0X100
#define DBGFLEX     0x200
#define DBGBISON    0x400
#define DBGTEX      0x800
#define DBGPAGE     0x1000
#define DBGFONT     0x2000
#define DBGRENDER   0x4000
#define DBGLABEL    0x8000
	/*:430*/
	#line 10763 "format.w"

	/*13:*/
	#line 647 "format.w"

#define KIND(T)      (((T)>>3)&0x1F)
#define NAME(T)      content_name[KIND(T)]
#define INFO(T)      ((T)&0x7)
#define TAG(K,I)     (((K)<<3)|(I))
	/*:13*/	/*78:*/
	#line 1651 "format.w"

#define ROUND(X)     ((int)((X)>=0.0?floor((X)+0.5):ceil((X)-0.5)))
	/*:78*/	/*113:*/
	#line 2153 "format.w"

#define RUNNING_DIMEN 0xC0000000
	/*:113*/	/*132:*/
	#line 2495 "format.w"

#define ZERO_GLUE(G) ((G).w.w==0  && (G).w.h==0.0  && (G).w.v==0.0  && (G).p.f==0.0 && (G).m.f==0.0)
	/*:132*/	/*244:*/
	#line 5177 "format.w"

#define HINT_NO_POS 0xFFFFFFFF
	/*:244*/	/*251:*/
	#line 5375 "format.w"

#define LABEL_UNDEF 0
#define LABEL_TOP 1
#define LABEL_BOT 2
#define LABEL_MID 3
	/*:251*/	/*311:*/
	#line 6719 "format.w"

#define ALLOCATE(R,S,T) ((R)= (T *)calloc((S),sizeof(T)),\
        (((R)==NULL)?QUIT("Out of memory for " #R):0))
#define REALLOCATE(R,S,T) ((R)= (T *)realloc((R),(S)*sizeof(T)),\
        (((R)==NULL)?QUIT("Out of memory for " #R):0))
	/*:311*/	/*319:*/
	#line 6884 "format.w"

#define MAX_BANNER 256
	/*:319*/	/*332:*/
	#line 7095 "format.w"

#define MAX_TAG_DISTANCE 32
	/*:332*/	/*446:*/
	#line 9617 "format.w"

#define NODE_SIZE(H,T) ((T)==0?(H)+2:-4*((H)+1)+((T)-1))
#define NODE_HEAD(N)   ((N)>0?(N)-2:-((N)>>2)-1)
#define NODE_TAIL(N)   ((N)<0?((N)&0x3)+1:0)
	/*:446*/
	#line 10764 "format.w"

	/*6:*/
	#line 496 "format.w"

#define DEF_KIND(C,D,N) C##_kind= N
typedef enum{	/*9:*/
	#line 564 "format.w"

DEF_KIND(list,list,0),
DEF_KIND(param,param,1),
DEF_KIND(range,range,2),
DEF_KIND(xdimen,xdimen,3),
DEF_KIND(adjust,adjust,4),
DEF_KIND(glyph,font,5),
DEF_KIND(kern,dimen,6),
DEF_KIND(glue,glue,7),
DEF_KIND(ligature,ligature,8),
DEF_KIND(disc,disc,9),
DEF_KIND(language,language,10),
DEF_KIND(rule,rule,11),
DEF_KIND(image,image,12),
DEF_KIND(leaders,leaders,13),
DEF_KIND(baseline,baseline,14),
DEF_KIND(hbox,hbox,15),
DEF_KIND(vbox,vbox,16),
DEF_KIND(par,par,17),
DEF_KIND(math,math,18),
DEF_KIND(table,table,19),
DEF_KIND(item,item,20),
DEF_KIND(hset,hset,21),
DEF_KIND(vset,vset,22),
DEF_KIND(hpack,hpack,23),
DEF_KIND(vpack,vpack,24),
DEF_KIND(stream,stream,25),
DEF_KIND(page,page,26),
DEF_KIND(link,label,27),
DEF_KIND(undefined1,undefined1,28),
DEF_KIND(undefined2,undefined2,29),
DEF_KIND(undefined3,undefined3,30),
DEF_KIND(penalty,int,31)

	/*:9*/
	#line 498 "format.w"
,	/*10:*/
	#line 603 "format.w"

font_kind= glyph_kind,int_kind= penalty_kind,unknown_kind= penalty_kind,dimen_kind= kern_kind,label_kind= link_kind,outline_kind= link_kind
	/*:10*/
	#line 498 "format.w"
}Kind;
#undef DEF_KIND
	/*:6*/	/*11:*/
	#line 629 "format.w"

typedef enum{b000= 0,b001= 1,b010= 2,b011= 3,b100= 4,b101= 5,b110= 6,b111= 7}Info;
	/*:11*/	/*12:*/
	#line 642 "format.w"

typedef uint8_t Tag;
	/*:12*/	/*57:*/
	#line 1310 "format.w"


#define FLT_M_BITS 23
#define FLT_E_BITS 8
#define FLT_EXCESS 127

#define DBL_M_BITS 52
#define DBL_E_BITS 11
#define DBL_EXCESS 1023

	/*:57*/	/*77:*/
	#line 1646 "format.w"

typedef int32_t Scaled;
#define ONE ((Scaled)(1<<16))
	/*:77*/	/*82:*/
	#line 1702 "format.w"

typedef Scaled Dimen;
#define MAX_DIMEN ((Dimen)(0x3FFFFFFF))
	/*:82*/	/*87:*/
	#line 1758 "format.w"

typedef struct{
Dimen w;float32_t h,v;
}Xdimen;
	/*:87*/	/*96:*/
	#line 1902 "format.w"

typedef enum{normal_o= 0,fil_o= 1,fill_o= 2,filll_o= 3}Order;
typedef struct{float64_t f;Order o;}Stretch;
typedef union{float32_t f;uint32_t u;}Stch;
	/*:96*/	/*131:*/
	#line 2485 "format.w"

typedef struct{
Xdimen w;
Stretch p,m;
}Glue;
	/*:131*/	/*180:*/
	#line 3939 "format.w"

typedef struct{
Glue bs,ls;
Dimen lsl;
}Baseline;
	/*:180*/	/*250:*/
	#line 5360 "format.w"

typedef struct
{uint32_t pos;
uint8_t where;
bool used;
int next;
uint32_t pos0;uint8_t f;
}Label;
	/*:250*/	/*390:*/
	#line 8598 "format.w"

#define MAX_FONT_PARAMS 11
	/*:390*/
	#line 10765 "format.w"

	/*403:*/
	#line 8837 "format.w"

typedef enum{
zero_int_no= 0,
pretolerance_no= 1,
tolerance_no= 2,
line_penalty_no= 3,
hyphen_penalty_no= 4,
ex_hyphen_penalty_no= 5,
club_penalty_no= 6,
widow_penalty_no= 7,
display_widow_penalty_no= 8,
broken_penalty_no= 9,
pre_display_penalty_no= 10,
post_display_penalty_no= 11,
inter_line_penalty_no= 12,
double_hyphen_demerits_no= 13,
final_hyphen_demerits_no= 14,
adj_demerits_no= 15,
looseness_no= 16,
time_no= 17,
day_no= 18,
month_no= 19,
year_no= 20,
hang_after_no= 21,
floating_penalty_no= 22
}Int_no;
#define MAX_INT_DEFAULT floating_penalty_no
	/*:403*/	/*405:*/
	#line 8910 "format.w"

typedef enum{
zero_dimen_no= 0,
hsize_dimen_no= 1,
vsize_dimen_no= 2,
line_skip_limit_no= 3,
max_depth_no= 4,
split_max_depth_no= 5,
hang_indent_no= 6,
emergency_stretch_no= 7,
quad_no= 8,
math_quad_no= 9
}Dimen_no;
#define MAX_DIMEN_DEFAULT math_quad_no
	/*:405*/	/*407:*/
	#line 8955 "format.w"

typedef enum{
zero_xdimen_no= 0,
hsize_xdimen_no= 1,
vsize_xdimen_no= 2
}Xdimen_no;
#define MAX_XDIMEN_DEFAULT vsize_xdimen_no
	/*:407*/	/*409:*/
	#line 8979 "format.w"

typedef enum{
zero_skip_no= 0,
fil_skip_no= 1,
fill_skip_no= 2,
line_skip_no= 3,
baseline_skip_no= 4,
above_display_skip_no= 5,
below_display_skip_no= 6,
above_display_short_skip_no= 7,
below_display_short_skip_no= 8,
left_skip_no= 9,
right_skip_no= 10,
top_skip_no= 11,
split_top_skip_no= 12,
tab_skip_no= 13,
par_fill_skip_no= 14
}Glue_no;
#define MAX_GLUE_DEFAULT par_fill_skip_no
	/*:409*/	/*411:*/
	#line 9065 "format.w"

typedef enum{
zero_baseline_no= 0
}Baseline_no;
#define MAX_BASELINE_DEFAULT zero_baseline_no
	/*:411*/	/*413:*/
	#line 9090 "format.w"

typedef enum{
zero_label_no= 0
}Label_no;
#define MAX_LABEL_DEFAULT zero_label_no
	/*:413*/	/*415:*/
	#line 9104 "format.w"

typedef enum{
zero_stream_no= 0
}Stream_no;
#define MAX_STREAM_DEFAULT zero_stream_no
	/*:415*/	/*417:*/
	#line 9120 "format.w"

typedef enum{
zero_page_no= 0
}Page_no;
#define MAX_PAGE_DEFAULT zero_page_no
	/*:417*/	/*419:*/
	#line 9137 "format.w"

typedef enum{
zero_range_no= 0
}Range_no;
#define MAX_RANGE_DEFAULT zero_range_no
	/*:419*/	/*421:*/
	#line 9150 "format.w"

typedef enum{
empty_list_no= 0
}List_no;
#define MAX_LIST_DEFAULT empty_list_no
	/*:421*/
	#line 10766 "format.w"


extern const char*content_name[32];
extern const char*definition_name[32];
extern unsigned int debugflags;
extern FILE*hlog;
extern int max_fixed[32],max_default[32],max_ref[32],max_outline;
extern int32_t int_defaults[MAX_INT_DEFAULT+1];
extern Dimen dimen_defaults[MAX_DIMEN_DEFAULT+1];
extern Xdimen xdimen_defaults[MAX_XDIMEN_DEFAULT+1];
extern Glue glue_defaults[MAX_GLUE_DEFAULT+1];
extern Baseline baseline_defaults[MAX_BASELINE_DEFAULT+1];
extern Label label_defaults[MAX_LABEL_DEFAULT+1];
extern signed char hnode_size[0x100];
extern uint8_t content_known[32];

#endif
	/*:526*/
