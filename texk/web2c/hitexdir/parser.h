/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_SHRINK_TAB_H_INCLUDED
# define YY_YY_SHRINK_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     START = 258,
     END = 259,
     GLYPH = 260,
     UNSIGNED = 261,
     REFERENCE = 262,
     SIGNED = 263,
     STRING = 264,
     CHARCODE = 265,
     FPNUM = 266,
     DIMEN = 267,
     PT = 268,
     MM = 269,
     INCH = 270,
     XDIMEN = 271,
     H = 272,
     V = 273,
     FIL = 274,
     FILL = 275,
     FILLL = 276,
     PENALTY = 277,
     INTEGER = 278,
     LANGUAGE = 279,
     RULE = 280,
     RUNNING = 281,
     KERN = 282,
     EXPLICIT = 283,
     GLUE = 284,
     PLUS = 285,
     MINUS = 286,
     TXT_START = 287,
     TXT_END = 288,
     TXT_IGNORE = 289,
     TXT_FONT_GLUE = 290,
     TXT_FONT_HYPHEN = 291,
     TXT_FONT = 292,
     TXT_LOCAL = 293,
     TXT_GLOBAL = 294,
     TXT_CC = 295,
     HBOX = 296,
     VBOX = 297,
     SHIFTED = 298,
     HPACK = 299,
     HSET = 300,
     VPACK = 301,
     VSET = 302,
     DEPTH = 303,
     ADD = 304,
     TO = 305,
     LEADERS = 306,
     ALIGN = 307,
     CENTER = 308,
     EXPAND = 309,
     BASELINE = 310,
     LIGATURE = 311,
     DISC = 312,
     PAR = 313,
     MATH = 314,
     ON = 315,
     OFF = 316,
     ADJUST = 317,
     TABLE = 318,
     ITEM = 319,
     IMAGE = 320,
     LABEL = 321,
     BOT = 322,
     MID = 323,
     LINK = 324,
     OUTLINE = 325,
     STREAM = 326,
     STREAMDEF = 327,
     FIRST = 328,
     LAST = 329,
     TOP = 330,
     NOREFERENCE = 331,
     PAGE = 332,
     RANGE = 333,
     DIRECTORY = 334,
     SECTION = 335,
     DEFINITIONS = 336,
     MAX = 337,
     PARAM = 338,
     FONT = 339,
     CONTENT = 340
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2058 of yacc.c  */
#line 79 "shrink.y"

	#line 10241 "format.w"
	uint32_t u;  int32_t i;  char *s;  float64_t f;  glyph_t c;
	dimen_t d;stretch_t st;xdimen_t xd;kern_t kt;
	rule_t r;glue_t g;image_t x;
	list_t l;box_t h;disc_t dc;lig_t lg;
	ref_t rf;info_t info;order_t o;bool b;
	

/* Line 2058 of yacc.c  */
#line 152 "shrink.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_YY_SHRINK_TAB_H_INCLUDED  */
