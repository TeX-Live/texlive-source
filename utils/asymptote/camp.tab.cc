/* A Bison parser, made by GNU Bison 2.6.1.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.6.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 336 of yacc.c  */
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

// Check if the symbol given is "keyword".  Returns true in this case and
// returns false and reports an error otherwise.
bool checkKeyword(position pos, symbol sym)
{
  if (sym != symbol::trans("keyword")) {
    em.error(pos);
    em << "expected 'keyword' here";

    return false;
  }
  return true;
}

namespace absyntax { file *root; }

using namespace absyntax;
using sym::symbol;
using mem::string;

/* Line 336 of yacc.c  */
#line 127 "camp.tab.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
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
   by #include "camp.tab.h".  */
#ifndef YY_CAMP_TAB_H
# define YY_CAMP_TAB_H
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
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
     OPERATOR = 280,
     LOOSE = 281,
     ASSIGN = 282,
     DIRTAG = 283,
     JOIN_PREC = 284,
     AND = 285,
     ELLIPSIS = 286,
     ACCESS = 287,
     UNRAVEL = 288,
     IMPORT = 289,
     INCLUDE = 290,
     FROM = 291,
     QUOTE = 292,
     STRUCT = 293,
     TYPEDEF = 294,
     NEW = 295,
     IF = 296,
     ELSE = 297,
     WHILE = 298,
     DO = 299,
     FOR = 300,
     BREAK = 301,
     CONTINUE = 302,
     RETURN_ = 303,
     THIS = 304,
     EXPLICIT = 305,
     GARBAGE = 306,
     LIT = 307,
     STRING = 308,
     PERM = 309,
     MODIFIER = 310,
     UNARY = 311,
     EXP_IN_PARENS_RULE = 312
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 350 of yacc.c  */
#line 60 "camp.y"

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


/* Line 350 of yacc.c  */
#line 275 "camp.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;

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

#endif /* !YY_CAMP_TAB_H  */

/* Copy the second part of user declarations.  */

/* Line 353 of yacc.c  */
#line 303 "camp.tab.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
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

