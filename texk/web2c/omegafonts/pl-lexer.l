%{
/* pl-lexer.l: Lexical analysis of property list files.

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright 2020 Karl Berry <tex-live@tug.org>
Copyright 2002 Roozbeh Pournader
Copyright 1994--2001 John Plaice and Yannis Haralambous

Omega is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Omega is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Omega; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

/* The xyarabic.opl test has an opl comment larger than 16k, which is
   the default buffer size for flex defined in its preamble.  This
   results in an error like:
input buffer overflow, can't enlarge buffer because scanner uses REJECT
   So we need to redefine YY_BUF_SIZE, which we do here.  Unfortunately
   it is not perfect, since the flex preamble code uses its own
   YY_BUF_SIZE to #define YY_STATE_BUF_SIZE before our override takes
   effect.  Fortunately this is not causing an error.  Apparently the
   only way to truly override it would be on the command line, and that
   seems like too much trouble when this works in practice.  */
#undef YY_BUF_SIZE
#define YY_BUF_SIZE 110000

#include <kpathsea/lib.h>
#include <kpathsea/c-memstr.h>
#include "parser.h"
#include "pl-parser.h"
#include "manifests.h"
#include "error_routines.h"
#ifdef __cplusplus
#define yyinput input
#endif

int line_number = 1;
extern YYSTYPE yylval;

/* POTENTIAL BUG: saved_text could be run over; should check */
#define MAX_PTR 100000
char saved_text[MAX_PTR];

static void scan_int(unsigned);
static void scan_char(void);
static void scan_fix(void);
static void scan_string(const char *, unsigned, unsigned);
static void scan_hex_string(void);

#define KEEP_MIN      0
#define KEEP_NONE     0
#define KEEP_ALL      1
#define KEEP_CONVERT  2
#define KEEP_MAX      2

#define BASE_MIN       2
#define BASE_MAX      16

%}

/* For Solaris's lex, to increase tables sizes --RP */
/* Space before number keeps ancient flex happy. */
%e 4000
%p 7000
%n 1000

ws		[ \t]+
hexnumber       H{ws}[A-F0-9]+
octnumber       O{ws}[0-7]+
decnumber       D{ws}[0-9]+
realnumber      R[ \t+-]+[0-9]*("."[0-9]*)?
charnumber      C{ws}.

%%

{ws}		{}
\n         	{line_number++; }

"("		return(LEFT);
")"		return(RIGHT);

{octnumber}	{scan_int(8);  return(NUMBER); }
{decnumber}	{scan_int(10); return(NUMBER); }
{hexnumber}	{scan_int(16); return(NUMBER); }
{charnumber}	{scan_char();  return(NUMBER); }
{realnumber}	{scan_fix();   return(FIX);    }

CODINGSCHEME	{scan_string("CODINGSCHEME", KEEP_CONVERT, LEN_CODING_SCHEME);
                 return(CODINGSCHEME); }
FAMILY		{scan_string("FAMILY", KEEP_CONVERT, LEN_FAMILY);
                 return(FAMILY); }
VTITLE		{scan_string("VTITLE", KEEP_ALL, LEN_VTITLE);
                 return(VTITLE); }
FONTNAME	{scan_string("FONTNAME", KEEP_ALL, LEN_FONT_NAME);
                 return(FONTNAME); }
FONTAREA	{scan_string("FONTAREA", KEEP_ALL, LEN_FONT_AREA);
                 return(FONTAREA); }
SPECIAL		{scan_string("SPECIAL", KEEP_ALL, 0);
                 return(SPECIAL); }
COMMENT		{scan_string("COMMENT", KEEP_NONE, 0);
                 return(COMMENT); }

SPECIALHEX	{scan_hex_string(); return(SPECIALHEX); }


SEVENBITSAFEFLAG{ws}TRUE  {yylval.yint=1; return(SEVENBITSAFEFLAG); }
SEVENBITSAFEFLAG{ws}FALSE {yylval.yint=0; return(SEVENBITSAFEFLAG); }

