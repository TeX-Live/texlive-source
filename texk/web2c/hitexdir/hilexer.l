	/*509:*/
%{
	#line 10175 "format.w"
	
#include "hibasetypes.h"
#include "hierror.h"
#include "hiformat.h"
#include "hiput.h"

	/*423:*/
#ifdef DEBUG
#define  YYDEBUG 1
extern int yydebug;
#else
#define YYDEBUG 0
#endif
	/*:423*/
#include "hiparser.h"

	/*22:*/
#define SCAN_UDEC(S) yylval.u= strtoul(S,NULL,10)
	/*:22*/	/*25:*/
#define SCAN_HEX(S) yylval.u= strtoul(S,NULL,16)
	/*:25*/	/*28:*/
#define SCAN_DEC(S) yylval.i= strtol(S,NULL,10)
	/*:28*/	/*31:*/
#define MAX_STR    (1<<10) 
static char str_buffer[MAX_STR];
static int str_length;
#define STR_START      (str_length= 0)
#define STR_PUT(C) (str_buffer[str_length++]= (C))
#define STR_ADD(C) STR_PUT(C);RNG("String length",str_length,0,MAX_STR-1)
#define STR_END        str_buffer[str_length]= 0
#define SCAN_STR       yylval.s= str_buffer
	/*:31*/	/*40:*/
#define SCAN_UTF8_1(S)   yylval.u= ((S)[0]&0x7F)
	/*:40*/	/*42:*/
#define SCAN_UTF8_2(S)   yylval.u= (((S)[0]&0x1F)<<6)+((S)[1]&0x3F)
	/*:42*/	/*44:*/
#define SCAN_UTF8_3(S)   yylval.u= (((S)[0]&0x0F)<<12)+(((S)[1]&0x3F)<<6)+((S)[2]&0x3F)
	/*:44*/	/*46:*/
#define SCAN_UTF8_4(S)   yylval.u= (((S)[0]&0x03)<<18)+(((S)[1]&0x3F)<<12)+(((S)[2]&0x3F)<<6)+((S)[3]&0x3F)
	/*:46*/	/*57:*/
#define SCAN_DECFLOAT       yylval.f= atof(yytext)
	/*:57*/	/*60:*/
#define SCAN_HEXFLOAT       yylval.f= xtof(yytext)
	/*:60*/	/*152:*/
#define SCAN_REF(K) yylval.rf.k= K; yylval.rf.n= atoi(yytext+2)
static int scan_level= 0;
#define SCAN_START          yy_push_state(INITIAL);if (1==scan_level++) hpos0= hpos;
#define SCAN_END            if (scan_level--) yy_pop_state(); else QUIT("Too many '>' in line %d",yylineno)
#define SCAN_TXT_START      BEGIN(TXT)
#define SCAN_TXT_END        BEGIN(INITIAL)
	/*:152*/
	/*61:*/

float64_t xtof(char*x)
{
	#line 1369 "format.w"
	int sign,digits,exp;
	uint64_t mantissa= 0;
	DBG(DBGFLOAT,"converting %s:\n",x);
		/*62:*/
	if(*x=='-'){sign= -1;x++;}
	else if(*x=='+'){sign= +1;x++;}
	else sign= +1;
	DBG(DBGFLOAT,"\tsign=%d\n",sign);
		/*:62*/
	x= x+2;
		/*63:*/
	digits= 0;
	while(*x=='0')x++;
	while(*x!='.')
	{mantissa= mantissa<<4;
	if(*x<'A')mantissa= mantissa+*x-'0';
	else mantissa= mantissa+*x-'A'+10;
	x++;
	digits++;
	}
	x++;
	exp= 0;
	while(*x!=0&&*x!='x')
	{mantissa= mantissa<<4;
	exp= exp-4;
	if(*x<'A')mantissa= mantissa+*x-'0';
	else mantissa= mantissa+*x-'A'+10;
	x++;
	digits++;
	}
	DBG(DBGFLOAT,"\tdigits=%d mantissa=0x%"PRIx64", exp=%d\n",digits,mantissa,exp);
		/*:63*/
		/*64:*/
	if(mantissa==0)return 0.0;
	{int s;
	s= digits-DBL_M_BITS/4;
	if(s>1)
	mantissa= mantissa>>(4*(s-1));
	else if(s<1)
	mantissa= mantissa<<(4*(1-s));
	exp= exp+4*(digits-1);
	DBG(DBGFLOAT,"\tdigits=%d mantissa=0x%"PRIx64", exp=%d\n",digits,mantissa,exp);
	while((mantissa>>DBL_M_BITS)>1){mantissa= mantissa>>1;exp++;}
	DBG(DBGFLOAT,"\tdigits=%d mantissa=0x%"PRIx64", exp=%d\n",digits,mantissa,exp);
	mantissa= mantissa&~((uint64_t)1<<DBL_M_BITS);
	DBG(DBGFLOAT,"\tdigits=%d mantissa=0x%"PRIx64", exp=%d\n",digits,mantissa,exp);
	}
		/*:64*/
		/*65:*/
	if(*x=='x')
	{int s;
	x++;
	if(*x=='-'){s= -1;x++;}
	else if(*x=='+'){s= +1;x++;}
	else s= +1;
	DBG(DBGFLOAT,"\texpsign=%d\n",s);
	DBG(DBGFLOAT,"\texp=%d\n",exp);
	while(*x!=0)
	{if(*x<'A')exp= exp+4*s*(*x-'0');
	else exp= exp+4*s*(*x-'A'+10);
	x++;
	DBG(DBGFLOAT,"\texp=%d\n",exp);
	}
	}
	RNG("Floating point exponent",exp,-DBL_EXCESS,DBL_EXCESS);
		/*:65*/
		/*66:*/
	{union{float64_t d;uint64_t bits;}u;
	if(sign<0)sign= 1;else sign= 0;
	exp= exp+DBL_EXCESS;
	u.bits= ((uint64_t)sign<<63)
		|((uint64_t)exp<<DBL_M_BITS)	|mantissa;
	DBG(DBGFLOAT," return %f\n",u.d);
	return u.d;
	}
		/*:66*/
	}
	/*:61*/
int yywrap(void){
	#line 10186 "format.w"
	return 1;}
#ifdef _MSC_VER
#pragma  warning( disable : 4267)
#endif

%}

