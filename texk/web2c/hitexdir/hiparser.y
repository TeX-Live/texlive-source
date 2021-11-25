	/*512:*/
%{
	#line 10314 "format.w"
	
#include "hibasetypes.h"
#include <string.h>
#include <math.h>
#include "hierror.h"
#include "hiformat.h"
#include "hiput.h"
extern char**hfont_name;

	/*358:*/
uint32_t definition_bits[0x100/32][32]= {
	#line 7627 "format.w"
	{0}};

#define SET_DBIT(N,K) ((N)>0xFF?1:(definition_bits[N/32][K]	|= (1<<((N)&(32-1)))))
#define GET_DBIT(N,K) ((N)>0xFF?1:((definition_bits[N/32][K]>>((N)&(32-1)))&1))
#define DEF(D,K,N) (D).k= K; (D).n= (N);SET_DBIT((D).n,(D).k);\
 DBG(DBGDEF,"Defining %s %d\n",definition_name[(D).k],(D).n);\
 RNG("Definition",(D).n,max_fixed[(D).k]+1,max_ref[(D).k]);
#define REF(K,N) REF_RNG(K,N);if(!GET_DBIT(N,K)) \
 QUIT("Reference %d to %s before definition",(N),definition_name[K])
	/*:358*/	/*362:*/
#define DEF_REF(D,K,M,N)  DEF(D,K,M);\
if ((M)>max_default[K]) QUIT("Defining non default reference %d for %s",M,definition_name[K]); \
if ((N)>max_fixed[K]) QUIT("Defining reference %d for %s by non fixed reference %d",M,definition_name[K],N);
	/*:362*/

extern void hset_entry(entry_t*e,uint16_t i,uint32_t size,
uint32_t xsize,char*file_name);

	/*425:*/
#ifdef DEBUG
#define  YYDEBUG 1
extern int yydebug;
#else
#define YYDEBUG 0
#endif
	/*:425*/
extern int yylex(void);

	/*354:*/
void hset_max(kind_t k,int n)
{
	#line 7470 "format.w"
	DBG(DBGDEF,"Setting max %s to %d\n",definition_name[k],n);
	RNG("Maximum",n,max_fixed[k]+1,MAX_REF(k));
	if(n>max_ref[k])
	max_ref[k]= n;
	}
	/*:354*/	/*365:*/
void check_param_def(ref_t*df)
{
	#line 7776 "format.w"
	if(df->k!=int_kind&&df->k!=dimen_kind&&df->k!=glue_kind)
	QUIT("Kind %s not allowed in parameter list",definition_name[df->k]);
	if(df->n<=max_fixed[df->k]||max_default[df->k]<df->n)
	QUIT("Parameter %d for %s not allowed in parameter list",df->n,definition_name[df->k]);
	}
	/*:365*/	/*424:*/
extern int yylineno;
int yyerror(const char*msg)
{
	#line 8901 "format.w"
	QUIT(" in line %d %s",yylineno,msg);
	return 0;
	}
	/*:424*/


%}





%union {
	#line 10339 "format.w"
	uint32_t u;  int32_t i;  char *s;  float64_t f;  glyph_t c;
	dimen_t d;stretch_t st;xdimen_t xd;kern_t kt;
	rule_t r;glue_t g;image_t x;
	list_t l;box_t h;disc_t dc;lig_t lg;
	ref_t rf;info_t info;order_t o;bool b;
	}



%error_verbose
%start hint

	/*2:*/
%token START    "<"
%token END      ">"
%token GLYPH     "glyph"
%token <u> UNSIGNED
%token <u> REFERENCE
	/*:2*/	/*4:*/
%type <u> start
%type <c> glyph
	/*:4*/	/*26:*/
%token <i> SIGNED
%type <i> integer
	/*:26*/	/*33:*/
%token <s> STRING
	/*:33*/	/*47:*/
%token <u> CHARCODE
	/*:47*/	/*49:*/
%type <s> string
	/*:49*/	/*54:*/
%token <f> FPNUM
%type <f> number
	/*:54*/	/*79:*/
%token DIMEN "dimen"
%token PT "pt"
%token MM "mm"
%token INCH "in"
%type <d> dimension
	/*:79*/	/*87:*/
%token XDIMEN "xdimen"
%token H "h"
%token V "v"
%type <xd> xdimen
	/*:87*/	/*98:*/
%token FIL "fil"
%token FILL "fill"
%token FILLL "filll"
%type <st> stretch
%type <o> order
	/*:98*/	/*102:*/
%token PENALTY "penalty"
%token INTEGER     "int"
%type <i> penalty
	/*:102*/	/*108:*/
%token LANGUAGE "language"
	/*:108*/	/*114:*/
%token RULE "rule"
%token RUNNING "|"
%type <d> rule_dimension
%type <r> rule
	/*:114*/	/*123:*/
%token KERN "kern"
%token EXPLICIT "!"
%type <b> explicit
%type <kt> kern
	/*:123*/	/*132:*/
%token GLUE "glue"
%token PLUS  "plus"
%token MINUS   "minus"
%type <g> glue
%type <b> glue_node
%type <st> plus minus
	/*:132*/	/*141:*/
%type <l>  list
%type <u> position content_list
	/*:141*/	/*150:*/
