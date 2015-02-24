/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "pl-parser.y" /* yacc.c:339  */

/* pl-parser.y: Grammar for reading property list files

This file is part of Omega,
which is based on the web2c distribution of TeX,

Copyright (c) 1994--2001 John Plaice and Yannis Haralambous

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

#include "cpascal.h"
#include "parser.h"
#include "pl-parser.h"
#include "manifests.h"
#include "list_routines.h"
#include "error_routines.h"
#include "out_routines.h"
#include "char_routines.h"
#include "header_routines.h"
#include "param_routines.h"
#include "dimen_routines.h"
#include "ligkern_routines.h"
#include "print_routines.h"
#include "font_routines.h"
#include "extra_routines.h"


#line 108 "pl-parser.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
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

/* Copy the second part of user declarations.  */

#line 329 "pl-parser.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   221

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  59
/* YYNRULES -- Number of rules.  */
#define YYNRULES  154
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  272

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   340

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   156,   156,   159,   161,   169,   171,   173,   175,   177,
     179,   181,   183,   186,   185,   189,   188,   191,   194,   193,
     196,   200,   203,   202,   208,   210,   213,   212,   217,   216,
     220,   219,   223,   222,   226,   225,   229,   228,   232,   231,
     236,   238,   243,   245,   247,   250,   252,   257,   259,   261,
     263,   265,   267,   269,   273,   275,   277,   279,   281,   285,
     287,   292,   294,   297,   296,   299,   304,   303,   310,   312,
     314,   316,   318,   320,   322,   327,   329,   334,   336,   342,
     344,   349,   351,   353,   355,   357,   359,   362,   364,   369,
     371,   373,   375,   377,   379,   381,   383,   389,   391,   397,
     396,   399,   402,   404,   409,   411,   414,   416,   422,   421,
     424,   427,   429,   434,   436,   439,   441,   447,   446,   449,
     452,   454,   459,   461,   464,   466,   472,   471,   474,   477,
     479,   484,   486,   489,   491,   497,   496,   499,   502,   504,
     509,   511,   514,   516,   522,   521,   524,   527,   529,   534,
     536,   538,   540,   542,   544
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LEFT", "RIGHT", "NUMBER", "FIX",
  "COMMENT", "CHECKSUM", "DESIGNSIZE", "DESIGNUNITS", "CODINGSCHEME",
  "FAMILY", "FACE", "SEVENBITSAFEFLAG", "HEADER", "FONTDIMEN", "LIGTABLE",
  "BOUNDARYCHAR", "CHARACTER", "NAMEDPARAMETER", "PARAMETER",
  "CHARMEASURE", "NEXTLARGER", "VARCHAR", "EXTEN", "LABEL", "LIG", "KRN",
  "STOP", "SKIP", "VTITLE", "MAPFONT", "FONTNAME", "FONTAREA",
  "FONTCHECKSUM", "FONTAT", "FONTDSIZE", "MAP", "SELECTFONT", "SETCHAR",
  "SETRULE", "MOVE", "PUSH", "POP", "SPECIAL", "SPECIALHEX", "CHARREPEAT",
  "FONTIVALUE", "FONTFVALUE", "FONTMVALUE", "FONTPENALTY", "FONTRULE",
  "FONTGLUE", "CLABEL", "CKRN", "CGLUE", "CPENALTY", "CPENGLUE",
  "CHARIVALUE", "CHARFVALUE", "CHARMVALUE", "CHARPENALTY", "CHARRULE",
  "CHARGLUE", "IVALUE", "IVALUEVAL", "MVALUE", "MVALUEVAL", "FVALUE",
  "FVALUEVAL", "PENALTY", "PENALTYVAL", "RULE", "RULEMEASURE", "GLUE",
  "GLUEWD", "GLUETYPE", "GLUEKIND", "GLUERULE", "GLUECHAR",
  "GLUESHRINKSTRETCH", "GLUEORDER", "OFMLEVEL", "FONTDIR", "ACCENT",
  "$accept", "File", "Entries", "OneEntry", "$@1", "$@2", "$@3", "$@4",
  "$@5", "$@6", "$@7", "$@8", "$@9", "$@10", "$@11", "FontParameters",
  "OneFontParameter", "LigKernEntries", "OneLigKernEntry",
  "CharacterEntries", "OneCharacterEntry", "$@12", "$@13",
  "VarCharParameters", "OneVarCharParameter", "MapFontEntries",
  "OneMapFontEntry", "MapEntries", "OneMapEntry", "FontIvalueEntries",
  "OneFontIvalueEntry", "$@14", "IvalueDefinition", "OneIvalueDefinition",
  "FontMvalueEntries", "OneFontMvalueEntry", "$@15", "MvalueDefinition",
  "OneMvalueDefinition", "FontFvalueEntries", "OneFontFvalueEntry", "$@16",
  "FvalueDefinition", "OneFvalueDefinition", "FontPenaltyEntries",
  "OneFontPenaltyEntry", "$@17", "PenaltyDefinition",
  "OnePenaltyDefinition", "FontRuleEntries", "OneFontRuleEntry", "$@18",
  "RuleDefinition", "OneRuleDefinition", "FontGlueEntries",
  "OneFontGlueEntry", "$@19", "GlueDefinition", "OneGlueDefinition", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340
};
# endif