%option yylineno batch stack
%option debug
%option  nounistd nounput noinput noyy_top_state

	/*23:*/
HEX	[0-9A-F]
	/*:23*/	/*32:*/
%x STR
	/*:32*/	/*39:*/
UTF8_1	[\x00-\x7F]
	/*:39*/	/*41:*/
UTF8_2	[\xC0-\xDF][\x80-\xBF]
	/*:41*/	/*43:*/
UTF8_3	[\xE0-\xEF][\x80-\xBF][\x80-\xBF]
	/*:43*/	/*45:*/
UTF8_4	[\xF0-\xF7][\x80-\xBF][\x80-\xBF][\x80-\xBF]
	/*:45*/	/*149:*/
%x TXT
	/*:149*/

%%

	/*3:*/
"<"	SCAN_START;return START;
">"	SCAN_END;return END;
glyph	return GLYPH;
0|[1-9][0-9]*	SCAN_UDEC(yytext);return UNSIGNED;
\*(0|[1-9][0-9]*)	SCAN_UDEC(yytext+1);return REFERENCE;
[[:space:]]	;
\([^()\n]*[)\n]	;
	/*:3*/	/*24:*/
0x{HEX}+	SCAN_HEX(yytext+2);return UNSIGNED;
	/*:24*/	/*27:*/
[+-](0|[1-9][0-9]*)	SCAN_DEC(yytext);return SIGNED;
	/*:27*/	/*34:*/
'	STR_START;BEGIN(STR);
<STR>{
'	STR_END;SCAN_STR;BEGIN(INITIAL);return STRING;
''	STR_ADD('\'');
[\x20-\x7E]	STR_ADD(yytext[0]);
.	RNG("String character",yytext[0],0x20,0x7E);
\n	QUIT("Unterminated String in line %d",yylineno);
}


	/*:34*/	/*48:*/
