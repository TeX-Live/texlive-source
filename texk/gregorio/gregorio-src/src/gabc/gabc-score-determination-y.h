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
    ATTRIBUTE = 258,
    COLON = 259,
    SEMICOLON = 260,
    OFFICE_PART = 261,
    ANNOTATION = 262,
    AUTHOR = 263,
    DATE = 264,
    MANUSCRIPT = 265,
    MANUSCRIPT_REFERENCE = 266,
    MANUSCRIPT_STORAGE_PLACE = 267,
    TRANSCRIBER = 268,
    TRANSCRIPTION_DATE = 269,
    BOOK = 270,
    STYLE = 271,
    VIRGULA_POSITION = 272,
    INITIAL_STYLE = 273,
    MODE = 274,
    GREGORIOTEX_FONT = 275,
    GENERATED_BY = 276,
    NAME = 277,
    OPENING_BRACKET = 278,
    NOTES = 279,
    VOICE_CUT = 280,
    CLOSING_BRACKET = 281,
    NUMBER_OF_VOICES = 282,
    VOICE_CHANGE = 283,
    END_OF_DEFINITIONS = 284,
    SPACE = 285,
    CHARACTERS = 286,
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
    CLOSING_BRACKET_WITH_SPACE = 306,
    TRANSLATION_BEGINNING = 307,
    TRANSLATION_END = 308,
    GABC_COPYRIGHT = 309,
    SCORE_COPYRIGHT = 310,
    OCCASION = 311,
    METER = 312,
    COMMENTARY = 313,
    ARRANGER = 314,
    GABC_VERSION = 315,
    USER_NOTES = 316,
    DEF_MACRO = 317,
    ALT_BEGIN = 318,
    ALT_END = 319,
    CENTERING_SCHEME = 320,
    TRANSLATION_CENTER_END = 321,
    BNLBA = 322,
    ENLBA = 323,
    EUOUAE_B = 324,
    EUOUAE_E = 325,
    NABC_CUT = 326,
    NABC_LINES = 327,
    LANGUAGE = 328
  };
#endif
/* Tokens.  */
#define ATTRIBUTE 258
#define COLON 259
#define SEMICOLON 260
#define OFFICE_PART 261
#define ANNOTATION 262
#define AUTHOR 263
#define DATE 264
#define MANUSCRIPT 265
#define MANUSCRIPT_REFERENCE 266
#define MANUSCRIPT_STORAGE_PLACE 267
#define TRANSCRIBER 268
#define TRANSCRIPTION_DATE 269
#define BOOK 270
#define STYLE 271
#define VIRGULA_POSITION 272
#define INITIAL_STYLE 273
#define MODE 274
#define GREGORIOTEX_FONT 275
#define GENERATED_BY 276
#define NAME 277
#define OPENING_BRACKET 278
#define NOTES 279
#define VOICE_CUT 280
#define CLOSING_BRACKET 281
#define NUMBER_OF_VOICES 282
#define VOICE_CHANGE 283
#define END_OF_DEFINITIONS 284
#define SPACE 285
#define CHARACTERS 286
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
#define CLOSING_BRACKET_WITH_SPACE 306
#define TRANSLATION_BEGINNING 307
#define TRANSLATION_END 308
#define GABC_COPYRIGHT 309
#define SCORE_COPYRIGHT 310
#define OCCASION 311
#define METER 312
#define COMMENTARY 313
#define ARRANGER 314
#define GABC_VERSION 315
#define USER_NOTES 316
#define DEF_MACRO 317
#define ALT_BEGIN 318
#define ALT_END 319
#define CENTERING_SCHEME 320
#define TRANSLATION_CENTER_END 321
#define BNLBA 322
#define ENLBA 323
#define EUOUAE_B 324
#define EUOUAE_E 325
#define NABC_CUT 326
#define NABC_LINES 327
#define LANGUAGE 328

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
