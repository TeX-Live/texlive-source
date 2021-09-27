	/*510:*/
%{
	
#include "basetypes.h"
#include <string.h>
#include <math.h>
#include "error.h"
#include "hformat.h"
#include "hput.h"
extern char**hfont_name;

	/*356:*/
uint32_t definition_bits[0x100/32][32]= {
	{0}};

#define SET_DBIT(N,K) ((N)>0xFF?1:(definition_bits[N/32][K]	|= (1<<((N)&(32-1)))))
#define GET_DBIT(N,K) ((N)>0xFF?1:((definition_bits[N/32][K]>>((N)&(32-1)))&1))
#define DEF(D,K,N) (D).k= K; (D).n= (N);SET_DBIT((D).n,(D).k);\
 DBG(DBGDEF,"Defining %s %d\n",definition_name[(D).k],(D).n);\
 RNG("Definition",(D).n,max_fixed[(D).k]+1,max_ref[(D).k]);
#define REF(K,N) REF_RNG(K,N);if(!GET_DBIT(N,K)) \
 QUIT("Reference %d to %s before definition",(N),definition_name[K])
	/*:356*/	/*360:*/
#define DEF_REF(D,K,M,N)  DEF(D,K,M);\
if ((M)>max_default[K]) QUIT("Defining non default reference %d for %s",M,definition_name[K]); \
if ((N)>max_fixed[K]) QUIT("Defining reference %d for %s by non fixed reference %d",M,definition_name[K],N);
	/*:360*/

extern void hset_entry(entry_t*e,uint16_t i,uint32_t size,
uint32_t xsize,char*file_name);

	/*423:*/
#ifdef DEBUG
#define  YYDEBUG 1
extern int yydebug;
#else
#define YYDEBUG 0
#endif
	/*:423*/
extern int yylex(void);

	/*352:*/
void hset_max(kind_t k,int n)
{
	DBG(DBGDEF,"Setting max %s to %d\n",definition_name[k],n);
	RNG("Maximum",n,max_fixed[k]+1,MAX_REF(k));
	if(n>max_ref[k])
	max_ref[k]= n;
	}
	/*:352*/	/*363:*/
void check_param_def(ref_t*df)
{
	if(df->k!=int_kind&&df->k!=dimen_kind&&df->k!=glue_kind)
	QUIT("Kind %s not allowed in parameter list",definition_name[df->k]);
	if(df->n<=max_fixed[df->k]||max_default[df->k]<df->n)
	QUIT("Parameter %d for %s not allowed in parameter list",df->n,definition_name[df->k]);
	}
	/*:363*/	/*422:*/
extern int yylineno;
int yyerror(const char*msg)
{
	QUIT(" in line %d %s",yylineno,msg);
	return 0;
	}
	/*:422*/


%}





%union {
	uint32_t u;  int32_t i;  char *s;  float64_t f;  glyph_t c;
	dimen_t d;stretch_t st;xdimen_t xd;kern_t kt;
	rule_t r;glue_t g;image_t x;
	list_t l;box_t h;disc_t dc;lig_t lg;
	ref_t rf;info_t info;order_t o;bool b;
	}



%start hint

	/*2:*/
%token START    
%token END      
%token GLYPH     
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
%token DIMEN 
%token PT 
%token MM 
%token INCH 
%type <d> dimension
	/*:79*/	/*87:*/
%token XDIMEN 
%token H 
%token V 
%type <xd> xdimen
	/*:87*/	/*98:*/
%token FIL 
%token FILL 
%token FILLL 
%type <st> stretch
%type <o> order
	/*:98*/	/*102:*/
%token PENALTY 
%token INTEGER     
%type <i> penalty
	/*:102*/	/*108:*/
%token LANGUAGE 
	/*:108*/	/*114:*/
%token RULE 
%token RUNNING 
%type <d> rule_dimension
%type <r> rule
	/*:114*/	/*123:*/
%token KERN 
%token EXPLICIT 
%type <b> explicit
%type <kt> kern
	/*:123*/	/*132:*/
