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
#line 25 "otp-parser.y" /* yacc.c:339  */

#include "otp.h"
#include "routines.h"
#include "yystype.h"
int k, len;

static void
yyerror(const char *msg)
{
fprintf(stderr, "line %d: %s\n", line_number, msg);
}

#line 79 "otp-parser.c" /* yacc.c:339  */

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

/* Copy the second part of user declarations.  */

#line 168 "otp-parser.c" /* yacc.c:358  */

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
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   173

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  40
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  87
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  161

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   274

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    39,    38,     2,     2,     2,
      34,    35,    22,    20,    29,    21,    32,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    23,
      30,    26,    31,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    24,    37,    25,    33,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    27,    36,    28,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    62,    62,    72,    73,    79,    80,    84,    86,    90,
      91,    96,    95,   100,   102,   106,   108,   112,   114,   118,
     120,   124,   126,   130,   131,   135,   140,   142,   144,   146,
     148,   153,   155,   157,   159,   161,   163,   168,   170,   175,
     190,   191,   196,   198,   200,   195,   205,   207,   212,   213,
     218,   220,   226,   227,   233,   234,   239,   241,   245,   247,
     251,   258,   260,   262,   264,   266,   271,   276,   281,   286,
     291,   292,   294,   296,   298,   300,   303,   302,   309,   311,
     313,   315,   317,   320,   322,   324,   326,   328
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "NUMBER", "ID", "STRING", "LEFTARROW",
  "RIGHTARROW", "MYINPUT", "OUTPUT", "ALIASES", "STATES", "TABLES",
  "EXPRESSIONS", "PUSH", "POP", "DIV", "MOD", "BEG", "END", "'+'", "'-'",
  "'*'", "';'", "'['", "']'", "'='", "'{'", "'}'", "','", "'<'", "'>'",
  "'.'", "'^'", "'('", "')'", "'|'", "'\\\\'", "'$'", "'#'", "$accept",
  "File", "Input", "Output", "Tables", "MoreTables", "OneTable", "$@1",
  "Numbers", "MoreNumbers", "States", "MoreStates", "Aliases",
  "MoreAliases", "OneAlias", "OneCompleteLeft", "OneLeft", "ChoiceLeft",
  "Expressions", "MoreExpressions", "OneExpr", "$@2", "$@3", "$@4",
  "PushBack", "LeftState", "TotalLeft", "BegLeft", "EndLeft", "Left",
  "Right", "OneRight", "RestRightExpr", "$@5", "OneRightExpr",
  "RightState", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      43,    45,    42,    59,    91,    93,    61,   123,   125,    44,
      60,    62,    46,    94,    40,    41,   124,    92,    36,    35
};
# endif

#define YYPACT_NINF -97

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-97)))

