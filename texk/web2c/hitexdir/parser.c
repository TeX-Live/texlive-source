/* A Bison parser, made by GNU Bison 2.7.  */

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
#define YYBISON_VERSION "2.7"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 2 "shrink.y"

	#line 10216 "format.w"
	
#include "basetypes.h"
#include <string.h>
#include <math.h>
#include "error.h"
#include "hformat.h"
#include "hput.h"
extern char**hfont_name;

	/*356:*/
uint32_t definition_bits[0x100/32][32]= {
	#line 7578 "format.w"
	{0}};

#define SET_DBIT(N,K) ((N)>0xFF?1:(definition_bits[N/32][K]	|= (1<<((N)&(32-1)))))
#define GET_DBIT(N,K) ((N)>0xFF?1:((definition_bits[N/32][K]>>((N)&(32-1)))&1))
#define DEF(D,K,N) (D).k= K; (D).n= (N);SET_DBIT((D).n,(D).k);\
 DBG(DBGDEF,"Defining %s %d\n",definition_name[(D).k],(D).n);\
 RNG("Definition",(D).n,max_fixed[(D).k]+1,max_ref[(D).k]);
#define REF(K,N) REF_RNG(K,N);if(!GET_DBIT(N,K)) \
 QUIT("Reference %d to %s before definition",(N),definition_name[K])
	/*:356*/	/*360:*/
#define DEF_REF(D,K,M,N)  DEF(D,K,M);\
if ((M)>max_default[K]) QUIT("Defining non default reference %d for %s",M,definition_name[K]); \
if ((N)>max_fixed[K]) QUIT("Defining reference %d for %s by non fixed reference %d",M,definition_name[K],N);
	/*:360*/

extern void hset_entry(entry_t*e,uint16_t i,uint32_t size,
uint32_t xsize,char*file_name);

	/*423:*/
#ifdef DEBUG
#define  YYDEBUG 1
extern int yydebug;
#else
#define YYDEBUG 0
#endif
	/*:423*/
extern int yylex(void);

	/*352:*/
void hset_max(kind_t k,int n)
{
	#line 7421 "format.w"
	DBG(DBGDEF,"Setting max %s to %d\n",definition_name[k],n);
	RNG("Maximum",n,max_fixed[k]+1,MAX_REF(k));
	if(n>max_ref[k])
	max_ref[k]= n;
	}
	/*:352*/	/*363:*/
void check_param_def(ref_t*df)
{
	#line 7727 "format.w"
	if(df->k!=int_kind&&df->k!=dimen_kind&&df->k!=glue_kind)
	QUIT("Kind %s not allowed in parameter list",definition_name[df->k]);
	if(df->n<=max_fixed[df->k]||max_default[df->k]<df->n)
	QUIT("Parameter %d for %s not allowed in parameter list",df->n,definition_name[df->k]);
	}
	/*:363*/	/*422:*/
extern int yylineno;
int yyerror(const char*msg)
{
	#line 8802 "format.w"
	QUIT(" in line %d %s",yylineno,msg);
	return 0;
	}
	/*:422*/



/* Line 371 of yacc.c  */
#line 141 "shrink.tab.c"

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
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "shrink.tab.h".  */
#ifndef YY_YY_SHRINK_TAB_H_INCLUDED
# define YY_YY_SHRINK_TAB_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
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
     START = 258,
     END = 259,
     GLYPH = 260,
     UNSIGNED = 261,
     REFERENCE = 262,
     SIGNED = 263,
     STRING = 264,
     CHARCODE = 265,
     FPNUM = 266,
     DIMEN = 267,
     PT = 268,
     MM = 269,
     INCH = 270,
     XDIMEN = 271,
     H = 272,
     V = 273,
     FIL = 274,
     FILL = 275,
     FILLL = 276,
     PENALTY = 277,
     INTEGER = 278,
     LANGUAGE = 279,
     RULE = 280,
     RUNNING = 281,
     KERN = 282,
     EXPLICIT = 283,
     GLUE = 284,
     PLUS = 285,
     MINUS = 286,
     TXT_START = 287,
     TXT_END = 288,
     TXT_IGNORE = 289,
     TXT_FONT_GLUE = 290,
     TXT_FONT_HYPHEN = 291,
     TXT_FONT = 292,
     TXT_LOCAL = 293,
     TXT_GLOBAL = 294,
     TXT_CC = 295,
     HBOX = 296,
     VBOX = 297,
     SHIFTED = 298,
     HPACK = 299,
     HSET = 300,
     VPACK = 301,
     VSET = 302,
     DEPTH = 303,
     ADD = 304,
     TO = 305,
     LEADERS = 306,
     ALIGN = 307,
     CENTER = 308,
     EXPAND = 309,
     BASELINE = 310,
     LIGATURE = 311,
     DISC = 312,
     PAR = 313,
     MATH = 314,
     ON = 315,
     OFF = 316,
     ADJUST = 317,
     TABLE = 318,
     ITEM = 319,
     IMAGE = 320,
     LABEL = 321,
     BOT = 322,
     MID = 323,
     LINK = 324,
     OUTLINE = 325,
     STREAM = 326,
     STREAMDEF = 327,
     FIRST = 328,
     LAST = 329,
     TOP = 330,
     NOREFERENCE = 331,
     PAGE = 332,
     RANGE = 333,
     DIRECTORY = 334,
     SECTION = 335,
     DEFINITIONS = 336,
     MAX = 337,
     PARAM = 338,
     FONT = 339,
     CONTENT = 340
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 79 "shrink.y"

	#line 10241 "format.w"
	uint32_t u;  int32_t i;  char *s;  float64_t f;  glyph_t c;
	dimen_t d;stretch_t st;xdimen_t xd;kern_t kt;
	rule_t r;glue_t g;image_t x;
	list_t l;box_t h;disc_t dc;lig_t lg;
	ref_t rf;info_t info;order_t o;bool b;
	

/* Line 387 of yacc.c  */
#line 279 "shrink.tab.c"
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