CHECKSUM	return(CHECKSUM);
DESIGNSIZE	return(DESIGNSIZE);
DESIGNUNITS	return(DESIGNUNITS);
FACE		return(FACE);
HEADER		return(HEADER);
BOUNDARYCHAR	return(BOUNDARYCHAR);
FONTDIMEN	return(FONTDIMEN);
LIGTABLE	return(LIGTABLE);
CHARACTER	return(CHARACTER);

PARAMETER	return(PARAMETER);
LABEL		return(LABEL);
KRN		return(KRN);
STOP		return(STOP);
SKIP		return(SKIP);
NEXTLARGER	return(NEXTLARGER);
VARCHAR		return(VARCHAR);

CHARWD		{yylval.yint = C_WD; return(CHARMEASURE); }
CHARHT		{yylval.yint = C_HT; return(CHARMEASURE); }
CHARDP		{yylval.yint = C_DP; return(CHARMEASURE); }
CHARIC		{yylval.yint = C_IC; return(CHARMEASURE); }

TOP		{yylval.yint = E_TOP; return(EXTEN); }
MID		{yylval.yint = E_MID; return(EXTEN); }
BOT		{yylval.yint = E_BOT; return(EXTEN); }
REP		{yylval.yint = E_REP; return(EXTEN); }

LIG		{yylval.yint = L_0;    return(LIG); }
"LIG/"		{yylval.yint = L_B;    return(LIG); }
"/LIG"		{yylval.yint = L_A;    return(LIG); }
"/LIG/"		{yylval.yint = L_AB;   return(LIG); }
"LIG/>"		{yylval.yint = L_Bx;   return(LIG); }
"/LIG>"		{yylval.yint = L_Ax;   return(LIG); }
"/LIG/>"	{yylval.yint = L_ABx;  return(LIG); }
"/LIG/>>"	{yylval.yint = L_ABxx; return(LIG); }

F{ws}MRR	{yylval.yint = F_MRR; return(NUMBER); }
F{ws}MIR	{yylval.yint = F_MIR; return(NUMBER); }
F{ws}BRR	{yylval.yint = F_BRR; return(NUMBER); }
F{ws}BIR	{yylval.yint = F_BIR; return(NUMBER); }
F{ws}LRR	{yylval.yint = F_LRR; return(NUMBER); }
F{ws}LIR	{yylval.yint = F_LIR; return(NUMBER); }
F{ws}MRC	{yylval.yint = F_MRC; return(NUMBER); }
F{ws}MIC	{yylval.yint = F_MIC; return(NUMBER); }
F{ws}BRC	{yylval.yint = F_BRC; return(NUMBER); }
F{ws}BIC	{yylval.yint = F_BIC; return(NUMBER); }
F{ws}LRC	{yylval.yint = F_LRC; return(NUMBER); }
F{ws}LIC	{yylval.yint = F_LIC; return(NUMBER); }
F{ws}MRE	{yylval.yint = F_MRE; return(NUMBER); }
F{ws}MIE	{yylval.yint = F_MIE; return(NUMBER); }
F{ws}BRE	{yylval.yint = F_BRE; return(NUMBER); }
F{ws}BIE	{yylval.yint = F_BIE; return(NUMBER); }
F{ws}LRE	{yylval.yint = F_LRE; return(NUMBER); }
F{ws}LIE	{yylval.yint = F_LIE; return(NUMBER); }