# define YYCOPY_NEEDED 1

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
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1860

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  199
/* YYNRULES -- Number of states.  */
#define YYNSTATES  374

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   312

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    29,     2,     2,
      41,    42,    27,    25,    44,    26,    43,    28,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    35,    47,
       2,     2,     2,    34,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    45,     2,    46,    30,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    39,     2,    40,     2,     2,     2,     2,
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
      31,    32,    33,    36,    37,    38,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74
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
     215,   218,   222,   226,   228,   229,   232,   236,   242,   247,
     254,   260,   267,   271,   274,   276,   279,   282,   286,   290,
     295,   300,   305,   310,   314,   319,   323,   328,   332,   336,
     338,   340,   344,   346,   349,   353,   357,   361,   365,   367,
     369,   371,   373,   376,   381,   387,   390,   393,   396,   400,
     404,   408,   412,   416,   420,   424,   428,   432,   436,   440,
     444,   448,   452,   456,   460,   464,   468,   472,   475,   479,
     484,   488,   493,   499,   506,   513,   521,   527,   531,   535,
     539,   542,   545,   548,   551,   555,   560,   562,   564,   567,
     570,   574,   579,   583,   589,   597,   599,   603,   607,   609,
     611,   614,   619,   623,   629,   632,   637,   639,   641,   644,
     650,   658,   664,   672,   682,   691,   694,   697,   700,   704,
     706,   708,   709,   711,   713,   714,   716,   717,   719,   721
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      76,     0,    -1,    77,    -1,    -1,    77,    80,    -1,    -1,
      78,    80,    -1,     3,    -1,    79,    43,     3,    -1,    29,
      -1,    82,    -1,   118,    -1,    81,    82,    -1,    81,   118,
      -1,    72,    -1,    71,    -1,    81,    72,    -1,    81,    71,
      -1,    88,    -1,   105,    -1,   106,    -1,    49,    87,    47,
      -1,    53,    79,    50,    84,    47,    -1,    53,    79,    50,
      27,    47,    -1,    50,    79,    47,    -1,    53,    85,    49,
      84,    47,    -1,    53,    85,    49,    27,    47,    -1,    51,
      86,    47,    -1,    52,     3,    47,    -1,    52,    70,    47,
      -1,     3,    -1,     3,     3,     3,    -1,    83,    -1,    84,
      44,    83,    -1,     3,    -1,    70,    -1,     3,    -1,    85,
       3,     3,    -1,    86,    -1,    87,    44,    86,    -1,    89,
      47,    -1,    90,    94,    -1,    91,    -1,    79,    92,    -1,
      79,    -1,    45,    46,    -1,    92,    45,    46,    -1,    45,
     112,    46,    -1,    93,    45,   112,    46,    -1,    95,    -1,
      94,    44,    95,    -1,    96,    -1,    96,    33,    97,    -1,
       3,    -1,     3,    92,    -1,     3,    41,    42,    -1,     3,
      41,   102,    42,    -1,   112,    -1,    99,    -1,    39,    78,
      40,    -1,    39,    40,    -1,    39,    48,    97,    40,    -1,
      39,   100,    40,    -1,    39,   100,    48,    97,    40,    -1,
      44,    -1,   101,    -1,   101,    44,    -1,    97,    -1,   101,
      44,    97,    -1,   104,    -1,    48,   104,    -1,   102,    44,
     104,    -1,   102,    48,   104,    -1,    67,    -1,    -1,   103,
      90,    -1,   103,    90,    96,    -1,   103,    90,    96,    33,
      97,    -1,   103,    90,     3,    96,    -1,   103,    90,     3,
      96,    33,    97,    -1,    90,     3,    41,    42,   120,    -1,
      90,     3,    41,   102,    42,   120,    -1,    55,     3,    98,
      -1,    56,    88,    -1,    35,    -1,   112,    35,    -1,    35,
     112,    -1,   112,    35,   112,    -1,   108,    43,     3,    -1,
      79,    45,   112,    46,    -1,   108,    45,   112,    46,    -1,
      79,    45,   107,    46,    -1,   108,    45,   107,    46,    -1,
      79,    41,    42,    -1,    79,    41,   110,    42,    -1,   108,
      41,    42,    -1,   108,    41,   110,    42,    -1,    41,   112,
      42,    -1,    41,    79,    42,    -1,    66,    -1,   112,    -1,
       3,    33,   112,    -1,   109,    -1,    48,   109,    -1,   110,
      44,   109,    -1,   110,    48,   109,    -1,   112,    44,   112,
      -1,   111,    44,   112,    -1,    79,    -1,   108,    -1,    69,
      -1,    70,    -1,    69,   112,    -1,    41,    79,    42,   112,
      -1,    41,    79,    92,    42,   112,    -1,    25,   112,    -1,
      26,   112,    -1,    31,   112,    -1,   112,    25,   112,    -1,
     112,    26,   112,    -1,   112,    27,   112,    -1,   112,    28,
     112,    -1,   112,    29,   112,    -1,   112,    30,   112,    -1,
     112,    20,   112,    -1,   112,    21,   112,    -1,   112,    22,
     112,    -1,   112,    23,   112,    -1,   112,    18,   112,    -1,
     112,    19,   112,    -1,   112,    15,   112,    -1,   112,    14,
     112,    -1,   112,    24,   112,    -1,   112,    17,   112,    -1,
     112,    16,   112,    -1,   112,    31,   112,    -1,   112,     8,
     112,    -1,    57,    91,    -1,    57,    91,    93,    -1,    57,
      91,    93,    92,    -1,    57,    91,    92,    -1,    57,    91,
      92,    99,    -1,    57,    91,    41,    42,   120,    -1,    57,
      91,    92,    41,    42,   120,    -1,    57,    91,    41,   102,
      42,   120,    -1,    57,    91,    92,    41,   102,    42,   120,
      -1,   112,    34,   112,    35,   112,    -1,   112,    33,   112,
      -1,    41,   111,    42,    -1,   112,   113,   112,    -1,   112,
     114,    -1,     8,   112,    -1,     7,   112,    -1,   112,     8,
      -1,   112,     4,   112,    -1,    54,    39,    77,    40,    -1,
       7,    -1,   115,    -1,   114,   115,    -1,   115,   114,    -1,
     114,   115,   114,    -1,    39,    13,   112,    40,    -1,    39,
     112,    40,    -1,    39,   112,    44,   112,    40,    -1,    39,
     112,    44,   112,    44,   112,    40,    -1,     5,    -1,     5,
     116,     5,    -1,     5,   117,     5,    -1,     6,    -1,     9,
      -1,    11,   112,    -1,    11,   112,    38,   112,    -1,    11,
      12,   112,    -1,    11,    12,   112,    38,   112,    -1,    10,
     112,    -1,    10,   112,    38,   112,    -1,    47,    -1,   120,
      -1,   119,    47,    -1,    58,    41,   112,    42,   118,    -1,
      58,    41,   112,    42,   118,    59,   118,    -1,    60,    41,
     112,    42,   118,    -1,    61,   118,    60,    41,   112,    42,
      47,    -1,    62,    41,   121,    47,   122,    47,   123,    42,
     118,    -1,    62,    41,    90,     3,    35,   112,    42,   118,
      -1,    63,    47,    -1,    64,    47,    -1,    65,    47,    -1,
      65,   112,    47,    -1,   112,    -1,    98,    -1,    -1,   124,
      -1,    89,    -1,    -1,   112,    -1,    -1,   124,    -1,   119,
      -1,   124,    44,   119,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   191,   191,   195,   196,   201,   202,   207,   208,   209,
     214,   215,   216,   218,   223,   224,   225,   227,   232,   233,
     234,   235,   237,   239,   241,   242,   244,   246,   248,   249,
     254,   256,   260,   261,   266,   267,   272,   274,   278,   279,
     284,   288,   292,   293,   297,   301,   302,   306,   307,   312,
     313,   318,   319,   324,   325,   326,   328,   333,   334,   338,
     343,   344,   346,   348,   353,   354,   355,   359,   361,   366,
     367,   368,   370,   375,   376,   380,   382,   384,   387,   390,
     396,   398,   403,   404,   408,   409,   410,   411,   415,   416,
     418,   419,   421,   422,   425,   429,   430,   432,   434,   436,
     440,   441,   445,   446,   448,   450,   456,   457,   461,   462,
     463,   464,   466,   467,   469,   471,   473,   475,   476,   477,
     478,   479,   480,   481,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   495,   497,   499,
     501,   503,   505,   507,   512,   514,   519,   521,   522,   523,
     525,   531,   533,   536,   538,   539,   546,   547,   549,   552,
     555,   561,   562,   563,   566,   572,   573,   575,   577,   578,
     582,   584,   587,   590,   596,   597,   602,   603,   604,   605,
     607,   609,   611,   613,   615,   617,   618,   619,   620,   624,
     628,   632,   633,   634,   638,   639,   643,   644,   648,   649
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "SELFOP", "DOTS", "COLONS",
  "DASHES", "INCR", "LONGDASH", "CONTROLS", "TENSION", "ATLEAST", "CURL",
  "COR", "CAND", "BAR", "AMPERSAND", "EQ", "NEQ", "LT", "LE", "GT", "GE",
  "CARETS", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "OPERATOR", "LOOSE",
  "ASSIGN", "'?'", "':'", "DIRTAG", "JOIN_PREC", "AND", "'{'", "'}'",
  "'('", "')'", "'.'", "','", "'['", "']'", "';'", "ELLIPSIS", "ACCESS",
  "UNRAVEL", "IMPORT", "INCLUDE", "FROM", "QUOTE", "STRUCT", "TYPEDEF",
  "NEW", "IF", "ELSE", "WHILE", "DO", "FOR", "BREAK", "CONTINUE",
  "RETURN_", "THIS", "EXPLICIT", "GARBAGE", "LIT", "STRING", "PERM",
  "MODIFIER", "UNARY", "EXP_IN_PARENS_RULE", "$accept", "file",
  "fileblock", "bareblock", "name", "runnable", "modifiers", "dec",
  "idpair", "idpairlist", "strid", "stridpair", "stridpairlist", "vardec",
  "barevardec", "type", "celltype", "dims", "dimexps", "decidlist",
  "decid", "decidstart", "varinit", "block", "arrayinit", "basearrayinit",
  "varinits", "formals", "explicitornot", "formal", "fundec", "typedec",
  "slice", "value", "argument", "arglist", "tuple", "exp", "join", "dir",
  "basicjoin", "tension", "controls", "stm", "stmexp", "blockstm",
  "forinit", "fortest", "forupdate", "stmexplist", YY_NULL
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
      94,   280,   281,   282,    63,    58,   283,   284,   285,   123,
     125,    40,    41,    46,    44,    91,    93,    59,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    77,    77,    78,    78,    79,    79,    79,
      80,    80,    80,    80,    81,    81,    81,    81,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      83,    83,    84,    84,    85,    85,    86,    86,    87,    87,
      88,    89,    90,    90,    91,    92,    92,    93,    93,    94,
      94,    95,    95,    96,    96,    96,    96,    97,    97,    98,
      99,    99,    99,    99,   100,   100,   100,   101,   101,   102,
     102,   102,   102,   103,   103,   104,   104,   104,   104,   104,
     105,   105,   106,   106,   107,   107,   107,   107,   108,   108,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   108,
     109,   109,   110,   110,   110,   110,   111,   111,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   113,   113,   113,   113,
     113,   114,   114,   114,   114,   115,   115,   115,   115,   115,
     116,   116,   116,   116,   117,   117,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   119,
     120,   121,   121,   121,   122,   122,   123,   123,   124,   124
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
       2,     3,     3,     1,     0,     2,     3,     5,     4,     6,
       5,     6,     3,     2,     1,     2,     2,     3,     3,     4,
       4,     4,     4,     3,     4,     3,     4,     3,     3,     1,
       1,     3,     1,     2,     3,     3,     3,     3,     1,     1,
       1,     1,     2,     4,     5,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     4,
       3,     4,     5,     6,     6,     7,     5,     3,     3,     3,
       2,     2,     2,     2,     3,     4,     1,     1,     2,     2,
       3,     4,     3,     5,     7,     1,     3,     3,     1,     1,
       2,     4,     3,     5,     2,     4,     1,     1,     2,     5,
       7,     5,     7,     9,     8,     2,     2,     2,     3,     1,
       1,     0,     1,     1,     0,     1,     0,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     7,     0,     0,     0,     0,     9,
       0,     5,     0,   176,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      99,   110,   111,    15,    14,   108,     4,     0,    10,    18,
       0,     0,    42,   190,    19,    20,   109,   189,    11,     0,
     177,   108,   152,   151,   115,   116,   117,     0,   108,     0,
       0,    36,    35,     0,    38,     0,     0,     0,     0,     0,
       7,     0,     0,     3,     0,    44,    83,     0,    44,   137,
       0,     0,     0,   191,   185,   186,   187,     0,   112,     0,
       0,     0,    43,    17,    16,    12,    13,    40,    53,    41,
      49,    51,     0,     0,     0,     0,   165,   168,   156,   153,
     169,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   150,   157,   178,     0,    59,     6,    98,
       0,   148,     0,    97,     0,     0,     0,    21,    24,    27,
      28,    29,     0,     0,     0,    82,     0,    53,    74,     0,
     140,   138,     0,     0,     0,   193,     0,   198,     0,   192,
     188,     7,    93,     0,   102,     0,   100,     8,    84,    45,
       0,     0,     0,    74,    54,     0,     0,    95,     0,    88,
       0,     0,   154,     0,     0,     0,     0,   136,   131,   130,
     134,   133,   128,   129,   124,   125,   126,   127,   132,   118,
     119,   120,   121,   122,   123,   135,   147,     0,     0,     0,
     149,   158,   159,   113,     0,   107,   106,    37,    39,    30,
       0,    32,     0,     0,     0,   155,    74,     0,    74,    73,
       0,     0,    69,     0,     0,    74,   141,     0,   139,     0,
       0,     0,    53,   194,     0,     0,   103,    94,     0,     0,
      86,    91,    85,    89,    46,    55,     0,    50,    52,    58,
      57,    96,    92,    90,   174,     0,   170,   166,   167,     0,
       0,   162,     0,   160,   114,     0,    23,     0,    22,    26,
      25,    55,     0,   142,    70,     0,    74,    74,    75,    47,
      60,    64,     0,    67,     0,    65,     0,     0,     0,   179,
     181,     0,     0,   195,     0,   199,   101,   104,   105,    87,
      80,    56,     0,   172,     0,   146,   161,     0,    31,    33,
      56,   144,    71,    72,    53,    76,     0,    62,     0,    66,
     143,     0,    48,     0,     0,     0,   196,    81,   175,     0,
     171,   163,     0,    78,     0,    61,     0,    68,   145,   180,
     182,     0,     0,   197,   173,     0,     0,    77,    63,   184,
       0,   164,    79,   183
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    57,    51,    36,    37,    38,   231,   232,
      63,    64,    65,    39,    40,    41,    42,   184,   161,    99,
     100,   101,   268,    43,   269,   304,   305,   240,   241,   242,
      44,    45,   180,    46,   174,   175,    59,    47,   132,   133,
     134,   195,   196,    48,    49,    50,   168,   314,   362,   169
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -282
static const yytype_int16 yypact[] =
{
    -282,    30,   398,  -282,  -282,   846,   846,   846,   846,  -282,
     846,  -282,   846,  -282,    13,    20,    13,    17,     8,    12,
      53,    20,    20,   108,   126,   538,   131,   112,   143,   664,
    -282,    11,  -282,  -282,  -282,    60,  -282,   468,  -282,  -282,
     157,   190,  -282,  -282,  -282,  -282,   220,  1493,  -282,   165,
    -282,   228,   192,   192,   192,   192,  1821,   145,   191,     1,
    1104,   226,  -282,   232,  -282,    51,    95,   198,   205,   211,
     213,    19,   215,  -282,   229,   137,  -282,   273,   235,   105,
     846,   846,   225,   846,  -282,  -282,  -282,   924,   192,   218,
     283,   607,   248,  -282,  -282,  -282,  -282,  -282,   146,   250,
    -282,   263,   637,   292,   694,   846,   245,  -282,  -282,    11,
    -282,   846,   846,   846,   846,   846,   846,   846,   846,   846,
     846,   846,   846,   846,   846,   846,   846,   846,   846,   846,
     846,   724,   846,  -282,   267,  -282,   694,  -282,  -282,    11,
      88,  -282,   846,  -282,   846,   305,    13,  -282,  -282,  -282,
    -282,  -282,    37,    39,   328,  -282,   264,   182,   -21,   751,
      49,   266,  1227,  1266,   268,  -282,   309,  -282,   269,   270,
    -282,   282,  -282,   857,  -282,   103,  1493,  -282,   846,  -282,
     272,   960,   275,    -9,   248,   273,   793,  -282,   113,  -282,
     276,   996,  1493,   846,   804,   315,   318,   300,  1705,  1732,
    1759,  1786,  1813,  1813,   274,   274,   274,   274,   405,   262,
     262,   192,   192,   192,   192,  1821,  1493,  1457,   846,  1145,
     300,   267,  -282,  -282,   846,  1493,  1493,  -282,  -282,   329,
     290,  -282,   193,   291,   194,  -282,    -7,   229,   278,  -282,
     118,    20,  -282,  1032,   580,     6,  -282,   751,   248,   538,
     538,   846,    44,   846,   846,   846,  -282,  -282,   857,   857,
    1493,  -282,   846,  -282,  -282,   229,   121,  -282,  -282,  -282,
    1493,  -282,  -282,  -282,  1527,   846,  1563,  -282,  -282,   846,
    1383,  -282,   846,  -282,  -282,   337,  -282,   338,  -282,  -282,
    -282,  -282,   135,  -282,  -282,   229,   278,   278,   340,  -282,
    -282,  -282,   793,  -282,    -4,   302,   229,   176,  1068,   289,
    -282,  1305,   846,  1493,   303,  -282,  1493,  -282,  -282,  1493,
    -282,   229,   846,  1599,   846,  1670,  -282,  1186,  -282,  -282,
    -282,  -282,  -282,  -282,    16,   319,   316,  -282,   793,   793,
    -282,   229,  -282,   538,   308,  1344,   846,  -282,  1635,   846,
    1635,  -282,   846,   325,   793,  -282,   320,  -282,  -282,  -282,
    -282,   538,   321,   270,  1635,  1420,   793,  -282,  -282,  -282,
     538,  -282,  -282,  -282
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -282,  -282,   293,  -282,    10,   307,  -282,   333,    75,   212,
     353,    -6,  -282,   351,   304,   -13,   352,   -20,  -282,  -282,
     188,  -281,  -210,   322,   216,  -282,  -282,  -154,  -282,  -225,
    -282,  -282,   298,  -282,  -123,   301,  -282,    -5,  -282,  -125,
     271,  -282,  -282,   -19,   -79,  -163,  -282,  -282,  -282,    61
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -45
static const yytype_int16 yytable[] =
{
      52,    53,    54,    55,   167,    56,    82,    60,    77,   222,
      67,    70,    35,   294,     4,    92,    61,   335,    96,   157,
      68,   237,    58,     4,    87,    66,    88,   238,    71,   266,
       3,    75,    78,   265,   303,   291,   337,     9,   140,   238,
     229,   238,   229,   141,   338,   142,   239,    35,   306,     9,
     256,    73,    12,   353,   238,    92,    74,   236,   239,   160,
     239,   156,    90,   -44,   230,    19,   233,    35,    22,   152,
     166,   332,   333,   239,   293,   162,   163,    30,    62,   312,
      31,    32,   292,    62,   176,   236,   181,    69,   244,   156,
     245,   307,   336,    35,   182,   146,   283,   176,   147,   191,
     192,    89,   320,    90,   197,    91,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   219,   220,   356,   357,
     224,   181,   331,   182,   223,   317,   318,   225,    90,   226,
     228,   248,   148,   340,   367,   257,   158,   258,     4,    80,
     159,   259,     5,     6,   243,   271,   372,   258,   347,    84,
     295,   259,   296,   321,    35,   296,   297,    81,   176,   297,
       7,     8,    83,   260,     9,   315,    10,   330,   358,   296,
      90,   270,   156,   297,    11,   137,    12,   183,   274,   276,
      85,   156,    13,    98,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    97,    24,    25,    26,    27,    28,
      29,    30,   135,   280,    31,    32,    33,    34,   341,   284,
     296,   171,   127,   236,   297,     5,     6,   156,   298,   -34,
     309,   310,    89,   139,    90,   145,    91,   287,   287,   270,
     288,   290,   308,     7,     8,   149,   311,     9,   313,    10,
     316,    75,   150,   176,   176,   193,   194,   319,   151,    12,
     172,   102,   -34,   103,   153,   104,   173,   167,    11,    89,
     323,    90,    19,   136,   325,    22,   157,   327,    90,   106,
     107,   108,   109,   110,    30,   164,   177,    31,    32,   124,
     125,   126,   127,   182,   185,   189,   186,   270,   121,   122,
     123,   124,   125,   126,   127,   128,   131,   345,   227,   251,
     179,   247,   252,   131,   254,   255,   253,   348,   261,   350,
     277,   264,   272,   278,   359,   122,   123,   124,   125,   126,
     127,     4,   285,   270,   270,     5,     6,   286,   289,   131,
     328,   229,   369,   334,   364,   239,   339,   365,   343,   270,
     346,   373,   354,     7,     8,   360,   355,     9,   366,    10,
     368,   270,   329,   370,   138,   234,   154,    11,   235,    12,
      95,    72,    76,   267,    79,    13,   246,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,   165,    24,    25,
      26,    27,    28,    29,    30,     0,   155,    31,    32,    33,
      34,     4,   190,   188,   221,     5,     6,   363,     0,     0,
     106,   107,   108,   109,   110,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     0,     0,     9,     0,    10,
     122,   123,   124,   125,   126,   127,     0,    11,     0,    12,
       0,     0,     0,     0,   131,    13,     0,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,     0,    24,    25,
      26,    27,    28,    29,    30,     0,     0,    31,    32,    33,
      34,     4,     0,     0,     0,     5,     6,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     0,     0,     9,     0,    10,
       0,     0,     0,     0,     0,     0,     0,    11,     0,    12,
       0,     0,     0,     0,     0,    13,     0,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,     0,    24,    25,
      26,    27,    28,    29,    30,     0,     0,    31,    32,    93,
      94,     4,     0,     0,     0,     5,     6,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     8,     0,     0,     9,     0,    10,
       0,     0,     0,     0,     0,     0,     0,    11,     0,    12,
       0,     0,     0,     4,     0,    13,     0,     5,     6,     0,
       0,     0,    19,     0,     0,    22,    23,     0,    24,    25,
      26,    27,    28,    29,    30,     7,     8,    31,    32,     9,
       4,    10,     0,     0,     5,     6,     0,     0,     0,   244,
     300,    12,     0,     0,   301,     0,     0,     0,   302,     0,
       0,     0,     7,     8,    19,     0,     9,    22,    10,     0,
     171,     0,   178,     0,     5,     6,    30,     0,    12,    31,
      32,     0,     0,   179,     0,     0,     0,     0,     0,     0,
       0,    19,     7,     8,    22,     0,     9,     4,    10,     0,
       0,     5,     6,    30,     0,     0,    31,    32,    12,   187,
       0,     0,     0,     0,     0,   173,     0,     0,     0,     7,
       8,    19,     0,     9,    22,    10,     0,     4,     0,     0,
       0,     5,     6,    30,     0,    12,    31,    32,     0,     0,
       0,    86,     0,     0,     0,     0,     0,     0,    19,     7,
       8,    22,     0,     9,     0,    10,     0,     4,     0,   178,
      30,     5,     6,    31,    32,    12,     0,   218,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    19,     7,
       8,    22,     0,     9,     4,    10,     0,     0,     5,     6,
      30,     0,     0,    31,    32,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     7,     8,    19,     0,
       9,    22,    10,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    12,    31,    32,     0,     4,   179,     0,     0,
       5,     6,     0,     0,     0,    19,     0,     4,    22,     0,
       0,     5,     6,     0,     0,     0,   275,    30,     7,     8,
      31,    32,     9,     0,    10,     0,     0,     0,     0,     7,
       8,     0,   244,     9,    12,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    12,     0,    19,     0,     4,
      22,     0,     0,     5,     6,     0,     0,     0,    19,    30,
     171,    22,    31,    32,     5,     6,     0,     0,     0,     0,
      30,     7,     8,    31,    32,     9,     0,    10,     0,     0,
       0,     0,     7,     8,     0,     0,     9,    12,    10,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,     0,
      19,     0,     0,    22,     0,     0,     0,     0,     0,     0,
       0,    19,    30,     0,    22,    31,    32,     0,     0,     0,
       0,     0,     0,    30,     0,     0,    31,    32,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,     0,   129,   130,     0,
       0,     0,     0,   131,   105,   106,   107,   108,   109,   110,
       0,   170,     0,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,     0,   129,   130,   262,     0,     0,     0,   131,
     105,   106,   107,   108,   109,   110,   263,     0,     0,     0,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,     0,   129,
     130,   262,     0,     0,     0,   131,   105,   106,   107,   108,
     109,   110,   273,     0,     0,     0,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,     0,   129,   130,     0,     0,     0,
       0,   131,   105,   106,   107,   108,   109,   110,   299,     0,
       0,     0,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
       0,   129,   130,     0,     0,     0,     0,   131,   105,   106,
     107,   108,   109,   110,   342,     0,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,     0,   129,   130,     0,
       0,     0,     0,   131,     0,     0,   143,     0,   144,   105,
     106,   107,   108,   109,   110,     0,     0,     0,     0,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,     0,   129,   130,
       0,     0,     0,     0,   131,   281,     0,     0,     0,   282,
     105,   106,   107,   108,   109,   110,     0,     0,     0,     0,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,     0,   129,
     130,     0,     0,     0,     0,   131,   351,     0,     0,     0,
     352,   105,   106,   107,   108,   109,   110,     0,     0,     0,
       0,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,     0,
     129,   130,     0,     0,     0,     0,   131,     0,     0,   249,
     105,   106,   107,   108,   109,   110,     0,     0,     0,     0,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,     0,   129,
     130,     0,     0,     0,     0,   131,     0,     0,   250,   105,
     106,   107,   108,   109,   110,     0,     0,     0,     0,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,     0,   129,   130,
       0,     0,     0,     0,   131,     0,     0,   344,   105,   106,
     107,   108,   109,   110,     0,     0,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,     0,   129,   130,     0,
       0,     0,     0,   131,     0,     0,   361,   105,   106,   107,
     108,   109,   110,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,     0,   129,   130,     0,     0,
       0,     0,   131,   326,   105,   106,   107,   108,   109,   110,
       0,     0,     0,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,     0,   129,   130,     0,     0,     0,     0,   131,
     371,   105,   106,   107,   108,   109,   110,     0,     0,     0,
       0,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,     0,
     129,   130,   279,     0,     0,     0,   131,   105,   106,   107,
     108,   109,   110,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,     0,   129,   130,     0,     0,
       0,   105,   131,   107,   108,   109,   110,     0,     0,     0,
       0,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,     0,
     129,   130,     0,     0,     0,   322,   131,   105,     0,   107,
     108,   109,   110,     0,     0,     0,     0,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,     0,   129,   130,     0,     0,
       0,   324,   131,   105,     0,   107,   108,   109,   110,     0,
       0,     0,     0,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,     0,   129,   130,     0,     0,     0,   349,   131,   105,
       0,   107,   108,   109,   110,     0,     0,     0,     0,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,     0,   129,   130,
       0,     0,     0,     0,   131,   106,   107,   108,   109,   110,
       0,     0,     0,     0,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,     0,     0,   130,     0,     0,     0,     0,   131,
     106,   107,   108,   109,   110,     0,     0,     0,     0,     0,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   106,   107,   108,
     109,   110,     0,     0,   131,     0,     0,     0,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   106,   107,   108,   109,   110,     0,
       0,   131,     0,     0,     0,     0,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   106,   107,   108,   109,   110,     0,     0,   131,     0,
       0,     0,     0,     0,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   106,   107,
     108,   109,   110,     0,     0,   131,   106,   107,   108,   109,
     110,     0,     0,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   121,   122,   123,   124,   125,
     126,   127,   131,     0,     0,     0,     0,     0,     0,     0,
     131
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-282))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       5,     6,     7,     8,    83,    10,    25,    12,    21,   134,
      16,     3,     2,   238,     3,    35,     3,   298,    37,     3,
       3,    42,    12,     3,    29,    15,    31,    48,    18,   183,
       0,    21,    22,    42,   244,    42,    40,    29,    58,    48,
       3,    48,     3,    42,    48,    44,    67,    37,    42,    29,
     173,    39,    41,   334,    48,    75,     3,    41,    67,    79,
      67,    45,    43,     3,    27,    54,    27,    57,    57,    50,
      83,   296,   297,    67,   237,    80,    81,    66,    70,    35,
      69,    70,   236,    70,    89,    41,    91,    70,    39,    45,
      41,   245,   302,    83,    45,    44,   221,   102,    47,   104,
     105,    41,   265,    43,   109,    45,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   338,   339,
      42,   136,   295,    45,   139,   258,   259,   142,    43,   144,
     146,   161,    47,   306,   354,    42,    41,    44,     3,    41,
      45,    48,     7,     8,   159,    42,   366,    44,   321,    47,
      42,    48,    44,    42,   154,    44,    48,    41,   173,    48,
      25,    26,    41,   178,    29,   254,    31,    42,   341,    44,
      43,   186,    45,    48,    39,    40,    41,    41,   193,   194,
      47,    45,    47,     3,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    47,    60,    61,    62,    63,    64,
      65,    66,    47,   218,    69,    70,    71,    72,    42,   224,
      44,     3,    30,    41,    48,     7,     8,    45,   241,     3,
     249,   250,    41,    42,    43,     3,    45,    44,    44,   244,
      47,    47,   247,    25,    26,    47,   251,    29,   253,    31,
     255,   241,    47,   258,   259,    10,    11,   262,    47,    41,
      42,    41,    49,    43,    49,    45,    48,   346,    39,    41,
     275,    43,    54,    45,   279,    57,     3,   282,    43,     5,
       6,     7,     8,     9,    66,    60,     3,    69,    70,    27,
      28,    29,    30,    45,    44,     3,    33,   302,    24,    25,
      26,    27,    28,    29,    30,    31,    39,   312,     3,    41,
      46,    45,     3,    39,    44,    33,    47,   322,    46,   324,
       5,    46,    46,     5,   343,    25,    26,    27,    28,    29,
      30,     3,     3,   338,   339,     7,     8,    47,    47,    39,
       3,     3,   361,     3,   349,    67,    44,   352,    59,   354,
      47,   370,    33,    25,    26,    47,    40,    29,    33,    31,
      40,   366,   287,    42,    57,   153,    73,    39,    40,    41,
      37,    18,    21,   185,    22,    47,   160,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    83,    60,    61,
      62,    63,    64,    65,    66,    -1,    74,    69,    70,    71,
      72,     3,   104,   102,   133,     7,     8,   346,    -1,    -1,
       5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    26,    -1,    -1,    29,    -1,    31,
      25,    26,    27,    28,    29,    30,    -1,    39,    -1,    41,
      -1,    -1,    -1,    -1,    39,    47,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      62,    63,    64,    65,    66,    -1,    -1,    69,    70,    71,
      72,     3,    -1,    -1,    -1,     7,     8,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    26,    -1,    -1,    29,    -1,    31,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,    41,
      -1,    -1,    -1,    -1,    -1,    47,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    60,    61,
      62,    63,    64,    65,    66,    -1,    -1,    69,    70,    71,
      72,     3,    -1,    -1,    -1,     7,     8,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    26,    -1,    -1,    29,    -1,    31,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,    41,
      -1,    -1,    -1,     3,    -1,    47,    -1,     7,     8,    -1,
      -1,    -1,    54,    -1,    -1,    57,    58,    -1,    60,    61,
      62,    63,    64,    65,    66,    25,    26,    69,    70,    29,
       3,    31,    -1,    -1,     7,     8,    -1,    -1,    -1,    39,
      40,    41,    -1,    -1,    44,    -1,    -1,    -1,    48,    -1,
      -1,    -1,    25,    26,    54,    -1,    29,    57,    31,    -1,
       3,    -1,    35,    -1,     7,     8,    66,    -1,    41,    69,
      70,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    25,    26,    57,    -1,    29,     3,    31,    -1,
      -1,     7,     8,    66,    -1,    -1,    69,    70,    41,    42,
      -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    25,
      26,    54,    -1,    29,    57,    31,    -1,     3,    -1,    -1,
      -1,     7,     8,    66,    -1,    41,    69,    70,    -1,    -1,
      -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    54,    25,
      26,    57,    -1,    29,    -1,    31,    -1,     3,    -1,    35,
      66,     7,     8,    69,    70,    41,    -1,    13,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54,    25,
      26,    57,    -1,    29,     3,    31,    -1,    -1,     7,     8,
      66,    -1,    -1,    69,    70,    41,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    54,    -1,
      29,    57,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      66,    -1,    41,    69,    70,    -1,     3,    46,    -1,    -1,
       7,     8,    -1,    -1,    -1,    54,    -1,     3,    57,    -1,
      -1,     7,     8,    -1,    -1,    -1,    12,    66,    25,    26,
      69,    70,    29,    -1,    31,    -1,    -1,    -1,    -1,    25,
      26,    -1,    39,    29,    41,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    -1,    54,    -1,     3,
      57,    -1,    -1,     7,     8,    -1,    -1,    -1,    54,    66,
       3,    57,    69,    70,     7,     8,    -1,    -1,    -1,    -1,
      66,    25,    26,    69,    70,    29,    -1,    31,    -1,    -1,
      -1,    -1,    25,    26,    -1,    -1,    29,    41,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    -1,
      54,    -1,    -1,    57,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    54,    66,    -1,    57,    69,    70,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    -1,    -1,    69,    70,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    -1,    33,    34,    -1,
      -1,    -1,    -1,    39,     4,     5,     6,     7,     8,     9,
      -1,    47,    -1,    -1,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    -1,    33,    34,    35,    -1,    -1,    -1,    39,
       4,     5,     6,     7,     8,     9,    46,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    -1,    33,
      34,    35,    -1,    -1,    -1,    39,     4,     5,     6,     7,
       8,     9,    46,    -1,    -1,    -1,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    -1,    33,    34,    -1,    -1,    -1,
      -1,    39,     4,     5,     6,     7,     8,     9,    46,    -1,
      -1,    -1,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      -1,    33,    34,    -1,    -1,    -1,    -1,    39,     4,     5,
       6,     7,     8,     9,    46,    -1,    -1,    -1,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    -1,    33,    34,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    42,    -1,    44,     4,
       5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    -1,    33,    34,
      -1,    -1,    -1,    -1,    39,    40,    -1,    -1,    -1,    44,
       4,     5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    -1,    33,
      34,    -1,    -1,    -1,    -1,    39,    40,    -1,    -1,    -1,
      44,     4,     5,     6,     7,     8,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      33,    34,    -1,    -1,    -1,    -1,    39,    -1,    -1,    42,
       4,     5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    -1,    33,
      34,    -1,    -1,    -1,    -1,    39,    -1,    -1,    42,     4,
       5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    -1,    33,    34,
      -1,    -1,    -1,    -1,    39,    -1,    -1,    42,     4,     5,
       6,     7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    -1,    33,    34,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    42,     4,     5,     6,
       7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    33,    34,    -1,    -1,
      -1,    -1,    39,    40,     4,     5,     6,     7,     8,     9,
      -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    -1,    33,    34,    -1,    -1,    -1,    -1,    39,
      40,     4,     5,     6,     7,     8,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      33,    34,    35,    -1,    -1,    -1,    39,     4,     5,     6,
       7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    33,    34,    -1,    -1,
      -1,     4,    39,     6,     7,     8,     9,    -1,    -1,    -1,
      -1,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      33,    34,    -1,    -1,    -1,    38,    39,     4,    -1,     6,
       7,     8,     9,    -1,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    33,    34,    -1,    -1,
      -1,    38,    39,     4,    -1,     6,     7,     8,     9,    -1,
      -1,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    -1,    33,    34,    -1,    -1,    -1,    38,    39,     4,
      -1,     6,     7,     8,     9,    -1,    -1,    -1,    -1,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    -1,    33,    34,
      -1,    -1,    -1,    -1,    39,     5,     6,     7,     8,     9,
      -1,    -1,    -1,    -1,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    -1,    -1,    34,    -1,    -1,    -1,    -1,    39,
       5,     6,     7,     8,     9,    -1,    -1,    -1,    -1,    -1,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,     5,     6,     7,
       8,     9,    -1,    -1,    39,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,     5,     6,     7,     8,     9,    -1,
      -1,    39,    -1,    -1,    -1,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,     5,     6,     7,     8,     9,    -1,    -1,    39,    -1,
      -1,    -1,    -1,    -1,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,     5,     6,
       7,     8,     9,    -1,    -1,    39,     5,     6,     7,     8,
       9,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    24,    25,    26,    27,    28,
      29,    30,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      39
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    76,    77,     0,     3,     7,     8,    25,    26,    29,
      31,    39,    41,    47,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    60,    61,    62,    63,    64,    65,
      66,    69,    70,    71,    72,    79,    80,    81,    82,    88,
      89,    90,    91,    98,   105,   106,   108,   112,   118,   119,
     120,    79,   112,   112,   112,   112,   112,    78,    79,   111,
     112,     3,    70,    85,    86,    87,    79,    86,     3,    70,
       3,    79,    85,    39,     3,    79,    88,    90,    79,    91,
      41,    41,   118,    41,    47,    47,    47,   112,   112,    41,
      43,    45,    92,    71,    72,    82,   118,    47,     3,    94,
      95,    96,    41,    43,    45,     4,     5,     6,     7,     8,
       9,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    33,
      34,    39,   113,   114,   115,    47,    45,    40,    80,    42,
      92,    42,    44,    42,    44,     3,    44,    47,    47,    47,
      47,    47,    50,    49,    77,    98,    45,     3,    41,    45,
      92,    93,   112,   112,    60,    89,    90,   119,   121,   124,
      47,     3,    42,    48,   109,   110,   112,     3,    35,    46,
     107,   112,    45,    41,    92,    44,    33,    42,   110,     3,
     107,   112,   112,    10,    11,   116,   117,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,    13,   112,
     112,   115,   114,   112,    42,   112,   112,     3,    86,     3,
      27,    83,    84,    27,    84,    40,    41,    42,    48,    67,
     102,   103,   104,   112,    39,    41,    99,    45,    92,    42,
      42,    41,     3,    47,    44,    33,   109,    42,    44,    48,
     112,    46,    35,    46,    46,    42,   102,    95,    97,    99,
     112,    42,    46,    46,   112,    12,   112,     5,     5,    35,
     112,    40,    44,   114,   112,     3,    47,    44,    47,    47,
      47,    42,   102,   120,   104,    42,    44,    48,    90,    46,
      40,    44,    48,    97,   100,   101,    42,   102,   112,   118,
     118,   112,    35,   112,   122,   119,   112,   109,   109,   112,
     120,    42,    38,   112,    38,   112,    40,   112,     3,    83,
      42,   120,   104,   104,     3,    96,    97,    40,    48,    44,
     120,    42,    46,    59,    42,   112,    47,   120,   112,    38,
     112,    40,    44,    96,    33,    40,    97,    97,   120,   118,
      47,    42,   123,   124,   112,   112,    33,    97,    40,   118,
      42,    40,    97,   118
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (YYID (0))
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])