#endif /* !YY_YY_SHRINK_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 307 "shrink.tab.c"

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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   657

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  106
/* YYNRULES -- Number of rules.  */
#define YYNRULES  266
/* YYNRULES -- Number of states.  */
#define YYNSTATES  566

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   340

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
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
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,    11,    13,    15,    17,    20,    22,
      24,    26,    28,    30,    33,    36,    39,    45,    49,    53,
      55,    60,    62,    64,    66,    68,    71,    73,    78,    80,
      82,    86,    91,    93,    94,    96,    99,   104,   105,   108,
     109,   112,   116,   121,   126,   127,   129,   132,   133,   135,
     140,   141,   147,   149,   152,   154,   156,   158,   160,   162,
     164,   166,   167,   170,   174,   175,   178,   179,   182,   185,
     190,   195,   200,   202,   204,   207,   213,   219,   222,   225,
     228,   231,   235,   236,   244,   249,   254,   259,   264,   266,
     268,   269,   271,   273,   275,   279,   283,   287,   292,   293,
     298,   303,   304,   307,   309,   311,   313,   314,   321,   326,
     328,   331,   335,   338,   340,   345,   347,   349,   353,   358,
     362,   363,   368,   373,   377,   382,   385,   389,   393,   396,
     400,   404,   408,   413,   418,   423,   425,   427,   429,   434,
     436,   441,   447,   452,   457,   462,   467,   470,   471,   476,
     481,   484,   486,   488,   490,   491,   497,   503,   511,   513,
     515,   519,   520,   525,   527,   529,   531,   533,   544,   549,
     551,   553,   556,   560,   563,   569,   570,   572,   573,   576,
     577,   578,   589,   595,   601,   605,   606,   613,   614,   617,
     623,   624,   631,   632,   635,   640,   641,   646,   649,   652,
     655,   658,   661,   664,   667,   670,   673,   676,   679,   682,
     685,   688,   691,   694,   697,   703,   709,   715,   721,   727,
     733,   739,   745,   751,   757,   763,   769,   775,   781,   787,
     793,   799,   801,   804,   807,   809,   810,   816,   819,   824,
     827,   830,   836,   842,   848,   854,   860,   866,   872,   878,
     880,   882,   884,   886,   891,   897,   904,   909,   914,   919,
     924,   929,   934,   939,   944,   949,   950
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     167,     0,    -1,     6,     7,    -1,    89,     5,    87,     4,
      -1,     3,    -1,     8,    -1,     6,    -1,    10,     7,    -1,
       9,    -1,    10,    -1,     6,    -1,     8,    -1,    11,    -1,
      92,    13,    -1,    92,    15,    -1,    92,    14,    -1,    93,
      92,    17,    92,    18,    -1,    93,    92,    17,    -1,    93,
      92,    18,    -1,    93,    -1,    89,    16,    94,     4,    -1,
      13,    -1,    19,    -1,    20,    -1,    21,    -1,    92,    96,
      -1,    90,    -1,    89,    22,    98,     4,    -1,    93,    -1,
      26,    -1,    99,    99,    99,    -1,    89,    25,   100,     4,
      -1,   101,    -1,    -1,    28,    -1,   102,    94,    -1,    89,
      27,   103,     4,    -1,    -1,    30,    97,    -1,    -1,    31,
      97,    -1,    94,   104,   105,    -1,    89,    29,   106,     4,
      -1,    89,    29,   106,     4,    -1,    -1,   108,    -1,   109,
      88,    -1,    -1,     6,    -1,    89,   110,   109,     4,    -1,
      -1,    32,   108,   112,   113,    33,    -1,   108,    -1,   113,
     114,    -1,    40,    -1,    37,    -1,    39,    -1,    38,    -1,
      35,    -1,    36,    -1,    34,    -1,    -1,   115,    88,    -1,
      93,    93,    93,    -1,    -1,    43,    93,    -1,    -1,    30,
      97,    -1,    31,    97,    -1,   116,   117,   118,   111,    -1,
      89,    41,   119,     4,    -1,    89,    42,   119,     4,    -1,
     120,    -1,   121,    -1,   104,   105,    -1,   116,   117,   122,
     187,   111,    -1,   116,   117,   122,    95,   111,    -1,    50,
     187,    -1,    49,   187,    -1,    50,    95,    -1,    49,    95,
      -1,   117,   124,   111,    -1,    -1,   117,    82,    48,    93,
     127,   124,   111,    -1,    89,    47,   123,     4,    -1,    89,
      46,   126,     4,    -1,    89,    45,   123,     4,    -1,    89,
      44,   125,     4,    -1,   128,    -1,   129,    -1,    -1,    52,
      -1,    53,    -1,    54,    -1,   107,   130,   101,    -1,   107,
     130,   120,    -1,   107,   130,   121,    -1,    89,    51,   131,
       4,    -1,    -1,    93,   133,   107,   107,    -1,    89,    55,
     132,     4,    -1,    -1,   134,    40,    -1,     6,    -1,    10,
      -1,     7,    -1,    -1,   136,   138,   135,    32,   134,    33,
      -1,    89,    56,   137,     4,    -1,   102,    -1,   102,     6,
      -1,   139,   111,   111,    -1,   139,   111,    -1,   139,    -1,
      89,    57,   140,     4,    -1,   141,    -1,    94,    -1,   187,
     188,   111,    -1,   187,   179,   180,   111,    -1,   187,   179,
     111,    -1,    -1,    94,   188,   144,   111,    -1,   142,   179,
     180,   111,    -1,   142,   179,   111,    -1,    89,    58,   143,
       4,    -1,   188,   111,    -1,   188,   111,   120,    -1,   188,
     120,   111,    -1,   179,   111,    -1,   179,   111,   120,    -1,
     179,   120,   111,    -1,   179,   180,   111,    -1,   179,   180,
     111,   120,    -1,   179,   180,   120,   111,    -1,    89,    59,
     145,     4,    -1,    60,    -1,    61,    -1,   146,    -1,    89,
      62,   111,     4,    -1,     6,    -1,    89,    64,    88,     4,
      -1,    89,    64,   147,    88,     4,    -1,    89,    64,   111,
       4,    -1,    17,   124,   111,   111,    -1,    18,   124,   111,
     111,    -1,    89,    63,   148,     4,    -1,    93,    93,    -1,
      -1,     6,   149,   104,   105,    -1,    89,    65,   150,     4,
      -1,    70,     6,    -1,    75,    -1,    67,    -1,    68,    -1,
      -1,     3,    66,     7,   152,     4,    -1,    89,    69,     7,
     146,     4,    -1,     3,    70,     7,    90,   108,   111,     4,
      -1,   136,    -1,    76,    -1,   154,   154,     6,    -1,    -1,
      95,     6,   157,   155,    -1,   156,    -1,    73,    -1,    74,
      -1,    75,    -1,    89,    72,   136,   158,   111,    95,   107,
     111,   107,     4,    -1,    89,    72,   136,     4,    -1,   159,
      -1,   160,    -1,   179,   111,    -1,   179,   180,   111,    -1,
     188,   111,    -1,    89,    71,   189,   161,     4,    -1,    -1,
       6,    -1,    -1,   163,   159,    -1,    -1,    -1,    91,   165,
     162,   107,    93,   166,    95,    95,   111,   163,    -1,     3,
      78,     7,    60,     4,    -1,     3,    78,     7,    61,     4,
      -1,   168,   172,   190,    -1,    -1,     3,    79,     6,   169,
     170,     4,    -1,    -1,   170,   171,    -1,     3,    80,     6,
      91,     4,    -1,    -1,     3,    81,   173,   175,   174,     4,
      -1,    -1,   174,   153,    -1,     3,    82,   176,     4,    -1,
      -1,   176,     3,   151,     4,    -1,    84,     6,    -1,    23,
       6,    -1,    12,     6,    -1,    56,     6,    -1,    57,     6,
      -1,    29,     6,    -1,    24,     6,    -1,    25,     6,    -1,
      65,     6,    -1,    51,     6,    -1,    55,     6,    -1,    16,
       6,    -1,    83,     6,    -1,    72,     6,    -1,    77,     6,
      -1,    78,     6,    -1,    66,     6,    -1,    89,    84,   136,
     182,     4,    -1,    89,    23,   136,    90,     4,    -1,    89,
      12,   136,    93,     4,    -1,    89,    24,   136,    91,     4,
      -1,    89,    29,   136,   106,     4,    -1,    89,    16,   136,
      94,     4,    -1,    89,    25,   136,   100,     4,    -1,    89,
      51,   136,   131,     4,    -1,    89,    55,   136,   132,     4,
      -1,    89,    56,   136,   137,     4,    -1,    89,    57,   136,
     140,     4,    -1,    89,    65,   136,   150,     4,    -1,    89,
      83,   136,   178,     4,    -1,    89,    77,   136,   164,     4,
      -1,    89,    23,   136,   136,     4,    -1,    89,    12,   136,
     136,     4,    -1,    89,    29,   136,   136,     4,    -1,   108,
      -1,   177,   153,    -1,   110,   177,    -1,   108,    -1,    -1,
      89,    83,   181,   178,     4,    -1,   183,   184,    -1,    91,
      93,     6,     6,    -1,   107,   141,    -1,   184,   185,    -1,
      89,    22,   186,    98,     4,    -1,    89,    27,   186,   103,
       4,    -1,    89,    56,   186,   137,     4,    -1,    89,    57,
     186,   140,     4,    -1,    89,    29,   186,   106,     4,    -1,
      89,    24,   186,    91,     4,    -1,    89,    25,   186,   100,
       4,    -1,    89,    65,   186,   150,     4,    -1,   136,    -1,
     136,    -1,   136,    -1,   136,    -1,    89,    22,   136,     4,
      -1,    89,    27,   102,   136,     4,    -1,    89,    27,   102,
      16,   136,     4,    -1,    89,    29,   136,     4,    -1,    89,
      56,   136,     4,    -1,    89,    57,   136,     4,    -1,    89,
      25,   136,     4,    -1,    89,    65,   136,     4,    -1,    89,
      51,   136,     4,    -1,    89,    55,   136,     4,    -1,    89,
      24,     7,     4,    -1,    89,    29,   136,     4,    -1,    -1,
       3,    85,   191,   109,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   264,   264,   267,   270,   274,   274,   278,   282,   282,
     288,   290,   292,   294,   297,   300,   304,   307,   310,   313,
     319,   324,   326,   328,   330,   334,   338,   341,   345,   345,
     348,   354,   357,   359,   361,   364,   367,   371,   373,   376,
     378,   381,   384,   388,   394,   397,   398,   399,   402,   405,
     412,   411,   420,   420,   422,   425,   428,   431,   434,   437,
     440,   443,   443,   448,   452,   455,   459,   462,   465,   470,
     474,   477,   480,   480,   482,   485,   488,   492,   495,   498,
     501,   505,   508,   508,   514,   517,   522,   525,   529,   529,
     531,   533,   535,   537,   540,   543,   546,   549,   553,   553,
     562,   567,   567,   570,   573,   576,   579,   579,   586,   590,
     593,   597,   601,   604,   609,   614,   616,   619,   622,   625,
     628,   628,   633,   636,   640,   644,   647,   650,   653,   656,
     659,   662,   665,   668,   672,   676,   678,   681,   685,   689,
     692,   695,   698,   702,   705,   709,   713,   715,   718,   721,
     725,   732,   734,   736,   738,   741,   746,   751,   761,   763,
     766,   769,   769,   773,   775,   777,   779,   783,   789,   794,
     794,   796,   799,   802,   805,   810,   813,   817,   817,   819,
     821,   819,   828,   831,   835,   837,   837,   840,   840,   841,
     846,   846,   853,   853,   855,   880,   880,   882,   885,   888,
     891,   894,   897,   900,   903,   906,   909,   912,   915,   918,
     921,   924,   927,   930,   936,   939,   942,   945,   948,   951,
     954,   957,   960,   963,   966,   969,   972,   975,   980,   983,
     986,   990,   991,   994,   998,  1001,  1001,  1009,  1011,  1016,
    1016,  1019,  1022,  1025,  1028,  1031,  1034,  1037,  1040,  1044,
    1048,  1051,  1054,  1060,  1063,  1067,  1071,  1074,  1077,  1080,
    1083,  1086,  1089,  1092,  1096,  1103,  1103
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\"<\"", "\">\"", "\"glyph\"",
  "UNSIGNED", "REFERENCE", "SIGNED", "STRING", "CHARCODE", "FPNUM",
  "\"dimen\"", "\"pt\"", "\"mm\"", "\"in\"", "\"xdimen\"", "\"h\"",
  "\"v\"", "\"fil\"", "\"fill\"", "\"filll\"", "\"penalty\"", "\"int\"",
  "\"language\"", "\"rule\"", "\"|\"", "\"kern\"", "\"!\"", "\"glue\"",
  "\"plus\"", "\"minus\"", "TXT_START", "TXT_END", "TXT_IGNORE",
  "TXT_FONT_GLUE", "TXT_FONT_HYPHEN", "TXT_FONT", "TXT_LOCAL",
  "TXT_GLOBAL", "TXT_CC", "\"hbox\"", "\"vbox\"", "\"shifted\"",
  "\"hpack\"", "\"hset\"", "\"vpack\"", "\"vset\"", "\"depth\"", "\"add\"",
  "\"to\"", "\"leaders\"", "\"align\"", "\"center\"", "\"expand\"",
  "\"baseline\"", "\"ligature\"", "\"disc\"", "\"par\"", "\"math\"",
  "\"on\"", "\"off\"", "\"adjust\"", "\"table\"", "\"item\"", "\"image\"",
  "\"label\"", "\"bot\"", "\"mid\"", "\"link\"", "\"outline\"",
  "\"stream\"", "\"stream (definition)\"", "\"first\"", "\"last\"",
  "\"top\"", "\"*\"", "\"page\"", "\"range\"", "\"directory\"",
  "\"entry\"", "\"definitions\"", "\"max\"", "\"param\"", "\"font\"",
  "\"content\"", "$accept", "glyph", "content_node", "start", "integer",
  "string", "number", "dimension", "xdimen", "xdimen_node", "order",
  "stretch", "penalty", "rule_dimension", "rule", "rule_node", "explicit",
  "kern", "plus", "minus", "glue", "glue_node", "position", "content_list",
  "estimate", "list", "$@1", "text", "txt", "$@2", "box_dimen",
  "box_shift", "box_glue_set", "box", "hbox_node", "vbox_node", "box_flex",
  "xbox", "box_goal", "hpack", "vpack", "$@3", "vxbox_node", "hxbox_node",
  "ltype", "leaders", "baseline", "$@4", "cc_list", "lig_cc", "ref",
  "ligature", "$@5", "replace_count", "disc", "disc_node", "par_dimen",
  "par", "$@6", "math", "on_off", "span_count", "table", "image_dimen",
  "image", "max_value", "placement", "def_node", "stream_link",
  "stream_split", "stream_info", "$@7", "stream_type", "stream_def_node",
  "stream_ins_node", "stream", "page_priority", "stream_def_list", "page",
  "$@8", "$@9", "hint", "directory_section", "$@10", "entry_list", "entry",
  "definition_section", "$@11", "definition_list", "max_definitions",
  "max_list", "def_list", "parameters", "empty_param_list",
  "non_empty_param_list", "$@12", "font", "font_head", "font_param_list",
  "font_param", "fref", "xdimen_ref", "param_ref", "stream_ref",
  "content_section", "$@13", YY_NULL
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
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    87,    88,    89,    90,    90,    87,    91,    91,
      92,    92,    92,    93,    93,    93,    94,    94,    94,    94,
      95,    96,    96,    96,    96,    97,    98,    88,    99,    99,
     100,   101,    88,   102,   102,   103,    88,   104,   104,   105,
     105,   106,    88,   107,   108,   109,   109,   110,   110,   111,
     112,   111,   113,   113,   114,   114,   114,   114,   114,   114,
     114,   115,   114,   116,   117,   117,   118,   118,   118,   119,
     120,   121,    88,    88,   122,   123,   123,   124,   124,   124,
     124,   125,   127,   126,   128,   128,   129,   129,    88,    88,
     130,   130,   130,   130,   131,   131,   131,    88,   133,   132,
      88,   134,   134,   135,   135,   136,   138,   137,    88,   139,
     139,   140,   140,   140,   141,    88,   142,   143,   143,   143,
     144,   143,   143,   143,    88,   145,   145,   145,   145,   145,
     145,   145,   145,   145,    88,   146,   146,   145,    88,   147,
      88,    88,    88,   148,   148,    88,   149,   149,   150,    88,
     151,   152,   152,   152,   152,    88,    88,   153,   154,   154,
     155,   157,   156,   158,   158,   158,   158,   159,   160,    88,
      88,   161,   161,   161,    88,   162,   162,   163,   163,   165,
     166,   164,    88,    88,   167,   169,   168,   170,   170,   171,
     173,   172,   174,   174,   175,   176,   176,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   153,   153,   153,   153,   153,   153,
     153,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   177,   177,   178,   179,   181,   180,   182,   183,   184,
     184,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     187,   188,   189,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,   107,   191,   190
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     4,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     2,     2,     2,     5,     3,     3,     1,
       4,     1,     1,     1,     1,     2,     1,     4,     1,     1,
       3,     4,     1,     0,     1,     2,     4,     0,     2,     0,
       2,     3,     4,     4,     0,     1,     2,     0,     1,     4,
       0,     5,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     0,     2,     3,     0,     2,     0,     2,     2,     4,
       4,     4,     1,     1,     2,     5,     5,     2,     2,     2,
       2,     3,     0,     7,     4,     4,     4,     4,     1,     1,
       0,     1,     1,     1,     3,     3,     3,     4,     0,     4,
       4,     0,     2,     1,     1,     1,     0,     6,     4,     1,
       2,     3,     2,     1,     4,     1,     1,     3,     4,     3,
       0,     4,     4,     3,     4,     2,     3,     3,     2,     3,
       3,     3,     4,     4,     4,     1,     1,     1,     4,     1,
       4,     5,     4,     4,     4,     4,     2,     0,     4,     4,
       2,     1,     1,     1,     0,     5,     5,     7,     1,     1,
       3,     0,     4,     1,     1,     1,     1,    10,     4,     1,
       1,     2,     3,     2,     5,     0,     1,     0,     2,     0,
       0,    10,     5,     5,     3,     0,     6,     0,     2,     5,
       0,     6,     0,     2,     4,     0,     4,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     5,     5,     5,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
       5,     1,     2,     2,     1,     0,     5,     2,     4,     2,
       2,     5,     5,     5,     5,     5,     5,     5,     5,     1,
       1,     1,     1,     4,     5,     6,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     0,     5
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,     0,     0,     0,     1,     0,     0,   185,   190,
       0,   184,   187,     0,   265,     0,     0,   192,    44,     0,
     186,   188,   195,     0,    45,     0,     0,     0,     4,   191,
       0,   193,     4,   266,    46,     0,    32,    72,    73,    88,
      89,   115,   169,   170,     0,     0,   194,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,     0,
       0,     0,    64,     0,    64,     0,     0,     0,     0,    33,
       0,    44,     0,     0,     0,     0,     0,     0,     0,     8,
       9,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   105,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,     0,     0,    47,     0,   154,     0,     0,
       0,     0,     6,     5,    26,     0,     0,     0,    10,    11,
      12,    29,     0,    28,     0,     0,     0,    34,     0,     0,
      19,    37,     0,     0,     0,    64,     0,     0,     0,     0,
       0,    64,     0,     0,     0,     0,     4,     0,    90,     0,
       0,    98,     0,     0,   106,     0,   109,     0,   113,     0,
     116,   250,    44,     0,    44,   135,   136,   234,   251,     0,
     137,     0,     0,    44,    47,     0,     0,     0,     0,   139,
       0,    47,     0,     0,   147,     0,     0,     0,   252,    44,
       0,   189,   199,   208,   198,   203,   204,   202,   206,   207,
     200,   201,   205,   213,   150,   210,   211,   212,   209,   197,
     196,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   106,     0,     0,     0,   179,     0,    48,
      44,     0,     0,     0,     0,   152,   153,   151,     0,     0,
       0,     2,     7,     3,    27,   253,   263,    13,    15,    14,
       0,    31,   259,     0,    35,     0,    36,     0,     0,    39,
      42,   256,     0,    66,    70,    71,    65,     0,     0,     0,
      87,    37,    86,     0,    85,    84,     0,    91,    92,    93,
       0,    97,   261,     0,   100,   262,   257,     0,   108,   110,
     258,   112,   114,   120,     0,   124,     0,     0,   134,    47,
     128,     0,     0,    47,   125,     0,    50,    44,   138,     0,
       0,   145,   140,   142,     0,     0,    37,   260,   149,     0,
       0,     0,     0,   168,   164,   165,   166,     0,     0,   163,
       0,     0,   216,   229,   219,   215,   228,   217,   220,   218,
     230,   221,   222,   223,   224,   225,   175,   227,   231,   233,
     226,     0,   214,     0,   237,   155,   182,   183,    30,     0,
     254,    17,    18,     0,    38,     0,    41,    63,     0,     0,
       0,    80,    78,    79,    77,    81,    39,     0,     0,     0,
       0,     0,    94,    95,    96,     0,   103,   104,     0,   111,
       0,    47,   123,     0,   119,     0,   117,   235,     0,   129,
     130,   131,     0,   126,   127,    44,     0,     0,     0,   141,
     146,    39,   156,   174,   171,     0,   173,     0,   161,     0,
       0,   176,     0,   232,     0,     0,   239,     0,   240,   255,
       0,    21,    22,    23,    24,    25,    40,    67,    68,    69,
      74,     0,     0,    82,    43,   264,     0,    99,   101,   121,
     122,   118,    47,   132,   133,    52,    61,    49,   143,   144,
     148,   172,     0,     0,     0,   157,     0,   238,    33,     0,
       0,     0,     0,     0,     0,     0,     0,    16,    76,    75,
       0,     0,     0,    51,    60,    58,    59,    55,    57,    56,
      54,    53,     0,    20,   159,   158,     0,   162,     0,   180,
     249,     0,     0,     0,    33,     0,     0,    33,     0,     0,
     107,   102,   236,    62,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    83,   160,     0,     0,
     241,   246,   247,   242,   245,   243,   244,   248,   167,     0,
     177,   181,     0,   178,     0,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   131,    34,   194,   134,    91,   142,   150,   151,   348,
     455,   384,   135,   144,   145,    36,   176,   149,   279,   386,
     152,   168,   187,    25,   327,   195,   425,   476,   511,   512,
     155,   159,   390,   156,    37,    38,   397,   162,   289,   160,
     164,   500,    39,    40,   300,   169,   172,   303,   501,   408,
     520,   175,   307,   178,   179,    41,   182,   183,   410,   189,
     190,   203,   198,   336,   206,   110,   258,    31,   516,   517,
     349,   483,   350,    42,    43,   340,   442,   561,   248,   366,
     536,     2,     3,    12,    15,    21,     7,    13,    23,    17,
      27,   369,   251,   191,   322,   472,   253,   254,   374,   448,
     521,   184,   192,   209,    11,    18
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -326
static const yytype_int16 yypact[] =
{
      48,    -4,   110,   113,   107,  -326,    52,   137,  -326,  -326,
      74,  -326,  -326,   153,  -326,   211,    85,  -326,  -326,    94,
    -326,  -326,  -326,   275,  -326,   278,   190,   280,   159,  -326,
     313,  -326,    31,  -326,  -326,   585,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,   223,   467,  -326,   203,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,   240,   248,   116,   219,   253,   140,   210,   229,
     182,   182,   220,   182,   220,   182,   121,   229,   234,    71,
     229,    41,    66,   271,    79,   301,   261,   234,   234,  -326,
    -326,   268,   270,   279,   284,   286,   288,   294,   297,   315,
     325,   326,   327,   328,   333,   334,   335,   337,   338,   339,
     343,   242,  -326,   229,   182,   219,   223,   152,   229,   345,
     182,   234,   210,   348,   223,   350,   223,    82,   251,   323,
     342,   353,  -326,  -326,  -326,   355,   357,   358,  -326,  -326,
    -326,  -326,   231,  -326,   152,   359,   361,  -326,   165,   362,
     182,   341,   372,   373,   182,   220,   375,   377,   182,   264,
     379,   220,   380,   305,   384,   385,  -326,   363,   200,   387,
     389,  -326,   390,   391,   395,   396,   398,   397,    66,   401,
     234,  -326,  -326,   404,   234,  -326,  -326,  -326,  -326,   405,
    -326,    66,    66,  -326,   350,   406,   264,   264,   407,  -326,
     408,   530,   409,   399,   182,   410,   411,   255,  -326,   234,
      80,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,   413,   414,   416,   417,   418,   419,   421,   422,
     423,   427,   429,  -326,   430,   432,   433,  -326,   435,  -326,
    -326,   436,   182,   437,   345,  -326,  -326,  -326,   438,   440,
     441,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
     152,  -326,  -326,   234,  -326,   442,  -326,   302,   182,   420,
    -326,  -326,   182,   293,  -326,  -326,  -326,   121,   121,    66,
    -326,   341,  -326,   400,  -326,  -326,   229,  -326,  -326,  -326,
     345,  -326,  -326,   345,  -326,  -326,  -326,   176,  -326,  -326,
    -326,    66,  -326,  -326,    66,  -326,    66,    66,  -326,    38,
     345,    66,    66,    53,   345,    66,  -326,  -326,  -326,    66,
      66,  -326,  -326,  -326,   445,   182,   341,  -326,  -326,   446,
     448,    66,    66,  -326,  -326,  -326,  -326,   439,   450,  -326,
      66,    66,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,   452,  -326,  -326,   451,
    -326,   453,  -326,   345,   345,  -326,  -326,  -326,  -326,   456,
    -326,   182,  -326,   164,  -326,   182,  -326,  -326,   182,   182,
      66,  -326,  -326,  -326,  -326,  -326,   420,   121,   182,   458,
     460,    49,  -326,  -326,  -326,   345,  -326,  -326,   434,  -326,
      66,    35,  -326,    66,  -326,    66,  -326,  -326,   424,  -326,
    -326,   345,    66,  -326,  -326,  -326,   324,    66,    66,  -326,
    -326,   420,  -326,  -326,  -326,    66,  -326,   182,  -326,   345,
     463,  -326,   345,  -326,   462,   412,  -326,   105,  -326,  -326,
     454,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,    66,    66,  -326,  -326,  -326,   152,  -326,  -326,  -326,
    -326,  -326,   350,  -326,  -326,  -326,   184,  -326,  -326,  -326,
    -326,  -326,   466,    39,   345,  -326,   182,  -326,   210,   234,
     234,   234,   234,   234,   234,   234,   234,  -326,  -326,  -326,
     264,    72,   471,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,   399,  -326,  -326,  -326,    39,  -326,    66,  -326,
    -326,   242,   223,   152,   210,   182,   234,   210,   348,    66,
    -326,  -326,  -326,  -326,   470,   345,   345,   473,   476,   478,
     182,   480,   481,   482,   483,   484,  -326,  -326,   489,   345,
    -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,    66,
    -326,   345,   425,  -326,   234,    69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -326,  -326,   -65,   -23,    97,   -79,   -83,   -13,   -62,  -232,
    -326,  -191,   -27,  -118,   -94,   195,   -67,   -26,  -228,  -325,
     -97,  -226,    -1,   172,   -86,   -55,  -326,  -326,  -326,  -326,
     198,   -36,  -326,   431,  -111,   201,  -326,   428,  -147,  -326,
    -326,  -326,  -326,  -326,  -326,   381,   386,  -326,  -326,  -326,
     -45,   -96,  -326,  -326,   -95,   132,  -326,  -326,  -326,  -326,
     300,  -326,  -326,  -326,   -93,  -326,  -326,   139,    -7,  -326,
    -326,  -326,  -326,   -50,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,  -326,
    -326,  -326,    42,   -89,  -227,  -326,  -326,  -326,  -326,  -326,
    -290,  -222,   -92,  -326,  -326,  -326
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
{
      30,   148,    35,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,    24,   180,   200,
     136,   239,   146,   238,   153,   244,   270,   245,   373,   202,
     246,   170,   173,   174,   177,   181,   188,   237,   163,   250,
     205,   249,   208,   210,   249,   247,   112,   252,   112,   329,
     330,     1,   234,   167,   143,   391,   393,   154,   154,   249,
     154,   201,   154,   396,   171,   392,   394,   277,   233,   166,
     236,   460,   166,   240,   466,     4,   243,   405,   112,    70,
     321,   325,    32,   166,   343,   199,   274,   413,   313,   415,
      70,    71,   317,   314,    70,   316,   167,    62,   193,   147,
     232,   185,   186,   275,   143,   530,   480,   171,   431,    63,
       5,   193,   531,     8,   435,   514,     6,   342,   417,   283,
     341,   417,   129,   311,   166,   291,   130,   489,   112,   490,
     491,   143,   492,     9,   493,   188,   320,   324,   334,   188,
      10,   282,   344,   345,   346,   286,   138,   112,   139,   255,
     256,   140,   378,   344,   345,   346,    16,   257,   138,    14,
     139,   494,   495,   140,   188,   461,   141,    22,   319,   323,
     496,   138,   112,   139,    26,   462,   140,   451,   141,   467,
      35,   273,   406,   452,   453,   454,   407,   347,   138,   403,
     139,   335,   326,   140,   456,   383,    44,   457,   458,   399,
     522,   523,   524,   525,   526,   527,   528,   484,   231,   419,
     111,   422,   235,   423,    19,    20,   486,   503,   504,   505,
     506,   507,   508,   509,   510,   132,   112,   133,   379,    47,
     351,   167,    89,    90,   395,   138,   112,   139,   147,   371,
     140,   112,   181,   181,   267,   268,   269,   127,   132,   368,
     133,   400,   297,   298,   299,   128,   409,   143,   518,   412,
     137,   414,   416,   158,   347,   347,   420,   421,   207,   387,
     424,   161,   211,   161,   427,   428,   212,   401,    28,    29,
     167,    32,    33,    45,    46,   213,   434,   436,   196,   197,
     214,   411,   215,   411,   216,   439,   440,   418,   450,   323,
     217,   418,   383,   218,   549,   383,   383,   204,   112,   548,
     473,   259,   260,   287,   288,   185,   186,   559,   411,   381,
     382,   219,   430,   388,   389,    48,    24,    32,   477,    49,
     261,   220,   221,   222,   223,   459,    50,    51,    52,   224,
     225,   226,    53,   227,   228,   229,    30,   230,   166,   262,
     445,   447,   181,   529,   204,   469,   249,   263,   470,   264,
     471,   265,   266,   271,    54,   272,   276,   474,    55,    56,
      57,   278,   478,   479,   347,   482,   280,   281,    58,   284,
     481,   285,   167,   290,   292,   463,   250,   293,   294,   295,
      59,   301,   296,   302,   304,   305,    60,    61,   418,   306,
     308,   310,    32,    35,   309,   312,   498,   499,   315,   318,
     328,   331,   332,   333,   337,   338,   347,   352,   353,   167,
     354,   355,   356,   357,   475,   358,   359,   360,   542,   539,
     543,   361,   544,   362,   363,   545,   364,   365,   515,   367,
     370,   372,   375,   538,   376,   377,   380,   533,   398,   429,
     432,   385,   433,   143,    28,   437,   438,   540,   441,   444,
     449,   167,   464,   535,   465,    70,   468,   485,   487,   488,
     513,   515,   497,   519,   546,   532,   547,   550,   274,    92,
     551,   243,   552,    93,   553,   554,   555,   556,   557,    35,
      94,    95,    96,   558,   537,   402,    97,   564,   541,   426,
     241,   404,   157,   165,   560,   446,   242,   339,   443,   534,
     143,   563,   167,   347,   502,     0,     0,     0,    98,   565,
       0,     0,    99,   100,   101,     0,   347,     0,     0,     0,
       0,     0,   102,   103,     0,    64,   249,   104,   562,   105,
       0,     0,   347,     0,   106,   107,     0,     0,     0,     0,
     108,   109,    65,     0,    66,    67,     0,    68,     0,    69,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    70,    71,     0,    72,    73,    74,    75,     0,     0,
       0,    76,     0,     0,     0,    77,    78,    79,    80,    81,
      64,     0,    82,    83,    84,    85,     0,     0,     0,    86,
       0,    87,    88,     0,     0,     0,     0,    65,     0,    66,
      67,     0,    68,     0,    69,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    70,    71,     0,    72,
      73,    74,    75,     0,     0,     0,    76,     0,     0,     0,
      77,    78,    79,    80,    81,     0,     0,    82,    83,    84,
      85,     0,     0,     0,    86,     0,    87,    88
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-326)))

