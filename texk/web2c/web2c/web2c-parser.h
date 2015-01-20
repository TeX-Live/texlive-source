/* A Bison parser, made by GNU Bison 3.0.3.  */

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

#ifndef YY_YY__TEX_LIVE_TEXK_WEB_C_WEB_C_WEB_C_PARSER_H_INCLUDED
# define YY_YY__TEX_LIVE_TEXK_WEB_C_WEB_C_WEB_C_PARSER_H_INCLUDED
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
    array_tok = 258,
    begin_tok = 259,
    case_tok = 260,
    const_tok = 261,
    do_tok = 262,
    downto_tok = 263,
    else_tok = 264,
    end_tok = 265,
    file_tok = 266,
    for_tok = 267,
    function_tok = 268,
    goto_tok = 269,
    if_tok = 270,
    label_tok = 271,
    of_tok = 272,
    procedure_tok = 273,
    program_tok = 274,
    record_tok = 275,
    repeat_tok = 276,
    then_tok = 277,
    to_tok = 278,
    type_tok = 279,
    until_tok = 280,
    var_tok = 281,
    while_tok = 282,
    noreturn_tok = 283,
    others_tok = 284,
    r_num_tok = 285,
    i_num_tok = 286,
    string_literal_tok = 287,
    single_char_tok = 288,
    assign_tok = 289,
    two_dots_tok = 290,
    undef_id_tok = 291,
    var_id_tok = 292,
    proc_id_tok = 293,
    proc_param_tok = 294,
    fun_id_tok = 295,
    fun_param_tok = 296,
    const_id_tok = 297,
    type_id_tok = 298,
    hhb0_tok = 299,
    hhb1_tok = 300,
    field_id_tok = 301,
    define_tok = 302,
    field_tok = 303,
    break_tok = 304,
    not_eq_tok = 305,
    less_eq_tok = 306,
    great_eq_tok = 307,
    or_tok = 308,
    unary_plus_tok = 309,
    unary_minus_tok = 310,
    div_tok = 311,
    mod_tok = 312,
    and_tok = 313,
    not_tok = 314
  };
#endif
/* Tokens.  */
#define array_tok 258
#define begin_tok 259
#define case_tok 260
#define const_tok 261
#define do_tok 262
#define downto_tok 263
#define else_tok 264
#define end_tok 265
#define file_tok 266
#define for_tok 267
#define function_tok 268
#define goto_tok 269
#define if_tok 270
#define label_tok 271
#define of_tok 272
#define procedure_tok 273
#define program_tok 274
#define record_tok 275
#define repeat_tok 276
#define then_tok 277
#define to_tok 278
#define type_tok 279
#define until_tok 280
#define var_tok 281
#define while_tok 282
#define noreturn_tok 283
#define others_tok 284
#define r_num_tok 285
#define i_num_tok 286
#define string_literal_tok 287
#define single_char_tok 288
#define assign_tok 289
#define two_dots_tok 290
#define undef_id_tok 291
#define var_id_tok 292
#define proc_id_tok 293
#define proc_param_tok 294
#define fun_id_tok 295
#define fun_param_tok 296
#define const_id_tok 297
#define type_id_tok 298
#define hhb0_tok 299
#define hhb1_tok 300
#define field_id_tok 301
#define define_tok 302
#define field_tok 303
#define break_tok 304
#define not_eq_tok 305
#define less_eq_tok 306
#define great_eq_tok 307
#define or_tok 308
#define unary_plus_tok 309
#define unary_minus_tok 310
#define div_tok 311
#define mod_tok 312
#define and_tok 313
#define not_tok 314

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY__TEX_LIVE_TEXK_WEB_C_WEB_C_WEB_C_PARSER_H_INCLUDED  */
