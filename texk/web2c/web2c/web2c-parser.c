/* A Bison parser, made by GNU Bison 2.5.1.  */

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
#define YYBISON_VERSION "2.5.1"

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

/* Line 268 of yacc.c  */
#line 24 "web2c-parser.y"

#include "web2c.h"

#define YYDEBUG 1

#define	symbol(x)	sym_table[x].id
#define	MAX_ARGS	50

static char fn_return_type[50], for_stack[300], control_var[50],
            relation[3];
static char arg_type[MAX_ARGS][30];
static int last_type = -1, ids_typed;
static int proc_is_noreturn = 0;
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

static long my_labs (long);
static void compute_array_bounds (void);
static void fixup_var_list (void);
static void do_proc_args (void);
static void gen_function_head (void);
static boolean doreturn (string);


/* Line 268 of yacc.c  */
#line 108 "web2c-parser.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

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
     great_eq_tok = 305,
     less_eq_tok = 306,
     not_eq_tok = 307,
     or_tok = 308,
     unary_minus_tok = 309,
     unary_plus_tok = 310,
     and_tok = 311,
     mod_tok = 312,
     div_tok = 313,
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
#define great_eq_tok 305
#define less_eq_tok 306
#define not_eq_tok 307
#define or_tok 308
#define unary_minus_tok 309
#define unary_plus_tok 310
#define and_tok 311
#define mod_tok 312
#define div_tok 313
#define not_tok 314




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 345 of yacc.c  */
#line 276 "web2c-parser.c"

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
#define YYLAST   562

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  193
/* YYNRULES -- Number of rules.  */
#define YYNRULES  315
/* YYNRULES -- Number of states.  */
#define YYNSTATES  492

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   314

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      68,    69,    61,    56,    70,    57,    75,    62,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    74,    67,
      51,    50,    52,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    72,     2,    73,    71,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    53,    54,    55,    58,    59,
      60,    63,    64,    65,    66
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
     399,   405,   407,   410,   413,   416,   419,   421,   422,   426,
     427,   433,   434,   440,   441,   442,   447,   449,   453,   454,
     459,   460,   463,   464,   468,   470,   472,   475,   476,   477,
     478,   488,   489,   490,   491,   501,   503,   505,   507,   511,
     512,   517,   519,   523,   525,   529,   531,   533,   535,   537,
     539,   541,   543,   545,   546,   551,   552,   557,   558,   562,
     564,   566,   568,   570,   573,   574,   579,   582,   585,   588,
     590,   591,   596,   599,   600,   605,   606,   611,   612,   617,
     618,   623,   624,   629,   630,   635,   636,   641,   642,   647,
     648,   653,   654,   659,   660,   665,   666,   671,   672,   677,
     678,   683,   685,   687,   689,   691,   692,   697,   699,   701,
     703,   704,   708,   709,   714,   716,   717,   722,   725,   727,
     730,   731,   733,   735,   736,   740,   743,   744,   746,   748,
     750,   752,   754,   755,   759,   760,   764,   765,   770,   771,
     772,   779,   780,   781,   785,   786,   787,   795,   797,   801,
     805,   807,   811,   813,   815,   817,   820,   822,   824,   826,
     827,   828,   835,   836,   837,   844,   845,   846,   847,   857,
     859,   860,   861,   867,   868,   869
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      77,     0,    -1,    -1,    -1,    80,    82,    78,    90,    94,
     121,   148,    79,   157,   155,    -1,    -1,    80,    81,    -1,
      47,    48,    36,    67,    -1,    47,    13,    36,    67,    -1,
      47,     6,    36,    67,    -1,    47,    13,    36,    68,    69,
      67,    -1,    47,    18,    36,    67,    -1,    47,    18,    36,
      68,    69,    67,    -1,    47,    24,    36,    67,    -1,    47,
      24,    36,    50,   129,    67,    -1,    47,    26,    36,    67,
      -1,    19,    36,    83,    67,    -1,    68,    84,    69,    -1,
      -1,    85,    -1,    84,    70,    85,    -1,    42,    -1,    36,
      -1,    -1,    -1,    -1,    87,    90,    94,   121,    88,   148,
      89,   184,    -1,    -1,    -1,    16,    91,    92,    67,    -1,
      93,    -1,    92,    70,    93,    -1,    31,    -1,    -1,     6,
      95,    -1,    96,    -1,    95,    96,    -1,    -1,    -1,    -1,
      -1,    97,    36,    98,    50,    99,   102,   100,    67,    -1,
      31,    -1,    30,    -1,   119,    -1,   120,    -1,   218,   102,
      -1,    -1,   102,    56,   103,   102,    -1,    -1,   102,    57,
     104,   102,    -1,    -1,   102,    61,   105,   102,    -1,    -1,
     102,    65,   106,   102,    -1,    -1,   102,    50,   107,   102,
      -1,    -1,   102,    55,   108,   102,    -1,    -1,   102,    64,
     109,   102,    -1,    -1,   102,    51,   110,   102,    -1,    -1,
     102,    52,   111,   102,    -1,    -1,   102,    54,   112,   102,
      -1,    -1,   102,    53,   113,   102,    -1,    -1,   102,    63,
     114,   102,    -1,    -1,   102,    58,   115,   102,    -1,    -1,
     102,    62,   116,   102,    -1,   117,    -1,    -1,    68,   118,
     102,    69,    -1,   101,    -1,    32,    -1,    33,    -1,    42,
      -1,    -1,    24,   122,    -1,   123,    -1,   122,   123,    -1,
      -1,    -1,    -1,   124,    36,   125,    50,   126,   127,    67,
      -1,   128,    -1,   133,    -1,   129,    -1,   132,    -1,   131,
      35,   131,    -1,    -1,    60,    -1,   130,    31,    -1,    42,
      -1,    37,    -1,    36,    -1,    43,    -1,   135,    -1,   138,
      -1,   146,    -1,   134,    -1,    71,    43,    -1,     3,    72,
     136,    73,    17,   137,    -1,     3,    72,   136,    70,   136,
      73,    17,   137,    -1,   129,    -1,    43,    -1,   127,    -1,
      -1,    20,   139,   140,    10,    -1,   141,    -1,   140,    67,
     141,    -1,    -1,    -1,   142,   144,    74,   143,   127,    -1,
      -1,   145,    -1,   144,    70,   145,    -1,    36,    -1,    46,
      -1,    -1,    11,    17,   147,   127,    -1,    -1,    26,   149,
      -1,   150,    -1,   149,   150,    -1,    -1,    -1,   151,   153,
      74,   152,   127,    67,    -1,   154,    -1,   153,    70,   154,
      -1,    36,    -1,    37,    -1,    46,    -1,    -1,    -1,     4,
     156,   187,    10,    75,    -1,   158,    -1,   157,   158,    -1,
     159,    67,    -1,   174,    67,    -1,   162,    86,    -1,    18,
      -1,    -1,    28,   161,    18,    -1,    -1,   160,    36,   163,
     165,    67,    -1,    -1,    18,   173,   164,   165,    67,    -1,
      -1,    -1,    68,   166,   167,    69,    -1,   170,    -1,   167,
      67,   170,    -1,    -1,   169,   153,    74,    43,    -1,    -1,
     171,   168,    -1,    -1,    26,   172,   168,    -1,    38,    -1,
      39,    -1,   175,    86,    -1,    -1,    -1,    -1,    13,    36,
     176,   165,    74,   177,   183,   178,    67,    -1,    -1,    -1,
      -1,    13,   182,   179,   165,    74,   180,   183,   181,    67,
      -1,    40,    -1,    41,    -1,   127,    -1,     4,   187,    10,
      -1,    -1,     4,   186,   187,    10,    -1,   188,    -1,   187,
      67,   188,    -1,   190,    -1,   189,    74,   190,    -1,    31,
      -1,   191,    -1,   232,    -1,   192,    -1,   228,    -1,   230,
      -1,   231,    -1,    49,    -1,    -1,   195,    34,   193,   203,
      -1,    -1,   197,    34,   194,   203,    -1,    -1,    37,   196,
     198,    -1,    37,    -1,    40,    -1,    41,    -1,   199,    -1,
     198,   199,    -1,    -1,    72,   200,   203,   201,    -1,    75,
      46,    -1,    75,    44,    -1,    75,    45,    -1,    73,    -1,
      -1,    70,   202,   203,    73,    -1,   218,   203,    -1,    -1,
     203,    56,   204,   203,    -1,    -1,   203,    57,   205,   203,
      -1,    -1,   203,    61,   206,   203,    -1,    -1,   203,    65,
     207,   203,    -1,    -1,   203,    50,   208,   203,    -1,    -1,
     203,    55,   209,   203,    -1,    -1,   203,    64,   210,   203,
      -1,    -1,   203,    51,   211,   203,    -1,    -1,   203,    52,
     212,   203,    -1,    -1,   203,    54,   213,   203,    -1,    -1,
     203,    53,   214,   203,    -1,    -1,   203,    63,   215,   203,
      -1,    -1,   203,    58,   216,   203,    -1,    -1,   203,    62,
     217,   203,    -1,   219,    -1,    60,    -1,    59,    -1,    66,
      -1,    -1,    68,   220,   203,    69,    -1,   195,    -1,   101,
      -1,    40,    -1,    -1,    41,   221,   222,    -1,    -1,    68,
     223,   224,    69,    -1,   226,    -1,    -1,   224,    70,   225,
     226,    -1,   203,   227,    -1,    43,    -1,    74,    31,    -1,
      -1,    38,    -1,    36,    -1,    -1,    39,   229,   222,    -1,
      14,    31,    -1,    -1,   185,    -1,   233,    -1,   252,    -1,
     234,    -1,   244,    -1,    -1,    15,   235,   236,    -1,    -1,
     203,   237,   238,    -1,    -1,    22,   239,   188,   242,    -1,
      -1,    -1,    22,    15,   240,   236,   241,   242,    -1,    -1,
      -1,     9,   243,   188,    -1,    -1,    -1,     5,   245,   203,
      17,   246,   247,   251,    -1,   248,    -1,   247,    67,   248,
      -1,   249,    74,   190,    -1,   250,    -1,   249,    70,   250,
      -1,    31,    -1,    29,    -1,    10,    -1,    67,    10,    -1,
     253,    -1,   256,    -1,   259,    -1,    -1,    -1,    27,   254,
     203,   255,     7,   188,    -1,    -1,    -1,    21,   257,   187,
      25,   258,   203,    -1,    -1,    -1,    -1,    12,   260,   263,
      34,   261,   264,     7,   262,   188,    -1,    37,    -1,    -1,
      -1,   203,   265,    23,   266,   203,    -1,    -1,    -1,   203,
     267,     8,   268,   203,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    66,    66,    73,    64,    80,    82,    85,    90,    95,
     100,   105,   110,   115,   120,   129,   138,   142,   143,   147,
     148,   152,   153,   157,   162,   176,   157,   206,   208,   207,
     213,   214,   218,   221,   223,   228,   229,   233,   241,   245,
     246,   233,   251,   259,   260,   261,   265,   267,   267,   269,
     269,   271,   271,   273,   273,   275,   275,   277,   277,   279,
     279,   281,   281,   283,   283,   285,   285,   287,   287,   289,
     289,   291,   291,   293,   293,   295,   300,   299,   303,   307,
     313,   322,   325,   326,   329,   330,   334,   336,   343,   334,
     360,   361,   365,   398,   402,   405,   407,   411,   418,   425,
     434,   446,   460,   464,   465,   469,   476,   491,   492,   496,
     498,   508,   512,   511,   517,   518,   522,   524,   522,   542,
     545,   546,   549,   561,   575,   574,   580,   582,   586,   587,
     591,   599,   591,   607,   608,   611,   625,   639,   655,   658,
     657,   670,   671,   674,   676,   681,   684,   686,   685,   692,
     691,   708,   707,   724,   730,   729,   741,   742,   746,   746,
     762,   762,   763,   763,   767,   768,   771,   775,   786,   792,
     774,   799,   809,   814,   798,   821,   822,   825,   828,   832,
     831,   837,   838,   841,   842,   846,   854,   856,   860,   861,
     862,   863,   864,   869,   868,   872,   871,   877,   876,   887,
     893,   895,   899,   900,   904,   903,   907,   925,   927,   931,
     933,   932,   937,   939,   939,   941,   941,   943,   943,   945,
     945,   947,   947,   949,   949,   951,   951,   953,   953,   955,
     955,   957,   957,   959,   959,   961,   961,   963,   963,   966,
     965,   969,   974,   975,   977,   983,   982,   986,   987,   988,
     991,   990,   996,   996,  1001,  1002,  1002,  1007,  1008,  1013,
    1014,  1017,  1019,  1026,  1025,  1030,  1044,  1047,  1048,  1049,
    1052,  1053,  1057,  1056,  1062,  1061,  1067,  1066,  1070,  1073,
    1069,  1077,  1079,  1078,  1084,  1086,  1083,  1093,  1094,  1097,
    1101,  1102,  1105,  1110,  1114,  1115,  1118,  1119,  1120,  1124,
    1128,  1123,  1133,  1135,  1132,  1143,  1152,  1158,  1142,  1192,
    1197,  1199,  1196,  1209,  1211,  1208
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
  "to_tok", "type_tok", "until_tok", "var_tok", "while_tok",
  "noreturn_tok", "others_tok", "r_num_tok", "i_num_tok",
  "string_literal_tok", "single_char_tok", "assign_tok", "two_dots_tok",
  "undef_id_tok", "var_id_tok", "proc_id_tok", "proc_param_tok",
  "fun_id_tok", "fun_param_tok", "const_id_tok", "type_id_tok", "hhb0_tok",
  "hhb1_tok", "field_id_tok", "define_tok", "field_tok", "break_tok",
  "'='", "'<'", "'>'", "great_eq_tok", "less_eq_tok", "not_eq_tok", "'+'",
  "'-'", "or_tok", "unary_minus_tok", "unary_plus_tok", "'*'", "'/'",
  "and_tok", "mod_tok", "div_tok", "not_tok", "';'", "'('", "')'", "','",
  "'^'", "'['", "']'", "':'", "'.'", "$accept", "PROGRAM", "$@1", "$@2",
  "DEFS", "DEF", "PROGRAM_HEAD", "PROGRAM_FILE_PART", "PROGRAM_FILE_LIST",
  "PROGRAM_FILE", "BLOCK", "$@3", "$@4", "$@5", "LABEL_DEC_PART", "$@6",
  "LABEL_LIST", "LABEL", "CONST_DEC_PART", "CONST_DEC_LIST", "CONST_DEC",
  "$@7", "$@8", "$@9", "$@10", "CONSTANT", "CONSTANT_EXPRESS", "$@11",
  "$@12", "$@13", "$@14", "$@15", "$@16", "$@17", "$@18", "$@19", "$@20",
  "$@21", "$@22", "$@23", "$@24", "CONST_FACTOR", "$@25", "STRING",
  "CONSTANT_ID", "TYPE_DEC_PART", "TYPE_DEF_LIST", "TYPE_DEF", "$@26",
  "$@27", "$@28", "TYPE", "SIMPLE_TYPE", "SUBRANGE_TYPE", "POSSIBLE_PLUS",
  "SUBRANGE_CONSTANT", "TYPE_ID", "STRUCTURED_TYPE", "POINTER_TYPE",
  "ARRAY_TYPE", "INDEX_TYPE", "COMPONENT_TYPE", "RECORD_TYPE", "$@29",
  "FIELD_LIST", "RECORD_SECTION", "$@30", "$@31", "FIELD_ID_LIST",
  "FIELD_ID", "FILE_TYPE", "$@32", "VAR_DEC_PART", "VAR_DEC_LIST",
  "VAR_DEC", "$@33", "$@34", "VAR_ID_DEC_LIST", "VAR_ID", "BODY", "$@35",
  "P_F_DEC_PART", "P_F_DEC", "PROCEDURE_DEC", "PROCEDURE_TOK", "$@36",
  "PROCEDURE_HEAD", "$@37", "$@38", "PARAM", "$@39", "FORM_PAR_SEC_L",
  "FORM_PAR_SEC1", "$@40", "FORM_PAR_SEC", "$@41", "$@42", "DECLARED_PROC",
  "FUNCTION_DEC", "FUNCTION_HEAD", "$@43", "$@44", "$@45", "$@46", "$@47",
  "$@48", "DECLARED_FUN", "RESULT_TYPE", "STAT_PART", "COMPOUND_STAT",
  "$@49", "STAT_LIST", "STATEMENT", "S_LABEL", "UNLAB_STAT", "SIMPLE_STAT",
  "ASSIGN_STAT", "$@50", "$@51", "VARIABLE", "@52", "FUNC_ID_AS",
  "VAR_DESIG_LIST", "VAR_DESIG", "$@53", "VAR_DESIG1", "$@54", "EXPRESS",
  "$@55", "$@56", "$@57", "$@58", "$@59", "$@60", "$@61", "$@62", "$@63",
  "$@64", "$@65", "$@66", "$@67", "$@68", "UNARY_OP", "FACTOR", "$@69",
  "$@70", "PARAM_LIST", "$@71", "ACTUAL_PARAM_L", "$@72", "ACTUAL_PARAM",
  "WIDTH_FIELD", "PROC_STAT", "$@73", "GO_TO_STAT", "EMPTY_STAT",
  "STRUCT_STAT", "CONDIT_STAT", "IF_STATEMENT", "$@74",
  "IF_THEN_ELSE_STAT", "$@75", "THEN_ELSE_STAT", "$@76", "$@77", "$@78",
  "ELSE_STAT", "$@79", "CASE_STATEMENT", "$@80", "$@81", "CASE_EL_LIST",
  "CASE_ELEMENT", "CASE_LAB_LIST", "CASE_LAB", "END_CASE", "REPETIT_STAT",
  "WHILE_STATEMENT", "$@82", "$@83", "REP_STATEMENT", "$@84", "$@85",
  "FOR_STATEMENT", "$@86", "$@87", "$@88", "CONTROL_VAR", "FOR_LIST",
  "$@89", "$@90", "$@91", "$@92", YY_NULL
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
      61,    60,    62,   305,   306,   307,    43,    45,   308,   309,
     310,    42,    47,   311,   312,   313,   314,    59,    40,    41,
      44,    94,    91,    93,    58,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,    76,    78,    79,    77,    80,    80,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    82,    83,    83,    84,
      84,    85,    85,    87,    88,    89,    86,    90,    91,    90,
      92,    92,    93,    94,    94,    95,    95,    97,    98,    99,
     100,    96,   101,   101,   101,   101,   102,   103,   102,   104,
     102,   105,   102,   106,   102,   107,   102,   108,   102,   109,
     102,   110,   102,   111,   102,   112,   102,   113,   102,   114,
     102,   115,   102,   116,   102,   102,   118,   117,   117,   119,
     119,   120,   121,   121,   122,   122,   124,   125,   126,   123,
     127,   127,   128,   128,   129,   130,   130,   131,   131,   131,
     131,   132,   133,   133,   133,   133,   134,   135,   135,   136,
     136,   137,   139,   138,   140,   140,   142,   143,   141,   141,
     144,   144,   145,   145,   147,   146,   148,   148,   149,   149,
     151,   152,   150,   153,   153,   154,   154,   154,   155,   156,
     155,   157,   157,   158,   158,   159,   160,   161,   160,   163,
     162,   164,   162,   165,   166,   165,   167,   167,   169,   168,
     171,   170,   172,   170,   173,   173,   174,   176,   177,   178,
     175,   179,   180,   181,   175,   182,   182,   183,   184,   186,
     185,   187,   187,   188,   188,   189,   190,   190,   191,   191,
     191,   191,   191,   193,   192,   194,   192,   196,   195,   195,
     197,   197,   198,   198,   200,   199,   199,   199,   199,   201,
     202,   201,   203,   204,   203,   205,   203,   206,   203,   207,
     203,   208,   203,   209,   203,   210,   203,   211,   203,   212,
     203,   213,   203,   214,   203,   215,   203,   216,   203,   217,
     203,   203,   218,   218,   218,   220,   219,   219,   219,   219,
     221,   219,   223,   222,   224,   225,   224,   226,   226,   227,
     227,   228,   228,   229,   228,   230,   231,   232,   232,   232,
     233,   233,   235,   234,   237,   236,   239,   238,   240,   241,
     238,   242,   243,   242,   245,   246,   244,   247,   247,   248,
     249,   249,   250,   250,   251,   251,   252,   252,   252,   254,
     255,   253,   257,   258,   256,   260,   261,   262,   259,   263,
     265,   266,   264,   267,   268,   264
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
       5,     1,     2,     2,     2,     2,     1,     0,     3,     0,
       5,     0,     5,     0,     0,     4,     1,     3,     0,     4,
       0,     2,     0,     3,     1,     1,     2,     0,     0,     0,
       9,     0,     0,     0,     9,     1,     1,     1,     3,     0,
       4,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     4,     0,     4,     0,     3,     1,
       1,     1,     1,     2,     0,     4,     2,     2,     2,     1,
       0,     4,     2,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     0,     4,     0,     4,     0,     4,     0,     4,     0,
       4,     1,     1,     1,     1,     0,     4,     1,     1,     1,
       0,     3,     0,     4,     1,     0,     4,     2,     1,     2,
       0,     1,     1,     0,     3,     2,     0,     1,     1,     1,
       1,     1,     0,     3,     0,     3,     0,     4,     0,     0,
       6,     0,     0,     3,     0,     0,     7,     1,     3,     3,
       1,     3,     1,     1,     1,     2,     1,     1,     1,     0,
       0,     6,     0,     0,     6,     0,     0,     0,     9,     1,
       0,     0,     5,     0,     0,     5
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
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
     136,   137,     0,   133,     0,   146,   147,   138,   141,     0,
       0,    23,     0,    23,     0,    88,     0,   131,   167,   175,
     176,   171,   164,   165,   151,     0,   139,     4,   142,   143,
     149,   145,    27,   144,   166,    43,    42,    79,    80,    81,
     243,   242,   244,    76,    78,    40,    75,    44,    45,     0,
      95,   134,    95,   153,   153,   153,   148,   266,   153,    33,
       0,    55,    61,    63,    67,    65,    57,    47,    49,    71,
      51,    73,    69,    59,    53,     0,    46,     0,     0,   112,
     101,     0,     0,    90,    92,    93,    91,   105,   102,   103,
     104,     0,   154,     0,     0,     0,   179,   284,   305,     0,
     272,   302,   299,   185,   262,   199,   261,   263,   200,   201,
     192,   267,     0,   181,     0,   183,   186,   188,     0,     0,
     189,   190,   191,   187,   268,   270,   271,   269,   296,   297,
     298,     0,    82,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    41,    95,
     124,   116,   106,    89,   132,   160,   168,   172,   152,   266,
       0,     0,   265,     0,   266,     0,     0,     0,     0,   266,
     266,   193,   195,   150,    24,    77,    56,    62,    64,    68,
      66,    58,    48,    50,    72,    52,    74,    70,    60,    54,
     110,   109,     0,    95,     0,   114,     0,   162,     0,   156,
     158,    95,    95,     0,   249,   250,   245,   248,   247,     0,
       0,   241,   309,     0,   274,   273,     0,   300,   204,     0,
     198,   202,   252,   264,   140,   182,   184,     0,     0,   126,
      95,     0,   125,   113,   116,   122,   123,     0,   120,   158,
     160,   155,   161,     0,   177,   169,   173,   180,     0,     0,
     285,   221,   227,   229,   233,   231,   223,   213,   215,   237,
     217,   239,   235,   225,   219,   212,   306,     0,   303,     0,
       0,   207,   208,   206,   203,     0,   194,   196,    25,     0,
      95,   115,     0,   117,   163,   157,     0,     0,     0,   251,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   276,   275,     0,
     266,     0,   258,   260,     0,   254,     0,     0,   111,   107,
     121,    95,     0,   170,   174,   246,   293,   292,     0,   287,
       0,   290,   222,   228,   230,   234,   232,   224,   214,   216,
     238,   218,   240,   236,   226,   220,   310,     0,   278,   266,
     304,   301,   210,   209,   205,     0,   257,   253,   255,   266,
      26,    95,   118,   159,   294,     0,   286,     0,   266,     0,
       0,   307,     0,   281,     0,   259,     0,     0,   108,   295,
     288,   291,   289,   311,   314,   266,   279,   282,   277,     0,
     256,   178,     0,     0,   308,   281,   266,   211,   312,   315,
     280,   283
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    15,    85,     2,     6,     7,    17,    28,    29,
     121,   122,   319,   406,    25,    40,    55,    56,    42,    57,
      58,    59,    79,   104,   165,   297,   135,   230,   231,   233,
     237,   224,   229,   236,   225,   226,   228,   227,   235,   232,
     234,   136,   150,   137,   138,    61,    72,    73,    74,    87,
     140,   334,   173,   174,    52,    53,   175,   176,   177,   178,
     282,   409,   179,   241,   284,   285,   286,   411,   327,   328,
     180,   283,    76,    82,    83,    84,   142,    92,    93,   117,
     147,    97,    98,    99,   100,   115,   101,   148,   145,   183,
     245,   288,   332,   333,   289,   290,   329,   114,   102,   103,
     143,   291,   377,   144,   292,   378,   111,   335,   450,   201,
     249,   202,   203,   204,   205,   206,   207,   317,   318,   298,
     256,   209,   310,   311,   360,   444,   464,   304,   388,   389,
     391,   395,   382,   387,   394,   383,   384,   386,   385,   393,
     390,   392,   300,   301,   339,   338,   313,   365,   404,   466,
     405,   446,   210,   257,   211,   212,   213,   214,   215,   253,
     305,   357,   398,   439,   462,   485,   478,   486,   216,   250,
     381,   418,   419,   420,   421,   456,   217,   218,   255,   359,
     219,   254,   399,   220,   251,   396,   475,   303,   437,   459,
     482,   460,   483
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -256
static const yytype_int16 yypact[] =
{
    -256,    32,     4,  -256,    10,     9,  -256,  -256,    43,    14,
      37,    42,    83,   109,   125,   183,    30,   144,   148,    38,
     165,   -24,   167,   178,  -256,   243,  -256,  -256,   169,  -256,
    -256,  -256,  -256,   194,  -256,   195,    -8,  -256,  -256,  -256,
     221,  -256,   246,  -256,    30,   206,   207,  -256,  -256,  -256,
    -256,   208,   240,   250,  -256,    13,  -256,   257,  -256,   258,
    -256,   269,  -256,  -256,  -256,  -256,  -256,    -8,  -256,   221,
    -256,  -256,   260,  -256,   263,  -256,  -256,  -256,  -256,   251,
    -256,  -256,    64,  -256,    72,    71,  -256,   253,  -256,  -256,
    -256,  -256,   -50,  -256,    86,   202,  -256,    49,  -256,   233,
     268,  -256,   239,  -256,   317,  -256,    72,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   291,  -256,  -256,  -256,  -256,
    -256,  -256,   183,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   449,  -256,  -256,  -256,   317,
       5,  -256,     5,   242,   242,   242,  -256,   293,   242,   243,
     317,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   248,  -256,   241,   299,  -256,
    -256,   274,   252,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,   254,  -256,   244,   249,   255,  -256,  -256,  -256,   294,
    -256,  -256,  -256,  -256,  -256,    19,  -256,  -256,  -256,  -256,
    -256,  -256,    -4,  -256,   264,  -256,  -256,  -256,   292,   303,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,   273,   246,   409,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,  -256,    -6,
    -256,     1,  -256,  -256,  -256,   301,  -256,  -256,  -256,   293,
     176,   304,  -256,   176,   293,   176,    53,   276,   278,   293,
     331,  -256,  -256,  -256,  -256,  -256,   465,   465,   465,   465,
     465,   465,   131,   131,   131,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,   127,     5,     2,  -256,    25,  -256,   -29,  -256,
    -256,     5,     5,     3,  -256,  -256,  -256,  -256,  -256,   226,
     176,  -256,  -256,   320,   481,  -256,   -18,   481,  -256,   180,
      53,  -256,  -256,  -256,  -256,  -256,  -256,   176,   176,   269,
      -6,   334,  -256,  -256,     1,  -256,  -256,   -15,  -256,  -256,
     301,  -256,  -256,    72,  -256,  -256,  -256,  -256,   276,   176,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,   333,  -256,   349,
     176,  -256,  -256,  -256,  -256,    55,   481,   481,  -256,   284,
       5,  -256,    25,  -256,  -256,  -256,   128,   295,   296,  -256,
     429,   198,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   345,  -256,   176,
     293,   361,  -256,   336,   181,  -256,   357,   347,  -256,  -256,
    -256,     5,   322,  -256,  -256,  -256,  -256,  -256,     7,  -256,
     140,  -256,   497,   497,   497,   497,   497,   497,   158,   158,
     158,  -256,  -256,  -256,  -256,  -256,   204,   359,  -256,   293,
     481,  -256,  -256,  -256,  -256,   342,  -256,  -256,  -256,   293,
    -256,     5,  -256,  -256,  -256,    29,  -256,   198,   331,   351,
     367,  -256,   176,   369,   176,  -256,    55,     8,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   293,  -256,  -256,  -256,   385,
    -256,  -256,   176,   176,  -256,   369,   293,  -256,   481,   481,
    -256,  -256
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,   335,
     279,  -256,  -256,  -256,   259,  -256,  -256,   315,   247,  -256,
     338,  -256,  -256,  -256,  -256,   -94,   -46,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   182,  -256,   330,  -256,  -256,
    -256,  -139,  -256,   -34,  -256,   339,  -256,  -256,  -256,  -256,
      85,   -48,  -256,  -256,  -256,    84,  -256,  -256,  -256,    35,
    -256,  -256,    90,  -256,   346,  -256,  -256,    87,   321,  -256,
    -256,  -256,   324,  -256,  -256,  -256,  -256,  -256,  -256,   -28,
    -256,  -256,   100,  -256,   102,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,   138,  -256,  -256,
    -256,  -245,  -238,  -256,  -255,  -256,  -256,  -256,  -256,  -147,
    -256,  -256,  -256,   123,  -256,  -256,  -256,  -236,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,   -60,  -256,  -256,  -256,   106,  -256,  -256,  -256,
     -21,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
     -11,  -256,  -256,  -256,  -256,  -256,   -33,  -256,  -256,  -256,
    -256,  -256,    -2,  -256,    -3,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -314
static const yytype_int16 yytable[] =
{
     208,   172,    51,   181,   293,   316,   258,   358,   167,   306,
     134,  -119,   323,   337,   299,     9,   168,   454,   481,   307,
     106,   315,    10,     4,   107,   169,    36,    11,    47,    48,
      47,    48,     3,    12,    49,    13,    49,   280,   330,   469,
     331,    47,    48,    37,   139,   134,     8,    49,   170,   259,
      18,     5,    50,   116,    50,   372,   134,    14,   416,   373,
     417,   325,    94,   259,   355,    50,    26,    95,  -119,   324,
     259,   326,    27,    19,   455,   259,   171,    96,    20,   139,
      68,   366,   367,    69,    94,   125,   126,   127,   128,    95,
     139,  -197,   195,   166,  -197,   294,   295,   129,   402,    96,
    -130,  -130,   208,   380,   223,    32,    33,   208,    89,    90,
    -130,    16,   208,   208,   130,   131,   184,   185,    91,    21,
     221,   132,   108,   296,   401,   308,   109,   110,   309,   403,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   322,    22,   422,   423,   424,   425,
     426,   427,   428,   429,   430,   431,   432,   433,   434,   435,
     436,    23,   441,   440,   139,   139,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   160,   161,   162,   163,   164,   320,   106,    24,
     321,   463,   412,   472,   467,   281,   125,   126,   127,   128,
     457,    30,  -313,   195,   458,    31,   294,   295,   129,   350,
     351,   352,   353,   354,   361,   362,   363,   416,   479,   417,
     403,   408,    34,    35,    38,   130,   131,   484,    43,    44,
     112,   113,   132,   340,   296,    39,   488,   489,   491,    41,
     447,   448,    54,   208,   341,   342,   343,   344,   345,   346,
     347,   348,   349,    45,    46,   350,   351,   352,   353,   354,
      60,    66,   452,    63,    64,    65,   341,   342,   343,   344,
     345,   346,   347,   348,   349,    67,   281,   350,   351,   352,
     353,   354,   208,   -37,    71,    75,   -86,   186,   187,    81,
     119,    86,   208,   105,   120,   188,   123,   189,   190,   146,
     182,   208,   408,   239,   191,   238,   240,   242,   246,   243,
     192,   244,   248,   247,   193,   252,   261,   287,   208,   194,
     195,   196,   197,   198,   199,   186,   187,   262,   260,   208,
     263,   302,   200,   188,   312,   189,   190,   125,   126,   127,
     128,   370,   191,   314,   356,   397,   400,   407,   192,   129,
     438,   449,   413,   414,   451,   453,   461,   194,   195,   196,
     197,   198,   199,   465,   473,   474,   130,   131,   477,    62,
     200,   149,   124,   132,    78,   133,   341,   342,   343,   344,
     345,   346,   347,   348,   349,    70,   222,   350,   351,   352,
     353,   354,    80,   468,   264,   369,    77,   410,   371,   368,
     445,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     376,   118,   350,   351,   352,   353,   354,   141,    88,   374,
     336,   442,   375,   364,   443,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   379,   480,   350,   351,   352,   353,
     354,   476,   490,   470,   471,     0,     0,     0,   487,   151,
     152,   153,   154,   155,   156,   157,   158,   159,     0,     0,
     160,   161,   162,   163,   164,     0,     0,     0,   265,   341,
     342,   343,   344,   345,   346,   347,   348,   349,     0,     0,
     350,   351,   352,   353,   354,     0,     0,     0,   415,   151,
     152,   153,   154,   155,   156,   157,   158,   159,     0,     0,
     160,   161,   162,   163,   164,  -314,  -314,  -314,  -314,  -314,
    -314,   157,   158,   159,     0,     0,   160,   161,   162,   163,
     164,   341,   342,   343,   344,   345,   346,   347,   348,   349,
       0,     0,   350,   351,   352,   353,   354,  -314,  -314,  -314,
    -314,  -314,  -314,   347,   348,   349,     0,     0,   350,   351,
     352,   353,   354
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-256))