%token TXT_START TXT_END TXT_IGNORE
%token TXT_FONT_GLUE TXT_FONT_HYPHEN
%token <u> TXT_FONT TXT_LOCAL
%token <rf> TXT_GLOBAL
%token <u> TXT_CC
%type <u> text
	/*:150*/	/*161:*/
%token HBOX     "hbox"
%token VBOX     "vbox"
%token SHIFTED  "shifted"
%type <info> box box_dimen box_shift box_glue_set

	/*:161*/	/*169:*/
%token HPACK "hpack"
%token HSET  "hset"
%token VPACK "vpack"
%token VSET  "vset"
%token DEPTH "depth"
%token ADD "add"
%token TO "to"
%type <info> xbox box_goal hpack vpack
	/*:169*/	/*174:*/
%token LEADERS "leaders"
%token ALIGN "align"
%token CENTER "center"
%token EXPAND "expand"
%type <info> leaders
%type <info> ltype
	/*:174*/	/*181:*/
%token BASELINE "baseline"
%type <info> baseline
	/*:181*/	/*188:*/
%token LIGATURE     "ligature"
%type <u>  lig_cc
%type <lg> ligature
%type <u> ref
	/*:188*/	/*196:*/
%token DISC     "disc"
%type <dc> disc
%type <u> replace_count
	/*:196*/	/*204:*/
%token PAR "par"
%type <info> par
	/*:204*/	/*209:*/
%token MATH "math"
%type <info> math
	/*:209*/	/*214:*/
%token ON "on"
%token OFF "off"
%type <i> on_off
	/*:214*/	/*218:*/
%token ADJUST "adjust"
	/*:218*/	/*222:*/
%token TABLE "table"
%token ITEM "item"
%type <info> table span_count
	/*:222*/	/*229:*/
%token IMAGE "image"
%type <x> image image_dimen
	/*:229*/	/*246:*/
%token LABEL "label"
%token BOT "bot"
%token MID "mid"
%type <i> placement
	/*:246*/	/*260:*/
%token LINK "link"
	/*:260*/	/*270:*/
%token OUTLINE "outline"
	/*:270*/	/*277:*/
%token STREAM "stream"
%token STREAMDEF "stream (definition)"
%token FIRST "first"
%token LAST "last"
%token TOP "top"
%token NOREFERENCE "*"
%type <info> stream_type
%type <u> stream_ref
%type <rf> stream_def_node
	/*:277*/	/*283:*/
%type <info> stream
	/*:283*/	/*287:*/
%token PAGE "page"
	/*:287*/	/*295:*/
%token RANGE "range"
	/*:295*/	/*322:*/
%token DIRECTORY "directory"
%token SECTION "entry"
	/*:322*/	/*343:*/
%token DEFINITIONS "definitions"
	/*:343*/	/*351:*/
%token MAX "max"
	/*:351*/	/*360:*/

%type <rf> def_node
	/*:360*/	/*366:*/
%token PARAM "param"
%type <u> def_list
%type <l> parameters
	/*:366*/	/*375:*/
%token FONT     "font"
%type <info> font font_head
	/*:375*/	/*403:*/
%token CONTENT "content"
	/*:403*/
%%
	/*5:*/
glyph:UNSIGNED REFERENCE{
	#line 418 "format.w"
	$$.c= $1;REF(font_kind,$2);$$.f= $2;};
content_node:start GLYPH glyph END{
	#line 419 "format.w"
	hput_tags($1,hput_glyph(&($3)));};
start:START{
	#line 420 "format.w"
	HPUTNODE;$$= (uint32_t)(hpos++-hstart);}
	/*:5*/	/*29:*/
integer:SIGNED	|UNSIGNED{
	#line 941 "format.w"
	RNG("number",$1,0,0x7FFFFFFF);};
	/*:29*/	/*38:*/
glyph:CHARCODE REFERENCE{
	#line 1082 "format.w"
	$$.c= $1;REF(font_kind,$2);$$.f= $2;};
	/*:38*/	/*50:*/
string:STRING	|CHARCODE{
	#line 1187 "format.w"
	static char s[2];
	RNG("String element",$1,0x20,0x7E);
	s[0]= $1;s[1]= 0;$$= s;};
	/*:50*/	/*58:*/
number:UNSIGNED{
	#line 1339 "format.w"
	$$= (float64_t)$1;}	|SIGNED{
	#line 1339 "format.w"
	$$= (float64_t)$1;}	|FPNUM;
	/*:58*/	/*82:*/
dimension:number PT{
	#line 1682 "format.w"
	$$= ROUND($1*ONE);RNG("Dimension",$$,-MAX_DIMEN,MAX_DIMEN);}
	|number INCH{
	#line 1683 "format.w"
	$$= ROUND($1*ONE*72.27);RNG("Dimension",$$,-MAX_DIMEN,MAX_DIMEN);}
	|number MM{
	#line 1684 "format.w"
	$$= ROUND($1*ONE*(72.27/25.4));RNG("Dimension",$$,-MAX_DIMEN,MAX_DIMEN);};
	/*:82*/	/*89:*/