%token GLUE 
%token PLUS  
%token MINUS   
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
%token HBOX     
%token VBOX     
%token SHIFTED  
%type <info> box box_dimen box_shift box_glue_set

	/*:161*/	/*169:*/
%token HPACK 
%token HSET  
%token VPACK 
%token VSET  
%token DEPTH 
%token ADD 
%token TO 
%type <info> xbox box_goal hpack vpack
	/*:169*/	/*174:*/
%token LEADERS 
%token ALIGN 
%token CENTER 
%token EXPAND 
%type <info> leaders
%type <info> ltype
	/*:174*/	/*181:*/
%token BASELINE 
%type <info> baseline
	/*:181*/	/*188:*/
%token LIGATURE     
%type <u>  lig_cc
%type <lg> ligature
%type <u> ref
	/*:188*/	/*196:*/
%token DISC     
%type <dc> disc
%type <u> replace_count
	/*:196*/	/*204:*/
%token PAR 
%type <info> par
	/*:204*/	/*209:*/
%token MATH 
%type <info> math
	/*:209*/	/*214:*/
%token ON 
%token OFF 
%type <i> on_off
	/*:214*/	/*218:*/
%token ADJUST 
	/*:218*/	/*222:*/
%token TABLE 
%token ITEM 
%type <info> table span_count
	/*:222*/	/*229:*/
%token IMAGE 
%type <x> image image_dimen
	/*:229*/	/*246:*/
%token LABEL 
%token BOT 
%token MID 
%type <i> placement
	/*:246*/	/*260:*/
%token LINK 
	/*:260*/	/*270:*/
%token OUTLINE 
	/*:270*/	/*277:*/
%token STREAM 
%token STREAMDEF 
%token FIRST 
%token LAST 
%token TOP 
%token NOREFERENCE 
%type <info> stream_type
%type <u> stream_ref
%type <rf> stream_def_node
	/*:277*/	/*283:*/
%type <info> stream
	/*:283*/	/*287:*/
%token PAGE 
	/*:287*/	/*295:*/
%token RANGE 
	/*:295*/	/*322:*/
%token DIRECTORY 
%token SECTION 
	/*:322*/	/*341:*/
%token DEFINITIONS 
	/*:341*/	/*349:*/
%token MAX 
	/*:349*/	/*358:*/

%type <rf> def_node
	/*:358*/	/*364:*/
%token PARAM 
%type <u> def_list
%type <l> parameters
	/*:364*/	/*373:*/
%token FONT     
%type <info> font font_head
	/*:373*/	/*401:*/
%token CONTENT 
	/*:401*/
%%
	/*5:*/
glyph:UNSIGNED REFERENCE{
	$$.c= $1;REF(font_kind,$2);$$.f= $2;};
content_node:start GLYPH glyph END{
	hput_tags($1,hput_glyph(&($3)));};
start:START{
	HPUTNODE;$$= (uint32_t)(hpos++-hstart);}
	/*:5*/	/*29:*/
integer:SIGNED	|UNSIGNED{
	RNG("number",$1,0,INT32_MAX);};
	/*:29*/	/*38:*/
glyph:CHARCODE REFERENCE{
	$$.c= $1;REF(font_kind,$2);$$.f= $2;};
	/*:38*/	/*50:*/
string:STRING	|CHARCODE{
	static char s[2];
	RNG("String element",$1,0x20,0x7E);
	s[0]= $1;s[1]= 0;$$= s;};
	/*:50*/	/*58:*/
number:UNSIGNED{
	$$= (float64_t)$1;}	|SIGNED{
	$$= (float64_t)$1;}	|FPNUM;
	/*:58*/	/*82:*/
dimension:number PT{
	$$= ROUND($1*ONE);RNG("Dimension",$$,-MAX_DIMEN,MAX_DIMEN);}
	|number INCH{
	$$= ROUND($1*ONE*72.27);RNG("Dimension",$$,-MAX_DIMEN,MAX_DIMEN);}
	|number MM{
	$$= ROUND($1*ONE*(72.27/25.4));RNG("Dimension",$$,-MAX_DIMEN,MAX_DIMEN);};
	/*:82*/	/*89:*/