SLANT		{yylval.yint = P_SLANT;      return(NAMEDPARAMETER); }
SPACE		{yylval.yint = P_SPACE;      return(NAMEDPARAMETER); }
STRETCH		{yylval.yint = P_STRETCH;    return(NAMEDPARAMETER); }
SHRINK		{yylval.yint = P_SHRINK;     return(NAMEDPARAMETER); }
XHEIGHT		{yylval.yint = P_XHEIGHT;    return(NAMEDPARAMETER); }
QUAD		{yylval.yint = P_QUAD;       return(NAMEDPARAMETER); }
EXTRASPACE	{yylval.yint = P_EXTRASPACE; return(NAMEDPARAMETER); }
NUM1		{yylval.yint = P_NUM1;       return(NAMEDPARAMETER); }
NUM2		{yylval.yint = P_NUM2;       return(NAMEDPARAMETER); }
NUM3		{yylval.yint = P_NUM3;       return(NAMEDPARAMETER); }
DENOM1		{yylval.yint = P_DENOM1;     return(NAMEDPARAMETER); }
DENOM2		{yylval.yint = P_DENOM2;     return(NAMEDPARAMETER); }
SUP1		{yylval.yint = P_SUP1;       return(NAMEDPARAMETER); }
SUP2		{yylval.yint = P_SUP2;       return(NAMEDPARAMETER); }
SUP3		{yylval.yint = P_SUP3;       return(NAMEDPARAMETER); }
SUB1		{yylval.yint = P_SUB1;       return(NAMEDPARAMETER); }
SUB2		{yylval.yint = P_SUB2;       return(NAMEDPARAMETER); }
SUPDROP		{yylval.yint = P_SUPDROP;    return(NAMEDPARAMETER); }
SUBDROP		{yylval.yint = P_SUBDROP;    return(NAMEDPARAMETER); }
DELIM1		{yylval.yint = P_DELIM1;     return(NAMEDPARAMETER); }
DELIM2		{yylval.yint = P_DELIM2;     return(NAMEDPARAMETER); }
AXISHEIGHT	{yylval.yint = P_AXISHEIGHT; return(NAMEDPARAMETER); }

DEFAULTRULETHICKNESS	{yylval.yint = P_DEFAULTRULETHICKNESS;
			 return(NAMEDPARAMETER); }
BIGOPSPACING1	{yylval.yint = P_BIGOPSPACING1; return(NAMEDPARAMETER); }
BIGOPSPACING2	{yylval.yint = P_BIGOPSPACING2; return(NAMEDPARAMETER); }
BIGOPSPACING3	{yylval.yint = P_BIGOPSPACING3; return(NAMEDPARAMETER); }
BIGOPSPACING4	{yylval.yint = P_BIGOPSPACING4; return(NAMEDPARAMETER); }
BIGOPSPACING5	{yylval.yint = P_BIGOPSPACING5; return(NAMEDPARAMETER); }

MAPFONT		{return(MAPFONT); }
FONTCHECKSUM	{return(FONTCHECKSUM); }
FONTAT		{return(FONTAT); }
FONTDSIZE	{return(FONTDSIZE); }
MAP		{return(MAP); }
SELECTFONT	{return(SELECTFONT); }
SETCHAR		{return(SETCHAR); }
SETRULE		{return(SETRULE); }
PUSH		{return(PUSH); }
POP		{return(POP); }

MOVERIGHT	{yylval.yint = M_RIGHT; return(MOVE); }
MOVELEFT	{yylval.yint = M_LEFT;  return(MOVE); }
MOVEUP		{yylval.yint = M_UP;    return(MOVE); }
MOVEDOWN	{yylval.yint = M_DOWN;  return(MOVE); }

OFMLEVEL	return(OFMLEVEL);
TOPACCENT	{yylval.yint = ACC_TOP; return(ACCENT); }
MIDACCENT	{yylval.yint = ACC_MID; return(ACCENT); }
BOTACCENT	{yylval.yint = ACC_BOT; return(ACCENT); }

FONTDIR{ws}TL		{yylval.yint = DIR_ORD+DIR_TL; return(FONTDIR);}
FONTDIR{ws}LT		{yylval.yint = DIR_ORD+DIR_LT; return(FONTDIR);}
FONTDIR{ws}TR		{yylval.yint = DIR_ORD+DIR_TR; return(FONTDIR);}
FONTDIR{ws}LB		{yylval.yint = DIR_ORD+DIR_LB; return(FONTDIR);}
FONTDIR{ws}BL		{yylval.yint = DIR_ORD+DIR_BL; return(FONTDIR);}
FONTDIR{ws}RT		{yylval.yint = DIR_ORD+DIR_RT; return(FONTDIR);}
FONTDIR{ws}BR		{yylval.yint = DIR_ORD+DIR_BR; return(FONTDIR);}
FONTDIR{ws}RB		{yylval.yint = DIR_ORD+DIR_RB; return(FONTDIR);}
NATURALFONTDIR{ws}TL	{yylval.yint = DIR_NAT+DIR_TL; return(FONTDIR);}
NATURALFONTDIR{ws}LT	{yylval.yint = DIR_NAT+DIR_LT; return(FONTDIR);}
NATURALFONTDIR{ws}TR	{yylval.yint = DIR_NAT+DIR_TR; return(FONTDIR);}
NATURALFONTDIR{ws}LB	{yylval.yint = DIR_NAT+DIR_LB; return(FONTDIR);}
NATURALFONTDIR{ws}BL	{yylval.yint = DIR_NAT+DIR_BL; return(FONTDIR);}
NATURALFONTDIR{ws}RT	{yylval.yint = DIR_NAT+DIR_RT; return(FONTDIR);}
NATURALFONTDIR{ws}BR	{yylval.yint = DIR_NAT+DIR_BR; return(FONTDIR);}
NATURALFONTDIR{ws}RB	{yylval.yint = DIR_NAT+DIR_RB; return(FONTDIR);}
	/* for compatibility with the omegaware versions */