xdimen:dimension number H number V{
	#line 1762 "format.w"
	$$.w= $1;$$.h= $2;$$.v= $4;}
	|dimension number H{
	#line 1763 "format.w"
	$$.w= $1;$$.h= $2;$$.v= 0.0;}
	|dimension number V{
	#line 1764 "format.w"
	$$.w= $1;$$.h= 0.0;$$.v= $2;}
	|dimension{
	#line 1765 "format.w"
	$$.w= $1;$$.h= 0.0;$$.v= 0.0;};



xdimen_node:start XDIMEN xdimen END{
	#line 1769 "format.w"
	hput_tags($1,hput_xdimen(&($3)));};
	/*:89*/	/*100:*/

order:PT{
	#line 1947 "format.w"
	$$= normal_o;}	|FIL{
	#line 1947 "format.w"
	$$= fil_o;}	|FILL{
	#line 1947 "format.w"
	$$= fill_o;}	|FILLL{
	#line 1947 "format.w"
	$$= filll_o;};

stretch:number order{
	#line 1949 "format.w"
	$$.f= $1;$$.o= $2;};
	/*:100*/	/*104:*/
penalty:integer{
	#line 2003 "format.w"
	RNG("Penalty",$1,-20000,+20000);$$= $1;};
content_node:start PENALTY penalty END{
	#line 2004 "format.w"
	hput_tags($1,hput_int($3));};
	/*:104*/	/*116:*/
rule_dimension:dimension	|RUNNING{
	#line 2179 "format.w"
	$$= RUNNING_DIMEN;};
rule:rule_dimension rule_dimension rule_dimension
{
	#line 2181 "format.w"
	$$.h= $1;$$.d= $2;$$.w= $3;
	if($3==RUNNING_DIMEN&&($1==RUNNING_DIMEN||$2==RUNNING_DIMEN))
	QUIT("Incompatible running dimensions 0x%x 0x%x 0x%x",$1,$2,$3);};
rule_node:start RULE rule END{
	#line 2184 "format.w"
	hput_tags($1,hput_rule(&($3)));};
content_node:rule_node;
	/*:116*/	/*125:*/
explicit:{
	#line 2292 "format.w"
	$$= false;}	|EXPLICIT{
	#line 2292 "format.w"
	$$= true;};
kern:explicit xdimen{
	#line 2293 "format.w"
	$$.x= $1;$$.d= $2;};
content_node:start KERN kern END{
	#line 2294 "format.w"
	hput_tags($1,hput_kern(&($3)));}
	/*:125*/	/*134:*/
plus:{
	#line 2504 "format.w"
	$$.f= 0.0;$$.o= 0;}	|PLUS stretch{
	#line 2504 "format.w"
	$$= $2;};
minus:{
	#line 2505 "format.w"
	$$.f= 0.0;$$.o= 0;}	|MINUS stretch{
	#line 2505 "format.w"
	$$= $2;};
glue:xdimen plus minus{
	#line 2506 "format.w"
	$$.w= $1;$$.p= $2;$$.m= $3;};
content_node:start GLUE glue END{
	#line 2507 "format.w"
	if(ZERO_GLUE($3)){HPUT8(zero_skip_no);
	hput_tags($1,TAG(glue_kind,0));}else hput_tags($1,hput_glue(&($3)));};
glue_node:start GLUE glue END
{
	#line 2510 "format.w"
	if(ZERO_GLUE($3)){hpos--;$$= false;}
	else{hput_tags($1,hput_glue(&($3)));$$= true;}};
	/*:134*/	/*142:*/
position:{
	#line 2786 "format.w"
	$$= hpos-hstart;};
content_list:position
	|content_list content_node;
estimate:{
	#line 2789 "format.w"
	hpos+= 2;}
	|UNSIGNED{
	#line 2790 "format.w"
	hpos+= hsize_bytes($1)+1;};
list:start estimate content_list END
{
	#line 2792 "format.w"
	$$.k= list_kind;$$.p= $3;$$.s= (hpos-hstart)-$3;
	hput_tags($1,hput_list($1+1,&($$)));};
	/*:142*/	/*153:*/
list:TXT_START position
{
	#line 3199 "format.w"
	hpos+= 4;}
text TXT_END
{
	#line 3201 "format.w"
	$$.k= text_kind;$$.p= $4;$$.s= (hpos-hstart)-$4;
	hput_tags($2,hput_list($2+1,&($$)));};
text:position	|text txt;

txt:TXT_CC{
	#line 3205 "format.w"
	hput_txt_cc($1);}
	|TXT_FONT{
	#line 3206 "format.w"
	REF(font_kind,$1);hput_txt_font($1);}
	|TXT_GLOBAL{
	#line 3207 "format.w"
	REF($1.k,$1.n);hput_txt_global(&($1));}
	|TXT_LOCAL{
	#line 3208 "format.w"
	RNG("Font parameter",$1,0,11);hput_txt_local($1);}
	|TXT_FONT_GLUE{
	#line 3209 "format.w"
	HPUTX(1);HPUT8(txt_glue);}
	|TXT_FONT_HYPHEN{
	#line 3210 "format.w"
	HPUTX(1);HPUT8(txt_hyphen);}
	|TXT_IGNORE{
	#line 3211 "format.w"
	HPUTX(1);HPUT8(txt_ignore);}
	|{
	#line 3212 "format.w"
	HPUTX(1);HPUT8(txt_node);}content_node;
	/*:153*/	/*163:*/