xdimen:dimension number H number V{
	$$.w= $1;$$.h= $2;$$.v= $4;}
	|dimension number H{
	$$.w= $1;$$.h= $2;$$.v= 0.0;}
	|dimension number V{
	$$.w= $1;$$.h= 0.0;$$.v= $2;}
	|dimension{
	$$.w= $1;$$.h= 0.0;$$.v= 0.0;};



xdimen_node:start XDIMEN xdimen END{
	hput_tags($1,hput_xdimen(&($3)));};
	/*:89*/	/*100:*/

order:PT{
	$$= normal_o;}	|FIL{
	$$= fil_o;}	|FILL{
	$$= fill_o;}	|FILLL{
	$$= filll_o;};

stretch:number order{
	$$.f= $1;$$.o= $2;};
	/*:100*/	/*104:*/
penalty:integer{
	RNG("Penalty",$1,-20000,+20000);$$= $1;};
content_node:start PENALTY penalty END{
	hput_tags($1,hput_int($3));};
	/*:104*/	/*116:*/
rule_dimension:dimension	|RUNNING{
	$$= RUNNING_DIMEN;};
rule:rule_dimension rule_dimension rule_dimension
{
	$$.h= $1;$$.d= $2;$$.w= $3;
	if($3==RUNNING_DIMEN&&($1==RUNNING_DIMEN||$2==RUNNING_DIMEN))
	QUIT("Incompatible running dimensions 0x%x 0x%x 0x%x",$1,$2,$3);};
rule_node:start RULE rule END{
	hput_tags($1,hput_rule(&($3)));};
content_node:rule_node;
	/*:116*/	/*125:*/
explicit:{
	$$= false;}	|EXPLICIT{
	$$= true;};
kern:explicit xdimen{
	$$.x= $1;$$.d= $2;};
content_node:start KERN kern END{
	hput_tags($1,hput_kern(&($3)));}
	/*:125*/	/*134:*/
plus:{
	$$.f= 0.0;$$.o= 0;}	|PLUS stretch{
	$$= $2;};
minus:{
	$$.f= 0.0;$$.o= 0;}	|MINUS stretch{
	$$= $2;};
glue:xdimen plus minus{
	$$.w= $1;$$.p= $2;$$.m= $3;};
content_node:start GLUE glue END{
	if(ZERO_GLUE($3)){HPUT8(zero_skip_no);
	hput_tags($1,TAG(glue_kind,0));}else hput_tags($1,hput_glue(&($3)));};
glue_node:start GLUE glue END
{
	if(ZERO_GLUE($3)){hpos--;$$= false;}
	else{hput_tags($1,hput_glue(&($3)));$$= true;}};
	/*:134*/	/*142:*/
position:{
	$$= hpos-hstart;};
content_list:position
	|content_list content_node;
estimate:{
	hpos+= 2;}
	|UNSIGNED{
	hpos+= hsize_bytes($1)+1;};
list:start estimate content_list END
{
	$$.k= list_kind;$$.p= $3;$$.s= (hpos-hstart)-$3;
	hput_tags($1,hput_list($1+1,&($$)));};
	/*:142*/	/*153:*/
list:TXT_START position
{
	hpos+= 4;}
text TXT_END
{
	$$.k= text_kind;$$.p= $4;$$.s= (hpos-hstart)-$4;
	hput_tags($2,hput_list($2+1,&($$)));};
text:position	|text txt;

txt:TXT_CC{
	hput_txt_cc($1);}
	|TXT_FONT{
	REF(font_kind,$1);hput_txt_font($1);}
	|TXT_GLOBAL{
	REF($1.k,$1.n);hput_txt_global(&($1));}
	|TXT_LOCAL{
	RNG("Font parameter",$1,0,11);hput_txt_local($1);}
	|TXT_FONT_GLUE{
	HPUTX(1);HPUT8(txt_glue);}
	|TXT_FONT_HYPHEN{
	HPUTX(1);HPUT8(txt_hyphen);}
	|TXT_IGNORE{
	HPUTX(1);HPUT8(txt_ignore);}
	|{
	HPUTX(1);HPUT8(txt_node);}content_node;
	/*:153*/	/*163:*/