'''	STR_START;STR_PUT('\'');BEGIN(STR);
''''	SCAN_UTF8_1(yytext+1);return CHARCODE;
'[\x20-\x7E]''	STR_START;STR_PUT(yytext[1]);STR_PUT('\'');BEGIN(STR);
'''''	STR_START;STR_PUT('\'');STR_PUT('\'');BEGIN(STR);
'{UTF8_1}'	SCAN_UTF8_1(yytext+1);return CHARCODE;
'{UTF8_2}'	SCAN_UTF8_2(yytext+1);return CHARCODE;
'{UTF8_3}'	SCAN_UTF8_3(yytext+1);return CHARCODE;
'{UTF8_4}'	SCAN_UTF8_4(yytext+1);return CHARCODE;
	/*:48*/	/*55:*/
[+-]?[0-9]+\.[0-9]+(e[+-]?[0-9])?	SCAN_DECFLOAT;return FPNUM;
	/*:55*/	/*59:*/
[+-]?0x{HEX}+\.{HEX}+(x[+-]?{HEX}+)?	SCAN_HEXFLOAT;return FPNUM;
	/*:59*/	/*80:*/
dimen	return DIMEN;
pt	return PT;
mm	return MM;
in	return INCH;
	/*:80*/	/*88:*/
xdimen	return XDIMEN;
h	return H;
v	return V;
	/*:88*/	/*99:*/
fil	return FIL;
fill	return FILL;
filll	return FILLL;
	/*:99*/	/*103:*/
penalty	return PENALTY;
int	return INTEGER;
	/*:103*/	/*109:*/
language	return LANGUAGE;
	/*:109*/	/*115:*/
rule	return RULE;
"|"	return RUNNING;
"_"	return RUNNING;
	/*:115*/	/*124:*/
kern	return KERN;
!	return EXPLICIT;
	/*:124*/	/*133:*/
glue	return GLUE;
plus	return PLUS;
minus	return MINUS;
	/*:133*/	/*151:*/
\"	SCAN_TXT_START;return TXT_START;

<TXT>{
\"	SCAN_TXT_END;return TXT_END;

"<"	SCAN_START;return START;
">"	QUIT("> not allowed in text mode");

\\\\	yylval.u= '\\';return TXT_CC;
\\\"	yylval.u= '"';return TXT_CC;
\\"<"	yylval.u= '<';return TXT_CC;
\\">"	yylval.u= '>';return TXT_CC;
\\" "	yylval.u= ' ';return TXT_CC;
\\"-"	yylval.u= '-';return TXT_CC;
\\"@"	return TXT_IGNORE;

[ \t\r]*(\n[ \t\r]*)+	return TXT_FONT_GLUE;
\\[ \t\r]*\n[ \t\r]*	;

\\[0-7]	yylval.u= yytext[1]-'0';return TXT_FONT;

\\F[0-9]+\\	SCAN_REF(font_kind);return TXT_GLOBAL;
\\P[0-9]+\\	SCAN_REF(penalty_kind);return TXT_GLOBAL;
\\K[0-9]+\\	SCAN_REF(kern_kind);return TXT_GLOBAL;
\\L[0-9]+\\	SCAN_REF(ligature_kind);return TXT_GLOBAL;
\\D[0-9]+\\	SCAN_REF(disc_kind);return TXT_GLOBAL;
\\G[0-9]+\\	SCAN_REF(glue_kind);return TXT_GLOBAL;
\\S[0-9]+\\	SCAN_REF(language_kind);return TXT_GLOBAL;
\\R[0-9]+\\	SCAN_REF(rule_kind);return TXT_GLOBAL;
\\I[0-9]+\\	SCAN_REF(image_kind);return TXT_GLOBAL;


\\C[0-9]+\\	SCAN_UDEC(yytext+2);return TXT_CC;

\\[a-l]	yylval.u= yytext[1]-'a';return TXT_LOCAL;
" "	return TXT_FONT_GLUE;
"-"	return TXT_FONT_HYPHEN;

{UTF8_1}	SCAN_UTF8_1(yytext);return TXT_CC;
{UTF8_2}	SCAN_UTF8_2(yytext);return TXT_CC;
{UTF8_3}	SCAN_UTF8_3(yytext);return TXT_CC;
{UTF8_4}	SCAN_UTF8_4(yytext);return TXT_CC;
}
	/*:151*/	/*162:*/
hbox	return HBOX;
vbox	return VBOX;
shifted	return SHIFTED;
	/*:162*/	/*170:*/
hpack	return HPACK;
hset	return HSET;
vpack	return VPACK;
vset	return VSET;
add	return ADD;
to	return TO;
depth	return DEPTH;
	/*:170*/	/*175:*/
leaders	return LEADERS;
align	return ALIGN;
center	return CENTER;
expand	return EXPAND;
	/*:175*/	/*182:*/
baseline	return BASELINE;
	/*:182*/	/*189:*/
ligature	return LIGATURE;
	/*:189*/	/*197:*/
disc	return DISC;
	/*:197*/	/*205:*/
par	return PAR;
	/*:205*/	/*210:*/
math	return MATH;
	/*:210*/	/*215:*/
on	return ON;
off	return OFF;
	/*:215*/	/*219:*/
adjust	return ADJUST;
	/*:219*/	/*223:*/
table	return TABLE;
item	return ITEM;
row	return ITEM;
column	return ITEM;
	/*:223*/	/*230:*/
image	return IMAGE;
	/*:230*/	/*247:*/
label	return LABEL;
bot	return BOT;
mid	return MID;
	/*:247*/	/*261:*/
link	return LINK;
	/*:261*/	/*271:*/
outline	return OUTLINE;
	/*:271*/	/*278:*/
stream	if(section_no==1)return STREAMDEF;else return STREAM;
first	return FIRST;
last	return LAST;
top	return TOP;
\*	return NOREFERENCE;
	/*:278*/	/*288:*/
page	return PAGE;
	/*:288*/	/*296:*/
range	return RANGE;
	/*:296*/	/*323:*/
directory	return DIRECTORY;
section	return SECTION;
	/*:323*/	/*342:*/
definitions	return DEFINITIONS;
	/*:342*/	/*350:*/
max	return MAX;
	/*:350*/	/*365:*/
param	return PARAM;
	/*:365*/	/*374:*/
font	return FONT;
	/*:374*/	/*402:*/
content	return CONTENT;
	/*:402*/
[a-z]+	QUIT("Unexpected keyword '%s' in line %d",yytext,yylineno);
.	QUIT("Unexpected character '%c' (0x%02X) in line %d",yytext[0]>' '?yytext[0]:' ',yytext[0],yylineno);

%%
	/*:509*/