box_dimen:dimension dimension dimension
{
	#line 3472 "format.w"
	$$= hput_box_dimen($1,$2,$3);};
box_shift:{
	#line 3473 "format.w"
	$$= b000;}
	|SHIFTED dimension{
	#line 3474 "format.w"
	$$= hput_box_shift($2);};

box_glue_set:{
	#line 3476 "format.w"
	$$= b000;}
	|PLUS stretch{
	#line 3477 "format.w"
	$$= hput_box_glue_set(+1,$2.f,$2.o);}
	|MINUS stretch{
	#line 3478 "format.w"
	$$= hput_box_glue_set(-1,$2.f,$2.o);};


box:box_dimen box_shift box_glue_set list{
	#line 3481 "format.w"
	$$= $1	|$2	|$3;};

hbox_node:start HBOX box END{
	#line 3483 "format.w"
	hput_tags($1,TAG(hbox_kind,$3));};
vbox_node:start VBOX box END{
	#line 3484 "format.w"
	hput_tags($1,TAG(vbox_kind,$3));};
content_node:hbox_node	|vbox_node;
	/*:163*/	/*171:*/
box_flex:plus minus{
	#line 3665 "format.w"
	hput_stretch(&($1));hput_stretch(&($2));};
xbox:box_dimen box_shift box_flex xdimen_ref list{
	#line 3666 "format.w"
	$$= $1	|$2;}
	|box_dimen box_shift box_flex xdimen_node list{
	#line 3667 "format.w"
	$$= $1	|$2	|b100;};

box_goal:TO xdimen_ref{
	#line 3669 "format.w"
	$$= b000;}
	|ADD xdimen_ref{
	#line 3670 "format.w"
	$$= b001;}
	|TO xdimen_node{
	#line 3671 "format.w"
	$$= b100;}
	|ADD xdimen_node{
	#line 3672 "format.w"
	$$= b101;};

hpack:box_shift box_goal list{
	#line 3674 "format.w"
	$$= $2;};
vpack:box_shift MAX DEPTH dimension{
	#line 3675 "format.w"
	HPUT32($4);}box_goal list{
	#line 3675 "format.w"
	$$= $1	|$6;};

vxbox_node:start VSET xbox END{
	#line 3677 "format.w"
	hput_tags($1,TAG(vset_kind,$3));}
	|start VPACK vpack END{
	#line 3678 "format.w"
	hput_tags($1,TAG(vpack_kind,$3));};


hxbox_node:start HSET xbox END{
	#line 3681 "format.w"
	hput_tags($1,TAG(hset_kind,$3));}
	|start HPACK hpack END{
	#line 3682 "format.w"
	hput_tags($1,TAG(hpack_kind,$3));};

content_node:vxbox_node	|hxbox_node;
	/*:171*/	/*176:*/
ltype:{
	#line 3792 "format.w"
	$$= 1;}	|ALIGN{
	#line 3792 "format.w"
	$$= 1;}	|CENTER{
	#line 3792 "format.w"
	$$= 2;}	|EXPAND{
	#line 3792 "format.w"
	$$= 3;};
leaders:glue_node ltype rule_node{
	#line 3793 "format.w"
	if($1)$$= $2	|b100;else $$= $2;}
	|glue_node ltype hbox_node{
	#line 3794 "format.w"
	if($1)$$= $2	|b100;else $$= $2;}
	|glue_node ltype vbox_node{
	#line 3795 "format.w"
	if($1)$$= $2	|b100;else $$= $2;};
content_node:start LEADERS leaders END{
	#line 3796 "format.w"
	hput_tags($1,TAG(leaders_kind,$3));}
	/*:176*/	/*183:*/
baseline:dimension{
	#line 3902 "format.w"
	if($1!=0)HPUT32($1);}
glue_node glue_node{
	#line 3903 "format.w"
	$$= b000;if($1!=0)$$	|= b001;
	if($3)$$	|= b100;
	if($4)$$	|= b010;
	};
content_node:start BASELINE baseline END
{
	#line 3908 "format.w"
	if($3==b000)HPUT8(0);hput_tags($1,TAG(baseline_kind,$3));};
	/*:183*/	/*190:*/
cc_list:	|cc_list TXT_CC{
	#line 3991 "format.w"
	hput_utf8($2);};
lig_cc:UNSIGNED{
	#line 3992 "format.w"
	RNG("UTF-8 code",$1,0,0x1FFFFF);$$= hpos-hstart;hput_utf8($1);};
lig_cc:CHARCODE{
	#line 3993 "format.w"
	$$= hpos-hstart;hput_utf8($1);};
ref:REFERENCE{
	#line 3994 "format.w"
	HPUT8($1);$$= $1;};
ligature:ref{
	#line 3995 "format.w"
	REF(font_kind,$1);}lig_cc TXT_START cc_list TXT_END
{
	#line 3996 "format.w"
	$$.f= $1;$$.l.p= $3;$$.l.s= (hpos-hstart)-$3;
	RNG("Ligature size",$$.l.s,0,255);};
content_node:start LIGATURE ligature END{
	#line 3998 "format.w"
	hput_tags($1,hput_ligature(&($3)));};
	/*:190*/	/*198:*/
