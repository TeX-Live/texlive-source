/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY__TEX_LIVE_TEXK_WEB_C_OMEGAFONTS_PL_PARSER_H_INCLUDED
# define YY_YY__TEX_LIVE_TEXK_WEB_C_OMEGAFONTS_PL_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    LEFT = 258,
    RIGHT = 259,
    NUMBER = 260,
    FIX = 261,
    COMMENT = 262,
    CHECKSUM = 263,
    DESIGNSIZE = 264,
    DESIGNUNITS = 265,
    CODINGSCHEME = 266,
    FAMILY = 267,
    FACE = 268,
    SEVENBITSAFEFLAG = 269,
    HEADER = 270,
    FONTDIMEN = 271,
    LIGTABLE = 272,
    BOUNDARYCHAR = 273,
    CHARACTER = 274,
    NAMEDPARAMETER = 275,
    PARAMETER = 276,
    CHARMEASURE = 277,
    NEXTLARGER = 278,
    VARCHAR = 279,
    EXTEN = 280,
    LABEL = 281,
    LIG = 282,
    KRN = 283,
    STOP = 284,
    SKIP = 285,
    VTITLE = 286,
    MAPFONT = 287,
    FONTNAME = 288,
    FONTAREA = 289,
    FONTCHECKSUM = 290,
    FONTAT = 291,
    FONTDSIZE = 292,
    MAP = 293,
    SELECTFONT = 294,
    SETCHAR = 295,
    SETRULE = 296,
    MOVE = 297,
    PUSH = 298,
    POP = 299,
    SPECIAL = 300,
    SPECIALHEX = 301,
    CHARREPEAT = 302,
    FONTIVALUE = 303,
    FONTFVALUE = 304,
    FONTMVALUE = 305,
    FONTPENALTY = 306,
    FONTRULE = 307,
    FONTGLUE = 308,
    CLABEL = 309,
    CKRN = 310,
    CGLUE = 311,
    CPENALTY = 312,
    CPENGLUE = 313,
    CHARIVALUE = 314,
    CHARFVALUE = 315,
    CHARMVALUE = 316,
    CHARPENALTY = 317,
    CHARRULE = 318,
    CHARGLUE = 319,
    IVALUE = 320,
    IVALUEVAL = 321,
    MVALUE = 322,
    MVALUEVAL = 323,
    FVALUE = 324,
    FVALUEVAL = 325,
    PENALTY = 326,
    PENALTYVAL = 327,
    RULE = 328,
    RULEMEASURE = 329,
    GLUE = 330,
    GLUEWD = 331,
    GLUETYPE = 332,
    GLUEKIND = 333,
    GLUERULE = 334,
    GLUECHAR = 335,
    GLUESHRINKSTRETCH = 336,
    GLUEORDER = 337,
    OFMLEVEL = 338,
    FONTDIR = 339,
    ACCENT = 340
  };
#endif
/* Tokens.  */
#define LEFT 258
#define RIGHT 259
#define NUMBER 260
#define FIX 261
#define COMMENT 262
#define CHECKSUM 263
#define DESIGNSIZE 264
#define DESIGNUNITS 265
#define CODINGSCHEME 266
#define FAMILY 267
#define FACE 268
#define SEVENBITSAFEFLAG 269
#define HEADER 270
#define FONTDIMEN 271
#define LIGTABLE 272
#define BOUNDARYCHAR 273
#define CHARACTER 274
#define NAMEDPARAMETER 275
#define PARAMETER 276
#define CHARMEASURE 277
#define NEXTLARGER 278
#define VARCHAR 279
#define EXTEN 280
#define LABEL 281
#define LIG 282
#define KRN 283
#define STOP 284
#define SKIP 285
#define VTITLE 286
#define MAPFONT 287
#define FONTNAME 288
#define FONTAREA 289
#define FONTCHECKSUM 290
#define FONTAT 291
#define FONTDSIZE 292
#define MAP 293
#define SELECTFONT 294
#define SETCHAR 295
#define SETRULE 296
#define MOVE 297
#define PUSH 298
#define POP 299
#define SPECIAL 300
#define SPECIALHEX 301
#define CHARREPEAT 302
#define FONTIVALUE 303
#define FONTFVALUE 304
#define FONTMVALUE 305
#define FONTPENALTY 306
#define FONTRULE 307
#define FONTGLUE 308
#define CLABEL 309
#define CKRN 310
#define CGLUE 311
#define CPENALTY 312
#define CPENGLUE 313
#define CHARIVALUE 314
#define CHARFVALUE 315
#define CHARMVALUE 316
#define CHARPENALTY 317
#define CHARRULE 318
#define CHARGLUE 319
#define IVALUE 320
#define IVALUEVAL 321
#define MVALUE 322
#define MVALUEVAL 323
#define FVALUE 324
#define FVALUEVAL 325
#define PENALTY 326
#define PENALTYVAL 327
#define RULE 328
#define RULEMEASURE 329
#define GLUE 330
#define GLUEWD 331
#define GLUETYPE 332
#define GLUEKIND 333
#define GLUERULE 334
#define GLUECHAR 335
#define GLUESHRINKSTRETCH 336
#define GLUEORDER 337
#define OFMLEVEL 338
#define FONTDIR 339
#define ACCENT 340

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY__TEX_LIVE_TEXK_WEB_C_OMEGAFONTS_PL_PARSER_H_INCLUDED  */