NFONTDIR{ws}TL		{yylval.yint = DIR_NAT+DIR_TL; return(FONTDIR);}
NFONTDIR{ws}LT		{yylval.yint = DIR_NAT+DIR_LT; return(FONTDIR);}
NFONTDIR{ws}TR		{yylval.yint = DIR_NAT+DIR_TR; return(FONTDIR);}
NFONTDIR{ws}LB		{yylval.yint = DIR_NAT+DIR_LB; return(FONTDIR);}
NFONTDIR{ws}BL		{yylval.yint = DIR_NAT+DIR_BL; return(FONTDIR);}
NFONTDIR{ws}RT		{yylval.yint = DIR_NAT+DIR_RT; return(FONTDIR);}
NFONTDIR{ws}BR		{yylval.yint = DIR_NAT+DIR_BR; return(FONTDIR);}
NFONTDIR{ws}RB		{yylval.yint = DIR_NAT+DIR_RB; return(FONTDIR);}

FONTIVALUE	return(FONTIVALUE);
FONTFVALUE	return(FONTFVALUE);
FONTMVALUE	return(FONTMVALUE);
FONTPENALTY	return(FONTPENALTY);
FONTRULE	return(FONTRULE);
FONTGLUE	return(FONTGLUE);

IVALUE		return(IVALUE);
FVALUE		return(FVALUE);
MVALUE		return(MVALUE);
PENALTY		return(PENALTY);
RULE		return(RULE);
GLUE		return(GLUE);

IVALUEVAL	return(IVALUEVAL);
FVALUEVAL	return(FVALUEVAL);
MVALUEVAL	return(MVALUEVAL);
PENALTYVAL	return(PENALTYVAL);

RULEWD		{ yylval.yint = RULE_WD; return(RULEMEASURE); }
RULEHT		{ yylval.yint = RULE_HT; return(RULEMEASURE); }
RULEDP		{ yylval.yint = RULE_DP; return(RULEMEASURE); }

GLUEWD		return(GLUEWD);
GLUESTRETCH	{ yylval.yint = GLUE_STRETCH; return(GLUESHRINKSTRETCH); }
GLUESHRINK	{ yylval.yint = GLUE_SHRINK; return(GLUESHRINKSTRETCH); }

GLUETYPE	return(GLUETYPE);
GLUERULE	return(GLUERULE);
GLUECHAR	return(GLUECHAR);

CHARIVALUE	return(CHARIVALUE);
CHARFVALUE	return(CHARFVALUE);
CHARMVALUE	return(CHARMVALUE);
CHARPENALTY	return(CHARPENALTY);
CHARRULE	return(CHARRULE);
CHARGLUE	return(CHARGLUE);

CKRN		return(CKRN);
CGLUE		return(CGLUE);
CPENALTY	return(CPENALTY);
CPENGLUE	return(CPENGLUE);
CLABEL		return(CLABEL);

CHARREPEAT	return(CHARREPEAT);


FILLL		{yylval.yint = O_FILLL; return(GLUEORDER); }
FILL		{yylval.yint = O_FILL;  return(GLUEORDER); }
FIL		{yylval.yint = O_FIL;   return(GLUEORDER); }
FI		{yylval.yint = O_FI;    return(GLUEORDER); }
UNIT		{yylval.yint = O_UNIT;  return(GLUEORDER); }