replace_count:explicit{
	#line 4108 "format.w"
	if($1){$$= 0x80;HPUT8(0x80);}else $$= 0x00;}
	|explicit UNSIGNED{
	#line 4109 "format.w"
	RNG("Replace count",$2,0,31);
	$$= ($2)	|(($1)?0x80:0x00);if($$!=0)HPUT8($$);};
disc:replace_count list list{
	#line 4111 "format.w"
	$$.r= $1;$$.p= $2;$$.q= $3;
	if($3.s==0){hpos= hpos-2;if($2.s==0)hpos= hpos-2;}}
	|replace_count list{
	#line 4113 "format.w"
	$$.r= $1;$$.p= $2;if($2.s==0)hpos= hpos-2;$$.q.s= 0;}
	|replace_count{
	#line 4114 "format.w"
	$$.r= $1;$$.p.s= 0;$$.q.s= 0;};


disc_node:start DISC disc END
{
	#line 4118 "format.w"
	hput_tags($1,hput_disc(&($3)));};

content_node:disc_node;
	/*:198*/	/*206:*/
par_dimen:xdimen{
	#line 4270 "format.w"
	hput_xdimen_node(&($1));};
par:xdimen_ref param_ref list{
	#line 4271 "format.w"
	$$= b000;}
	|xdimen_ref empty_param_list non_empty_param_list list{
	#line 4272 "format.w"
	$$= b010;}
	|xdimen_ref empty_param_list list{
	#line 4273 "format.w"
	$$= b010;}
	|xdimen param_ref{
	#line 4274 "format.w"
	hput_xdimen_node(&($1));}list{
	#line 4274 "format.w"
	$$= b100;}
	|par_dimen empty_param_list non_empty_param_list list{
	#line 4275 "format.w"
	$$= b110;}
	|par_dimen empty_param_list list{
	#line 4276 "format.w"
	$$= b110;};

content_node:start PAR par END{
	#line 4278 "format.w"
	hput_tags($1,TAG(par_kind,$3));};
	/*:206*/	/*211:*/
math:param_ref list{
	#line 4344 "format.w"
	$$= b000;}
	|param_ref list hbox_node{
	#line 4345 "format.w"
	$$= b001;}
	|param_ref hbox_node list{
	#line 4346 "format.w"
	$$= b010;}
	|empty_param_list list{
	#line 4347 "format.w"
	$$= b100;}
	|empty_param_list list hbox_node{
	#line 4348 "format.w"
	$$= b101;}
	|empty_param_list hbox_node list{
	#line 4349 "format.w"
	$$= b110;}
	|empty_param_list non_empty_param_list list{
	#line 4350 "format.w"
	$$= b100;}
	|empty_param_list non_empty_param_list list hbox_node{
	#line 4351 "format.w"
	$$= b101;}
	|empty_param_list non_empty_param_list hbox_node list{
	#line 4352 "format.w"
	$$= b110;};

content_node:start MATH math END{
	#line 4354 "format.w"
	hput_tags($1,TAG(math_kind,$3));};
	/*:211*/	/*216:*/
on_off:ON{
	#line 4404 "format.w"
	$$= 1;}	|OFF{
	#line 4404 "format.w"
	$$= 0;};
math:on_off{
	#line 4405 "format.w"
	$$= b011	|($1<<2);};
	/*:216*/	/*220:*/
content_node:start ADJUST list END{
	#line 4436 "format.w"
	hput_tags($1,TAG(adjust_kind,1));};
	/*:220*/	/*224:*/
span_count:UNSIGNED{
	#line 4535 "format.w"
	$$= hput_span_count($1);};
content_node:start ITEM content_node END{
	#line 4536 "format.w"
	hput_tags($1,TAG(item_kind,1));};
content_node:start ITEM span_count content_node END{
	#line 4537 "format.w"
	hput_tags($1,TAG(item_kind,$3));};
content_node:start ITEM list END{
	#line 4538 "format.w"
	hput_tags($1,TAG(item_kind,b000));};

table:H box_goal list list{
	#line 4540 "format.w"
	$$= $2;};
table:V box_goal list list{
	#line 4541 "format.w"
	$$= $2	|b010;};

content_node:start TABLE table END{
	#line 4543 "format.w"
	hput_tags($1,TAG(table_kind,$3));};
	/*:224*/	/*231:*/
image_dimen:dimension dimension{
	#line 4631 "format.w"
	$$.w= $1;$$.h= $2;}	|{
	#line 4631 "format.w"
	$$.w= $$.h= 0;};
image:UNSIGNED image_dimen plus minus{
	#line 4632 "format.w"
	$$.w= $2.w;$$.h= $2.h;$$.p= $3;$$.m= $4;RNG("Section number",$1,3,max_section_no);$$.n= $1;};
content_node:start IMAGE image END{
	#line 4633 "format.w"
	hput_tags($1,hput_image(&($3)));}
	/*:231*/	/*241:*/
max_value:OUTLINE UNSIGNED{
	#line 4870 "format.w"
	max_outline= $2;
	RNG("max outline",max_outline,0,0xFFFF);
	DBG(DBGDEF	|DBGLABEL,"Setting max outline to %d\n",max_outline);
	};
	/*:241*/	/*248:*/