#define yytable_value_is_error(yytable_value) \
  ((yytable_value) == (-314))

static const yytype_int16 yycheck[] =
{
     147,   140,    36,   142,   249,   260,    10,    25,     3,   254,
     104,    10,    10,    10,   250,     6,    11,    10,    10,   255,
      70,   259,    13,    19,    74,    20,    50,    18,    36,    37,
      36,    37,     0,    24,    42,    26,    42,    43,    67,    10,
      69,    36,    37,    67,   104,   139,    36,    42,    43,    67,
      36,    47,    60,     4,    60,    70,   150,    48,    29,    74,
      31,    36,    13,    67,   300,    60,    36,    18,    67,    67,
      67,    46,    42,    36,    67,    67,    71,    28,    36,   139,
      67,   317,   318,    70,    13,    30,    31,    32,    33,    18,
     150,    72,    37,   139,    75,    40,    41,    42,    43,    28,
      36,    37,   249,   339,   150,    67,    68,   254,    36,    37,
      46,    68,   259,   260,    59,    60,   144,   145,    46,    36,
     148,    66,    36,    68,   360,    72,    40,    41,    75,   365,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   283,    36,   382,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,    36,   400,   399,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    61,    62,    63,    64,    65,    70,    70,    16,
      73,   439,    74,   458,   449,   239,    30,    31,    32,    33,
      70,    67,     8,    37,    74,    67,    40,    41,    42,    61,
      62,    63,    64,    65,    44,    45,    46,    29,   464,    31,
     466,   370,    67,    68,    67,    59,    60,   475,    69,    70,
      38,    39,    66,    17,    68,    67,   482,   483,   486,     6,
      69,    70,    31,   400,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    69,    69,    61,    62,    63,    64,    65,
      24,    31,   411,    67,    67,    67,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    35,   320,    61,    62,    63,
      64,    65,   439,    36,    36,    26,    36,     4,     5,    36,
      67,    50,   449,    50,    36,    12,    67,    14,    15,    18,
      68,   458,   451,    72,    21,    67,    17,    43,    74,    67,
      27,    67,    67,    74,    31,    31,    34,    26,   475,    36,
      37,    38,    39,    40,    41,     4,     5,    34,    74,   486,
      67,    37,    49,    12,    68,    14,    15,    30,    31,    32,
      33,    17,    21,    75,    34,    22,     7,    73,    27,    42,
      15,     4,    67,    67,    17,    43,     7,    36,    37,    38,
      39,    40,    41,    31,    23,     8,    59,    60,     9,    44,
      49,   122,   103,    66,    69,    68,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    57,   149,    61,    62,    63,
      64,    65,    72,   451,   222,   320,    67,   372,   324,   319,
      74,    50,    51,    52,    53,    54,    55,    56,    57,    58,
     333,    97,    61,    62,    63,    64,    65,   106,    82,   329,
     292,    70,   330,   310,    73,    50,    51,    52,    53,    54,
      55,    56,    57,    58,   338,   466,    61,    62,    63,    64,
      65,   462,   485,   455,   457,    -1,    -1,    -1,    73,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      61,    62,    63,    64,    65,    -1,    -1,    -1,    69,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      61,    62,    63,    64,    65,    -1,    -1,    -1,    69,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      61,    62,    63,    64,    65,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    -1,    61,    62,    63,    64,
      65,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    -1,    61,    62,    63,    64,    65,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    -1,    61,    62,
      63,    64,    65
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    77,    80,     0,    19,    47,    81,    82,    36,     6,
      13,    18,    24,    26,    48,    78,    68,    83,    36,    36,
      36,    36,    36,    36,    16,    90,    36,    42,    84,    85,
      67,    67,    67,    68,    67,    68,    50,    67,    67,    67,
      91,     6,    94,    69,    70,    69,    69,    36,    37,    42,
      60,   129,   130,   131,    31,    92,    93,    95,    96,    97,
      24,   121,    85,    67,    67,    67,    31,    35,    67,    70,
      96,    36,   122,   123,   124,    26,   148,   131,    93,    98,
     123,    36,   149,   150,   151,    79,    50,   125,   150,    36,
      37,    46,   153,   154,    13,    18,    28,   157,   158,   159,
     160,   162,   174,   175,    99,    50,    70,    74,    36,    40,
      41,   182,    38,    39,   173,   161,     4,   155,   158,    67,
      36,    86,    87,    67,    86,    30,    31,    32,    33,    42,
      59,    60,    66,    68,   101,   102,   117,   119,   120,   218,
     126,   154,   152,   176,   179,   164,    18,   156,   163,    90,
     118,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      61,    62,    63,    64,    65,   100,   102,     3,    11,    20,
      43,    71,   127,   128,   129,   132,   133,   134,   135,   138,
     146,   127,    68,   165,   165,   165,     4,     5,    12,    14,
      15,    21,    27,    31,    36,    37,    38,    39,    40,    41,
      49,   185,   187,   188,   189,   190,   191,   192,   195,   197,
     228,   230,   231,   232,   233,   234,   244,   252,   253,   256,
     259,   165,    94,   102,   107,   110,   111,   113,   112,   108,
     103,   104,   115,   105,   116,   114,   109,   106,    67,    72,
      17,   139,    43,    67,    67,   166,    74,    74,    67,   186,
     245,   260,    31,   235,   257,   254,   196,   229,    10,    67,
      74,    34,    34,    67,   121,    69,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
      43,   129,   136,   147,   140,   141,   142,    26,   167,   170,
     171,   177,   180,   187,    40,    41,    68,   101,   195,   203,
     218,   219,    37,   263,   203,   236,   187,   203,    72,    75,
     198,   199,    68,   222,    75,   188,   190,   193,   194,    88,
      70,    73,   127,    10,    67,    36,    46,   144,   145,   172,
      67,    69,   168,   169,   127,   183,   183,    10,   221,   220,
      17,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      61,    62,    63,    64,    65,   203,    34,   237,    25,   255,
     200,    44,    45,    46,   199,   223,   203,   203,   148,   136,
      17,   141,    70,    74,   168,   170,   153,   178,   181,   222,
     203,   246,   208,   211,   212,   214,   213,   209,   204,   205,
     216,   206,   217,   215,   210,   207,   261,    22,   238,   258,
       7,   203,    43,   203,   224,   226,    89,    73,   127,   137,
     145,   143,    74,    67,    67,    69,    29,    31,   247,   248,
     249,   250,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   264,    15,   239,
     203,   188,    70,    73,   201,    74,   227,    69,    70,     4,
     184,    17,   127,    43,    10,    67,   251,    70,    74,   265,
     267,     7,   240,   188,   202,    31,   225,   187,   137,    10,
     248,   250,   190,    23,     8,   262,   236,     9,   242,   203,
     226,    10,   266,   268,   188,   241,   243,    73,   203,   203,
     242,   188
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

/* Line 1810 of yacc.c  */
#line 66 "web2c-parser.y"
    {
	    printf ("#define %s\n", uppercasify (program_name));
            block_level++;
	    printf ("#include \"%s\"\n", std_header);
	  }
    break;

  case 3:

/* Line 1810 of yacc.c  */
#line 73 "web2c-parser.y"
    { printf ("\n#include \"%s\"\n", coerce_name); }
    break;

  case 4:

/* Line 1810 of yacc.c  */
#line 76 "web2c-parser.y"
    { YYACCEPT; }
    break;

  case 7:

/* Line 1810 of yacc.c  */
#line 86 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = field_id_tok;
	    }
    break;

  case 8:

/* Line 1810 of yacc.c  */
#line 91 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = fun_id_tok;
	    }
    break;

  case 9:

/* Line 1810 of yacc.c  */
#line 96 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = const_id_tok;
	    }
    break;

  case 10:

/* Line 1810 of yacc.c  */
#line 101 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = fun_param_tok;
	    }
    break;

  case 11:

/* Line 1810 of yacc.c  */
#line 106 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = proc_id_tok;
	    }
    break;

  case 12:

/* Line 1810 of yacc.c  */
#line 111 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = proc_param_tok;
	    }
    break;

  case 13:

/* Line 1810 of yacc.c  */
#line 116 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = type_id_tok;
	    }
    break;

  case 14:

/* Line 1810 of yacc.c  */
#line 121 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 130 "web2c-parser.y"
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = var_id_tok;
	    }
    break;

  case 23:

/* Line 1810 of yacc.c  */
#line 157 "web2c-parser.y"
    {	if (block_level > 0) my_output("{\n ");
                indent++; block_level++;
              }
    break;

  case 24:

/* Line 1810 of yacc.c  */
#line 162 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 176 "web2c-parser.y"
    { doing_statements = true; }
    break;

  case 26:

/* Line 1810 of yacc.c  */
#line 178 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 208 "web2c-parser.y"
    { my_output("/*"); }
    break;

  case 29:

/* Line 1810 of yacc.c  */
#line 210 "web2c-parser.y"
    { my_output("*/"); }
    break;

  case 32:

/* Line 1810 of yacc.c  */
#line 218 "web2c-parser.y"
    { my_output(temp); }
    break;

  case 34:

/* Line 1810 of yacc.c  */
#line 224 "web2c-parser.y"
    { new_line(); }
    break;

  case 37:

/* Line 1810 of yacc.c  */
#line 233 "web2c-parser.y"
    { /* `#define' must be in column 1 for pcc. */
            unsigned save = indent;
	    new_line ();
	    indent = 0;
	    my_output ("#define");
	    indent = save;
	  }
    break;

  case 38:

/* Line 1810 of yacc.c  */
#line 241 "web2c-parser.y"
    { ii = add_to_table (last_id);
	    sym_table[ii].typ = const_id_tok;
            my_output (last_id);
          }
    break;

  case 39:

/* Line 1810 of yacc.c  */
#line 245 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 40:

/* Line 1810 of yacc.c  */
#line 246 "web2c-parser.y"
    { my_output (")"); }
    break;

  case 41:

/* Line 1810 of yacc.c  */
#line 247 "web2c-parser.y"
    { sym_table[ii].val = last_i_num; new_line(); }
    break;

  case 42:

/* Line 1810 of yacc.c  */
#line 252 "web2c-parser.y"
    {
             sscanf (temp, "%ld", &last_i_num);
             if (my_labs ((long) last_i_num) > 32767)
               strcat (temp, "L");
             my_output (temp);
             (yyval) = ex_32;
           }
    break;

  case 43:

/* Line 1810 of yacc.c  */
#line 259 "web2c-parser.y"
    { my_output(temp); (yyval) = ex_real; }
    break;

  case 44:

/* Line 1810 of yacc.c  */
#line 260 "web2c-parser.y"
    { (yyval) = 0; }
    break;

  case 45:

/* Line 1810 of yacc.c  */
#line 261 "web2c-parser.y"
    { (yyval) = ex_32; }
    break;

  case 46:

/* Line 1810 of yacc.c  */
#line 266 "web2c-parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); }
    break;

  case 47:

/* Line 1810 of yacc.c  */
#line 267 "web2c-parser.y"
    { my_output ("+"); }
    break;

  case 48:

/* Line 1810 of yacc.c  */
#line 268 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 49:

/* Line 1810 of yacc.c  */
#line 269 "web2c-parser.y"
    { my_output ("-"); }
    break;

  case 50:

/* Line 1810 of yacc.c  */
#line 270 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 51:

/* Line 1810 of yacc.c  */
#line 271 "web2c-parser.y"
    { my_output ("*"); }
    break;

  case 52:

/* Line 1810 of yacc.c  */
#line 272 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 53:

/* Line 1810 of yacc.c  */
#line 273 "web2c-parser.y"
    { my_output ("/"); }
    break;

  case 54:

/* Line 1810 of yacc.c  */
#line 274 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 55:

/* Line 1810 of yacc.c  */
#line 275 "web2c-parser.y"
    { my_output ("=="); }
    break;

  case 56:

/* Line 1810 of yacc.c  */
#line 276 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 57:

/* Line 1810 of yacc.c  */
#line 277 "web2c-parser.y"
    { my_output ("!="); }
    break;

  case 58:

/* Line 1810 of yacc.c  */
#line 278 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 59:

/* Line 1810 of yacc.c  */
#line 279 "web2c-parser.y"
    { my_output ("%"); }
    break;

  case 60:

/* Line 1810 of yacc.c  */
#line 280 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 61:

/* Line 1810 of yacc.c  */
#line 281 "web2c-parser.y"
    { my_output ("<"); }
    break;

  case 62:

/* Line 1810 of yacc.c  */
#line 282 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 63:

/* Line 1810 of yacc.c  */
#line 283 "web2c-parser.y"
    { my_output (">"); }
    break;

  case 64:

/* Line 1810 of yacc.c  */
#line 284 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 65:

/* Line 1810 of yacc.c  */
#line 285 "web2c-parser.y"
    { my_output ("<="); }
    break;

  case 66:

/* Line 1810 of yacc.c  */
#line 286 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 67:

/* Line 1810 of yacc.c  */
#line 287 "web2c-parser.y"
    { my_output (">="); }
    break;

  case 68:

/* Line 1810 of yacc.c  */
#line 288 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 69:

/* Line 1810 of yacc.c  */
#line 289 "web2c-parser.y"
    { my_output ("&&"); }
    break;

  case 70:

/* Line 1810 of yacc.c  */
#line 290 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 71:

/* Line 1810 of yacc.c  */
#line 291 "web2c-parser.y"
    { my_output ("||"); }
    break;

  case 72:

/* Line 1810 of yacc.c  */
#line 292 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 73:

/* Line 1810 of yacc.c  */
#line 293 "web2c-parser.y"
    { my_output ("/ ((double)"); }
    break;

  case 74:

/* Line 1810 of yacc.c  */
#line 294 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); my_output (")"); }
    break;

  case 75:

/* Line 1810 of yacc.c  */
#line 295 "web2c-parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 76:

/* Line 1810 of yacc.c  */
#line 300 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 77:

/* Line 1810 of yacc.c  */
#line 302 "web2c-parser.y"
    { my_output (")"); (yyval) = (yyvsp[(1) - (4)]); }
    break;

  case 79:

/* Line 1810 of yacc.c  */
#line 308 "web2c-parser.y"
    {
              char s[132];
              get_string_literal(s);
              my_output (s);
            }
    break;

  case 80:

/* Line 1810 of yacc.c  */
#line 314 "web2c-parser.y"
    {
              char s[5];
              get_single_char(s);
              my_output (s);
            }
    break;

  case 81:

/* Line 1810 of yacc.c  */
#line 322 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 86:

/* Line 1810 of yacc.c  */
#line 334 "web2c-parser.y"
    { my_output ("typedef"); }
    break;

  case 87:

/* Line 1810 of yacc.c  */
#line 336 "web2c-parser.y"
    {
            ii = add_to_table(last_id);
            sym_table[ii].typ = type_id_tok;
            strcpy(safe_string, last_id);
            last_type = ii;
          }
    break;

  case 88:

/* Line 1810 of yacc.c  */
#line 343 "web2c-parser.y"
    {
            array_bounds[0] = 0;
            array_offset[0] = 0;
          }
    break;

  case 89:

/* Line 1810 of yacc.c  */
#line 348 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 366 "web2c-parser.y"
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
                 can represent it.  We avoid using char as such variables
                 are frequently used as array indices.  We avoid using
                 schar and unsigned short where possible, since they are
                 treated differently by different compilers
                 (see also config.h).  */
              if (lower_sym == -1 && upper_sym == -1) {
                if (0 <= lower_bound && upper_bound <= UCHAR_MAX)
                  my_output ("unsigned char");
                else if (SCHAR_MIN <= lower_bound && upper_bound <= SCHAR_MAX)
                  my_output ("schar");
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

/* Line 1810 of yacc.c  */
#line 412 "web2c-parser.y"
    {
              lower_bound = upper_bound;
              lower_sym = upper_sym;
              sscanf (temp, "%ld", &upper_bound);
              upper_sym = -1; /* no sym table entry */
            }
    break;

  case 98:

/* Line 1810 of yacc.c  */
#line 419 "web2c-parser.y"
    {
              lower_bound = upper_bound;
              lower_sym = upper_sym;
              upper_bound = sym_table[l_s].val;
              upper_sym = l_s;
            }
    break;

  case 99:

/* Line 1810 of yacc.c  */
#line 426 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 435 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 447 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 461 "web2c-parser.y"
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
    break;

  case 104:

/* Line 1810 of yacc.c  */
#line 466 "web2c-parser.y"
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
    break;

  case 105:

/* Line 1810 of yacc.c  */
#line 470 "web2c-parser.y"
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
    break;

  case 106:

/* Line 1810 of yacc.c  */
#line 477 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 497 "web2c-parser.y"
    { compute_array_bounds(); }
    break;

  case 110:

/* Line 1810 of yacc.c  */
#line 499 "web2c-parser.y"
    {
              lower_bound = sym_table[l_s].val;
              lower_sym = sym_table[l_s].val_sym;
              upper_bound = sym_table[l_s].upper;
              upper_sym = sym_table[l_s].upper_sym;
              compute_array_bounds();
            }
    break;

  case 112:

/* Line 1810 of yacc.c  */
#line 512 "web2c-parser.y"
    { my_output ("struct"); my_output ("{"); indent++; }
    break;

  case 113:

/* Line 1810 of yacc.c  */
#line 514 "web2c-parser.y"
    { indent--; my_output ("}"); semicolon(); }
    break;

  case 116:

/* Line 1810 of yacc.c  */
#line 522 "web2c-parser.y"
    { field_list[0] = 0; }
    break;

  case 117:

/* Line 1810 of yacc.c  */
#line 524 "web2c-parser.y"
    {
				  /*array_bounds[0] = 0;
				  array_offset[0] = 0;*/
				}
    break;

  case 118:

/* Line 1810 of yacc.c  */
#line 529 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 550 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 562 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 575 "web2c-parser.y"
    { my_output ("text /* of "); }
    break;

  case 125:

/* Line 1810 of yacc.c  */
#line 577 "web2c-parser.y"
    { my_output ("*/"); }
    break;

  case 130:

/* Line 1810 of yacc.c  */
#line 591 "web2c-parser.y"
    {
            var_list[0] = 0;
            array_bounds[0] = 0;
            array_offset[0] = 0;
            var_formals = false;
            ids_paramed = 0;
          }
    break;

  case 131:

/* Line 1810 of yacc.c  */
#line 599 "web2c-parser.y"
    {
            array_bounds[0] = 0;
            array_offset[0] = 0;
          }
    break;

  case 132:

/* Line 1810 of yacc.c  */
#line 604 "web2c-parser.y"
    { fixup_var_list(); }
    break;

  case 135:

/* Line 1810 of yacc.c  */
#line 612 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 626 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 640 "web2c-parser.y"
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

/* Line 1810 of yacc.c  */
#line 658 "web2c-parser.y"
    { my_output ("void mainbody( void ) {");
		  indent++;
		  new_line ();
		}
    break;

  case 140:

/* Line 1810 of yacc.c  */
#line 663 "web2c-parser.y"
    { indent--;
                  my_output ("}");
                  new_line ();
                }
    break;

  case 143:

/* Line 1810 of yacc.c  */
#line 675 "web2c-parser.y"
    { new_line(); remove_locals(); }
    break;

  case 144:

/* Line 1810 of yacc.c  */
#line 677 "web2c-parser.y"
    { new_line(); remove_locals(); }
    break;

  case 147:

/* Line 1810 of yacc.c  */
#line 686 "web2c-parser.y"
    { proc_is_noreturn = 1; }
    break;

  case 149:

/* Line 1810 of yacc.c  */
#line 692 "web2c-parser.y"
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

  case 150:

/* Line 1810 of yacc.c  */
#line 704 "web2c-parser.y"
    { strcpy(fn_return_type, "void");
	      do_proc_args();
	      gen_function_head(); }
    break;

  case 151:

/* Line 1810 of yacc.c  */
#line 708 "web2c-parser.y"
    { ii = l_s;
	      if (debug)
	        fprintf(stderr, "%3d Procedure %s\n", pf_count++, last_id);
	      strcpy(my_routine, last_id);
	      my_output ("void");
	      new_line ();
	    }
    break;

  case 152:

/* Line 1810 of yacc.c  */
#line 716 "web2c-parser.y"
    { strcpy(fn_return_type, "void");
	      do_proc_args();
	      gen_function_head();
            }
    break;

  case 153:

/* Line 1810 of yacc.c  */
#line 724 "web2c-parser.y"
    {
              strcpy (z_id, last_id);
	      mark ();
	      ids_paramed = 0;
	    }
    break;

  case 154:

/* Line 1810 of yacc.c  */
#line 730 "web2c-parser.y"
    { sprintf (z_id, "z%s", last_id);
	      ids_paramed = 0;
	      if (sym_table[ii].typ == proc_id_tok)
	        sym_table[ii].typ = proc_param_tok;
	      else if (sym_table[ii].typ == fun_id_tok)
	        sym_table[ii].typ = fun_param_tok;
	      mark();
	    }
    break;

  case 158:

/* Line 1810 of yacc.c  */
#line 746 "web2c-parser.y"
    { ids_typed = ids_paramed; }
    break;

  case 159:

/* Line 1810 of yacc.c  */
#line 748 "web2c-parser.y"
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

  case 160:

/* Line 1810 of yacc.c  */
#line 762 "web2c-parser.y"
    {var_formals = 0; }
    break;

  case 162:

/* Line 1810 of yacc.c  */
#line 763 "web2c-parser.y"
    {var_formals = 1; }
    break;

  case 167:

/* Line 1810 of yacc.c  */
#line 775 "web2c-parser.y"
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

  case 168:

/* Line 1810 of yacc.c  */
#line 786 "web2c-parser.y"
    {
              normal();
              array_bounds[0] = 0;
              array_offset[0] = 0;
            }
    break;

  case 169:

/* Line 1810 of yacc.c  */
#line 792 "web2c-parser.y"
    {
              get_result_type(fn_return_type);
              do_proc_args();
              gen_function_head();
            }
    break;

  case 171:

/* Line 1810 of yacc.c  */
#line 799 "web2c-parser.y"
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

  case 172:

/* Line 1810 of yacc.c  */
#line 809 "web2c-parser.y"
    { normal();
              array_bounds[0] = 0;
              array_offset[0] = 0;
            }
    break;

  case 173:

/* Line 1810 of yacc.c  */
#line 814 "web2c-parser.y"
    { get_result_type(fn_return_type);
              do_proc_args();
              gen_function_head();
            }
    break;

  case 179:

/* Line 1810 of yacc.c  */
#line 832 "web2c-parser.y"
    { my_output ("{"); indent++; new_line(); }
    break;

  case 180:

/* Line 1810 of yacc.c  */
#line 834 "web2c-parser.y"
    { indent--; my_output ("}"); new_line(); }
    break;

  case 185:

/* Line 1810 of yacc.c  */
#line 847 "web2c-parser.y"
    {if (!doreturn(temp)) {
				      sprintf(safe_string, "lab%s:", temp);
				    my_output (safe_string);
				 }
				}
    break;

  case 186:

/* Line 1810 of yacc.c  */
#line 855 "web2c-parser.y"
    { semicolon(); }
    break;

  case 187:

/* Line 1810 of yacc.c  */
#line 857 "web2c-parser.y"
    { semicolon(); }
    break;

  case 192:

/* Line 1810 of yacc.c  */
#line 865 "web2c-parser.y"
    { my_output ("break"); }
    break;

  case 193:

/* Line 1810 of yacc.c  */
#line 869 "web2c-parser.y"
    { my_output ("="); }
    break;

  case 195:

/* Line 1810 of yacc.c  */
#line 872 "web2c-parser.y"
    { my_output ("Result ="); }
    break;

  case 197:

/* Line 1810 of yacc.c  */
#line 877 "web2c-parser.y"
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

  case 199:

/* Line 1810 of yacc.c  */
#line 888 "web2c-parser.y"
    { if (sym_table[l_s].var_formal)
					putchar('*');
				  my_output (last_id); (yyval) = ex_32; }
    break;

  case 200:

/* Line 1810 of yacc.c  */
#line 894 "web2c-parser.y"
    { (yyval) = ex_32; }
    break;

  case 201:

/* Line 1810 of yacc.c  */
#line 896 "web2c-parser.y"
    { (yyval) = ex_32; }
    break;

  case 204:

/* Line 1810 of yacc.c  */
#line 904 "web2c-parser.y"
    { my_output ("["); }
    break;

  case 205:

/* Line 1810 of yacc.c  */
#line 906 "web2c-parser.y"
    { my_output ("]"); }
    break;

  case 206:

/* Line 1810 of yacc.c  */
#line 908 "web2c-parser.y"
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

  case 207:

/* Line 1810 of yacc.c  */
#line 926 "web2c-parser.y"
    { my_output (".hh.b0"); }
    break;

  case 208:

/* Line 1810 of yacc.c  */
#line 928 "web2c-parser.y"
    { my_output (".hh.b1"); }
    break;

  case 210:

/* Line 1810 of yacc.c  */
#line 933 "web2c-parser.y"
    { my_output ("]["); }
    break;

  case 212:

/* Line 1810 of yacc.c  */
#line 938 "web2c-parser.y"
    { (yyval) = (yyvsp[(2) - (2)]); }
    break;

  case 213:

/* Line 1810 of yacc.c  */
#line 939 "web2c-parser.y"
    { my_output ("+"); }
    break;

  case 214:

/* Line 1810 of yacc.c  */
#line 940 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 215:

/* Line 1810 of yacc.c  */
#line 941 "web2c-parser.y"
    { my_output ("-"); }
    break;

  case 216:

/* Line 1810 of yacc.c  */
#line 942 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 217:

/* Line 1810 of yacc.c  */
#line 943 "web2c-parser.y"
    { my_output ("*"); }
    break;

  case 218:

/* Line 1810 of yacc.c  */
#line 944 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 219:

/* Line 1810 of yacc.c  */
#line 945 "web2c-parser.y"
    { my_output ("/"); }
    break;

  case 220:

/* Line 1810 of yacc.c  */
#line 946 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 221:

/* Line 1810 of yacc.c  */
#line 947 "web2c-parser.y"
    { my_output ("=="); }
    break;

  case 222:

/* Line 1810 of yacc.c  */
#line 948 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 223:

/* Line 1810 of yacc.c  */
#line 949 "web2c-parser.y"
    { my_output ("!="); }
    break;

  case 224:

/* Line 1810 of yacc.c  */
#line 950 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 225:

/* Line 1810 of yacc.c  */
#line 951 "web2c-parser.y"
    { my_output ("%"); }
    break;

  case 226:

/* Line 1810 of yacc.c  */
#line 952 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 227:

/* Line 1810 of yacc.c  */
#line 953 "web2c-parser.y"
    { my_output ("<"); }
    break;

  case 228:

/* Line 1810 of yacc.c  */
#line 954 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 229:

/* Line 1810 of yacc.c  */
#line 955 "web2c-parser.y"
    { my_output (">"); }
    break;

  case 230:

/* Line 1810 of yacc.c  */
#line 956 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 231:

/* Line 1810 of yacc.c  */
#line 957 "web2c-parser.y"
    { my_output ("<="); }
    break;

  case 232:

/* Line 1810 of yacc.c  */
#line 958 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 233:

/* Line 1810 of yacc.c  */
#line 959 "web2c-parser.y"
    { my_output (">="); }
    break;

  case 234:

/* Line 1810 of yacc.c  */
#line 960 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 235:

/* Line 1810 of yacc.c  */
#line 961 "web2c-parser.y"
    { my_output ("&&"); }
    break;

  case 236:

/* Line 1810 of yacc.c  */
#line 962 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 237:

/* Line 1810 of yacc.c  */
#line 963 "web2c-parser.y"
    { my_output ("||"); }
    break;

  case 238:

/* Line 1810 of yacc.c  */
#line 964 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); }
    break;

  case 239:

/* Line 1810 of yacc.c  */
#line 966 "web2c-parser.y"
    { my_output ("/ ((double)"); }
    break;

  case 240:

/* Line 1810 of yacc.c  */
#line 968 "web2c-parser.y"
    { (yyval) = max ((yyvsp[(1) - (4)]), (yyvsp[(4) - (4)])); my_output (")"); }
    break;

  case 241:

/* Line 1810 of yacc.c  */
#line 970 "web2c-parser.y"
    { (yyval) = (yyvsp[(1) - (1)]); }
    break;

  case 243:

/* Line 1810 of yacc.c  */
#line 976 "web2c-parser.y"
    { my_output ("- (integer)"); }
    break;

  case 244:

/* Line 1810 of yacc.c  */
#line 978 "web2c-parser.y"
    { my_output ("!"); }
    break;

  case 245:

/* Line 1810 of yacc.c  */
#line 983 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 246:

/* Line 1810 of yacc.c  */
#line 985 "web2c-parser.y"
    { my_output (")"); (yyval) = (yyvsp[(1) - (4)]); }
    break;

  case 249:

/* Line 1810 of yacc.c  */
#line 989 "web2c-parser.y"
    { my_output (last_id); my_output ("()"); }
    break;

  case 250:

/* Line 1810 of yacc.c  */
#line 991 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 252:

/* Line 1810 of yacc.c  */
#line 996 "web2c-parser.y"
    { my_output ("("); }
    break;

  case 253:

/* Line 1810 of yacc.c  */
#line 997 "web2c-parser.y"
    { my_output (")"); }
    break;

  case 255:

/* Line 1810 of yacc.c  */
#line 1002 "web2c-parser.y"
    { my_output (","); }
    break;

  case 258:

/* Line 1810 of yacc.c  */
#line 1009 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 261:

/* Line 1810 of yacc.c  */
#line 1018 "web2c-parser.y"
    { my_output (last_id); my_output ("()"); }
    break;

  case 262:

/* Line 1810 of yacc.c  */
#line 1020 "web2c-parser.y"
    { my_output (last_id);
				  ii = add_to_table(last_id);
				  sym_table[ii].typ = proc_id_tok;
				  my_output ("()");
				}
    break;

  case 263:

/* Line 1810 of yacc.c  */
#line 1026 "web2c-parser.y"
    { my_output (last_id); }
    break;

  case 265:

/* Line 1810 of yacc.c  */
#line 1031 "web2c-parser.y"
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

  case 272:

/* Line 1810 of yacc.c  */
#line 1057 "web2c-parser.y"
    { my_output ("if"); my_output ("("); }
    break;

  case 274:

/* Line 1810 of yacc.c  */
#line 1062 "web2c-parser.y"
    { my_output (")"); }
    break;

  case 276:

/* Line 1810 of yacc.c  */
#line 1067 "web2c-parser.y"
    { new_line (); }
    break;

  case 278:

/* Line 1810 of yacc.c  */
#line 1070 "web2c-parser.y"
    { my_output ("{"); indent++; new_line();
				  my_output ("if"); my_output ("("); }
    break;

  case 279:

/* Line 1810 of yacc.c  */
#line 1073 "web2c-parser.y"
    { indent--; my_output ("}"); new_line(); }
    break;

  case 282:

/* Line 1810 of yacc.c  */
#line 1079 "web2c-parser.y"
    { my_output ("else"); }
    break;

  case 284:

/* Line 1810 of yacc.c  */
#line 1084 "web2c-parser.y"
    { my_output ("switch"); my_output ("("); }
    break;

  case 285:

/* Line 1810 of yacc.c  */
#line 1086 "web2c-parser.y"
    { my_output (")"); new_line();
				  my_output ("{"); indent++;
				}
    break;

  case 286:

/* Line 1810 of yacc.c  */
#line 1090 "web2c-parser.y"
    { indent--; my_output ("}"); new_line(); }
    break;

  case 289:

/* Line 1810 of yacc.c  */
#line 1098 "web2c-parser.y"
    { my_output ("break"); semicolon(); }
    break;

  case 292:

/* Line 1810 of yacc.c  */
#line 1106 "web2c-parser.y"
    { my_output ("case");
				  my_output (temp);
				  my_output (":"); new_line();
				}
    break;

  case 293:

/* Line 1810 of yacc.c  */
#line 1111 "web2c-parser.y"
    { my_output ("default:"); new_line(); }
    break;

  case 299:

/* Line 1810 of yacc.c  */
#line 1124 "web2c-parser.y"
    { my_output ("while");
				  my_output ("(");
				}
    break;

  case 300:

/* Line 1810 of yacc.c  */
#line 1128 "web2c-parser.y"
    { my_output (")"); }
    break;

  case 302:

/* Line 1810 of yacc.c  */
#line 1133 "web2c-parser.y"
    { my_output ("do"); my_output ("{"); indent++; }
    break;

  case 303:

/* Line 1810 of yacc.c  */
#line 1135 "web2c-parser.y"
    { indent--; my_output ("}");
				  my_output ("while"); my_output ("( ! (");
				}
    break;

  case 304:

/* Line 1810 of yacc.c  */
#line 1139 "web2c-parser.y"
    { my_output (") )"); }
    break;

  case 305:

/* Line 1810 of yacc.c  */
#line 1143 "web2c-parser.y"
    {
				  my_output ("{");
				  my_output ("register");
				  my_output ("integer");
				  if (strict_for)
					my_output ("for_begin,");
				  my_output ("for_end;");
				 }
    break;

  case 306:

/* Line 1810 of yacc.c  */
#line 1152 "web2c-parser.y"
    { if (strict_for)
					my_output ("for_begin");
				  else
					my_output (control_var);
				  my_output ("="); }
    break;

  case 307:

/* Line 1810 of yacc.c  */
#line 1158 "web2c-parser.y"
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

  case 308:

/* Line 1810 of yacc.c  */
#line 1175 "web2c-parser.y"
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

  case 309:

/* Line 1810 of yacc.c  */
#line 1193 "web2c-parser.y"
    { strcpy(control_var, last_id); }
    break;

  case 310:

/* Line 1810 of yacc.c  */
#line 1197 "web2c-parser.y"
    { my_output (";"); }
    break;

  case 311:

/* Line 1810 of yacc.c  */
#line 1199 "web2c-parser.y"
    {
				  strcpy(relation, "<=");
				  my_output ("for_end");
				  my_output ("="); }
    break;

  case 312:

/* Line 1810 of yacc.c  */
#line 1204 "web2c-parser.y"
    {
				  sprintf(for_stack + strlen(for_stack),
				    "#%s++ < for_end", control_var);
				}
    break;

  case 313:

/* Line 1810 of yacc.c  */
#line 1209 "web2c-parser.y"
    { my_output (";"); }
    break;

  case 314:

/* Line 1810 of yacc.c  */
#line 1211 "web2c-parser.y"
    {
				  strcpy(relation, ">=");
				  my_output ("for_end");
				  my_output ("="); }
    break;

  case 315:

/* Line 1810 of yacc.c  */
#line 1216 "web2c-parser.y"
    {
				  sprintf(for_stack + strlen(for_stack),
				    "#%s-- > for_end", control_var);
				}
    break;



/* Line 1810 of yacc.c  */
#line 3959 "web2c-parser.c"
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



/* Line 2071 of yacc.c  */
#line 1221 "web2c-parser.y"


static void
compute_array_bounds (void)
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
fixup_var_list (void)
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
doreturn (string label)
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
my_labs (long x)
{
    if (x < 0L) return(-x);
    return(x);
}


/* Output current function declaration to coerce file.  */

static void
do_proc_args (void)
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

  if (proc_is_noreturn) {
    fprintf (coerce, "WEB2C_NORETURN ");
    proc_is_noreturn = 0;
  }
  /* We can't use our P?H macros here, since there might be an arbitrary
     number of function arguments.  */
  fprintf (coerce, "%s %s (", fn_return_type, z_id);
  if (ids_paramed == 0) fprintf (coerce, "void");
  for (i = 0; i < ids_paramed; i++) {
    if (i > 0)
      putc (',', coerce);
    fprintf (coerce, "%s %s", arg_type[i], symbol (param_id_list[i]));
  }
  fprintf (coerce, ");\n");
}

static void
gen_function_head (void)
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
    /* We now always use ANSI C prototypes.  */
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
}

