/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     ADD = 259,
     SUBTRACT = 260,
     TIMES = 261,
     DIVIDE = 262,
     MOD = 263,
     EXPONENT = 264,
     DOTS = 265,
     COLONS = 266,
     DASHES = 267,
     INCR = 268,
     LONGDASH = 269,
     CONTROLS = 270,
     TENSION = 271,
     ATLEAST = 272,
     CURL = 273,
     COR = 274,
     CAND = 275,
     BAR = 276,
     AMPERSAND = 277,
     EQ = 278,
     NEQ = 279,
     LT = 280,
     LE = 281,
     GT = 282,
     GE = 283,
     CARETS = 284,
     LOGNOT = 285,
     OPERATOR = 286,
     STRING = 287,
     LOOSE = 288,
     ASSIGN = 289,
     DIRTAG = 290,
     JOIN_PREC = 291,
     AND = 292,
     ELLIPSIS = 293,
     ACCESS = 294,
     UNRAVEL = 295,
     IMPORT = 296,
     INCLUDE = 297,
     FROM = 298,
     QUOTE = 299,
     STRUCT = 300,
     TYPEDEF = 301,
     NEW = 302,
     IF = 303,
     ELSE = 304,
     WHILE = 305,
     DO = 306,
     FOR = 307,
     BREAK = 308,
     CONTINUE = 309,
     RETURN_ = 310,
     THIS = 311,
     EXPLICIT = 312,
     GARBAGE = 313,
     LIT = 314,
     PERM = 315,
     MODIFIER = 316,
     UNARY = 317,
     EXP_IN_PARENS_RULE = 318
   };
#endif
/* Tokens.  */
#define ID 258
#define ADD 259
#define SUBTRACT 260
#define TIMES 261
#define DIVIDE 262
#define MOD 263
#define EXPONENT 264
#define DOTS 265
#define COLONS 266
#define DASHES 267
#define INCR 268
#define LONGDASH 269
#define CONTROLS 270
#define TENSION 271
#define ATLEAST 272
#define CURL 273
#define COR 274
#define CAND 275
#define BAR 276
#define AMPERSAND 277
#define EQ 278
#define NEQ 279
#define LT 280
#define LE 281
#define GT 282
#define GE 283
#define CARETS 284
#define LOGNOT 285
#define OPERATOR 286
#define STRING 287
#define LOOSE 288
#define ASSIGN 289
#define DIRTAG 290
#define JOIN_PREC 291
#define AND 292
#define ELLIPSIS 293
#define ACCESS 294
#define UNRAVEL 295
#define IMPORT 296
#define INCLUDE 297
#define FROM 298
#define QUOTE 299
#define STRUCT 300
#define TYPEDEF 301
#define NEW 302
#define IF 303
#define ELSE 304
#define WHILE 305
#define DO 306
#define FOR 307
#define BREAK 308
#define CONTINUE 309
#define RETURN_ 310
#define THIS 311
#define EXPLICIT 312
#define GARBAGE 313
#define LIT 314
#define PERM 315
#define MODIFIER 316
#define UNARY 317
#define EXP_IN_PARENS_RULE 318




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 46 "camp.y"
{
  position pos;
  bool boo;
  struct {
    position pos;
    sym::symbol *sym;
  } ps;
  absyntax::name *n;
  absyntax::varinit *vi;
  absyntax::arrayinit *ai;
  absyntax::exp *e;
  absyntax::specExp *se;
  absyntax::joinExp *j;
  absyntax::explist *elist;
  absyntax::argument arg;
  absyntax::arglist *alist;
  absyntax::slice *slice;
  absyntax::dimensions *dim;
  absyntax::ty  *t;
  absyntax::decid *di;
  absyntax::decidlist *dil;
  absyntax::decidstart *dis;
  absyntax::runnable *run;
  struct {
    position pos;
    trans::permission val;
  } perm;
  struct {
    position pos;
    trans::modifier val;
  } mod;
  absyntax::modifierList *ml;
  //absyntax::program *prog;
  absyntax::vardec *vd;
  //absyntax::vardecs *vds;
  absyntax::dec *d;
  absyntax::idpair *ip;
  absyntax::idpairlist *ipl;
  absyntax::stm *s;
  absyntax::block *b;
  absyntax::stmExpList *sel;
  //absyntax::funheader *fh;
  absyntax::formal *fl;
  absyntax::formals *fls;
}
/* Line 1489 of yacc.c.  */
#line 221 "camp.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