placement:TOP{
	#line 4962 "format.w"
	$$= LABEL_TOP;}	|BOT{
	#line 4962 "format.w"
	$$= LABEL_BOT;}	|MID{
	#line 4962 "format.w"
	$$= LABEL_MID;}	|{
	#line 4962 "format.w"
	$$= LABEL_MID;};
content_node:START LABEL REFERENCE placement END
{
	#line 4964 "format.w"
	hset_label($3,$4);}
	/*:248*/	/*262:*/
content_node:start LINK REFERENCE on_off END
{
	#line 5222 "format.w"
	hput_tags($1,hput_link($3,$4));};
	/*:262*/	/*272:*/
def_node:START OUTLINE REFERENCE integer position list END{
	#line 5352 "format.w"
	
	static int outline_no= -1;
	$$.k= outline_kind;$$.n= $3;
	if($6.s==0)QUIT("Outline with empty title in line %d",yylineno);
	outline_no++;
	hset_outline(outline_no,$3,$4,$5);
	};
	/*:272*/	/*279:*/
stream_link:ref{
	#line 5767 "format.w"
	REF_RNG(stream_kind,$1);}	|NOREFERENCE{
	#line 5767 "format.w"
	HPUT8(255);};
stream_split:stream_link stream_link UNSIGNED{
	#line 5768 "format.w"
	RNG("split ratio",$3,0,1000);HPUT16($3);};
stream_info:xdimen_node UNSIGNED{
	#line 5769 "format.w"
	RNG("magnification factor",$2,0,1000);HPUT16($2);}stream_split;

stream_type:stream_info{
	#line 5771 "format.w"
	$$= 0;}	|FIRST{
	#line 5771 "format.w"
	$$= 1;}	|LAST{
	#line 5771 "format.w"
	$$= 2;}	|TOP{
	#line 5771 "format.w"
	$$= 3;};

stream_def_node:start STREAMDEF ref stream_type
list xdimen_node glue_node list glue_node END
{
	#line 5775 "format.w"
	DEF($$,stream_kind,$3);hput_tags($1,TAG(stream_kind,$4	|b100));};

stream_ins_node:start STREAMDEF ref END
{
	#line 5778 "format.w"
	RNG("Stream insertion",$3,0,max_ref[stream_kind]);hput_tags($1,TAG(stream_kind,b100));};

content_node:stream_def_node	|stream_ins_node;
	/*:279*/	/*284:*/
stream:empty_param_list list{
	#line 5873 "format.w"
	$$= b010;}
	|empty_param_list non_empty_param_list list{
	#line 5874 "format.w"
	$$= b010;}
	|param_ref list{
	#line 5875 "format.w"
	$$= b000;};
content_node:start STREAM stream_ref stream END
{
	#line 5877 "format.w"
	hput_tags($1,TAG(stream_kind,$4));};
	/*:284*/	/*289:*/
page_priority:{
	#line 5980 "format.w"
	HPUT8(1);}
	|UNSIGNED{
	#line 5981 "format.w"
	RNG("page priority",$1,0,255);HPUT8($1);};

stream_def_list:	|stream_def_list stream_def_node;

page:string{
	#line 5985 "format.w"
	hput_string($1);}page_priority glue_node dimension{
	#line 5985 "format.w"
	HPUT32($5);}
xdimen_node xdimen_node
list stream_def_list;
	/*:289*/	/*297:*/

content_node:START RANGE REFERENCE ON END{
	#line 6097 "format.w"
	REF(page_kind,$3);hput_range($3,true);}
	|START RANGE REFERENCE OFF END{
	#line 6098 "format.w"
	REF(page_kind,$3);hput_range($3,false);};
	/*:297*/	/*308:*/
hint:directory_section definition_section content_section;
	/*:308*/	/*324:*/
directory_section:START DIRECTORY UNSIGNED{
	#line 6788 "format.w"
	new_directory($3+1);new_output_buffers();}entry_list END;
entry_list:	|entry_list entry;
entry:START SECTION UNSIGNED string END
{
	#line 6791 "format.w"
	RNG("Section number",$3,3,max_section_no);hset_entry(&(dir[$3]),$3,0,0,$4);};
	/*:324*/	/*345:*/
definition_section:START DEFINITIONS{
	#line 7326 "format.w"
	hput_definitions_start();}
max_definitions definition_list
END{
	#line 7328 "format.w"
	hput_definitions_end();};
definition_list:	|definition_list def_node;
	/*:345*/	/*353:*/
max_definitions:START MAX max_list END
{
	#line 7444 "format.w"
		/*245:*/
	if(max_ref[label_kind]>=0)
	ALLOCATE(labels,max_ref[label_kind]+1,label_t);
		/*:245*/	/*266:*/
	if(max_outline>=0)
	ALLOCATE(outlines,max_outline+1,outline_t);
		/*:266*/	/*293:*/
	ALLOCATE(page_on,max_ref[page_kind]+1,int);
	ALLOCATE(range_pos,2*(max_ref[range_kind]+1),range_pos_t);
		/*:293*/	/*359:*/
	definition_bits[0][int_kind]= (1<<(MAX_INT_DEFAULT+1))-1;
	definition_bits[0][dimen_kind]= (1<<(MAX_DIMEN_DEFAULT+1))-1;
	definition_bits[0][xdimen_kind]= (1<<(MAX_XDIMEN_DEFAULT+1))-1;
	definition_bits[0][glue_kind]= (1<<(MAX_GLUE_DEFAULT+1))-1;
	definition_bits[0][baseline_kind]= (1<<(MAX_BASELINE_DEFAULT+1))-1;
	definition_bits[0][page_kind]= (1<<(MAX_PAGE_DEFAULT+1))-1;
	definition_bits[0][stream_kind]= (1<<(MAX_STREAM_DEFAULT+1))-1;
	definition_bits[0][range_kind]= (1<<(MAX_RANGE_DEFAULT+1))-1;
		/*:359*/	/*374:*/
	ALLOCATE(hfont_name,max_ref[font_kind]+1,char*);
		/*:374*/hput_max_definitions();};

