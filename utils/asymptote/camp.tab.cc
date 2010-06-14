
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
#include "opsymbols.h"

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
#line 120 "camp.tab.c"

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

/* Line 214 of yacc.c  */
#line 47 "camp.y"

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



/* Line 214 of yacc.c  */
#line 263 "camp.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 275 "camp.tab.c"

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
#define YYLAST   2174

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  199
/* YYNRULES -- Number of states.  */
#define YYNSTATES  379

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    29,     2,     2,
      42,    43,    27,    25,    45,    26,    44,    28,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    36,    48,
       2,     2,     2,    35,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    46,     2,    47,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    40,     2,    41,     2,     2,     2,     2,
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
      31,    32,    33,    34,    37,    38,    39,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75
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
     547,   550,   553,   556,   559,   563,   568,   570,   572,   575,
     578,   582,   587,   591,   597,   605,   607,   611,   615,   617,
     619,   622,   627,   631,   637,   640,   645,   647,   649,   652,
     658,   666,   672,   680,   690,   699,   702,   705,   708,   712,
     714,   716,   717,   719,   721,   722,   724,   725,   727,   729
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      77,     0,    -1,    78,    -1,    -1,    78,    81,    -1,    -1,
      79,    81,    -1,     3,    -1,    80,    44,     3,    -1,    29,
      -1,    83,    -1,   120,    -1,    82,    83,    -1,    82,   120,
      -1,    73,    -1,    72,    -1,    82,    73,    -1,    82,    72,
      -1,    89,    -1,   107,    -1,   108,    -1,    50,    88,    48,
      -1,    54,    80,    51,    85,    48,    -1,    54,    80,    51,
      27,    48,    -1,    51,    80,    48,    -1,    54,    86,    50,
      85,    48,    -1,    54,    86,    50,    27,    48,    -1,    52,
      87,    48,    -1,    53,     3,    48,    -1,    53,    71,    48,
      -1,     3,    -1,     3,     3,     3,    -1,    84,    -1,    85,
      45,    84,    -1,     3,    -1,    71,    -1,     3,    -1,    86,
       3,     3,    -1,    87,    -1,    88,    45,    87,    -1,    90,
      48,    -1,    91,    95,    -1,    92,    -1,    80,    93,    -1,
      80,    -1,    46,    47,    -1,    93,    46,    47,    -1,    46,
     114,    47,    -1,    94,    46,   114,    47,    -1,    96,    -1,
      95,    45,    96,    -1,    97,    -1,    97,    34,    98,    -1,
       3,    -1,     3,    93,    -1,     3,    42,    43,    -1,     3,
      42,   103,    43,    -1,   114,    -1,   100,    -1,    40,    79,
      41,    -1,    40,    41,    -1,    40,    49,    98,    41,    -1,
      40,   101,    41,    -1,    40,   101,    49,    98,    41,    -1,
      45,    -1,   102,    -1,   102,    45,    -1,    98,    -1,   102,
      45,    98,    -1,   104,    -1,   104,    49,   106,    -1,    49,
     106,    -1,   106,    -1,   104,    45,   106,    -1,    68,    -1,
      -1,   105,    91,    -1,   105,    91,    97,    -1,   105,    91,
      97,    34,    98,    -1,    91,     3,    42,    43,   122,    -1,
      91,     3,    42,   103,    43,   122,    -1,    56,     3,    99,
      -1,    57,    89,    -1,    36,    -1,   114,    36,    -1,    36,
     114,    -1,   114,    36,   114,    -1,   110,    44,     3,    -1,
      80,    46,   114,    47,    -1,   110,    46,   114,    47,    -1,
      80,    46,   109,    47,    -1,   110,    46,   109,    47,    -1,
      80,    42,    43,    -1,    80,    42,   112,    43,    -1,   110,
      42,    43,    -1,   110,    42,   112,    43,    -1,    42,   114,
      43,    -1,    42,    80,    43,    -1,    67,    -1,   114,    -1,
       3,    34,   114,    -1,   113,    -1,   113,    49,   111,    -1,
      49,   111,    -1,   111,    -1,   113,    45,   111,    -1,    80,
      -1,   110,    -1,    70,    -1,    71,    -1,    70,   114,    -1,
      42,    80,    43,   114,    -1,    42,    80,    93,    43,   114,
      -1,    25,   114,    -1,    26,   114,    -1,    31,   114,    -1,
     114,    25,   114,    -1,   114,    26,   114,    -1,   114,    27,
     114,    -1,   114,    28,   114,    -1,   114,    29,   114,    -1,
     114,    30,   114,    -1,   114,    20,   114,    -1,   114,    21,
     114,    -1,   114,    22,   114,    -1,   114,    23,   114,    -1,
     114,    18,   114,    -1,   114,    19,   114,    -1,   114,    15,
     114,    -1,   114,    14,   114,    -1,   114,    24,   114,    -1,
     114,    17,   114,    -1,   114,    16,   114,    -1,   114,    32,
     114,    -1,   114,     8,   114,    -1,    58,    92,    -1,    58,
      92,    94,    -1,    58,    92,    94,    93,    -1,    58,    92,
      93,    -1,    58,    92,    93,   100,    -1,    58,    92,    42,
      43,   122,    -1,    58,    92,    93,    42,    43,   122,    -1,
      58,    92,    42,   103,    43,   122,    -1,    58,    92,    93,
      42,   103,    43,   122,    -1,   114,    35,   114,    36,   114,
      -1,   114,    34,   114,    -1,    42,   114,    45,   114,    43,
      -1,    42,   114,    45,   114,    45,   114,    43,    -1,    42,
     114,    45,   114,    45,   114,    45,   114,    45,   114,    45,
     114,    43,    -1,   114,   115,   114,    -1,   114,   116,    -1,
       8,   114,    -1,     7,   114,    -1,   114,     8,    -1,   114,
       4,   114,    -1,    55,    40,    78,    41,    -1,     7,    -1,
     117,    -1,   116,   117,    -1,   117,   116,    -1,   116,   117,
     116,    -1,    40,    13,   114,    41,    -1,    40,   114,    41,
      -1,    40,   114,    45,   114,    41,    -1,    40,   114,    45,
     114,    45,   114,    41,    -1,     5,    -1,     5,   118,     5,
      -1,     5,   119,     5,    -1,     6,    -1,     9,    -1,    11,
     114,    -1,    11,   114,    39,   114,    -1,    11,    12,   114,
      -1,    11,    12,   114,    39,   114,    -1,    10,   114,    -1,
      10,   114,    39,   114,    -1,    48,    -1,   122,    -1,   121,
      48,    -1,    59,    42,   114,    43,   120,    -1,    59,    42,
     114,    43,   120,    60,   120,    -1,    61,    42,   114,    43,
     120,    -1,    62,   120,    61,    42,   114,    43,    48,    -1,
      63,    42,   123,    48,   124,    48,   125,    43,   120,    -1,
      63,    42,    91,     3,    36,   114,    43,   120,    -1,    64,
      48,    -1,    65,    48,    -1,    66,    48,    -1,    66,   114,
      48,    -1,   114,    -1,    99,    -1,    -1,   126,    -1,    90,
      -1,    -1,   114,    -1,    -1,   126,    -1,   121,    -1,   126,
      45,   121,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   179,   179,   183,   184,   189,   190,   195,   196,   197,
     202,   203,   204,   206,   211,   212,   213,   215,   220,   221,
     222,   223,   225,   227,   229,   230,   232,   234,   236,   237,
     242,   244,   248,   249,   254,   255,   260,   262,   266,   267,
     272,   276,   280,   281,   285,   289,   290,   294,   295,   300,
     301,   306,   307,   312,   313,   314,   316,   321,   322,   326,
     331,   332,   334,   336,   341,   342,   343,   347,   349,   354,
     355,   357,   361,   362,   367,   368,   372,   374,   376,   381,
     383,   388,   389,   393,   394,   395,   396,   400,   401,   403,
     404,   406,   407,   410,   414,   415,   417,   419,   421,   425,
     426,   430,   431,   433,   437,   438,   444,   445,   446,   447,
     449,   450,   452,   454,   456,   458,   459,   460,   461,   462,
     463,   464,   465,   466,   467,   468,   469,   470,   471,   472,
     473,   474,   475,   476,   477,   478,   480,   482,   484,   486,
     488,   490,   495,   497,   502,   504,   506,   508,   510,   512,
     514,   520,   522,   525,   527,   528,   535,   536,   538,   541,
     544,   550,   551,   552,   555,   561,   562,   564,   566,   567,
     571,   573,   576,   579,   585,   586,   591,   592,   593,   594,
     596,   598,   600,   602,   604,   606,   607,   608,   609,   613,
     617,   621,   622,   623,   627,   628,   632,   633,   637,   638
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "SELFOP", "DOTS", "COLONS",
  "DASHES", "INCR", "LONGDASH", "CONTROLS", "TENSION", "ATLEAST", "CURL",
  "COR", "CAND", "BAR", "AMPERSAND", "EQ", "NEQ", "LT", "LE", "GT", "GE",
  "CARETS", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "LOGNOT", "OPERATOR",
  "LOOSE", "ASSIGN", "'?'", "':'", "DIRTAG", "JOIN_PREC", "AND", "'{'",
  "'}'", "'('", "')'", "'.'", "','", "'['", "']'", "';'", "ELLIPSIS",
  "ACCESS", "UNRAVEL", "IMPORT", "INCLUDE", "FROM", "QUOTE", "STRUCT",
  "TYPEDEF", "NEW", "IF", "ELSE", "WHILE", "DO", "FOR", "BREAK",
  "CONTINUE", "RETURN_", "THIS", "EXPLICIT", "GARBAGE", "LIT", "STRING",
  "PERM", "MODIFIER", "UNARY", "EXP_IN_PARENS_RULE", "$accept", "file",
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
     275,   276,   277,   278,   279,    43,    45,    42,    47,    37,
      94,   280,   281,   282,   283,    63,    58,   284,   285,   286,
     123,   125,    40,    41,    46,    44,    91,    93,    59,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    78,    78,    79,    79,    80,    80,    80,
      81,    81,    81,    81,    82,    82,    82,    82,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      84,    84,    85,    85,    86,    86,    87,    87,    88,    88,
      89,    90,    91,    91,    92,    93,    93,    94,    94,    95,
      95,    96,    96,    97,    97,    97,    97,    98,    98,    99,
     100,   100,   100,   100,   101,   101,   101,   102,   102,   103,
     103,   103,   104,   104,   105,   105,   106,   106,   106,   107,
     107,   108,   108,   109,   109,   109,   109,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   111,
     111,   112,   112,   112,   113,   113,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   115,   115,   115,   115,
     115,   116,   116,   116,   116,   117,   117,   117,   117,   117,
     118,   118,   118,   118,   119,   119,   120,   120,   120,   120,
     120,   120,   120,   120,   120,   120,   120,   120,   120,   121,
     122,   123,   123,   123,   124,   124,   125,   125,   126,   126
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
       2,     2,     2,     2,     3,     4,     1,     1,     2,     2,
       3,     4,     3,     5,     7,     1,     3,     3,     1,     1,
       2,     4,     3,     5,     2,     4,     1,     1,     2,     5,
       7,     5,     7,     9,     8,     2,     2,     2,     3,     1,
       1,     0,     1,     1,     0,     1,     0,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     7,     0,     0,     0,     0,     9,
       0,     5,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      98,   108,   109,    15,    14,   106,     4,     0,    10,    18,
       0,     0,    42,   190,    19,    20,   107,   189,    11,     0,
     177,   106,   152,   151,   113,   114,   115,     0,   106,     0,
      36,    35,     0,    38,     0,     0,     0,     0,     0,     7,
       0,     0,     3,     0,    44,    82,     0,    44,   135,     0,
       0,     0,   191,   185,   186,   187,     0,   110,     0,     0,
       0,    43,    17,    16,    12,    13,    40,    53,    41,    49,
      51,     0,     0,     0,     0,   165,   168,   156,   153,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   150,   157,   178,     0,    59,     6,    97,     0,
      96,     0,     0,     0,    21,    24,    27,    28,    29,     0,
       0,     0,    81,     0,    53,    75,     0,   138,   136,     0,
       0,     0,   193,     0,   198,     0,   192,   188,     7,    92,
       0,   104,     0,   101,    99,     8,    83,    45,     0,     0,
       0,    75,    54,     0,     0,    94,     0,    87,     0,     0,
     154,     0,     0,     0,     0,   134,   129,   128,   132,   131,
     126,   127,   122,   123,   124,   125,   130,   116,   117,   118,
     119,   120,   121,   133,   145,     0,     0,     0,   149,   158,
     159,   111,     0,     0,    37,    39,    30,     0,    32,     0,
       0,     0,   155,    75,     0,    75,    74,     0,    69,     0,
      72,     0,     0,    75,   139,     0,   137,     0,     0,     0,
      53,   194,     0,     0,   103,    93,     0,     0,    85,    90,
      84,    88,    46,    55,     0,    50,    52,    58,    57,    95,
      91,    89,   174,     0,   170,   166,   167,     0,     0,   162,
       0,   160,   112,   146,     0,     0,    23,     0,    22,    26,
      25,    55,     0,   140,    71,     0,    75,    75,    76,    47,
      60,    64,     0,    67,     0,    65,     0,     0,     0,   179,
     181,     0,     0,   195,     0,   199,   100,   105,   102,    86,
      79,    56,     0,   172,     0,   144,   161,     0,     0,    31,
      33,    56,   142,    73,    70,    77,     0,    62,     0,    66,
     141,     0,    48,     0,     0,     0,   196,    80,   175,     0,
     171,   163,     0,   147,     0,     0,    61,     0,    68,   143,
     180,   182,     0,     0,   197,   173,     0,     0,    78,    63,
     184,     0,   164,     0,   183,     0,     0,     0,   148
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    57,    51,    36,    37,    38,   228,   229,
      62,    63,    64,    39,    40,    41,    42,   182,   158,    98,
      99,   100,   266,    43,   267,   304,   305,   237,   238,   239,
     240,    44,    45,   178,    46,   171,   172,   173,    47,   131,
     132,   133,   193,   194,    48,    49,    50,   165,   314,   363,
     166
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -208
static const yytype_int16 yypact[] =
{
    -208,    23,   474,  -208,  -208,   913,   913,   913,   913,  -208,
     913,  -208,   913,  -208,    17,    12,    17,    18,    16,   -27,
      69,    12,    12,    42,    44,   616,    62,    79,    80,   733,
    -208,     6,  -208,  -208,  -208,    36,  -208,   545,  -208,  -208,
      90,   146,  -208,  -208,  -208,  -208,   100,  1804,  -208,   109,
    -208,   132,   128,   128,   128,   128,  -208,   332,   110,  1197,
     164,  -208,   166,  -208,   102,   115,   134,   145,   155,   133,
       0,   154,  -208,   169,    -6,  -208,   205,   175,   118,   913,
     913,   160,   913,  -208,  -208,  -208,  1012,   128,   235,   221,
     659,   179,  -208,  -208,  -208,  -208,  -208,   124,   181,  -208,
     193,   690,   227,   743,   913,    83,  -208,  -208,     6,  -208,
     913,   913,   913,   913,   913,   913,   913,   913,   913,   913,
     913,   913,   913,   913,   913,   913,   913,   913,   913,   913,
     786,   913,  -208,   191,  -208,   743,  -208,  -208,     6,   149,
    -208,   913,   230,    17,  -208,  -208,  -208,  -208,  -208,    39,
      43,   403,  -208,   187,   135,   -16,   817,    99,   195,  1491,
    1531,   197,  -208,   242,  -208,   199,   208,  -208,   216,  -208,
     944,  -208,   211,   139,  1804,  -208,   913,  -208,   209,  1049,
     210,   -13,   179,   205,   860,  -208,   215,  -208,   212,  1086,
    1804,   913,   870,   257,   258,   172,  2022,  2050,  2078,  2106,
    2134,  2134,   483,   483,   483,   483,   552,    30,    30,   128,
     128,   128,   128,   412,  1804,  1767,   913,  1239,   172,   191,
    -208,  -208,   913,  1281,  -208,  -208,   262,   219,  -208,   168,
     222,   170,  -208,    13,   169,   201,  -208,   228,   140,    12,
    -208,  1123,   165,    94,  -208,   817,   179,   616,   616,   913,
       7,   913,   913,   913,  -208,  -208,   944,   944,  1804,  -208,
     913,  -208,  -208,   169,   233,  -208,  -208,  -208,  1804,  -208,
    -208,  -208,  1839,   913,  1876,  -208,  -208,   913,  1691,  -208,
     913,  -208,  -208,  -208,   913,   270,  -208,   277,  -208,  -208,
    -208,  -208,   238,  -208,  -208,   169,   201,   201,   205,  -208,
    -208,  -208,   860,  -208,    22,   237,   169,   240,  1160,   225,
    -208,  1571,   913,  1804,   239,  -208,  1804,  -208,  -208,  1804,
    -208,   169,   913,  1913,   913,  1986,  -208,  1323,  1365,  -208,
    -208,  -208,  -208,  -208,  -208,   252,   247,  -208,   860,   860,
    -208,   169,  -208,   616,   241,  1611,   913,  -208,  1950,   913,
    1950,  -208,   913,  -208,   913,   860,  -208,   250,  -208,  -208,
    -208,  -208,   616,   249,   208,  1950,  1729,  1407,  -208,  -208,
    -208,   616,  -208,   913,  -208,  1449,   913,  1651,  -208
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -208,  -208,   223,  -208,    10,   243,  -208,   259,    11,   144,
     281,    -8,  -208,   280,   226,   -17,   282,   -24,  -208,  -208,
     120,    14,  -207,   236,   153,  -208,  -208,  -164,  -208,  -208,
    -206,  -208,  -208,   213,  -208,  -156,   214,  -208,    -5,  -208,
    -117,   182,  -208,  -208,   -19,   -72,  -166,  -208,  -208,  -208,
     -35
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -45
static const yytype_int16 yytable[] =
{
      52,    53,    54,    55,    76,    56,    81,    59,    66,     4,
     164,    91,    35,    72,   254,     4,   220,   264,    95,    69,
      60,    67,    58,     3,    86,    65,    87,   234,    70,   294,
     263,    74,    77,   235,   139,   303,   235,    10,    89,   -44,
     153,     9,   226,   312,    89,     9,   226,    35,    12,   233,
      91,   149,   236,   153,   157,   236,   291,   123,   124,   125,
     126,    19,   235,   337,    22,   163,   227,    35,   293,   292,
     230,   338,    73,    30,   159,   160,    31,    32,    88,   307,
      89,   236,    90,   174,    79,   179,    80,    61,    61,    68,
     333,   334,    35,   191,   192,   336,   174,   320,   189,   190,
     317,   318,   281,   195,    82,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   217,   218,    83,    84,   332,
     179,   357,   358,   221,   246,   225,   223,   306,    96,   242,
     340,   243,   101,   235,   102,   180,   103,   143,   368,    97,
     144,   241,    88,   138,    89,   347,    90,   134,   126,    89,
     155,    35,   236,   145,   156,   174,   181,   -34,     4,   142,
     153,   258,     5,     6,    88,   359,    89,   233,   135,   268,
     315,   153,   146,   -34,   256,   296,   272,   274,   257,   297,
       7,     8,   222,   147,     9,   180,    10,   121,   122,   123,
     124,   125,   126,   148,   150,   242,   300,    12,   154,    11,
     301,   278,   130,   287,   302,   287,   288,   282,   290,    89,
      19,   161,   298,    22,   175,   180,   183,   184,   309,   310,
     187,   130,    30,   224,   177,    31,    32,   268,   168,   249,
     308,   245,     5,     6,   311,   250,   313,   251,   316,    74,
     253,   174,   174,   252,   255,   319,   259,   262,   269,   270,
       7,     8,   275,   276,     9,   285,    10,   286,   323,   236,
     289,   295,   325,   329,   164,   327,   321,    12,   169,   328,
     226,   331,   339,   341,   170,   343,   355,   346,   356,   361,
      19,   369,   371,    22,   231,   151,    94,   268,   330,    71,
     137,    75,    30,   265,    78,    31,    32,   345,   162,   152,
     244,   364,   335,     0,   219,   186,   188,   348,     0,   350,
       0,     0,     0,     0,   360,     0,     0,     0,     0,     0,
       0,     0,     0,   268,   268,     4,     0,     0,     0,     5,
       6,     0,     0,   370,   365,     0,     0,   366,     0,   367,
     268,     0,   374,     0,     0,     0,     0,     7,     8,     0,
       0,     9,     0,    10,     0,     0,     0,     0,   375,     0,
       0,   377,    11,   136,    12,     0,     0,     0,     0,     0,
      13,     0,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,     0,    24,    25,    26,    27,    28,    29,    30,
       0,     0,    31,    32,    33,    34,     4,     0,     0,     0,
       5,     6,     0,     0,     0,     0,     0,   105,   106,   107,
     108,   109,     0,     0,     0,     0,     0,     0,     7,     8,
       0,     0,     9,     0,    10,     0,   120,   121,   122,   123,
     124,   125,   126,    11,   232,    12,     0,     0,     0,     0,
       0,    13,   130,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,     0,    24,    25,    26,    27,    28,    29,
      30,     0,     0,    31,    32,    33,    34,     4,     0,     0,
       0,     5,     6,     0,     0,     0,     0,     0,   105,   106,
     107,   108,   109,     0,     0,     0,     0,     0,     0,     7,
       8,     0,     0,     9,     0,    10,     0,   120,   121,   122,
     123,   124,   125,   126,    11,   127,    12,     0,     0,     0,
       0,     0,    13,   130,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,     0,    24,    25,    26,    27,    28,
      29,    30,     0,     0,    31,    32,    33,    34,     4,     0,
       0,     0,     5,     6,     0,     0,     0,   105,   106,   107,
     108,   109,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     8,     0,     0,     9,     0,    10,   121,   122,   123,
     124,   125,   126,     0,     0,    11,     0,    12,     0,     0,
       0,     0,   130,    13,     0,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,     0,    24,    25,    26,    27,
      28,    29,    30,     0,     0,    31,    32,    92,    93,     4,
       0,     0,     0,     5,     6,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     8,     0,     0,     9,     0,    10,     0,     0,
       0,     0,     0,     0,     0,     0,    11,     0,    12,     0,
       0,     0,     4,     0,    13,     0,     5,     6,     0,     0,
       0,    19,     0,     0,    22,    23,     0,    24,    25,    26,
      27,    28,    29,    30,     7,     8,    31,    32,     9,     0,
      10,     0,     0,   168,     0,   176,     0,     5,     6,     0,
       0,    12,     0,     0,     0,     0,   177,     0,     0,     0,
       0,     0,     0,     0,    19,     7,     8,    22,     0,     9,
       0,    10,     0,     0,     0,     0,    30,     0,     0,    31,
      32,     0,    12,   185,     0,     0,     4,     0,     0,   170,
       5,     6,     0,     0,     0,    19,     4,     0,    22,     0,
       5,     6,     0,     0,     0,     0,     0,    30,     7,     8,
      31,    32,     9,     0,    10,     0,     0,     0,     7,     8,
       0,     0,     9,     0,    10,    12,     0,     0,     0,   176,
       0,    85,     0,     0,     0,    12,     0,     0,    19,     4,
       0,    22,     0,     5,     6,     0,     0,     0,    19,   216,
      30,    22,     0,    31,    32,     0,     0,     0,     0,     0,
      30,     7,     8,    31,    32,     9,     0,    10,     0,     0,
       4,     0,     0,     0,     5,     6,     0,     0,    12,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    19,     7,     8,    22,     0,     9,     0,    10,     0,
       0,     0,     0,    30,     0,     0,    31,    32,     0,    12,
       0,     0,     0,     4,   177,     0,     0,     5,     6,     0,
       0,     0,    19,     4,     0,    22,     0,     5,     6,     0,
       0,     0,   273,     0,    30,     7,     8,    31,    32,     9,
       0,    10,     0,     0,     0,     7,     8,     0,     0,     9,
     242,    10,    12,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    12,     0,     0,    19,     4,     0,    22,     0,
       5,     6,     0,     0,     0,    19,     0,    30,    22,     0,
      31,    32,     0,     0,     0,     0,     0,    30,     7,     8,
      31,    32,     9,     0,    10,     0,     0,   168,     0,     0,
       0,     5,     6,     0,     0,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    19,     7,
       8,    22,     0,     9,     0,    10,     0,     0,     0,     0,
      30,     0,     0,    31,    32,     0,    12,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    19,
       0,     0,    22,     0,     0,     0,     0,     0,     0,     0,
       0,    30,     0,     0,    31,    32,   104,   105,   106,   107,
     108,   109,     0,     0,     0,     0,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,     0,   127,     0,   128,   129,     0,     0,
       0,     0,   130,   104,   105,   106,   107,   108,   109,     0,
     167,     0,     0,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       0,   127,     0,   128,   129,   260,     0,     0,     0,   130,
     104,   105,   106,   107,   108,   109,   261,     0,     0,     0,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,     0,   127,     0,
     128,   129,   260,     0,     0,     0,   130,   104,   105,   106,
     107,   108,   109,   271,     0,     0,     0,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,     0,   127,     0,   128,   129,     0,
       0,     0,     0,   130,   104,   105,   106,   107,   108,   109,
     299,     0,     0,     0,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,     0,   127,     0,   128,   129,     0,     0,     0,     0,
     130,   104,   105,   106,   107,   108,   109,   342,     0,     0,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,     0,   127,
       0,   128,   129,     0,     0,     0,     0,   130,     0,     0,
     140,     0,   141,   104,   105,   106,   107,   108,   109,     0,
       0,     0,     0,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       0,   127,     0,   128,   129,     0,     0,     0,     0,   130,
     279,     0,     0,     0,   280,   104,   105,   106,   107,   108,
     109,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,     0,   127,     0,   128,   129,     0,     0,     0,
       0,   130,     0,     0,   283,     0,   284,   104,   105,   106,
     107,   108,   109,     0,     0,     0,     0,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,     0,   127,     0,   128,   129,     0,
       0,     0,     0,   130,   351,     0,     0,     0,   352,   104,
     105,   106,   107,   108,   109,     0,     0,     0,     0,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,     0,   127,     0,   128,
     129,     0,     0,     0,     0,   130,     0,     0,   353,     0,
     354,   104,   105,   106,   107,   108,   109,     0,     0,     0,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,     0,   127,
       0,   128,   129,     0,     0,     0,     0,   130,     0,     0,
       0,     0,   373,   104,   105,   106,   107,   108,   109,     0,
       0,     0,     0,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       0,   127,     0,   128,   129,     0,     0,     0,     0,   130,
       0,     0,     0,     0,   376,   104,   105,   106,   107,   108,
     109,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,     0,   127,     0,   128,   129,     0,     0,     0,
       0,   130,     0,     0,   247,   104,   105,   106,   107,   108,
     109,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,     0,   127,     0,   128,   129,     0,     0,     0,
       0,   130,     0,     0,   248,   104,   105,   106,   107,   108,
     109,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,     0,   127,     0,   128,   129,     0,     0,     0,
       0,   130,     0,     0,   344,   104,   105,   106,   107,   108,
     109,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,     0,   127,     0,   128,   129,     0,     0,     0,
       0,   130,     0,     0,   362,   104,   105,   106,   107,   108,
     109,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,     0,   127,     0,   128,   129,     0,     0,     0,
       0,   130,     0,     0,   378,   104,   105,   106,   107,   108,
     109,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,     0,   127,     0,   128,   129,     0,     0,     0,
       0,   130,   326,   104,   105,   106,   107,   108,   109,     0,
       0,     0,     0,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       0,   127,     0,   128,   129,     0,     0,     0,     0,   130,
     372,   104,   105,   106,   107,   108,   109,     0,     0,     0,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,     0,   127,
       0,   128,   129,   277,     0,     0,     0,   130,   104,   105,
     106,   107,   108,   109,     0,     0,     0,     0,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,     0,   127,     0,   128,   129,
       0,     0,     0,   104,   130,   106,   107,   108,   109,     0,
       0,     0,     0,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
       0,   127,     0,   128,   129,     0,     0,     0,   322,   130,
     104,     0,   106,   107,   108,   109,     0,     0,     0,     0,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,     0,   127,     0,
     128,   129,     0,     0,     0,   324,   130,   104,     0,   106,
     107,   108,   109,     0,     0,     0,     0,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,     0,   127,     0,   128,   129,     0,
       0,     0,   349,   130,   104,     0,   106,   107,   108,   109,
       0,     0,     0,     0,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,     0,   127,     0,   128,   129,     0,     0,     0,     0,
     130,   105,   106,   107,   108,   109,     0,     0,     0,     0,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,     0,   127,     0,
       0,   129,     0,     0,     0,     0,   130,   105,   106,   107,
     108,   109,     0,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,     0,   127,   105,   106,   107,   108,   109,
       0,     0,   130,     0,     0,     0,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,     0,   127,   105,   106,   107,   108,   109,     0,     0,
     130,     0,     0,     0,     0,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,     0,
     127,   105,   106,   107,   108,   109,     0,     0,   130,     0,
       0,     0,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,     0,   127,   105,
     106,   107,   108,   109,     0,     0,   130,     0,     0,     0,
       0,     0,     0,     0,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,     0,   127,     0,     0,     0,
       0,     0,     0,     0,   130
};

static const yytype_int16 yycheck[] =
{
       5,     6,     7,     8,    21,    10,    25,    12,    16,     3,
      82,    35,     2,    40,   170,     3,   133,   181,    37,     3,
       3,     3,    12,     0,    29,    15,    31,    43,    18,   235,
      43,    21,    22,    49,    58,   242,    49,    31,    44,     3,
      46,    29,     3,    36,    44,    29,     3,    37,    42,    42,
      74,    51,    68,    46,    78,    68,    43,    27,    28,    29,
      30,    55,    49,    41,    58,    82,    27,    57,   234,   233,
      27,    49,     3,    67,    79,    80,    70,    71,    42,   243,
      44,    68,    46,    88,    42,    90,    42,    71,    71,    71,
     296,   297,    82,    10,    11,   302,   101,   263,   103,   104,
     256,   257,   219,   108,    42,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,    48,    48,   295,
     135,   338,   339,   138,   158,   143,   141,    43,    48,    40,
     306,    42,    42,    49,    44,    46,    46,    45,   355,     3,
      48,   156,    42,    43,    44,   321,    46,    48,    30,    44,
      42,   151,    68,    48,    46,   170,    42,     3,     3,     3,
      46,   176,     7,     8,    42,   341,    44,    42,    46,   184,
     252,    46,    48,    50,    45,    45,   191,   192,    49,    49,
      25,    26,    43,    48,    29,    46,    31,    25,    26,    27,
      28,    29,    30,    48,    50,    40,    41,    42,     3,    40,
      45,   216,    40,    45,    49,    45,    48,   222,    48,    44,
      55,    61,   239,    58,     3,    46,    45,    34,   247,   248,
       3,    40,    67,     3,    47,    70,    71,   242,     3,    42,
     245,    46,     7,     8,   249,     3,   251,    48,   253,   239,
      34,   256,   257,    45,    43,   260,    47,    47,    43,    47,
      25,    26,     5,     5,    29,     3,    31,    48,   273,    68,
      48,    43,   277,     3,   346,   280,    43,    42,    43,   284,
       3,    43,    45,    43,    49,    60,    34,    48,    41,    48,
      55,    41,    43,    58,   150,    72,    37,   302,   287,    18,
      57,    21,    67,   183,    22,    70,    71,   312,    82,    73,
     157,   346,   298,    -1,   132,   101,   103,   322,    -1,   324,
      -1,    -1,    -1,    -1,   343,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   338,   339,     3,    -1,    -1,    -1,     7,
       8,    -1,    -1,   362,   349,    -1,    -1,   352,    -1,   354,
     355,    -1,   371,    -1,    -1,    -1,    -1,    25,    26,    -1,
      -1,    29,    -1,    31,    -1,    -1,    -1,    -1,   373,    -1,
      -1,   376,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      48,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    61,    62,    63,    64,    65,    66,    67,
      -1,    -1,    70,    71,    72,    73,     3,    -1,    -1,    -1,
       7,     8,    -1,    -1,    -1,    -1,    -1,     5,     6,     7,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,
      -1,    -1,    29,    -1,    31,    -1,    24,    25,    26,    27,
      28,    29,    30,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    48,    40,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    61,    62,    63,    64,    65,    66,
      67,    -1,    -1,    70,    71,    72,    73,     3,    -1,    -1,
      -1,     7,     8,    -1,    -1,    -1,    -1,    -1,     5,     6,
       7,     8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    25,
      26,    -1,    -1,    29,    -1,    31,    -1,    24,    25,    26,
      27,    28,    29,    30,    40,    32,    42,    -1,    -1,    -1,
      -1,    -1,    48,    40,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    61,    62,    63,    64,    65,
      66,    67,    -1,    -1,    70,    71,    72,    73,     3,    -1,
      -1,    -1,     7,     8,    -1,    -1,    -1,     5,     6,     7,
       8,     9,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    26,    -1,    -1,    29,    -1,    31,    25,    26,    27,
      28,    29,    30,    -1,    -1,    40,    -1,    42,    -1,    -1,
      -1,    -1,    40,    48,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    61,    62,    63,    64,
      65,    66,    67,    -1,    -1,    70,    71,    72,    73,     3,
      -1,    -1,    -1,     7,     8,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,    -1,    -1,    29,    -1,    31,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    40,    -1,    42,    -1,
      -1,    -1,     3,    -1,    48,    -1,     7,     8,    -1,    -1,
      -1,    55,    -1,    -1,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    67,    25,    26,    70,    71,    29,    -1,
      31,    -1,    -1,     3,    -1,    36,    -1,     7,     8,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    47,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    25,    26,    58,    -1,    29,
      -1,    31,    -1,    -1,    -1,    -1,    67,    -1,    -1,    70,
      71,    -1,    42,    43,    -1,    -1,     3,    -1,    -1,    49,
       7,     8,    -1,    -1,    -1,    55,     3,    -1,    58,    -1,
       7,     8,    -1,    -1,    -1,    -1,    -1,    67,    25,    26,
      70,    71,    29,    -1,    31,    -1,    -1,    -1,    25,    26,
      -1,    -1,    29,    -1,    31,    42,    -1,    -1,    -1,    36,
      -1,    48,    -1,    -1,    -1,    42,    -1,    -1,    55,     3,
      -1,    58,    -1,     7,     8,    -1,    -1,    -1,    55,    13,
      67,    58,    -1,    70,    71,    -1,    -1,    -1,    -1,    -1,
      67,    25,    26,    70,    71,    29,    -1,    31,    -1,    -1,
       3,    -1,    -1,    -1,     7,     8,    -1,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    25,    26,    58,    -1,    29,    -1,    31,    -1,
      -1,    -1,    -1,    67,    -1,    -1,    70,    71,    -1,    42,
      -1,    -1,    -1,     3,    47,    -1,    -1,     7,     8,    -1,
      -1,    -1,    55,     3,    -1,    58,    -1,     7,     8,    -1,
      -1,    -1,    12,    -1,    67,    25,    26,    70,    71,    29,
      -1,    31,    -1,    -1,    -1,    25,    26,    -1,    -1,    29,
      40,    31,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    42,    -1,    -1,    55,     3,    -1,    58,    -1,
       7,     8,    -1,    -1,    -1,    55,    -1,    67,    58,    -1,
      70,    71,    -1,    -1,    -1,    -1,    -1,    67,    25,    26,
      70,    71,    29,    -1,    31,    -1,    -1,     3,    -1,    -1,
      -1,     7,     8,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    25,
      26,    58,    -1,    29,    -1,    31,    -1,    -1,    -1,    -1,
      67,    -1,    -1,    70,    71,    -1,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    -1,    -1,    70,    71,     4,     5,     6,     7,
       8,     9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,
      -1,    -1,    40,     4,     5,     6,     7,     8,     9,    -1,
      48,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    32,    -1,    34,    35,    36,    -1,    -1,    -1,    40,
       4,     5,     6,     7,     8,     9,    47,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    32,    -1,
      34,    35,    36,    -1,    -1,    -1,    40,     4,     5,     6,
       7,     8,     9,    47,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    -1,    32,    -1,    34,    35,    -1,
      -1,    -1,    -1,    40,     4,     5,     6,     7,     8,     9,
      47,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,    -1,
      40,     4,     5,     6,     7,     8,     9,    47,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      -1,    34,    35,    -1,    -1,    -1,    -1,    40,    -1,    -1,
      43,    -1,    45,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    32,    -1,    34,    35,    -1,    -1,    -1,    -1,    40,
      41,    -1,    -1,    -1,    45,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    43,    -1,    45,     4,     5,     6,
       7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    -1,    32,    -1,    34,    35,    -1,
      -1,    -1,    -1,    40,    41,    -1,    -1,    -1,    45,     4,
       5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    -1,    32,    -1,    34,
      35,    -1,    -1,    -1,    -1,    40,    -1,    -1,    43,    -1,
      45,     4,     5,     6,     7,     8,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      -1,    34,    35,    -1,    -1,    -1,    -1,    40,    -1,    -1,
      -1,    -1,    45,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    32,    -1,    34,    35,    -1,    -1,    -1,    -1,    40,
      -1,    -1,    -1,    -1,    45,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    43,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    43,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    43,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    43,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,
      -1,    40,    -1,    -1,    43,     4,     5,     6,     7,     8,
       9,    -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,
      -1,    40,    41,     4,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    32,    -1,    34,    35,    -1,    -1,    -1,    -1,    40,
      41,     4,     5,     6,     7,     8,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    -1,    32,
      -1,    34,    35,    36,    -1,    -1,    -1,    40,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    32,    -1,    34,    35,
      -1,    -1,    -1,     4,    40,     6,     7,     8,     9,    -1,
      -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      -1,    32,    -1,    34,    35,    -1,    -1,    -1,    39,    40,
       4,    -1,     6,     7,     8,     9,    -1,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    32,    -1,
      34,    35,    -1,    -1,    -1,    39,    40,     4,    -1,     6,
       7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    -1,    32,    -1,    34,    35,    -1,
      -1,    -1,    39,    40,     4,    -1,     6,     7,     8,     9,
      -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    -1,    32,    -1,    34,    35,    -1,    -1,    -1,    -1,
      40,     5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    32,    -1,
      -1,    35,    -1,    -1,    -1,    -1,    40,     5,     6,     7,
       8,     9,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    -1,    32,     5,     6,     7,     8,     9,
      -1,    -1,    40,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    -1,    32,     5,     6,     7,     8,     9,    -1,    -1,
      40,    -1,    -1,    -1,    -1,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
      32,     5,     6,     7,     8,     9,    -1,    -1,    40,    -1,
      -1,    -1,    -1,    -1,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    -1,    32,     5,
       6,     7,     8,     9,    -1,    -1,    40,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    40
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    77,    78,     0,     3,     7,     8,    25,    26,    29,
      31,    40,    42,    48,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    61,    62,    63,    64,    65,    66,
      67,    70,    71,    72,    73,    80,    81,    82,    83,    89,
      90,    91,    92,    99,   107,   108,   110,   114,   120,   121,
     122,    80,   114,   114,   114,   114,   114,    79,    80,   114,
       3,    71,    86,    87,    88,    80,    87,     3,    71,     3,
      80,    86,    40,     3,    80,    89,    91,    80,    92,    42,
      42,   120,    42,    48,    48,    48,   114,   114,    42,    44,
      46,    93,    72,    73,    83,   120,    48,     3,    95,    96,
      97,    42,    44,    46,     4,     5,     6,     7,     8,     9,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    32,    34,    35,
      40,   115,   116,   117,    48,    46,    41,    81,    43,    93,
      43,    45,     3,    45,    48,    48,    48,    48,    48,    51,
      50,    78,    99,    46,     3,    42,    46,    93,    94,   114,
     114,    61,    90,    91,   121,   123,   126,    48,     3,    43,
      49,   111,   112,   113,   114,     3,    36,    47,   109,   114,
      46,    42,    93,    45,    34,    43,   112,     3,   109,   114,
     114,    10,    11,   118,   119,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,    13,   114,   114,   117,
     116,   114,    43,   114,     3,    87,     3,    27,    84,    85,
      27,    85,    41,    42,    43,    49,    68,   103,   104,   105,
     106,   114,    40,    42,   100,    46,    93,    43,    43,    42,
       3,    48,    45,    34,   111,    43,    45,    49,   114,    47,
      36,    47,    47,    43,   103,    96,    98,   100,   114,    43,
      47,    47,   114,    12,   114,     5,     5,    36,   114,    41,
      45,   116,   114,    43,    45,     3,    48,    45,    48,    48,
      48,    43,   103,   122,   106,    43,    45,    49,    91,    47,
      41,    45,    49,    98,   101,   102,    43,   103,   114,   120,
     120,   114,    36,   114,   124,   121,   114,   111,   111,   114,
     122,    43,    39,   114,    39,   114,    41,   114,   114,     3,
      84,    43,   122,   106,   106,    97,    98,    41,    49,    45,
     122,    43,    47,    60,    43,   114,    48,   122,   114,    39,
     114,    41,    45,    43,    45,    34,    41,    98,    98,   122,
     120,    48,    43,   125,   126,   114,   114,   114,    98,    41,
     120,    43,    41,    45,   120,   114,    45,   114,    43
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
#line 179 "camp.y"
    { absyntax::root = (yyvsp[(1) - (1)].b); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 183 "camp.y"
    { (yyval.b) = new file(lexerPos(), false); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 185 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 189 "camp.y"
    { (yyval.b) = new block(lexerPos(), true); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 191 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 195 "camp.y"
    { (yyval.n) = new simpleName((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 196 "camp.y"
    { (yyval.n) = new qualifiedName((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 197 "camp.y"
    { (yyval.n) = new simpleName((yyvsp[(1) - (1)].ps).pos,
                                  symbol::trans("operator answer")); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 202 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 203 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 205 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].d)); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 207 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].s)); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 211 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].mod).pos); (yyval.ml)->add((yyvsp[(1) - (1)].mod).val); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 212 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].perm).pos); (yyval.ml)->add((yyvsp[(1) - (1)].perm).val); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 214 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].mod).val); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 216 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].perm).val); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 220 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].vd); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 221 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 222 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 224 "camp.y"
    { (yyval.d) = new accessdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ipl)); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 226 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), (yyvsp[(4) - (5)].ipl)); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 228 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), WILDCARD); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 229 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].n), WILDCARD); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 231 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, (yyvsp[(4) - (5)].ipl)); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 233 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, WILDCARD); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 235 "camp.y"
    { (yyval.d) = new importdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ip)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 236 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 238 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].stre)->getString()); ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 242 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 244 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 248 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 250 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 254 "camp.y"
    { (yyval.ps) = (yyvsp[(1) - (1)].ps); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 255 "camp.y"
    { (yyval.ps).pos = (yyvsp[(1) - (1)].stre)->getPos();
                     (yyval.ps).sym = symbol::trans((yyvsp[(1) - (1)].stre)->getString()); ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 260 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 262 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 266 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 268 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 272 "camp.y"
    { (yyval.vd) = (yyvsp[(1) - (2)].vd); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 276 "camp.y"
    { (yyval.vd) = new vardec((yyvsp[(1) - (2)].t)->getPos(), (yyvsp[(1) - (2)].t), (yyvsp[(2) - (2)].dil)); ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 280 "camp.y"
    { (yyval.t) = (yyvsp[(1) - (1)].t); ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 281 "camp.y"
    { (yyval.t) = new arrayTy((yyvsp[(1) - (2)].n), (yyvsp[(2) - (2)].dim)); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 285 "camp.y"
    { (yyval.t) = new nameTy((yyvsp[(1) - (1)].n)); ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 289 "camp.y"
    { (yyval.dim) = new dimensions((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 290 "camp.y"
    { (yyval.dim) = (yyvsp[(1) - (3)].dim); (yyval.dim)->increase(); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 294 "camp.y"
    { (yyval.elist) = new explist((yyvsp[(1) - (3)].pos)); (yyval.elist)->add((yyvsp[(2) - (3)].e)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 296 "camp.y"
    { (yyval.elist) = (yyvsp[(1) - (4)].elist); (yyval.elist)->add((yyvsp[(3) - (4)].e)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 300 "camp.y"
    { (yyval.dil) = new decidlist((yyvsp[(1) - (1)].di)->getPos()); (yyval.dil)->add((yyvsp[(1) - (1)].di)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 302 "camp.y"
    { (yyval.dil) = (yyvsp[(1) - (3)].dil); (yyval.dil)->add((yyvsp[(3) - (3)].di)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 306 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (1)].dis)->getPos(), (yyvsp[(1) - (1)].dis)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 308 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (3)].dis)->getPos(), (yyvsp[(1) - (3)].dis), (yyvsp[(3) - (3)].vi)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 312 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 313 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (2)].ps).pos, (yyvsp[(1) - (2)].ps).sym, (yyvsp[(2) - (2)].dim)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 314 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, 0,
                                            new formals((yyvsp[(2) - (3)].pos))); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 317 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (4)].ps).pos, (yyvsp[(1) - (4)].ps).sym, 0, (yyvsp[(3) - (4)].fls)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 321 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 322 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].ai); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 327 "camp.y"
    { (yyval.b) = (yyvsp[(2) - (3)].b); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 331 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 333 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (4)].pos)); (yyval.ai)->addRest((yyvsp[(3) - (4)].vi)); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 335 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (3)].ai); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 337 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (5)].ai); (yyval.ai)->addRest((yyvsp[(4) - (5)].vi)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 341 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 342 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (1)].ai); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 343 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (2)].ai); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 347 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].vi)->getPos());
		     (yyval.ai)->add((yyvsp[(1) - (1)].vi));;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 350 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (3)].ai); (yyval.ai)->add((yyvsp[(3) - (3)].vi)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 354 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (1)].fls); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 356 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->addRest((yyvsp[(3) - (3)].fl)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 357 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (2)].pos)); (yyval.fls)->addRest((yyvsp[(2) - (2)].fl)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 361 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (1)].fl)->getPos()); (yyval.fls)->add((yyvsp[(1) - (1)].fl)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 363 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->add((yyvsp[(3) - (3)].fl)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 367 "camp.y"
    { (yyval.boo) = true; ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 368 "camp.y"
    { (yyval.boo) = false; ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 373 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (2)].t)->getPos(), (yyvsp[(2) - (2)].t), 0, 0, (yyvsp[(1) - (2)].boo)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 375 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (3)].t)->getPos(), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].dis), 0, (yyvsp[(1) - (3)].boo)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 377 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (5)].t)->getPos(), (yyvsp[(2) - (5)].t), (yyvsp[(3) - (5)].dis), (yyvsp[(5) - (5)].vi), (yyvsp[(1) - (5)].boo)); ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 382 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (5)].pos), (yyvsp[(1) - (5)].t), (yyvsp[(2) - (5)].ps).sym, new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 384 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (6)].pos), (yyvsp[(1) - (6)].t), (yyvsp[(2) - (6)].ps).sym, (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 388 "camp.y"
    { (yyval.d) = new recorddec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].b)); ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 389 "camp.y"
    { (yyval.d) = new typedec((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].vd)); ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 393 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (1)].pos), 0, 0); ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 394 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (2)].pos), (yyvsp[(1) - (2)].e), 0); ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 395 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (2)].pos), 0, (yyvsp[(2) - (2)].e)); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 396 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 400 "camp.y"
    { (yyval.e) = new fieldExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].ps).sym); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 401 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].e)); ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 403 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].e)); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 404 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].slice)); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 406 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].slice)); ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 407 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos),
                                      new nameExp((yyvsp[(1) - (3)].n)->getPos(), (yyvsp[(1) - (3)].n)),
                                      new arglist()); ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 411 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), 
                                      new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)),
                                      (yyvsp[(3) - (4)].alist)); ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 414 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), new arglist()); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 416 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].alist)); ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 418 "camp.y"
    { (yyval.e) = (yyvsp[(2) - (3)].e); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 420 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(2) - (3)].n)->getPos(), (yyvsp[(2) - (3)].n)); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 421 "camp.y"
    { (yyval.e) = new thisExp((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 425 "camp.y"
    { (yyval.arg).name = symbol::nullsym; (yyval.arg).val=(yyvsp[(1) - (1)].e); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 426 "camp.y"
    { (yyval.arg).name = (yyvsp[(1) - (3)].ps).sym; (yyval.arg).val=(yyvsp[(3) - (3)].e); ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 430 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (1)].alist); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 432 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->rest = (yyvsp[(3) - (3)].arg); ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 433 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->rest = (yyvsp[(2) - (2)].arg); ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 437 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->add((yyvsp[(1) - (1)].arg)); ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 439 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->add((yyvsp[(3) - (3)].arg)); ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 444 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(1) - (1)].n)->getPos(), (yyvsp[(1) - (1)].n)); ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 445 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 446 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 447 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].stre); ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 449 "camp.y"
    { (yyval.e) = new scaleExp((yyvsp[(1) - (2)].e)->getPos(), (yyvsp[(1) - (2)].e), (yyvsp[(2) - (2)].e)); ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 451 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (4)].n)->getPos(), new nameTy((yyvsp[(2) - (4)].n)), (yyvsp[(4) - (4)].e)); ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 453 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (5)].n)->getPos(), new arrayTy((yyvsp[(2) - (5)].n), (yyvsp[(3) - (5)].dim)), (yyvsp[(5) - (5)].e)); ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 455 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 457 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 458 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 459 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 460 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 461 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 462 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 463 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 464 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 465 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 466 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 467 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 468 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 469 "camp.y"
    { (yyval.e) = new equalityExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 470 "camp.y"
    { (yyval.e) = new equalityExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 471 "camp.y"
    { (yyval.e) = new andExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 472 "camp.y"
    { (yyval.e) = new orExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 473 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 474 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 475 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 476 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 477 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 479 "camp.y"
    { (yyval.e) = new newRecordExp((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].t)); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 481 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].elist), 0, 0); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 483 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), (yyvsp[(3) - (4)].elist), (yyvsp[(4) - (4)].dim), 0); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 485 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), 0, (yyvsp[(3) - (3)].dim), 0); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 487 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), 0, (yyvsp[(3) - (4)].dim), (yyvsp[(4) - (4)].ai)); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 489 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].t), new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 491 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos),
                                             new arrayTy((yyvsp[(2) - (6)].t)->getPos(), (yyvsp[(2) - (6)].t), (yyvsp[(3) - (6)].dim)),
                                             new formals((yyvsp[(4) - (6)].pos)),
                                             (yyvsp[(6) - (6)].s)); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 496 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos), (yyvsp[(2) - (6)].t), (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 498 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (7)].pos),
                                             new arrayTy((yyvsp[(2) - (7)].t)->getPos(), (yyvsp[(2) - (7)].t), (yyvsp[(3) - (7)].dim)),
                                             (yyvsp[(5) - (7)].fls),
                                             (yyvsp[(7) - (7)].s)); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 503 "camp.y"
    { (yyval.e) = new conditionalExp((yyvsp[(2) - (5)].pos), (yyvsp[(1) - (5)].e), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].e)); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 504 "camp.y"
    { (yyval.e) = new assignExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 507 "camp.y"
    { (yyval.e) = new pairExp((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].e)); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 509 "camp.y"
    { (yyval.e) = new tripleExp((yyvsp[(1) - (7)].pos), (yyvsp[(2) - (7)].e), (yyvsp[(4) - (7)].e), (yyvsp[(6) - (7)].e)); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 511 "camp.y"
    { (yyval.e) = new transformExp((yyvsp[(1) - (13)].pos), (yyvsp[(2) - (13)].e), (yyvsp[(4) - (13)].e), (yyvsp[(6) - (13)].e), (yyvsp[(8) - (13)].e), (yyvsp[(10) - (13)].e), (yyvsp[(12) - (13)].e)); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 513 "camp.y"
    { (yyvsp[(2) - (3)].j)->pushFront((yyvsp[(1) - (3)].e)); (yyvsp[(2) - (3)].j)->pushBack((yyvsp[(3) - (3)].e)); (yyval.e) = (yyvsp[(2) - (3)].j); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 515 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::OUT);
                     joinExp *jexp =
                         new joinExp((yyvsp[(2) - (2)].se)->getPos(), SYM_DOTS);
                     (yyval.e)=jexp;
                     jexp->pushBack((yyvsp[(1) - (2)].e)); jexp->pushBack((yyvsp[(2) - (2)].se)); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 521 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), SYM_PLUS); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 523 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), SYM_MINUS); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 526 "camp.y"
    { (yyval.e) = new postfixExp((yyvsp[(2) - (2)].ps).pos, (yyvsp[(1) - (2)].e), SYM_PLUS); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 527 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 529 "camp.y"
    { (yyval.e) = new quoteExp((yyvsp[(1) - (4)].pos), (yyvsp[(3) - (4)].b)); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 535 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos,(yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 537 "camp.y"
    { (yyval.j) = (yyvsp[(1) - (1)].j); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 539 "camp.y"
    { (yyvsp[(1) - (2)].se)->setSide(camp::OUT);
                     (yyval.j) = (yyvsp[(2) - (2)].j); (yyval.j)->pushFront((yyvsp[(1) - (2)].se)); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 542 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(1) - (2)].j); (yyval.j)->pushBack((yyvsp[(2) - (2)].se)); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 545 "camp.y"
    { (yyvsp[(1) - (3)].se)->setSide(camp::OUT); (yyvsp[(3) - (3)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(2) - (3)].j); (yyval.j)->pushFront((yyvsp[(1) - (3)].se)); (yyval.j)->pushBack((yyvsp[(3) - (3)].se)); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 550 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(2) - (4)].ps).pos, (yyvsp[(2) - (4)].ps).sym, (yyvsp[(3) - (4)].e)); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 551 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (3)].pos), symbol::opTrans("spec"), (yyvsp[(2) - (3)].e)); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 553 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (5)].pos), symbol::opTrans("spec"),
				      new pairExp((yyvsp[(3) - (5)].pos), (yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].e))); ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 556 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (7)].pos), symbol::opTrans("spec"),
				      new tripleExp((yyvsp[(3) - (7)].pos), (yyvsp[(2) - (7)].e), (yyvsp[(4) - (7)].e), (yyvsp[(6) - (7)].e))); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 561 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 563 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 565 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 566 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 567 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 571 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym,
                              new booleanExp((yyvsp[(1) - (2)].ps).pos, false)); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 574 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e),
                              new booleanExp((yyvsp[(1) - (4)].ps).pos, false)); ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 577 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(3) - (3)].e), (yyvsp[(1) - (3)].ps).sym,
                              new booleanExp((yyvsp[(2) - (3)].ps).pos, true)); ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 580 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (5)].ps).pos, (yyvsp[(3) - (5)].e), (yyvsp[(1) - (5)].ps).sym, (yyvsp[(5) - (5)].e),
                              new booleanExp((yyvsp[(2) - (5)].ps).pos, true)); ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 585 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 587 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e)); ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 591 "camp.y"
    { (yyval.s) = new emptyStm((yyvsp[(1) - (1)].pos)); ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 592 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (1)].s); ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 593 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (2)].s); ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 595 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 597 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (7)].pos), (yyvsp[(3) - (7)].e), (yyvsp[(5) - (7)].s), (yyvsp[(7) - (7)].s)); ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 599 "camp.y"
    { (yyval.s) = new whileStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 601 "camp.y"
    { (yyval.s) = new doStm((yyvsp[(1) - (7)].pos), (yyvsp[(2) - (7)].s), (yyvsp[(5) - (7)].e)); ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 603 "camp.y"
    { (yyval.s) = new forStm((yyvsp[(1) - (9)].pos), (yyvsp[(3) - (9)].run), (yyvsp[(5) - (9)].e), (yyvsp[(7) - (9)].sel), (yyvsp[(9) - (9)].s)); ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 605 "camp.y"
    { (yyval.s) = new extendedForStm((yyvsp[(1) - (8)].pos), (yyvsp[(3) - (8)].t), (yyvsp[(4) - (8)].ps).sym, (yyvsp[(6) - (8)].e), (yyvsp[(8) - (8)].s)); ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 606 "camp.y"
    { (yyval.s) = new breakStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 607 "camp.y"
    { (yyval.s) = new continueStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 608 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (2)].pos)); ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 609 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].e)); ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 613 "camp.y"
    { (yyval.s) = new expStm((yyvsp[(1) - (1)].e)->getPos(), (yyvsp[(1) - (1)].e)); ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 617 "camp.y"
    { (yyval.s) = new blockStm((yyvsp[(1) - (1)].b)->getPos(), (yyvsp[(1) - (1)].b)); ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 621 "camp.y"
    { (yyval.run) = 0; ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 622 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].sel); ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 623 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].vd); ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 627 "camp.y"
    { (yyval.e) = 0; ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 628 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 632 "camp.y"
    { (yyval.sel) = 0; ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 633 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (1)].sel); ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 637 "camp.y"
    { (yyval.sel) = new stmExpList((yyvsp[(1) - (1)].s)->getPos()); (yyval.sel)->add((yyvsp[(1) - (1)].s)); ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 639 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (3)].sel); (yyval.sel)->add((yyvsp[(3) - (3)].s)); ;}
    break;



/* Line 1455 of yacc.c  */
#line 3601 "camp.tab.c"
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