box_dimen:dimension dimension dimension
{
	$$= hput_box_dimen($1,$2,$3);};
box_shift:{
	$$= b000;}
	|SHIFTED dimension{
	$$= hput_box_shift($2);};

box_glue_set:{
	$$= b000;}
	|PLUS stretch{
	$$= hput_box_glue_set(+1,$2.f,$2.o);}
	|MINUS stretch{
	$$= hput_box_glue_set(-1,$2.f,$2.o);};


box:box_dimen box_shift box_glue_set list{
	$$= $1	|$2	|$3;};

hbox_node:start HBOX box END{
	hput_tags($1,TAG(hbox_kind,$3));};
vbox_node:start VBOX box END{
	hput_tags($1,TAG(vbox_kind,$3));};
content_node:hbox_node	|vbox_node;
	/*:163*/	/*171:*/
box_flex:plus minus{
	hput_stretch(&($1));hput_stretch(&($2));};
xbox:box_dimen box_shift box_flex xdimen_ref list{
	$$= $1	|$2;}
	|box_dimen box_shift box_flex xdimen_node list{
	$$= $1	|$2	|b100;};

box_goal:TO xdimen_ref{
	$$= b000;}
	|ADD xdimen_ref{
	$$= b001;}
	|TO xdimen_node{
	$$= b100;}
	|ADD xdimen_node{
	$$= b101;};

hpack:box_shift box_goal list{
	$$= $2;};
vpack:box_shift MAX DEPTH dimension{
	HPUT32($4);}box_goal list{
	$$= $1	|$6;};

vxbox_node:start VSET xbox END{
	hput_tags($1,TAG(vset_kind,$3));}
	|start VPACK vpack END{
	hput_tags($1,TAG(vpack_kind,$3));};


hxbox_node:start HSET xbox END{
	hput_tags($1,TAG(hset_kind,$3));}
	|start HPACK hpack END{
	hput_tags($1,TAG(hpack_kind,$3));};

content_node:vxbox_node	|hxbox_node;
	/*:171*/	/*176:*/
ltype:{
	$$= 1;}	|ALIGN{
	$$= 1;}	|CENTER{
	$$= 2;}	|EXPAND{
	$$= 3;};
leaders:glue_node ltype rule_node{
	if($1)$$= $2	|b100;else $$= $2;}
	|glue_node ltype hbox_node{
	if($1)$$= $2	|b100;else $$= $2;}
	|glue_node ltype vbox_node{
	if($1)$$= $2	|b100;else $$= $2;};
content_node:start LEADERS leaders END{
	hput_tags($1,TAG(leaders_kind,$3));}
	/*:176*/	/*183:*/
baseline:dimension{
	if($1!=0)HPUT32($1);}
glue_node glue_node{
	$$= b000;if($1!=0)$$	|= b001;
	if($3)$$	|= b100;
	if($4)$$	|= b010;
	};
content_node:start BASELINE baseline END
{
	if($3==b000)HPUT8(0);hput_tags($1,TAG(baseline_kind,$3));};
	/*:183*/	/*190:*/
cc_list:	|cc_list TXT_CC{
	hput_utf8($2);};
lig_cc:UNSIGNED{
	RNG("UTF-8 code",$1,0,0x1FFFFF);$$= hpos-hstart;hput_utf8($1);};
lig_cc:CHARCODE{
	$$= hpos-hstart;hput_utf8($1);};
ref:REFERENCE{
	HPUT8($1);$$= $1;};
ligature:ref{
	REF(font_kind,$1);}lig_cc TXT_START cc_list TXT_END
{
	$$.f= $1;$$.l.p= $3;$$.l.s= (hpos-hstart)-$3;
	RNG("Ligature size",$$.l.s,0,255);};
content_node:start LIGATURE ligature END{
	hput_tags($1,hput_ligature(&($3)));};
	/*:190*/	/*198:*/
replace_count:explicit{
	if($1){$$= 0x80;HPUT8(0x80);}else $$= 0x00;}
	|explicit UNSIGNED{
	RNG("Replace count",$2,0,31);
	$$= ($2)	|(($1)?0x80:0x00);if($$!=0)HPUT8($$);};