#define YYTABLE_NINF -40

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       0,    13,    18,    11,    34,   -97,    69,    66,   -97,    65,
      85,    79,   -97,    67,    85,   -97,    88,    83,    91,   -97,
     -97,    56,    92,    82,    72,   -97,    94,    73,    92,   -97,
      70,   -97,   -97,   -97,    22,   -97,    97,    17,   -97,   -97,
      76,    84,   -97,    99,   -97,    74,    41,    81,    77,    75,
     -97,    93,    86,   106,    87,    41,   -97,    47,   -97,   107,
     -97,   -97,   -97,    19,   109,   -97,   -97,    51,   -97,    41,
     -16,   110,   -97,   -97,   -97,    19,   -97,    90,    95,   -97,
     -97,    45,   -97,   -97,   -97,   -97,    96,   111,    89,   -97,
      -3,   -97,   -97,   -97,   -97,   -97,     1,     8,   115,   -97,
     -97,   -97,   -17,   -97,   -97,     6,     3,   -97,   -97,    98,
      60,   101,   -97,    49,   -97,   -97,    78,   -97,    -3,    46,
     100,   122,   123,   124,   105,     8,     8,     8,     8,     8,
     -97,   112,   103,   126,   104,   -97,   -97,    -7,   102,   108,
       6,   -97,   -97,   -97,   -97,   -97,   128,   -97,   113,   -97,
     129,   -97,   -97,   -97,    42,   114,   -97,   116,   -97,   -97,
     -97
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     5,     0,     1,     0,     7,     4,     0,
       0,    17,     6,     0,     8,     9,     0,    21,     0,    10,
      19,     0,     0,     0,     0,    18,     0,     0,    22,    23,
      48,     2,    11,    20,     0,    24,     0,    48,    40,    42,
       0,    31,    26,     0,    33,     0,     0,     0,    30,     0,
      41,    52,     0,     0,     0,     0,    37,     0,    25,     0,
      49,    53,    43,    54,    13,    32,    36,     0,    35,     0,
       0,     0,    55,    56,    51,    54,    15,     0,    14,    34,
      38,     0,    29,    58,    57,    50,     0,     0,     0,    28,
      44,    12,    16,    27,    61,    60,     0,     0,    46,    59,
      62,    65,     0,    63,    78,     0,     0,    69,    58,    83,
       0,     0,    76,     0,    70,    79,     0,    80,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      82,     0,     0,     0,     0,    84,    45,     0,     0,     0,
       0,    74,    75,    71,    72,    73,     0,    85,     0,    87,
       0,    66,    67,    64,     0,     0,    86,     0,    77,    81,
      68
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -97,   -97,   -97,   -97,   -97,   -97,   125,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   117,   -56,   -43,   118,   -97,   -97,
     119,   -97,   -97,   -97,   -97,   -97,   -97,   -97,    61,   -97,
      30,   -97,     2,   -97,   -96,   -97
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,     7,    11,    14,    15,    40,    77,    78,
      17,    21,    23,    28,    29,    47,    48,    57,    31,    37,
      38,    51,    71,    98,   109,    39,    62,    63,    74,    75,
      90,    99,   113,   124,   114,   120
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      94,   107,    95,    56,   100,   110,   115,    73,     1,   104,
     112,   104,    56,    81,   150,    82,     4,   -39,     5,    84,
       6,   111,    41,   101,    42,    41,    80,    42,   151,   141,
     142,   143,   144,   145,    96,   102,    97,   116,    72,   103,
     105,   117,   105,   106,    41,   106,    43,    36,    88,    43,
     132,    44,    45,    46,    44,    45,    46,     8,   125,   126,
     133,   134,   127,   128,   129,   125,   126,   158,    43,   127,
     128,   129,     9,    44,    45,    46,    89,   135,    10,    25,
     121,   122,    68,    69,   130,    26,    79,    69,    12,    13,
      16,    18,    20,    22,    24,    30,    27,    32,    33,    34,
      36,    49,    52,    54,    58,    53,    60,    59,    55,    65,
      70,    61,    76,    64,    92,    66,   131,    83,    86,    91,
      93,   108,   123,   136,    87,   137,   138,   139,   119,   140,
     148,   155,   157,   146,   147,   149,    85,   152,   118,    19,
       0,     0,   154,   153,   156,    35,     0,     0,     0,   159,
       0,   160,     0,     0,     0,     0,    50,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    67
};