#define YYPACT_NINF -8

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-8)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -8,     8,     9,    -8,    11,    -8,     4,     5,    29,    -8,
      -8,    34,    -8,    35,    -8,    -8,    36,    39,    -8,    40,
      41,    44,    46,    76,    77,    79,    81,    82,    -8,    10,
      -8,    -8,    -8,    -8,    83,    -8,    -8,    -8,    -8,    -8,
      84,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      33,    80,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    27,    72,    87,    88,    -8,    89,   101,   102,   108,
     109,   118,    -8,   116,   119,   121,    -8,    32,   126,   127,
      -8,   128,   129,   130,   131,   132,   133,   135,    -7,    73,
      87,     0,    -2,    -1,     6,     3,    -3,    -8,   117,    -8,
      -8,    -8,   136,   134,    -8,    -8,   137,   139,   140,   141,
      -8,    -8,   142,   144,    -8,    -8,   145,   146,   147,   148,
     149,   150,    -8,   138,    -8,    -8,    -8,   151,   152,   153,
     143,    -8,   155,   157,    -8,   158,   160,    -8,   161,   163,
      -8,   164,   166,    -8,   167,   169,    -8,   170,   172,    -8,
      -8,    -8,    -8,    -8,    -8,   173,    -8,    -8,    -8,    -8,
     174,   175,   176,   177,   178,   179,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,   154,   159,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    78,    74,
     162,   165,   168,   171,   182,   183,    -8,   184,   186,   187,
     188,   181,   185,    -8,    -8,    -8,    -8,   190,    31,    26,
      25,    -4,    -5,    -6,    -8,    -8,    -8,    -8,   189,    -8,
      -8,    -8,   191,   193,    -8,   192,   195,    -8,   194,   197,
      -8,   198,   200,    -8,   196,   201,    -8,   202,    99,   204,
     205,   206,   203,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,   208,    -8,   106,    -8,
      -8,    -8
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,    20,     0,     0,     0,     8,
       9,     0,    11,     0,    13,    15,     0,     0,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    25,     0,
       5,     6,     7,    10,     0,    40,    45,    17,    18,    22,
       0,    28,    32,    30,    34,    36,    38,    24,     4,    12,
      14,    16,    59,    79,    26,    97,   115,   106,   124,   133,
     142,     0,     0,    19,    23,    59,    29,    33,    31,    35,
      37,    39,    44,     0,     0,     0,    53,     0,     0,     0,
      51,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,     0,     0,     0,    42,     0,    41,
      47,    48,     0,     0,    52,    54,     0,     0,     0,     0,
      46,    65,     0,     0,    63,    66,     0,     0,     0,     0,
       0,     0,    68,     0,    86,    81,    82,     0,     0,     0,
       0,   101,     0,     0,   119,     0,     0,   110,     0,     0,
     128,     0,     0,   137,     0,     0,   146,     0,     0,    43,
      49,    50,    55,    56,    57,     0,    61,    62,    75,    87,
       0,     0,     0,     0,     0,     0,    60,    83,    84,    85,
      80,    99,    98,   117,   116,   108,   107,   126,   125,   135,
     134,   144,   143,    58,    64,    67,    69,    71,    70,    72,
      73,    74,   102,   120,   111,   129,   138,   147,     0,     0,
     100,   118,   109,   127,   136,   145,    78,     0,     0,     0,
       0,     0,     0,    93,    94,    95,    96,     0,     0,     0,
       0,     0,     0,     0,    77,    76,    89,    90,     0,    92,
      88,   105,     0,     0,   123,     0,     0,   114,     0,     0,
     132,     0,     0,   141,     0,     0,   154,     0,     0,     0,
       0,     0,     0,    91,   104,   103,   122,   121,   113,   112,
     131,   130,   140,   139,   150,   149,     0,   151,     0,   148,
     152,   153
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,   156,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,
      -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8,    -8
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    29,    35,    36,    52,    53,    65,    55,
      57,    56,    58,    59,    60,    50,    75,    51,    87,    63,
     123,   158,   159,   184,   208,    64,   130,   185,   217,    66,
     133,   192,   200,   233,    68,   139,   194,   202,   239,    67,
     136,   193,   201,   236,    69,   142,   195,   203,   242,    70,
     145,   196,   204,   245,    71,   148,   197,   205,   252
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint16 yytable[] =
{
     111,   246,   243,   240,   146,   134,   137,   131,     3,    30,
     143,    31,     4,   140,    48,   112,   113,   114,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,   115,   237,   234,    72,    32,    61,   100,   231,    33,
      34,    37,    18,    19,    38,    39,    40,    73,    74,    41,
     101,    42,   116,   117,   118,   119,   120,   121,    20,    21,
      22,    23,    24,    25,    26,   132,   138,   135,   241,   244,
     247,   248,   147,   249,   250,   251,   144,   141,   122,    76,
     124,    43,    44,    62,    45,   206,    46,    47,    49,    54,
      88,    89,    91,   238,    27,    28,   235,   232,    77,    78,
      79,    80,    81,   207,    92,    93,   125,   126,   127,   128,
     129,    94,    95,   209,   210,   211,   212,   213,   214,   215,
     216,    96,    97,   149,    98,    99,    82,    83,    84,    85,
      86,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     151,   150,   166,   152,   153,   154,   155,   170,   156,   157,
     160,   161,   162,   163,   164,   165,   167,   198,   168,   169,
     171,   172,   199,   173,   174,   218,   175,   176,   219,   177,
     178,   220,   179,   180,   221,   181,   182,   265,   183,   186,
     187,   188,   189,   190,   191,   222,   223,   228,   271,   224,
     225,   229,   226,   227,   230,   253,   254,   255,   256,   257,
     258,   259,   262,   260,   261,   263,     0,   269,   264,   266,
     267,     0,   268,   270,     0,     0,     0,     0,     0,     0,
       0,    90
};