disc:replace_count list list{
	$$.r= $1;$$.p= $2;$$.q= $3;
	if($3.s==0){hpos= hpos-2;if($2.s==0)hpos= hpos-2;}}
	|replace_count list{
	$$.r= $1;$$.p= $2;if($2.s==0)hpos= hpos-2;$$.q.s= 0;}
	|replace_count{
	$$.r= $1;$$.p.s= 0;$$.q.s= 0;};


disc_node:start DISC disc END
{
	hput_tags($1,hput_disc(&($3)));};

content_node:disc_node;
	/*:198*/	/*206:*/
par_dimen:xdimen{
	hput_xdimen_node(&($1));};
par:xdimen_ref param_ref list{
	$$= b000;}
	|xdimen_ref empty_param_list non_empty_param_list list{
	$$= b010;}
	|xdimen_ref empty_param_list list{
	$$= b010;}
	|xdimen param_ref{
	hput_xdimen_node(&($1));}list{
	$$= b100;}
	|par_dimen empty_param_list non_empty_param_list list{
	$$= b110;}
	|par_dimen empty_param_list list{
	$$= b110;};

content_node:start PAR par END{
	hput_tags($1,TAG(par_kind,$3));};
	/*:206*/	/*211:*/
math:param_ref list{
	$$= b000;}
	|param_ref list hbox_node{
	$$= b001;}
	|param_ref hbox_node list{
	$$= b010;}
	|empty_param_list list{
	$$= b100;}
	|empty_param_list list hbox_node{
	$$= b101;}
	|empty_param_list hbox_node list{
	$$= b110;}
	|empty_param_list non_empty_param_list list{
	$$= b100;}
	|empty_param_list non_empty_param_list list hbox_node{
	$$= b101;}
	|empty_param_list non_empty_param_list hbox_node list{
	$$= b110;};

content_node:start MATH math END{
	hput_tags($1,TAG(math_kind,$3));};
	/*:211*/	/*216:*/
on_off:ON{
	$$= 1;}	|OFF{
	$$= 0;};
math:on_off{
	$$= b011	|($1<<2);};
	/*:216*/	/*220:*/
content_node:start ADJUST list END{
	hput_tags($1,TAG(adjust_kind,1));};
	/*:220*/	/*224:*/
span_count:UNSIGNED{
	$$= hput_span_count($1);};
content_node:start ITEM content_node END{
	hput_tags($1,TAG(item_kind,1));};
content_node:start ITEM span_count content_node END{
	hput_tags($1,TAG(item_kind,$3));};
content_node:start ITEM list END{
	hput_tags($1,TAG(item_kind,b000));};

table:H box_goal list list{
	$$= $2;};
table:V box_goal list list{
	$$= $2	|b010;};

content_node:start TABLE table END{
	hput_tags($1,TAG(table_kind,$3));};
	/*:224*/	/*231:*/
image_dimen:dimension dimension{
	$$.w= $1;$$.h= $2;}	|{
	$$.w= $$.h= 0;};
image:UNSIGNED image_dimen plus minus{
	$$.w= $2.w;$$.h= $2.h;$$.p= $3;$$.m= $4;RNG("Section number",$1,3,max_section_no);$$.n= $1;};
content_node:start IMAGE image END{
	hput_tags($1,hput_image(&($3)));}
	/*:231*/	/*241:*/
max_value:OUTLINE UNSIGNED{
	max_outline= $2;
	RNG("max outline",max_outline,0,0xFFFF);
	DBG(DBGDEF	|DBGLABEL,"Setting max outline to %d\n",max_outline);
	};
	/*:241*/	/*248:*/
placement:TOP{
	$$= LABEL_TOP;}	|BOT{
	$$= LABEL_BOT;}	|MID{
	$$= LABEL_MID;}	|{
	$$= LABEL_MID;};
content_node:START LABEL REFERENCE placement END
{
	hset_label($3,$4);}
	/*:248*/	/*262:*/
content_node:start LINK REFERENCE on_off END
{
	hput_tags($1,hput_link($3,$4));};
	/*:262*/	/*272:*/