#define yytable_value_is_error(Yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      23,    68,    25,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    18,    80,    84,
      65,   118,    67,   117,    69,   121,   144,   122,   254,    84,
     123,    76,    77,    78,    79,    80,    81,   116,    74,   125,
      85,     6,    87,    88,     6,   124,     7,   126,     7,   196,
     197,     3,   114,    76,    67,   287,   288,    70,    71,     6,
      73,    84,    75,   291,    77,   287,   288,   150,   113,     3,
     115,   396,     3,   118,    25,    79,   121,   303,     7,    41,
     191,   192,     3,     3,     4,     6,   148,   314,   180,   316,
      41,    42,   184,   182,    41,   184,   119,    66,    32,    28,
     113,    60,    61,   148,   117,    33,   431,   120,   336,    78,
       0,    32,    40,     6,   341,    76,     3,   209,    83,   155,
     209,    83,     6,   178,     3,   161,    10,    22,     7,    24,
      25,   144,    27,    81,    29,   180,   191,   192,   203,   184,
       3,   154,    73,    74,    75,   158,     6,     7,     8,    67,
      68,    11,   270,    73,    74,    75,     3,    75,     6,    85,
       8,    56,    57,    11,   209,   397,    26,    82,   191,   192,
      65,     6,     7,     8,    80,   397,    11,    13,    26,   405,
     203,    16,     6,    19,    20,    21,    10,   210,     6,   300,
       8,   204,   193,    11,   385,   278,     6,   388,   389,   296,
     490,   491,   492,   493,   494,   495,   496,   439,   111,   320,
       7,   322,   115,   324,     3,     4,   442,    33,    34,    35,
      36,    37,    38,    39,    40,     6,     7,     8,   273,    70,
     231,   254,     9,    10,   289,     6,     7,     8,    28,   252,
      11,     7,   287,   288,    13,    14,    15,     7,     6,   250,
       8,   296,    52,    53,    54,     7,   311,   270,   484,   314,
       7,   316,   317,    43,   287,   288,   321,   322,     7,   282,
     325,    73,     4,    75,   329,   330,     6,   300,     3,     4,
     303,     3,     4,     3,     4,     6,   341,   342,    17,    18,
       6,   314,     6,   316,     6,   350,   351,   320,   381,   322,
       6,   324,   385,     6,   536,   388,   389,     6,     7,   535,
     421,    60,    61,    49,    50,    60,    61,   549,   341,    17,
      18,     6,   335,    30,    31,    12,   327,     3,     4,    16,
       7,     6,     6,     6,     6,   390,    23,    24,    25,     6,
       6,     6,    29,     6,     6,     6,   369,     4,     3,     7,
     373,   374,   397,   500,     6,   410,     6,     4,   413,     4,
     415,     4,     4,     4,    51,     4,     4,   422,    55,    56,
      57,    30,   427,   428,   397,   437,     4,     4,    65,     4,
     435,     4,   405,     4,     4,   398,   472,    82,     4,     4,
      77,     4,    29,     4,     4,     4,    83,    84,   421,     4,
       4,     4,     3,   426,     6,     4,   461,   462,     4,     4,
       4,     4,     4,     4,     4,     4,   439,     4,     4,   442,
       4,     4,     4,     4,   425,     4,     4,     4,   525,   523,
     526,     4,   527,     4,     4,   528,     4,     4,   483,     4,
       4,     4,     4,   522,     4,     4,     4,   512,    48,     4,
       4,    31,     4,   466,     3,    16,     6,   524,     6,     6,
       4,   484,     4,   518,     4,    41,    32,     4,     6,    57,
       4,   516,    18,   486,   529,     4,     6,     4,   540,    12,
       4,   526,     4,    16,     4,     4,     4,     4,     4,   512,
      23,    24,    25,     4,   521,   300,    29,    72,   524,   327,
     119,   300,    71,    75,   559,   373,   120,   207,   369,   516,
     523,   561,   535,   536,   472,    -1,    -1,    -1,    51,   564,
      -1,    -1,    55,    56,    57,    -1,   549,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,     5,     6,    70,   561,    72,
      -1,    -1,   565,    -1,    77,    78,    -1,    -1,    -1,    -1,
      83,    84,    22,    -1,    24,    25,    -1,    27,    -1,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    42,    -1,    44,    45,    46,    47,    -1,    -1,
      -1,    51,    -1,    -1,    -1,    55,    56,    57,    58,    59,
       5,    -1,    62,    63,    64,    65,    -1,    -1,    -1,    69,
      -1,    71,    72,    -1,    -1,    -1,    -1,    22,    -1,    24,
      25,    -1,    27,    -1,    29,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    -1,    44,
      45,    46,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,
      55,    56,    57,    58,    59,    -1,    -1,    62,    63,    64,
      65,    -1,    -1,    -1,    69,    -1,    71,    72
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,   167,   168,    79,     0,     3,   172,     6,    81,
       3,   190,   169,   173,    85,   170,     3,   175,   191,     3,
       4,   171,    82,   174,   108,   109,    80,   176,     3,     4,
      89,   153,     3,     4,    88,    89,   101,   120,   121,   128,
     129,   141,   159,   160,     6,     3,     4,    70,    12,    16,
      23,    24,    25,    29,    51,    55,    56,    57,    65,    77,
      83,    84,    66,    78,     5,    22,    24,    25,    27,    29,
      41,    42,    44,    45,    46,    47,    51,    55,    56,    57,
      58,    59,    62,    63,    64,    65,    69,    71,    72,     9,
      10,    91,    12,    16,    23,    24,    25,    29,    51,    55,
      56,    57,    65,    66,    70,    72,    77,    78,    83,    84,
     151,     7,     7,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,     7,     7,     6,
      10,    87,     6,     8,    90,    98,   136,     7,     6,     8,
      11,    26,    92,    93,    99,   100,   136,    28,   102,   103,
      93,    94,   106,   136,    93,   116,   119,   119,    43,   117,
     125,   116,   123,   117,   126,   123,     3,    89,   107,   131,
     136,    93,   132,   136,   136,   137,   102,   136,   139,   140,
      94,   136,   142,   143,   187,    60,    61,   108,   136,   145,
     146,   179,   188,    32,    89,   111,    17,    18,   148,     6,
      88,    89,   111,   147,     6,   136,   150,     7,   136,   189,
     136,     4,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       4,    90,    93,   136,    94,    90,   136,    91,   100,   106,
     136,   131,   132,   136,   137,   140,   150,    91,   164,     6,
     110,   178,    91,   182,   183,    67,    68,    75,   152,    60,
      61,     7,     7,     4,     4,     4,     4,    13,    14,    15,
      99,     4,     4,    16,    94,   136,     4,    92,    30,   104,
       4,     4,    93,   117,     4,     4,    93,    49,    50,   124,
       4,   117,     4,    82,     4,     4,    29,    52,    53,    54,
     130,     4,     4,   133,     4,     4,     4,   138,     4,     6,
       4,   111,     4,   188,   179,     4,   179,   188,     4,    89,
     111,   120,   180,    89,   111,   120,   108,   110,     4,   124,
     124,     4,     4,     4,    88,    93,   149,     4,     4,   146,
     161,   179,   188,     4,    73,    74,    75,    89,    95,   156,
     158,   108,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,   165,     4,   108,   177,
       4,    93,     4,   107,   184,     4,     4,     4,    99,   136,
       4,    17,    18,    92,    97,    31,   105,    93,    30,    31,
     118,    95,   187,    95,   187,   111,   104,   122,    48,   106,
     136,    89,   101,   120,   121,   107,     6,    10,   135,   111,
     144,    89,   111,   180,   111,   180,   111,    83,    89,   120,
     111,   111,   120,   120,   111,   112,   109,   111,   111,     4,
      93,   104,     4,     4,   111,   180,   111,    16,     6,   111,
     111,     6,   162,   153,     6,    89,   141,    89,   185,     4,
      92,    13,    19,    20,    21,    96,    97,    97,    97,   111,
     105,    95,   187,    93,     4,     4,    25,   107,    32,   111,
     111,   111,   181,   120,   111,   108,   113,     4,   111,   111,
     105,   111,    94,   157,    95,     4,   107,     6,    57,    22,
      24,    25,    27,    29,    56,    57,    65,    18,   111,   111,
     127,   134,   178,    33,    34,    35,    36,    37,    38,    39,
      40,   114,   115,     4,    76,   136,   154,   155,   107,    93,
     136,   186,   186,   186,   186,   186,   186,   186,   186,   124,
      33,    40,     4,    88,   154,   111,   166,    98,    91,   100,
     102,   103,   106,   137,   140,   150,   111,     6,   107,    95,
       4,     4,     4,     4,     4,     4,     4,     4,     4,    95,
     111,   163,    89,   159,    72,   136
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

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


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
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
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


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

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
/* Line 1792 of yacc.c  */
#line 264 "shrink.y"
    {
	#line 418 "format.w"
	(yyval.c).c= (yyvsp[(1) - (2)].u);REF(font_kind,(yyvsp[(2) - (2)].u));(yyval.c).f= (yyvsp[(2) - (2)].u);}
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 267 "shrink.y"
    {
	#line 419 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_glyph(&((yyvsp[(3) - (4)].c))));}
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 270 "shrink.y"
    {
	#line 420 "format.w"
	HPUTNODE;(yyval.u)= (uint32_t)(hpos++-hstart);}
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 274 "shrink.y"
    {
	#line 941 "format.w"
	RNG("number",(yyvsp[(1) - (1)].u),0,INT32_MAX);}
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 278 "shrink.y"
    {
	#line 1082 "format.w"
	(yyval.c).c= (yyvsp[(1) - (2)].u);REF(font_kind,(yyvsp[(2) - (2)].u));(yyval.c).f= (yyvsp[(2) - (2)].u);}
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 282 "shrink.y"
    {
	#line 1187 "format.w"
	static char s[2];
	RNG("String element",(yyvsp[(1) - (1)].u),0x20,0x7E);
	s[0]= (yyvsp[(1) - (1)].u);s[1]= 0;(yyval.s)= s;}
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 288 "shrink.y"
    {
	#line 1339 "format.w"
	(yyval.f)= (float64_t)(yyvsp[(1) - (1)].u);}
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 290 "shrink.y"
    {
	#line 1339 "format.w"
	(yyval.f)= (float64_t)(yyvsp[(1) - (1)].i);}
    break;

  case 13:
/* Line 1792 of yacc.c  */
#line 294 "shrink.y"
    {
	#line 1682 "format.w"
	(yyval.d)= ROUND((yyvsp[(1) - (2)].f)*ONE);RNG("Dimension",(yyval.d),-MAX_DIMEN,MAX_DIMEN);}
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 297 "shrink.y"
    {
	#line 1683 "format.w"
	(yyval.d)= ROUND((yyvsp[(1) - (2)].f)*ONE*72.27);RNG("Dimension",(yyval.d),-MAX_DIMEN,MAX_DIMEN);}
    break;

  case 15:
/* Line 1792 of yacc.c  */
#line 300 "shrink.y"
    {
	#line 1684 "format.w"
	(yyval.d)= ROUND((yyvsp[(1) - (2)].f)*ONE*(72.27/25.4));RNG("Dimension",(yyval.d),-MAX_DIMEN,MAX_DIMEN);}
    break;

  case 16:
/* Line 1792 of yacc.c  */
#line 304 "shrink.y"
    {
	#line 1762 "format.w"
	(yyval.xd).w= (yyvsp[(1) - (5)].d);(yyval.xd).h= (yyvsp[(2) - (5)].f);(yyval.xd).v= (yyvsp[(4) - (5)].f);}
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 307 "shrink.y"
    {
	#line 1763 "format.w"
	(yyval.xd).w= (yyvsp[(1) - (3)].d);(yyval.xd).h= (yyvsp[(2) - (3)].f);(yyval.xd).v= 0.0;}
    break;

  case 18:
/* Line 1792 of yacc.c  */
#line 310 "shrink.y"
    {
	#line 1764 "format.w"
	(yyval.xd).w= (yyvsp[(1) - (3)].d);(yyval.xd).h= 0.0;(yyval.xd).v= (yyvsp[(2) - (3)].f);}
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 313 "shrink.y"
    {
	#line 1765 "format.w"
	(yyval.xd).w= (yyvsp[(1) - (1)].d);(yyval.xd).h= 0.0;(yyval.xd).v= 0.0;}
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 319 "shrink.y"
    {
	#line 1769 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_xdimen(&((yyvsp[(3) - (4)].xd))));}
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 324 "shrink.y"
    {
	#line 1947 "format.w"
	(yyval.o)= normal_o;}
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 326 "shrink.y"
    {
	#line 1947 "format.w"
	(yyval.o)= fil_o;}
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 328 "shrink.y"
    {
	#line 1947 "format.w"
	(yyval.o)= fill_o;}
    break;

  case 24:
/* Line 1792 of yacc.c  */
#line 330 "shrink.y"
    {
	#line 1947 "format.w"
	(yyval.o)= filll_o;}
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 334 "shrink.y"
    {
	#line 1949 "format.w"
	(yyval.st).f= (yyvsp[(1) - (2)].f);(yyval.st).o= (yyvsp[(2) - (2)].o);}
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 338 "shrink.y"
    {
	#line 2003 "format.w"
	RNG("Penalty",(yyvsp[(1) - (1)].i),-20000,+20000);(yyval.i)= (yyvsp[(1) - (1)].i);}
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 341 "shrink.y"
    {
	#line 2004 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_int((yyvsp[(3) - (4)].i)));}
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 345 "shrink.y"
    {
	#line 2179 "format.w"
	(yyval.d)= RUNNING_DIMEN;}
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 349 "shrink.y"
    {
	#line 2181 "format.w"
	(yyval.r).h= (yyvsp[(1) - (3)].d);(yyval.r).d= (yyvsp[(2) - (3)].d);(yyval.r).w= (yyvsp[(3) - (3)].d);
	if((yyvsp[(3) - (3)].d)==RUNNING_DIMEN&&((yyvsp[(1) - (3)].d)==RUNNING_DIMEN||(yyvsp[(2) - (3)].d)==RUNNING_DIMEN))
	QUIT("Incompatible running dimensions 0x%x 0x%x 0x%x",(yyvsp[(1) - (3)].d),(yyvsp[(2) - (3)].d),(yyvsp[(3) - (3)].d));}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 354 "shrink.y"
    {
	#line 2184 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_rule(&((yyvsp[(3) - (4)].r))));}
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 359 "shrink.y"
    {
	#line 2292 "format.w"
	(yyval.b)= false;}
    break;

  case 34:
/* Line 1792 of yacc.c  */
#line 361 "shrink.y"
    {
	#line 2292 "format.w"
	(yyval.b)= true;}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 364 "shrink.y"
    {
	#line 2293 "format.w"
	(yyval.kt).x= (yyvsp[(1) - (2)].b);(yyval.kt).d= (yyvsp[(2) - (2)].xd);}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 367 "shrink.y"
    {
	#line 2294 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_kern(&((yyvsp[(3) - (4)].kt))));}
    break;

  case 37:
/* Line 1792 of yacc.c  */
#line 371 "shrink.y"
    {
	#line 2504 "format.w"
	(yyval.st).f= 0.0;(yyval.st).o= 0;}
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 373 "shrink.y"
    {
	#line 2504 "format.w"
	(yyval.st)= (yyvsp[(2) - (2)].st);}
    break;

  case 39:
/* Line 1792 of yacc.c  */
#line 376 "shrink.y"
    {
	#line 2505 "format.w"
	(yyval.st).f= 0.0;(yyval.st).o= 0;}
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 378 "shrink.y"
    {
	#line 2505 "format.w"
	(yyval.st)= (yyvsp[(2) - (2)].st);}
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 381 "shrink.y"
    {
	#line 2506 "format.w"
	(yyval.g).w= (yyvsp[(1) - (3)].xd);(yyval.g).p= (yyvsp[(2) - (3)].st);(yyval.g).m= (yyvsp[(3) - (3)].st);}
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 384 "shrink.y"
    {
	#line 2507 "format.w"
	if(ZERO_GLUE((yyvsp[(3) - (4)].g))){HPUT8(zero_skip_no);
	hput_tags((yyvsp[(1) - (4)].u),TAG(glue_kind,0));}else hput_tags((yyvsp[(1) - (4)].u),hput_glue(&((yyvsp[(3) - (4)].g))));}
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 389 "shrink.y"
    {
	#line 2510 "format.w"
	if(ZERO_GLUE((yyvsp[(3) - (4)].g))){hpos--;(yyval.b)= false;}
	else{hput_tags((yyvsp[(1) - (4)].u),hput_glue(&((yyvsp[(3) - (4)].g))));(yyval.b)= true;}}
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 394 "shrink.y"
    {
	#line 2786 "format.w"
	(yyval.u)= hpos-hstart;}
    break;

  case 47:
/* Line 1792 of yacc.c  */
#line 399 "shrink.y"
    {
	#line 2789 "format.w"
	hpos+= 2;}
    break;

  case 48:
/* Line 1792 of yacc.c  */
#line 402 "shrink.y"
    {
	#line 2790 "format.w"
	hpos+= hsize_bytes((yyvsp[(1) - (1)].u))+1;}
    break;

  case 49:
/* Line 1792 of yacc.c  */
#line 406 "shrink.y"
    {
	#line 2792 "format.w"
	(yyval.l).k= list_kind;(yyval.l).p= (yyvsp[(3) - (4)].u);(yyval.l).s= (hpos-hstart)-(yyvsp[(3) - (4)].u);
	hput_tags((yyvsp[(1) - (4)].u),hput_list((yyvsp[(1) - (4)].u)+1,&((yyval.l))));}
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 412 "shrink.y"
    {
	#line 3199 "format.w"
	hpos+= 4;}
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 416 "shrink.y"
    {
	#line 3201 "format.w"
	(yyval.l).k= text_kind;(yyval.l).p= (yyvsp[(4) - (5)].u);(yyval.l).s= (hpos-hstart)-(yyvsp[(4) - (5)].u);
	hput_tags((yyvsp[(2) - (5)].u),hput_list((yyvsp[(2) - (5)].u)+1,&((yyval.l))));}
    break;

  case 54:
/* Line 1792 of yacc.c  */
#line 422 "shrink.y"
    {
	#line 3205 "format.w"
	hput_txt_cc((yyvsp[(1) - (1)].u));}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 425 "shrink.y"
    {
	#line 3206 "format.w"
	REF(font_kind,(yyvsp[(1) - (1)].u));hput_txt_font((yyvsp[(1) - (1)].u));}
    break;

  case 56:
/* Line 1792 of yacc.c  */
#line 428 "shrink.y"
    {
	#line 3207 "format.w"
	REF((yyvsp[(1) - (1)].rf).k,(yyvsp[(1) - (1)].rf).n);hput_txt_global(&((yyvsp[(1) - (1)].rf)));}
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 431 "shrink.y"
    {
	#line 3208 "format.w"
	RNG("Font parameter",(yyvsp[(1) - (1)].u),0,11);hput_txt_local((yyvsp[(1) - (1)].u));}
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 434 "shrink.y"
    {
	#line 3209 "format.w"
	HPUTX(1);HPUT8(txt_glue);}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 437 "shrink.y"
    {
	#line 3210 "format.w"
	HPUTX(1);HPUT8(txt_hyphen);}
    break;

  case 60:
/* Line 1792 of yacc.c  */
#line 440 "shrink.y"
    {
	#line 3211 "format.w"
	HPUTX(1);HPUT8(txt_ignore);}
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 443 "shrink.y"
    {
	#line 3212 "format.w"
	HPUTX(1);HPUT8(txt_node);}
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 449 "shrink.y"
    {
	#line 3470 "format.w"
	(yyval.info)= hput_box_dimen((yyvsp[(1) - (3)].d),(yyvsp[(2) - (3)].d),(yyvsp[(3) - (3)].d));}
    break;

  case 64:
/* Line 1792 of yacc.c  */
#line 452 "shrink.y"
    {
	#line 3471 "format.w"
	(yyval.info)= b000;}
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 455 "shrink.y"
    {
	#line 3472 "format.w"
	(yyval.info)= hput_box_shift((yyvsp[(2) - (2)].d));}
    break;

  case 66:
/* Line 1792 of yacc.c  */
#line 459 "shrink.y"
    {
	#line 3474 "format.w"
	(yyval.info)= b000;}
    break;

  case 67:
/* Line 1792 of yacc.c  */
#line 462 "shrink.y"
    {
	#line 3475 "format.w"
	(yyval.info)= hput_box_glue_set(+1,(yyvsp[(2) - (2)].st).f,(yyvsp[(2) - (2)].st).o);}
    break;

  case 68:
/* Line 1792 of yacc.c  */
#line 465 "shrink.y"
    {
	#line 3476 "format.w"
	(yyval.info)= hput_box_glue_set(-1,(yyvsp[(2) - (2)].st).f,(yyvsp[(2) - (2)].st).o);}
    break;

  case 69:
/* Line 1792 of yacc.c  */
#line 470 "shrink.y"
    {
	#line 3479 "format.w"
	(yyval.info)= (yyvsp[(1) - (4)].info)	|(yyvsp[(2) - (4)].info)	|(yyvsp[(3) - (4)].info);}
    break;

  case 70:
/* Line 1792 of yacc.c  */
#line 474 "shrink.y"
    {
	#line 3481 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(hbox_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 477 "shrink.y"
    {
	#line 3482 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(vbox_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 482 "shrink.y"
    {
	#line 3663 "format.w"
	hput_stretch(&((yyvsp[(1) - (2)].st)));hput_stretch(&((yyvsp[(2) - (2)].st)));}
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 485 "shrink.y"
    {
	#line 3664 "format.w"
	(yyval.info)= (yyvsp[(1) - (5)].info)	|(yyvsp[(2) - (5)].info);}
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 488 "shrink.y"
    {
	#line 3665 "format.w"
	(yyval.info)= (yyvsp[(1) - (5)].info)	|(yyvsp[(2) - (5)].info)	|b100;}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 492 "shrink.y"
    {
	#line 3667 "format.w"
	(yyval.info)= b000;}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 495 "shrink.y"
    {
	#line 3668 "format.w"
	(yyval.info)= b001;}
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 498 "shrink.y"
    {
	#line 3669 "format.w"
	(yyval.info)= b100;}
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 501 "shrink.y"
    {
	#line 3670 "format.w"
	(yyval.info)= b101;}
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 505 "shrink.y"
    {
	#line 3672 "format.w"
	(yyval.info)= (yyvsp[(2) - (3)].info);}
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 508 "shrink.y"
    {
	#line 3673 "format.w"
	HPUT32((yyvsp[(4) - (4)].d));}
    break;

  case 83:
/* Line 1792 of yacc.c  */
#line 510 "shrink.y"
    {
	#line 3673 "format.w"
	(yyval.info)= (yyvsp[(1) - (7)].info)	|(yyvsp[(6) - (7)].info);}
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 514 "shrink.y"
    {
	#line 3675 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(vset_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 517 "shrink.y"
    {
	#line 3676 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(vpack_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 522 "shrink.y"
    {
	#line 3679 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(hset_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 525 "shrink.y"
    {
	#line 3680 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(hpack_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 90:
/* Line 1792 of yacc.c  */
#line 531 "shrink.y"
    {
	#line 3790 "format.w"
	(yyval.info)= 1;}
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 533 "shrink.y"
    {
	#line 3790 "format.w"
	(yyval.info)= 1;}
    break;

  case 92:
/* Line 1792 of yacc.c  */
#line 535 "shrink.y"
    {
	#line 3790 "format.w"
	(yyval.info)= 2;}
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 537 "shrink.y"
    {
	#line 3790 "format.w"
	(yyval.info)= 3;}
    break;

  case 94:
/* Line 1792 of yacc.c  */
#line 540 "shrink.y"
    {
	#line 3791 "format.w"
	if((yyvsp[(1) - (3)].b))(yyval.info)= (yyvsp[(2) - (3)].info)	|b100;else (yyval.info)= (yyvsp[(2) - (3)].info);}
    break;

  case 95:
/* Line 1792 of yacc.c  */
#line 543 "shrink.y"
    {
	#line 3792 "format.w"
	if((yyvsp[(1) - (3)].b))(yyval.info)= (yyvsp[(2) - (3)].info)	|b100;else (yyval.info)= (yyvsp[(2) - (3)].info);}
    break;

  case 96:
/* Line 1792 of yacc.c  */
#line 546 "shrink.y"
    {
	#line 3793 "format.w"
	if((yyvsp[(1) - (3)].b))(yyval.info)= (yyvsp[(2) - (3)].info)	|b100;else (yyval.info)= (yyvsp[(2) - (3)].info);}
    break;

  case 97:
/* Line 1792 of yacc.c  */
#line 549 "shrink.y"
    {
	#line 3794 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(leaders_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 98:
/* Line 1792 of yacc.c  */
#line 553 "shrink.y"
    {
	#line 3900 "format.w"
	if((yyvsp[(1) - (1)].d)!=0)HPUT32((yyvsp[(1) - (1)].d));}
    break;

  case 99:
/* Line 1792 of yacc.c  */
#line 556 "shrink.y"
    {
	#line 3901 "format.w"
	(yyval.info)= b000;if((yyvsp[(1) - (4)].d)!=0)(yyval.info)	|= b001;
	if((yyvsp[(3) - (4)].b))(yyval.info)	|= b100;
	if((yyvsp[(4) - (4)].b))(yyval.info)	|= b010;
	}
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 563 "shrink.y"
    {
	#line 3906 "format.w"
	if((yyvsp[(3) - (4)].info)==b000)HPUT8(0);hput_tags((yyvsp[(1) - (4)].u),TAG(baseline_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 102:
/* Line 1792 of yacc.c  */
#line 567 "shrink.y"
    {
	#line 3989 "format.w"
	hput_utf8((yyvsp[(2) - (2)].u));}
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 570 "shrink.y"
    {
	#line 3990 "format.w"
	RNG("UTF-8 code",(yyvsp[(1) - (1)].u),0,0x1FFFFF);(yyval.u)= hpos-hstart;hput_utf8((yyvsp[(1) - (1)].u));}
    break;

  case 104:
/* Line 1792 of yacc.c  */
#line 573 "shrink.y"
    {
	#line 3991 "format.w"
	(yyval.u)= hpos-hstart;hput_utf8((yyvsp[(1) - (1)].u));}
    break;

  case 105:
/* Line 1792 of yacc.c  */
#line 576 "shrink.y"
    {
	#line 3992 "format.w"
	HPUT8((yyvsp[(1) - (1)].u));(yyval.u)= (yyvsp[(1) - (1)].u);}
    break;

  case 106:
/* Line 1792 of yacc.c  */
#line 579 "shrink.y"
    {
	#line 3993 "format.w"
	REF(font_kind,(yyvsp[(1) - (1)].u));}
    break;

  case 107:
/* Line 1792 of yacc.c  */
#line 582 "shrink.y"
    {
	#line 3994 "format.w"
	(yyval.lg).f= (yyvsp[(1) - (6)].u);(yyval.lg).l.p= (yyvsp[(3) - (6)].u);(yyval.lg).l.s= (hpos-hstart)-(yyvsp[(3) - (6)].u);
	RNG("Ligature size",(yyval.lg).l.s,0,255);}
    break;

  case 108:
/* Line 1792 of yacc.c  */
#line 586 "shrink.y"
    {
	#line 3996 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_ligature(&((yyvsp[(3) - (4)].lg))));}
    break;

  case 109:
/* Line 1792 of yacc.c  */
#line 590 "shrink.y"
    {
	#line 4106 "format.w"
	if((yyvsp[(1) - (1)].b)){(yyval.u)= 0x80;HPUT8(0x80);}else (yyval.u)= 0x00;}
    break;

  case 110:
/* Line 1792 of yacc.c  */
#line 593 "shrink.y"
    {
	#line 4107 "format.w"
	RNG("Replace count",(yyvsp[(2) - (2)].u),0,31);
	(yyval.u)= ((yyvsp[(2) - (2)].u))	|(((yyvsp[(1) - (2)].b))?0x80:0x00);if((yyval.u)!=0)HPUT8((yyval.u));}
    break;

  case 111:
/* Line 1792 of yacc.c  */
#line 597 "shrink.y"
    {
	#line 4109 "format.w"
	(yyval.dc).r= (yyvsp[(1) - (3)].u);(yyval.dc).p= (yyvsp[(2) - (3)].l);(yyval.dc).q= (yyvsp[(3) - (3)].l);
	if((yyvsp[(3) - (3)].l).s==0){hpos= hpos-2;if((yyvsp[(2) - (3)].l).s==0)hpos= hpos-2;}}
    break;

  case 112:
/* Line 1792 of yacc.c  */
#line 601 "shrink.y"
    {
	#line 4111 "format.w"
	(yyval.dc).r= (yyvsp[(1) - (2)].u);(yyval.dc).p= (yyvsp[(2) - (2)].l);if((yyvsp[(2) - (2)].l).s==0)hpos= hpos-2;(yyval.dc).q.s= 0;}
    break;

  case 113:
/* Line 1792 of yacc.c  */
#line 604 "shrink.y"
    {
	#line 4112 "format.w"
	(yyval.dc).r= (yyvsp[(1) - (1)].u);(yyval.dc).p.s= 0;(yyval.dc).q.s= 0;}
    break;

  case 114:
/* Line 1792 of yacc.c  */
#line 610 "shrink.y"
    {
	#line 4116 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_disc(&((yyvsp[(3) - (4)].dc))));}
    break;

  case 116:
/* Line 1792 of yacc.c  */
#line 616 "shrink.y"
    {
	#line 4268 "format.w"
	hput_xdimen_node(&((yyvsp[(1) - (1)].xd)));}
    break;

  case 117:
/* Line 1792 of yacc.c  */
#line 619 "shrink.y"
    {
	#line 4269 "format.w"
	(yyval.info)= b000;}
    break;

  case 118:
/* Line 1792 of yacc.c  */
#line 622 "shrink.y"
    {
	#line 4270 "format.w"
	(yyval.info)= b010;}
    break;

  case 119:
/* Line 1792 of yacc.c  */
#line 625 "shrink.y"
    {
	#line 4271 "format.w"
	(yyval.info)= b010;}
    break;

  case 120:
/* Line 1792 of yacc.c  */
#line 628 "shrink.y"
    {
	#line 4272 "format.w"
	hput_xdimen_node(&((yyvsp[(1) - (2)].xd)));}
    break;

  case 121:
/* Line 1792 of yacc.c  */
#line 630 "shrink.y"
    {
	#line 4272 "format.w"
	(yyval.info)= b100;}
    break;

  case 122:
/* Line 1792 of yacc.c  */
#line 633 "shrink.y"
    {
	#line 4273 "format.w"
	(yyval.info)= b110;}
    break;

  case 123:
/* Line 1792 of yacc.c  */
#line 636 "shrink.y"
    {
	#line 4274 "format.w"
	(yyval.info)= b110;}
    break;

  case 124:
/* Line 1792 of yacc.c  */
#line 640 "shrink.y"
    {
	#line 4276 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(par_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 125:
/* Line 1792 of yacc.c  */
#line 644 "shrink.y"
    {
	#line 4342 "format.w"
	(yyval.info)= b000;}
    break;

  case 126:
/* Line 1792 of yacc.c  */
#line 647 "shrink.y"
    {
	#line 4343 "format.w"
	(yyval.info)= b001;}
    break;

  case 127:
/* Line 1792 of yacc.c  */
#line 650 "shrink.y"
    {
	#line 4344 "format.w"
	(yyval.info)= b010;}
    break;

  case 128:
/* Line 1792 of yacc.c  */
#line 653 "shrink.y"
    {
	#line 4345 "format.w"
	(yyval.info)= b100;}
    break;

  case 129:
/* Line 1792 of yacc.c  */
#line 656 "shrink.y"
    {
	#line 4346 "format.w"
	(yyval.info)= b101;}
    break;

  case 130:
/* Line 1792 of yacc.c  */
#line 659 "shrink.y"
    {
	#line 4347 "format.w"
	(yyval.info)= b110;}
    break;

  case 131:
/* Line 1792 of yacc.c  */
#line 662 "shrink.y"
    {
	#line 4348 "format.w"
	(yyval.info)= b100;}
    break;

  case 132:
/* Line 1792 of yacc.c  */
#line 665 "shrink.y"
    {
	#line 4349 "format.w"
	(yyval.info)= b101;}
    break;

  case 133:
/* Line 1792 of yacc.c  */
#line 668 "shrink.y"
    {
	#line 4350 "format.w"
	(yyval.info)= b110;}
    break;

  case 134:
/* Line 1792 of yacc.c  */
#line 672 "shrink.y"
    {
	#line 4352 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(math_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 135:
/* Line 1792 of yacc.c  */
#line 676 "shrink.y"
    {
	#line 4402 "format.w"
	(yyval.i)= 1;}
    break;

  case 136:
/* Line 1792 of yacc.c  */
#line 678 "shrink.y"
    {
	#line 4402 "format.w"
	(yyval.i)= 0;}
    break;

  case 137:
/* Line 1792 of yacc.c  */
#line 681 "shrink.y"
    {
	#line 4403 "format.w"
	(yyval.info)= b011	|((yyvsp[(1) - (1)].i)<<2);}
    break;

  case 138:
/* Line 1792 of yacc.c  */
#line 685 "shrink.y"
    {
	#line 4434 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(adjust_kind,1));}
    break;

  case 139:
/* Line 1792 of yacc.c  */
#line 689 "shrink.y"
    {
	#line 4533 "format.w"
	(yyval.info)= hput_span_count((yyvsp[(1) - (1)].u));}
    break;

  case 140:
/* Line 1792 of yacc.c  */
#line 692 "shrink.y"
    {
	#line 4534 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(item_kind,1));}
    break;

  case 141:
/* Line 1792 of yacc.c  */
#line 695 "shrink.y"
    {
	#line 4535 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),TAG(item_kind,(yyvsp[(3) - (5)].info)));}
    break;

  case 142:
/* Line 1792 of yacc.c  */
#line 698 "shrink.y"
    {
	#line 4536 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(item_kind,b000));}
    break;

  case 143:
/* Line 1792 of yacc.c  */
#line 702 "shrink.y"
    {
	#line 4538 "format.w"
	(yyval.info)= (yyvsp[(2) - (4)].info);}
    break;

  case 144:
/* Line 1792 of yacc.c  */
#line 705 "shrink.y"
    {
	#line 4539 "format.w"
	(yyval.info)= (yyvsp[(2) - (4)].info)	|b010;}
    break;

  case 145:
/* Line 1792 of yacc.c  */
#line 709 "shrink.y"
    {
	#line 4541 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),TAG(table_kind,(yyvsp[(3) - (4)].info)));}
    break;

  case 146:
/* Line 1792 of yacc.c  */
#line 713 "shrink.y"
    {
	#line 4629 "format.w"
	(yyval.x).w= (yyvsp[(1) - (2)].d);(yyval.x).h= (yyvsp[(2) - (2)].d);}
    break;

  case 147:
/* Line 1792 of yacc.c  */
#line 715 "shrink.y"
    {
	#line 4629 "format.w"
	(yyval.x).w= (yyval.x).h= 0;}
    break;

  case 148:
/* Line 1792 of yacc.c  */
#line 718 "shrink.y"
    {
	#line 4630 "format.w"
	(yyval.x).w= (yyvsp[(2) - (4)].x).w;(yyval.x).h= (yyvsp[(2) - (4)].x).h;(yyval.x).p= (yyvsp[(3) - (4)].st);(yyval.x).m= (yyvsp[(4) - (4)].st);RNG("Section number",(yyvsp[(1) - (4)].u),3,max_section_no);(yyval.x).n= (yyvsp[(1) - (4)].u);}
    break;

  case 149:
/* Line 1792 of yacc.c  */
#line 721 "shrink.y"
    {
	#line 4631 "format.w"
	hput_tags((yyvsp[(1) - (4)].u),hput_image(&((yyvsp[(3) - (4)].x))));}
    break;

  case 150:
/* Line 1792 of yacc.c  */
#line 725 "shrink.y"
    {
	#line 4868 "format.w"
	max_outline= (yyvsp[(2) - (2)].u);
	RNG("max outline",max_outline,0,0xFFFF);
	DBG(DBGDEF	|DBGLABEL,"Setting max outline to %d\n",max_outline);
	}
    break;

  case 151:
/* Line 1792 of yacc.c  */
#line 732 "shrink.y"
    {
	#line 4960 "format.w"
	(yyval.i)= LABEL_TOP;}
    break;

  case 152:
/* Line 1792 of yacc.c  */
#line 734 "shrink.y"
    {
	#line 4960 "format.w"
	(yyval.i)= LABEL_BOT;}
    break;

  case 153:
/* Line 1792 of yacc.c  */
#line 736 "shrink.y"
    {
	#line 4960 "format.w"
	(yyval.i)= LABEL_MID;}
    break;

  case 154:
/* Line 1792 of yacc.c  */
#line 738 "shrink.y"
    {
	#line 4960 "format.w"
	(yyval.i)= LABEL_MID;}
    break;

  case 155:
/* Line 1792 of yacc.c  */
#line 742 "shrink.y"
    {
	#line 4962 "format.w"
	hset_label((yyvsp[(3) - (5)].u),(yyvsp[(4) - (5)].i));}
    break;

  case 156:
/* Line 1792 of yacc.c  */
#line 747 "shrink.y"
    {
	#line 5220 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_link((yyvsp[(3) - (5)].u),(yyvsp[(4) - (5)].i)));}
    break;

  case 157:
/* Line 1792 of yacc.c  */
#line 751 "shrink.y"
    {
	#line 5350 "format.w"
	
	static int outline_no= -1;
	(yyval.rf).k= outline_kind;(yyval.rf).n= (yyvsp[(3) - (7)].u);
	if((yyvsp[(6) - (7)].l).s==0)QUIT("Outline with empty title in line %d",yylineno);
	outline_no++;
	hset_outline(outline_no,(yyvsp[(3) - (7)].u),(yyvsp[(4) - (7)].i),(yyvsp[(5) - (7)].u));
	}
    break;

  case 158:
/* Line 1792 of yacc.c  */
#line 761 "shrink.y"
    {
	#line 5765 "format.w"
	REF_RNG(stream_kind,(yyvsp[(1) - (1)].u));}
    break;

  case 159:
/* Line 1792 of yacc.c  */
#line 763 "shrink.y"
    {
	#line 5765 "format.w"
	HPUT8(255);}
    break;

  case 160:
/* Line 1792 of yacc.c  */
#line 766 "shrink.y"
    {
	#line 5766 "format.w"
	RNG("split ratio",(yyvsp[(3) - (3)].u),0,1000);HPUT16((yyvsp[(3) - (3)].u));}
    break;

  case 161:
/* Line 1792 of yacc.c  */
#line 769 "shrink.y"
    {
	#line 5767 "format.w"
	RNG("magnification factor",(yyvsp[(2) - (2)].u),0,1000);HPUT16((yyvsp[(2) - (2)].u));}
    break;

  case 163:
/* Line 1792 of yacc.c  */
#line 773 "shrink.y"
    {
	#line 5769 "format.w"
	(yyval.info)= 0;}
    break;

  case 164:
/* Line 1792 of yacc.c  */
#line 775 "shrink.y"
    {
	#line 5769 "format.w"
	(yyval.info)= 1;}
    break;

  case 165:
/* Line 1792 of yacc.c  */
#line 777 "shrink.y"
    {
	#line 5769 "format.w"
	(yyval.info)= 2;}
    break;

  case 166:
/* Line 1792 of yacc.c  */
#line 779 "shrink.y"
    {
	#line 5769 "format.w"
	(yyval.info)= 3;}
    break;

  case 167:
/* Line 1792 of yacc.c  */
#line 785 "shrink.y"
    {
	#line 5773 "format.w"
	DEF((yyval.rf),stream_kind,(yyvsp[(3) - (10)].u));hput_tags((yyvsp[(1) - (10)].u),TAG(stream_kind,(yyvsp[(4) - (10)].info)	|b100));}
    break;

  case 168:
/* Line 1792 of yacc.c  */
#line 790 "shrink.y"
    {
	#line 5776 "format.w"
	RNG("Stream insertion",(yyvsp[(3) - (4)].u),0,max_ref[stream_kind]);hput_tags((yyvsp[(1) - (4)].u),TAG(stream_kind,b100));}
    break;

  case 171:
/* Line 1792 of yacc.c  */
#line 796 "shrink.y"
    {
	#line 5871 "format.w"
	(yyval.info)= b010;}
    break;

  case 172:
/* Line 1792 of yacc.c  */
#line 799 "shrink.y"
    {
	#line 5872 "format.w"
	(yyval.info)= b010;}
    break;

  case 173:
/* Line 1792 of yacc.c  */
#line 802 "shrink.y"
    {
	#line 5873 "format.w"
	(yyval.info)= b000;}
    break;

  case 174:
/* Line 1792 of yacc.c  */
#line 806 "shrink.y"
    {
	#line 5875 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),TAG(stream_kind,(yyvsp[(4) - (5)].info)));}
    break;

  case 175:
/* Line 1792 of yacc.c  */
#line 810 "shrink.y"
    {
	#line 5978 "format.w"
	HPUT8(1);}
    break;

  case 176:
/* Line 1792 of yacc.c  */
#line 813 "shrink.y"
    {
	#line 5979 "format.w"
	RNG("page priority",(yyvsp[(1) - (1)].u),0,255);HPUT8((yyvsp[(1) - (1)].u));}
    break;

  case 179:
/* Line 1792 of yacc.c  */
#line 819 "shrink.y"
    {
	#line 5983 "format.w"
	hput_string((yyvsp[(1) - (1)].s));}
    break;

  case 180:
/* Line 1792 of yacc.c  */
#line 821 "shrink.y"
    {
	#line 5983 "format.w"
	HPUT32((yyvsp[(5) - (5)].d));}
    break;

  case 182:
/* Line 1792 of yacc.c  */
#line 828 "shrink.y"
    {
	#line 6095 "format.w"
	REF(page_kind,(yyvsp[(3) - (5)].u));hput_range((yyvsp[(3) - (5)].u),true);}
    break;

  case 183:
/* Line 1792 of yacc.c  */
#line 831 "shrink.y"
    {
	#line 6096 "format.w"
	REF(page_kind,(yyvsp[(3) - (5)].u));hput_range((yyvsp[(3) - (5)].u),false);}
    break;

  case 185:
/* Line 1792 of yacc.c  */
#line 837 "shrink.y"
    {
	#line 6786 "format.w"
	new_directory((yyvsp[(3) - (3)].u)+1);new_output_buffers();}
    break;

  case 189:
/* Line 1792 of yacc.c  */
#line 842 "shrink.y"
    {
	#line 6789 "format.w"
	RNG("Section number",(yyvsp[(3) - (5)].u),3,max_section_no);hset_entry(&(dir[(yyvsp[(3) - (5)].u)]),(yyvsp[(3) - (5)].u),0,0,(yyvsp[(4) - (5)].s));}
    break;

  case 190:
/* Line 1792 of yacc.c  */
#line 846 "shrink.y"
    {
	#line 7277 "format.w"
	hput_definitions_start();}
    break;

  case 191:
/* Line 1792 of yacc.c  */
#line 850 "shrink.y"
    {
	#line 7279 "format.w"
	hput_definitions_end();}
    break;

  case 194:
/* Line 1792 of yacc.c  */
#line 856 "shrink.y"
    {
	#line 7395 "format.w"
		/*245:*/
	if(max_ref[label_kind]>=0)
	ALLOCATE(labels,max_ref[label_kind]+1,label_t);
		/*:245*/	/*266:*/
	if(max_outline>=0)
	ALLOCATE(outlines,max_outline+1,outline_t);
		/*:266*/	/*293:*/
	ALLOCATE(page_on,max_ref[page_kind]+1,int);
	ALLOCATE(range_pos,2*(max_ref[range_kind]+1),range_pos_t);
		/*:293*/	/*357:*/
	definition_bits[0][int_kind]= (1<<(MAX_INT_DEFAULT+1))-1;
	definition_bits[0][dimen_kind]= (1<<(MAX_DIMEN_DEFAULT+1))-1;
	definition_bits[0][xdimen_kind]= (1<<(MAX_XDIMEN_DEFAULT+1))-1;
	definition_bits[0][glue_kind]= (1<<(MAX_GLUE_DEFAULT+1))-1;
	definition_bits[0][baseline_kind]= (1<<(MAX_BASELINE_DEFAULT+1))-1;
	definition_bits[0][page_kind]= (1<<(MAX_PAGE_DEFAULT+1))-1;
	definition_bits[0][stream_kind]= (1<<(MAX_STREAM_DEFAULT+1))-1;
	definition_bits[0][range_kind]= (1<<(MAX_RANGE_DEFAULT+1))-1;
		/*:357*/	/*372:*/
	ALLOCATE(hfont_name,max_ref[font_kind]+1,char*);
		/*:372*/hput_max_definitions();}
    break;

  case 197:
/* Line 1792 of yacc.c  */
#line 882 "shrink.y"
    {
	#line 7399 "format.w"
	hset_max(font_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 198:
/* Line 1792 of yacc.c  */
#line 885 "shrink.y"
    {
	#line 7400 "format.w"
	hset_max(int_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 199:
/* Line 1792 of yacc.c  */
#line 888 "shrink.y"
    {
	#line 7401 "format.w"
	hset_max(dimen_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 200:
/* Line 1792 of yacc.c  */
#line 891 "shrink.y"
    {
	#line 7402 "format.w"
	hset_max(ligature_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 201:
/* Line 1792 of yacc.c  */
#line 894 "shrink.y"
    {
	#line 7403 "format.w"
	hset_max(disc_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 202:
/* Line 1792 of yacc.c  */
#line 897 "shrink.y"
    {
	#line 7404 "format.w"
	hset_max(glue_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 203:
/* Line 1792 of yacc.c  */
#line 900 "shrink.y"
    {
	#line 7405 "format.w"
	hset_max(language_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 204:
/* Line 1792 of yacc.c  */
#line 903 "shrink.y"
    {
	#line 7406 "format.w"
	hset_max(rule_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 205:
/* Line 1792 of yacc.c  */
#line 906 "shrink.y"
    {
	#line 7407 "format.w"
	hset_max(image_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 206:
/* Line 1792 of yacc.c  */
#line 909 "shrink.y"
    {
	#line 7408 "format.w"
	hset_max(leaders_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 207:
/* Line 1792 of yacc.c  */
#line 912 "shrink.y"
    {
	#line 7409 "format.w"
	hset_max(baseline_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 208:
/* Line 1792 of yacc.c  */
#line 915 "shrink.y"
    {
	#line 7410 "format.w"
	hset_max(xdimen_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 209:
/* Line 1792 of yacc.c  */
#line 918 "shrink.y"
    {
	#line 7411 "format.w"
	hset_max(param_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 210:
/* Line 1792 of yacc.c  */
#line 921 "shrink.y"
    {
	#line 7412 "format.w"
	hset_max(stream_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 211:
/* Line 1792 of yacc.c  */
#line 924 "shrink.y"
    {
	#line 7413 "format.w"
	hset_max(page_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 212:
/* Line 1792 of yacc.c  */
#line 927 "shrink.y"
    {
	#line 7414 "format.w"
	hset_max(range_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 213:
/* Line 1792 of yacc.c  */
#line 930 "shrink.y"
    {
	#line 7415 "format.w"
	hset_max(label_kind,(yyvsp[(2) - (2)].u));}
    break;

  case 214:
/* Line 1792 of yacc.c  */
#line 936 "shrink.y"
    {
	#line 7612 "format.w"
	DEF((yyval.rf),font_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),(yyvsp[(4) - (5)].info));}
    break;

  case 215:
/* Line 1792 of yacc.c  */
#line 939 "shrink.y"
    {
	#line 7613 "format.w"
	DEF((yyval.rf),int_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_int((yyvsp[(4) - (5)].i)));}
    break;

  case 216:
/* Line 1792 of yacc.c  */
#line 942 "shrink.y"
    {
	#line 7614 "format.w"
	DEF((yyval.rf),dimen_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_dimen((yyvsp[(4) - (5)].d)));}
    break;

  case 217:
/* Line 1792 of yacc.c  */
#line 945 "shrink.y"
    {
	#line 7615 "format.w"
	DEF((yyval.rf),language_kind,(yyvsp[(3) - (5)].u));hput_string((yyvsp[(4) - (5)].s));hput_tags((yyvsp[(1) - (5)].u),TAG(language_kind,0));}
    break;

  case 218:
/* Line 1792 of yacc.c  */
#line 948 "shrink.y"
    {
	#line 7616 "format.w"
	DEF((yyval.rf),glue_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_glue(&((yyvsp[(4) - (5)].g))));}
    break;

  case 219:
/* Line 1792 of yacc.c  */
#line 951 "shrink.y"
    {
	#line 7617 "format.w"
	DEF((yyval.rf),xdimen_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_xdimen(&((yyvsp[(4) - (5)].xd))));}
    break;

  case 220:
/* Line 1792 of yacc.c  */
#line 954 "shrink.y"
    {
	#line 7618 "format.w"
	DEF((yyval.rf),rule_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_rule(&((yyvsp[(4) - (5)].r))));}
    break;

  case 221:
/* Line 1792 of yacc.c  */
#line 957 "shrink.y"
    {
	#line 7619 "format.w"
	DEF((yyval.rf),leaders_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),TAG(leaders_kind,(yyvsp[(4) - (5)].info)));}
    break;

  case 222:
/* Line 1792 of yacc.c  */
#line 960 "shrink.y"
    {
	#line 7620 "format.w"
	DEF((yyval.rf),baseline_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),TAG(baseline_kind,(yyvsp[(4) - (5)].info)));}
    break;

  case 223:
/* Line 1792 of yacc.c  */
#line 963 "shrink.y"
    {
	#line 7621 "format.w"
	DEF((yyval.rf),ligature_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_ligature(&((yyvsp[(4) - (5)].lg))));}
    break;

  case 224:
/* Line 1792 of yacc.c  */
#line 966 "shrink.y"
    {
	#line 7622 "format.w"
	DEF((yyval.rf),disc_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_disc(&((yyvsp[(4) - (5)].dc))));}
    break;

  case 225:
/* Line 1792 of yacc.c  */
#line 969 "shrink.y"
    {
	#line 7623 "format.w"
	DEF((yyval.rf),image_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_image(&((yyvsp[(4) - (5)].x))));}
    break;

  case 226:
/* Line 1792 of yacc.c  */
#line 972 "shrink.y"
    {
	#line 7624 "format.w"
	DEF((yyval.rf),param_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),hput_list((yyvsp[(1) - (5)].u)+2,&((yyvsp[(4) - (5)].l))));}
    break;

  case 227:
/* Line 1792 of yacc.c  */
#line 975 "shrink.y"
    {
	#line 7625 "format.w"
	DEF((yyval.rf),page_kind,(yyvsp[(3) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),TAG(page_kind,0));}
    break;

  case 228:
/* Line 1792 of yacc.c  */
#line 980 "shrink.y"
    {
	#line 7644 "format.w"
	DEF_REF((yyval.rf),int_kind,(yyvsp[(3) - (5)].u),(yyvsp[(4) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),TAG(int_kind,0));}
    break;

  case 229:
/* Line 1792 of yacc.c  */
#line 983 "shrink.y"
    {
	#line 7645 "format.w"
	DEF_REF((yyval.rf),dimen_kind,(yyvsp[(3) - (5)].u),(yyvsp[(4) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),TAG(dimen_kind,0));}
    break;

  case 230:
/* Line 1792 of yacc.c  */
#line 986 "shrink.y"
    {
	#line 7646 "format.w"
	DEF_REF((yyval.rf),glue_kind,(yyvsp[(3) - (5)].u),(yyvsp[(4) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),TAG(glue_kind,0));}
    break;

  case 232:
/* Line 1792 of yacc.c  */
#line 991 "shrink.y"
    {
	#line 7760 "format.w"
	check_param_def(&((yyvsp[(2) - (2)].rf)));}
    break;

  case 233:
/* Line 1792 of yacc.c  */
#line 994 "shrink.y"
    {
	#line 7761 "format.w"
	(yyval.l).p= (yyvsp[(2) - (2)].u);(yyval.l).k= param_kind;(yyval.l).s= (hpos-hstart)-(yyvsp[(2) - (2)].u);}
    break;

  case 234:
/* Line 1792 of yacc.c  */
#line 998 "shrink.y"
    {
	#line 7782 "format.w"
	HPUTX(2);hpos++;hput_tags((yyvsp[(1) - (1)].u),TAG(param_kind,1));}
    break;

  case 235:
/* Line 1792 of yacc.c  */
#line 1001 "shrink.y"
    {
	#line 7783 "format.w"
	hpos= hpos-2;}
    break;

  case 236:
/* Line 1792 of yacc.c  */
#line 1004 "shrink.y"
    {
	#line 7784 "format.w"
	hput_tags((yyvsp[(1) - (5)].u)-2,hput_list((yyvsp[(1) - (5)].u)-1,&((yyvsp[(4) - (5)].l))));}
    break;

  case 238:
/* Line 1792 of yacc.c  */
#line 1012 "shrink.y"
    {
	#line 7926 "format.w"
	uint8_t f= (yyvsp[(0) - (4)].u);SET_DBIT(f,font_kind);hfont_name[f]= strdup((yyvsp[(1) - (4)].s));(yyval.info)= hput_font_head(f,hfont_name[f],(yyvsp[(2) - (4)].d),(yyvsp[(3) - (4)].u),(yyvsp[(4) - (4)].u));}
    break;

  case 241:
/* Line 1792 of yacc.c  */
#line 1019 "shrink.y"
    {
	#line 7931 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_int((yyvsp[(4) - (5)].i)));}
    break;

  case 242:
/* Line 1792 of yacc.c  */
#line 1022 "shrink.y"
    {
	#line 7932 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_kern(&((yyvsp[(4) - (5)].kt))));}
    break;

  case 243:
/* Line 1792 of yacc.c  */
#line 1025 "shrink.y"
    {
	#line 7933 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_ligature(&((yyvsp[(4) - (5)].lg))));}
    break;

  case 244:
/* Line 1792 of yacc.c  */
#line 1028 "shrink.y"
    {
	#line 7934 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_disc(&((yyvsp[(4) - (5)].dc))));}
    break;

  case 245:
/* Line 1792 of yacc.c  */
#line 1031 "shrink.y"
    {
	#line 7935 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_glue(&((yyvsp[(4) - (5)].g))));}
    break;

  case 246:
/* Line 1792 of yacc.c  */
#line 1034 "shrink.y"
    {
	#line 7936 "format.w"
	hput_string((yyvsp[(4) - (5)].s));hput_tags((yyvsp[(1) - (5)].u),TAG(language_kind,0));}
    break;

  case 247:
/* Line 1792 of yacc.c  */
#line 1037 "shrink.y"
    {
	#line 7937 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_rule(&((yyvsp[(4) - (5)].r))));}
    break;

  case 248:
/* Line 1792 of yacc.c  */
#line 1040 "shrink.y"
    {
	#line 7938 "format.w"
	hput_tags((yyvsp[(1) - (5)].u),hput_image(&((yyvsp[(4) - (5)].x))));}
    break;

  case 249:
/* Line 1792 of yacc.c  */
#line 1044 "shrink.y"
    {
	#line 7940 "format.w"
	RNG("Font parameter",(yyvsp[(1) - (1)].u),0,MAX_FONT_PARAMS);}
    break;

  case 250:
/* Line 1792 of yacc.c  */
#line 1048 "shrink.y"
    {
	#line 8013 "format.w"
	REF(xdimen_kind,(yyvsp[(1) - (1)].u));}
    break;

  case 251:
/* Line 1792 of yacc.c  */
#line 1051 "shrink.y"
    {
	#line 8014 "format.w"
	REF(param_kind,(yyvsp[(1) - (1)].u));}
    break;

  case 252:
/* Line 1792 of yacc.c  */
#line 1054 "shrink.y"
    {
	#line 8015 "format.w"
	REF_RNG(stream_kind,(yyvsp[(1) - (1)].u));}
    break;

  case 253:
/* Line 1792 of yacc.c  */
#line 1060 "shrink.y"
    {
	#line 8019 "format.w"
	REF(penalty_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(penalty_kind,0));}
    break;

  case 254:
/* Line 1792 of yacc.c  */
#line 1064 "shrink.y"
    {
	#line 8021 "format.w"
	REF(dimen_kind,(yyvsp[(4) - (5)].u));hput_tags((yyvsp[(1) - (5)].u),TAG(kern_kind,((yyvsp[(3) - (5)].b))?b100:b000));}
    break;

  case 255:
/* Line 1792 of yacc.c  */
#line 1068 "shrink.y"
    {
	#line 8023 "format.w"
	REF(xdimen_kind,(yyvsp[(5) - (6)].u));hput_tags((yyvsp[(1) - (6)].u),TAG(kern_kind,((yyvsp[(3) - (6)].b))?b101:b001));}
    break;

  case 256:
/* Line 1792 of yacc.c  */
#line 1071 "shrink.y"
    {
	#line 8024 "format.w"
	REF(glue_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(glue_kind,0));}
    break;

  case 257:
/* Line 1792 of yacc.c  */
#line 1074 "shrink.y"
    {
	#line 8025 "format.w"
	REF(ligature_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(ligature_kind,0));}
    break;

  case 258:
/* Line 1792 of yacc.c  */
#line 1077 "shrink.y"
    {
	#line 8026 "format.w"
	REF(disc_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(disc_kind,0));}
    break;

  case 259:
/* Line 1792 of yacc.c  */
#line 1080 "shrink.y"
    {
	#line 8027 "format.w"
	REF(rule_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(rule_kind,0));}
    break;

  case 260:
/* Line 1792 of yacc.c  */
#line 1083 "shrink.y"
    {
	#line 8028 "format.w"
	REF(image_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(image_kind,0));}
    break;

  case 261:
/* Line 1792 of yacc.c  */
#line 1086 "shrink.y"
    {
	#line 8029 "format.w"
	REF(leaders_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(leaders_kind,0));}
    break;

  case 262:
/* Line 1792 of yacc.c  */
#line 1089 "shrink.y"
    {
	#line 8030 "format.w"
	REF(baseline_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),TAG(baseline_kind,0));}
    break;

  case 263:
/* Line 1792 of yacc.c  */
#line 1092 "shrink.y"
    {
	#line 8031 "format.w"
	REF(language_kind,(yyvsp[(3) - (4)].u));hput_tags((yyvsp[(1) - (4)].u),hput_language((yyvsp[(3) - (4)].u)));}
    break;

  case 264:
/* Line 1792 of yacc.c  */
#line 1096 "shrink.y"
    {
	#line 8033 "format.w"
	REF(glue_kind,(yyvsp[(3) - (4)].u));
	if((yyvsp[(3) - (4)].u)==zero_skip_no){hpos= hpos-2;(yyval.b)= false;}
	else{hput_tags((yyvsp[(1) - (4)].u),TAG(glue_kind,0));(yyval.b)= true;}}
    break;

  case 265:
/* Line 1792 of yacc.c  */
#line 1103 "shrink.y"
    {
	#line 8464 "format.w"
	hput_content_start();}
    break;

  case 266:
/* Line 1792 of yacc.c  */
#line 1106 "shrink.y"
    {
	#line 8465 "format.w"
	hput_content_end();hput_range_defs();hput_label_defs();}
    break;


/* Line 1792 of yacc.c  */
#line 3942 "shrink.tab.c"
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


/* Line 2055 of yacc.c  */
#line 1110 "shrink.y"

	/*:510*/