NORMAL		{yylval.yint = K_NORMAL;   return(GLUEKIND); }
ALEADERS	{yylval.yint = K_ALEADERS; return(GLUEKIND); }
CLEADERS	{yylval.yint = K_CLEADERS; return(GLUEKIND); }
XLEADERS	{yylval.yint = K_XLEADERS; return(GLUEKIND); }

SECWD		{yylval.yint = C_SECWD;        return(CHARMEASURE); }
SECHT		{yylval.yint = C_SECHT;        return(CHARMEASURE); }
SECDP		{yylval.yint = C_SECDP;        return(CHARMEASURE); }
SECIC		{yylval.yint = C_SECIC;        return(CHARMEASURE); }

PRIMTOPAXIS	{yylval.yint = C_P_TOPAXIS;    return(CHARMEASURE); }
PRIMTOPAXISBIS	{yylval.yint = C_P_TOPAXISBIs; return(CHARMEASURE); }
PRIMBOTAXIS	{yylval.yint = C_P_BOTAXIS;    return(CHARMEASURE); }
PRIMBOTAXISBIS	{yylval.yint = C_P_BOTAXISBIS; return(CHARMEASURE); }
PRIMMIDHOR	{yylval.yint = C_P_MIDHOR;     return(CHARMEASURE); }
PRIMMIDVERT	{yylval.yint = C_P_MIDVERT;    return(CHARMEASURE); }
PRIMBASESLANT	{yylval.yint = C_P_BASESLANT;  return(CHARMEASURE); }

SECTOPAXIS	{yylval.yint = C_S_TOPAXIS;    return(CHARMEASURE); }
SECTOPAXISBIS	{yylval.yint = C_S_TOPAXISBIs; return(CHARMEASURE); }
SECBOTAXIS	{yylval.yint = C_S_BOTAXIS;    return(CHARMEASURE); }
SECBOTAXISBIS	{yylval.yint = C_S_BOTAXISBIS; return(CHARMEASURE); }
SECMIDHOR	{yylval.yint = C_S_MIDHOR;     return(CHARMEASURE); }
SECMIDVERT	{yylval.yint = C_S_MIDVERT;    return(CHARMEASURE); }
SECBASESLANT	{yylval.yint = C_S_BASESLANT;  return(CHARMEASURE); }

.		{lex_error_1("unexpected character (%x); ignored",
                             yytext[0]);}
%%

/* added by Thomas Esser, suggested by Olaf Weber */
#ifdef yywrap
#undef yywrap
#endif

int
yywrap (void)
{ return 1; }

static void
scan_int(unsigned base)
{
    register unsigned j, q = 0x10000 / base, c0=0, c1=0, i=1;

    if ((base<BASE_MIN) || (base>BASE_MAX))
        internal_error_1("scan_int (base=%d)", base);
    while ((yytext[i]==' ') || (yytext[i]=='\t')) {
        yytext[i] = ' ';
        i++;
        }
    for (; i<yyleng; i++) {
    	j = yytext[i];
    	if (j>='A') j = j + '0' + 10 - 'A' ;
    	c0 = base*c0 + (c1 / q);
    	c1 = base*(c1 % q) + j - '0';
    	if (c0 > 0xffff) {
      	    lex_error_s("numeric value (%s) too large; set to 0", yytext);
    	    c0=0; c1=0; break;
    	}
    }
    yylval.yint = c0 * 0x10000 + c1;
}