def_node:START OUTLINE REFERENCE integer position list END{
	
	static int outline_no= -1;
	$$.k= outline_kind;$$.n= $3;
	if($6.s==0)QUIT("Outline with empty title in line %d",yylineno);
	outline_no++;
	hset_outline(outline_no,$3,$4,$5);
	};
	/*:272*/	/*279:*/
stream_link:ref{
	REF_RNG(stream_kind,$1);}	|NOREFERENCE{
	HPUT8(255);};
stream_split:stream_link stream_link UNSIGNED{
	RNG("split ratio",$3,0,1000);HPUT16($3);};
stream_info:xdimen_node UNSIGNED{
	RNG("magnification factor",$2,0,1000);HPUT16($2);}stream_split;

stream_type:stream_info{
	$$= 0;}	|FIRST{
	$$= 1;}	|LAST{
	$$= 2;}	|TOP{
	$$= 3;};

stream_def_node:start STREAMDEF ref stream_type
list xdimen_node glue_node list glue_node END
{
	DEF($$,stream_kind,$3);hput_tags($1,TAG(stream_kind,$4	|b100));};

stream_ins_node:start STREAMDEF ref END
{
	RNG("Stream insertion",$3,0,max_ref[stream_kind]);hput_tags($1,TAG(stream_kind,b100));};

content_node:stream_def_node	|stream_ins_node;
	/*:279*/	/*284:*/
stream:empty_param_list list{
	$$= b010;}
	|empty_param_list non_empty_param_list list{
	$$= b010;}
	|param_ref list{
	$$= b000;};
content_node:start STREAM stream_ref stream END
{
	hput_tags($1,TAG(stream_kind,$4));};
	/*:284*/	/*289:*/
page_priority:{
	HPUT8(1);}
	|UNSIGNED{
	RNG("page priority",$1,0,255);HPUT8($1);};

stream_def_list:	|stream_def_list stream_def_node;

page:string{
	hput_string($1);}page_priority glue_node dimension{
	HPUT32($5);}
xdimen_node xdimen_node
list stream_def_list;
	/*:289*/	/*297:*/

content_node:START RANGE REFERENCE ON END{
	REF(page_kind,$3);hput_range($3,true);}
	|START RANGE REFERENCE OFF END{
	REF(page_kind,$3);hput_range($3,false);};
	/*:297*/	/*308:*/
hint:directory_section definition_section content_section;
	/*:308*/	/*324:*/
directory_section:START DIRECTORY UNSIGNED{
	new_directory($3+1);new_output_buffers();}entry_list END;
entry_list:	|entry_list entry;
entry:START SECTION UNSIGNED string END
{
	RNG("Section number",$3,3,max_section_no);hset_entry(&(dir[$3]),$3,0,0,$4);};
	/*:324*/	/*343:*/
definition_section:START DEFINITIONS{
	hput_definitions_start();}
max_definitions definition_list
END{
	hput_definitions_end();};
definition_list:	|definition_list def_node;
	/*:343*/	/*351:*/
max_definitions:START MAX max_list END
{
		/*245:*/
	if(max_ref[label_kind]>=0)
	ALLOCATE(labels,max_ref[label_kind]+1,label_t);
		/*:245*/	/*266:*/
	if(max_outline>=0)
	ALLOCATE(outlines,max_outline+1,outline_t);
		/*:266*/	/*293:*/
	ALLOCATE(page_on,max_ref[page_kind]+1,int);
	ALLOCATE(range_pos,2*(max_ref[range_kind]+1),range_pos_t);
		/*:293*/	/*357:*/
	definition_bits[0][int_kind]= (1<<(MAX_INT_DEFAULT+1))-1;
	definition_bits[0][dimen_kind]= (1<<(MAX_DIMEN_DEFAULT+1))-1;
	definition_bits[0][xdimen_kind]= (1<<(MAX_XDIMEN_DEFAULT+1))-1;
	definition_bits[0][glue_kind]= (1<<(MAX_GLUE_DEFAULT+1))-1;
	definition_bits[0][baseline_kind]= (1<<(MAX_BASELINE_DEFAULT+1))-1;
	definition_bits[0][page_kind]= (1<<(MAX_PAGE_DEFAULT+1))-1;
	definition_bits[0][stream_kind]= (1<<(MAX_STREAM_DEFAULT+1))-1;
	definition_bits[0][range_kind]= (1<<(MAX_RANGE_DEFAULT+1))-1;
		/*:357*/	/*372:*/
	ALLOCATE(hfont_name,max_ref[font_kind]+1,char*);
		/*:372*/hput_max_definitions();};

