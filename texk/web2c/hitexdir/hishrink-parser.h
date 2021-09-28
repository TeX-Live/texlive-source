/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY__TEXK_WEB_C_HITEXDIR_HISHRINK_PARSER_H_INCLUDED
# define YY_YY__TEXK_WEB_C_HITEXDIR_HISHRINK_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    START = 258,                   /* "<"  */
    END = 259,                     /* ">"  */
    GLYPH = 260,                   /* "glyph"  */
    UNSIGNED = 261,                /* UNSIGNED  */
    REFERENCE = 262,               /* REFERENCE  */
    SIGNED = 263,                  /* SIGNED  */
    STRING = 264,                  /* STRING  */
    CHARCODE = 265,                /* CHARCODE  */
    FPNUM = 266,                   /* FPNUM  */
    DIMEN = 267,                   /* "dimen"  */
    PT = 268,                      /* "pt"  */
    MM = 269,                      /* "mm"  */
    INCH = 270,                    /* "in"  */
    XDIMEN = 271,                  /* "xdimen"  */
    H = 272,                       /* "h"  */
    V = 273,                       /* "v"  */
    FIL = 274,                     /* "fil"  */
    FILL = 275,                    /* "fill"  */
    FILLL = 276,                   /* "filll"  */
    PENALTY = 277,                 /* "penalty"  */
    INTEGER = 278,                 /* "int"  */
    LANGUAGE = 279,                /* "language"  */
    RULE = 280,                    /* "rule"  */
    RUNNING = 281,                 /* "|"  */
    KERN = 282,                    /* "kern"  */
    EXPLICIT = 283,                /* "!"  */
    GLUE = 284,                    /* "glue"  */
    PLUS = 285,                    /* "plus"  */
    MINUS = 286,                   /* "minus"  */
    TXT_START = 287,               /* TXT_START  */
    TXT_END = 288,                 /* TXT_END  */
    TXT_IGNORE = 289,              /* TXT_IGNORE  */
    TXT_FONT_GLUE = 290,           /* TXT_FONT_GLUE  */
    TXT_FONT_HYPHEN = 291,         /* TXT_FONT_HYPHEN  */
    TXT_FONT = 292,                /* TXT_FONT  */
    TXT_LOCAL = 293,               /* TXT_LOCAL  */
    TXT_GLOBAL = 294,              /* TXT_GLOBAL  */
    TXT_CC = 295,                  /* TXT_CC  */
    HBOX = 296,                    /* "hbox"  */
    VBOX = 297,                    /* "vbox"  */
    SHIFTED = 298,                 /* "shifted"  */
    HPACK = 299,                   /* "hpack"  */
    HSET = 300,                    /* "hset"  */
    VPACK = 301,                   /* "vpack"  */
    VSET = 302,                    /* "vset"  */
    DEPTH = 303,                   /* "depth"  */
    ADD = 304,                     /* "add"  */
    TO = 305,                      /* "to"  */
    LEADERS = 306,                 /* "leaders"  */
    ALIGN = 307,                   /* "align"  */
    CENTER = 308,                  /* "center"  */
    EXPAND = 309,                  /* "expand"  */
    BASELINE = 310,                /* "baseline"  */
    LIGATURE = 311,                /* "ligature"  */
    DISC = 312,                    /* "disc"  */
    PAR = 313,                     /* "par"  */
    MATH = 314,                    /* "math"  */
    ON = 315,                      /* "on"  */
    OFF = 316,                     /* "off"  */
    ADJUST = 317,                  /* "adjust"  */
    TABLE = 318,                   /* "table"  */
    ITEM = 319,                    /* "item"  */
    IMAGE = 320,                   /* "image"  */
    LABEL = 321,                   /* "label"  */
    BOT = 322,                     /* "bot"  */
    MID = 323,                     /* "mid"  */
    LINK = 324,                    /* "link"  */
    OUTLINE = 325,                 /* "outline"  */
    STREAM = 326,                  /* "stream"  */
    STREAMDEF = 327,               /* "stream (definition)"  */
    FIRST = 328,                   /* "first"  */
    LAST = 329,                    /* "last"  */
    TOP = 330,                     /* "top"  */
    NOREFERENCE = 331,             /* "*"  */
    PAGE = 332,                    /* "page"  */
    RANGE = 333,                   /* "range"  */
    DIRECTORY = 334,               /* "directory"  */
    SECTION = 335,                 /* "entry"  */
    DEFINITIONS = 336,             /* "definitions"  */
    MAX = 337,                     /* "max"  */
    PARAM = 338,                   /* "param"  */
    FONT = 339,                    /* "font"  */
    CONTENT = 340                  /* "content"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define START 258
#define END 259
#define GLYPH 260
#define UNSIGNED 261
#define REFERENCE 262
#define SIGNED 263
#define STRING 264
#define CHARCODE 265
#define FPNUM 266
#define DIMEN 267
#define PT 268
#define MM 269
#define INCH 270
#define XDIMEN 271
#define H 272
#define V 273
#define FIL 274
#define FILL 275
#define FILLL 276
#define PENALTY 277
#define INTEGER 278
#define LANGUAGE 279
#define RULE 280
#define RUNNING 281
#define KERN 282
#define EXPLICIT 283
#define GLUE 284
#define PLUS 285
#define MINUS 286
#define TXT_START 287
#define TXT_END 288
#define TXT_IGNORE 289
#define TXT_FONT_GLUE 290
#define TXT_FONT_HYPHEN 291
#define TXT_FONT 292
#define TXT_LOCAL 293
#define TXT_GLOBAL 294
#define TXT_CC 295
#define HBOX 296
#define VBOX 297
#define SHIFTED 298
#define HPACK 299
#define HSET 300
#define VPACK 301
#define VSET 302
#define DEPTH 303
#define ADD 304
#define TO 305
#define LEADERS 306
#define ALIGN 307
#define CENTER 308
#define EXPAND 309
#define BASELINE 310
#define LIGATURE 311
#define DISC 312
#define PAR 313
#define MATH 314
#define ON 315
#define OFF 316
#define ADJUST 317
#define TABLE 318
#define ITEM 319
#define IMAGE 320
#define LABEL 321
#define BOT 322
#define MID 323
#define LINK 324
#define OUTLINE 325
#define STREAM 326
#define STREAMDEF 327
#define FIRST 328
#define LAST 329
#define TOP 330
#define NOREFERENCE 331
#define PAGE 332
#define RANGE 333
#define DIRECTORY 334
#define SECTION 335
#define DEFINITIONS 336
#define MAX 337
#define PARAM 338
#define FONT 339
#define CONTENT 340

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 78 "../../../texk/web2c/hitexdir/hishrink-parser.y"
uint32_t u;int32_t i;char*s;float64_t f;glyph_t c;
dimen_t d;stretch_t st;xdimen_t xd;kern_t kt;
rule_t r;glue_t g;image_t x;
list_t l;box_t h;disc_t dc;lig_t lg;
ref_t rf;info_t info;order_t o;bool b;

#line 244 "../../../texk/web2c/hitexdir/hishrink-parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY__TEXK_WEB_C_HITEXDIR_HISHRINK_PARSER_H_INCLUDED  */