static void
scan_fix(void)
{
    unsigned i=1;
    unsigned sign = 1;
    unsigned j=0;
    unsigned acc=0;
    unsigned int_part;
    unsigned fraction_digits[7];

    while ((yytext[i]==' ') || (yytext[i]=='\t') ||
           (yytext[i]=='+') || (yytext[i]=='-')) {
    	if (yytext[i]=='\t') yytext[i] = ' ';
    	if (yytext[i]=='-') sign *= -1;
    	i++;
    }
    acc=0;
    while ((i<yyleng) && (yytext[i]!='.')) {
    	acc = acc*10 + yytext[i]-'0';
    	i++;
    	if (acc >=0x800 ) {
    	    lex_error_s("fix value (%s) too large; set to 0", yytext);
    	    yylval.yfix = 0;
    	    return;
    	}
    }
    int_part = acc; acc = 0;
    if (i < yyleng) {
    	i++;
    	while ((i<yyleng) && j<7) {
    	    fraction_digits[j] = 0x200000*(yytext[i]-'0');
    	    i++; j++;
    	}
    	while (j>0) {acc = fraction_digits[--j] + acc / 10;}
    	acc = (acc + 10) / 20;
    }
    if ((acc > UNITY) && (int_part=2047)) {
    	lex_error_s("fix value (%s) too large; set to 0", yytext);
    	yylval.yfix = 0;
    	return;
    }
    yylval.yfix = sign * (int_part*UNITY + acc);
}

static void
scan_char(void)
{
    register unsigned i=1;

    while ((yytext[i]==' ') || (yytext[i]=='\t')) i++;
    if ((yytext[i]<041) || (yytext[i]>0176) ||
        (yytext[i]=='(') || (yytext[i]==')')) {
        lex_error_1("C value (H %X) must be ASCII (not paren); "
                    "set to 'A'", yytext[i]);
    	yylval.yint = 'A';
    } else yylval.yint = yytext[i];
}

static void
scan_string(const char *attribute, unsigned keep, unsigned length)
{
    register unsigned c, saved_ptr = 0, paren_level = 0;
    unsigned error_msg = FALSE;

    if ((keep<KEEP_MIN) || (keep>KEEP_MAX))
        internal_error_1("scan_string (keep=%d)", keep);
    while (((c = input()) != EOF) &&
    	((c == ' ') || (c == '\t') || (c == '\n'))) {
    	if (c == '\n') {line_number++;}
    }
    if (c==EOF) fatal_error_s("EOF while scanning %s", attribute);
    while ((c != EOF) &&
    	((c != ')') || (paren_level>0))) {
    	if (c==')') {
            paren_level--;
            if (keep==KEEP_CONVERT) {
                lex_error_s_1("%s character (H %X) converted to slash",
                             attribute, c);
                c = '/';
            }
        } else if (c=='(') {
            paren_level++;
            if (keep==KEEP_CONVERT) {
                lex_error_s_1("%s character (H %X) converted to slash",
                             attribute, c);
                c = '/';
            }
    	} else if ((c<' ') || (c>='~')) {
            if (c=='\n') line_number++;
            if (keep==KEEP_CONVERT) {
                lex_error_s_1("%s character (H %X) converted to blank",
                             attribute, c);
                c = ' ';
            }
        }
    	if (saved_ptr<(MAX_PTR-3))
            saved_text[saved_ptr++] = c;
        else if (error_msg == FALSE) {
            lex_error_s_1("%s string longer than %d characters; truncating",
                          attribute, MAX_PTR);
            error_msg = TRUE;
        }
    	c = input();
    }
    if (c==EOF) fatal_error_s("EOF while scanning %s", attribute);
    unput(')');
    saved_text[saved_ptr++] = '\0';
    if (keep!=KEEP_NONE) {
        if ((length != 0) && (strlen(saved_text)>length)) {
            lex_error_s_1("%s string limited to %d characters; truncated",
                          attribute, length);
            saved_text[length] = '\0';
        }
        yylval.ystring = xstrdup(saved_text);
    } else {
        yylval.ystring = NULL;
    }
}

static void
scan_hex_string(void)
{
    register unsigned c, saved_ptr = 0;
    unsigned error_msg = FALSE;

    while ((c = input()) != EOF) {
        if (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
            if (saved_ptr<(MAX_PTR-3))
                saved_text[saved_ptr++] = c;
            else if (error_msg == FALSE) {
                lex_error_1("SPECIALHEX string longer than %d characters; truncating",
                            MAX_PTR);
                error_msg = TRUE;
            }
        } else if (c == '\n') {
            line_number++;
        } else if ((c != ' ') && (c != '\t')) {
            unput(c);
            break;
        }
    }
    if (c == EOF) fatal_error_0("EOF while scanning SPECIALHEX");
    saved_text[saved_ptr++] = '\0';
    yylval.ystring = xstrdup(saved_text);
}