max_list:	|max_list START max_value END;

max_value:FONT UNSIGNED{
	hset_max(font_kind,$2);}
	|INTEGER UNSIGNED{
	hset_max(int_kind,$2);}
	|DIMEN UNSIGNED{
	hset_max(dimen_kind,$2);}
	|LIGATURE UNSIGNED{
	hset_max(ligature_kind,$2);}
	|DISC UNSIGNED{
	hset_max(disc_kind,$2);}
	|GLUE UNSIGNED{
	hset_max(glue_kind,$2);}
	|LANGUAGE UNSIGNED{
	hset_max(language_kind,$2);}
	|RULE UNSIGNED{
	hset_max(rule_kind,$2);}
	|IMAGE UNSIGNED{
	hset_max(image_kind,$2);}
	|LEADERS UNSIGNED{
	hset_max(leaders_kind,$2);}
	|BASELINE UNSIGNED{
	hset_max(baseline_kind,$2);}
	|XDIMEN UNSIGNED{
	hset_max(xdimen_kind,$2);}
	|PARAM UNSIGNED{
	hset_max(param_kind,$2);}
	|STREAMDEF UNSIGNED{
	hset_max(stream_kind,$2);}
	|PAGE UNSIGNED{
	hset_max(page_kind,$2);}
	|RANGE UNSIGNED{
	hset_max(range_kind,$2);}
	|LABEL UNSIGNED{
	hset_max(label_kind,$2);};

	/*:351*/	/*359:*/
def_node:
start FONT ref font END{
	DEF($$,font_kind,$3);hput_tags($1,$4);}
	|start INTEGER ref integer END{
	DEF($$,int_kind,$3);hput_tags($1,hput_int($4));}
	|start DIMEN ref dimension END{
	DEF($$,dimen_kind,$3);hput_tags($1,hput_dimen($4));}
	|start LANGUAGE ref string END{
	DEF($$,language_kind,$3);hput_string($4);hput_tags($1,TAG(language_kind,0));}
	|start GLUE ref glue END{
	DEF($$,glue_kind,$3);hput_tags($1,hput_glue(&($4)));}
	|start XDIMEN ref xdimen END{
	DEF($$,xdimen_kind,$3);hput_tags($1,hput_xdimen(&($4)));}
	|start RULE ref rule END{
	DEF($$,rule_kind,$3);hput_tags($1,hput_rule(&($4)));}
	|start LEADERS ref leaders END{
	DEF($$,leaders_kind,$3);hput_tags($1,TAG(leaders_kind,$4));}
	|start BASELINE ref baseline END{
	DEF($$,baseline_kind,$3);hput_tags($1,TAG(baseline_kind,$4));}
	|start LIGATURE ref ligature END{
	DEF($$,ligature_kind,$3);hput_tags($1,hput_ligature(&($4)));}
	|start DISC ref disc END{
	DEF($$,disc_kind,$3);hput_tags($1,hput_disc(&($4)));}
	|start IMAGE ref image END{
	DEF($$,image_kind,$3);hput_tags($1,hput_image(&($4)));}
	|start PARAM ref parameters END{
	DEF($$,param_kind,$3);hput_tags($1,hput_list($1+2,&($4)));}
	|start PAGE ref page END{
	DEF($$,page_kind,$3);hput_tags($1,TAG(page_kind,0));};
	/*:359*/	/*361:*/
