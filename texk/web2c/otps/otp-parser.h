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

#ifndef YY_YY__TEX_LIVE_TEXK_WEB_C_OTPS_OTP_PARSER_H_INCLUDED
# define YY_YY__TEX_LIVE_TEXK_WEB_C_OTPS_OTP_PARSER_H_INCLUDED
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
    NUMBER = 258,
    ID = 259,
    STRING = 260,
    LEFTARROW = 261,
    RIGHTARROW = 262,
    MYINPUT = 263,
    OUTPUT = 264,
    ALIASES = 265,
    STATES = 266,
    TABLES = 267,
    EXPRESSIONS = 268,
    PUSH = 269,
    POP = 270,
    DIV = 271,
    MOD = 272,
    BEG = 273,
    END = 274
  };
#endif
/* Tokens.  */
#define NUMBER 258
#define ID 259
#define STRING 260
#define LEFTARROW 261
#define RIGHTARROW 262
#define MYINPUT 263
#define OUTPUT 264
#define ALIASES 265
#define STATES 266
#define TABLES 267
#define EXPRESSIONS 268
#define PUSH 269
#define POP 270
#define DIV 271
#define MOD 272
#define BEG 273
#define END 274

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY__TEX_LIVE_TEXK_WEB_C_OTPS_OTP_PARSER_H_INCLUDED  */
