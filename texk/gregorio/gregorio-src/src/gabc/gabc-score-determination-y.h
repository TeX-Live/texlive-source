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

#ifndef YY_GABC_SCORE_DETERMINATION_GABC_GABC_SCORE_DETERMINATION_Y_H_INCLUDED
# define YY_GABC_SCORE_DETERMINATION_GABC_GABC_SCORE_DETERMINATION_Y_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gabc_score_determination_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NAME = 258,
    AUTHOR = 259,
    GABC_COPYRIGHT = 260,
    SCORE_COPYRIGHT = 261,
    NUMBER_OF_VOICES = 262,
    LANGUAGE = 263,
    STAFF_LINES = 264,
    ORISCUS_ORIENTATION = 265,
    DEF_MACRO = 266,
    OTHER_HEADER = 267,
    ANNOTATION = 268,
    MODE = 269,
    MODE_MODIFIER = 270,
    MODE_DIFFERENTIA = 271,
    INITIAL_STYLE = 272,
    VOICE_CUT = 273,
    VOICE_CHANGE = 274,
    END_OF_DEFINITIONS = 275,
    END_OF_FILE = 276,
    COLON = 277,
    SEMICOLON = 278,
    SPACE = 279,
    CHARACTERS = 280,
    NOTES = 281,
    HYPHEN = 282,
    ATTRIBUTE = 283,
    OPENING_BRACKET = 284,
    CLOSING_BRACKET = 285,
    CLOSING_BRACKET_WITH_SPACE = 286,
    I_BEGINNING = 287,
    I_END = 288,
    TT_BEGINNING = 289,
    TT_END = 290,
    UL_BEGINNING = 291,
    UL_END = 292,
    C_BEGINNING = 293,
    C_END = 294,
    B_BEGINNING = 295,
    B_END = 296,
    SC_BEGINNING = 297,
    SC_END = 298,
    SP_BEGINNING = 299,
    SP_END = 300,
    VERB_BEGINNING = 301,
    VERB = 302,
    VERB_END = 303,
    CENTER_BEGINNING = 304,
    CENTER_END = 305,
    TRANSLATION_BEGINNING = 306,
    TRANSLATION_END = 307,
    TRANSLATION_CENTER_END = 308,
    ALT_BEGIN = 309,
    ALT_END = 310,
    BNLBA = 311,
    ENLBA = 312,
    EUOUAE_B = 313,
    EUOUAE_E = 314,
    NABC_CUT = 315,
    NABC_LINES = 316
  };
#endif
/* Tokens.  */
#define NAME 258
#define AUTHOR 259
#define GABC_COPYRIGHT 260
#define SCORE_COPYRIGHT 261
#define NUMBER_OF_VOICES 262
#define LANGUAGE 263
#define STAFF_LINES 264
#define ORISCUS_ORIENTATION 265
#define DEF_MACRO 266
#define OTHER_HEADER 267
#define ANNOTATION 268
#define MODE 269
#define MODE_MODIFIER 270
#define MODE_DIFFERENTIA 271
#define INITIAL_STYLE 272
#define VOICE_CUT 273
#define VOICE_CHANGE 274
#define END_OF_DEFINITIONS 275
#define END_OF_FILE 276
#define COLON 277
#define SEMICOLON 278
#define SPACE 279
#define CHARACTERS 280
#define NOTES 281
#define HYPHEN 282
#define ATTRIBUTE 283
#define OPENING_BRACKET 284
#define CLOSING_BRACKET 285
#define CLOSING_BRACKET_WITH_SPACE 286
#define I_BEGINNING 287
#define I_END 288
#define TT_BEGINNING 289
#define TT_END 290
#define UL_BEGINNING 291
#define UL_END 292
#define C_BEGINNING 293
#define C_END 294
#define B_BEGINNING 295
#define B_END 296
#define SC_BEGINNING 297
#define SC_END 298
#define SP_BEGINNING 299
#define SP_END 300
#define VERB_BEGINNING 301
#define VERB 302
#define VERB_END 303
#define CENTER_BEGINNING 304
#define CENTER_END 305
#define TRANSLATION_BEGINNING 306
#define TRANSLATION_END 307
#define TRANSLATION_CENTER_END 308
#define ALT_BEGIN 309
#define ALT_END 310
#define BNLBA 311
#define ENLBA 312
#define EUOUAE_B 313
#define EUOUAE_E 314
#define NABC_CUT 315
#define NABC_LINES 316

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE gabc_score_determination_lval;
extern YYLTYPE gabc_score_determination_lloc;
int gabc_score_determination_parse (void);

#endif /* !YY_GABC_SCORE_DETERMINATION_GABC_GABC_SCORE_DETERMINATION_Y_H_INCLUDED  */