def_node:
start INTEGER ref ref END{
	DEF_REF($$,int_kind,$3,$4);hput_tags($1,TAG(int_kind,0));}
	|start DIMEN ref ref END{
	DEF_REF($$,dimen_kind,$3,$4);hput_tags($1,TAG(dimen_kind,0));}
	|start GLUE ref ref END{
	DEF_REF($$,glue_kind,$3,$4);hput_tags($1,TAG(glue_kind,0));};
	/*:361*/	/*366:*/
def_list:position
	|def_list def_node{
	check_param_def(&($2));};
parameters:estimate def_list{
	$$.p= $2;$$.k= param_kind;$$.s= (hpos-hstart)-$2;};
	/*:366*/	/*367:*/
empty_param_list:position{
	HPUTX(2);hpos++;hput_tags($1,TAG(param_kind,1));};
non_empty_param_list:start PARAM{
	hpos= hpos-2;}parameters END
{
	hput_tags($1-2,hput_list($1-1,&($4)));};
	/*:367*/	/*375:*/

font:font_head font_param_list;

font_head:string dimension UNSIGNED UNSIGNED
{
	uint8_t f= $<u>0;SET_DBIT(f,font_kind);hfont_name[f]= strdup($1);$$= hput_font_head(f,hfont_name[f],$2,$3,$4);};

font_param_list:glue_node disc_node	|font_param_list font_param;

font_param:
start PENALTY fref penalty END{
	hput_tags($1,hput_int($4));}
	|start KERN fref kern END{
	hput_tags($1,hput_kern(&($4)));}
	|start LIGATURE fref ligature END{
	hput_tags($1,hput_ligature(&($4)));}
	|start DISC fref disc END{
	hput_tags($1,hput_disc(&($4)));}
	|start GLUE fref glue END{
	hput_tags($1,hput_glue(&($4)));}
	|start LANGUAGE fref string END{
	hput_string($4);hput_tags($1,TAG(language_kind,0));}
	|start RULE fref rule END{
	hput_tags($1,hput_rule(&($4)));}
	|start IMAGE fref image END{
	hput_tags($1,hput_image(&($4)));};

fref:ref{
	RNG("Font parameter",$1,0,MAX_FONT_PARAMS);};
	/*:375*/	/*378:*/
xdimen_ref:ref{
	REF(xdimen_kind,$1);};
param_ref:ref{
	REF(param_kind,$1);};
stream_ref:ref{
	REF_RNG(stream_kind,$1);};


content_node:
start PENALTY ref END{
	REF(penalty_kind,$3);hput_tags($1,TAG(penalty_kind,0));}
	|start KERN explicit ref END
{
	REF(dimen_kind,$4);hput_tags($1,TAG(kern_kind,($3)?b100:b000));}
	|start KERN explicit XDIMEN ref END
{
	REF(xdimen_kind,$5);hput_tags($1,TAG(kern_kind,($3)?b101:b001));}
	|start GLUE ref END{
	REF(glue_kind,$3);hput_tags($1,TAG(glue_kind,0));}
	|start LIGATURE ref END{
	REF(ligature_kind,$3);hput_tags($1,TAG(ligature_kind,0));}
	|start DISC ref END{
	REF(disc_kind,$3);hput_tags($1,TAG(disc_kind,0));}
	|start RULE ref END{
	REF(rule_kind,$3);hput_tags($1,TAG(rule_kind,0));}
	|start IMAGE ref END{
	REF(image_kind,$3);hput_tags($1,TAG(image_kind,0));}
	|start LEADERS ref END{
	REF(leaders_kind,$3);hput_tags($1,TAG(leaders_kind,0));}
	|start BASELINE ref END{
	REF(baseline_kind,$3);hput_tags($1,TAG(baseline_kind,0));}
	|start LANGUAGE REFERENCE END{
	REF(language_kind,$3);hput_tags($1,hput_language($3));};

glue_node:start GLUE ref END{
	REF(glue_kind,$3);
	if($3==zero_skip_no){hpos= hpos-2;$$= false;}
	else{hput_tags($1,TAG(glue_kind,0));$$= true;}};

	/*:378*/	/*403:*/
content_section:START CONTENT{
	hput_content_start();}content_list END
{
	hput_content_end();hput_range_defs();hput_label_defs();};
	/*:403*/
%%
	/*:510*/
