/* otp-lexer.l: Lexical analysis for OTP files

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous
Copyright (c) 2002 Behdad Esfahbod
Copyright (C) 2002, 2005, 2006 Roozbeh Pournader

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

%option never-interactive

%top{
#include "routines.h"
#include "yystype.h"
#include "otp-parser.h"
}

%{
#define YY_NO_UNPUT
#define OTP_MAXCODE 0xFFFF
int line_number = 1;
extern YYSTYPE yylval;
%}


hexnumber	@\"[a-fA-F0-9]+
octnumber	@'[0-7]+
decnumber	[0-9]+
id		([a-zA-Z][_a-zA-Z0-9]*)
string		\"([^\"]|\"\")*\"

%%
[ \t]		{}
"%".*\n		{ line_number++;}
\r\n|\n|\r	{ line_number++;}
{hexnumber}	{
		int i, j;
		yylval.yint=0;
		for (i=2; i<yyleng; i++) {
			j = yytext[i];
			if ((j>='0')&&(j<='9')) {
				yylval.yint = (yylval.yint*16)+(j-'0');
			} else if ((j>='a')&&(j<='f')) {
				yylval.yint = (yylval.yint*16)+(j-'a'+10);
			} else { /* ((j>='A') && (j>='F')) */
				yylval.yint = (yylval.yint*16)+(j-'A'+10);
			}
			if (yylval.yint>OTP_MAXCODE) {
			   fprintf(stderr, "line %d: value too large (%x)\n",
			   	line_number, yylval.yint);
			   exit(1);
			}
		}
		return(NUMBER);
		}
{octnumber}	{
		int i, j;
		yylval.yint=0;
		for (i=2; i<yyleng; i++) {
			j = yytext[i];
			yylval.yint = (yylval.yint*8)+(j-'0');
			if (yylval.yint>OTP_MAXCODE) {
			   fprintf(stderr, "line %d: value too large (%o)\n",
				   line_number, yylval.yint);
			   exit(1);
			}
		}
		return(NUMBER);
		}
{decnumber}	{
		int i, j;
		yylval.yint=0;
		for (i=0; i<yyleng; i++) {
			j = yytext[i];
			yylval.yint = (yylval.yint*10)+(j-'0');
			if (yylval.yint>OTP_MAXCODE) {
			    fprintf(stderr, "line %d: value too large (%d)\n",
				    line_number, yylval.yint);
			    exit(1);
			}
		}
		return(NUMBER);
		}
"`"[\x20-\x7e]"'" {
		yylval.yint=yytext[1]; 
		return(NUMBER);
		}
{id}		{ yylval.ystring=xstrdup(yytext); return(ID); }
{string}	{
		int i, j, last;
		char *newtext;
		last=yyleng-2;
		i=0;
		j=1;
		newtext=yylval.ystring=xstrdup(yytext);
		while (j<=last) {
		   newtext[i]=yytext[j];
		   if ((yytext[j]=='"') && (yytext[j+1]=='"')) j=j+2;
		   else j++;
		   i++;
		}
		newtext[i]='\0';
		return(STRING);
		}
=>		return(RIGHTARROW);
"<"=		return(LEFTARROW);
input:		return(MYINPUT);
output:		return(OUTPUT);
aliases:	return(ALIASES);
states:		return(STATES);
tables:		return(TABLES);
expressions:	return(EXPRESSIONS);
push:		return(PUSH);
pop:		return(POP);
div:		return(DIV);
mod:		return(MOD);
beg:		return(BEG);
end:		return(END);
.		return(yytext[0]);

%%

