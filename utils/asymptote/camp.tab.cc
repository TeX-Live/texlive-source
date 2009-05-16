/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "camp.y"

/*****
 * camp.y
 * Andy Hammerlindl 08/12/2002
 *
 * The grammar of the camp language.
 *****/

#include "errormsg.h"
#include "exp.h"
#include "newexp.h"
#include "dec.h"
#include "fundec.h"
#include "stm.h"
#include "modifier.h"

// Avoid error messages with unpatched bison-1.875:
#ifndef __attribute__
#define __attribute__(x)
#endif

// Used when a position needs to be determined and no token is
// available.  Defined in camp.l.
position lexerPos();

bool lexerEOF();

int yylex(void); /* function prototype */

void yyerror(const char *s)
{
  if (!lexerEOF()) {
    em.error(lexerPos());
    em << s;
    em.cont();
  }
}

namespace absyntax { file *root; }

using namespace absyntax;
using sym::symbol;
using mem::string;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 187 of yacc.c.  */
#line 313 "camp.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 326 "camp.tab.c"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2252

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  203
/* YYNRULES -- Number of states.  */
#define YYNSTATES  388

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    34,     2,     2,
      48,    49,    32,    29,    51,    31,    50,    33,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    42,    54,
       2,     2,     2,    40,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    52,     2,    53,    35,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,     2,    47,     2,     2,     2,     2,
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
      25,    26,    27,    28,    30,    36,    37,    38,    39,    41,
      43,    44,    46,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    10,    13,    15,    19,
      21,    23,    26,    29,    31,    33,    36,    39,    41,    43,
      45,    49,    55,    61,    65,    71,    77,    81,    85,    89,
      91,    95,    97,   101,   103,   105,   107,   111,   113,   117,
     120,   123,   125,   128,   130,   133,   137,   141,   146,   148,
     152,   154,   158,   160,   163,   167,   172,   174,   176,   180,
     183,   188,   192,   198,   200,   202,   205,   207,   211,   213,
     217,   220,   222,   226,   228,   229,   232,   236,   242,   248,
     255,   259,   262,   264,   267,   270,   274,   278,   283,   288,
     293,   298,   302,   307,   311,   316,   320,   324,   326,   328,
     332,   334,   338,   341,   343,   347,   349,   351,   353,   355,
     358,   363,   369,   372,   375,   378,   382,   386,   390,   394,
     398,   402,   406,   410,   414,   418,   422,   426,   430,   434,
     438,   442,   446,   450,   454,   457,   461,   466,   470,   475,
     481,   488,   495,   503,   509,   513,   519,   527,   541,   545,
     548,   551,   554,   557,   561,   565,   569,   573,   577,   581,
     586,   588,   590,   593,   596,   600,   605,   609,   615,   623,
     625,   629,   633,   635,   637,   640,   645,   649,   655,   658,
     663,   665,   667,   670,   676,   684,   690,   698,   708,   717,
     720,   723,   726,   730,   732,   734,   735,   737,   739,   740,
     742,   743,   745,   747
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      82,     0,    -1,    83,    -1,    -1,    83,    86,    -1,    -1,
      84,    86,    -1,     3,    -1,    85,    50,     3,    -1,    88,
      -1,   125,    -1,    87,    88,    -1,    87,   125,    -1,    78,
      -1,    77,    -1,    87,    78,    -1,    87,    77,    -1,    94,
      -1,   112,    -1,   113,    -1,    56,    93,    54,    -1,    60,
      85,    57,    90,    54,    -1,    60,    85,    57,    32,    54,
      -1,    57,    85,    54,    -1,    60,    91,    56,    90,    54,
      -1,    60,    91,    56,    32,    54,    -1,    58,    92,    54,
      -1,    59,     3,    54,    -1,    59,    38,    54,    -1,     3,
      -1,     3,     3,     3,    -1,    89,    -1,    90,    51,    89,
      -1,    38,    -1,     3,    -1,     3,    -1,    91,     3,     3,
      -1,    92,    -1,    93,    51,    92,    -1,    95,    54,    -1,
      96,   100,    -1,    97,    -1,    85,    98,    -1,    85,    -1,
      52,    53,    -1,    98,    52,    53,    -1,    52,   119,    53,
      -1,    99,    52,   119,    53,    -1,   101,    -1,   100,    51,
     101,    -1,   102,    -1,   102,    41,   103,    -1,     3,    -1,
       3,    98,    -1,     3,    48,    49,    -1,     3,    48,   108,
      49,    -1,   119,    -1,   105,    -1,    45,    84,    47,    -1,
      45,    47,    -1,    45,    55,   103,    47,    -1,    45,   106,
      47,    -1,    45,   106,    55,   103,    47,    -1,    51,    -1,
     107,    -1,   107,    51,    -1,   103,    -1,   107,    51,   103,
      -1,   109,    -1,   109,    55,   111,    -1,    55,   111,    -1,
     111,    -1,   109,    51,   111,    -1,    74,    -1,    -1,   110,
      96,    -1,   110,    96,   102,    -1,   110,    96,   102,    41,
     103,    -1,    96,     3,    48,    49,   127,    -1,    96,     3,
      48,   108,    49,   127,    -1,    62,     3,   104,    -1,    63,
      94,    -1,    42,    -1,   119,    42,    -1,    42,   119,    -1,
     119,    42,   119,    -1,   115,    50,     3,    -1,    85,    52,
     119,    53,    -1,   115,    52,   119,    53,    -1,    85,    52,
     114,    53,    -1,   115,    52,   114,    53,    -1,    85,    48,
      49,    -1,    85,    48,   117,    49,    -1,   115,    48,    49,
      -1,   115,    48,   117,    49,    -1,    48,   119,    49,    -1,
      48,    85,    49,    -1,    73,    -1,   119,    -1,     3,    41,
     119,    -1,   118,    -1,   118,    55,   116,    -1,    55,   116,
      -1,   116,    -1,   118,    51,   116,    -1,    85,    -1,   115,
      -1,    76,    -1,    38,    -1,    76,   119,    -1,    48,    85,
      49,   119,    -1,    48,    85,    98,    49,   119,    -1,    29,
     119,    -1,    31,   119,    -1,    36,   119,    -1,   119,    29,
     119,    -1,   119,    31,   119,    -1,   119,    32,   119,    -1,
     119,    33,   119,    -1,   119,    34,   119,    -1,   119,    35,
     119,    -1,   119,    25,   119,    -1,   119,    26,   119,    -1,
     119,    27,   119,    -1,   119,    28,   119,    -1,   119,    23,
     119,    -1,   119,    24,   119,    -1,   119,    20,   119,    -1,
     119,    19,   119,    -1,   119,    30,   119,    -1,   119,    22,
     119,    -1,   119,    21,   119,    -1,   119,    37,   119,    -1,
     119,    13,   119,    -1,    64,    97,    -1,    64,    97,    99,
      -1,    64,    97,    99,    98,    -1,    64,    97,    98,    -1,
      64,    97,    98,   105,    -1,    64,    97,    48,    49,   127,
      -1,    64,    97,    98,    48,    49,   127,    -1,    64,    97,
      48,   108,    49,   127,    -1,    64,    97,    98,    48,   108,
      49,   127,    -1,   119,    40,   119,    42,   119,    -1,   119,
      41,   119,    -1,    48,   119,    51,   119,    49,    -1,    48,
     119,    51,   119,    51,   119,    49,    -1,    48,   119,    51,
     119,    51,   119,    51,   119,    51,   119,    51,   119,    49,
      -1,   119,   120,   119,    -1,   119,   121,    -1,    13,   119,
      -1,    12,   119,    -1,   119,    13,    -1,   119,     4,   119,
      -1,   119,     5,   119,    -1,   119,     6,   119,    -1,   119,
       7,   119,    -1,   119,     8,   119,    -1,   119,     9,   119,
      -1,    61,    45,    83,    47,    -1,    12,    -1,   122,    -1,
     121,   122,    -1,   122,   121,    -1,   121,   122,   121,    -1,
      45,    18,   119,    47,    -1,    45,   119,    47,    -1,    45,
     119,    51,   119,    47,    -1,    45,   119,    51,   119,    51,
     119,    47,    -1,    10,    -1,    10,   123,    10,    -1,    10,
     124,    10,    -1,    11,    -1,    14,    -1,    16,   119,    -1,
      16,   119,    46,   119,    -1,    16,    17,   119,    -1,    16,
      17,   119,    46,   119,    -1,    15,   119,    -1,    15,   119,
      46,   119,    -1,    54,    -1,   127,    -1,   126,    54,    -1,
      65,    48,   119,    49,   125,    -1,    65,    48,   119,    49,
     125,    66,   125,    -1,    67,    48,   119,    49,   125,    -1,
      68,   125,    67,    48,   119,    49,    54,    -1,    69,    48,
     128,    54,   129,    54,   130,    49,   125,    -1,    69,    48,
      96,     3,    42,   119,    49,   125,    -1,    70,    54,    -1,
      71,    54,    -1,    72,    54,    -1,    72,   119,    54,    -1,
     119,    -1,   104,    -1,    -1,   131,    -1,    95,    -1,    -1,
     119,    -1,    -1,   131,    -1,   126,    -1,   131,    51,   126,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   177,   177,   181,   182,   187,   188,   193,   194,   198,
     199,   200,   202,   207,   208,   209,   211,   216,   217,   218,
     219,   221,   223,   225,   226,   228,   230,   232,   233,   238,
     240,   244,   245,   250,   251,   255,   257,   261,   262,   267,
     271,   275,   276,   280,   284,   285,   289,   290,   295,   296,
     301,   302,   307,   308,   309,   311,   316,   317,   321,   326,
     327,   329,   331,   336,   337,   338,   342,   344,   349,   350,
     352,   356,   357,   362,   363,   367,   369,   371,   376,   378,
     383,   384,   388,   389,   390,   391,   395,   396,   398,   399,
     401,   402,   405,   409,   410,   412,   414,   416,   420,   421,
     425,   426,   428,   432,   433,   439,   440,   441,   442,   444,
     445,   447,   449,   451,   453,   454,   455,   456,   457,   458,
     459,   460,   461,   462,   463,   464,   465,   466,   467,   468,
     469,   470,   471,   472,   473,   475,   477,   479,   481,   483,
     485,   490,   492,   497,   499,   501,   503,   505,   507,   509,
     515,   517,   520,   522,   523,   525,   526,   527,   528,   530,
     537,   538,   540,   543,   546,   552,   553,   554,   557,   563,
     564,   566,   568,   569,   573,   575,   578,   581,   587,   588,
     593,   594,   595,   596,   598,   600,   602,   604,   606,   608,
     609,   610,   611,   615,   619,   623,   624,   625,   629,   630,
     634,   635,   639,   640
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "ADD", "SUBTRACT", "TIMES",
  "DIVIDE", "MOD", "EXPONENT", "DOTS", "COLONS", "DASHES", "INCR",
  "LONGDASH", "CONTROLS", "TENSION", "ATLEAST", "CURL", "COR", "CAND",
  "BAR", "AMPERSAND", "EQ", "NEQ", "LT", "LE", "GT", "GE", "'+'", "CARETS",
  "'-'", "'*'", "'/'", "'%'", "'^'", "LOGNOT", "OPERATOR", "STRING",
  "LOOSE", "'?'", "ASSIGN", "':'", "DIRTAG", "JOIN_PREC", "'{'", "AND",
  "'}'", "'('", "')'", "'.'", "','", "'['", "']'", "';'", "ELLIPSIS",
  "ACCESS", "UNRAVEL", "IMPORT", "INCLUDE", "FROM", "QUOTE", "STRUCT",
  "TYPEDEF", "NEW", "IF", "ELSE", "WHILE", "DO", "FOR", "BREAK",
  "CONTINUE", "RETURN_", "THIS", "EXPLICIT", "GARBAGE", "LIT", "PERM",
  "MODIFIER", "UNARY", "EXP_IN_PARENS_RULE", "$accept", "file",
  "fileblock", "bareblock", "name", "runnable", "modifiers", "dec",
  "idpair", "idpairlist", "strid", "stridpair", "stridpairlist", "vardec",
  "barevardec", "type", "celltype", "dims", "dimexps", "decidlist",
  "decid", "decidstart", "varinit", "block", "arrayinit", "basearrayinit",
  "varinits", "formals", "baseformals", "explicitornot", "formal",
  "fundec", "typedec", "slice", "value", "argument", "arglist",
  "basearglist", "exp", "join", "dir", "basicjoin", "tension", "controls",
  "stm", "stmexp", "blockstm", "forinit", "fortest", "forupdate",
  "stmexplist", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,    43,
     284,    45,    42,    47,    37,    94,   285,   286,   287,   288,
      63,   289,    58,   290,   291,   123,   292,   125,    40,    41,
      46,    44,    91,    93,    59,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    81,    82,    83,    83,    84,    84,    85,    85,    86,
      86,    86,    86,    87,    87,    87,    87,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    89,
      89,    90,    90,    91,    91,    92,    92,    93,    93,    94,
      95,    96,    96,    97,    98,    98,    99,    99,   100,   100,
     101,   101,   102,   102,   102,   102,   103,   103,   104,   105,
     105,   105,   105,   106,   106,   106,   107,   107,   108,   108,
     108,   109,   109,   110,   110,   111,   111,   111,   112,   112,
     113,   113,   114,   114,   114,   114,   115,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   116,   116,
     117,   117,   117,   118,   118,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     120,   120,   120,   120,   120,   121,   121,   121,   121,   122,
     122,   122,   122,   122,   123,   123,   123,   123,   124,   124,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   126,   127,   128,   128,   128,   129,   129,
     130,   130,   131,   131
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     0,     2,     1,     3,     1,
       1,     2,     2,     1,     1,     2,     2,     1,     1,     1,
       3,     5,     5,     3,     5,     5,     3,     3,     3,     1,
       3,     1,     3,     1,     1,     1,     3,     1,     3,     2,
       2,     1,     2,     1,     2,     3,     3,     4,     1,     3,
       1,     3,     1,     2,     3,     4,     1,     1,     3,     2,
       4,     3,     5,     1,     1,     2,     1,     3,     1,     3,
       2,     1,     3,     1,     0,     2,     3,     5,     5,     6,
       3,     2,     1,     2,     2,     3,     3,     4,     4,     4,
       4,     3,     4,     3,     4,     3,     3,     1,     1,     3,
       1,     3,     2,     1,     3,     1,     1,     1,     1,     2,
       4,     5,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     4,     3,     4,     5,
       6,     6,     7,     5,     3,     5,     7,    13,     3,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     4,
       1,     1,     2,     2,     3,     4,     3,     5,     7,     1,
       3,     3,     1,     1,     2,     4,     3,     5,     2,     4,
       1,     1,     2,     5,     7,     5,     7,     9,     8,     2,
       2,     2,     3,     1,     1,     0,     1,     1,     0,     1,
       0,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     7,     0,     0,     0,     0,     0,
     108,     5,     0,   180,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      97,   107,    14,    13,   105,     4,     0,     9,    17,     0,
       0,    41,   194,    18,    19,   106,   193,    10,     0,   181,
     105,   151,   150,   112,   113,   114,     0,   105,     0,    35,
      33,     0,    37,     0,     0,     0,     0,     0,     7,     0,
       0,     3,     0,    43,    81,     0,    43,   134,     0,     0,
       0,   195,   189,   190,   191,     0,   109,     0,     0,     0,
      42,    16,    15,    11,    12,    39,    52,    40,    48,    50,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   169,
     172,   160,   152,   173,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   149,   161,   182,     0,
      58,     6,    96,     0,    95,     0,     0,     0,    20,    23,
      26,    27,    28,     0,     0,     0,    80,     0,    52,    74,
       0,   137,   135,     0,     0,     0,   197,     0,   202,     0,
     196,   192,     7,    91,     0,   103,     0,   100,    98,     8,
      82,    44,     0,     0,     0,    74,    53,     0,     0,    93,
       0,    86,     0,     0,   153,   154,   155,   156,   157,   158,
       0,     0,     0,     0,   133,   128,   127,   131,   130,   125,
     126,   121,   122,   123,   124,   115,   129,   116,   117,   118,
     119,   120,   132,     0,   144,     0,     0,   148,   162,   163,
     110,     0,     0,    36,    38,    29,     0,    31,     0,     0,
       0,   159,    74,     0,    74,    73,     0,    68,     0,    71,
       0,     0,    74,   138,     0,   136,     0,     0,     0,    52,
     198,     0,     0,   102,    92,     0,     0,    84,    89,    83,
      87,    45,    54,     0,    49,    51,    57,    56,    94,    90,
      88,   178,     0,   174,   170,   171,     0,     0,   166,     0,
     164,   111,   145,     0,     0,    22,     0,    21,    25,    24,
      54,     0,   139,    70,     0,    74,    74,    75,    46,    59,
      63,     0,    66,     0,    64,     0,     0,     0,   183,   185,
       0,     0,   199,     0,   203,    99,   104,   101,    85,    78,
      55,     0,   176,     0,   143,   165,     0,     0,    30,    32,
      55,   141,    72,    69,    76,     0,    61,     0,    65,   140,
       0,    47,     0,     0,     0,   200,    79,   179,     0,   175,
     167,     0,   146,     0,     0,    60,     0,    67,   142,   184,
     186,     0,     0,   201,   177,     0,     0,    77,    62,   188,
       0,   168,     0,   187,     0,     0,     0,   147
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    56,    50,    35,    36,    37,   237,   238,
      61,    62,    63,    38,    39,    40,    41,   186,   162,    97,
      98,    99,   275,    42,   276,   313,   314,   246,   247,   248,
     249,    43,    44,   182,    45,   175,   176,   177,    46,   135,
     136,   137,   202,   203,    47,    48,    49,   169,   323,   372,
     170
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -230
static const yytype_int16 yypact[] =
{
    -230,    40,   476,  -230,  -230,   780,   780,   780,   780,   780,
    -230,  -230,   780,  -230,     6,    60,     6,    18,    20,    27,
      76,    60,    60,    46,    83,   202,    96,    91,   113,   647,
    -230,    17,  -230,  -230,    35,  -230,   547,  -230,  -230,   140,
     176,  -230,  -230,  -230,  -230,   111,  1972,  -230,   148,  -230,
     118,   152,   152,   152,   152,  -230,   334,   108,  1147,   204,
    -230,   205,  -230,    92,    38,   155,   156,   157,   162,    -8,
     163,  -230,   167,    19,  -230,   210,   171,   124,   780,   780,
     160,   780,  -230,  -230,  -230,   896,   152,   168,   219,   291,
     172,  -230,  -230,  -230,  -230,  -230,   125,   174,  -230,   187,
     626,   227,   693,   780,   780,   780,   780,   780,   780,    14,
    -230,  -230,    17,  -230,   780,   780,   780,   780,   780,   780,
     780,   780,   780,   780,   780,   780,   780,   780,   780,   780,
     780,   780,   780,   780,   701,   780,  -230,   189,  -230,   693,
    -230,  -230,    17,    37,  -230,   780,   236,     6,  -230,  -230,
    -230,  -230,  -230,    15,    16,   405,  -230,   190,   126,   -22,
     715,    90,   193,  1483,  1529,   194,  -230,   245,  -230,   197,
     201,  -230,   213,  -230,   823,  -230,   216,   133,  1972,  -230,
     780,  -230,   206,   947,   209,   -14,   172,   210,   758,  -230,
     230,  -230,   215,   997,  1972,  1972,  1972,  1972,  1972,  1972,
     780,   769,   266,   272,   119,  2086,  2114,  2142,  2170,  2198,
    2198,  2207,  2207,  2207,  2207,   158,   413,   158,   152,   152,
     152,   152,   279,  1930,  1972,   780,  1195,   119,   189,  -230,
    -230,   780,  1243,  -230,  -230,   280,   231,  -230,   147,   232,
     149,  -230,   -12,   167,   221,  -230,   238,   134,    60,  -230,
    1047,   618,   -10,  -230,   715,   172,   202,   202,   780,     9,
     780,   780,   780,  -230,  -230,   823,   823,  1972,  -230,   780,
    -230,  -230,   167,   247,  -230,  -230,  -230,  1972,  -230,  -230,
    -230,  1801,   780,  1844,  -230,  -230,   780,  1713,  -230,   780,
    -230,  -230,  -230,   780,   294,  -230,   295,  -230,  -230,  -230,
    -230,   250,  -230,  -230,   167,   221,   221,   210,  -230,  -230,
    -230,   758,  -230,    12,   249,   167,   252,  1097,   239,  -230,
    1575,   780,  1972,   248,  -230,  1972,  -230,  -230,  1972,  -230,
     167,   780,  1887,   780,  2050,  -230,  1291,  1339,  -230,  -230,
    -230,  -230,  -230,  -230,   274,   260,  -230,   758,   758,  -230,
     167,  -230,   202,   263,  1621,   780,  -230,  2014,   780,  2014,
    -230,   780,  -230,   780,   758,  -230,   271,  -230,  -230,  -230,
    -230,   202,   270,   201,  2014,  1757,  1387,  -230,  -230,  -230,
     202,  -230,   780,  -230,  1435,   780,  1667,  -230
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -230,  -230,   254,  -230,    10,   265,  -230,   287,    34,   177,
     316,    -6,  -230,   314,   255,   -13,   318,   -23,  -230,  -230,
     151,    41,  -215,   269,   184,  -230,  -230,  -172,  -230,  -230,
    -229,  -230,  -230,   259,  -230,  -160,   257,  -230,    -5,  -230,
    -120,   214,  -230,  -230,   -20,   -75,  -168,  -230,  -230,  -230,
      -1
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -44
static const yytype_int16 yytable[] =
{
      51,    52,    53,    54,    55,    80,   168,    58,    75,    59,
      65,    90,    34,   273,   263,   303,    94,   229,   235,   235,
       4,    66,    57,    68,    85,    64,    86,   243,    69,   200,
     201,    73,    76,   244,   143,   272,   312,   300,   -43,   315,
       3,   244,    88,   244,    60,   244,    34,   236,   239,   153,
      90,   321,   245,     9,   161,    10,    67,   242,    60,   346,
     245,   157,   245,     4,   245,    12,    34,   347,   167,    88,
     301,   157,    71,   163,   164,   302,   342,   343,    19,    72,
     316,    22,   178,    87,   183,    88,   231,    89,    88,   184,
      30,    34,   149,    31,    78,   178,   345,   193,   194,   195,
     196,   197,   198,   199,   329,   326,   327,   204,   290,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   226,
     227,    79,   366,   367,   183,   251,   341,   230,   252,   255,
     232,   234,   184,   147,    81,    82,   148,   349,   124,   377,
     126,   127,   128,   129,   130,   250,    87,   142,    88,   100,
      89,   101,   356,   102,   134,    34,    87,    83,    88,   178,
     139,   172,   159,   185,   242,   267,   160,   157,   157,    96,
       5,     6,   368,   277,   265,   305,   324,   130,   266,   306,
     127,   128,   129,   130,    95,   281,   283,     7,   296,     8,
     296,   297,   138,   299,     9,     4,    10,   -34,   146,   150,
     151,   152,    11,   158,     5,     6,    12,   173,   -34,   154,
     287,    88,   179,   174,   184,   187,   291,   165,   188,    19,
     191,     7,    22,     8,   134,   307,   318,   319,     9,   233,
      10,    30,   258,   181,    31,   254,   277,    11,   259,   317,
      12,   260,   261,   320,   262,   322,    13,   325,    73,   268,
     178,   178,   271,    19,   328,   264,    22,    23,   279,    24,
      25,    26,    27,    28,    29,    30,   284,   332,    31,   278,
     168,   334,   285,   294,   336,   295,   298,   304,   337,   109,
     110,   111,   112,   113,     4,   245,   330,   338,   235,   340,
     348,   350,   355,     5,     6,   352,   277,   365,   124,   125,
     126,   127,   128,   129,   130,   364,   354,   370,   378,   380,
       7,   141,     8,    93,   134,   155,   357,     9,   359,    10,
     339,   240,   369,   180,    70,    74,   166,     4,   274,    12,
      77,   156,   277,   277,   181,   253,     5,     6,   344,     0,
     228,   379,    19,   374,   373,    22,   375,   190,   376,   277,
     383,   192,     0,     7,    30,     8,     0,    31,     0,     0,
       9,     0,    10,     0,     0,     0,     0,   384,     0,    11,
     386,   140,    12,     0,     0,     0,     0,     0,    13,     0,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
       0,    24,    25,    26,    27,    28,    29,    30,     4,     0,
      31,    32,    33,     0,     0,     0,     0,     5,     6,     0,
       0,     0,     0,   109,   110,   111,   112,   113,     0,     0,
       0,     0,     0,     0,     7,     0,     8,     0,     0,     0,
       0,     9,   124,    10,   126,   127,   128,   129,   130,     0,
      11,     0,   241,    12,     0,     0,     0,     0,   134,    13,
       0,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,     0,    24,    25,    26,    27,    28,    29,    30,     4,
       0,    31,    32,    33,     0,     0,     0,     0,     5,     6,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     7,     0,     8,     0,     0,
       0,     0,     9,     0,    10,     0,     0,     0,     0,     0,
       0,    11,     0,     0,    12,     0,     0,     0,     0,     0,
      13,     0,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
       4,     0,    31,    32,    33,     0,     0,     0,     0,     5,
       6,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     7,     0,     8,     0,
       0,     0,     0,     9,     0,    10,     0,     0,     0,     0,
       0,     0,    11,     0,     0,    12,     0,     0,     0,     0,
       0,    13,     0,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,     0,    24,    25,    26,    27,    28,    29,
      30,     4,     0,    31,    91,    92,     0,     0,     0,   172,
       5,     6,     0,     0,     0,     0,     0,     0,     5,     6,
       0,     0,     0,     0,     0,     0,     0,     7,     0,     8,
       4,     0,     0,     0,     9,     7,    10,     8,     0,     5,
       6,     0,     9,   251,    10,   309,    12,     0,     0,   310,
       0,     0,     0,   311,    12,   189,     7,     0,     8,    19,
       0,   174,    22,     9,     0,    10,     0,    19,     0,     0,
      22,    30,     0,     0,    31,    12,     4,     0,     0,    30,
       0,    84,    31,     0,     4,     5,     6,     0,    19,     0,
       0,    22,     0,     5,     6,     0,     0,     0,     4,   225,
      30,     0,     7,    31,     8,     0,     0,     5,     6,     9,
       7,    10,     8,     0,     0,   180,     0,     9,     0,    10,
       0,    12,     0,     0,     7,     0,     8,     0,     0,    12,
       0,     9,     0,    10,    19,     0,     0,    22,     0,     0,
       0,     4,    19,    12,     0,    22,    30,     0,   181,    31,
       5,     6,     4,     0,    30,     0,    19,    31,     0,    22,
       0,     5,     6,     4,     0,     0,   282,     7,    30,     8,
       0,    31,     5,     6,     9,     0,    10,     0,     7,     0,
       8,     0,     0,   251,     0,     9,    12,    10,     0,     7,
       0,     8,     0,     0,     0,     0,     9,    12,    10,    19,
       0,     0,    22,     0,     0,     0,   172,     0,    12,     0,
      19,    30,     0,    22,    31,     5,     6,     0,     0,     0,
       0,    19,    30,     0,    22,    31,     0,     0,     0,     0,
       0,     0,     7,    30,     8,     0,    31,     0,     0,     9,
       0,    10,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    12,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    19,     0,     0,    22,     0,     0,
       0,     0,     0,     0,     0,     0,    30,     0,     0,    31,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,     0,     0,     0,     0,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,     0,   131,     0,     0,   132,   133,     0,     0,
       0,   134,     0,     0,     0,     0,     0,     0,     0,     0,
     171,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,   269,
       0,     0,   134,     0,     0,     0,     0,     0,     0,     0,
     270,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,   269,
       0,     0,   134,     0,     0,     0,     0,     0,     0,     0,
     280,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,     0,
       0,     0,   134,     0,     0,     0,     0,     0,     0,     0,
     308,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,     0,
       0,     0,   134,     0,     0,     0,     0,     0,     0,     0,
     351,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,     0,
       0,     0,   134,     0,     0,     0,   144,     0,   145,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
       0,     0,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,     0,   131,     0,     0,   132,   133,     0,     0,     0,
     134,     0,   288,     0,     0,     0,   289,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,     0,     0,
       0,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,     0,
     131,     0,     0,   132,   133,     0,     0,     0,   134,     0,
       0,     0,   292,     0,   293,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,     0,     0,     0,     0,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,     0,   131,     0,
       0,   132,   133,     0,     0,     0,   134,     0,   360,     0,
       0,     0,   361,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,     0,     0,     0,     0,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,     0,   131,     0,     0,   132,
     133,     0,     0,     0,   134,     0,     0,     0,   362,     0,
     363,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,     0,
       0,     0,   134,     0,     0,     0,     0,     0,   382,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
       0,     0,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,     0,   131,     0,     0,   132,   133,     0,     0,     0,
     134,     0,     0,     0,     0,     0,   385,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,     0,     0,
       0,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,     0,
     131,     0,     0,   132,   133,     0,     0,     0,   134,     0,
       0,     0,   256,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,     0,     0,     0,     0,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,     0,   131,     0,     0,   132,
     133,     0,     0,     0,   134,     0,     0,     0,   257,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
       0,     0,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,     0,   131,     0,     0,   132,   133,     0,     0,     0,
     134,     0,     0,     0,   353,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,     0,     0,     0,     0,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,     0,   131,     0,
       0,   132,   133,     0,     0,     0,   134,     0,     0,     0,
     371,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,     0,
       0,     0,   134,     0,     0,     0,   387,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,     0,     0,
       0,     0,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,     0,
     131,     0,     0,   132,   133,     0,     0,     0,   134,     0,
     335,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,     0,
       0,     0,   134,     0,   381,   103,   104,   105,   106,   107,
     108,     0,   110,   111,   112,   113,     0,     0,     0,     0,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,     0,   131,     0,
       0,   132,   133,     0,     0,     0,   134,   331,   103,   104,
     105,   106,   107,   108,     0,   110,   111,   112,   113,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
       0,   131,     0,     0,   132,   133,     0,     0,     0,   134,
     333,   103,   104,   105,   106,   107,   108,     0,   110,   111,
     112,   113,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,     0,   131,     0,     0,   132,   133,     0,
       0,     0,   134,   358,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,     0,   131,     0,     0,
     132,   133,   286,     0,     0,   134,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,     0,     0,     0,
       0,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,     0,   131,
       0,     0,   132,   133,     0,     0,     0,   134,   103,   104,
     105,   106,   107,   108,     0,   110,   111,   112,   113,     0,
       0,     0,     0,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
       0,   131,     0,     0,   132,   133,     0,     0,     0,   134,
     109,   110,   111,   112,   113,     0,     0,     0,     0,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,     0,   131,     0,     0,
     132,     0,     0,     0,     0,   134,   109,   110,   111,   112,
     113,     0,     0,     0,     0,     0,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,     0,   131,   109,   110,   111,   112,   113,     0,
       0,   134,     0,     0,     0,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
       0,   131,   109,   110,   111,   112,   113,     0,     0,   134,
       0,     0,     0,     0,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,     0,   131,
     109,   110,   111,   112,   113,     0,     0,   134,     0,     0,
       0,     0,     0,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,     0,   131,   109,   110,
     111,   112,   113,     0,     0,   134,     0,   109,   110,   111,
     112,   113,     0,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,     0,   131,   124,   125,   126,   127,
     128,   129,   130,   134,   131,     0,     0,     0,     0,     0,
       0,     0,   134
};

static const yytype_int16 yycheck[] =
{
       5,     6,     7,     8,     9,    25,    81,    12,    21,     3,
      16,    34,     2,   185,   174,   244,    36,   137,     3,     3,
       3,     3,    12,     3,    29,    15,    31,    49,    18,    15,
      16,    21,    22,    55,    57,    49,   251,    49,     3,    49,
       0,    55,    50,    55,    38,    55,    36,    32,    32,    57,
      73,    42,    74,    36,    77,    38,    38,    48,    38,    47,
      74,    52,    74,     3,    74,    48,    56,    55,    81,    50,
     242,    52,    45,    78,    79,   243,   305,   306,    61,     3,
     252,    64,    87,    48,    89,    50,    49,    52,    50,    52,
      73,    81,    54,    76,    48,   100,   311,   102,   103,   104,
     105,   106,   107,   108,   272,   265,   266,   112,   228,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,    48,   347,   348,   139,    45,   304,   142,    48,   162,
     145,   147,    52,    51,    48,    54,    54,   315,    29,   364,
      31,    32,    33,    34,    35,   160,    48,    49,    50,    48,
      52,    50,   330,    52,    45,   155,    48,    54,    50,   174,
      52,     3,    48,    48,    48,   180,    52,    52,    52,     3,
      12,    13,   350,   188,    51,    51,   261,    35,    55,    55,
      32,    33,    34,    35,    54,   200,   201,    29,    51,    31,
      51,    54,    54,    54,    36,     3,    38,     3,     3,    54,
      54,    54,    45,     3,    12,    13,    48,    49,    56,    56,
     225,    50,     3,    55,    52,    51,   231,    67,    41,    61,
       3,    29,    64,    31,    45,   248,   256,   257,    36,     3,
      38,    73,    48,    53,    76,    52,   251,    45,     3,   254,
      48,    54,    51,   258,    41,   260,    54,   262,   248,    53,
     265,   266,    53,    61,   269,    49,    64,    65,    53,    67,
      68,    69,    70,    71,    72,    73,    10,   282,    76,    49,
     355,   286,    10,     3,   289,    54,    54,    49,   293,    10,
      11,    12,    13,    14,     3,    74,    49,     3,     3,    49,
      51,    49,    54,    12,    13,    66,   311,    47,    29,    30,
      31,    32,    33,    34,    35,    41,   321,    54,    47,    49,
      29,    56,    31,    36,    45,    71,   331,    36,   333,    38,
     296,   154,   352,    42,    18,    21,    81,     3,   187,    48,
      22,    72,   347,   348,    53,   161,    12,    13,   307,    -1,
     136,   371,    61,   358,   355,    64,   361,   100,   363,   364,
     380,   102,    -1,    29,    73,    31,    -1,    76,    -1,    -1,
      36,    -1,    38,    -1,    -1,    -1,    -1,   382,    -1,    45,
     385,    47,    48,    -1,    -1,    -1,    -1,    -1,    54,    -1,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      -1,    67,    68,    69,    70,    71,    72,    73,     3,    -1,
      76,    77,    78,    -1,    -1,    -1,    -1,    12,    13,    -1,
      -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    29,    -1,    31,    -1,    -1,    -1,
      -1,    36,    29,    38,    31,    32,    33,    34,    35,    -1,
      45,    -1,    47,    48,    -1,    -1,    -1,    -1,    45,    54,
      -1,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    -1,    67,    68,    69,    70,    71,    72,    73,     3,
      -1,    76,    77,    78,    -1,    -1,    -1,    -1,    12,    13,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    29,    -1,    31,    -1,    -1,
      -1,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    67,    68,    69,    70,    71,    72,    73,
       3,    -1,    76,    77,    78,    -1,    -1,    -1,    -1,    12,
      13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,    31,    -1,
      -1,    -1,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,
      -1,    -1,    45,    -1,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    54,    -1,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    -1,    67,    68,    69,    70,    71,    72,
      73,     3,    -1,    76,    77,    78,    -1,    -1,    -1,     3,
      12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    12,    13,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,    -1,    31,
       3,    -1,    -1,    -1,    36,    29,    38,    31,    -1,    12,
      13,    -1,    36,    45,    38,    47,    48,    -1,    -1,    51,
      -1,    -1,    -1,    55,    48,    49,    29,    -1,    31,    61,
      -1,    55,    64,    36,    -1,    38,    -1,    61,    -1,    -1,
      64,    73,    -1,    -1,    76,    48,     3,    -1,    -1,    73,
      -1,    54,    76,    -1,     3,    12,    13,    -1,    61,    -1,
      -1,    64,    -1,    12,    13,    -1,    -1,    -1,     3,    18,
      73,    -1,    29,    76,    31,    -1,    -1,    12,    13,    36,
      29,    38,    31,    -1,    -1,    42,    -1,    36,    -1,    38,
      -1,    48,    -1,    -1,    29,    -1,    31,    -1,    -1,    48,
      -1,    36,    -1,    38,    61,    -1,    -1,    64,    -1,    -1,
      -1,     3,    61,    48,    -1,    64,    73,    -1,    53,    76,
      12,    13,     3,    -1,    73,    -1,    61,    76,    -1,    64,
      -1,    12,    13,     3,    -1,    -1,    17,    29,    73,    31,
      -1,    76,    12,    13,    36,    -1,    38,    -1,    29,    -1,
      31,    -1,    -1,    45,    -1,    36,    48,    38,    -1,    29,
      -1,    31,    -1,    -1,    -1,    -1,    36,    48,    38,    61,
      -1,    -1,    64,    -1,    -1,    -1,     3,    -1,    48,    -1,
      61,    73,    -1,    64,    76,    12,    13,    -1,    -1,    -1,
      -1,    61,    73,    -1,    64,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    29,    73,    31,    -1,    76,    -1,    -1,    36,
      -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    -1,    -1,    76,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,
      -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    42,
      -1,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    42,
      -1,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      53,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    45,    -1,    -1,    -1,    49,    -1,    51,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,
      45,    -1,    47,    -1,    -1,    -1,    51,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    -1,
      -1,    -1,    49,    -1,    51,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    37,    -1,
      -1,    40,    41,    -1,    -1,    -1,    45,    -1,    47,    -1,
      -1,    -1,    51,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    -1,    -1,    40,
      41,    -1,    -1,    -1,    45,    -1,    -1,    -1,    49,    -1,
      51,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    45,    -1,    -1,    -1,    -1,    -1,    51,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,
      45,    -1,    -1,    -1,    -1,    -1,    51,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    -1,
      -1,    -1,    49,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    -1,    -1,    40,
      41,    -1,    -1,    -1,    45,    -1,    -1,    -1,    49,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,
      45,    -1,    -1,    -1,    49,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    37,    -1,
      -1,    40,    41,    -1,    -1,    -1,    45,    -1,    -1,    -1,
      49,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    45,    -1,    -1,    -1,    49,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    -1,
      47,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    45,    -1,    47,     4,     5,     6,     7,     8,
       9,    -1,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    37,    -1,
      -1,    40,    41,    -1,    -1,    -1,    45,    46,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,
      46,     4,     5,     6,     7,     8,     9,    -1,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    45,    46,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    45,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      -1,    -1,    40,    41,    -1,    -1,    -1,    45,     4,     5,
       6,     7,     8,     9,    -1,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    -1,    -1,    -1,    -1,    45,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    10,    11,    12,    13,    14,    -1,
      -1,    45,    -1,    -1,    -1,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    10,    11,    12,    13,    14,    -1,    -1,    45,
      -1,    -1,    -1,    -1,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      10,    11,    12,    13,    14,    -1,    -1,    45,    -1,    -1,
      -1,    -1,    -1,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    10,    11,
      12,    13,    14,    -1,    -1,    45,    -1,    10,    11,    12,
      13,    14,    -1,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    29,    30,    31,    32,
      33,    34,    35,    45,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    45
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    82,    83,     0,     3,    12,    13,    29,    31,    36,
      38,    45,    48,    54,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    67,    68,    69,    70,    71,    72,
      73,    76,    77,    78,    85,    86,    87,    88,    94,    95,
      96,    97,   104,   112,   113,   115,   119,   125,   126,   127,
      85,   119,   119,   119,   119,   119,    84,    85,   119,     3,
      38,    91,    92,    93,    85,    92,     3,    38,     3,    85,
      91,    45,     3,    85,    94,    96,    85,    97,    48,    48,
     125,    48,    54,    54,    54,   119,   119,    48,    50,    52,
      98,    77,    78,    88,   125,    54,     3,   100,   101,   102,
      48,    50,    52,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    37,    40,    41,    45,   120,   121,   122,    54,    52,
      47,    86,    49,    98,    49,    51,     3,    51,    54,    54,
      54,    54,    54,    57,    56,    83,   104,    52,     3,    48,
      52,    98,    99,   119,   119,    67,    95,    96,   126,   128,
     131,    54,     3,    49,    55,   116,   117,   118,   119,     3,
      42,    53,   114,   119,    52,    48,    98,    51,    41,    49,
     117,     3,   114,   119,   119,   119,   119,   119,   119,   119,
      15,    16,   123,   124,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,    18,   119,   119,   122,   121,
     119,    49,   119,     3,    92,     3,    32,    89,    90,    32,
      90,    47,    48,    49,    55,    74,   108,   109,   110,   111,
     119,    45,    48,   105,    52,    98,    49,    49,    48,     3,
      54,    51,    41,   116,    49,    51,    55,   119,    53,    42,
      53,    53,    49,   108,   101,   103,   105,   119,    49,    53,
      53,   119,    17,   119,    10,    10,    42,   119,    47,    51,
     121,   119,    49,    51,     3,    54,    51,    54,    54,    54,
      49,   108,   127,   111,    49,    51,    55,    96,    53,    47,
      51,    55,   103,   106,   107,    49,   108,   119,   125,   125,
     119,    42,   119,   129,   126,   119,   116,   116,   119,   127,
      49,    46,   119,    46,   119,    47,   119,   119,     3,    89,
      49,   127,   111,   111,   102,   103,    47,    55,    51,   127,
      49,    53,    66,    49,   119,    54,   127,   119,    46,   119,
      47,    51,    49,    51,    41,    47,   103,   103,   127,   125,
      54,    49,   130,   131,   119,   119,   119,   103,    47,   125,
      49,    47,    51,   125,   119,    51,   119,    49
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 177 "camp.y"
    { absyntax::root = (yyvsp[(1) - (1)].b); ;}
    break;

  case 3:
#line 181 "camp.y"
    { (yyval.b) = new file(lexerPos(), false); ;}
    break;

  case 4:
#line 183 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); ;}
    break;

  case 5:
#line 187 "camp.y"
    { (yyval.b) = new block(lexerPos(), true); ;}
    break;

  case 6:
#line 189 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); ;}
    break;

  case 7:
#line 193 "camp.y"
    { (yyval.n) = new simpleName((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 8:
#line 194 "camp.y"
    { (yyval.n) = new qualifiedName((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 9:
#line 198 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 10:
#line 199 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 11:
#line 201 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].d)); ;}
    break;

  case 12:
#line 203 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].s)); ;}
    break;

  case 13:
#line 207 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].mod).pos); (yyval.ml)->add((yyvsp[(1) - (1)].mod).val); ;}
    break;

  case 14:
#line 208 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].perm).pos); (yyval.ml)->add((yyvsp[(1) - (1)].perm).val); ;}
    break;

  case 15:
#line 210 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].mod).val); ;}
    break;

  case 16:
#line 212 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].perm).val); ;}
    break;

  case 17:
#line 216 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].vd); ;}
    break;

  case 18:
#line 217 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 19:
#line 218 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 20:
#line 220 "camp.y"
    { (yyval.d) = new accessdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ipl)); ;}
    break;

  case 21:
#line 222 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), (yyvsp[(4) - (5)].ipl)); ;}
    break;

  case 22:
#line 224 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), WILDCARD); ;}
    break;

  case 23:
#line 225 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].n), WILDCARD); ;}
    break;

  case 24:
#line 227 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, (yyvsp[(4) - (5)].ipl)); ;}
    break;

  case 25:
#line 229 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, WILDCARD); ;}
    break;

  case 26:
#line 231 "camp.y"
    { (yyval.d) = new importdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ip)); ;}
    break;

  case 27:
#line 232 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym); ;}
    break;

  case 28:
#line 234 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (string)*(yyvsp[(2) - (3)].ps).sym); ;}
    break;

  case 29:
#line 238 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 30:
#line 240 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 31:
#line 244 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); ;}
    break;

  case 32:
#line 246 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); ;}
    break;

  case 33:
#line 250 "camp.y"
    { (yyval.ps) = (yyvsp[(1) - (1)].ps); ;}
    break;

  case 34:
#line 251 "camp.y"
    { (yyval.ps) = (yyvsp[(1) - (1)].ps); ;}
    break;

  case 35:
#line 255 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 36:
#line 257 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 37:
#line 261 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); ;}
    break;

  case 38:
#line 263 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); ;}
    break;

  case 39:
#line 267 "camp.y"
    { (yyval.vd) = (yyvsp[(1) - (2)].vd); ;}
    break;

  case 40:
#line 271 "camp.y"
    { (yyval.vd) = new vardec((yyvsp[(1) - (2)].t)->getPos(), (yyvsp[(1) - (2)].t), (yyvsp[(2) - (2)].dil)); ;}
    break;

  case 41:
#line 275 "camp.y"
    { (yyval.t) = (yyvsp[(1) - (1)].t); ;}
    break;

  case 42:
#line 276 "camp.y"
    { (yyval.t) = new arrayTy((yyvsp[(1) - (2)].n), (yyvsp[(2) - (2)].dim)); ;}
    break;

  case 43:
#line 280 "camp.y"
    { (yyval.t) = new nameTy((yyvsp[(1) - (1)].n)); ;}
    break;

  case 44:
#line 284 "camp.y"
    { (yyval.dim) = new dimensions((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 45:
#line 285 "camp.y"
    { (yyval.dim) = (yyvsp[(1) - (3)].dim); (yyval.dim)->increase(); ;}
    break;

  case 46:
#line 289 "camp.y"
    { (yyval.elist) = new explist((yyvsp[(1) - (3)].pos)); (yyval.elist)->add((yyvsp[(2) - (3)].e)); ;}
    break;

  case 47:
#line 291 "camp.y"
    { (yyval.elist) = (yyvsp[(1) - (4)].elist); (yyval.elist)->add((yyvsp[(3) - (4)].e)); ;}
    break;

  case 48:
#line 295 "camp.y"
    { (yyval.dil) = new decidlist((yyvsp[(1) - (1)].di)->getPos()); (yyval.dil)->add((yyvsp[(1) - (1)].di)); ;}
    break;

  case 49:
#line 297 "camp.y"
    { (yyval.dil) = (yyvsp[(1) - (3)].dil); (yyval.dil)->add((yyvsp[(3) - (3)].di)); ;}
    break;

  case 50:
#line 301 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (1)].dis)->getPos(), (yyvsp[(1) - (1)].dis)); ;}
    break;

  case 51:
#line 303 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (3)].dis)->getPos(), (yyvsp[(1) - (3)].dis), (yyvsp[(3) - (3)].vi)); ;}
    break;

  case 52:
#line 307 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 53:
#line 308 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (2)].ps).pos, (yyvsp[(1) - (2)].ps).sym, (yyvsp[(2) - (2)].dim)); ;}
    break;

  case 54:
#line 309 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, 0,
                                            new formals((yyvsp[(2) - (3)].pos))); ;}
    break;

  case 55:
#line 312 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (4)].ps).pos, (yyvsp[(1) - (4)].ps).sym, 0, (yyvsp[(3) - (4)].fls)); ;}
    break;

  case 56:
#line 316 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 57:
#line 317 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].ai); ;}
    break;

  case 58:
#line 322 "camp.y"
    { (yyval.b) = (yyvsp[(2) - (3)].b); ;}
    break;

  case 59:
#line 326 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 60:
#line 328 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (4)].pos)); (yyval.ai)->addRest((yyvsp[(3) - (4)].vi)); ;}
    break;

  case 61:
#line 330 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (3)].ai); ;}
    break;

  case 62:
#line 332 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (5)].ai); (yyval.ai)->addRest((yyvsp[(4) - (5)].vi)); ;}
    break;

  case 63:
#line 336 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 64:
#line 337 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (1)].ai); ;}
    break;

  case 65:
#line 338 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (2)].ai); ;}
    break;

  case 66:
#line 342 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].vi)->getPos());
		     (yyval.ai)->add((yyvsp[(1) - (1)].vi));;}
    break;

  case 67:
#line 345 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (3)].ai); (yyval.ai)->add((yyvsp[(3) - (3)].vi)); ;}
    break;

  case 68:
#line 349 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (1)].fls); ;}
    break;

  case 69:
#line 351 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->addRest((yyvsp[(3) - (3)].fl)); ;}
    break;

  case 70:
#line 352 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (2)].pos)); (yyval.fls)->addRest((yyvsp[(2) - (2)].fl)); ;}
    break;

  case 71:
#line 356 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (1)].fl)->getPos()); (yyval.fls)->add((yyvsp[(1) - (1)].fl)); ;}
    break;

  case 72:
#line 358 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->add((yyvsp[(3) - (3)].fl)); ;}
    break;

  case 73:
#line 362 "camp.y"
    { (yyval.boo) = true; ;}
    break;

  case 74:
#line 363 "camp.y"
    { (yyval.boo) = false; ;}
    break;

  case 75:
#line 368 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (2)].t)->getPos(), (yyvsp[(2) - (2)].t), 0, 0, (yyvsp[(1) - (2)].boo)); ;}
    break;

  case 76:
#line 370 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (3)].t)->getPos(), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].dis), 0, (yyvsp[(1) - (3)].boo)); ;}
    break;

  case 77:
#line 372 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (5)].t)->getPos(), (yyvsp[(2) - (5)].t), (yyvsp[(3) - (5)].dis), (yyvsp[(5) - (5)].vi), (yyvsp[(1) - (5)].boo)); ;}
    break;

  case 78:
#line 377 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (5)].pos), (yyvsp[(1) - (5)].t), (yyvsp[(2) - (5)].ps).sym, new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 79:
#line 379 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (6)].pos), (yyvsp[(1) - (6)].t), (yyvsp[(2) - (6)].ps).sym, (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); ;}
    break;

  case 80:
#line 383 "camp.y"
    { (yyval.d) = new recorddec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].b)); ;}
    break;

  case 81:
#line 384 "camp.y"
    { (yyval.d) = new typedec((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].vd)); ;}
    break;

  case 82:
#line 388 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (1)].pos), 0, 0); ;}
    break;

  case 83:
#line 389 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (2)].pos), (yyvsp[(1) - (2)].e), 0); ;}
    break;

  case 84:
#line 390 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (2)].pos), 0, (yyvsp[(2) - (2)].e)); ;}
    break;

  case 85:
#line 391 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 86:
#line 395 "camp.y"
    { (yyval.e) = new fieldExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 87:
#line 396 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].e)); ;}
    break;

  case 88:
#line 398 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].e)); ;}
    break;

  case 89:
#line 399 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].slice)); ;}
    break;

  case 90:
#line 401 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].slice)); ;}
    break;

  case 91:
#line 402 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos),
                                      new nameExp((yyvsp[(1) - (3)].n)->getPos(), (yyvsp[(1) - (3)].n)),
                                      new arglist()); ;}
    break;

  case 92:
#line 406 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), 
                                      new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)),
                                      (yyvsp[(3) - (4)].alist)); ;}
    break;

  case 93:
#line 409 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), new arglist()); ;}
    break;

  case 94:
#line 411 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].alist)); ;}
    break;

  case 95:
#line 413 "camp.y"
    { (yyval.e) = (yyvsp[(2) - (3)].e); ;}
    break;

  case 96:
#line 415 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(2) - (3)].n)->getPos(), (yyvsp[(2) - (3)].n)); ;}
    break;

  case 97:
#line 416 "camp.y"
    { (yyval.e) = new thisExp((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 98:
#line 420 "camp.y"
    { (yyval.arg).name=0;      (yyval.arg).val=(yyvsp[(1) - (1)].e); ;}
    break;

  case 99:
#line 421 "camp.y"
    { (yyval.arg).name=(yyvsp[(1) - (3)].ps).sym; (yyval.arg).val=(yyvsp[(3) - (3)].e); ;}
    break;

  case 100:
#line 425 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (1)].alist); ;}
    break;

  case 101:
#line 427 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->rest = (yyvsp[(3) - (3)].arg); ;}
    break;

  case 102:
#line 428 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->rest = (yyvsp[(2) - (2)].arg); ;}
    break;

  case 103:
#line 432 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->add((yyvsp[(1) - (1)].arg)); ;}
    break;

  case 104:
#line 434 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->add((yyvsp[(3) - (3)].arg)); ;}
    break;

  case 105:
#line 439 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(1) - (1)].n)->getPos(), (yyvsp[(1) - (1)].n)); ;}
    break;

  case 106:
#line 440 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 107:
#line 441 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 108:
#line 442 "camp.y"
    { (yyval.e) = new stringExp((yyvsp[(1) - (1)].ps).pos, *(yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 109:
#line 444 "camp.y"
    { (yyval.e) = new scaleExp((yyvsp[(1) - (2)].e)->getPos(), (yyvsp[(1) - (2)].e), (yyvsp[(2) - (2)].e)); ;}
    break;

  case 110:
#line 446 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (4)].n)->getPos(), new nameTy((yyvsp[(2) - (4)].n)), (yyvsp[(4) - (4)].e)); ;}
    break;

  case 111:
#line 448 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (5)].n)->getPos(), new arrayTy((yyvsp[(2) - (5)].n), (yyvsp[(3) - (5)].dim)), (yyvsp[(5) - (5)].e)); ;}
    break;

  case 112:
#line 450 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 113:
#line 452 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 114:
#line 453 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 115:
#line 454 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 116:
#line 455 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 117:
#line 456 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 118:
#line 457 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 119:
#line 458 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 120:
#line 459 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 121:
#line 460 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 122:
#line 461 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 123:
#line 462 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 124:
#line 463 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 125:
#line 464 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 126:
#line 465 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 127:
#line 466 "camp.y"
    { (yyval.e) = new andExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 128:
#line 467 "camp.y"
    { (yyval.e) = new orExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 129:
#line 468 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 130:
#line 469 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 131:
#line 470 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 132:
#line 471 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 133:
#line 472 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 134:
#line 474 "camp.y"
    { (yyval.e) = new newRecordExp((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].t)); ;}
    break;

  case 135:
#line 476 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].elist), 0, 0); ;}
    break;

  case 136:
#line 478 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), (yyvsp[(3) - (4)].elist), (yyvsp[(4) - (4)].dim), 0); ;}
    break;

  case 137:
#line 480 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), 0, (yyvsp[(3) - (3)].dim), 0); ;}
    break;

  case 138:
#line 482 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), 0, (yyvsp[(3) - (4)].dim), (yyvsp[(4) - (4)].ai)); ;}
    break;

  case 139:
#line 484 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].t), new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 140:
#line 486 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos),
                                             new arrayTy((yyvsp[(2) - (6)].t)->getPos(), (yyvsp[(2) - (6)].t), (yyvsp[(3) - (6)].dim)),
                                             new formals((yyvsp[(4) - (6)].pos)),
                                             (yyvsp[(6) - (6)].s)); ;}
    break;

  case 141:
#line 491 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos), (yyvsp[(2) - (6)].t), (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); ;}
    break;

  case 142:
#line 493 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (7)].pos),
                                             new arrayTy((yyvsp[(2) - (7)].t)->getPos(), (yyvsp[(2) - (7)].t), (yyvsp[(3) - (7)].dim)),
                                             (yyvsp[(5) - (7)].fls),
                                             (yyvsp[(7) - (7)].s)); ;}
    break;

  case 143:
#line 498 "camp.y"
    { (yyval.e) = new conditionalExp((yyvsp[(2) - (5)].pos), (yyvsp[(1) - (5)].e), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].e)); ;}
    break;

  case 144:
#line 499 "camp.y"
    { (yyval.e) = new assignExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 145:
#line 502 "camp.y"
    { (yyval.e) = new pairExp((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].e)); ;}
    break;

  case 146:
#line 504 "camp.y"
    { (yyval.e) = new tripleExp((yyvsp[(1) - (7)].pos), (yyvsp[(2) - (7)].e), (yyvsp[(4) - (7)].e), (yyvsp[(6) - (7)].e)); ;}
    break;

  case 147:
#line 506 "camp.y"
    { (yyval.e) = new transformExp((yyvsp[(1) - (13)].pos), (yyvsp[(2) - (13)].e), (yyvsp[(4) - (13)].e), (yyvsp[(6) - (13)].e), (yyvsp[(8) - (13)].e), (yyvsp[(10) - (13)].e), (yyvsp[(12) - (13)].e)); ;}
    break;

  case 148:
#line 508 "camp.y"
    { (yyvsp[(2) - (3)].j)->pushFront((yyvsp[(1) - (3)].e)); (yyvsp[(2) - (3)].j)->pushBack((yyvsp[(3) - (3)].e)); (yyval.e) = (yyvsp[(2) - (3)].j); ;}
    break;

  case 149:
#line 510 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::OUT);
                     joinExp *jexp =
                         new joinExp((yyvsp[(2) - (2)].se)->getPos(), symbol::trans(".."));
                     (yyval.e)=jexp;
                     jexp->pushBack((yyvsp[(1) - (2)].e)); jexp->pushBack((yyvsp[(2) - (2)].se)); ;}
    break;

  case 150:
#line 516 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), symbol::trans("+")); ;}
    break;

  case 151:
#line 518 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), symbol::trans("-")); ;}
    break;

  case 152:
#line 521 "camp.y"
    { (yyval.e) = new postfixExp((yyvsp[(2) - (2)].ps).pos, (yyvsp[(1) - (2)].e), symbol::trans("+")); ;}
    break;

  case 153:
#line 522 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 154:
#line 524 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 155:
#line 525 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 156:
#line 526 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 157:
#line 527 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 158:
#line 529 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 159:
#line 531 "camp.y"
    { (yyval.e) = new quoteExp((yyvsp[(1) - (4)].pos), (yyvsp[(3) - (4)].b)); ;}
    break;

  case 160:
#line 537 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos,(yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 161:
#line 539 "camp.y"
    { (yyval.j) = (yyvsp[(1) - (1)].j); ;}
    break;

  case 162:
#line 541 "camp.y"
    { (yyvsp[(1) - (2)].se)->setSide(camp::OUT);
                     (yyval.j) = (yyvsp[(2) - (2)].j); (yyval.j)->pushFront((yyvsp[(1) - (2)].se)); ;}
    break;

  case 163:
#line 544 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(1) - (2)].j); (yyval.j)->pushBack((yyvsp[(2) - (2)].se)); ;}
    break;

  case 164:
#line 547 "camp.y"
    { (yyvsp[(1) - (3)].se)->setSide(camp::OUT); (yyvsp[(3) - (3)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(2) - (3)].j); (yyval.j)->pushFront((yyvsp[(1) - (3)].se)); (yyval.j)->pushBack((yyvsp[(3) - (3)].se)); ;}
    break;

  case 165:
#line 552 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(2) - (4)].ps).pos, (yyvsp[(2) - (4)].ps).sym, (yyvsp[(3) - (4)].e)); ;}
    break;

  case 166:
#line 553 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (3)].pos), symbol::opTrans("spec"), (yyvsp[(2) - (3)].e)); ;}
    break;

  case 167:
#line 555 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (5)].pos), symbol::opTrans("spec"),
				      new pairExp((yyvsp[(3) - (5)].pos), (yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].e))); ;}
    break;

  case 168:
#line 558 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (7)].pos), symbol::opTrans("spec"),
				      new tripleExp((yyvsp[(3) - (7)].pos), (yyvsp[(2) - (7)].e), (yyvsp[(4) - (7)].e), (yyvsp[(6) - (7)].e))); ;}
    break;

  case 169:
#line 563 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 170:
#line 565 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); ;}
    break;

  case 171:
#line 567 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); ;}
    break;

  case 172:
#line 568 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 173:
#line 569 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 174:
#line 573 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym,
                              new booleanExp((yyvsp[(1) - (2)].ps).pos, false)); ;}
    break;

  case 175:
#line 576 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e),
                              new booleanExp((yyvsp[(1) - (4)].ps).pos, false)); ;}
    break;

  case 176:
#line 579 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(3) - (3)].e), (yyvsp[(1) - (3)].ps).sym,
                              new booleanExp((yyvsp[(2) - (3)].ps).pos, true)); ;}
    break;

  case 177:
#line 582 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (5)].ps).pos, (yyvsp[(3) - (5)].e), (yyvsp[(1) - (5)].ps).sym, (yyvsp[(5) - (5)].e),
                              new booleanExp((yyvsp[(2) - (5)].ps).pos, true)); ;}
    break;

  case 178:
#line 587 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 179:
#line 589 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e)); ;}
    break;

  case 180:
#line 593 "camp.y"
    { (yyval.s) = new emptyStm((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 181:
#line 594 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 182:
#line 595 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (2)].s); ;}
    break;

  case 183:
#line 597 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 184:
#line 599 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (7)].pos), (yyvsp[(3) - (7)].e), (yyvsp[(5) - (7)].s), (yyvsp[(7) - (7)].s)); ;}
    break;

  case 185:
#line 601 "camp.y"
    { (yyval.s) = new whileStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 186:
#line 603 "camp.y"
    { (yyval.s) = new doStm((yyvsp[(1) - (7)].pos), (yyvsp[(2) - (7)].s), (yyvsp[(5) - (7)].e)); ;}
    break;

  case 187:
#line 605 "camp.y"
    { (yyval.s) = new forStm((yyvsp[(1) - (9)].pos), (yyvsp[(3) - (9)].run), (yyvsp[(5) - (9)].e), (yyvsp[(7) - (9)].sel), (yyvsp[(9) - (9)].s)); ;}
    break;

  case 188:
#line 607 "camp.y"
    { (yyval.s) = new extendedForStm((yyvsp[(1) - (8)].pos), (yyvsp[(3) - (8)].t), (yyvsp[(4) - (8)].ps).sym, (yyvsp[(6) - (8)].e), (yyvsp[(8) - (8)].s)); ;}
    break;

  case 189:
#line 608 "camp.y"
    { (yyval.s) = new breakStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 190:
#line 609 "camp.y"
    { (yyval.s) = new continueStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 191:
#line 610 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 192:
#line 611 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].e)); ;}
    break;

  case 193:
#line 615 "camp.y"
    { (yyval.s) = new expStm((yyvsp[(1) - (1)].e)->getPos(), (yyvsp[(1) - (1)].e)); ;}
    break;

  case 194:
#line 619 "camp.y"
    { (yyval.s) = new blockStm((yyvsp[(1) - (1)].b)->getPos(), (yyvsp[(1) - (1)].b)); ;}
    break;

  case 195:
#line 623 "camp.y"
    { (yyval.run) = 0; ;}
    break;

  case 196:
#line 624 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].sel); ;}
    break;

  case 197:
#line 625 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].vd); ;}
    break;

  case 198:
#line 629 "camp.y"
    { (yyval.e) = 0; ;}
    break;

  case 199:
#line 630 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 200:
#line 634 "camp.y"
    { (yyval.sel) = 0; ;}
    break;

  case 201:
#line 635 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (1)].sel); ;}
    break;

  case 202:
#line 639 "camp.y"
    { (yyval.sel) = new stmExpList((yyvsp[(1) - (1)].s)->getPos()); (yyval.sel)->add((yyvsp[(1) - (1)].s)); ;}
    break;

  case 203:
#line 641 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (3)].sel); (yyval.sel)->add((yyvsp[(3) - (3)].s)); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3303 "camp.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
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

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



