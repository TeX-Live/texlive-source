
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
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


/* Line 189 of yacc.c  */
#line 119 "camp.tab.c"

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



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 46 "camp.y"

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



/* Line 214 of yacc.c  */
#line 266 "camp.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 278 "camp.tab.c"

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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2361

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  81
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  204
/* YYNRULES -- Number of states.  */
#define YYNSTATES  389

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
      48,    49,    32,    30,    51,    31,    50,    33,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    42,    54,
       2,     2,     2,    41,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    52,     2,    53,    35,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,     2,    47,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    36,    37,    38,    39,    40,
      43,    44,    45,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    10,    13,    15,    19,
      21,    23,    25,    28,    31,    33,    35,    38,    41,    43,
      45,    47,    51,    57,    63,    67,    73,    79,    83,    87,
      91,    93,    97,    99,   103,   105,   107,   109,   113,   115,
     119,   122,   125,   127,   130,   132,   135,   139,   143,   148,
     150,   154,   156,   160,   162,   165,   169,   174,   176,   178,
     182,   185,   190,   194,   200,   202,   204,   207,   209,   213,
     215,   219,   222,   224,   228,   230,   231,   234,   238,   244,
     250,   257,   261,   264,   266,   269,   272,   276,   280,   285,
     290,   295,   300,   304,   309,   313,   318,   322,   326,   328,
     330,   334,   336,   340,   343,   345,   349,   351,   353,   355,
     357,   360,   365,   371,   374,   377,   380,   384,   388,   392,
     396,   400,   404,   408,   412,   416,   420,   424,   428,   432,
     436,   440,   444,   448,   452,   456,   459,   463,   468,   472,
     477,   483,   490,   497,   505,   511,   515,   521,   529,   543,
     547,   550,   553,   556,   559,   563,   567,   571,   575,   579,
     583,   588,   590,   592,   595,   598,   602,   607,   611,   617,
     625,   627,   631,   635,   637,   639,   642,   647,   651,   657,
     660,   665,   667,   669,   672,   678,   686,   692,   700,   710,
     719,   722,   725,   728,   732,   734,   736,   737,   739,   741,
     742,   744,   745,   747,   749
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      82,     0,    -1,    83,    -1,    -1,    83,    86,    -1,    -1,
      84,    86,    -1,     3,    -1,    85,    50,     3,    -1,    34,
      -1,    88,    -1,   125,    -1,    87,    88,    -1,    87,   125,
      -1,    78,    -1,    77,    -1,    87,    78,    -1,    87,    77,
      -1,    94,    -1,   112,    -1,   113,    -1,    56,    93,    54,
      -1,    60,    85,    57,    90,    54,    -1,    60,    85,    57,
      32,    54,    -1,    57,    85,    54,    -1,    60,    91,    56,
      90,    54,    -1,    60,    91,    56,    32,    54,    -1,    58,
      92,    54,    -1,    59,     3,    54,    -1,    59,    38,    54,
      -1,     3,    -1,     3,     3,     3,    -1,    89,    -1,    90,
      51,    89,    -1,    38,    -1,     3,    -1,     3,    -1,    91,
       3,     3,    -1,    92,    -1,    93,    51,    92,    -1,    95,
      54,    -1,    96,   100,    -1,    97,    -1,    85,    98,    -1,
      85,    -1,    52,    53,    -1,    98,    52,    53,    -1,    52,
     119,    53,    -1,    99,    52,   119,    53,    -1,   101,    -1,
     100,    51,   101,    -1,   102,    -1,   102,    40,   103,    -1,
       3,    -1,     3,    98,    -1,     3,    48,    49,    -1,     3,
      48,   108,    49,    -1,   119,    -1,   105,    -1,    46,    84,
      47,    -1,    46,    47,    -1,    46,    55,   103,    47,    -1,
      46,   106,    47,    -1,    46,   106,    55,   103,    47,    -1,
      51,    -1,   107,    -1,   107,    51,    -1,   103,    -1,   107,
      51,   103,    -1,   109,    -1,   109,    55,   111,    -1,    55,
     111,    -1,   111,    -1,   109,    51,   111,    -1,    74,    -1,
      -1,   110,    96,    -1,   110,    96,   102,    -1,   110,    96,
     102,    40,   103,    -1,    96,     3,    48,    49,   127,    -1,
      96,     3,    48,   108,    49,   127,    -1,    62,     3,   104,
      -1,    63,    94,    -1,    42,    -1,   119,    42,    -1,    42,
     119,    -1,   119,    42,   119,    -1,   115,    50,     3,    -1,
      85,    52,   119,    53,    -1,   115,    52,   119,    53,    -1,
      85,    52,   114,    53,    -1,   115,    52,   114,    53,    -1,
      85,    48,    49,    -1,    85,    48,   117,    49,    -1,   115,
      48,    49,    -1,   115,    48,   117,    49,    -1,    48,   119,
      49,    -1,    48,    85,    49,    -1,    73,    -1,   119,    -1,
       3,    40,   119,    -1,   118,    -1,   118,    55,   116,    -1,
      55,   116,    -1,   116,    -1,   118,    51,   116,    -1,    85,
      -1,   115,    -1,    76,    -1,    38,    -1,    76,   119,    -1,
      48,    85,    49,   119,    -1,    48,    85,    98,    49,   119,
      -1,    30,   119,    -1,    31,   119,    -1,    36,   119,    -1,
     119,    30,   119,    -1,   119,    31,   119,    -1,   119,    32,
     119,    -1,   119,    33,   119,    -1,   119,    34,   119,    -1,
     119,    35,   119,    -1,   119,    25,   119,    -1,   119,    26,
     119,    -1,   119,    27,   119,    -1,   119,    28,   119,    -1,
     119,    23,   119,    -1,   119,    24,   119,    -1,   119,    20,
     119,    -1,   119,    19,   119,    -1,   119,    29,   119,    -1,
     119,    22,   119,    -1,   119,    21,   119,    -1,   119,    37,
     119,    -1,   119,    13,   119,    -1,    64,    97,    -1,    64,
      97,    99,    -1,    64,    97,    99,    98,    -1,    64,    97,
      98,    -1,    64,    97,    98,   105,    -1,    64,    97,    48,
      49,   127,    -1,    64,    97,    98,    48,    49,   127,    -1,
      64,    97,    48,   108,    49,   127,    -1,    64,    97,    98,
      48,   108,    49,   127,    -1,   119,    41,   119,    42,   119,
      -1,   119,    40,   119,    -1,    48,   119,    51,   119,    49,
      -1,    48,   119,    51,   119,    51,   119,    49,    -1,    48,
     119,    51,   119,    51,   119,    51,   119,    51,   119,    51,
     119,    49,    -1,   119,   120,   119,    -1,   119,   121,    -1,
      13,   119,    -1,    12,   119,    -1,   119,    13,    -1,   119,
       4,   119,    -1,   119,     5,   119,    -1,   119,     6,   119,
      -1,   119,     7,   119,    -1,   119,     8,   119,    -1,   119,
       9,   119,    -1,    61,    46,    83,    47,    -1,    12,    -1,
     122,    -1,   121,   122,    -1,   122,   121,    -1,   121,   122,
     121,    -1,    46,    18,   119,    47,    -1,    46,   119,    47,
      -1,    46,   119,    51,   119,    47,    -1,    46,   119,    51,
     119,    51,   119,    47,    -1,    10,    -1,    10,   123,    10,
      -1,    10,   124,    10,    -1,    11,    -1,    14,    -1,    16,
     119,    -1,    16,   119,    45,   119,    -1,    16,    17,   119,
      -1,    16,    17,   119,    45,   119,    -1,    15,   119,    -1,
      15,   119,    45,   119,    -1,    54,    -1,   127,    -1,   126,
      54,    -1,    65,    48,   119,    49,   125,    -1,    65,    48,
     119,    49,   125,    66,   125,    -1,    67,    48,   119,    49,
     125,    -1,    68,   125,    67,    48,   119,    49,    54,    -1,
      69,    48,   128,    54,   129,    54,   130,    49,   125,    -1,
      69,    48,    96,     3,    42,   119,    49,   125,    -1,    70,
      54,    -1,    71,    54,    -1,    72,    54,    -1,    72,   119,
      54,    -1,   119,    -1,   104,    -1,    -1,   131,    -1,    95,
      -1,    -1,   119,    -1,    -1,   131,    -1,   126,    -1,   131,
      51,   126,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   177,   177,   181,   182,   187,   188,   193,   194,   195,
     200,   201,   202,   204,   209,   210,   211,   213,   218,   219,
     220,   221,   223,   225,   227,   228,   230,   232,   234,   235,
     240,   242,   246,   247,   252,   253,   257,   259,   263,   264,
     269,   273,   277,   278,   282,   286,   287,   291,   292,   297,
     298,   303,   304,   309,   310,   311,   313,   318,   319,   323,
     328,   329,   331,   333,   338,   339,   340,   344,   346,   351,
     352,   354,   358,   359,   364,   365,   369,   371,   373,   378,
     380,   385,   386,   390,   391,   392,   393,   397,   398,   400,
     401,   403,   404,   407,   411,   412,   414,   416,   418,   422,
     423,   427,   428,   430,   434,   435,   441,   442,   443,   444,
     446,   447,   449,   451,   453,   455,   456,   457,   458,   459,
     460,   461,   462,   463,   464,   465,   466,   467,   468,   469,
     470,   471,   472,   473,   474,   475,   477,   479,   481,   483,
     485,   487,   492,   494,   499,   501,   503,   505,   507,   509,
     511,   517,   519,   522,   524,   525,   527,   528,   529,   530,
     532,   539,   540,   542,   545,   548,   554,   555,   556,   559,
     565,   566,   568,   570,   571,   575,   577,   580,   583,   589,
     590,   595,   596,   597,   598,   600,   602,   604,   606,   608,
     610,   611,   612,   613,   617,   621,   625,   626,   627,   631,
     632,   636,   637,   641,   642
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
  "BAR", "AMPERSAND", "EQ", "NEQ", "LT", "LE", "GT", "GE", "CARETS", "'+'",
  "'-'", "'*'", "'/'", "'%'", "'^'", "LOGNOT", "OPERATOR", "STRING",
  "LOOSE", "ASSIGN", "'?'", "':'", "DIRTAG", "JOIN_PREC", "AND", "'{'",
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
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
      43,    45,    42,    47,    37,    94,   285,   286,   287,   288,
     289,    63,    58,   290,   291,   292,   123,   125,    40,    41,
      46,    44,    91,    93,    59,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    81,    82,    83,    83,    84,    84,    85,    85,    85,
      86,    86,    86,    86,    87,    87,    87,    87,    88,    88,
      88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
      89,    89,    90,    90,    91,    91,    92,    92,    93,    93,
      94,    95,    96,    96,    97,    98,    98,    99,    99,   100,
     100,   101,   101,   102,   102,   102,   102,   103,   103,   104,
     105,   105,   105,   105,   106,   106,   106,   107,   107,   108,
     108,   108,   109,   109,   110,   110,   111,   111,   111,   112,
     112,   113,   113,   114,   114,   114,   114,   115,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   116,
     116,   117,   117,   117,   118,   118,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   120,   120,   120,   120,   120,   121,   121,   121,   121,
     122,   122,   122,   122,   122,   123,   123,   123,   123,   124,
     124,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   126,   127,   128,   128,   128,   129,
     129,   130,   130,   131,   131
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     0,     2,     1,     3,     1,
       1,     1,     2,     2,     1,     1,     2,     2,     1,     1,
       1,     3,     5,     5,     3,     5,     5,     3,     3,     3,
       1,     3,     1,     3,     1,     1,     1,     3,     1,     3,
       2,     2,     1,     2,     1,     2,     3,     3,     4,     1,
       3,     1,     3,     1,     2,     3,     4,     1,     1,     3,
       2,     4,     3,     5,     1,     1,     2,     1,     3,     1,
       3,     2,     1,     3,     1,     0,     2,     3,     5,     5,
       6,     3,     2,     1,     2,     2,     3,     3,     4,     4,
       4,     4,     3,     4,     3,     4,     3,     3,     1,     1,
       3,     1,     3,     2,     1,     3,     1,     1,     1,     1,
       2,     4,     5,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     3,     4,     3,     4,
       5,     6,     6,     7,     5,     3,     5,     7,    13,     3,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       4,     1,     1,     2,     2,     3,     4,     3,     5,     7,
       1,     3,     3,     1,     1,     2,     4,     3,     5,     2,
       4,     1,     1,     2,     5,     7,     5,     7,     9,     8,
       2,     2,     2,     3,     1,     1,     0,     1,     1,     0,
       1,     0,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     7,     0,     0,     0,     0,     9,
       0,   109,     5,     0,   181,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    98,   108,    15,    14,   106,     4,     0,    10,    18,
       0,     0,    42,   195,    19,    20,   107,   194,    11,     0,
     182,   106,   152,   151,   113,   114,   115,     0,   106,     0,
      36,    34,     0,    38,     0,     0,     0,     0,     0,     7,
       0,     0,     3,     0,    44,    82,     0,    44,   135,     0,
       0,     0,   196,   190,   191,   192,     0,   110,     0,     0,
       0,    43,    17,    16,    12,    13,    40,    53,    41,    49,
      51,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     170,   173,   161,   153,   174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   150,   162,   183,
       0,    59,     6,    97,     0,    96,     0,     0,     0,    21,
      24,    27,    28,    29,     0,     0,     0,    81,     0,    53,
      75,     0,   138,   136,     0,     0,     0,   198,     0,   203,
       0,   197,   193,     7,    92,     0,   104,     0,   101,    99,
       8,    83,    45,     0,     0,     0,    75,    54,     0,     0,
      94,     0,    87,     0,     0,   154,   155,   156,   157,   158,
     159,     0,     0,     0,     0,   134,   129,   128,   132,   131,
     126,   127,   122,   123,   124,   125,   130,   116,   117,   118,
     119,   120,   121,   133,   145,     0,     0,     0,   149,   163,
     164,   111,     0,     0,    37,    39,    30,     0,    32,     0,
       0,     0,   160,    75,     0,    75,    74,     0,    69,     0,
      72,     0,     0,    75,   139,     0,   137,     0,     0,     0,
      53,   199,     0,     0,   103,    93,     0,     0,    85,    90,
      84,    88,    46,    55,     0,    50,    52,    58,    57,    95,
      91,    89,   179,     0,   175,   171,   172,     0,     0,   167,
       0,   165,   112,   146,     0,     0,    23,     0,    22,    26,
      25,    55,     0,   140,    71,     0,    75,    75,    76,    47,
      60,    64,     0,    67,     0,    65,     0,     0,     0,   184,
     186,     0,     0,   200,     0,   204,   100,   105,   102,    86,
      79,    56,     0,   177,     0,   144,   166,     0,     0,    31,
      33,    56,   142,    73,    70,    77,     0,    62,     0,    66,
     141,     0,    48,     0,     0,     0,   201,    80,   180,     0,
     176,   168,     0,   147,     0,     0,    61,     0,    68,   143,
     185,   187,     0,     0,   202,   178,     0,     0,    78,    63,
     189,     0,   169,     0,   188,     0,     0,     0,   148
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    57,    51,    36,    37,    38,   238,   239,
      62,    63,    64,    39,    40,    41,    42,   187,   163,    98,
      99,   100,   276,    43,   277,   314,   315,   247,   248,   249,
     250,    44,    45,   183,    46,   176,   177,   178,    47,   136,
     137,   138,   203,   204,    48,    49,    50,   170,   324,   373,
     171
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -236
static const yytype_int16 yypact[] =
{
    -236,    58,   478,  -236,  -236,   892,   892,   892,   892,  -236,
     892,  -236,  -236,   892,  -236,    13,    28,    13,    19,    18,
      -3,    25,    28,    28,    31,    46,   620,    84,    12,   118,
     696,  -236,    17,  -236,  -236,    11,  -236,   549,  -236,  -236,
     140,    70,  -236,  -236,  -236,  -236,    34,  1957,  -236,   147,
    -236,   110,   150,   150,   150,   150,  -236,   336,   103,  1260,
     156,  -236,   202,  -236,   113,   -10,   164,   171,   172,   174,
     -12,   177,  -236,   176,   143,  -236,   225,   184,   117,   892,
     892,   169,   892,  -236,  -236,  -236,  1009,   150,   239,   232,
     292,   187,  -236,  -236,  -236,  -236,  -236,   125,   189,  -236,
     203,   664,   242,   740,   892,   892,   892,   892,   892,   892,
      72,  -236,  -236,    17,  -236,   892,   892,   892,   892,   892,
     892,   892,   892,   892,   892,   892,   892,   892,   892,   892,
     892,   892,   892,   892,   892,   772,   892,  -236,   200,  -236,
     740,  -236,  -236,    17,   105,  -236,   892,   245,    13,  -236,
    -236,  -236,  -236,  -236,    16,    36,   407,  -236,   196,   126,
     -25,   816,    43,   201,  1596,  1642,   207,  -236,   254,  -236,
     206,   212,  -236,   224,  -236,   936,  -236,   217,   124,  1957,
    -236,   892,  -236,   214,  1060,   215,   -14,   187,   225,   827,
    -236,   222,  -236,   219,  1110,  1957,  1957,  1957,  1957,  1957,
    1957,   892,   881,   264,   266,   464,  2203,  2231,  2259,  2287,
    2315,  2315,   948,   948,   948,   948,   416,   112,   112,   150,
     150,   150,   150,   178,  1957,  1914,   892,  1308,   464,   200,
    -236,  -236,   892,  1356,  -236,  -236,   276,   226,  -236,   149,
     229,   166,  -236,    87,   176,   210,  -236,   237,   131,    28,
    -236,  1160,   168,    94,  -236,   816,   187,   620,   620,   892,
      -6,   892,   892,   892,  -236,  -236,   936,   936,  1957,  -236,
     892,  -236,  -236,   176,   241,  -236,  -236,  -236,  1957,  -236,
    -236,  -236,  2000,   892,  2043,  -236,  -236,   892,  1826,  -236,
     892,  -236,  -236,  -236,   892,   288,  -236,   289,  -236,  -236,
    -236,  -236,   244,  -236,  -236,   176,   210,   210,   225,  -236,
    -236,  -236,   827,  -236,    22,   246,   176,   247,  1210,   233,
    -236,  1688,   892,  1957,   248,  -236,  1957,  -236,  -236,  1957,
    -236,   176,   892,  2086,   892,  2166,  -236,  1404,  1452,  -236,
    -236,  -236,  -236,  -236,  -236,   258,   259,  -236,   827,   827,
    -236,   176,  -236,   620,   255,  1734,   892,  -236,  2129,   892,
    2129,  -236,   892,  -236,   892,   827,  -236,   261,  -236,  -236,
    -236,  -236,   620,   252,   212,  2129,  1870,  1500,  -236,  -236,
    -236,   620,  -236,   892,  -236,  1548,   892,  1780,  -236
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -236,  -236,   238,  -236,    10,   256,  -236,   274,    21,   159,
     297,    -8,  -236,   298,   243,   -18,   296,   -24,  -236,  -236,
     133,    23,  -215,   251,   170,  -236,  -236,  -173,  -236,  -236,
    -235,  -236,  -236,   234,  -236,  -160,   235,  -236,    -5,  -236,
    -120,   198,  -236,  -236,   -20,   -75,  -168,  -236,  -236,  -236,
     -15
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -45
static const yytype_int16 yytable[] =
{
      52,    53,    54,    55,    76,    56,    81,   169,    59,    66,
     304,    91,    35,   274,   -44,   264,    60,    95,   230,   236,
       4,    69,    67,    58,   244,    86,    65,    87,    73,    70,
     245,     4,    74,    77,   144,   273,   322,   313,    89,   236,
      89,   245,   243,    72,   150,   154,   158,    35,   237,   246,
      91,    61,     9,    10,   162,    11,    61,    68,     3,    88,
     246,    89,     9,    90,   168,    13,    83,    35,   240,   347,
     302,   343,   344,    97,   164,   165,   303,   348,    20,    79,
     317,    23,   101,   179,   102,   184,   103,   201,   202,   252,
      31,   253,    35,    32,    80,   185,   179,   346,   194,   195,
     196,   197,   198,   199,   200,   330,   327,   328,   205,   291,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     227,   228,    82,   367,   368,   184,   301,   342,   231,   256,
     235,   233,   245,   316,   128,   129,   130,   131,   350,   245,
     378,    88,   143,    89,   232,    90,   251,   185,    88,   -35,
      89,   246,   140,   357,   148,   160,    35,   149,   246,   161,
     179,     4,    84,   186,   243,   266,   268,   158,   158,   267,
       5,     6,   306,   369,   278,   131,   307,   325,   110,   111,
     112,   113,   114,    89,    96,   158,   282,   284,     7,     8,
     297,   139,     9,   298,    10,   147,    11,   125,   126,   127,
     128,   129,   130,   131,   252,   310,    13,   297,   151,   311,
     300,   288,    12,   312,   135,   152,   153,   292,   159,    20,
     -35,   308,    23,   155,    89,   180,   166,   319,   320,   185,
     188,    31,   173,   189,    32,   192,   135,   278,   234,   182,
     318,     5,     6,   255,   321,   259,   323,   260,   326,    74,
     261,   179,   179,   262,   263,   329,   265,   269,   272,     7,
       8,   279,   280,     9,   285,    10,   286,    11,   333,   295,
     296,   169,   335,   299,   246,   337,   305,    13,   174,   338,
     331,   339,   236,   341,   175,     4,   351,   349,   365,   353,
      20,   381,   356,    23,     5,     6,   366,   278,   379,   371,
     156,    94,    31,   142,   241,    32,    71,   355,   340,    78,
      75,   275,     7,     8,   157,   167,     9,   358,    10,   360,
      11,   345,   254,   370,   181,   229,   191,   193,     0,     4,
      13,   374,     0,   278,   278,   182,     0,     0,     5,     6,
       0,     0,   380,    20,   375,     0,    23,   376,     0,   377,
     278,   384,     0,     0,     0,    31,     7,     8,    32,     0,
       9,     0,    10,     0,    11,     0,     0,     0,   385,     0,
       0,   387,    12,   141,    13,     0,     0,     0,     0,     0,
      14,     0,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,     0,    25,    26,    27,    28,    29,    30,    31,
       4,     0,    32,    33,    34,     0,     0,     0,     0,     5,
       6,     0,     0,     0,     0,     0,   110,   111,   112,   113,
     114,     0,     0,     0,     0,     0,     0,     7,     8,     0,
       0,     9,     0,    10,     0,    11,   126,   127,   128,   129,
     130,   131,     0,    12,   242,    13,     0,     0,     0,     0,
       0,    14,   135,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,     0,    25,    26,    27,    28,    29,    30,
      31,     4,     0,    32,    33,    34,     0,     0,     0,     0,
       5,     6,     0,     0,   126,   127,   128,   129,   130,   131,
       0,     0,     0,     0,     0,     0,     0,     0,     7,     8,
     135,     0,     9,     0,    10,     0,    11,     0,     0,     0,
       0,     0,     0,     0,    12,     0,    13,     0,     0,     0,
       0,     0,    14,     0,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,     0,    25,    26,    27,    28,    29,
      30,    31,     4,     0,    32,    33,    34,     0,     0,     0,
       0,     5,     6,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       8,     0,     0,     9,     0,    10,     0,    11,     0,     0,
       0,     0,     0,     0,     0,    12,     0,    13,     0,     0,
       0,     0,     0,    14,     0,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,     0,    25,    26,    27,    28,
      29,    30,    31,     4,     0,    32,    92,    93,     0,     0,
       0,     0,     5,     6,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     8,     0,     0,     9,     0,    10,     0,    11,     0,
       0,     0,     0,     0,     0,     0,    12,   173,    13,     0,
       0,     0,     0,     0,    14,     0,     5,     6,     0,     0,
       0,    20,     0,     0,    23,    24,     0,    25,    26,    27,
      28,    29,    30,    31,     7,     8,    32,     0,     9,     4,
      10,     0,    11,     0,     0,     0,     0,     0,     5,     6,
       0,     0,    13,   190,     0,     0,     0,     0,     0,   175,
       0,     0,     0,     0,     0,    20,     7,     8,    23,     0,
       9,     0,    10,     0,    11,     0,     0,    31,     0,     0,
      32,     0,     0,     4,    13,     0,     0,     0,     0,     0,
      85,     0,     5,     6,     0,     0,     0,    20,     0,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,    31,
       7,     8,    32,     0,     9,     4,    10,     0,    11,     0,
       0,     0,   181,     0,     5,     6,     0,     0,    13,     0,
     226,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    20,     7,     8,    23,     0,     9,     0,    10,     0,
      11,     0,     0,    31,     0,     0,    32,     0,     0,     4,
      13,     0,     0,     0,     0,     0,     0,     0,     5,     6,
       4,     0,     0,    20,     0,     0,    23,     0,     0,     5,
       6,     0,     0,     0,     0,    31,     7,     8,    32,     0,
       9,     0,    10,     0,    11,     0,     0,     7,     8,     0,
       0,     9,     0,    10,    13,    11,     0,     0,     0,   182,
       0,     0,     0,   252,     0,    13,     0,    20,     0,     0,
      23,     0,     0,     0,     4,     0,     0,     0,    20,    31,
       0,    23,    32,     5,     6,     4,     0,     0,   283,     0,
      31,     0,     0,    32,     5,     6,     0,     0,     0,     0,
       0,     7,     8,     0,     0,     9,     0,    10,     0,    11,
       0,     0,     7,     8,     0,     0,     9,     0,    10,    13,
      11,     0,     0,     0,     0,     0,     0,     0,     0,   173,
      13,     0,    20,     0,     0,    23,     0,     0,     5,     6,
       0,     0,     0,    20,    31,     0,    23,    32,   110,   111,
     112,   113,   114,     0,     0,    31,     7,     8,    32,     0,
       9,     0,    10,     0,    11,     0,     0,   125,   126,   127,
     128,   129,   130,   131,    13,   132,     0,     0,     0,     0,
       0,     0,     0,     0,   135,     0,     0,    20,     0,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,    31,
       0,     0,    32,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,     0,     0,     0,     0,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,     0,   132,     0,     0,   133,
     134,     0,     0,     0,     0,   135,     0,     0,     0,     0,
       0,     0,     0,   172,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,   270,     0,     0,     0,   135,     0,     0,     0,
       0,     0,     0,   271,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,   270,     0,     0,     0,   135,     0,     0,     0,
       0,     0,     0,   281,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,     0,     0,     0,     0,   135,     0,     0,     0,
       0,     0,     0,   309,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,     0,     0,     0,     0,   135,     0,     0,     0,
       0,     0,     0,   352,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,     0,     0,     0,     0,   135,     0,     0,   145,
       0,   146,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,     0,     0,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,     0,   132,     0,     0,   133,   134,
       0,     0,     0,     0,   135,   289,     0,     0,     0,   290,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,     0,     0,     0,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,     0,     0,   133,   134,     0,     0,
       0,     0,   135,     0,     0,   293,     0,   294,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,     0,
       0,     0,     0,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
       0,   132,     0,     0,   133,   134,     0,     0,     0,     0,
     135,   361,     0,     0,     0,   362,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,     0,     0,     0,
       0,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,     0,   132,
       0,     0,   133,   134,     0,     0,     0,     0,   135,     0,
       0,   363,     0,   364,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,     0,     0,     0,     0,   135,     0,     0,     0,
       0,   383,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,     0,     0,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,     0,   132,     0,     0,   133,   134,
       0,     0,     0,     0,   135,     0,     0,     0,     0,   386,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,     0,     0,     0,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,     0,     0,   133,   134,     0,     0,
       0,     0,   135,     0,     0,   257,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,     0,     0,     0,
       0,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,     0,   132,
       0,     0,   133,   134,     0,     0,     0,     0,   135,     0,
       0,   258,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,     0,     0,     0,     0,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,     0,   132,     0,     0,   133,   134,
       0,     0,     0,     0,   135,     0,     0,   354,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,     0,
       0,     0,     0,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
       0,   132,     0,     0,   133,   134,     0,     0,     0,     0,
     135,     0,     0,   372,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,     0,     0,     0,     0,   135,     0,     0,   388,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,     0,     0,     0,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,     0,     0,   133,   134,     0,     0,
       0,     0,   135,   336,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,     0,     0,     0,     0,   135,   382,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,     0,
       0,     0,     0,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
       0,   132,     0,     0,   133,   134,   287,     0,     0,     0,
     135,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,     0,     0,     0,     0,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,     0,   132,     0,     0,   133,   134,     0,
       0,     0,     0,   135,   104,   105,   106,   107,   108,   109,
       0,   111,   112,   113,   114,     0,     0,     0,     0,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,     0,     0,
     133,   134,     0,     0,     0,   332,   135,   104,   105,   106,
     107,   108,   109,     0,   111,   112,   113,   114,     0,     0,
       0,     0,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,     0,
     132,     0,     0,   133,   134,     0,     0,     0,   334,   135,
     104,   105,   106,   107,   108,   109,     0,   111,   112,   113,
     114,     0,     0,     0,     0,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,     0,     0,   133,   134,     0,     0,
       0,   359,   135,   104,   105,   106,   107,   108,   109,     0,
     111,   112,   113,   114,     0,     0,     0,     0,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,     0,   132,     0,     0,   133,
     134,     0,     0,     0,     0,   135,   110,   111,   112,   113,
     114,     0,     0,     0,     0,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,     0,     0,     0,   134,     0,     0,
       0,     0,   135,   110,   111,   112,   113,   114,     0,     0,
       0,     0,     0,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,     0,
     132,   110,   111,   112,   113,   114,     0,     0,     0,   135,
       0,     0,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   110,
     111,   112,   113,   114,     0,     0,     0,   135,     0,     0,
       0,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,     0,   132,   110,   111,   112,
     113,   114,     0,     0,     0,   135,     0,     0,     0,     0,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,     0,   132,   110,   111,   112,   113,   114,
       0,     0,     0,   135,     0,     0,     0,     0,     0,     0,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,     0,   132,     0,     0,     0,     0,     0,     0,     0,
       0,   135
};

static const yytype_int16 yycheck[] =
{
       5,     6,     7,     8,    22,    10,    26,    82,    13,    17,
     245,    35,     2,   186,     3,   175,     3,    37,   138,     3,
       3,     3,     3,    13,    49,    30,    16,    32,     3,    19,
      55,     3,    22,    23,    58,    49,    42,   252,    50,     3,
      50,    55,    48,    46,    54,    57,    52,    37,    32,    74,
      74,    38,    34,    36,    78,    38,    38,    38,     0,    48,
      74,    50,    34,    52,    82,    48,    54,    57,    32,    47,
     243,   306,   307,     3,    79,    80,   244,    55,    61,    48,
     253,    64,    48,    88,    50,    90,    52,    15,    16,    46,
      73,    48,    82,    76,    48,    52,   101,   312,   103,   104,
     105,   106,   107,   108,   109,   273,   266,   267,   113,   229,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,    48,   348,   349,   140,    49,   305,   143,   163,
     148,   146,    55,    49,    32,    33,    34,    35,   316,    55,
     365,    48,    49,    50,    49,    52,   161,    52,    48,     3,
      50,    74,    52,   331,    51,    48,   156,    54,    74,    52,
     175,     3,    54,    48,    48,    51,   181,    52,    52,    55,
      12,    13,    51,   351,   189,    35,    55,   262,    10,    11,
      12,    13,    14,    50,    54,    52,   201,   202,    30,    31,
      51,    54,    34,    54,    36,     3,    38,    29,    30,    31,
      32,    33,    34,    35,    46,    47,    48,    51,    54,    51,
      54,   226,    46,    55,    46,    54,    54,   232,     3,    61,
      56,   249,    64,    56,    50,     3,    67,   257,   258,    52,
      51,    73,     3,    40,    76,     3,    46,   252,     3,    53,
     255,    12,    13,    52,   259,    48,   261,     3,   263,   249,
      54,   266,   267,    51,    40,   270,    49,    53,    53,    30,
      31,    49,    53,    34,    10,    36,    10,    38,   283,     3,
      54,   356,   287,    54,    74,   290,    49,    48,    49,   294,
      49,     3,     3,    49,    55,     3,    49,    51,    40,    66,
      61,    49,    54,    64,    12,    13,    47,   312,    47,    54,
      72,    37,    73,    57,   155,    76,    19,   322,   297,    23,
      22,   188,    30,    31,    73,    82,    34,   332,    36,   334,
      38,   308,   162,   353,    42,   137,   101,   103,    -1,     3,
      48,   356,    -1,   348,   349,    53,    -1,    -1,    12,    13,
      -1,    -1,   372,    61,   359,    -1,    64,   362,    -1,   364,
     365,   381,    -1,    -1,    -1,    73,    30,    31,    76,    -1,
      34,    -1,    36,    -1,    38,    -1,    -1,    -1,   383,    -1,
      -1,   386,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    67,    68,    69,    70,    71,    72,    73,
       3,    -1,    76,    77,    78,    -1,    -1,    -1,    -1,    12,
      13,    -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    30,    31,    -1,
      -1,    34,    -1,    36,    -1,    38,    30,    31,    32,    33,
      34,    35,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,
      -1,    54,    46,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    -1,    67,    68,    69,    70,    71,    72,
      73,     3,    -1,    76,    77,    78,    -1,    -1,    -1,    -1,
      12,    13,    -1,    -1,    30,    31,    32,    33,    34,    35,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,    31,
      46,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    54,    -1,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    -1,    67,    68,    69,    70,    71,
      72,    73,     3,    -1,    76,    77,    78,    -1,    -1,    -1,
      -1,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    30,
      31,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    54,    -1,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    -1,    67,    68,    69,    70,
      71,    72,    73,     3,    -1,    76,    77,    78,    -1,    -1,
      -1,    -1,    12,    13,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      30,    31,    -1,    -1,    34,    -1,    36,    -1,    38,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,     3,    48,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    12,    13,    -1,    -1,
      -1,    61,    -1,    -1,    64,    65,    -1,    67,    68,    69,
      70,    71,    72,    73,    30,    31,    76,    -1,    34,     3,
      36,    -1,    38,    -1,    -1,    -1,    -1,    -1,    12,    13,
      -1,    -1,    48,    49,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    61,    30,    31,    64,    -1,
      34,    -1,    36,    -1,    38,    -1,    -1,    73,    -1,    -1,
      76,    -1,    -1,     3,    48,    -1,    -1,    -1,    -1,    -1,
      54,    -1,    12,    13,    -1,    -1,    -1,    61,    -1,    -1,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,
      30,    31,    76,    -1,    34,     3,    36,    -1,    38,    -1,
      -1,    -1,    42,    -1,    12,    13,    -1,    -1,    48,    -1,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    30,    31,    64,    -1,    34,    -1,    36,    -1,
      38,    -1,    -1,    73,    -1,    -1,    76,    -1,    -1,     3,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    12,    13,
       3,    -1,    -1,    61,    -1,    -1,    64,    -1,    -1,    12,
      13,    -1,    -1,    -1,    -1,    73,    30,    31,    76,    -1,
      34,    -1,    36,    -1,    38,    -1,    -1,    30,    31,    -1,
      -1,    34,    -1,    36,    48,    38,    -1,    -1,    -1,    53,
      -1,    -1,    -1,    46,    -1,    48,    -1,    61,    -1,    -1,
      64,    -1,    -1,    -1,     3,    -1,    -1,    -1,    61,    73,
      -1,    64,    76,    12,    13,     3,    -1,    -1,    17,    -1,
      73,    -1,    -1,    76,    12,    13,    -1,    -1,    -1,    -1,
      -1,    30,    31,    -1,    -1,    34,    -1,    36,    -1,    38,
      -1,    -1,    30,    31,    -1,    -1,    34,    -1,    36,    48,
      38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      48,    -1,    61,    -1,    -1,    64,    -1,    -1,    12,    13,
      -1,    -1,    -1,    61,    73,    -1,    64,    76,    10,    11,
      12,    13,    14,    -1,    -1,    73,    30,    31,    76,    -1,
      34,    -1,    36,    -1,    38,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    48,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    61,    -1,    -1,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,
      -1,    -1,    76,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    -1,    -1,    40,
      41,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    54,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    53,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    53,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    53,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    53,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,
      -1,    51,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    -1,    -1,    40,    41,
      -1,    -1,    -1,    -1,    46,    47,    -1,    -1,    -1,    51,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,    -1,    51,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,    -1,
      46,    47,    -1,    -1,    -1,    51,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      -1,    -1,    40,    41,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,    -1,    51,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,
      -1,    51,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    -1,    -1,    40,    41,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    51,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    49,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      -1,    -1,    40,    41,    -1,    -1,    -1,    -1,    46,    -1,
      -1,    49,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    -1,    -1,    40,    41,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    49,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    40,    41,    -1,    -1,    -1,    -1,
      46,    -1,    -1,    49,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    -1,    46,    -1,    -1,    49,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,
      -1,    -1,    46,    47,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    -1,    46,    47,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      46,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,
      -1,    -1,    -1,    46,     4,     5,     6,     7,     8,     9,
      -1,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    -1,    37,    -1,    -1,
      40,    41,    -1,    -1,    -1,    45,    46,     4,     5,     6,
       7,     8,     9,    -1,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,    46,
       4,     5,     6,     7,     8,     9,    -1,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    40,    41,    -1,    -1,
      -1,    45,    46,     4,     5,     6,     7,     8,     9,    -1,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    -1,    -1,    40,
      41,    -1,    -1,    -1,    -1,    46,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    -1,    46,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    10,    11,    12,    13,    14,    -1,    -1,    -1,    46,
      -1,    -1,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    -1,    37,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    -1,    37,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    82,    83,     0,     3,    12,    13,    30,    31,    34,
      36,    38,    46,    48,    54,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    67,    68,    69,    70,    71,
      72,    73,    76,    77,    78,    85,    86,    87,    88,    94,
      95,    96,    97,   104,   112,   113,   115,   119,   125,   126,
     127,    85,   119,   119,   119,   119,   119,    84,    85,   119,
       3,    38,    91,    92,    93,    85,    92,     3,    38,     3,
      85,    91,    46,     3,    85,    94,    96,    85,    97,    48,
      48,   125,    48,    54,    54,    54,   119,   119,    48,    50,
      52,    98,    77,    78,    88,   125,    54,     3,   100,   101,
     102,    48,    50,    52,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    37,    40,    41,    46,   120,   121,   122,    54,
      52,    47,    86,    49,    98,    49,    51,     3,    51,    54,
      54,    54,    54,    54,    57,    56,    83,   104,    52,     3,
      48,    52,    98,    99,   119,   119,    67,    95,    96,   126,
     128,   131,    54,     3,    49,    55,   116,   117,   118,   119,
       3,    42,    53,   114,   119,    52,    48,    98,    51,    40,
      49,   117,     3,   114,   119,   119,   119,   119,   119,   119,
     119,    15,    16,   123,   124,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   119,   119,   119,   119,    18,   119,   119,   122,
     121,   119,    49,   119,     3,    92,     3,    32,    89,    90,
      32,    90,    47,    48,    49,    55,    74,   108,   109,   110,
     111,   119,    46,    48,   105,    52,    98,    49,    49,    48,
       3,    54,    51,    40,   116,    49,    51,    55,   119,    53,
      42,    53,    53,    49,   108,   101,   103,   105,   119,    49,
      53,    53,   119,    17,   119,    10,    10,    42,   119,    47,
      51,   121,   119,    49,    51,     3,    54,    51,    54,    54,
      54,    49,   108,   127,   111,    49,    51,    55,    96,    53,
      47,    51,    55,   103,   106,   107,    49,   108,   119,   125,
     125,   119,    42,   119,   129,   126,   119,   116,   116,   119,
     127,    49,    45,   119,    45,   119,    47,   119,   119,     3,
      89,    49,   127,   111,   111,   102,   103,    47,    55,    51,
     127,    49,    53,    66,    49,   119,    54,   127,   119,    45,
     119,    47,    51,    49,    51,    40,    47,   103,   103,   127,
     125,    54,    49,   130,   131,   119,   119,   119,   103,    47,
     125,    49,    47,    51,   125,   119,    51,   119,    49
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
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
  int yytoken;
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

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1455 of yacc.c  */
#line 177 "camp.y"
    { absyntax::root = (yyvsp[(1) - (1)].b); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 181 "camp.y"
    { (yyval.b) = new file(lexerPos(), false); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 183 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 187 "camp.y"
    { (yyval.b) = new block(lexerPos(), true); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 189 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 193 "camp.y"
    { (yyval.n) = new simpleName((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 194 "camp.y"
    { (yyval.n) = new qualifiedName((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 195 "camp.y"
    { (yyval.n) = new simpleName((yyvsp[(1) - (1)].ps).pos,
                                  symbol::trans("operator answer")); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 200 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 201 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 203 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].d)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 205 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].s)); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 209 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].mod).pos); (yyval.ml)->add((yyvsp[(1) - (1)].mod).val); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 210 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].perm).pos); (yyval.ml)->add((yyvsp[(1) - (1)].perm).val); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 212 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].mod).val); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 214 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].perm).val); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 218 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].vd); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 219 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 220 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 222 "camp.y"
    { (yyval.d) = new accessdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ipl)); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 224 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), (yyvsp[(4) - (5)].ipl)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 226 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), WILDCARD); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 227 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].n), WILDCARD); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 229 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, (yyvsp[(4) - (5)].ipl)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 231 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, WILDCARD); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 233 "camp.y"
    { (yyval.d) = new importdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ip)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 234 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 236 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (string)*(yyvsp[(2) - (3)].ps).sym); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 240 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 242 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 246 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 248 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 252 "camp.y"
    { (yyval.ps) = (yyvsp[(1) - (1)].ps); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 253 "camp.y"
    { (yyval.ps) = (yyvsp[(1) - (1)].ps); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 257 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 259 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 263 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 265 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 269 "camp.y"
    { (yyval.vd) = (yyvsp[(1) - (2)].vd); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 273 "camp.y"
    { (yyval.vd) = new vardec((yyvsp[(1) - (2)].t)->getPos(), (yyvsp[(1) - (2)].t), (yyvsp[(2) - (2)].dil)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 277 "camp.y"
    { (yyval.t) = (yyvsp[(1) - (1)].t); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 278 "camp.y"
    { (yyval.t) = new arrayTy((yyvsp[(1) - (2)].n), (yyvsp[(2) - (2)].dim)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 282 "camp.y"
    { (yyval.t) = new nameTy((yyvsp[(1) - (1)].n)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 286 "camp.y"
    { (yyval.dim) = new dimensions((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 287 "camp.y"
    { (yyval.dim) = (yyvsp[(1) - (3)].dim); (yyval.dim)->increase(); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 291 "camp.y"
    { (yyval.elist) = new explist((yyvsp[(1) - (3)].pos)); (yyval.elist)->add((yyvsp[(2) - (3)].e)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 293 "camp.y"
    { (yyval.elist) = (yyvsp[(1) - (4)].elist); (yyval.elist)->add((yyvsp[(3) - (4)].e)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 297 "camp.y"
    { (yyval.dil) = new decidlist((yyvsp[(1) - (1)].di)->getPos()); (yyval.dil)->add((yyvsp[(1) - (1)].di)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 299 "camp.y"
    { (yyval.dil) = (yyvsp[(1) - (3)].dil); (yyval.dil)->add((yyvsp[(3) - (3)].di)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 303 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (1)].dis)->getPos(), (yyvsp[(1) - (1)].dis)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 305 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (3)].dis)->getPos(), (yyvsp[(1) - (3)].dis), (yyvsp[(3) - (3)].vi)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 309 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 310 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (2)].ps).pos, (yyvsp[(1) - (2)].ps).sym, (yyvsp[(2) - (2)].dim)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 311 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, 0,
                                            new formals((yyvsp[(2) - (3)].pos))); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 314 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (4)].ps).pos, (yyvsp[(1) - (4)].ps).sym, 0, (yyvsp[(3) - (4)].fls)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 318 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 319 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].ai); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 324 "camp.y"
    { (yyval.b) = (yyvsp[(2) - (3)].b); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 328 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 330 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (4)].pos)); (yyval.ai)->addRest((yyvsp[(3) - (4)].vi)); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 332 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (3)].ai); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 334 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (5)].ai); (yyval.ai)->addRest((yyvsp[(4) - (5)].vi)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 338 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 339 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (1)].ai); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 340 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (2)].ai); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 344 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].vi)->getPos());
		     (yyval.ai)->add((yyvsp[(1) - (1)].vi));;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 347 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (3)].ai); (yyval.ai)->add((yyvsp[(3) - (3)].vi)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 351 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (1)].fls); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 353 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->addRest((yyvsp[(3) - (3)].fl)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 354 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (2)].pos)); (yyval.fls)->addRest((yyvsp[(2) - (2)].fl)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 358 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (1)].fl)->getPos()); (yyval.fls)->add((yyvsp[(1) - (1)].fl)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 360 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->add((yyvsp[(3) - (3)].fl)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 364 "camp.y"
    { (yyval.boo) = true; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 365 "camp.y"
    { (yyval.boo) = false; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 370 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (2)].t)->getPos(), (yyvsp[(2) - (2)].t), 0, 0, (yyvsp[(1) - (2)].boo)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 372 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (3)].t)->getPos(), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].dis), 0, (yyvsp[(1) - (3)].boo)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 374 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (5)].t)->getPos(), (yyvsp[(2) - (5)].t), (yyvsp[(3) - (5)].dis), (yyvsp[(5) - (5)].vi), (yyvsp[(1) - (5)].boo)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 379 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (5)].pos), (yyvsp[(1) - (5)].t), (yyvsp[(2) - (5)].ps).sym, new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 381 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (6)].pos), (yyvsp[(1) - (6)].t), (yyvsp[(2) - (6)].ps).sym, (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 385 "camp.y"
    { (yyval.d) = new recorddec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].b)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 386 "camp.y"
    { (yyval.d) = new typedec((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].vd)); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 390 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (1)].pos), 0, 0); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 391 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (2)].pos), (yyvsp[(1) - (2)].e), 0); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 392 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (2)].pos), 0, (yyvsp[(2) - (2)].e)); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 393 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 397 "camp.y"
    { (yyval.e) = new fieldExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 398 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].e)); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 400 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].e)); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 401 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].slice)); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 403 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].slice)); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 404 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos),
                                      new nameExp((yyvsp[(1) - (3)].n)->getPos(), (yyvsp[(1) - (3)].n)),
                                      new arglist()); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 408 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), 
                                      new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)),
                                      (yyvsp[(3) - (4)].alist)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 411 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), new arglist()); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 413 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].alist)); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 415 "camp.y"
    { (yyval.e) = (yyvsp[(2) - (3)].e); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 417 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(2) - (3)].n)->getPos(), (yyvsp[(2) - (3)].n)); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 418 "camp.y"
    { (yyval.e) = new thisExp((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 422 "camp.y"
    { (yyval.arg).name=0;      (yyval.arg).val=(yyvsp[(1) - (1)].e); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 423 "camp.y"
    { (yyval.arg).name=(yyvsp[(1) - (3)].ps).sym; (yyval.arg).val=(yyvsp[(3) - (3)].e); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 427 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (1)].alist); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 429 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->rest = (yyvsp[(3) - (3)].arg); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 430 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->rest = (yyvsp[(2) - (2)].arg); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 434 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->add((yyvsp[(1) - (1)].arg)); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 436 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->add((yyvsp[(3) - (3)].arg)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 441 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(1) - (1)].n)->getPos(), (yyvsp[(1) - (1)].n)); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 442 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 443 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 444 "camp.y"
    { (yyval.e) = new stringExp((yyvsp[(1) - (1)].ps).pos, *(yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 446 "camp.y"
    { (yyval.e) = new scaleExp((yyvsp[(1) - (2)].e)->getPos(), (yyvsp[(1) - (2)].e), (yyvsp[(2) - (2)].e)); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 448 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (4)].n)->getPos(), new nameTy((yyvsp[(2) - (4)].n)), (yyvsp[(4) - (4)].e)); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 450 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (5)].n)->getPos(), new arrayTy((yyvsp[(2) - (5)].n), (yyvsp[(3) - (5)].dim)), (yyvsp[(5) - (5)].e)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 452 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 454 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 455 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 456 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 457 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 458 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 459 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 460 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 461 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 462 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 463 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 464 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 465 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 466 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 467 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 468 "camp.y"
    { (yyval.e) = new andExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 469 "camp.y"
    { (yyval.e) = new orExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 470 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 471 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 472 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 473 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 474 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 476 "camp.y"
    { (yyval.e) = new newRecordExp((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].t)); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 478 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].elist), 0, 0); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 480 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), (yyvsp[(3) - (4)].elist), (yyvsp[(4) - (4)].dim), 0); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 482 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), 0, (yyvsp[(3) - (3)].dim), 0); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 484 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), 0, (yyvsp[(3) - (4)].dim), (yyvsp[(4) - (4)].ai)); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 486 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].t), new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 488 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos),
                                             new arrayTy((yyvsp[(2) - (6)].t)->getPos(), (yyvsp[(2) - (6)].t), (yyvsp[(3) - (6)].dim)),
                                             new formals((yyvsp[(4) - (6)].pos)),
                                             (yyvsp[(6) - (6)].s)); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 493 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos), (yyvsp[(2) - (6)].t), (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 495 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (7)].pos),
                                             new arrayTy((yyvsp[(2) - (7)].t)->getPos(), (yyvsp[(2) - (7)].t), (yyvsp[(3) - (7)].dim)),
                                             (yyvsp[(5) - (7)].fls),
                                             (yyvsp[(7) - (7)].s)); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 500 "camp.y"
    { (yyval.e) = new conditionalExp((yyvsp[(2) - (5)].pos), (yyvsp[(1) - (5)].e), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].e)); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 501 "camp.y"
    { (yyval.e) = new assignExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 504 "camp.y"
    { (yyval.e) = new pairExp((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].e)); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 506 "camp.y"
    { (yyval.e) = new tripleExp((yyvsp[(1) - (7)].pos), (yyvsp[(2) - (7)].e), (yyvsp[(4) - (7)].e), (yyvsp[(6) - (7)].e)); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 508 "camp.y"
    { (yyval.e) = new transformExp((yyvsp[(1) - (13)].pos), (yyvsp[(2) - (13)].e), (yyvsp[(4) - (13)].e), (yyvsp[(6) - (13)].e), (yyvsp[(8) - (13)].e), (yyvsp[(10) - (13)].e), (yyvsp[(12) - (13)].e)); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 510 "camp.y"
    { (yyvsp[(2) - (3)].j)->pushFront((yyvsp[(1) - (3)].e)); (yyvsp[(2) - (3)].j)->pushBack((yyvsp[(3) - (3)].e)); (yyval.e) = (yyvsp[(2) - (3)].j); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 512 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::OUT);
                     joinExp *jexp =
                         new joinExp((yyvsp[(2) - (2)].se)->getPos(), symbol::trans(".."));
                     (yyval.e)=jexp;
                     jexp->pushBack((yyvsp[(1) - (2)].e)); jexp->pushBack((yyvsp[(2) - (2)].se)); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 518 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), symbol::trans("+")); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 520 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), symbol::trans("-")); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 523 "camp.y"
    { (yyval.e) = new postfixExp((yyvsp[(2) - (2)].ps).pos, (yyvsp[(1) - (2)].e), symbol::trans("+")); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 524 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 526 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 527 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 528 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 529 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 531 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 533 "camp.y"
    { (yyval.e) = new quoteExp((yyvsp[(1) - (4)].pos), (yyvsp[(3) - (4)].b)); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 539 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos,(yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 541 "camp.y"
    { (yyval.j) = (yyvsp[(1) - (1)].j); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 543 "camp.y"
    { (yyvsp[(1) - (2)].se)->setSide(camp::OUT);
                     (yyval.j) = (yyvsp[(2) - (2)].j); (yyval.j)->pushFront((yyvsp[(1) - (2)].se)); ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 546 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(1) - (2)].j); (yyval.j)->pushBack((yyvsp[(2) - (2)].se)); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 549 "camp.y"
    { (yyvsp[(1) - (3)].se)->setSide(camp::OUT); (yyvsp[(3) - (3)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(2) - (3)].j); (yyval.j)->pushFront((yyvsp[(1) - (3)].se)); (yyval.j)->pushBack((yyvsp[(3) - (3)].se)); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 554 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(2) - (4)].ps).pos, (yyvsp[(2) - (4)].ps).sym, (yyvsp[(3) - (4)].e)); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 555 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (3)].pos), symbol::opTrans("spec"), (yyvsp[(2) - (3)].e)); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 557 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (5)].pos), symbol::opTrans("spec"),
				      new pairExp((yyvsp[(3) - (5)].pos), (yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].e))); ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 560 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (7)].pos), symbol::opTrans("spec"),
				      new tripleExp((yyvsp[(3) - (7)].pos), (yyvsp[(2) - (7)].e), (yyvsp[(4) - (7)].e), (yyvsp[(6) - (7)].e))); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 565 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 567 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 569 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 570 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 571 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 575 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym,
                              new booleanExp((yyvsp[(1) - (2)].ps).pos, false)); ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 578 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e),
                              new booleanExp((yyvsp[(1) - (4)].ps).pos, false)); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 581 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(3) - (3)].e), (yyvsp[(1) - (3)].ps).sym,
                              new booleanExp((yyvsp[(2) - (3)].ps).pos, true)); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 584 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (5)].ps).pos, (yyvsp[(3) - (5)].e), (yyvsp[(1) - (5)].ps).sym, (yyvsp[(5) - (5)].e),
                              new booleanExp((yyvsp[(2) - (5)].ps).pos, true)); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 589 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 591 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e)); ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 595 "camp.y"
    { (yyval.s) = new emptyStm((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 596 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 597 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (2)].s); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 599 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 601 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (7)].pos), (yyvsp[(3) - (7)].e), (yyvsp[(5) - (7)].s), (yyvsp[(7) - (7)].s)); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 603 "camp.y"
    { (yyval.s) = new whileStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 605 "camp.y"
    { (yyval.s) = new doStm((yyvsp[(1) - (7)].pos), (yyvsp[(2) - (7)].s), (yyvsp[(5) - (7)].e)); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 607 "camp.y"
    { (yyval.s) = new forStm((yyvsp[(1) - (9)].pos), (yyvsp[(3) - (9)].run), (yyvsp[(5) - (9)].e), (yyvsp[(7) - (9)].sel), (yyvsp[(9) - (9)].s)); ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 609 "camp.y"
    { (yyval.s) = new extendedForStm((yyvsp[(1) - (8)].pos), (yyvsp[(3) - (8)].t), (yyvsp[(4) - (8)].ps).sym, (yyvsp[(6) - (8)].e), (yyvsp[(8) - (8)].s)); ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 610 "camp.y"
    { (yyval.s) = new breakStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 611 "camp.y"
    { (yyval.s) = new continueStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 612 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 613 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].e)); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 617 "camp.y"
    { (yyval.s) = new expStm((yyvsp[(1) - (1)].e)->getPos(), (yyvsp[(1) - (1)].e)); ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 621 "camp.y"
    { (yyval.s) = new blockStm((yyvsp[(1) - (1)].b)->getPos(), (yyvsp[(1) - (1)].b)); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 625 "camp.y"
    { (yyval.run) = 0; ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 626 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].sel); ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 627 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].vd); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 631 "camp.y"
    { (yyval.e) = 0; ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 632 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 636 "camp.y"
    { (yyval.sel) = 0; ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 637 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (1)].sel); ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 641 "camp.y"
    { (yyval.sel) = new stmExpList((yyvsp[(1) - (1)].s)->getPos()); (yyval.sel)->add((yyvsp[(1) - (1)].s)); ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 643 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (3)].sel); (yyval.sel)->add((yyvsp[(3) - (3)].s)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 3687 "camp.tab.c"
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
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