/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
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
  FILE *yyo = yyoutput;
  YYUSE (yyo);
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
                yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
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

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

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




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
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
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

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
  if (yypact_value_is_default (yyn))
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
/* Line 1787 of yacc.c  */
#line 191 "camp.y"
    { absyntax::root = (yyvsp[(1) - (1)].b); }
    break;

  case 3:
/* Line 1787 of yacc.c  */
#line 195 "camp.y"
    { (yyval.b) = new file(lexerPos(), false); }
    break;

  case 4:
/* Line 1787 of yacc.c  */
#line 197 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); }
    break;

  case 5:
/* Line 1787 of yacc.c  */
#line 201 "camp.y"
    { (yyval.b) = new block(lexerPos(), true); }
    break;

  case 6:
/* Line 1787 of yacc.c  */
#line 203 "camp.y"
    { (yyval.b) = (yyvsp[(1) - (2)].b); (yyval.b)->add((yyvsp[(2) - (2)].run)); }
    break;

  case 7:
/* Line 1787 of yacc.c  */
#line 207 "camp.y"
    { (yyval.n) = new simpleName((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); }
    break;

  case 8:
/* Line 1787 of yacc.c  */
#line 208 "camp.y"
    { (yyval.n) = new qualifiedName((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].n), (yyvsp[(3) - (3)].ps).sym); }
    break;

  case 9:
/* Line 1787 of yacc.c  */
#line 209 "camp.y"
    { (yyval.n) = new simpleName((yyvsp[(1) - (1)].ps).pos,
                                  symbol::trans("operator answer")); }
    break;

  case 10:
/* Line 1787 of yacc.c  */
#line 214 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].d); }
    break;

  case 11:
/* Line 1787 of yacc.c  */
#line 215 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].s); }
    break;

  case 12:
/* Line 1787 of yacc.c  */
#line 217 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].d)); }
    break;

  case 13:
/* Line 1787 of yacc.c  */
#line 219 "camp.y"
    { (yyval.run) = new modifiedRunnable((yyvsp[(1) - (2)].ml)->getPos(), (yyvsp[(1) - (2)].ml), (yyvsp[(2) - (2)].s)); }
    break;

  case 14:
/* Line 1787 of yacc.c  */
#line 223 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].mod).pos); (yyval.ml)->add((yyvsp[(1) - (1)].mod).val); }
    break;

  case 15:
/* Line 1787 of yacc.c  */
#line 224 "camp.y"
    { (yyval.ml) = new modifierList((yyvsp[(1) - (1)].perm).pos); (yyval.ml)->add((yyvsp[(1) - (1)].perm).val); }
    break;

  case 16:
/* Line 1787 of yacc.c  */
#line 226 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].mod).val); }
    break;

  case 17:
/* Line 1787 of yacc.c  */
#line 228 "camp.y"
    { (yyval.ml) = (yyvsp[(1) - (2)].ml); (yyval.ml)->add((yyvsp[(2) - (2)].perm).val); }
    break;

  case 18:
/* Line 1787 of yacc.c  */
#line 232 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].vd); }
    break;

  case 19:
/* Line 1787 of yacc.c  */
#line 233 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); }
    break;

  case 20:
/* Line 1787 of yacc.c  */
#line 234 "camp.y"
    { (yyval.d) = (yyvsp[(1) - (1)].d); }
    break;

  case 21:
/* Line 1787 of yacc.c  */
#line 236 "camp.y"
    { (yyval.d) = new accessdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ipl)); }
    break;

  case 22:
/* Line 1787 of yacc.c  */
#line 238 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), (yyvsp[(4) - (5)].ipl)); }
    break;

  case 23:
/* Line 1787 of yacc.c  */
#line 240 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].n), WILDCARD); }
    break;

  case 24:
/* Line 1787 of yacc.c  */
#line 241 "camp.y"
    { (yyval.d) = new unraveldec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].n), WILDCARD); }
    break;

  case 25:
/* Line 1787 of yacc.c  */
#line 243 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, (yyvsp[(4) - (5)].ipl)); }
    break;

  case 26:
/* Line 1787 of yacc.c  */
#line 245 "camp.y"
    { (yyval.d) = new fromaccessdec((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].ps).sym, WILDCARD); }
    break;

  case 27:
/* Line 1787 of yacc.c  */
#line 247 "camp.y"
    { (yyval.d) = new importdec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ip)); }
    break;

  case 28:
/* Line 1787 of yacc.c  */
#line 248 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym); }
    break;

  case 29:
/* Line 1787 of yacc.c  */
#line 250 "camp.y"
    { (yyval.d) = new includedec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].stre)->getString()); }
    break;

  case 30:
/* Line 1787 of yacc.c  */
#line 254 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); }
    break;

  case 31:
/* Line 1787 of yacc.c  */
#line 256 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); }
    break;

  case 32:
/* Line 1787 of yacc.c  */
#line 260 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); }
    break;

  case 33:
/* Line 1787 of yacc.c  */
#line 262 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); }
    break;

  case 34:
/* Line 1787 of yacc.c  */
#line 266 "camp.y"
    { (yyval.ps) = (yyvsp[(1) - (1)].ps); }
    break;

  case 35:
/* Line 1787 of yacc.c  */
#line 267 "camp.y"
    { (yyval.ps).pos = (yyvsp[(1) - (1)].stre)->getPos();
                     (yyval.ps).sym = symbol::literalTrans((yyvsp[(1) - (1)].stre)->getString()); }
    break;

  case 36:
/* Line 1787 of yacc.c  */
#line 272 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); }
    break;

  case 37:
/* Line 1787 of yacc.c  */
#line 274 "camp.y"
    { (yyval.ip) = new idpair((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, (yyvsp[(2) - (3)].ps).sym , (yyvsp[(3) - (3)].ps).sym); }
    break;

  case 38:
/* Line 1787 of yacc.c  */
#line 278 "camp.y"
    { (yyval.ipl) = new idpairlist(); (yyval.ipl)->add((yyvsp[(1) - (1)].ip)); }
    break;

  case 39:
/* Line 1787 of yacc.c  */
#line 280 "camp.y"
    { (yyval.ipl) = (yyvsp[(1) - (3)].ipl); (yyval.ipl)->add((yyvsp[(3) - (3)].ip)); }
    break;

  case 40:
/* Line 1787 of yacc.c  */
#line 284 "camp.y"
    { (yyval.vd) = (yyvsp[(1) - (2)].vd); }
    break;

  case 41:
/* Line 1787 of yacc.c  */
#line 288 "camp.y"
    { (yyval.vd) = new vardec((yyvsp[(1) - (2)].t)->getPos(), (yyvsp[(1) - (2)].t), (yyvsp[(2) - (2)].dil)); }
    break;

  case 42:
/* Line 1787 of yacc.c  */
#line 292 "camp.y"
    { (yyval.t) = (yyvsp[(1) - (1)].t); }
    break;

  case 43:
/* Line 1787 of yacc.c  */
#line 293 "camp.y"
    { (yyval.t) = new arrayTy((yyvsp[(1) - (2)].n), (yyvsp[(2) - (2)].dim)); }
    break;

  case 44:
/* Line 1787 of yacc.c  */
#line 297 "camp.y"
    { (yyval.t) = new nameTy((yyvsp[(1) - (1)].n)); }
    break;

  case 45:
/* Line 1787 of yacc.c  */
#line 301 "camp.y"
    { (yyval.dim) = new dimensions((yyvsp[(1) - (2)].pos)); }
    break;

  case 46:
/* Line 1787 of yacc.c  */
#line 302 "camp.y"
    { (yyval.dim) = (yyvsp[(1) - (3)].dim); (yyval.dim)->increase(); }
    break;

  case 47:
/* Line 1787 of yacc.c  */
#line 306 "camp.y"
    { (yyval.elist) = new explist((yyvsp[(1) - (3)].pos)); (yyval.elist)->add((yyvsp[(2) - (3)].e)); }
    break;

  case 48:
/* Line 1787 of yacc.c  */
#line 308 "camp.y"
    { (yyval.elist) = (yyvsp[(1) - (4)].elist); (yyval.elist)->add((yyvsp[(3) - (4)].e)); }
    break;

  case 49:
/* Line 1787 of yacc.c  */
#line 312 "camp.y"
    { (yyval.dil) = new decidlist((yyvsp[(1) - (1)].di)->getPos()); (yyval.dil)->add((yyvsp[(1) - (1)].di)); }
    break;

  case 50:
/* Line 1787 of yacc.c  */
#line 314 "camp.y"
    { (yyval.dil) = (yyvsp[(1) - (3)].dil); (yyval.dil)->add((yyvsp[(3) - (3)].di)); }
    break;

  case 51:
/* Line 1787 of yacc.c  */
#line 318 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (1)].dis)->getPos(), (yyvsp[(1) - (1)].dis)); }
    break;

  case 52:
/* Line 1787 of yacc.c  */
#line 320 "camp.y"
    { (yyval.di) = new decid((yyvsp[(1) - (3)].dis)->getPos(), (yyvsp[(1) - (3)].dis), (yyvsp[(3) - (3)].vi)); }
    break;

  case 53:
/* Line 1787 of yacc.c  */
#line 324 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); }
    break;

  case 54:
/* Line 1787 of yacc.c  */
#line 325 "camp.y"
    { (yyval.dis) = new decidstart((yyvsp[(1) - (2)].ps).pos, (yyvsp[(1) - (2)].ps).sym, (yyvsp[(2) - (2)].dim)); }
    break;

  case 55:
/* Line 1787 of yacc.c  */
#line 326 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym, 0,
                                            new formals((yyvsp[(2) - (3)].pos))); }
    break;

  case 56:
/* Line 1787 of yacc.c  */
#line 329 "camp.y"
    { (yyval.dis) = new fundecidstart((yyvsp[(1) - (4)].ps).pos, (yyvsp[(1) - (4)].ps).sym, 0, (yyvsp[(3) - (4)].fls)); }
    break;

  case 57:
/* Line 1787 of yacc.c  */
#line 333 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].e); }
    break;

  case 58:
/* Line 1787 of yacc.c  */
#line 334 "camp.y"
    { (yyval.vi) = (yyvsp[(1) - (1)].ai); }
    break;

  case 59:
/* Line 1787 of yacc.c  */
#line 339 "camp.y"
    { (yyval.b) = (yyvsp[(2) - (3)].b); }
    break;

  case 60:
/* Line 1787 of yacc.c  */
#line 343 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (2)].pos)); }
    break;

  case 61:
/* Line 1787 of yacc.c  */
#line 345 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (4)].pos)); (yyval.ai)->addRest((yyvsp[(3) - (4)].vi)); }
    break;

  case 62:
/* Line 1787 of yacc.c  */
#line 347 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (3)].ai); }
    break;

  case 63:
/* Line 1787 of yacc.c  */
#line 349 "camp.y"
    { (yyval.ai) = (yyvsp[(2) - (5)].ai); (yyval.ai)->addRest((yyvsp[(4) - (5)].vi)); }
    break;

  case 64:
/* Line 1787 of yacc.c  */
#line 353 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].pos)); }
    break;

  case 65:
/* Line 1787 of yacc.c  */
#line 354 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (1)].ai); }
    break;

  case 66:
/* Line 1787 of yacc.c  */
#line 355 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (2)].ai); }
    break;

  case 67:
/* Line 1787 of yacc.c  */
#line 359 "camp.y"
    { (yyval.ai) = new arrayinit((yyvsp[(1) - (1)].vi)->getPos());
		     (yyval.ai)->add((yyvsp[(1) - (1)].vi));}
    break;

  case 68:
/* Line 1787 of yacc.c  */
#line 362 "camp.y"
    { (yyval.ai) = (yyvsp[(1) - (3)].ai); (yyval.ai)->add((yyvsp[(3) - (3)].vi)); }
    break;

  case 69:
/* Line 1787 of yacc.c  */
#line 366 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (1)].fl)->getPos()); (yyval.fls)->add((yyvsp[(1) - (1)].fl)); }
    break;

  case 70:
/* Line 1787 of yacc.c  */
#line 367 "camp.y"
    { (yyval.fls) = new formals((yyvsp[(1) - (2)].pos)); (yyval.fls)->addRest((yyvsp[(2) - (2)].fl)); }
    break;

  case 71:
/* Line 1787 of yacc.c  */
#line 369 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->add((yyvsp[(3) - (3)].fl)); }
    break;

  case 72:
/* Line 1787 of yacc.c  */
#line 371 "camp.y"
    { (yyval.fls) = (yyvsp[(1) - (3)].fls); (yyval.fls)->addRest((yyvsp[(3) - (3)].fl)); }
    break;

  case 73:
/* Line 1787 of yacc.c  */
#line 375 "camp.y"
    { (yyval.boo) = true; }
    break;

  case 74:
/* Line 1787 of yacc.c  */
#line 376 "camp.y"
    { (yyval.boo) = false; }
    break;

  case 75:
/* Line 1787 of yacc.c  */
#line 381 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (2)].t)->getPos(), (yyvsp[(2) - (2)].t), 0, 0, (yyvsp[(1) - (2)].boo), 0); }
    break;

  case 76:
/* Line 1787 of yacc.c  */
#line 383 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (3)].t)->getPos(), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].dis), 0, (yyvsp[(1) - (3)].boo), 0); }
    break;

  case 77:
/* Line 1787 of yacc.c  */
#line 385 "camp.y"
    { (yyval.fl) = new formal((yyvsp[(2) - (5)].t)->getPos(), (yyvsp[(2) - (5)].t), (yyvsp[(3) - (5)].dis), (yyvsp[(5) - (5)].vi), (yyvsp[(1) - (5)].boo), 0); }
    break;

  case 78:
/* Line 1787 of yacc.c  */
#line 388 "camp.y"
    { bool k = checkKeyword((yyvsp[(3) - (4)].ps).pos, (yyvsp[(3) - (4)].ps).sym);
                     (yyval.fl) = new formal((yyvsp[(2) - (4)].t)->getPos(), (yyvsp[(2) - (4)].t), (yyvsp[(4) - (4)].dis), 0, (yyvsp[(1) - (4)].boo), k); }
    break;

  case 79:
/* Line 1787 of yacc.c  */
#line 391 "camp.y"
    { bool k = checkKeyword((yyvsp[(3) - (6)].ps).pos, (yyvsp[(3) - (6)].ps).sym);
                     (yyval.fl) = new formal((yyvsp[(2) - (6)].t)->getPos(), (yyvsp[(2) - (6)].t), (yyvsp[(4) - (6)].dis), (yyvsp[(6) - (6)].vi), (yyvsp[(1) - (6)].boo), k); }
    break;

  case 80:
/* Line 1787 of yacc.c  */
#line 397 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (5)].pos), (yyvsp[(1) - (5)].t), (yyvsp[(2) - (5)].ps).sym, new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); }
    break;

  case 81:
/* Line 1787 of yacc.c  */
#line 399 "camp.y"
    { (yyval.d) = new fundec((yyvsp[(3) - (6)].pos), (yyvsp[(1) - (6)].t), (yyvsp[(2) - (6)].ps).sym, (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); }
    break;

  case 82:
/* Line 1787 of yacc.c  */
#line 403 "camp.y"
    { (yyval.d) = new recorddec((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].b)); }
    break;

  case 83:
/* Line 1787 of yacc.c  */
#line 404 "camp.y"
    { (yyval.d) = new typedec((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].vd)); }
    break;

  case 84:
/* Line 1787 of yacc.c  */
#line 408 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (1)].pos), 0, 0); }
    break;

  case 85:
/* Line 1787 of yacc.c  */
#line 409 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (2)].pos), (yyvsp[(1) - (2)].e), 0); }
    break;

  case 86:
/* Line 1787 of yacc.c  */
#line 410 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(1) - (2)].pos), 0, (yyvsp[(2) - (2)].e)); }
    break;

  case 87:
/* Line 1787 of yacc.c  */
#line 411 "camp.y"
    { (yyval.slice) = new slice((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); }
    break;

  case 88:
/* Line 1787 of yacc.c  */
#line 415 "camp.y"
    { (yyval.e) = new fieldExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].ps).sym); }
    break;

  case 89:
/* Line 1787 of yacc.c  */
#line 416 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].e)); }
    break;

  case 90:
/* Line 1787 of yacc.c  */
#line 418 "camp.y"
    { (yyval.e) = new subscriptExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].e)); }
    break;

  case 91:
/* Line 1787 of yacc.c  */
#line 419 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos),
                              new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)), (yyvsp[(3) - (4)].slice)); }
    break;

  case 92:
/* Line 1787 of yacc.c  */
#line 421 "camp.y"
    { (yyval.e) = new sliceExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].slice)); }
    break;

  case 93:
/* Line 1787 of yacc.c  */
#line 422 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos),
                                      new nameExp((yyvsp[(1) - (3)].n)->getPos(), (yyvsp[(1) - (3)].n)),
                                      new arglist()); }
    break;

  case 94:
/* Line 1787 of yacc.c  */
#line 426 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), 
                                      new nameExp((yyvsp[(1) - (4)].n)->getPos(), (yyvsp[(1) - (4)].n)),
                                      (yyvsp[(3) - (4)].alist)); }
    break;

  case 95:
/* Line 1787 of yacc.c  */
#line 429 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), new arglist()); }
    break;

  case 96:
/* Line 1787 of yacc.c  */
#line 431 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(2) - (4)].pos), (yyvsp[(1) - (4)].e), (yyvsp[(3) - (4)].alist)); }
    break;

  case 97:
