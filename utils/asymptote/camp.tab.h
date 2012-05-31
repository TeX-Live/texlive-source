/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     SELFOP = 259,
     DOTS = 260,
     COLONS = 261,
     DASHES = 262,
     INCR = 263,
     LONGDASH = 264,
     CONTROLS = 265,
     TENSION = 266,
     ATLEAST = 267,
     CURL = 268,
     COR = 269,
     CAND = 270,
     BAR = 271,
     AMPERSAND = 272,
     EQ = 273,
     NEQ = 274,
     LT = 275,
     LE = 276,
     GT = 277,
     GE = 278,
     CARETS = 279,
     LOGNOT = 280,
     OPERATOR = 281,
     LOOSE = 282,
     ASSIGN = 283,
     DIRTAG = 284,
     JOIN_PREC = 285,
     AND = 286,
     ELLIPSIS = 287,
     ACCESS = 288,
     UNRAVEL = 289,
     IMPORT = 290,
     INCLUDE = 291,
     FROM = 292,
     QUOTE = 293,
     STRUCT = 294,
     TYPEDEF = 295,
     NEW = 296,
     IF = 297,
     ELSE = 298,
     WHILE = 299,
     DO = 300,
     FOR = 301,
     BREAK = 302,
     CONTINUE = 303,
     RETURN_ = 304,
     THIS = 305,
     EXPLICIT = 306,
     GARBAGE = 307,
     LIT = 308,
     STRING = 309,
     PERM = 310,
     MODIFIER = 311,
     UNARY = 312,
     EXP_IN_PARENS_RULE = 313
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 60 "camp.y"

  position pos;
  bool boo;
  struct {
    position pos;
    sym::symbol sym;
  } ps;
  absyntax::name *n;
  absyntax::varinit *vi;
  absyntax::arrayinit *ai;
  absyntax::exp *e;
  absyntax::stringExp *stre;
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



/* Line 2068 of yacc.c  */
#line 157 "camp.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