static const yytype_int8 yycheck[] =
{
       7,     7,     7,     7,     7,     7,     7,     7,     0,     5,
       7,     6,     3,     7,     4,    22,    23,    24,     7,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    38,     7,     7,     7,     6,     3,     5,     7,     5,
       5,     5,    31,    32,     5,     5,     5,    20,    21,     5,
      18,     5,    59,    60,    61,    62,    63,    64,    47,    48,
      49,    50,    51,    52,    53,    65,    67,    69,    72,    74,
      76,    77,    75,    79,    80,    81,    73,    71,    85,     7,
       7,     5,     5,     3,     5,     7,     5,     5,     5,     5,
       3,     3,     3,    68,    83,    84,    70,    66,    26,    27,
      28,    29,    30,    25,     3,     3,    33,    34,    35,    36,
      37,     3,     3,    39,    40,    41,    42,    43,    44,    45,
      46,     3,     6,     6,     5,     4,    54,    55,    56,    57,
      58,     5,     5,     5,     5,     5,     5,     5,     5,     4,
       6,     5,     4,     6,     5,     5,     5,     4,     6,     5,
       5,     5,     5,     5,     5,     5,     5,     3,     6,     6,
       5,     4,     3,     5,     4,     3,     5,     4,     3,     5,
       4,     3,     5,     4,     3,     5,     4,    78,     5,     5,
       5,     5,     5,     5,     5,     3,     3,     6,    82,     5,
       4,     6,     5,     5,     4,     6,     5,     4,     6,     4,
       6,     4,     6,     5,     4,     4,    -1,     4,     6,     5,
       5,    -1,     6,     5,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    65
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    87,    88,     0,     3,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    31,    32,
      47,    48,    49,    50,    51,    52,    53,    83,    84,    89,
       5,     6,     6,     5,     5,    90,    91,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     4,     5,
     101,   103,    92,    93,     5,    95,    97,    96,    98,    99,
     100,     3,     3,   105,   111,    94,   115,   125,   120,   130,
     135,   140,     7,    20,    21,   102,     7,    26,    27,    28,
      29,    30,    54,    55,    56,    57,    58,   104,     3,     3,
     105,     3,     3,     3,     3,     3,     3,     6,     5,     4,
       5,    18,     5,     5,     5,     5,     5,     5,     5,     5,
       4,     7,    22,    23,    24,    38,    59,    60,    61,    62,
      63,    64,    85,   106,     7,    33,    34,    35,    36,    37,
     112,     7,    65,   116,     7,    69,   126,     7,    67,   121,
       7,    71,   131,     7,    73,   136,     7,    75,   141,     6,
       5,     6,     6,     5,     5,     5,     6,     5,   107,   108,
       5,     5,     5,     5,     5,     5,     4,     5,     6,     6,
       4,     5,     4,     5,     4,     5,     4,     5,     4,     5,
       4,     5,     4,     5,   109,   113,     5,     5,     5,     5,
       5,     5,   117,   127,   122,   132,   137,   142,     3,     3,
     118,   128,   123,   133,   138,   143,     7,    25,   110,    39,
      40,    41,    42,    43,    44,    45,    46,   114,     3,     3,
       3,     3,     3,     3,     5,     4,     5,     5,     6,     6,
       4,     7,    66,   119,     7,    70,   129,     7,    68,   124,
       7,    72,   134,     7,    74,   139,     7,    76,    77,    79,
      80,    81,   144,     6,     5,     4,     6,     4,     6,     4,
       5,     4,     6,     4,     6,    78,     5,     5,     6,     4,
       5,    82
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    87,    88,    88,    89,    89,    89,    89,    89,
      89,    89,    89,    90,    89,    91,    89,    89,    92,    89,
      89,    89,    93,    89,    89,    89,    94,    89,    95,    89,
      96,    89,    97,    89,    98,    89,    99,    89,   100,    89,
     101,   101,   102,   102,   102,   103,   103,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   105,
     105,   106,   106,   107,   106,   106,   108,   106,   106,   106,
     106,   106,   106,   106,   106,   109,   109,   110,   110,   111,
     111,   112,   112,   112,   112,   112,   112,   113,   113,   114,
     114,   114,   114,   114,   114,   114,   114,   115,   115,   117,
     116,   116,   118,   118,   119,   119,   120,   120,   122,   121,
     121,   123,   123,   124,   124,   125,   125,   127,   126,   126,
     128,   128,   129,   129,   130,   130,   132,   131,   131,   133,
     133,   134,   134,   135,   135,   137,   136,   136,   138,   138,
     139,   139,   140,   140,   142,   141,   141,   143,   143,   144,
     144,   144,   144,   144,   144
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     4,     2,     2,     2,     1,     1,
       2,     1,     3,     0,     3,     0,     3,     2,     0,     4,
       1,     1,     0,     4,     2,     1,     0,     5,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     2,     3,     1,     0,     4,     2,     2,     3,
       3,     1,     2,     1,     2,     3,     3,     3,     4,     0,
       4,     2,     2,     0,     3,     1,     0,     3,     1,     3,
       3,     3,     3,     3,     3,     0,     4,     2,     1,     0,
       4,     1,     1,     2,     2,     2,     1,     0,     4,     2,
       2,     3,     2,     1,     1,     1,     1,     0,     4,     0,
       4,     1,     0,     4,     2,     1,     0,     4,     0,     4,
       1,     0,     4,     2,     1,     0,     4,     0,     4,     1,
       0,     4,     2,     1,     0,     4,     0,     4,     1,     0,
       4,     2,     1,     0,     4,     0,     4,     1,     0,     4,
       2,     1,     0,     4,     0,     4,     1,     0,     4,     2,
       2,     2,     3,     3,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 5:
#line 170 "pl-parser.y" /* yacc.c:1646  */
    { set_check_sum((yyvsp[0]).yint); }
#line 1618 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 6:
#line 172 "pl-parser.y" /* yacc.c:1646  */
    { set_design_size((yyvsp[0]).yfix); }
#line 1624 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 7:
#line 174 "pl-parser.y" /* yacc.c:1646  */
    { set_design_units((yyvsp[0]).yfix); }
#line 1630 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 8:
#line 176 "pl-parser.y" /* yacc.c:1646  */
    { set_coding_scheme((yyvsp[0]).ystring); }
#line 1636 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 9:
#line 178 "pl-parser.y" /* yacc.c:1646  */
    { set_family((yyvsp[0]).ystring); }
#line 1642 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 10:
#line 180 "pl-parser.y" /* yacc.c:1646  */
    { set_face((yyvsp[0]).yint); }
#line 1648 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 11:
#line 182 "pl-parser.y" /* yacc.c:1646  */
    { set_seven_bit_safe_flag((yyvsp[0]).yint); }
#line 1654 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 12:
#line 184 "pl-parser.y" /* yacc.c:1646  */
    { set_header_word((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1660 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 13:
#line 186 "pl-parser.y" /* yacc.c:1646  */
    { init_parameters(); }
#line 1666 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 15:
#line 189 "pl-parser.y" /* yacc.c:1646  */
    { init_ligkern(); }
#line 1672 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 17:
#line 192 "pl-parser.y" /* yacc.c:1646  */
    { set_boundary_character((yyvsp[0]).yint); }
#line 1678 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 18:
#line 194 "pl-parser.y" /* yacc.c:1646  */
    { init_character((yyvsp[0]).yint, NULL); }
#line 1684 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 21:
#line 201 "pl-parser.y" /* yacc.c:1646  */
    { set_vtitle((yyvsp[0]).ystring); }
#line 1690 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 22:
#line 203 "pl-parser.y" /* yacc.c:1646  */
    { init_map_font((yyvsp[0]).yint); }
#line 1696 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 24:
#line 209 "pl-parser.y" /* yacc.c:1646  */
    { set_ofm_level((yyvsp[0]).yint); }
#line 1702 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 25:
#line 211 "pl-parser.y" /* yacc.c:1646  */
    { set_font_dir((yyvsp[0]).yint); }
#line 1708 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 26:
#line 213 "pl-parser.y" /* yacc.c:1646  */
    { init_character((yyvsp[-1]).yint,NULL); }
#line 1714 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 27:
#line 215 "pl-parser.y" /* yacc.c:1646  */
    { copy_characters((yyvsp[-3]).yint,(yyvsp[-2]).yint); }
#line 1720 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 217 "pl-parser.y" /* yacc.c:1646  */
    {init_font_ivalue((yyvsp[0]).yint);}
#line 1726 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 30:
#line 220 "pl-parser.y" /* yacc.c:1646  */
    { init_font_mvalue((yyvsp[0]).yint);}
#line 1732 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 223 "pl-parser.y" /* yacc.c:1646  */
    { init_font_fvalue((yyvsp[0]).yint);}
#line 1738 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 34:
#line 226 "pl-parser.y" /* yacc.c:1646  */
    { init_font_penalty((yyvsp[0]).yint);}
#line 1744 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 36:
#line 229 "pl-parser.y" /* yacc.c:1646  */
    { init_font_rule((yyvsp[0]).yint);}
#line 1750 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 38:
#line 232 "pl-parser.y" /* yacc.c:1646  */
    { init_font_glue((yyvsp[0]).yint);}
#line 1756 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 42:
#line 244 "pl-parser.y" /* yacc.c:1646  */
    { set_param_word((yyvsp[-1]).yint, (yyvsp[0]).yfix); }
#line 1762 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 43:
#line 246 "pl-parser.y" /* yacc.c:1646  */
    { set_param_word((yyvsp[-1]).yint, (yyvsp[0]).yfix); }
#line 1768 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 47:
#line 258 "pl-parser.y" /* yacc.c:1646  */
    { set_label_command((yyvsp[0]).yint); }
#line 1774 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 48:
#line 260 "pl-parser.y" /* yacc.c:1646  */
    { set_label_command(CHAR_BOUNDARY); }
#line 1780 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 49:
#line 262 "pl-parser.y" /* yacc.c:1646  */
    { set_ligature_command((yyvsp[-2]).yint, (yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1786 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 50:
#line 264 "pl-parser.y" /* yacc.c:1646  */
    { set_kerning_command((yyvsp[-1]).yint, (yyvsp[0]).yfix); }
#line 1792 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 51:
#line 266 "pl-parser.y" /* yacc.c:1646  */
    { set_stop_command(); }
#line 1798 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 52:
#line 268 "pl-parser.y" /* yacc.c:1646  */
    { set_skip_command((yyvsp[0]).yint); }
#line 1804 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 54:
#line 274 "pl-parser.y" /* yacc.c:1646  */
    { set_c_label_command((yyvsp[0]).yint); }
#line 1810 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 276 "pl-parser.y" /* yacc.c:1646  */
    { set_c_kerning_command((yyvsp[-1]).yint, (yyvsp[0]).yfix); }
#line 1816 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 56:
#line 278 "pl-parser.y" /* yacc.c:1646  */
    { set_c_glue_command((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1822 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 57:
#line 280 "pl-parser.y" /* yacc.c:1646  */
    { set_c_penalty_command((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1828 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 58:
#line 282 "pl-parser.y" /* yacc.c:1646  */
    { set_c_penglue_command((yyvsp[-2]).yint, (yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1834 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 61:
#line 293 "pl-parser.y" /* yacc.c:1646  */
    { set_character_measure((yyvsp[-1]).yint, (yyvsp[0]).yfix); }
#line 1840 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 295 "pl-parser.y" /* yacc.c:1646  */
    { set_next_larger((yyvsp[0]).yint); }
#line 1846 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 63:
#line 297 "pl-parser.y" /* yacc.c:1646  */
    { init_var_character(); }
#line 1852 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 304 "pl-parser.y" /* yacc.c:1646  */
    { init_map(); }
#line 1858 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 67:
#line 306 "pl-parser.y" /* yacc.c:1646  */
    { end_map(); }
#line 1864 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 311 "pl-parser.y" /* yacc.c:1646  */
    { set_accent((yyvsp[0]).yint); }
#line 1870 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 69:
#line 313 "pl-parser.y" /* yacc.c:1646  */
    { set_character_ivalue((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1876 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 70:
#line 315 "pl-parser.y" /* yacc.c:1646  */
    { set_character_mvalue((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1882 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 71:
#line 317 "pl-parser.y" /* yacc.c:1646  */
    { set_character_fvalue((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1888 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 319 "pl-parser.y" /* yacc.c:1646  */
    { set_character_penalty((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1894 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 73:
#line 321 "pl-parser.y" /* yacc.c:1646  */
    { set_character_rule((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1900 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 74:
#line 323 "pl-parser.y" /* yacc.c:1646  */
    { set_character_glue((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1906 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 77:
#line 335 "pl-parser.y" /* yacc.c:1646  */
    { set_extensible_piece((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 1912 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 81:
#line 350 "pl-parser.y" /* yacc.c:1646  */
    { set_font_name((yyvsp[0]).ystring); }
#line 1918 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 82:
#line 352 "pl-parser.y" /* yacc.c:1646  */
    { set_font_area((yyvsp[0]).ystring); }
#line 1924 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 83:
#line 354 "pl-parser.y" /* yacc.c:1646  */
    { set_font_check_sum((yyvsp[0]).yint); }
#line 1930 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 84:
#line 356 "pl-parser.y" /* yacc.c:1646  */
    { set_font_at((yyvsp[0]).yfix); }
#line 1936 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 85:
#line 358 "pl-parser.y" /* yacc.c:1646  */
    { set_font_design_size((yyvsp[0]).yfix); }
#line 1942 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 89:
#line 370 "pl-parser.y" /* yacc.c:1646  */
    { set_select_font((yyvsp[0]).yint); }
#line 1948 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 90:
#line 372 "pl-parser.y" /* yacc.c:1646  */
    { set_set_char((yyvsp[0]).yint); }
#line 1954 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 91:
#line 374 "pl-parser.y" /* yacc.c:1646  */
    { set_set_rule((yyvsp[-1]).yfix, (yyvsp[0]).yfix); }
#line 1960 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 92:
#line 376 "pl-parser.y" /* yacc.c:1646  */
    { set_move((yyvsp[-1]).yint, (yyvsp[0]).yfix); }
#line 1966 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 93:
#line 378 "pl-parser.y" /* yacc.c:1646  */
    { set_push(); }
#line 1972 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 94:
#line 380 "pl-parser.y" /* yacc.c:1646  */
    { set_pop(); }
#line 1978 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 95:
#line 382 "pl-parser.y" /* yacc.c:1646  */
    { set_special((yyvsp[0]).ystring); }
#line 1984 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 96:
#line 384 "pl-parser.y" /* yacc.c:1646  */
    { set_special_hex((yyvsp[0]).ystring); }
#line 1990 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 99:
#line 397 "pl-parser.y" /* yacc.c:1646  */
    { init_font_ivalue_entry((yyvsp[0]).yint); }
#line 1996 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 104:
#line 410 "pl-parser.y" /* yacc.c:1646  */
    { set_font_ivalue_definition((yyvsp[0]).yint); }
#line 2002 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 108:
#line 422 "pl-parser.y" /* yacc.c:1646  */
    { init_font_mvalue_entry((yyvsp[0]).yint); }
#line 2008 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 113:
#line 435 "pl-parser.y" /* yacc.c:1646  */
    { set_font_mvalue_definition((yyvsp[0]).yfix); }
#line 2014 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 117:
#line 447 "pl-parser.y" /* yacc.c:1646  */
    { init_font_fvalue_entry((yyvsp[0]).yint); }
#line 2020 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 122:
#line 460 "pl-parser.y" /* yacc.c:1646  */
    { set_font_fvalue_definition((yyvsp[0]).yfix); }
#line 2026 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 126:
#line 472 "pl-parser.y" /* yacc.c:1646  */
    { init_font_penalty_entry((yyvsp[0]).yint); }
#line 2032 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 131:
#line 485 "pl-parser.y" /* yacc.c:1646  */
    { set_font_penalty_definition((yyvsp[0]).yint); }
#line 2038 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 135:
#line 497 "pl-parser.y" /* yacc.c:1646  */
    { init_font_rule_entry((yyvsp[0]).yint); }
#line 2044 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 140:
#line 510 "pl-parser.y" /* yacc.c:1646  */
    { set_font_rule_measure((yyvsp[-1]).yint, (yyvsp[0]).yfix); }
#line 2050 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 144:
#line 522 "pl-parser.y" /* yacc.c:1646  */
    { init_font_glue_entry((yyvsp[0]).yint); }
#line 2056 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 149:
#line 535 "pl-parser.y" /* yacc.c:1646  */
    { set_font_glue_type((yyvsp[0]).yint); }
#line 2062 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 150:
#line 537 "pl-parser.y" /* yacc.c:1646  */
    { set_font_glue_width((yyvsp[0]).yfix); }
#line 2068 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 151:
#line 539 "pl-parser.y" /* yacc.c:1646  */
    { set_font_glue_character((yyvsp[0]).yint); }
#line 2074 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 152:
#line 541 "pl-parser.y" /* yacc.c:1646  */
    { set_font_glue_rule((yyvsp[-1]).yint, (yyvsp[0]).yint); }
#line 2080 "pl-parser.c" /* yacc.c:1646  */
    break;

  case 153:
#line 543 "pl-parser.y" /* yacc.c:1646  */
    { set_font_glue_shrink_stretch((yyvsp[-2]).yint, (yyvsp[-1]).yfix, (yyvsp[0]).yint); }
#line 2086 "pl-parser.c" /* yacc.c:1646  */
    break;


#line 2090 "pl-parser.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