/* Line 1787 of yacc.c  */
#line 433 "camp.y"
    { (yyval.e) = (yyvsp[(2) - (3)].e); }
    break;

  case 98:
/* Line 1787 of yacc.c  */
#line 435 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(2) - (3)].n)->getPos(), (yyvsp[(2) - (3)].n)); }
    break;

  case 99:
/* Line 1787 of yacc.c  */
#line 436 "camp.y"
    { (yyval.e) = new thisExp((yyvsp[(1) - (1)].pos)); }
    break;

  case 100:
/* Line 1787 of yacc.c  */
#line 440 "camp.y"
    { (yyval.arg).name = symbol::nullsym; (yyval.arg).val=(yyvsp[(1) - (1)].e); }
    break;

  case 101:
/* Line 1787 of yacc.c  */
#line 441 "camp.y"
    { (yyval.arg).name = (yyvsp[(1) - (3)].ps).sym; (yyval.arg).val=(yyvsp[(3) - (3)].e); }
    break;

  case 102:
/* Line 1787 of yacc.c  */
#line 445 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->add((yyvsp[(1) - (1)].arg)); }
    break;

  case 103:
/* Line 1787 of yacc.c  */
#line 447 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->addRest((yyvsp[(2) - (2)].arg)); }
    break;

  case 104:
/* Line 1787 of yacc.c  */
#line 449 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->add((yyvsp[(3) - (3)].arg)); }
    break;

  case 105:
/* Line 1787 of yacc.c  */
#line 451 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->addRest((yyvsp[(3) - (3)].arg)); }
    break;

  case 106:
/* Line 1787 of yacc.c  */
#line 456 "camp.y"
    { (yyval.alist) = new arglist(); (yyval.alist)->add((yyvsp[(1) - (3)].e)); (yyval.alist)->add((yyvsp[(3) - (3)].e)); }
    break;

  case 107:
/* Line 1787 of yacc.c  */
#line 457 "camp.y"
    { (yyval.alist) = (yyvsp[(1) - (3)].alist); (yyval.alist)->add((yyvsp[(3) - (3)].e)); }
    break;

  case 108:
/* Line 1787 of yacc.c  */
#line 461 "camp.y"
    { (yyval.e) = new nameExp((yyvsp[(1) - (1)].n)->getPos(), (yyvsp[(1) - (1)].n)); }
    break;

  case 109:
/* Line 1787 of yacc.c  */
#line 462 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); }
    break;

  case 110:
/* Line 1787 of yacc.c  */
#line 463 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); }
    break;

  case 111:
/* Line 1787 of yacc.c  */
#line 464 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].stre); }
    break;

  case 112:
/* Line 1787 of yacc.c  */
#line 466 "camp.y"
    { (yyval.e) = new scaleExp((yyvsp[(1) - (2)].e)->getPos(), (yyvsp[(1) - (2)].e), (yyvsp[(2) - (2)].e)); }
    break;

  case 113:
/* Line 1787 of yacc.c  */
#line 468 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (4)].n)->getPos(), new nameTy((yyvsp[(2) - (4)].n)), (yyvsp[(4) - (4)].e)); }
    break;

  case 114:
/* Line 1787 of yacc.c  */
#line 470 "camp.y"
    { (yyval.e) = new castExp((yyvsp[(2) - (5)].n)->getPos(), new arrayTy((yyvsp[(2) - (5)].n), (yyvsp[(3) - (5)].dim)), (yyvsp[(5) - (5)].e)); }
    break;

  case 115:
/* Line 1787 of yacc.c  */
#line 472 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); }
    break;

  case 116:
/* Line 1787 of yacc.c  */
#line 474 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); }
    break;

  case 117:
/* Line 1787 of yacc.c  */
#line 475 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); }
    break;

  case 118:
/* Line 1787 of yacc.c  */
#line 476 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 119:
/* Line 1787 of yacc.c  */
#line 477 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 120:
/* Line 1787 of yacc.c  */
#line 478 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 121:
/* Line 1787 of yacc.c  */
#line 479 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 122:
/* Line 1787 of yacc.c  */
#line 480 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 123:
/* Line 1787 of yacc.c  */
#line 481 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 124:
/* Line 1787 of yacc.c  */
#line 482 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 125:
/* Line 1787 of yacc.c  */
#line 483 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 126:
/* Line 1787 of yacc.c  */
#line 484 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 127:
/* Line 1787 of yacc.c  */
#line 485 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 128:
/* Line 1787 of yacc.c  */
#line 486 "camp.y"
    { (yyval.e) = new equalityExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 129:
/* Line 1787 of yacc.c  */
#line 487 "camp.y"
    { (yyval.e) = new equalityExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 130:
/* Line 1787 of yacc.c  */
#line 488 "camp.y"
    { (yyval.e) = new andExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 131:
/* Line 1787 of yacc.c  */
#line 489 "camp.y"
    { (yyval.e) = new orExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 132:
/* Line 1787 of yacc.c  */
#line 490 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 133:
/* Line 1787 of yacc.c  */
#line 491 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 134:
/* Line 1787 of yacc.c  */
#line 492 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 135:
/* Line 1787 of yacc.c  */
#line 493 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 136:
/* Line 1787 of yacc.c  */
#line 494 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 137:
/* Line 1787 of yacc.c  */
#line 496 "camp.y"
    { (yyval.e) = new newRecordExp((yyvsp[(1) - (2)].pos), (yyvsp[(2) - (2)].t)); }
    break;

  case 138:
/* Line 1787 of yacc.c  */
#line 498 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), (yyvsp[(3) - (3)].elist), 0, 0); }
    break;

  case 139:
/* Line 1787 of yacc.c  */
#line 500 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), (yyvsp[(3) - (4)].elist), (yyvsp[(4) - (4)].dim), 0); }
    break;

  case 140:
/* Line 1787 of yacc.c  */
#line 502 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].t), 0, (yyvsp[(3) - (3)].dim), 0); }
    break;

  case 141:
/* Line 1787 of yacc.c  */
#line 504 "camp.y"
    { (yyval.e) = new newArrayExp((yyvsp[(1) - (4)].pos), (yyvsp[(2) - (4)].t), 0, (yyvsp[(3) - (4)].dim), (yyvsp[(4) - (4)].ai)); }
    break;

  case 142:
/* Line 1787 of yacc.c  */
#line 506 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (5)].pos), (yyvsp[(2) - (5)].t), new formals((yyvsp[(3) - (5)].pos)), (yyvsp[(5) - (5)].s)); }
    break;

  case 143:
/* Line 1787 of yacc.c  */
#line 508 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos),
                                             new arrayTy((yyvsp[(2) - (6)].t)->getPos(), (yyvsp[(2) - (6)].t), (yyvsp[(3) - (6)].dim)),
                                             new formals((yyvsp[(4) - (6)].pos)),
                                             (yyvsp[(6) - (6)].s)); }
    break;

  case 144:
/* Line 1787 of yacc.c  */
#line 513 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (6)].pos), (yyvsp[(2) - (6)].t), (yyvsp[(4) - (6)].fls), (yyvsp[(6) - (6)].s)); }
    break;

  case 145:
/* Line 1787 of yacc.c  */
#line 515 "camp.y"
    { (yyval.e) = new newFunctionExp((yyvsp[(1) - (7)].pos),
                                             new arrayTy((yyvsp[(2) - (7)].t)->getPos(), (yyvsp[(2) - (7)].t), (yyvsp[(3) - (7)].dim)),
                                             (yyvsp[(5) - (7)].fls),
                                             (yyvsp[(7) - (7)].s)); }
    break;

  case 146:
/* Line 1787 of yacc.c  */
#line 520 "camp.y"
    { (yyval.e) = new conditionalExp((yyvsp[(2) - (5)].pos), (yyvsp[(1) - (5)].e), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].e)); }
    break;

  case 147:
/* Line 1787 of yacc.c  */
#line 521 "camp.y"
    { (yyval.e) = new assignExp((yyvsp[(2) - (3)].pos), (yyvsp[(1) - (3)].e), (yyvsp[(3) - (3)].e)); }
    break;

  case 148:
/* Line 1787 of yacc.c  */
#line 522 "camp.y"
    { (yyval.e) = new callExp((yyvsp[(1) - (3)].pos), new nameExp((yyvsp[(1) - (3)].pos), SYM_TUPLE), (yyvsp[(2) - (3)].alist)); }
    break;

  case 149:
/* Line 1787 of yacc.c  */
#line 524 "camp.y"
    { (yyvsp[(2) - (3)].j)->pushFront((yyvsp[(1) - (3)].e)); (yyvsp[(2) - (3)].j)->pushBack((yyvsp[(3) - (3)].e)); (yyval.e) = (yyvsp[(2) - (3)].j); }
    break;

  case 150:
/* Line 1787 of yacc.c  */
#line 526 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::OUT);
                     joinExp *jexp =
                         new joinExp((yyvsp[(2) - (2)].se)->getPos(), SYM_DOTS);
                     (yyval.e)=jexp;
                     jexp->pushBack((yyvsp[(1) - (2)].e)); jexp->pushBack((yyvsp[(2) - (2)].se)); }
    break;

  case 151:
/* Line 1787 of yacc.c  */
#line 532 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), SYM_PLUS); }
    break;

  case 152:
/* Line 1787 of yacc.c  */
#line 534 "camp.y"
    { (yyval.e) = new prefixExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), SYM_MINUS); }
    break;

  case 153:
/* Line 1787 of yacc.c  */
#line 537 "camp.y"
    { (yyval.e) = new postfixExp((yyvsp[(2) - (2)].ps).pos, (yyvsp[(1) - (2)].e), SYM_PLUS); }
    break;

  case 154:
/* Line 1787 of yacc.c  */
#line 538 "camp.y"
    { (yyval.e) = new selfExp((yyvsp[(2) - (3)].ps).pos, (yyvsp[(1) - (3)].e), (yyvsp[(2) - (3)].ps).sym, (yyvsp[(3) - (3)].e)); }
    break;

  case 155:
/* Line 1787 of yacc.c  */
#line 540 "camp.y"
    { (yyval.e) = new quoteExp((yyvsp[(1) - (4)].pos), (yyvsp[(3) - (4)].b)); }
    break;

  case 156:
/* Line 1787 of yacc.c  */
#line 546 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos,(yyvsp[(1) - (1)].ps).sym); }
    break;

  case 157:
/* Line 1787 of yacc.c  */
#line 548 "camp.y"
    { (yyval.j) = (yyvsp[(1) - (1)].j); }
    break;

  case 158:
/* Line 1787 of yacc.c  */
#line 550 "camp.y"
    { (yyvsp[(1) - (2)].se)->setSide(camp::OUT);
                     (yyval.j) = (yyvsp[(2) - (2)].j); (yyval.j)->pushFront((yyvsp[(1) - (2)].se)); }
    break;

  case 159:
/* Line 1787 of yacc.c  */
#line 553 "camp.y"
    { (yyvsp[(2) - (2)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(1) - (2)].j); (yyval.j)->pushBack((yyvsp[(2) - (2)].se)); }
    break;

  case 160:
/* Line 1787 of yacc.c  */
#line 556 "camp.y"
    { (yyvsp[(1) - (3)].se)->setSide(camp::OUT); (yyvsp[(3) - (3)].se)->setSide(camp::IN);
                     (yyval.j) = (yyvsp[(2) - (3)].j); (yyval.j)->pushFront((yyvsp[(1) - (3)].se)); (yyval.j)->pushBack((yyvsp[(3) - (3)].se)); }
    break;

  case 161:
/* Line 1787 of yacc.c  */
#line 561 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(2) - (4)].ps).pos, (yyvsp[(2) - (4)].ps).sym, (yyvsp[(3) - (4)].e)); }
    break;

  case 162:
/* Line 1787 of yacc.c  */
#line 562 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (3)].pos), symbol::opTrans("spec"), (yyvsp[(2) - (3)].e)); }
    break;

  case 163:
/* Line 1787 of yacc.c  */
#line 564 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (5)].pos), symbol::opTrans("spec"),
				      new pairExp((yyvsp[(3) - (5)].pos), (yyvsp[(2) - (5)].e), (yyvsp[(4) - (5)].e))); }
    break;

  case 164:
/* Line 1787 of yacc.c  */
#line 567 "camp.y"
    { (yyval.se) = new specExp((yyvsp[(1) - (7)].pos), symbol::opTrans("spec"),
				      new tripleExp((yyvsp[(3) - (7)].pos), (yyvsp[(2) - (7)].e), (yyvsp[(4) - (7)].e), (yyvsp[(6) - (7)].e))); }
    break;

  case 165:
/* Line 1787 of yacc.c  */
#line 572 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); }
    break;

  case 166:
/* Line 1787 of yacc.c  */
#line 574 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); }
    break;

  case 167:
/* Line 1787 of yacc.c  */
#line 576 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(1) - (3)].ps).sym); (yyval.j)->pushBack((yyvsp[(2) - (3)].e)); }
    break;

  case 168:
/* Line 1787 of yacc.c  */
#line 577 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); }
    break;

  case 169:
/* Line 1787 of yacc.c  */
#line 578 "camp.y"
    { (yyval.j) = new joinExp((yyvsp[(1) - (1)].ps).pos, (yyvsp[(1) - (1)].ps).sym); }
    break;

  case 170:
/* Line 1787 of yacc.c  */
#line 582 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym,
                              new booleanExp((yyvsp[(1) - (2)].ps).pos, false)); }
    break;

  case 171:
/* Line 1787 of yacc.c  */
#line 585 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e),
                              new booleanExp((yyvsp[(1) - (4)].ps).pos, false)); }
    break;

  case 172:
/* Line 1787 of yacc.c  */
#line 588 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (3)].ps).pos, (yyvsp[(3) - (3)].e), (yyvsp[(1) - (3)].ps).sym,
                              new booleanExp((yyvsp[(2) - (3)].ps).pos, true)); }
    break;

  case 173:
/* Line 1787 of yacc.c  */
#line 591 "camp.y"
    { (yyval.e) = new ternaryExp((yyvsp[(1) - (5)].ps).pos, (yyvsp[(3) - (5)].e), (yyvsp[(1) - (5)].ps).sym, (yyvsp[(5) - (5)].e),
                              new booleanExp((yyvsp[(2) - (5)].ps).pos, true)); }
    break;

  case 174:
/* Line 1787 of yacc.c  */
#line 596 "camp.y"
    { (yyval.e) = new unaryExp((yyvsp[(1) - (2)].ps).pos, (yyvsp[(2) - (2)].e), (yyvsp[(1) - (2)].ps).sym); }
    break;

  case 175:
/* Line 1787 of yacc.c  */
#line 598 "camp.y"
    { (yyval.e) = new binaryExp((yyvsp[(1) - (4)].ps).pos, (yyvsp[(2) - (4)].e), (yyvsp[(1) - (4)].ps).sym, (yyvsp[(4) - (4)].e)); }
    break;

  case 176:
/* Line 1787 of yacc.c  */
#line 602 "camp.y"
    { (yyval.s) = new emptyStm((yyvsp[(1) - (1)].pos)); }
    break;

  case 177:
/* Line 1787 of yacc.c  */
#line 603 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (1)].s); }
    break;

  case 178:
/* Line 1787 of yacc.c  */
#line 604 "camp.y"
    { (yyval.s) = (yyvsp[(1) - (2)].s); }
    break;

  case 179:
/* Line 1787 of yacc.c  */
#line 606 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); }
    break;

  case 180:
/* Line 1787 of yacc.c  */
#line 608 "camp.y"
    { (yyval.s) = new ifStm((yyvsp[(1) - (7)].pos), (yyvsp[(3) - (7)].e), (yyvsp[(5) - (7)].s), (yyvsp[(7) - (7)].s)); }
    break;

  case 181:
/* Line 1787 of yacc.c  */
#line 610 "camp.y"
    { (yyval.s) = new whileStm((yyvsp[(1) - (5)].pos), (yyvsp[(3) - (5)].e), (yyvsp[(5) - (5)].s)); }
    break;

  case 182:
/* Line 1787 of yacc.c  */
#line 612 "camp.y"
    { (yyval.s) = new doStm((yyvsp[(1) - (7)].pos), (yyvsp[(2) - (7)].s), (yyvsp[(5) - (7)].e)); }
    break;

  case 183:
/* Line 1787 of yacc.c  */
#line 614 "camp.y"
    { (yyval.s) = new forStm((yyvsp[(1) - (9)].pos), (yyvsp[(3) - (9)].run), (yyvsp[(5) - (9)].e), (yyvsp[(7) - (9)].sel), (yyvsp[(9) - (9)].s)); }
    break;

  case 184:
/* Line 1787 of yacc.c  */
#line 616 "camp.y"
    { (yyval.s) = new extendedForStm((yyvsp[(1) - (8)].pos), (yyvsp[(3) - (8)].t), (yyvsp[(4) - (8)].ps).sym, (yyvsp[(6) - (8)].e), (yyvsp[(8) - (8)].s)); }
    break;

  case 185:
/* Line 1787 of yacc.c  */
#line 617 "camp.y"
    { (yyval.s) = new breakStm((yyvsp[(1) - (2)].pos)); }
    break;

  case 186:
/* Line 1787 of yacc.c  */
#line 618 "camp.y"
    { (yyval.s) = new continueStm((yyvsp[(1) - (2)].pos)); }
    break;

  case 187:
/* Line 1787 of yacc.c  */
#line 619 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (2)].pos)); }
    break;

  case 188:
/* Line 1787 of yacc.c  */
#line 620 "camp.y"
    { (yyval.s) = new returnStm((yyvsp[(1) - (3)].pos), (yyvsp[(2) - (3)].e)); }
    break;

  case 189:
/* Line 1787 of yacc.c  */
#line 624 "camp.y"
    { (yyval.s) = new expStm((yyvsp[(1) - (1)].e)->getPos(), (yyvsp[(1) - (1)].e)); }
    break;

  case 190:
/* Line 1787 of yacc.c  */
#line 628 "camp.y"
    { (yyval.s) = new blockStm((yyvsp[(1) - (1)].b)->getPos(), (yyvsp[(1) - (1)].b)); }
    break;

  case 191:
/* Line 1787 of yacc.c  */
#line 632 "camp.y"
    { (yyval.run) = 0; }
    break;

  case 192:
/* Line 1787 of yacc.c  */
#line 633 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].sel); }
    break;

  case 193:
/* Line 1787 of yacc.c  */
#line 634 "camp.y"
    { (yyval.run) = (yyvsp[(1) - (1)].vd); }
    break;

  case 194:
/* Line 1787 of yacc.c  */
#line 638 "camp.y"
    { (yyval.e) = 0; }
    break;

  case 195:
/* Line 1787 of yacc.c  */
#line 639 "camp.y"
    { (yyval.e) = (yyvsp[(1) - (1)].e); }
    break;

  case 196:
/* Line 1787 of yacc.c  */
#line 643 "camp.y"
    { (yyval.sel) = 0; }
    break;

  case 197:
/* Line 1787 of yacc.c  */
#line 644 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (1)].sel); }
    break;

  case 198:
/* Line 1787 of yacc.c  */
#line 648 "camp.y"
    { (yyval.sel) = new stmExpList((yyvsp[(1) - (1)].s)->getPos()); (yyval.sel)->add((yyvsp[(1) - (1)].s)); }
    break;

  case 199:
/* Line 1787 of yacc.c  */
#line 650 "camp.y"
    { (yyval.sel) = (yyvsp[(1) - (3)].sel); (yyval.sel)->add((yyvsp[(3) - (3)].s)); }
    break;


/* Line 1787 of yacc.c  */
#line 3388 "camp.tab.c"
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



