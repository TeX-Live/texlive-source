
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
#line 24 "web2c-parser.y"

#include "web2c.h"

#define YYDEBUG 1

#define	symbol(x)	sym_table[x].id
#define	MAX_ARGS	50

static char fn_return_type[50], for_stack[300], control_var[50],
            relation[3];
static char arg_type[MAX_ARGS][30];
static int last_type = -1, ids_typed;
char my_routine[100];	/* Name of routine being parsed, if any */
static char array_bounds[80], array_offset[80];
static int uses_mem, uses_eqtb, lower_sym, upper_sym;
static FILE *orig_out;
boolean doing_statements = false;
static boolean var_formals = false;
static int param_id_list[MAX_ARGS], ids_paramed=0;

extern char conditional[], temp[], *std_header;
extern int tex, mf, strict_for;
extern FILE *coerce;
extern char coerce_name[];
extern string program_name;
extern boolean debug;

static long my_labs P1H(long);
static void compute_array_bounds P1H(void);
static void fixup_var_list P1H(void);
static void do_proc_args P1H(void);
static void gen_function_head P1H(void);
static boolean doreturn P1H(string);


/* Line 189 of yacc.c  */
#line 109 "../../../../tex-live-2009test-1.3/texk/web2c/web2c/web2c-parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
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
     others_tok = 283,
     r_num_tok = 284,
     i_num_tok = 285,
     string_literal_tok = 286,
     single_char_tok = 287,
     assign_tok = 288,
     two_dots_tok = 289,
     undef_id_tok = 290,
     var_id_tok = 291,
     proc_id_tok = 292,
     proc_param_tok = 293,
     fun_id_tok = 294,
     fun_param_tok = 295,
     const_id_tok = 296,
     type_id_tok = 297,
     hhb0_tok = 298,
     hhb1_tok = 299,
     field_id_tok = 300,
     define_tok = 301,
     field_tok = 302,
     break_tok = 303,
     great_eq_tok = 304,
     less_eq_tok = 305,
     not_eq_tok = 306,
     or_tok = 307,
     unary_minus_tok = 308,
     unary_plus_tok = 309,
     and_tok = 310,
     mod_tok = 311,
     div_tok = 312,
     not_tok = 313
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
#define others_tok 283
#define r_num_tok 284
#define i_num_tok 285
#define string_literal_tok 286
#define single_char_tok 287
#define assign_tok 288
#define two_dots_tok 289
#define undef_id_tok 290
#define var_id_tok 291
#define proc_id_tok 292
#define proc_param_tok 293
#define fun_id_tok 294
#define fun_param_tok 295
#define const_id_tok 296
#define type_id_tok 297
#define hhb0_tok 298
#define hhb1_tok 299
#define field_id_tok 300
#define define_tok 301
#define field_tok 302
#define break_tok 303
#define great_eq_tok 304
#define less_eq_tok 305
#define not_eq_tok 306
#define or_tok 307
#define unary_minus_tok 308
#define unary_plus_tok 309
#define and_tok 310
#define mod_tok 311
#define div_tok 312
#define not_tok 313




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 267 "../../../../tex-live-2009test-1.3/texk/web2c/web2c/web2c-parser.c"

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
#define YYLAST   608

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  187
/* YYNRULES -- Number of rules.  */
#define YYNRULES  307
/* YYNRULES -- Number of states.  */
#define YYNSTATES  481

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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      67,    68,    60,    55,    69,    56,    74,    61,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    73,    66,
      50,    49,    51,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    71,     2,    72,    70,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    52,    53,    54,    57,    58,    59,
      62,    63,    64,    65
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     5,    16,    17,    20,    25,    30,
      35,    42,    47,    54,    59,    66,    71,    76,    80,    81,
      83,    87,    89,    91,    92,    93,    94,   103,   104,   105,
     110,   112,   116,   118,   119,   122,   124,   127,   128,   129,
     130,   131,   140,   142,   144,   146,   148,   151,   152,   157,
     158,   163,   164,   169,   170,   175,   176,   181,   182,   187,
     188,   193,   194,   199,   200,   205,   206,   211,   212,   217,
     218,   223,   224,   229,   230,   235,   237,   238,   243,   245,
     247,   249,   251,   252,   255,   257,   260,   261,   262,   263,
     271,   273,   275,   277,   279,   283,   284,   286,   289,   291,
     293,   295,   297,   299,   301,   303,   305,   308,   315,   324,
     326,   328,   330,   331,   336,   338,   342,   343,   344,   350,
     351,   353,   357,   359,   361,   362,   367,   368,   371,   373,
     376,   377,   378,   385,   387,   391,   393,   395,   397,   398,
     399,   405,   407,   410,   413,   416,   419,   420,   426,   427,
     433,   434,   435,   440,   442,   446,   447,   452,   453,   456,
     457,   461,   463,   465,   468,   469,   470,   471,   481,   482,
     483,   484,   494,   496,   498,   500,   504,   505,   510,   512,
     516,   518,   522,   524,   526,   528,   530,   532,   534,   536,
     538,   539,   544,   545,   550,   551,   555,   557,   559,   561,
     563,   566,   567,   572,   575,   578,   581,   583,   584,   589,
     592,   593,   598,   599,   604,   605,   610,   611,   616,   617,
     622,   623,   628,   629,   634,   635,   640,   641,   646,   647,
     652,   653,   658,   659,   664,   665,   670,   671,   676,   678,
     680,   682,   684,   685,   690,   692,   694,   696,   697,   701,
     702,   707,   709,   710,   715,   718,   720,   723,   724,   726,
     728,   729,   733,   736,   737,   739,   741,   743,   745,   747,
     749,   752,   753,   754,   761,   762,   766,   767,   768,   776,
     778,   782,   786,   788,   792,   794,   796,   798,   801,   803,
     805,   807,   808,   809,   816,   817,   818,   825,   826,   827,
     828,   838,   840,   841,   842,   848,   849,   850
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      76,     0,    -1,    -1,    -1,    79,    81,    77,    89,    93,
     120,   147,    78,   156,   154,    -1,    -1,    79,    80,    -1,
      46,    47,    35,    66,    -1,    46,    13,    35,    66,    -1,
      46,     6,    35,    66,    -1,    46,    13,    35,    67,    68,
      66,    -1,    46,    18,    35,    66,    -1,    46,    18,    35,
      67,    68,    66,    -1,    46,    24,    35,    66,    -1,    46,
      24,    35,    49,   128,    66,    -1,    46,    26,    35,    66,
      -1,    19,    35,    82,    66,    -1,    67,    83,    68,    -1,
      -1,    84,    -1,    83,    69,    84,    -1,    41,    -1,    35,
      -1,    -1,    -1,    -1,    86,    89,    93,   120,    87,   147,
      88,   181,    -1,    -1,    -1,    16,    90,    91,    66,    -1,
      92,    -1,    91,    69,    92,    -1,    30,    -1,    -1,     6,
      94,    -1,    95,    -1,    94,    95,    -1,    -1,    -1,    -1,
      -1,    96,    35,    97,    49,    98,   101,    99,    66,    -1,
      30,    -1,    29,    -1,   118,    -1,   119,    -1,   215,   101,
      -1,    -1,   101,    55,   102,   101,    -1,    -1,   101,    56,
     103,   101,    -1,    -1,   101,    60,   104,   101,    -1,    -1,
     101,    64,   105,   101,    -1,    -1,   101,    49,   106,   101,
      -1,    -1,   101,    54,   107,   101,    -1,    -1,   101,    63,
     108,   101,    -1,    -1,   101,    50,   109,   101,    -1,    -1,
     101,    51,   110,   101,    -1,    -1,   101,    53,   111,   101,
      -1,    -1,   101,    52,   112,   101,    -1,    -1,   101,    62,
     113,   101,    -1,    -1,   101,    57,   114,   101,    -1,    -1,
     101,    61,   115,   101,    -1,   116,    -1,    -1,    67,   117,
     101,    68,    -1,   100,    -1,    31,    -1,    32,    -1,    41,
      -1,    -1,    24,   121,    -1,   122,    -1,   121,   122,    -1,
      -1,    -1,    -1,   123,    35,   124,    49,   125,   126,    66,
      -1,   127,    -1,   132,    -1,   128,    -1,   131,    -1,   130,
      34,   130,    -1,    -1,    59,    -1,   129,    30,    -1,    41,
      -1,    36,    -1,    35,    -1,    42,    -1,   134,    -1,   137,
      -1,   145,    -1,   133,    -1,    70,    42,    -1,     3,    71,
     135,    72,    17,   136,    -1,     3,    71,   135,    69,   135,
      72,    17,   136,    -1,   128,    -1,    42,    -1,   126,    -1,
      -1,    20,   138,   139,    10,    -1,   140,    -1,   139,    66,
     140,    -1,    -1,    -1,   141,   143,    73,   142,   126,    -1,
      -1,   144,    -1,   143,    69,   144,    -1,    35,    -1,    45,
      -1,    -1,    11,    17,   146,   126,    -1,    -1,    26,   148,
      -1,   149,    -1,   148,   149,    -1,    -1,    -1,   150,   152,
      73,   151,   126,    66,    -1,   153,    -1,   152,    69,   153,
      -1,    35,    -1,    36,    -1,    45,    -1,    -1,    -1,     4,
     155,   184,    10,    74,    -1,   157,    -1,   156,   157,    -1,
     158,    66,    -1,   171,    66,    -1,   159,    85,    -1,    -1,
      18,    35,   160,   162,    66,    -1,    -1,    18,   170,   161,
     162,    66,    -1,    -1,    -1,    67,   163,   164,    68,    -1,
     167,    -1,   164,    66,   167,    -1,    -1,   166,   152,    73,
      42,    -1,    -1,   168,   165,    -1,    -1,    26,   169,   165,
      -1,    37,    -1,    38,    -1,   172,    85,    -1,    -1,    -1,
      -1,    13,    35,   173,   162,    73,   174,   180,   175,    66,
      -1,    -1,    -1,    -1,    13,   179,   176,   162,    73,   177,
     180,   178,    66,    -1,    39,    -1,    40,    -1,   126,    -1,
       4,   184,    10,    -1,    -1,     4,   183,   184,    10,    -1,
     185,    -1,   184,    66,   185,    -1,   187,    -1,   186,    73,
     187,    -1,    30,    -1,   188,    -1,   229,    -1,   189,    -1,
     225,    -1,   227,    -1,   228,    -1,    48,    -1,    -1,   192,
      33,   190,   200,    -1,    -1,   194,    33,   191,   200,    -1,
      -1,    36,   193,   195,    -1,    36,    -1,    39,    -1,    40,
      -1,   196,    -1,   195,   196,    -1,    -1,    71,   197,   200,
     198,    -1,    74,    45,    -1,    74,    43,    -1,    74,    44,
      -1,    72,    -1,    -1,    69,   199,   200,    72,    -1,   215,
     200,    -1,    -1,   200,    55,   201,   200,    -1,    -1,   200,
      56,   202,   200,    -1,    -1,   200,    60,   203,   200,    -1,
      -1,   200,    64,   204,   200,    -1,    -1,   200,    49,   205,
     200,    -1,    -1,   200,    54,   206,   200,    -1,    -1,   200,
      63,   207,   200,    -1,    -1,   200,    50,   208,   200,    -1,
      -1,   200,    51,   209,   200,    -1,    -1,   200,    53,   210,
     200,    -1,    -1,   200,    52,   211,   200,    -1,    -1,   200,
      62,   212,   200,    -1,    -1,   200,    57,   213,   200,    -1,
      -1,   200,    61,   214,   200,    -1,   216,    -1,    59,    -1,
      58,    -1,    65,    -1,    -1,    67,   217,   200,    68,    -1,
     192,    -1,   100,    -1,    39,    -1,    -1,    40,   218,   219,
      -1,    -1,    67,   220,   221,    68,    -1,   223,    -1,    -1,
     221,    69,   222,   223,    -1,   200,   224,    -1,    42,    -1,
      73,    30,    -1,    -1,    37,    -1,    35,    -1,    -1,    38,
     226,   219,    -1,    14,    30,    -1,    -1,   182,    -1,   230,
      -1,   245,    -1,   231,    -1,   237,    -1,   232,    -1,   232,
     235,    -1,    -1,    -1,    15,   233,   200,   234,    22,   185,
      -1,    -1,     9,   236,   185,    -1,    -1,    -1,     5,   238,
     200,    17,   239,   240,   244,    -1,   241,    -1,   240,    66,
     241,    -1,   242,    73,   187,    -1,   243,    -1,   242,    69,
     243,    -1,    30,    -1,    28,    -1,    10,    -1,    66,    10,
      -1,   246,    -1,   249,    -1,   252,    -1,    -1,    -1,    27,
     247,   200,   248,     7,   185,    -1,    -1,    -1,    21,   250,
     184,    25,   251,   200,    -1,    -1,    -1,    -1,    12,   253,
     256,    33,   254,   257,     7,   255,   185,    -1,    36,    -1,
      -1,    -1,   200,   258,    23,   259,   200,    -1,    -1,    -1,
     200,   260,     8,   261,   200,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    65,    65,    72,    63,    79,    81,    84,    89,    94,
      99,   104,   109,   114,   119,   128,   137,   141,   142,   146,
     147,   151,   152,   156,   161,   175,   156,   205,   207,   206,
     212,   213,   217,   220,   222,   227,   228,   232,   240,   244,
     245,   232,   250,   258,   259,   260,   264,   266,   266,   268,
     268,   270,   270,   272,   272,   274,   274,   276,   276,   278,
     278,   280,   280,   282,   282,   284,   284,   286,   286,   288,
     288,   290,   290,   292,   292,   294,   299,   298,   302,   306,
     312,   321,   324,   325,   328,   329,   333,   335,   342,   333,
     359,   360,   364,   400,   404,   407,   409,   413,   420,   427,
     436,   448,   462,   466,   467,   471,   478,   493,   494,   498,
     500,   510,   514,   513,   519,   520,   524,   526,   524,   544,
     547,   548,   551,   563,   577,   576,   582,   584,   588,   589,
     593,   601,   593,   609,   610,   613,   627,   641,   657,   660,
     659,   672,   673,   676,   678,   683,   687,   686,   703,   702,
     719,   725,   724,   736,   737,   741,   741,   757,   757,   758,
     758,   762,   763,   766,   770,   781,   787,   769,   794,   804,
     809,   793,   816,   817,   820,   823,   827,   826,   832,   833,
     836,   837,   841,   849,   851,   855,   856,   857,   858,   859,
     864,   863,   867,   866,   872,   871,   882,   888,   890,   894,
     895,   899,   898,   902,   920,   922,   926,   928,   927,   932,
     934,   934,   936,   936,   938,   938,   940,   940,   942,   942,
     944,   944,   946,   946,   948,   948,   950,   950,   952,   952,
     954,   954,   956,   956,   958,   958,   961,   960,   964,   969,
     970,   972,   978,   977,   981,   982,   983,   986,   985,   991,
     991,   996,   997,   997,  1002,  1003,  1008,  1009,  1012,  1014,
    1021,  1020,  1025,  1039,  1042,  1043,  1044,  1047,  1048,  1051,
    1052,  1056,  1058,  1055,  1063,  1062,  1068,  1070,  1067,  1077,
    1078,  1081,  1085,  1086,  1089,  1094,  1098,  1099,  1102,  1103,
    1104,  1108,  1112,  1107,  1117,  1119,  1116,  1127,  1136,  1142,
    1126,  1176,  1181,  1183,  1180,  1193,  1195,  1192
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "array_tok", "begin_tok", "case_tok",
  "const_tok", "do_tok", "downto_tok", "else_tok", "end_tok", "file_tok",
  "for_tok", "function_tok", "goto_tok", "if_tok", "label_tok", "of_tok",
  "procedure_tok", "program_tok", "record_tok", "repeat_tok", "then_tok",
  "to_tok", "type_tok", "until_tok", "var_tok", "while_tok", "others_tok",
  "r_num_tok", "i_num_tok", "string_literal_tok", "single_char_tok",
  "assign_tok", "two_dots_tok", "undef_id_tok", "var_id_tok",
  "proc_id_tok", "proc_param_tok", "fun_id_tok", "fun_param_tok",
  "const_id_tok", "type_id_tok", "hhb0_tok", "hhb1_tok", "field_id_tok",
  "define_tok", "field_tok", "break_tok", "'='", "'<'", "'>'",
  "great_eq_tok", "less_eq_tok", "not_eq_tok", "'+'", "'-'", "or_tok",
  "unary_minus_tok", "unary_plus_tok", "'*'", "'/'", "and_tok", "mod_tok",
  "div_tok", "not_tok", "';'", "'('", "')'", "','", "'^'", "'['", "']'",
  "':'", "'.'", "$accept", "PROGRAM", "$@1", "$@2", "DEFS", "DEF",
  "PROGRAM_HEAD", "PROGRAM_FILE_PART", "PROGRAM_FILE_LIST", "PROGRAM_FILE",
  "BLOCK", "$@3", "$@4", "$@5", "LABEL_DEC_PART", "$@6", "LABEL_LIST",
  "LABEL", "CONST_DEC_PART", "CONST_DEC_LIST", "CONST_DEC", "$@7", "$@8",
  "$@9", "$@10", "CONSTANT", "CONSTANT_EXPRESS", "$@11", "$@12", "$@13",
  "$@14", "$@15", "$@16", "$@17", "$@18", "$@19", "$@20", "$@21", "$@22",
  "$@23", "$@24", "CONST_FACTOR", "$@25", "STRING", "CONSTANT_ID",
  "TYPE_DEC_PART", "TYPE_DEF_LIST", "TYPE_DEF", "$@26", "$@27", "$@28",
  "TYPE", "SIMPLE_TYPE", "SUBRANGE_TYPE", "POSSIBLE_PLUS",
  "SUBRANGE_CONSTANT", "TYPE_ID", "STRUCTURED_TYPE", "POINTER_TYPE",
  "ARRAY_TYPE", "INDEX_TYPE", "COMPONENT_TYPE", "RECORD_TYPE", "$@29",
  "FIELD_LIST", "RECORD_SECTION", "$@30", "$@31", "FIELD_ID_LIST",
  "FIELD_ID", "FILE_TYPE", "$@32", "VAR_DEC_PART", "VAR_DEC_LIST",
  "VAR_DEC", "$@33", "$@34", "VAR_ID_DEC_LIST", "VAR_ID", "BODY", "$@35",
  "P_F_DEC_PART", "P_F_DEC", "PROCEDURE_DEC", "PROCEDURE_HEAD", "$@36",
  "$@37", "PARAM", "$@38", "FORM_PAR_SEC_L", "FORM_PAR_SEC1", "$@39",
  "FORM_PAR_SEC", "$@40", "$@41", "DECLARED_PROC", "FUNCTION_DEC",
  "FUNCTION_HEAD", "$@42", "$@43", "$@44", "$@45", "$@46", "$@47",
  "DECLARED_FUN", "RESULT_TYPE", "STAT_PART", "COMPOUND_STAT", "$@48",
  "STAT_LIST", "STATEMENT", "S_LABEL", "UNLAB_STAT", "SIMPLE_STAT",
  "ASSIGN_STAT", "$@49", "$@50", "VARIABLE", "@51", "FUNC_ID_AS",
  "VAR_DESIG_LIST", "VAR_DESIG", "$@52", "VAR_DESIG1", "$@53", "EXPRESS",
  "$@54", "$@55", "$@56", "$@57", "$@58", "$@59", "$@60", "$@61", "$@62",
  "$@63", "$@64", "$@65", "$@66", "$@67", "UNARY_OP", "FACTOR", "$@68",
  "$@69", "PARAM_LIST", "$@70", "ACTUAL_PARAM_L", "$@71", "ACTUAL_PARAM",
  "WIDTH_FIELD", "PROC_STAT", "$@72", "GO_TO_STAT", "EMPTY_STAT",
  "STRUCT_STAT", "CONDIT_STAT", "IF_STATEMENT", "BEGIN_IF_STAT", "$@73",
  "$@74", "ELSE_STAT", "$@75", "CASE_STATEMENT", "$@76", "$@77",
  "CASE_EL_LIST", "CASE_ELEMENT", "CASE_LAB_LIST", "CASE_LAB", "END_CASE",
  "REPETIT_STAT", "WHILE_STATEMENT", "$@78", "$@79", "REP_STATEMENT",
  "$@80", "$@81", "FOR_STATEMENT", "$@82", "$@83", "$@84", "CONTROL_VAR",
  "FOR_LIST", "$@85", "$@86", "$@87", "$@88", 0
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
     295,   296,   297,   298,   299,   300,   301,   302,   303,    61,
      60,    62,   304,   305,   306,    43,    45,   307,   308,   309,
      42,    47,   310,   311,   312,   313,    59,    40,    41,    44,
      94,    91,    93,    58,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,    75,    77,    78,    76,    79,    79,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    81,    82,    82,    83,
      83,    84,    84,    86,    87,    88,    85,    89,    90,    89,
      91,    91,    92,    93,    93,    94,    94,    96,    97,    98,
      99,    95,   100,   100,   100,   100,   101,   102,   101,   103,
     101,   104,   101,   105,   101,   106,   101,   107,   101,   108,
     101,   109,   101,   110,   101,   111,   101,   112,   101,   113,
     101,   114,   101,   115,   101,   101,   117,   116,   116,   118,
     118,   119,   120,   120,   121,   121,   123,   124,   125,   122,
     126,   126,   127,   127,   128,   129,   129,   130,   130,   130,
     130,   131,   132,   132,   132,   132,   133,   134,   134,   135,
     135,   136,   138,   137,   139,   139,   141,   142,   140,   140,
     143,   143,   144,   144,   146,   145,   147,   147,   148,   148,
     150,   151,   149,   152,   152,   153,   153,   153,   154,   155,
     154,   156,   156,   157,   157,   158,   160,   159,   161,   159,
     162,   163,   162,   164,   164,   166,   165,   168,   167,   169,
     167,   170,   170,   171,   173,   174,   175,   172,   176,   177,
     178,   172,   179,   179,   180,   181,   183,   182,   184,   184,
     185,   185,   186,   187,   187,   188,   188,   188,   188,   188,
     190,   189,   191,   189,   193,   192,   192,   194,   194,   195,
     195,   197,   196,   196,   196,   196,   198,   199,   198,   200,
     201,   200,   202,   200,   203,   200,   204,   200,   205,   200,
     206,   200,   207,   200,   208,   200,   209,   200,   210,   200,
     211,   200,   212,   200,   213,   200,   214,   200,   200,   215,
     215,   215,   217,   216,   216,   216,   216,   218,   216,   220,
     219,   221,   222,   221,   223,   223,   224,   224,   225,   225,
     226,   225,   227,   228,   229,   229,   229,   230,   230,   231,
     231,   233,   234,   232,   236,   235,   238,   239,   237,   240,
     240,   241,   242,   242,   243,   243,   244,   244,   245,   245,
     245,   247,   248,   246,   250,   251,   249,   253,   254,   255,
     252,   256,   258,   259,   257,   260,   261,   257
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     0,    10,     0,     2,     4,     4,     4,
       6,     4,     6,     4,     6,     4,     4,     3,     0,     1,
       3,     1,     1,     0,     0,     0,     8,     0,     0,     4,
       1,     3,     1,     0,     2,     1,     2,     0,     0,     0,
       0,     8,     1,     1,     1,     1,     2,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     1,     0,     4,     1,     1,
       1,     1,     0,     2,     1,     2,     0,     0,     0,     7,
       1,     1,     1,     1,     3,     0,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     6,     8,     1,
       1,     1,     0,     4,     1,     3,     0,     0,     5,     0,
       1,     3,     1,     1,     0,     4,     0,     2,     1,     2,
       0,     0,     6,     1,     3,     1,     1,     1,     0,     0,
       5,     1,     2,     2,     2,     2,     0,     5,     0,     5,
       0,     0,     4,     1,     3,     0,     4,     0,     2,     0,
       3,     1,     1,     2,     0,     0,     0,     9,     0,     0,
       0,     9,     1,     1,     1,     3,     0,     4,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     4,     0,     4,     0,     3,     1,     1,     1,     1,
       2,     0,     4,     2,     2,     2,     1,     0,     4,     2,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     0,     4,
       0,     4,     0,     4,     0,     4,     0,     4,     1,     1,
       1,     1,     0,     4,     1,     1,     1,     0,     3,     0,
       4,     1,     0,     4,     2,     1,     2,     0,     1,     1,
       0,     3,     2,     0,     1,     1,     1,     1,     1,     1,
       2,     0,     0,     6,     0,     3,     0,     0,     7,     1,
       3,     3,     1,     3,     1,     1,     1,     2,     1,     1,
       1,     0,     0,     6,     0,     0,     6,     0,     0,     0,
       9,     1,     0,     0,     5,     0,     0,     5
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       5,     0,     0,     1,     0,     0,     6,     2,    18,     0,
       0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
       0,     0,     0,     0,    28,    33,    22,    21,     0,    19,
      16,     9,     8,     0,    11,     0,    95,    13,    15,     7,
       0,    37,    82,    17,     0,     0,     0,   100,    99,    98,
      96,     0,     0,     0,    32,     0,    30,    34,    35,     0,
      86,   126,    20,    10,    12,    14,    97,    95,    29,     0,
      36,    38,    83,    84,     0,   130,     3,    94,    31,     0,
      85,    87,   127,   128,     0,     0,    39,     0,   129,   135,
     136,   137,     0,   133,     0,     0,   138,   141,     0,    23,
       0,    23,     0,    88,     0,   131,   164,   172,   173,   168,
     146,   161,   162,   148,   139,     4,   142,   143,   145,    27,
     144,   163,    43,    42,    79,    80,    81,   240,   239,   241,
      76,    78,    40,    75,    44,    45,     0,    95,   134,    95,
     150,   150,   150,   150,   263,    33,     0,    55,    61,    63,
      67,    65,    57,    47,    49,    71,    51,    73,    69,    59,
      53,     0,    46,     0,     0,   112,   101,     0,     0,    90,
      92,    93,    91,   105,   102,   103,   104,     0,   151,     0,
       0,     0,     0,   176,   276,   297,     0,   271,   294,   291,
     182,   259,   196,   258,   260,   197,   198,   189,   264,     0,
     178,     0,   180,   183,   185,     0,     0,   186,   187,   188,
     184,   265,   267,   269,   268,   266,   288,   289,   290,    82,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    41,    95,   124,   116,   106,
      89,   132,   157,   165,   169,   147,   149,   263,     0,     0,
     262,     0,   263,     0,     0,     0,     0,   263,   263,   190,
     192,   274,   270,    24,    77,    56,    62,    64,    68,    66,
      58,    48,    50,    72,    52,    74,    70,    60,    54,   110,
     109,     0,    95,     0,   114,     0,   159,     0,   153,   155,
      95,    95,     0,   246,   247,   242,   245,   244,     0,     0,
     238,   301,     0,   272,     0,   292,   201,     0,   195,   199,
     249,   261,   140,   179,   181,     0,     0,   263,   126,    95,
       0,   125,   113,   116,   122,   123,     0,   120,   155,   157,
     152,   158,     0,   174,   166,   170,   177,     0,     0,   277,
     218,   224,   226,   230,   228,   220,   210,   212,   234,   214,
     236,   232,   222,   216,   209,   298,     0,   295,     0,     0,
     204,   205,   203,   200,     0,   191,   193,   275,    25,     0,
      95,   115,     0,   117,   160,   154,     0,     0,     0,   248,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   263,     0,   263,
       0,   255,   257,     0,   251,     0,     0,   111,   107,   121,
      95,     0,   167,   171,   243,   285,   284,     0,   279,     0,
     282,   219,   225,   227,   231,   229,   221,   211,   213,   235,
     215,   237,   233,   223,   217,   302,     0,   273,   296,   293,
     207,   206,   202,     0,   254,   250,   252,   263,    26,    95,
     118,   156,   286,     0,   278,     0,   263,     0,     0,   299,
       0,   256,     0,     0,   108,   287,   280,   283,   281,   303,
     306,   263,     0,   253,   175,     0,     0,   300,   208,   304,
     307
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    15,    85,     2,     6,     7,    17,    28,    29,
     118,   119,   318,   405,    25,    40,    55,    56,    42,    57,
      58,    59,    79,   102,   161,   296,   132,   227,   228,   230,
     234,   221,   226,   233,   222,   223,   225,   224,   232,   229,
     231,   133,   146,   134,   135,    61,    72,    73,    74,    87,
     137,   333,   169,   170,    52,    53,   171,   172,   173,   174,
     281,   408,   175,   238,   283,   284,   285,   410,   326,   327,
     176,   282,    76,    82,    83,    84,   139,    92,    93,   115,
     144,    96,    97,    98,    99,   142,   143,   179,   242,   287,
     331,   332,   288,   289,   328,   113,   100,   101,   140,   290,
     377,   141,   291,   378,   109,   334,   448,   198,   247,   199,
     200,   201,   202,   203,   204,   315,   316,   297,   254,   206,
     308,   309,   359,   442,   460,   402,   388,   389,   391,   395,
     382,   387,   394,   383,   384,   386,   385,   393,   390,   392,
     299,   300,   338,   337,   311,   364,   403,   462,   404,   444,
     207,   255,   208,   209,   210,   211,   212,   213,   251,   356,
     262,   317,   214,   248,   381,   417,   418,   419,   420,   454,
     215,   216,   253,   358,   217,   252,   398,   218,   249,   396,
     471,   302,   436,   457,   475,   458,   476
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -254
static const yytype_int16 yypact[] =
{
    -254,    47,    12,  -254,   -13,    10,  -254,  -254,   -15,    71,
     106,   109,   112,   128,   131,    64,    37,   122,   129,     9,
      45,   -12,   132,   139,  -254,   210,  -254,  -254,   103,  -254,
    -254,  -254,  -254,   145,  -254,   161,    -6,  -254,  -254,  -254,
     200,  -254,   208,  -254,    37,   165,   169,  -254,  -254,  -254,
    -254,   172,   214,   206,  -254,   -45,  -254,   211,  -254,   212,
    -254,   222,  -254,  -254,  -254,  -254,  -254,    -6,  -254,   200,
    -254,  -254,   215,  -254,   216,  -254,  -254,  -254,  -254,   203,
    -254,  -254,    38,  -254,    51,    27,  -254,   205,  -254,  -254,
    -254,  -254,   -10,  -254,    53,    72,    66,  -254,   183,  -254,
     199,  -254,   364,  -254,    51,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,    64,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,   496,  -254,  -254,  -254,   364,     7,  -254,     7,
     209,   209,   209,   209,   337,   210,   364,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,   213,  -254,   195,   255,  -254,  -254,   231,   217,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,   218,  -254,   202,
     204,   235,   236,  -254,  -254,  -254,   248,  -254,  -254,  -254,
    -254,  -254,    31,  -254,  -254,  -254,  -254,  -254,  -254,    -4,
    -254,   232,  -254,  -254,  -254,   247,   249,  -254,  -254,  -254,
    -254,  -254,  -254,   272,  -254,  -254,  -254,  -254,  -254,   208,
     456,   364,   364,   364,   364,   364,   364,   364,   364,   364,
     364,   364,   364,   364,   364,  -254,    -3,  -254,    -2,  -254,
    -254,  -254,   278,  -254,  -254,  -254,  -254,   337,   178,   270,
    -254,   178,   337,   178,    69,   240,   234,   337,   185,  -254,
    -254,  -254,  -254,  -254,  -254,   512,   512,   512,   512,   512,
     512,    55,    55,    55,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,    73,     7,     1,  -254,    15,  -254,    32,  -254,  -254,
       7,     7,     2,  -254,  -254,  -254,  -254,  -254,   283,   178,
    -254,  -254,   276,   528,    -5,   528,  -254,   125,    69,  -254,
    -254,  -254,  -254,  -254,  -254,   178,   178,   337,   222,    -3,
     293,  -254,  -254,    -2,  -254,  -254,    21,  -254,  -254,   278,
    -254,  -254,    51,  -254,  -254,  -254,  -254,   240,   178,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,   289,  -254,   307,   178,
    -254,  -254,  -254,  -254,   350,   528,   528,  -254,  -254,   243,
       7,  -254,    15,  -254,  -254,  -254,    22,   250,   251,  -254,
     476,   166,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   337,   178,   337,
     408,  -254,   383,   159,  -254,   314,   302,  -254,  -254,  -254,
       7,   279,  -254,  -254,  -254,  -254,  -254,     3,  -254,    28,
    -254,   544,   544,   544,   544,   544,   544,    60,    60,    60,
    -254,  -254,  -254,  -254,  -254,   207,   313,  -254,   528,  -254,
    -254,  -254,  -254,   292,  -254,  -254,  -254,   337,  -254,     7,
    -254,  -254,  -254,    16,  -254,   166,   185,   300,   316,  -254,
     178,  -254,   350,     5,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,   337,   432,  -254,  -254,   178,   178,  -254,  -254,   528,
     528
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,   281,
     225,  -254,  -254,  -254,   229,  -254,  -254,   259,   184,  -254,
     273,  -254,  -254,  -254,  -254,   -95,    65,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,   134,  -254,   282,  -254,  -254,
    -254,  -136,  -254,   -34,  -254,   264,  -254,  -254,  -254,  -254,
      36,   -99,  -254,  -254,  -254,    33,  -254,  -254,  -254,   -11,
    -254,  -254,    39,  -254,   277,  -254,  -254,    30,   256,  -254,
    -254,  -254,   267,  -254,  -254,  -254,  -254,    50,  -254,  -254,
      40,  -254,    41,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,    74,  -254,  -254,  -254,  -243,
    -232,  -254,  -253,  -254,  -254,  -254,  -254,  -144,  -254,  -254,
    -254,    58,  -254,  -254,  -254,  -234,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
     -47,  -254,  -254,  -254,    34,  -254,  -254,  -254,   -93,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,   -75,  -254,   -72,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -306
static const yytype_int16 yytable[] =
{
     205,   168,    51,   177,   292,   314,   256,   131,  -119,   304,
     163,   322,   336,   452,   298,   474,     9,   303,   164,   305,
     357,    68,     8,    10,    69,   313,   465,   165,    11,    47,
      48,     4,    47,    48,    12,    49,    13,    36,    49,   279,
      94,   131,    47,    48,   415,    95,   416,     3,    49,   166,
     324,   131,    16,    50,    37,   136,    50,    14,     5,   104,
     325,   257,   257,   105,  -119,   354,    50,   323,   257,   453,
     114,   257,    26,  -130,  -130,    32,    33,   167,    27,    94,
      24,   365,   366,  -130,    95,   367,    89,    90,   106,   136,
     372,   104,   107,   108,   373,   411,    91,   455,   329,   136,
     330,   456,  -194,   205,   380,  -194,    18,   110,   205,   111,
     112,    34,    35,   205,   205,   156,   157,   158,   159,   160,
     349,   350,   351,   352,   353,   400,   131,   131,   131,   131,
     131,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     306,    19,   319,   307,    20,   320,   321,    21,   421,   422,
     423,   424,   425,   426,   427,   428,   429,   430,   431,   432,
     433,   434,   435,    22,   438,   437,    23,   439,   360,   361,
     362,    43,    44,   205,   136,   136,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,    30,   183,
     184,   180,   181,   182,   415,    31,   416,   185,    38,   186,
     187,   162,   280,   468,   463,    39,   188,   122,   123,   124,
     125,   220,   189,    45,   192,  -305,    41,   293,   294,   126,
     191,   192,   193,   194,   195,   196,   472,   445,   446,    46,
      54,    63,    60,   197,   407,    64,   127,   128,    65,   477,
      67,   479,   480,   129,    66,   295,   -37,    71,    75,   117,
     -86,    81,    86,   205,   103,   205,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   120,   236,   349,   350,   351,
     352,   353,   237,   239,   450,   243,   178,   244,   250,   235,
     259,   261,   260,   240,   241,   280,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
     339,   245,   246,   205,   286,   258,   301,   310,   312,   355,
     370,   397,   205,   407,   399,   406,   412,   413,   447,   449,
     459,   451,   461,   469,   470,    62,   121,   205,    78,   219,
      70,    77,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   183,   184,   349,   350,   351,   352,   353,   145,   185,
     464,   186,   187,   263,    80,   369,   371,   368,   188,    88,
     138,   409,   376,   116,   189,   335,   363,   190,   374,   473,
     375,   379,   191,   192,   193,   194,   195,   196,   466,   122,
     123,   124,   125,   467,     0,   197,   192,     0,     0,   293,
     294,   126,   401,   122,   123,   124,   125,     0,     0,     0,
       0,     0,     0,     0,     0,   126,     0,     0,   127,   128,
       0,     0,     0,     0,     0,   129,     0,   295,     0,     0,
       0,     0,   127,   128,     0,     0,     0,     0,     0,   129,
       0,   130,   340,   341,   342,   343,   344,   345,   346,   347,
     348,     0,     0,   349,   350,   351,   352,   353,     0,     0,
       0,     0,     0,     0,     0,     0,   443,   340,   341,   342,
     343,   344,   345,   346,   347,   348,     0,     0,   349,   350,
     351,   352,   353,     0,     0,     0,     0,   440,     0,     0,
     441,   340,   341,   342,   343,   344,   345,   346,   347,   348,
       0,     0,   349,   350,   351,   352,   353,     0,     0,     0,
       0,     0,     0,     0,   478,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,   156,   157,   158,   159,
     160,     0,     0,     0,   264,   340,   341,   342,   343,   344,
     345,   346,   347,   348,     0,     0,   349,   350,   351,   352,
     353,     0,     0,     0,   414,   147,   148,   149,   150,   151,
     152,   153,   154,   155,     0,     0,   156,   157,   158,   159,
     160,  -306,  -306,  -306,  -306,  -306,  -306,   153,   154,   155,
       0,     0,   156,   157,   158,   159,   160,   340,   341,   342,
     343,   344,   345,   346,   347,   348,     0,     0,   349,   350,
     351,   352,   353,  -306,  -306,  -306,  -306,  -306,  -306,   346,
     347,   348,     0,     0,   349,   350,   351,   352,   353
};

static const yytype_int16 yycheck[] =
{
     144,   137,    36,   139,   247,   258,    10,   102,    10,   252,
       3,    10,    10,    10,   248,    10,     6,   251,    11,   253,
      25,    66,    35,    13,    69,   257,    10,    20,    18,    35,
      36,    19,    35,    36,    24,    41,    26,    49,    41,    42,
      13,   136,    35,    36,    28,    18,    30,     0,    41,    42,
      35,   146,    67,    59,    66,   102,    59,    47,    46,    69,
      45,    66,    66,    73,    66,   299,    59,    66,    66,    66,
       4,    66,    35,    35,    36,    66,    67,    70,    41,    13,
      16,   315,   316,    45,    18,   317,    35,    36,    35,   136,
      69,    69,    39,    40,    73,    73,    45,    69,    66,   146,
      68,    73,    71,   247,   338,    74,    35,    35,   252,    37,
      38,    66,    67,   257,   258,    60,    61,    62,    63,    64,
      60,    61,    62,    63,    64,   359,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
      71,    35,    69,    74,    35,    72,   282,    35,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,    35,   398,   397,    35,   399,    43,    44,
      45,    68,    69,   317,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,    66,     4,
       5,   141,   142,   143,    28,    66,    30,    12,    66,    14,
      15,   136,   236,   456,   447,    66,    21,    29,    30,    31,
      32,   146,    27,    68,    36,     8,     6,    39,    40,    41,
      35,    36,    37,    38,    39,    40,   460,    68,    69,    68,
      30,    66,    24,    48,   370,    66,    58,    59,    66,   471,
      34,   475,   476,    65,    30,    67,    35,    35,    26,    66,
      35,    35,    49,   397,    49,   399,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    66,    71,    60,    61,    62,
      63,    64,    17,    42,   410,    73,    67,    73,    30,    66,
      33,     9,    33,    66,    66,   319,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
      17,    66,    66,   447,    26,    73,    36,    67,    74,    33,
      17,    22,   456,   449,     7,    72,    66,    66,     4,    17,
       7,    42,    30,    23,     8,    44,   101,   471,    69,   145,
      57,    67,    49,    50,    51,    52,    53,    54,    55,    56,
      57,     4,     5,    60,    61,    62,    63,    64,   119,    12,
     449,    14,    15,   219,    72,   319,   323,   318,    21,    82,
     104,   372,   332,    96,    27,   291,   308,    30,   328,   462,
     329,   337,    35,    36,    37,    38,    39,    40,   453,    29,
      30,    31,    32,   455,    -1,    48,    36,    -1,    -1,    39,
      40,    41,    42,    29,    30,    31,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    -1,    -1,
      -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,    65,
      -1,    67,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    -1,    60,    61,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    -1,    60,    61,
      62,    63,    64,    -1,    -1,    -1,    -1,    69,    -1,    -1,
      72,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    -1,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    72,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    -1,    60,    61,    62,    63,
      64,    -1,    -1,    -1,    68,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    -1,    60,    61,    62,    63,
      64,    -1,    -1,    -1,    68,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    -1,    -1,    60,    61,    62,    63,
      64,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      -1,    -1,    60,    61,    62,    63,    64,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    -1,    -1,    60,    61,
      62,    63,    64,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    -1,    60,    61,    62,    63,    64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    76,    79,     0,    19,    46,    80,    81,    35,     6,
      13,    18,    24,    26,    47,    77,    67,    82,    35,    35,
      35,    35,    35,    35,    16,    89,    35,    41,    83,    84,
      66,    66,    66,    67,    66,    67,    49,    66,    66,    66,
      90,     6,    93,    68,    69,    68,    68,    35,    36,    41,
      59,   128,   129,   130,    30,    91,    92,    94,    95,    96,
      24,   120,    84,    66,    66,    66,    30,    34,    66,    69,
      95,    35,   121,   122,   123,    26,   147,   130,    92,    97,
     122,    35,   148,   149,   150,    78,    49,   124,   149,    35,
      36,    45,   152,   153,    13,    18,   156,   157,   158,   159,
     171,   172,    98,    49,    69,    73,    35,    39,    40,   179,
      35,    37,    38,   170,     4,   154,   157,    66,    85,    86,
      66,    85,    29,    30,    31,    32,    41,    58,    59,    65,
      67,   100,   101,   116,   118,   119,   215,   125,   153,   151,
     173,   176,   160,   161,   155,    89,   117,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    60,    61,    62,    63,
      64,    99,   101,     3,    11,    20,    42,    70,   126,   127,
     128,   131,   132,   133,   134,   137,   145,   126,    67,   162,
     162,   162,   162,     4,     5,    12,    14,    15,    21,    27,
      30,    35,    36,    37,    38,    39,    40,    48,   182,   184,
     185,   186,   187,   188,   189,   192,   194,   225,   227,   228,
     229,   230,   231,   232,   237,   245,   246,   249,   252,    93,
     101,   106,   109,   110,   112,   111,   107,   102,   103,   114,
     104,   115,   113,   108,   105,    66,    71,    17,   138,    42,
      66,    66,   163,    73,    73,    66,    66,   183,   238,   253,
      30,   233,   250,   247,   193,   226,    10,    66,    73,    33,
      33,     9,   235,   120,    68,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,    42,
     128,   135,   146,   139,   140,   141,    26,   164,   167,   168,
     174,   177,   184,    39,    40,    67,   100,   192,   200,   215,
     216,    36,   256,   200,   184,   200,    71,    74,   195,   196,
      67,   219,    74,   185,   187,   190,   191,   236,    87,    69,
      72,   126,    10,    66,    35,    45,   143,   144,   169,    66,
      68,   165,   166,   126,   180,   180,    10,   218,   217,    17,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    60,
      61,    62,    63,    64,   200,    33,   234,    25,   248,   197,
      43,    44,    45,   196,   220,   200,   200,   185,   147,   135,
      17,   140,    69,    73,   165,   167,   152,   175,   178,   219,
     200,   239,   205,   208,   209,   211,   210,   206,   201,   202,
     213,   203,   214,   212,   207,   204,   254,    22,   251,     7,
     200,    42,   200,   221,   223,    88,    72,   126,   136,   144,
     142,    73,    66,    66,    68,    28,    30,   240,   241,   242,
     243,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   257,   185,   200,   185,
      69,    72,   198,    73,   224,    68,    69,     4,   181,    17,
     126,    42,    10,    66,   244,    69,    73,   258,   260,     7,
     199,    30,   222,   184,   136,    10,   241,   243,   187,    23,
       8,   255,   200,   223,    10,   259,   261,   185,    72,   200,
     200
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
#line 65 "web2c-parser.y"
    { 
 	    printf ("#define %s\n", uppercasify (program_name));
            block_level++;
 	    printf ("#include \"%s\"\n", std_header);
	  }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 72 "web2c-parser.y"
    { printf ("\n#include \"%s\"\n", coerce_name); }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 75 "web2c-parser.y"
    { YYACCEPT; }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 85 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = field_id_tok;
	    }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 90 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = fun_id_tok;
	    }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 95 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = const_id_tok;
	    }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 100 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = fun_param_tok;
	    }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 105 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = proc_id_tok;
	    }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 110 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = proc_param_tok;
	    }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 115 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = type_id_tok;
	    }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 120 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = type_id_tok;
	      sym_table[ii].val = lower_bound;
	      sym_table[ii].val_sym = lower_sym;
	      sym_table[ii].upper = upper_bound;
	      sym_table[ii].upper_sym = upper_sym;
	    }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 129 "web2c-parser.y"
    {
	      ii = add_to_table (last_id); 
	      sym_table[ii].typ = var_id_tok;
	    }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 156 "web2c-parser.y"
    {	if (block_level > 0) my_output("{\n ");
                indent++; block_level++;
              }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 161 "web2c-parser.y"
    { if (block_level == 2) {
                  if (strcmp(fn_return_type, "void")) {
                    my_output("register");
                    my_output(fn_return_type);
                    my_output("Result;");
                  }
                  if (tex) {
                    sprintf(safe_string, "%s_regmem", my_routine);
                    my_output(safe_string);
                    new_line();
                  }
               }
             }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 175 "web2c-parser.y"
    { doing_statements = true; }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 177 "web2c-parser.y"
    {
              if (block_level == 2) {
                if (strcmp(fn_return_type,"void")) {
                  my_output("return Result");
                  semicolon();
                 }
                 if (tex) {
                   if (uses_mem && uses_eqtb)
                    fprintf(coerce,
             "#define %s_regmem register memoryword *mem=zmem, *eqtb=zeqtb;\n",
                       my_routine);
                   else if (uses_mem)
          fprintf(coerce, "#define %s_regmem register memoryword *mem=zmem;\n",
                          my_routine);
                   else if (uses_eqtb)
        fprintf(coerce, "#define %s_regmem register memoryword *eqtb=zeqtb;\n",
                          my_routine);
                   else
                     fprintf(coerce, "#define %s_regmem\n", my_routine);
                }
                my_routine[0] = '\0';
             }
             indent--; block_level--;
             my_output("}"); new_line();
             doing_statements = false;
            }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 207 "web2c-parser.y"
    { my_output("/*"); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 209 "web2c-parser.y"
    { my_output("*/"); }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 217 "web2c-parser.y"
    { my_output(temp); }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 223 "web2c-parser.y"
    { new_line(); }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 232 "web2c-parser.y"
    { /* `#define' must be in column 1 for pcc. */
            unsigned save = indent;
	    new_line ();
	    indent = 0;
	    my_output ("#define");
	    indent = save;
	  }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 240 "web2c-parser.y"
    { ii = add_to_table (last_id);
	    sym_table[ii].typ = const_id_tok;
            my_output (last_id);
          }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 244 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 245 "web2c-parser.y"
    { my_output (")"); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 246 "web2c-parser.y"
    { sym_table[ii].val = last_i_num; new_line(); }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 251 "web2c-parser.y"
    {
             sscanf (temp, "%ld", &last_i_num);
             if (my_labs ((long) last_i_num) > 32767)
               strcat (temp, "L");
             my_output (temp);
             (yyval) = ex_32;
           }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 258 "web2c-parser.y"
    { my_output(temp); (yyval) = ex_real; }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 259 "web2c-parser.y"
    { (yyval) = 0; }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 260 "web2c-parser.y"
    { (yyval) = ex_32; }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 265 "web2c-parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 266 "web2c-parser.y"
    { my_output ("+"); }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 267 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 268 "web2c-parser.y"
    { my_output ("-"); }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 269 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 270 "web2c-parser.y"
    { my_output ("*"); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 271 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 272 "web2c-parser.y"
    { my_output ("/"); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 273 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 274 "web2c-parser.y"
    { my_output ("=="); }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 275 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 276 "web2c-parser.y"
    { my_output ("!="); }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 277 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 278 "web2c-parser.y"
    { my_output ("%"); }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 279 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 280 "web2c-parser.y"
    { my_output ("<"); }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 281 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 282 "web2c-parser.y"
    { my_output (">"); }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 283 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 284 "web2c-parser.y"
    { my_output ("<="); }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 285 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 286 "web2c-parser.y"
    { my_output (">="); }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 287 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 288 "web2c-parser.y"
    { my_output ("&&"); }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 289 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 290 "web2c-parser.y"
    { my_output ("||"); }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 291 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 292 "web2c-parser.y"
    { my_output ("/ ((double)"); }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 293 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); my_output (")"); }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 294 "web2c-parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 299 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 301 "web2c-parser.y"
    { my_output (")"); (yyval) = (yyvsp[(1) - (4)]); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 307 "web2c-parser.y"
    { 
              char s[132];
              get_string_literal(s);
              my_output (s);
            }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 313 "web2c-parser.y"
    { 
              char s[5];
              get_single_char(s);
              my_output (s);
            }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 321 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 333 "web2c-parser.y"
    { my_output ("typedef"); }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 335 "web2c-parser.y"
    { 
            ii = add_to_table(last_id);
            sym_table[ii].typ = type_id_tok;
            strcpy(safe_string, last_id);
            last_type = ii;
          }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 342 "web2c-parser.y"
    {
            array_bounds[0] = 0;
            array_offset[0] = 0;
          }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 347 "web2c-parser.y"
    { 
            if (*array_offset) {
              yyerror ("Cannot typedef arrays with offsets");
            }
            my_output (safe_string);
            my_output (array_bounds);
            semicolon ();
            last_type = -1;
          }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 365 "web2c-parser.y"
    {
              if (last_type >= 0)
                {
                   sym_table[ii].val = lower_bound;
                   sym_table[ii].val_sym = lower_sym;
                   sym_table[ii].upper = upper_bound;
                   sym_table[ii].upper_sym = upper_sym;
                   ii= -1;
                 }
                 
              /* If the bounds on an integral type are known at
                 translation time, select the smallest ANSI C type which
                 can represent it.  We avoid using unsigned char and
                 unsigned short where possible, as ANSI compilers
                 (typically) convert them to int, while traditional
                 compilers convert them to unsigned int.  We also avoid
                 schar if possible, since it also is different on
                 different compilers (see config.h).  */
              if (lower_sym == -1 && upper_sym == -1) {
                if (0 <= lower_bound && upper_bound <= SCHAR_MAX)
                  my_output ("char");
                else if (SCHAR_MIN <= lower_bound && upper_bound <= SCHAR_MAX)
                  my_output ("schar");
                else if (0 <= lower_bound && upper_bound <= UCHAR_MAX)
                  my_output ("unsigned char");
                else if (SHRT_MIN <= lower_bound && upper_bound <= SHRT_MAX)
                  my_output ("short");
                else if (0 <= lower_bound && upper_bound <= USHRT_MAX)
                  my_output ("unsigned short");
                else
                  my_output ("integer");
              } else {
                  my_output ("integer");
              }
            }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 414 "web2c-parser.y"
    {
              lower_bound = upper_bound;
              lower_sym = upper_sym;
              sscanf (temp, "%ld", &upper_bound);
              upper_sym = -1; /* no sym table entry */
            }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 421 "web2c-parser.y"
    { 
              lower_bound = upper_bound;
              lower_sym = upper_sym;
              upper_bound = sym_table[l_s].val;
              upper_sym = l_s;
            }
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 428 "web2c-parser.y"
    { /* We've changed some constants into dynamic variables.
	         To avoid changing all the subrange decls, just use integer.
	         This does not work for arrays, for which we check later.  */
	      lower_bound = upper_bound;
	      lower_sym = upper_sym;
	      upper_bound = 0;
	      upper_sym = 0; /* Translate to integer.  */
	    }
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 437 "web2c-parser.y"
    { /* Same as var_id_tok, to avoid changing type definitions.
	         Should keep track of the variables we use in this way
	         and make sure they're all eventually defined.  */
	      lower_bound = upper_bound;
	      lower_sym = upper_sym;
	      upper_bound = 0;
	      upper_sym = 0;
	    }
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 449 "web2c-parser.y"
    {
            if (last_type >= 0) {
           sym_table[last_type].var_not_needed = sym_table[l_s].var_not_needed;
              sym_table[last_type].upper = sym_table[l_s].upper;
              sym_table[last_type].upper_sym = sym_table[l_s].upper_sym;
              sym_table[last_type].val = sym_table[l_s].val;
              sym_table[last_type].val_sym = sym_table[l_s].val_sym;
	    }
	    my_output (last_id);
	  }
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 463 "web2c-parser.y"
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 468 "web2c-parser.y"
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 472 "web2c-parser.y"
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 479 "web2c-parser.y"
    {
            if (last_type >= 0) {
              sym_table[last_type].var_not_needed = sym_table[l_s].var_not_needed;
              sym_table[last_type].upper = sym_table[l_s].upper;
              sym_table[last_type].upper_sym = sym_table[l_s].upper_sym;
              sym_table[last_type].val = sym_table[l_s].val;
              sym_table[last_type].val_sym = sym_table[l_s].val_sym;
	    }
	    my_output (last_id);
	    my_output ("*");
          }
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 499 "web2c-parser.y"
    { compute_array_bounds(); }
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 501 "web2c-parser.y"
    { 
              lower_bound = sym_table[l_s].val;
              lower_sym = sym_table[l_s].val_sym;
              upper_bound = sym_table[l_s].upper;
              upper_sym = sym_table[l_s].upper_sym;
              compute_array_bounds();
            }
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 514 "web2c-parser.y"
    { my_output ("struct"); my_output ("{"); indent++; }
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 516 "web2c-parser.y"
    { indent--; my_output ("}"); semicolon(); }
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 524 "web2c-parser.y"
    { field_list[0] = 0; }
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 526 "web2c-parser.y"
    {
				  /*array_bounds[0] = 0;
				  array_offset[0] = 0;*/
				}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 531 "web2c-parser.y"
    { int i=0, j; char ltemp[80];
				  while(field_list[i++] == '!') {
					j = 0;
					while (field_list[i])
					    ltemp[j++] = field_list[i++];
					i++;
					if (field_list[i] == '!')
						ltemp[j++] = ',';
					ltemp[j] = 0;
					my_output (ltemp);
				  }
				  semicolon();
				}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 552 "web2c-parser.y"
    { int i=0, j=0;
				  while (field_list[i] == '!')
					while(field_list[i++]);
				  ii = add_to_table(last_id);
				  sym_table[ii].typ = field_id_tok;
				  field_list[i++] = '!';
				  while (last_id[j])
					field_list[i++] = last_id[j++];
				  field_list[i++] = 0;
				  field_list[i++] = 0;
				}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 564 "web2c-parser.y"
    { int i=0, j=0;
				  while (field_list[i] == '!')
					while(field_list[i++]);
				  field_list[i++] = '!';
				  while (last_id[j])
					field_list[i++] = last_id[j++];
				  field_list[i++] = 0;
				  field_list[i++] = 0;
				}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 577 "web2c-parser.y"
    { my_output ("text /* of "); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 579 "web2c-parser.y"
    { my_output ("*/"); }
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 593 "web2c-parser.y"
    { 
            var_list[0] = 0;
            array_bounds[0] = 0;
            array_offset[0] = 0;
            var_formals = false;
            ids_paramed = 0;
          }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 601 "web2c-parser.y"
    {
            array_bounds[0] = 0;	
            array_offset[0] = 0;
          }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 606 "web2c-parser.y"
    { fixup_var_list(); }
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 614 "web2c-parser.y"
    { int i=0, j=0;
				  ii = add_to_table(last_id);
				  sym_table[ii].typ = var_id_tok;
				  sym_table[ii].var_formal = var_formals;
				  param_id_list[ids_paramed++] = ii;
	  			  while (var_list[i] == '!')
					while(var_list[i++]);
				  var_list[i++] = '!';
				  while (last_id[j])
					var_list[i++] = last_id[j++];
	  			  var_list[i++] = 0;
				  var_list[i++] = 0;
				}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 628 "web2c-parser.y"
    { int i=0, j=0;
				  ii = add_to_table(last_id);
	  			  sym_table[ii].typ = var_id_tok;
				  sym_table[ii].var_formal = var_formals;
				  param_id_list[ids_paramed++] = ii;
	  			  while (var_list[i] == '!')
					while (var_list[i++]);
	  			  var_list[i++] = '!';
				  while (last_id[j])
					var_list[i++] = last_id[j++];
	  			  var_list[i++] = 0;
				  var_list[i++] = 0;
				}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 642 "web2c-parser.y"
    { int i=0, j=0;
				  ii = add_to_table(last_id);
	  			  sym_table[ii].typ = var_id_tok;
				  sym_table[ii].var_formal = var_formals;
				  param_id_list[ids_paramed++] = ii;
	  			  while (var_list[i] == '!')
					while(var_list[i++]);
	  			  var_list[i++] = '!';
				  while (last_id[j])
					var_list[i++] = last_id[j++];
				  var_list[i++] = 0;
				  var_list[i++] = 0;
				}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 660 "web2c-parser.y"
    { my_output ("void mainbody() {");
		  indent++;
		  new_line ();
		}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 665 "web2c-parser.y"
    { indent--;
                  my_output ("}");
                  new_line ();
                }
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 677 "web2c-parser.y"
    { new_line(); remove_locals(); }
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 679 "web2c-parser.y"
    { new_line(); remove_locals(); }
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 687 "web2c-parser.y"
    { ii = add_to_table(last_id);
	      if (debug)
	        fprintf(stderr, "%3d Procedure %s\n", pf_count++, last_id);
	      sym_table[ii].typ = proc_id_tok;
	      strcpy(my_routine, last_id);
	      uses_eqtb = uses_mem = false;
	      my_output ("void");
	      new_line ();
	      orig_out = out;
	      out = 0;
	    }
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 699 "web2c-parser.y"
    { strcpy(fn_return_type, "void");
	      do_proc_args();
	      gen_function_head(); }
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 703 "web2c-parser.y"
    { ii = l_s; 
	      if (debug)
	        fprintf(stderr, "%3d Procedure %s\n", pf_count++, last_id);
	      strcpy(my_routine, last_id);
	      my_output ("void");
	      new_line ();
	    }
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 711 "web2c-parser.y"
    { strcpy(fn_return_type, "void");
	      do_proc_args();
	      gen_function_head();
            }
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 719 "web2c-parser.y"
    {
              strcpy (z_id, last_id);
	      mark ();
	      ids_paramed = 0;
	    }
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 725 "web2c-parser.y"
    { sprintf (z_id, "z%s", last_id);
	      ids_paramed = 0;
	      if (sym_table[ii].typ == proc_id_tok)
	        sym_table[ii].typ = proc_param_tok;
	      else if (sym_table[ii].typ == fun_id_tok)
	        sym_table[ii].typ = fun_param_tok;
	      mark();
	    }
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 741 "web2c-parser.y"
    { ids_typed = ids_paramed; }
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 743 "web2c-parser.y"
    { int i, need_var;
	      i = search_table(last_id);
	      need_var = !sym_table[i].var_not_needed;
	      for (i=ids_typed; i<ids_paramed; i++)
                {
	          strcpy(arg_type[i], last_id);
		  if (need_var && sym_table[param_id_list[i]].var_formal)
	            strcat(arg_type[i], " *");
		  else
                    sym_table[param_id_list[i]].var_formal = false;
	        }
	    }
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 757 "web2c-parser.y"
    {var_formals = 0; }
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 758 "web2c-parser.y"
    {var_formals = 1; }
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 770 "web2c-parser.y"
    { 
              orig_out = out;
              out = 0;
              ii = add_to_table(last_id);
              if (debug)
                fprintf(stderr, "%3d Function %s\n", pf_count++, last_id);
              sym_table[ii].typ = fun_id_tok;
              strcpy (my_routine, last_id);
              uses_eqtb = uses_mem = false;
            }
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 781 "web2c-parser.y"
    { 
              normal();
              array_bounds[0] = 0;
              array_offset[0] = 0;
            }
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 787 "web2c-parser.y"
    { 
              get_result_type(fn_return_type);
              do_proc_args();
              gen_function_head();
            }
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 794 "web2c-parser.y"
    { 
              orig_out = out;
              out = 0;
              ii = l_s;
              if (debug)
                fprintf(stderr, "%3d Function %s\n", pf_count++, last_id);
              strcpy(my_routine, last_id);
              uses_eqtb = uses_mem = false;
            }
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 804 "web2c-parser.y"
    { normal();
              array_bounds[0] = 0;
              array_offset[0] = 0;
            }
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 809 "web2c-parser.y"
    { get_result_type(fn_return_type);
              do_proc_args();
              gen_function_head();
            }
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 827 "web2c-parser.y"
    { my_output ("{"); indent++; new_line(); }
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 829 "web2c-parser.y"
    { indent--; my_output ("}"); new_line(); }
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 842 "web2c-parser.y"
    {if (!doreturn(temp)) {
				      sprintf(safe_string, "lab%s:", temp);
				    my_output (safe_string);
				 }
				}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 850 "web2c-parser.y"
    { semicolon(); }
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 852 "web2c-parser.y"
    { semicolon(); }
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 860 "web2c-parser.y"
    { my_output ("break"); }
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 864 "web2c-parser.y"
    { my_output ("="); }
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 867 "web2c-parser.y"
    { my_output ("Result ="); }
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 872 "web2c-parser.y"
    { if (strcmp(last_id, "mem") == 0)
					uses_mem = 1;
				  else if (strcmp(last_id, "eqtb") == 0)
					uses_eqtb = 1;
				  if (sym_table[l_s].var_formal)
					putchar('*');
				  my_output (last_id);
				  (yyval) = ex_32;
				}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 883 "web2c-parser.y"
    { if (sym_table[l_s].var_formal)
					putchar('*');
				  my_output (last_id); (yyval) = ex_32; }
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 889 "web2c-parser.y"
    { (yyval) = ex_32; }
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 891 "web2c-parser.y"
    { (yyval) = ex_32; }
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 899 "web2c-parser.y"
    { my_output ("["); }
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 901 "web2c-parser.y"
    { my_output ("]"); }
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 903 "web2c-parser.y"
    {if (tex || mf) {
				   if (strcmp(last_id, "int")==0)
					my_output (".cint");
				   else if (strcmp(last_id, "lh")==0)
					my_output (".v.LH");
				   else if (strcmp(last_id, "rh")==0)
					my_output (".v.RH");
				   else {
				     sprintf(safe_string, ".%s", last_id);
				     my_output (safe_string);
				   }
				 }
				 else {
				    sprintf(safe_string, ".%s", last_id);
				    my_output (safe_string);
				 }
				}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 921 "web2c-parser.y"
    { my_output (".hh.b0"); }
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 923 "web2c-parser.y"
    { my_output (".hh.b1"); }
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 928 "web2c-parser.y"
    { my_output ("]["); }
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 933 "web2c-parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); }
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 934 "web2c-parser.y"
    { my_output ("+"); }
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 935 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 936 "web2c-parser.y"
    { my_output ("-"); }
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 937 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 938 "web2c-parser.y"
    { my_output ("*"); }
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 939 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 940 "web2c-parser.y"
    { my_output ("/"); }
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 941 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 942 "web2c-parser.y"
    { my_output ("=="); }
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 943 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 944 "web2c-parser.y"
    { my_output ("!="); }
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 945 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 946 "web2c-parser.y"
    { my_output ("%"); }
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 947 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 948 "web2c-parser.y"
    { my_output ("<"); }
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 949 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 950 "web2c-parser.y"
    { my_output (">"); }
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 951 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 952 "web2c-parser.y"
    { my_output ("<="); }
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 953 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 954 "web2c-parser.y"
    { my_output (">="); }
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 955 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 956 "web2c-parser.y"
    { my_output ("&&"); }
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 957 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 958 "web2c-parser.y"
    { my_output ("||"); }
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 959 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 961 "web2c-parser.y"
    { my_output ("/ ((double)"); }
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 963 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); my_output (")"); }
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 965 "web2c-parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 971 "web2c-parser.y"
    { my_output ("- (integer)"); }
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 973 "web2c-parser.y"
    { my_output ("!"); }
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 978 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 980 "web2c-parser.y"
    { my_output (")"); (yyval) = (yyvsp[(1) - (4)]); }
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 984 "web2c-parser.y"
    { my_output (last_id); my_output ("()"); }
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 986 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 991 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 992 "web2c-parser.y"
    { my_output (")"); }
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 997 "web2c-parser.y"
    { my_output (","); }
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1004 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1013 "web2c-parser.y"
    { my_output (last_id); my_output ("()"); }
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 1015 "web2c-parser.y"
    { my_output (last_id);
				  ii = add_to_table(last_id);
	  			  sym_table[ii].typ = proc_id_tok;
				  my_output ("()");
				}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 1021 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1026 "web2c-parser.y"
    {if (doreturn(temp)) {
				    if (strcmp(fn_return_type,"void"))
					my_output ("return Result");
				    else
					my_output ("return");
				 } else {
				     sprintf(safe_string, "goto lab%s",
					temp);
				     my_output (safe_string);
				 }
				}
    break;

  case 271:

/* Line 1455 of yacc.c  */
#line 1056 "web2c-parser.y"
    { my_output ("if"); my_output ("("); }
    break;

  case 272:

/* Line 1455 of yacc.c  */
#line 1058 "web2c-parser.y"
    { my_output (")"); new_line (); }
    break;

  case 274:

/* Line 1455 of yacc.c  */
#line 1063 "web2c-parser.y"
    { my_output ("else"); }
    break;

  case 276:

/* Line 1455 of yacc.c  */
#line 1068 "web2c-parser.y"
    { my_output ("switch"); my_output ("("); }
    break;

  case 277:

/* Line 1455 of yacc.c  */
#line 1070 "web2c-parser.y"
    { my_output (")"); new_line();
				  my_output ("{"); indent++;
				}
    break;

  case 278:

/* Line 1455 of yacc.c  */
#line 1074 "web2c-parser.y"
    { indent--; my_output ("}"); new_line(); }
    break;

  case 281:

/* Line 1455 of yacc.c  */
#line 1082 "web2c-parser.y"
    { my_output ("break"); semicolon(); }
    break;

  case 284:

/* Line 1455 of yacc.c  */
#line 1090 "web2c-parser.y"
    { my_output ("case"); 
				  my_output (temp);
				  my_output (":"); new_line();
				}
    break;

  case 285:

/* Line 1455 of yacc.c  */
#line 1095 "web2c-parser.y"
    { my_output ("default:"); new_line(); }
    break;

  case 291:

/* Line 1455 of yacc.c  */
#line 1108 "web2c-parser.y"
    { my_output ("while");
				  my_output ("(");
				}
    break;

  case 292:

/* Line 1455 of yacc.c  */
#line 1112 "web2c-parser.y"
    { my_output (")"); }
    break;

  case 294:

/* Line 1455 of yacc.c  */
#line 1117 "web2c-parser.y"
    { my_output ("do"); my_output ("{"); indent++; }
    break;

  case 295:

/* Line 1455 of yacc.c  */
#line 1119 "web2c-parser.y"
    { indent--; my_output ("}"); 
				  my_output ("while"); my_output ("( ! (");
				}
    break;

  case 296:

/* Line 1455 of yacc.c  */
#line 1123 "web2c-parser.y"
    { my_output (") )"); }
    break;

  case 297:

/* Line 1455 of yacc.c  */
#line 1127 "web2c-parser.y"
    {
				  my_output ("{");
				  my_output ("register");
				  my_output ("integer");
				  if (strict_for)
					my_output ("for_begin,");
				  my_output ("for_end;");
				 }
    break;

  case 298:

/* Line 1455 of yacc.c  */
#line 1136 "web2c-parser.y"
    { if (strict_for)
					my_output ("for_begin");
				  else
					my_output (control_var);
				  my_output ("="); }
    break;

  case 299:

/* Line 1455 of yacc.c  */
#line 1142 "web2c-parser.y"
    { my_output ("; if (");
				  if (strict_for) my_output ("for_begin");
				  else my_output (control_var);
				  my_output (relation);
				  my_output ("for_end)");
				  if (strict_for) {
					my_output ("{");
					my_output (control_var);
					my_output ("=");
					my_output ("for_begin");
					semicolon();
				  }
				  my_output ("do"); 
				  indent++; 
				  new_line();
				  }
    break;

  case 300:

/* Line 1455 of yacc.c  */
#line 1159 "web2c-parser.y"
    {
				  char *top = strrchr (for_stack, '#');
				  indent--;
                                  new_line();
				  my_output ("while"); 
				  my_output ("("); 
				  my_output (top+1); 
				  my_output (")"); 
				  my_output (";");
				  my_output ("}");
				  if (strict_for)
					my_output ("}");
				  *top=0;
				  new_line();
				}
    break;

  case 301:

/* Line 1455 of yacc.c  */
#line 1177 "web2c-parser.y"
    { strcpy(control_var, last_id); }
    break;

  case 302:

/* Line 1455 of yacc.c  */
#line 1181 "web2c-parser.y"
    { my_output (";"); }
    break;

  case 303:

/* Line 1455 of yacc.c  */
#line 1183 "web2c-parser.y"
    { 
				  strcpy(relation, "<=");
				  my_output ("for_end");
				  my_output ("="); }
    break;

  case 304:

/* Line 1455 of yacc.c  */
#line 1188 "web2c-parser.y"
    { 
				  sprintf(for_stack + strlen(for_stack),
				    "#%s++ < for_end", control_var);
				}
    break;

  case 305:

/* Line 1455 of yacc.c  */
#line 1193 "web2c-parser.y"
    { my_output (";"); }
    break;

  case 306:

/* Line 1455 of yacc.c  */
#line 1195 "web2c-parser.y"
    {
				  strcpy(relation, ">=");
				  my_output ("for_end");
				  my_output ("="); }
    break;

  case 307:

/* Line 1455 of yacc.c  */
#line 1200 "web2c-parser.y"
    { 
				  sprintf(for_stack + strlen(for_stack),
				    "#%s-- > for_end", control_var);
				}
    break;



/* Line 1455 of yacc.c  */
#line 3884 "../../../../tex-live-2009test-1.3/texk/web2c/web2c/web2c-parser.c"
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



/* Line 1675 of yacc.c  */
#line 1205 "web2c-parser.y"


static void
compute_array_bounds P1H(void)
{
  long lb;
  char tmp[200];

  if (lower_sym == 0 || upper_sym == 0) {
    yyerror ("Cannot handle variable subrange in array decl");
  }
  else if (lower_sym == -1) {	/* lower is a constant */
    lb = lower_bound - 1;
    if (lb==0) lb = -1;	/* Treat lower_bound==1 as if lower_bound==0 */
    if (upper_sym == -1)	/* both constants */
        sprintf(tmp, "[%ld]", upper_bound - lb);
    else {			/* upper a symbol, lower constant */
        if (lb < 0)
            sprintf(tmp, "[%s + %ld]",
                            symbol(upper_sym), (-lb));
        else
            sprintf(tmp, "[%s - %ld]",
                            symbol(upper_sym), lb);
    }
    if (lower_bound < 0 || lower_bound > 1) {
        if (*array_bounds) {
          yyerror ("Cannot handle offset in second dimension");
        }
        if (lower_bound < 0) {
            sprintf(array_offset, "+%ld", -lower_bound);
        } else {
            sprintf(array_offset, "-%ld", lower_bound);
        }
    }
    strcat(array_bounds, tmp);
  } else {			/* lower is a symbol */
      if (upper_sym != -1)	/* both are symbols */
          sprintf(tmp, "[%s - %s + 1]", symbol(upper_sym),
              symbol(lower_sym));
      else {			/* upper constant, lower symbol */
          sprintf(tmp, "[%ld - %s]", upper_bound + 1,
              symbol(lower_sym));
      }
      if (*array_bounds) {
        yyerror ("Cannot handle symbolic offset in second dimension");
      }
      sprintf(array_offset, "- (int)(%s)", symbol(lower_sym));
      strcat(array_bounds, tmp);
  }
}


/* Kludge around negative lower array bounds.  */

static void
fixup_var_list ()
{
  int i, j;
  char output_string[100], real_symbol[100];

  for (i = 0; var_list[i++] == '!'; )
    {
      for (j = 0; (real_symbol[j++] = var_list[i++]); )
        ;
      if (*array_offset)
        {
          fprintf (out, "\n#define %s (%s %s)\n  ",
                          real_symbol, next_temp, array_offset);
          strcpy (real_symbol, next_temp);
          /* Add the temp to the symbol table, so that change files can
             use it later on if necessary.  */
          j = add_to_table (next_temp);
          sym_table[j].typ = var_id_tok;
          find_next_temp ();
        }
      sprintf (output_string, "%s%s%c", real_symbol, array_bounds,
                      var_list[i] == '!' ? ',' : ' ');
      my_output (output_string);
  }
  semicolon ();
}


/* If we're not processing TeX, we return false.  Otherwise,
   return true if the label is "10" and we're not in one of four TeX
   routines where the line labeled "10" isn't the end of the routine.
   Otherwise, return 0.  */
   
static boolean
doreturn P1C(string, label)
{
    return
      tex
      && STREQ (label, "10")
      && !STREQ (my_routine, "macrocall")
      && !STREQ (my_routine, "hpack")
      && !STREQ (my_routine, "vpackage")
      && !STREQ (my_routine, "trybreak");
}


/* Return the absolute value of a long.  */
static long 
my_labs P1C(long, x)
{
    if (x < 0L) return(-x);
    return(x);
}


/* Output current function declaration to coerce file.  */

static void
do_proc_args P1H(void)
{
  /* If we want ANSI code and one of the parameters is a var
     parameter, then use the #define to add the &.  We do this by
     adding a 'z' at the front of the name.  gen_function_head will do
     the real work.  */
  int i;
  int var = 0;
  for (i = 0; i < ids_paramed; ++i)
    var += sym_table[param_id_list[i]].var_formal;
  if (var) {
    for (i = strlen (z_id); i >= 0; --i)
      z_id[i+1] = z_id[i];
    z_id[0] = 'z';
  }

  /* We can't use our P?H macros here, since there might be an arbitrary
     number of function arguments.  */
  fprintf (coerce, "%s %s AA((", fn_return_type, z_id);
  if (ids_paramed == 0) fprintf (coerce, "void");
  for (i = 0; i < ids_paramed; i++) {
    if (i > 0)
      putc (',', coerce);
    fprintf (coerce, "%s %s", arg_type[i], symbol (param_id_list[i]));
  }
  fprintf (coerce, "));\n");
}

static void
gen_function_head P1H(void)
{
    int i;

    if (strcmp(my_routine, z_id)) {
	fprintf(coerce, "#define %s(", my_routine);
	for (i=0; i<ids_paramed; i++) {
	    if (i > 0)
		fprintf(coerce, ", %s", symbol(param_id_list[i]));
	    else
		fprintf(coerce, "%s", symbol(param_id_list[i]));
	}
	fprintf(coerce, ") %s(", z_id);
	for (i=0; i<ids_paramed; i++) {
	    if (i > 0)
		fputs(", ", coerce);
	    fprintf(coerce, "(%s) ", arg_type[i]);
	    fprintf(coerce, "%s(%s)",
		    sym_table[param_id_list[i]].var_formal?"&":"",
		    symbol(param_id_list[i]));
	}
	fprintf(coerce, ")\n");
    }
    out = orig_out;
    new_line ();
    /* We can't use our P?C macros here, since there might be an arbitrary
       number of function arguments.  We have to include the #ifdef in the
       generated code, or we'd generate different code with and without
       prototypes, which might cause splitup to create different numbers
       of files in each case. */
    fputs ("#ifdef HAVE_PROTOTYPES\n", out);
    my_output (z_id);
    my_output ("(");
    if (ids_paramed == 0) my_output ("void");
    for (i=0; i<ids_paramed; i++) {
        if (i > 0) my_output (",");
        my_output (arg_type[i]);
        my_output (symbol (param_id_list[i]));
    }
    my_output (")");
    new_line ();
    fputs ("#else\n", out);
    my_output (z_id);
    my_output ("(");
    for (i=0; i<ids_paramed; i++) {
        if (i > 0) my_output (",");
        my_output (symbol (param_id_list[i]));
    }
    my_output (")");
    new_line ();
    indent++;
    for (i=0; i<ids_paramed; i++) {
        my_output (arg_type[i]);
        my_output (symbol(param_id_list[i]));
        semicolon ();
    }
    indent--;
    fputs ("#endif\n", out);
}