max_list:	|max_list START max_value END;

max_value:FONT UNSIGNED{
	#line 7448 "format.w"
	hset_max(font_kind,$2);}
	|INTEGER UNSIGNED{
	#line 7449 "format.w"
	hset_max(int_kind,$2);}
	|DIMEN UNSIGNED{
	#line 7450 "format.w"
	hset_max(dimen_kind,$2);}
	|LIGATURE UNSIGNED{
	#line 7451 "format.w"
	hset_max(ligature_kind,$2);}
	|DISC UNSIGNED{
	#line 7452 "format.w"
	hset_max(disc_kind,$2);}
	|GLUE UNSIGNED{
	#line 7453 "format.w"
	hset_max(glue_kind,$2);}
	|LANGUAGE UNSIGNED{
	#line 7454 "format.w"
	hset_max(language_kind,$2);}
	|RULE UNSIGNED{
	#line 7455 "format.w"
	hset_max(rule_kind,$2);}
	|IMAGE UNSIGNED{
	#line 7456 "format.w"
	hset_max(image_kind,$2);}
	|LEADERS UNSIGNED{
	#line 7457 "format.w"
	hset_max(leaders_kind,$2);}
	|BASELINE UNSIGNED{
	#line 7458 "format.w"
	hset_max(baseline_kind,$2);}
	|XDIMEN UNSIGNED{
	#line 7459 "format.w"
	hset_max(xdimen_kind,$2);}
	|PARAM UNSIGNED{
	#line 7460 "format.w"
	hset_max(param_kind,$2);}
	|STREAMDEF UNSIGNED{
	#line 7461 "format.w"
	hset_max(stream_kind,$2);}
	|PAGE UNSIGNED{
	#line 7462 "format.w"
	hset_max(page_kind,$2);}
	|RANGE UNSIGNED{
	#line 7463 "format.w"
	hset_max(range_kind,$2);}
	|LABEL UNSIGNED{
	#line 7464 "format.w"
	hset_max(label_kind,$2);};

	/*:353*/	/*361:*/
def_node:
start FONT ref font END{
	#line 7661 "format.w"
	DEF($$,font_kind,$3);hput_tags($1,$4);}
	|start INTEGER ref integer END{
	#line 7662 "format.w"
	DEF($$,int_kind,$3);hput_tags($1,hput_int($4));}
	|start DIMEN ref dimension END{
	#line 7663 "format.w"
	DEF($$,dimen_kind,$3);hput_tags($1,hput_dimen($4));}
	|start LANGUAGE ref string END{
	#line 7664 "format.w"
	DEF($$,language_kind,$3);hput_string($4);hput_tags($1,TAG(language_kind,0));}
	|start GLUE ref glue END{
	#line 7665 "format.w"
	DEF($$,glue_kind,$3);hput_tags($1,hput_glue(&($4)));}
	|start XDIMEN ref xdimen END{
	#line 7666 "format.w"
	DEF($$,xdimen_kind,$3);hput_tags($1,hput_xdimen(&($4)));}
	|start RULE ref rule END{
	#line 7667 "format.w"
	DEF($$,rule_kind,$3);hput_tags($1,hput_rule(&($4)));}
	|start LEADERS ref leaders END{
	#line 7668 "format.w"
	DEF($$,leaders_kind,$3);hput_tags($1,TAG(leaders_kind,$4));}
	|start BASELINE ref baseline END{
	#line 7669 "format.w"
	DEF($$,baseline_kind,$3);hput_tags($1,TAG(baseline_kind,$4));}
	|start LIGATURE ref ligature END{
	#line 7670 "format.w"
	DEF($$,ligature_kind,$3);hput_tags($1,hput_ligature(&($4)));}
	|start DISC ref disc END{
	#line 7671 "format.w"
	DEF($$,disc_kind,$3);hput_tags($1,hput_disc(&($4)));}
	|start IMAGE ref image END{
	#line 7672 "format.w"
	DEF($$,image_kind,$3);hput_tags($1,hput_image(&($4)));}
	|start PARAM ref parameters END{
	#line 7673 "format.w"
	DEF($$,param_kind,$3);hput_tags($1,hput_list($1+2,&($4)));}
	|start PAGE ref page END{
	#line 7674 "format.w"
	DEF($$,page_kind,$3);hput_tags($1,TAG(page_kind,0));};
	/*:361*/	/*363:*/