static const yytype_int16 yycheck[] =
{
       3,    97,     5,    46,     3,    22,     3,    63,     8,     3,
       4,     3,    55,    29,    21,    31,     3,     0,     0,    75,
       9,    38,     3,    22,     5,     3,    69,     5,    35,   125,
     126,   127,   128,   129,    37,    34,    39,    34,    19,    38,
      34,    38,    34,    37,     3,    37,    27,    30,     3,    27,
       4,    32,    33,    34,    32,    33,    34,    23,    16,    17,
      14,    15,    20,    21,    22,    16,    17,    25,    27,    20,
      21,    22,     3,    32,    33,    34,    31,    31,    12,    23,
      20,    21,    35,    36,    35,    29,    35,    36,    23,     4,
      11,    24,     4,    10,     3,    13,     4,    25,     4,    26,
      30,     4,    26,     4,    23,    21,    31,    30,    34,     3,
       3,    18,     3,    27,     3,    28,    38,     7,    28,    23,
      31,     6,    21,    23,    29,     3,     3,     3,    30,    24,
       4,     3,     3,    21,    31,    31,    75,    35,   108,    14,
      -1,    -1,   140,    35,    31,    28,    -1,    -1,    -1,    35,
      -1,    35,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    55
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,    41,    42,     3,     0,     9,    43,    23,     3,
      12,    44,    23,     4,    45,    46,    11,    50,    24,    46,
       4,    51,    10,    52,     3,    23,    29,     4,    53,    54,
      13,    58,    25,     4,    26,    54,    30,    59,    60,    65,
      47,     3,     5,    27,    32,    33,    34,    55,    56,     4,
      60,    61,    26,    21,     4,    34,    56,    57,    23,    30,
      31,    18,    66,    67,    27,     3,    28,    57,    35,    36,
       3,    62,    19,    55,    68,    69,     3,    48,    49,    35,
      56,    29,    31,     7,    55,    68,    28,    29,     3,    31,
      70,    23,     3,    31,     3,     5,    37,    39,    63,    71,
       3,    22,    34,    38,     3,    34,    37,    74,     6,    64,
      22,    38,     4,    72,    74,     3,    34,    38,    70,    30,
      75,    20,    21,    21,    73,    16,    17,    20,    21,    22,
      35,    38,     4,    14,    15,    31,    23,     3,     3,     3,
      24,    74,    74,    74,    74,    74,    21,    31,     4,    31,
      21,    35,    35,    35,    72,     3,    31,     3,    25,    35,
      35
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    40,    41,    42,    42,    43,    43,    44,    44,    45,
      45,    47,    46,    48,    48,    49,    49,    50,    50,    51,
      51,    52,    52,    53,    53,    54,    55,    55,    55,    55,
      55,    56,    56,    56,    56,    56,    56,    57,    57,    58,
      59,    59,    61,    62,    63,    60,    64,    64,    65,    65,
      66,    66,    67,    67,    68,    68,    69,    69,    70,    70,
      71,    71,    71,    71,    71,    71,    71,    71,    71,    71,
      72,    72,    72,    72,    72,    72,    73,    72,    74,    74,
      74,    74,    74,    75,    75,    75,    75,    75
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     6,     0,     3,     0,     3,     0,     2,     1,
       2,     0,    10,     0,     1,     1,     3,     0,     3,     1,
       3,     0,     2,     1,     2,     4,     1,     6,     5,     4,
       1,     1,     3,     1,     4,     3,     3,     1,     3,     2,
       1,     2,     0,     0,     0,    10,     0,     2,     0,     3,
       3,     2,     0,     1,     0,     1,     1,     2,     0,     2,
       1,     1,     2,     2,     6,     2,     6,     6,     8,     2,
       1,     3,     3,     3,     3,     3,     0,     5,     1,     2,
       2,     6,     3,     0,     2,     3,     4,     3
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
        case 3:
#line 72 "otp-parser.y" /* yacc.c:1646  */
    { input_bytes=2; }
#line 1360 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 4:
#line 74 "otp-parser.y" /* yacc.c:1646  */
    { input_bytes=(yyvsp[-1]).yint; }
#line 1366 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 5:
#line 79 "otp-parser.y" /* yacc.c:1646  */
    { output_bytes=2; }
#line 1372 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 6:
#line 81 "otp-parser.y" /* yacc.c:1646  */
    { output_bytes=(yyvsp[-1]).yint; }
#line 1378 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 11:
#line 96 "otp-parser.y" /* yacc.c:1646  */
    { store_table((yyvsp[-3]).ystring, (yyvsp[-1]).yint); }
#line 1384 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 15:
#line 107 "otp-parser.y" /* yacc.c:1646  */
    { add_to_table((yyvsp[0]).yint); }
#line 1390 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 16:
#line 109 "otp-parser.y" /* yacc.c:1646  */
    { add_to_table((yyvsp[0]).yint); }
#line 1396 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 19:
#line 119 "otp-parser.y" /* yacc.c:1646  */
    { store_state((yyvsp[0]).ystring); }
#line 1402 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 20:
#line 121 "otp-parser.y" /* yacc.c:1646  */
    { store_state((yyvsp[0]).ystring); }
#line 1408 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 25:
#line 136 "otp-parser.y" /* yacc.c:1646  */
    { store_alias((yyvsp[-3]).ystring, (yyvsp[-1]).yleft); }
#line 1414 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 26:
#line 141 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = StringLeft((yyvsp[0]).ystring); }
#line 1420 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 27:
#line 143 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = CompleteLeft((yyvsp[-5]).yleft, (yyvsp[-3]).yint, (yyvsp[-1]).yint); }
#line 1426 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 145 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = PlusLeft((yyvsp[-4]).yleft, (yyvsp[-2]).yint); }
#line 1432 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 29:
#line 147 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = CompleteLeft((yyvsp[-3]).yleft, (yyvsp[-1]).yint, (yyvsp[-1]).yint); }
#line 1438 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 30:
#line 149 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = (yyvsp[0]).yleft; }
#line 1444 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 31:
#line 154 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = SingleLeft((yyvsp[0]).yint); }
#line 1450 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 156 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = DoubleLeft((yyvsp[-2]).yint, (yyvsp[0]).yint); }
#line 1456 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 33:
#line 158 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = WildCard(); }
#line 1462 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 34:
#line 160 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = NotChoiceLeft((yyvsp[-1]).ylleft); }
#line 1468 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 35:
#line 162 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = ChoiceLeft((yyvsp[-1]).ylleft); }
#line 1474 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 36:
#line 164 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).yleft = lookup_alias((yyvsp[-1]).ystring); }
#line 1480 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 37:
#line 169 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = llist1((yyvsp[0]).yleft); }
#line 1486 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 38:
#line 171 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = lappend1((yyvsp[-2]).ylleft, (yyvsp[0]).yleft); }
#line 1492 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 39:
#line 176 "otp-parser.y" /* yacc.c:1646  */
    {
	  for(cur_state=0; cur_state<no_states; cur_state++) {
		  if ((states[cur_state].no_exprs)==0) {
        	     out_int(OTP_LEFT_START, 0);
		  } else {
        	     out_int(OTP_LEFT_RETURN, 0);
                  }
		  out_int(OTP_RIGHT_CHAR, 1);
		  out_int(OTP_STOP, 0);
	  }
	}
#line 1508 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 42:
#line 196 "otp-parser.y" /* yacc.c:1646  */
    { states[cur_state].no_exprs++; }
#line 1514 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 43:
#line 198 "otp-parser.y" /* yacc.c:1646  */
    { out_left((yyvsp[0]).ylleft); right_offset=0; }
#line 1520 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 44:
#line 200 "otp-parser.y" /* yacc.c:1646  */
    { right_offset=OTP_PBACK_OFFSET; }
#line 1526 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 45:
#line 202 "otp-parser.y" /* yacc.c:1646  */
    { fill_in_left(); }
#line 1532 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 48:
#line 212 "otp-parser.y" /* yacc.c:1646  */
    { cur_state = 0; }
#line 1538 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 49:
#line 214 "otp-parser.y" /* yacc.c:1646  */
    { cur_state = lookup_state((yyvsp[-1]).ystring); }
#line 1544 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 50:
#line 219 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = lappend((yyvsp[-2]).ylleft, lappend((yyvsp[-1]).ylleft, (yyvsp[0]).ylleft)); }
#line 1550 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 51:
#line 221 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = lappend((yyvsp[-1]).ylleft, (yyvsp[0]).ylleft); }
#line 1556 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 52:
#line 226 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = nil; }
#line 1562 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 53:
#line 228 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = llist1(BeginningLeft()); }
#line 1568 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 54:
#line 233 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = nil; }
#line 1574 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 235 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = llist1(EndLeft()); }
#line 1580 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 56:
#line 240 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = llist1((yyvsp[0]).yleft); }
#line 1586 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 57:
#line 242 "otp-parser.y" /* yacc.c:1646  */
    { (yyval).ylleft = lappend1((yyvsp[-1]).ylleft, (yyvsp[0]).yleft); }
#line 1592 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 60:
#line 252 "otp-parser.y" /* yacc.c:1646  */
    {
	 len=strlen((yyvsp[0]).ystring);
	 for (k=0; k<len; k++) {
            out_right(OTP_RIGHT_NUM, ((yyvsp[0]).ystring)[k]);
         }
	}
#line 1603 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 61:
#line 259 "otp-parser.y" /* yacc.c:1646  */
    { out_right(OTP_RIGHT_NUM, (yyvsp[0]).yint); }
#line 1609 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 261 "otp-parser.y" /* yacc.c:1646  */
    { out_right(OTP_RIGHT_CHAR, (yyvsp[0]).yint); }
#line 1615 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 63:
#line 263 "otp-parser.y" /* yacc.c:1646  */
    { out_right(OTP_RIGHT_LCHAR, 0); }
#line 1621 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 64:
#line 265 "otp-parser.y" /* yacc.c:1646  */
    { out_right(OTP_RIGHT_LCHAR, (yyvsp[-1]).yint); }
#line 1627 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 65:
#line 267 "otp-parser.y" /* yacc.c:1646  */
    {
	 out_right(OTP_RIGHT_SOME, 0); 
	 out_int(0,0);
	}
#line 1636 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 272 "otp-parser.y" /* yacc.c:1646  */
    {
	 out_right(OTP_RIGHT_SOME, (yyvsp[-1]).yint);
	 out_int(0, 0);
	}
#line 1645 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 67:
#line 277 "otp-parser.y" /* yacc.c:1646  */
    {
	 out_right(OTP_RIGHT_SOME, 0);
	 out_int(0, (yyvsp[-1]).yint);
	}
#line 1654 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 282 "otp-parser.y" /* yacc.c:1646  */
    {
	 out_right(OTP_RIGHT_SOME, (yyvsp[-3]).yint);
	 out_int(0, (yyvsp[-1]).yint);
	}
#line 1663 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 69:
#line 287 "otp-parser.y" /* yacc.c:1646  */
    { out_right(OTP_RIGHT_OUTPUT, 0); }
#line 1669 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 71:
#line 293 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_ADD, 0); }
#line 1675 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 295 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_SUB, 0); }
#line 1681 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 73:
#line 297 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_MULT, 0); }
#line 1687 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 74:
#line 299 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_DIV, 0); }
#line 1693 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 75:
#line 301 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_MOD, 0); }
#line 1699 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 76:
#line 303 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_PUSH_NUM, lookup_table((yyvsp[0]).ystring)); }
#line 1705 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 77:
#line 305 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_LOOKUP, 0); }
#line 1711 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 78:
#line 310 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_PUSH_NUM, (yyvsp[0]).yint); }
#line 1717 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 79:
#line 312 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_PUSH_CHAR, (yyvsp[0]).yint); }
#line 1723 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 80:
#line 314 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_PUSH_LCHAR, 0); }
#line 1729 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 81:
#line 316 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_PUSH_LCHAR, (yyvsp[-1]).yint); }
#line 1735 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 84:
#line 323 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_STATE_CHANGE, 0); }
#line 1741 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 85:
#line 325 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_STATE_CHANGE, lookup_state((yyvsp[-1]).ystring)); }
#line 1747 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 86:
#line 327 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_STATE_PUSH, lookup_state((yyvsp[-1]).ystring)); }
#line 1753 "otp-parser.c" /* yacc.c:1646  */
    break;

  case 87:
#line 329 "otp-parser.y" /* yacc.c:1646  */
    { out_int(OTP_STATE_POP, 0); }
#line 1759 "otp-parser.c" /* yacc.c:1646  */
    break;


#line 1763 "otp-parser.c" /* yacc.c:1646  */
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
#line 331 "otp-parser.y" /* yacc.c:1906  */