def_node:
start INTEGER ref ref END{
	#line 7693 "format.w"
	DEF_REF($$,int_kind,$3,$4);hput_tags($1,TAG(int_kind,0));}
	|start DIMEN ref ref END{
	#line 7694 "format.w"
	DEF_REF($$,dimen_kind,$3,$4);hput_tags($1,TAG(dimen_kind,0));}
	|start GLUE ref ref END{
	#line 7695 "format.w"
	DEF_REF($$,glue_kind,$3,$4);hput_tags($1,TAG(glue_kind,0));};
	/*:363*/	/*368:*/
def_list:position
	|def_list def_node{
	#line 7809 "format.w"
	check_param_def(&($2));};
parameters:estimate def_list{
	#line 7810 "format.w"
	$$.p= $2;$$.k= param_kind;$$.s= (hpos-hstart)-$2;};
	/*:368*/	/*369:*/
empty_param_list:position{
	#line 7831 "format.w"
	HPUTX(2);hpos++;hput_tags($1,TAG(param_kind,1));};
non_empty_param_list:start PARAM{
	#line 7832 "format.w"
	hpos= hpos-2;}parameters END
{
	#line 7833 "format.w"
	hput_tags($1-2,hput_list($1-1,&($4)));};
	/*:369*/	/*377:*/

font:font_head font_param_list;

font_head:string dimension UNSIGNED UNSIGNED
{
	#line 7975 "format.w"
	uint8_t f= $<u>0;SET_DBIT(f,font_kind);hfont_name[f]= strdup($1);$$= hput_font_head(f,hfont_name[f],$2,$3,$4);};

font_param_list:glue_node disc_node	|font_param_list font_param;

font_param:
start PENALTY fref penalty END{
	#line 7980 "format.w"
	hput_tags($1,hput_int($4));}
	|start KERN fref kern END{
	#line 7981 "format.w"
	hput_tags($1,hput_kern(&($4)));}
	|start LIGATURE fref ligature END{
	#line 7982 "format.w"
	hput_tags($1,hput_ligature(&($4)));}
	|start DISC fref disc END{
	#line 7983 "format.w"
	hput_tags($1,hput_disc(&($4)));}
	|start GLUE fref glue END{
	#line 7984 "format.w"
	hput_tags($1,hput_glue(&($4)));}
	|start LANGUAGE fref string END{
	#line 7985 "format.w"
	hput_string($4);hput_tags($1,TAG(language_kind,0));}
	|start RULE fref rule END{
	#line 7986 "format.w"
	hput_tags($1,hput_rule(&($4)));}
	|start IMAGE fref image END{
	#line 7987 "format.w"
	hput_tags($1,hput_image(&($4)));};

fref:ref{
	#line 7989 "format.w"
	RNG("Font parameter",$1,0,MAX_FONT_PARAMS);};
	/*:377*/	/*380:*/
xdimen_ref:ref{
	#line 8062 "format.w"
	REF(xdimen_kind,$1);};
param_ref:ref{
	#line 8063 "format.w"
	REF(param_kind,$1);};
stream_ref:ref{
	#line 8064 "format.w"
	REF_RNG(stream_kind,$1);};


content_node:
start PENALTY ref END{
	#line 8068 "format.w"
	REF(penalty_kind,$3);hput_tags($1,TAG(penalty_kind,0));}
	|start KERN explicit ref END
{
	#line 8070 "format.w"
	REF(dimen_kind,$4);hput_tags($1,TAG(kern_kind,($3)?b100:b000));}
	|start KERN explicit XDIMEN ref END
{
	#line 8072 "format.w"
	REF(xdimen_kind,$5);hput_tags($1,TAG(kern_kind,($3)?b101:b001));}
	|start GLUE ref END{
	#line 8073 "format.w"
	REF(glue_kind,$3);hput_tags($1,TAG(glue_kind,0));}
	|start LIGATURE ref END{
	#line 8074 "format.w"
	REF(ligature_kind,$3);hput_tags($1,TAG(ligature_kind,0));}
	|start DISC ref END{
	#line 8075 "format.w"
	REF(disc_kind,$3);hput_tags($1,TAG(disc_kind,0));}
	|start RULE ref END{
	#line 8076 "format.w"
	REF(rule_kind,$3);hput_tags($1,TAG(rule_kind,0));}
	|start IMAGE ref END{
	#line 8077 "format.w"
	REF(image_kind,$3);hput_tags($1,TAG(image_kind,0));}
	|start LEADERS ref END{
	#line 8078 "format.w"
	REF(leaders_kind,$3);hput_tags($1,TAG(leaders_kind,0));}
	|start BASELINE ref END{
	#line 8079 "format.w"
	REF(baseline_kind,$3);hput_tags($1,TAG(baseline_kind,0));}
	|start LANGUAGE REFERENCE END{
	#line 8080 "format.w"
	REF(language_kind,$3);hput_tags($1,hput_language($3));};

glue_node:start GLUE ref END{
	#line 8082 "format.w"
	REF(glue_kind,$3);
	if($3==zero_skip_no){hpos= hpos-2;$$= false;}
	else{hput_tags($1,TAG(glue_kind,0));$$= true;}};

	/*:380*/	/*405:*/
content_section:START CONTENT{
	#line 8513 "format.w"
	hput_content_start();}content_list END
{
	#line 8514 "format.w"
	hput_content_end();hput_range_defs();hput_label_defs();};
	/*:405*/
%%
	/*:512*/
