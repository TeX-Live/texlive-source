/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 24 "web2c-parser.y" /* yacc.c:339  */

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

#line 102 "web2c-parser.c" /* yacc.c:339  */

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

/* Copy the second part of user declarations.  */

#line 271 "web2c-parser.c" /* yacc.c:358  */

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
#define YYLAST   562

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  193
/* YYNRULES -- Number of rules.  */
#define YYNRULES  315
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  492

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   314

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
      68,    69,    61,    56,    70,    57,    75,    62,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    74,    67,
      52,    50,    53,     2,     2,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    51,    54,    55,    58,    59,
      60,    63,    64,    65,    66
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
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

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "'='", "not_eq_tok", "'<'", "'>'", "less_eq_tok", "great_eq_tok", "'+'",
  "'-'", "or_tok", "unary_plus_tok", "unary_minus_tok", "'*'", "'/'",
  "div_tok", "mod_tok", "and_tok", "not_tok", "';'", "'('", "')'", "','",
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
  "$@89", "$@90", "$@91", "$@92", YY_NULLPTR
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
      61,   305,    60,    62,   306,   307,    43,    45,   308,   309,
     310,    42,    47,   311,   312,   313,   314,    59,    40,    41,
      44,    94,    91,    93,    58,    46
};
# endif

#define YYPACT_NINF -256

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-256)))

#define YYTABLE_NINF -314

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-314)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -256,    23,     8,  -256,    -7,    12,  -256,  -256,    32,    89,
     109,   125,   163,   205,   209,   112,   -16,   167,   173,   -18,
      16,   -28,   182,   183,  -256,   245,  -256,  -256,    48,  -256,
    -256,  -256,  -256,   195,  -256,   201,    17,  -256,  -256,  -256,
     221,  -256,   239,  -256,   -16,   206,   207,  -256,  -256,  -256,
    -256,   208,   240,   250,  -256,    39,  -256,   257,  -256,   258,
    -256,   269,  -256,  -256,  -256,  -256,  -256,    17,  -256,   221,
    -256,  -256,   260,  -256,   263,  -256,  -256,  -256,  -256,   251,
    -256,  -256,    -3,  -256,    21,    92,  -256,   253,  -256,  -256,
    -256,  -256,    -8,  -256,    86,   194,  -256,    24,  -256,   233,
     268,  -256,   242,  -256,   317,  -256,    21,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   288,  -256,  -256,  -256,  -256,
    -256,  -256,   112,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   449,  -256,  -256,  -256,   317,
       4,  -256,     4,   247,   247,   247,  -256,   293,   247,   245,
     317,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   243,  -256,   241,   299,  -256,
    -256,   274,   252,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,   254,  -256,   244,   248,   256,  -256,  -256,  -256,   294,
    -256,  -256,  -256,  -256,  -256,    36,  -256,  -256,  -256,  -256,
    -256,  -256,    -2,  -256,   264,  -256,  -256,  -256,   292,   303,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,   273,   239,   409,   317,   317,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,  -256,    35,
    -256,     1,  -256,  -256,  -256,   301,  -256,  -256,  -256,   293,
     176,   304,  -256,   176,   293,   176,    44,   276,   278,   293,
     331,  -256,  -256,  -256,  -256,  -256,   465,   465,   465,   465,
     465,   465,   131,   131,   131,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,   127,     4,     2,  -256,    15,  -256,    22,  -256,
    -256,     4,     4,     3,  -256,  -256,  -256,  -256,  -256,   226,
     176,  -256,  -256,   320,   481,  -256,   -19,   481,  -256,   180,
      44,  -256,  -256,  -256,  -256,  -256,  -256,   176,   176,   269,
      35,   334,  -256,  -256,     1,  -256,  -256,   128,  -256,  -256,
     301,  -256,  -256,    21,  -256,  -256,  -256,  -256,   276,   176,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,   333,  -256,   349,
     176,  -256,  -256,  -256,  -256,    55,   481,   481,  -256,   284,
       4,  -256,    15,  -256,  -256,  -256,   140,   295,   296,  -256,
     429,   198,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   345,  -256,   176,
     293,   361,  -256,   336,   169,  -256,   357,   347,  -256,  -256,
    -256,     4,   322,  -256,  -256,  -256,  -256,  -256,     6,  -256,
     141,  -256,   497,   497,   497,   497,   497,   497,   158,   158,
     158,  -256,  -256,  -256,  -256,  -256,   204,   359,  -256,   293,
     481,  -256,  -256,  -256,  -256,   342,  -256,  -256,  -256,   293,
    -256,     4,  -256,  -256,  -256,    70,  -256,   198,   331,   351,
     367,  -256,   176,   369,   176,  -256,    55,     7,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   293,  -256,  -256,  -256,   385,
    -256,  -256,   176,   176,  -256,   369,   293,  -256,   481,   481,
    -256,  -256
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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
     242,   243,   244,    76,    78,    40,    75,    44,    45,     0,
      95,   134,    95,   153,   153,   153,   148,   266,   153,    33,
       0,    55,    57,    61,    63,    65,    67,    47,    49,    71,
      51,    73,    53,    59,    69,     0,    46,     0,     0,   112,
     101,     0,     0,    90,    92,    93,    91,   105,   102,   103,
     104,     0,   154,     0,     0,     0,   179,   284,   305,     0,
     272,   302,   299,   185,   262,   199,   261,   263,   200,   201,
     192,   267,     0,   181,     0,   183,   186,   188,     0,     0,
     189,   190,   191,   187,   268,   270,   271,   269,   296,   297,
     298,     0,    82,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    41,    95,
     124,   116,   106,    89,   132,   160,   168,   172,   152,   266,
       0,     0,   265,     0,   266,     0,     0,     0,     0,   266,
     266,   193,   195,   150,    24,    77,    56,    58,    62,    64,
      66,    68,    48,    50,    72,    52,    74,    54,    60,    70,
     110,   109,     0,    95,     0,   114,     0,   162,     0,   156,
     158,    95,    95,     0,   249,   250,   245,   248,   247,     0,
       0,   241,   309,     0,   274,   273,     0,   300,   204,     0,
     198,   202,   252,   264,   140,   182,   184,     0,     0,   126,
      95,     0,   125,   113,   116,   122,   123,     0,   120,   158,
     160,   155,   161,     0,   177,   169,   173,   180,     0,     0,
     285,   221,   223,   227,   229,   231,   233,   213,   215,   237,
     217,   239,   219,   225,   235,   212,   306,     0,   303,     0,
       0,   207,   208,   206,   203,     0,   194,   196,    25,     0,
      95,   115,     0,   117,   163,   157,     0,     0,     0,   251,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   276,   275,     0,
     266,     0,   258,   260,     0,   254,     0,     0,   111,   107,
     121,    95,     0,   170,   174,   246,   293,   292,     0,   287,
       0,   290,   222,   224,   228,   230,   232,   234,   214,   216,
     238,   218,   240,   220,   226,   236,   310,     0,   278,   266,
     304,   301,   210,   209,   205,     0,   257,   253,   255,   266,
      26,    95,   118,   159,   294,     0,   286,     0,   266,     0,
       0,   307,     0,   281,     0,   259,     0,     0,   108,   295,
     288,   291,   289,   311,   314,   266,   279,   282,   277,     0,
     256,   178,     0,     0,   308,   281,   266,   211,   312,   315,
     280,   283
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,   335,
     279,  -256,  -256,  -256,   259,  -256,  -256,   315,   246,  -256,
     339,  -256,  -256,  -256,  -256,   -94,   -46,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,   181,  -256,   330,  -256,  -256,
    -256,  -139,  -256,   -34,  -256,   337,  -256,  -256,  -256,  -256,
      85,   -45,  -256,  -256,  -256,    83,  -256,  -256,  -256,    37,
    -256,  -256,   101,  -256,   326,  -256,  -256,    88,   321,  -256,
    -256,  -256,   332,  -256,  -256,  -256,  -256,  -256,  -256,  -113,
    -256,  -256,    99,  -256,   100,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,   152,  -256,  -256,
    -256,  -245,  -238,  -256,  -255,  -256,  -256,  -256,  -256,  -147,
    -256,  -256,  -256,   122,  -256,  -256,  -256,  -236,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,   -60,  -256,  -256,  -256,    95,  -256,  -256,  -256,
     -21,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
     -11,  -256,  -256,  -256,  -256,  -256,   -33,  -256,  -256,  -256,
    -256,  -256,    -1,  -256,    -4,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,  -256,
    -256,  -256,  -256
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    15,    85,     2,     6,     7,    17,    28,    29,
     121,   122,   319,   406,    25,    40,    55,    56,    42,    57,
      58,    59,    79,   104,   165,   297,   135,   230,   231,   233,
     235,   224,   225,   236,   226,   227,   228,   229,   237,   232,
     234,   136,   150,   137,   138,    61,    72,    73,    74,    87,
     140,   334,   173,   174,    52,    53,   175,   176,   177,   178,
     282,   409,   179,   241,   284,   285,   286,   411,   327,   328,
     180,   283,    76,    82,    83,    84,   142,    92,    93,   117,
     147,    97,    98,    99,   100,   115,   101,   148,   145,   183,
     245,   288,   332,   333,   289,   290,   329,   114,   102,   103,
     143,   291,   377,   144,   292,   378,   111,   335,   450,   201,
     249,   202,   203,   204,   205,   206,   207,   317,   318,   298,
     256,   209,   310,   311,   360,   444,   464,   304,   388,   389,
     391,   393,   382,   383,   394,   384,   385,   386,   387,   395,
     390,   392,   300,   301,   339,   338,   313,   365,   404,   466,
     405,   446,   210,   257,   211,   212,   213,   214,   215,   253,
     305,   357,   398,   439,   462,   485,   478,   486,   216,   250,
     381,   418,   419,   420,   421,   456,   217,   218,   255,   359,
     219,   254,   399,   220,   251,   396,   475,   303,   437,   459,
     482,   460,   483
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     208,   172,    51,   181,   293,   316,   358,   167,   258,   306,
     134,  -119,   323,   337,   299,   168,   454,   481,     9,   307,
      26,   315,    36,     3,   169,    10,    27,     4,   116,     8,
      11,   184,   185,  -130,  -130,   221,    12,    94,    13,    37,
      47,    48,    95,  -130,   139,   134,    49,   170,   259,    32,
      33,   325,    96,    47,    48,     5,   134,    89,    90,    49,
      14,   326,   106,    50,   355,   259,   107,    91,  -119,   324,
     259,    47,    48,   455,   259,   171,    50,    49,   280,   139,
     469,   366,   367,    34,    35,   125,   126,   127,   128,   330,
     139,   331,   195,   166,    50,   294,   295,   129,   402,   416,
      16,   417,   208,   380,   223,    94,    68,   208,  -197,    69,
      95,  -197,   208,   208,   130,   131,   308,    43,    44,   309,
      96,   132,   108,   296,   401,    18,   109,   110,    24,   403,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   134,   134,   134,   322,    19,   422,   423,   424,   425,
     426,   427,   428,   429,   430,   431,   432,   433,   434,   435,
     436,    20,   441,   440,   139,   139,   139,   139,   139,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,   277,
     278,   279,   160,   161,   162,   163,   164,   320,   372,    21,
     321,   463,   373,   472,   467,   281,   125,   126,   127,   128,
     106,   457,  -313,   195,   412,   458,   294,   295,   129,   350,
     351,   352,   353,   354,   361,   362,   363,   416,   479,   417,
     403,   408,   112,   113,    30,   130,   131,   484,   447,   448,
      31,    22,   132,   340,   296,    23,   488,   489,   491,    38,
      39,    41,    54,   208,   341,   342,   343,   344,   345,   346,
     347,   348,   349,    60,    45,   350,   351,   352,   353,   354,
      46,    66,   452,    63,    64,    65,   341,   342,   343,   344,
     345,   346,   347,   348,   349,    67,   281,   350,   351,   352,
     353,   354,   208,   -37,    71,    75,   -86,   186,   187,    81,
     119,    86,   208,   105,   120,   188,   146,   189,   190,   123,
     238,   208,   408,   239,   191,   182,   240,   242,   246,   243,
     192,   244,   247,   248,   193,   252,   261,   287,   208,   194,
     195,   196,   197,   198,   199,   186,   187,   262,   260,   208,
     263,   302,   200,   188,   312,   189,   190,   125,   126,   127,
     128,   370,   191,   314,   356,   397,   400,   407,   192,   129,
     438,   449,   413,   414,   451,   453,   461,   194,   195,   196,
     197,   198,   199,   465,   473,   474,   130,   131,   477,    62,
     200,   149,   124,   132,    78,   133,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   222,    70,   350,   351,   352,
     353,   354,    80,   264,    77,   369,   468,   371,    88,   410,
     445,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     368,   376,   350,   351,   352,   353,   354,   141,   374,   118,
     375,   442,   364,   379,   443,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   336,   480,   350,   351,   352,   353,
     354,   476,   490,   471,   470,     0,     0,     0,   487,   151,
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

static const yytype_int16 yycheck[] =
{
     147,   140,    36,   142,   249,   260,    25,     3,    10,   254,
     104,    10,    10,    10,   250,    11,    10,    10,     6,   255,
      36,   259,    50,     0,    20,    13,    42,    19,     4,    36,
      18,   144,   145,    36,    37,   148,    24,    13,    26,    67,
      36,    37,    18,    46,   104,   139,    42,    43,    67,    67,
      68,    36,    28,    36,    37,    47,   150,    36,    37,    42,
      48,    46,    70,    59,   300,    67,    74,    46,    67,    67,
      67,    36,    37,    67,    67,    71,    59,    42,    43,   139,
      10,   317,   318,    67,    68,    30,    31,    32,    33,    67,
     150,    69,    37,   139,    59,    40,    41,    42,    43,    29,
      68,    31,   249,   339,   150,    13,    67,   254,    72,    70,
      18,    75,   259,   260,    59,    60,    72,    69,    70,    75,
      28,    66,    36,    68,   360,    36,    40,    41,    16,   365,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   283,    36,   382,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,    36,   400,   399,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    61,    62,    63,    64,    65,    70,    70,    36,
      73,   439,    74,   458,   449,   239,    30,    31,    32,    33,
      70,    70,     8,    37,    74,    74,    40,    41,    42,    61,
      62,    63,    64,    65,    44,    45,    46,    29,   464,    31,
     466,   370,    38,    39,    67,    59,    60,   475,    69,    70,
      67,    36,    66,    17,    68,    36,   482,   483,   486,    67,
      67,     6,    31,   400,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    24,    69,    61,    62,    63,    64,    65,
      69,    31,   411,    67,    67,    67,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    35,   320,    61,    62,    63,
      64,    65,   439,    36,    36,    26,    36,     4,     5,    36,
      67,    50,   449,    50,    36,    12,    18,    14,    15,    67,
      67,   458,   451,    72,    21,    68,    17,    43,    74,    67,
      27,    67,    74,    67,    31,    31,    34,    26,   475,    36,
      37,    38,    39,    40,    41,     4,     5,    34,    74,   486,
      67,    37,    49,    12,    68,    14,    15,    30,    31,    32,
      33,    17,    21,    75,    34,    22,     7,    73,    27,    42,
      15,     4,    67,    67,    17,    43,     7,    36,    37,    38,
      39,    40,    41,    31,    23,     8,    59,    60,     9,    44,
      49,   122,   103,    66,    69,    68,    50,    51,    52,    53,
      54,    55,    56,    57,    58,   149,    57,    61,    62,    63,
      64,    65,    72,   222,    67,   320,   451,   324,    82,   372,
      74,    50,    51,    52,    53,    54,    55,    56,    57,    58,
     319,   333,    61,    62,    63,    64,    65,   106,   329,    97,
     330,    70,   310,   338,    73,    50,    51,    52,    53,    54,
      55,    56,    57,    58,   292,   466,    61,    62,    63,    64,
      65,   462,   485,   457,   455,    -1,    -1,    -1,    73,    50,
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
      59,   129,   130,   131,    31,    92,    93,    95,    96,    97,
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
     259,   165,    94,   102,   107,   108,   110,   111,   112,   113,
     103,   104,   115,   105,   116,   106,   109,   114,    67,    72,
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
     203,   246,   208,   209,   211,   212,   213,   214,   204,   205,
     216,   206,   217,   207,   210,   215,   261,    22,   238,   258,
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

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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
        case 2:
#line 66 "web2c-parser.y" /* yacc.c:1646  */
    {
	    printf ("#define %s\n", uppercasify (program_name));
            block_level++;
	    printf ("#include \"%s\"\n", std_header);
	  }
#line 1787 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 3:
#line 73 "web2c-parser.y" /* yacc.c:1646  */
    { printf ("\n#include \"%s\"\n", coerce_name); }
#line 1793 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 4:
#line 76 "web2c-parser.y" /* yacc.c:1646  */
    { YYACCEPT; }
#line 1799 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 7:
#line 86 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = field_id_tok;
	    }
#line 1808 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 8:
#line 91 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = fun_id_tok;
	    }
#line 1817 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 9:
#line 96 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = const_id_tok;
	    }
#line 1826 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 10:
#line 101 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = fun_param_tok;
	    }
#line 1835 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 11:
#line 106 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = proc_id_tok;
	    }
#line 1844 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 12:
#line 111 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = proc_param_tok;
	    }
#line 1853 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 13:
#line 116 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = type_id_tok;
	    }
#line 1862 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 14:
#line 121 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = type_id_tok;
	      sym_table[ii].val = lower_bound;
	      sym_table[ii].val_sym = lower_sym;
	      sym_table[ii].upper = upper_bound;
	      sym_table[ii].upper_sym = upper_sym;
	    }
#line 1875 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 15:
#line 130 "web2c-parser.y" /* yacc.c:1646  */
    {
	      ii = add_to_table (last_id);
	      sym_table[ii].typ = var_id_tok;
	    }
#line 1884 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 23:
#line 157 "web2c-parser.y" /* yacc.c:1646  */
    {	if (block_level > 0) my_output("{\n ");
                indent++; block_level++;
              }
#line 1892 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 24:
#line 162 "web2c-parser.y" /* yacc.c:1646  */
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
#line 1910 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 25:
#line 176 "web2c-parser.y" /* yacc.c:1646  */
    { doing_statements = true; }
#line 1916 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 26:
#line 178 "web2c-parser.y" /* yacc.c:1646  */
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
#line 1947 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 28:
#line 208 "web2c-parser.y" /* yacc.c:1646  */
    { my_output("/*"); }
#line 1953 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 29:
#line 210 "web2c-parser.y" /* yacc.c:1646  */
    { my_output("*/"); }
#line 1959 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 32:
#line 218 "web2c-parser.y" /* yacc.c:1646  */
    { my_output(temp); }
#line 1965 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 34:
#line 224 "web2c-parser.y" /* yacc.c:1646  */
    { new_line(); }
#line 1971 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 37:
#line 233 "web2c-parser.y" /* yacc.c:1646  */
    { /* `#define' must be in column 1 for pcc. */
            unsigned save = indent;
	    new_line ();
	    indent = 0;
	    my_output ("#define");
	    indent = save;
	  }
#line 1983 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 38:
#line 241 "web2c-parser.y" /* yacc.c:1646  */
    { ii = add_to_table (last_id);
	    sym_table[ii].typ = const_id_tok;
            my_output (last_id);
          }
#line 1992 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 39:
#line 245 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("("); }
#line 1998 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 40:
#line 246 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (")"); }
#line 2004 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 41:
#line 247 "web2c-parser.y" /* yacc.c:1646  */
    { sym_table[ii].val = last_i_num; new_line(); }
#line 2010 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 42:
#line 252 "web2c-parser.y" /* yacc.c:1646  */
    {
             sscanf (temp, "%ld", &last_i_num);
             if (my_labs ((long) last_i_num) > 32767)
               strcat (temp, "L");
             my_output (temp);
             (yyval) = ex_32;
           }
#line 2022 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 43:
#line 259 "web2c-parser.y" /* yacc.c:1646  */
    { my_output(temp); (yyval) = ex_real; }
#line 2028 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 44:
#line 260 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = 0; }
#line 2034 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 45:
#line 261 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = ex_32; }
#line 2040 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 46:
#line 266 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 2046 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 47:
#line 267 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("+"); }
#line 2052 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 48:
#line 268 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2058 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 49:
#line 269 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("-"); }
#line 2064 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 50:
#line 270 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2070 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 51:
#line 271 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("*"); }
#line 2076 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 52:
#line 272 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2082 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 53:
#line 273 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("/"); }
#line 2088 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 54:
#line 274 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2094 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 55:
#line 275 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("=="); }
#line 2100 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 56:
#line 276 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2106 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 57:
#line 277 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("!="); }
#line 2112 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 58:
#line 278 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2118 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 59:
#line 279 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("%"); }
#line 2124 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 60:
#line 280 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2130 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 61:
#line 281 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("<"); }
#line 2136 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 62:
#line 282 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2142 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 63:
#line 283 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (">"); }
#line 2148 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 64:
#line 284 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2154 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 65:
#line 285 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("<="); }
#line 2160 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 66:
#line 286 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2166 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 67:
#line 287 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (">="); }
#line 2172 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 68:
#line 288 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2178 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 69:
#line 289 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("&&"); }
#line 2184 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 70:
#line 290 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2190 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 71:
#line 291 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("||"); }
#line 2196 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 72:
#line 292 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2202 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 73:
#line 293 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("/ ((double)"); }
#line 2208 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 74:
#line 294 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); my_output (")"); }
#line 2214 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 75:
#line 295 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 2220 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 76:
#line 300 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("("); }
#line 2226 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 77:
#line 302 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (")"); (yyval) = (yyvsp[-3]); }
#line 2232 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 79:
#line 308 "web2c-parser.y" /* yacc.c:1646  */
    {
              char s[132];
              get_string_literal(s);
              my_output (s);
            }
#line 2242 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 80:
#line 314 "web2c-parser.y" /* yacc.c:1646  */
    {
              char s[5];
              get_single_char(s);
              my_output (s);
            }
#line 2252 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 81:
#line 322 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (last_id); }
#line 2258 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 86:
#line 334 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("typedef"); }
#line 2264 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 87:
#line 336 "web2c-parser.y" /* yacc.c:1646  */
    {
            ii = add_to_table(last_id);
            sym_table[ii].typ = type_id_tok;
            strcpy(safe_string, last_id);
            last_type = ii;
          }
#line 2275 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 88:
#line 343 "web2c-parser.y" /* yacc.c:1646  */
    {
            array_bounds[0] = 0;
            array_offset[0] = 0;
          }
#line 2284 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 89:
#line 348 "web2c-parser.y" /* yacc.c:1646  */
    {
            if (*array_offset) {
              yyerror ("Cannot typedef arrays with offsets");
            }
            my_output (safe_string);
            my_output (array_bounds);
            semicolon ();
            last_type = -1;
          }
#line 2298 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 92:
#line 366 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2335 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 97:
#line 412 "web2c-parser.y" /* yacc.c:1646  */
    {
              lower_bound = upper_bound;
              lower_sym = upper_sym;
              sscanf (temp, "%ld", &upper_bound);
              upper_sym = -1; /* no sym table entry */
            }
#line 2346 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 98:
#line 419 "web2c-parser.y" /* yacc.c:1646  */
    {
              lower_bound = upper_bound;
              lower_sym = upper_sym;
              upper_bound = sym_table[l_s].val;
              upper_sym = l_s;
            }
#line 2357 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 99:
#line 426 "web2c-parser.y" /* yacc.c:1646  */
    { /* We've changed some constants into dynamic variables.
	         To avoid changing all the subrange decls, just use integer.
	         This does not work for arrays, for which we check later.  */
	      lower_bound = upper_bound;
	      lower_sym = upper_sym;
	      upper_bound = 0;
	      upper_sym = 0; /* Translate to integer.  */
	    }
#line 2370 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 100:
#line 435 "web2c-parser.y" /* yacc.c:1646  */
    { /* Same as var_id_tok, to avoid changing type definitions.
	         Should keep track of the variables we use in this way
	         and make sure they're all eventually defined.  */
	      lower_bound = upper_bound;
	      lower_sym = upper_sym;
	      upper_bound = 0;
	      upper_sym = 0;
	    }
#line 2383 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 101:
#line 447 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2398 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 102:
#line 461 "web2c-parser.y" /* yacc.c:1646  */
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
#line 2406 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 104:
#line 466 "web2c-parser.y" /* yacc.c:1646  */
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
#line 2414 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 105:
#line 470 "web2c-parser.y" /* yacc.c:1646  */
    { if (last_type >= 0)
	        sym_table[last_type].var_not_needed = true;
            }
#line 2422 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 106:
#line 477 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2438 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 109:
#line 497 "web2c-parser.y" /* yacc.c:1646  */
    { compute_array_bounds(); }
#line 2444 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 110:
#line 499 "web2c-parser.y" /* yacc.c:1646  */
    {
              lower_bound = sym_table[l_s].val;
              lower_sym = sym_table[l_s].val_sym;
              upper_bound = sym_table[l_s].upper;
              upper_sym = sym_table[l_s].upper_sym;
              compute_array_bounds();
            }
#line 2456 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 112:
#line 512 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("struct"); my_output ("{"); indent++; }
#line 2462 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 113:
#line 514 "web2c-parser.y" /* yacc.c:1646  */
    { indent--; my_output ("}"); semicolon(); }
#line 2468 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 116:
#line 522 "web2c-parser.y" /* yacc.c:1646  */
    { field_list[0] = 0; }
#line 2474 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 117:
#line 524 "web2c-parser.y" /* yacc.c:1646  */
    {
				  /*array_bounds[0] = 0;
				  array_offset[0] = 0;*/
				}
#line 2483 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 118:
#line 529 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2501 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 122:
#line 550 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2517 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 123:
#line 562 "web2c-parser.y" /* yacc.c:1646  */
    { int i=0, j=0;
				  while (field_list[i] == '!')
					while(field_list[i++]);
				  field_list[i++] = '!';
				  while (last_id[j])
					field_list[i++] = last_id[j++];
				  field_list[i++] = 0;
				  field_list[i++] = 0;
				}
#line 2531 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 124:
#line 575 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("text /* of "); }
#line 2537 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 125:
#line 577 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("*/"); }
#line 2543 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 130:
#line 591 "web2c-parser.y" /* yacc.c:1646  */
    {
            var_list[0] = 0;
            array_bounds[0] = 0;
            array_offset[0] = 0;
            var_formals = false;
            ids_paramed = 0;
          }
#line 2555 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 131:
#line 599 "web2c-parser.y" /* yacc.c:1646  */
    {
            array_bounds[0] = 0;
            array_offset[0] = 0;
          }
#line 2564 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 132:
#line 604 "web2c-parser.y" /* yacc.c:1646  */
    { fixup_var_list(); }
#line 2570 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 135:
#line 612 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2588 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 136:
#line 626 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2606 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 137:
#line 640 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2624 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 139:
#line 658 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("void mainbody( void ) {");
		  indent++;
		  new_line ();
		}
#line 2633 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 140:
#line 663 "web2c-parser.y" /* yacc.c:1646  */
    { indent--;
                  my_output ("}");
                  new_line ();
                }
#line 2642 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 143:
#line 675 "web2c-parser.y" /* yacc.c:1646  */
    { new_line(); remove_locals(); }
#line 2648 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 144:
#line 677 "web2c-parser.y" /* yacc.c:1646  */
    { new_line(); remove_locals(); }
#line 2654 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 147:
#line 686 "web2c-parser.y" /* yacc.c:1646  */
    { proc_is_noreturn = 1; }
#line 2660 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 149:
#line 692 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2676 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 150:
#line 704 "web2c-parser.y" /* yacc.c:1646  */
    { strcpy(fn_return_type, "void");
	      do_proc_args();
	      gen_function_head(); }
#line 2684 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 151:
#line 708 "web2c-parser.y" /* yacc.c:1646  */
    { ii = l_s;
	      if (debug)
	        fprintf(stderr, "%3d Procedure %s\n", pf_count++, last_id);
	      strcpy(my_routine, last_id);
	      my_output ("void");
	      new_line ();
	    }
#line 2696 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 152:
#line 716 "web2c-parser.y" /* yacc.c:1646  */
    { strcpy(fn_return_type, "void");
	      do_proc_args();
	      gen_function_head();
            }
#line 2705 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 153:
#line 724 "web2c-parser.y" /* yacc.c:1646  */
    {
              strcpy (z_id, last_id);
	      mark ();
	      ids_paramed = 0;
	    }
#line 2715 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 154:
#line 730 "web2c-parser.y" /* yacc.c:1646  */
    { sprintf (z_id, "z%s", last_id);
	      ids_paramed = 0;
	      if (sym_table[ii].typ == proc_id_tok)
	        sym_table[ii].typ = proc_param_tok;
	      else if (sym_table[ii].typ == fun_id_tok)
	        sym_table[ii].typ = fun_param_tok;
	      mark();
	    }
#line 2728 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 158:
#line 746 "web2c-parser.y" /* yacc.c:1646  */
    { ids_typed = ids_paramed; }
#line 2734 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 159:
#line 748 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2751 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 160:
#line 762 "web2c-parser.y" /* yacc.c:1646  */
    {var_formals = 0; }
#line 2757 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 162:
#line 763 "web2c-parser.y" /* yacc.c:1646  */
    {var_formals = 1; }
#line 2763 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 167:
#line 775 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2778 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 168:
#line 786 "web2c-parser.y" /* yacc.c:1646  */
    {
              normal();
              array_bounds[0] = 0;
              array_offset[0] = 0;
            }
#line 2788 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 169:
#line 792 "web2c-parser.y" /* yacc.c:1646  */
    {
              get_result_type(fn_return_type);
              do_proc_args();
              gen_function_head();
            }
#line 2798 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 171:
#line 799 "web2c-parser.y" /* yacc.c:1646  */
    {
              orig_out = out;
              out = 0;
              ii = l_s;
              if (debug)
                fprintf(stderr, "%3d Function %s\n", pf_count++, last_id);
              strcpy(my_routine, last_id);
              uses_eqtb = uses_mem = false;
            }
#line 2812 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 172:
#line 809 "web2c-parser.y" /* yacc.c:1646  */
    { normal();
              array_bounds[0] = 0;
              array_offset[0] = 0;
            }
#line 2821 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 173:
#line 814 "web2c-parser.y" /* yacc.c:1646  */
    { get_result_type(fn_return_type);
              do_proc_args();
              gen_function_head();
            }
#line 2830 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 179:
#line 832 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("{"); indent++; new_line(); }
#line 2836 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 180:
#line 834 "web2c-parser.y" /* yacc.c:1646  */
    { indent--; my_output ("}"); new_line(); }
#line 2842 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 185:
#line 847 "web2c-parser.y" /* yacc.c:1646  */
    {if (!doreturn(temp)) {
				      sprintf(safe_string, "lab%s:", temp);
				    my_output (safe_string);
				 }
				}
#line 2852 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 186:
#line 855 "web2c-parser.y" /* yacc.c:1646  */
    { semicolon(); }
#line 2858 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 187:
#line 857 "web2c-parser.y" /* yacc.c:1646  */
    { semicolon(); }
#line 2864 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 192:
#line 865 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("break"); }
#line 2870 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 193:
#line 869 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("="); }
#line 2876 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 195:
#line 872 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("Result ="); }
#line 2882 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 197:
#line 877 "web2c-parser.y" /* yacc.c:1646  */
    { if (strcmp(last_id, "mem") == 0)
					uses_mem = 1;
				  else if (strcmp(last_id, "eqtb") == 0)
					uses_eqtb = 1;
				  if (sym_table[l_s].var_formal)
					putchar('*');
				  my_output (last_id);
				  (yyval) = ex_32;
				}
#line 2896 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 199:
#line 888 "web2c-parser.y" /* yacc.c:1646  */
    { if (sym_table[l_s].var_formal)
					putchar('*');
				  my_output (last_id); (yyval) = ex_32; }
#line 2904 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 200:
#line 894 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = ex_32; }
#line 2910 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 201:
#line 896 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = ex_32; }
#line 2916 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 204:
#line 904 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("["); }
#line 2922 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 205:
#line 906 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("]"); }
#line 2928 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 206:
#line 908 "web2c-parser.y" /* yacc.c:1646  */
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
#line 2950 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 207:
#line 926 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (".hh.b0"); }
#line 2956 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 208:
#line 928 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (".hh.b1"); }
#line 2962 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 210:
#line 933 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("]["); }
#line 2968 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 212:
#line 938 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 2974 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 213:
#line 939 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("+"); }
#line 2980 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 214:
#line 940 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2986 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 215:
#line 941 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("-"); }
#line 2992 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 216:
#line 942 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 2998 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 217:
#line 943 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("*"); }
#line 3004 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 218:
#line 944 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3010 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 219:
#line 945 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("/"); }
#line 3016 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 220:
#line 946 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3022 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 221:
#line 947 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("=="); }
#line 3028 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 222:
#line 948 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3034 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 223:
#line 949 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("!="); }
#line 3040 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 224:
#line 950 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3046 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 225:
#line 951 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("%"); }
#line 3052 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 226:
#line 952 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3058 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 227:
#line 953 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("<"); }
#line 3064 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 228:
#line 954 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3070 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 229:
#line 955 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (">"); }
#line 3076 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 230:
#line 956 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3082 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 231:
#line 957 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("<="); }
#line 3088 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 232:
#line 958 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3094 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 233:
#line 959 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (">="); }
#line 3100 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 234:
#line 960 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3106 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 235:
#line 961 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("&&"); }
#line 3112 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 236:
#line 962 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3118 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 237:
#line 963 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("||"); }
#line 3124 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 238:
#line 964 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); }
#line 3130 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 239:
#line 966 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("/ ((double)"); }
#line 3136 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 240:
#line 968 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = max ((yyvsp[-3]), (yyvsp[0])); my_output (")"); }
#line 3142 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 241:
#line 970 "web2c-parser.y" /* yacc.c:1646  */
    { (yyval) = (yyvsp[0]); }
#line 3148 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 243:
#line 976 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("- (integer)"); }
#line 3154 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 244:
#line 978 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("!"); }
#line 3160 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 245:
#line 983 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("("); }
#line 3166 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 246:
#line 985 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (")"); (yyval) = (yyvsp[-3]); }
#line 3172 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 249:
#line 989 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (last_id); my_output ("()"); }
#line 3178 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 250:
#line 991 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (last_id); }
#line 3184 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 252:
#line 996 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("("); }
#line 3190 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 253:
#line 997 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (")"); }
#line 3196 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 255:
#line 1002 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (","); }
#line 3202 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1009 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (last_id); }
#line 3208 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1018 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (last_id); my_output ("()"); }
#line 3214 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 262:
#line 1020 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (last_id);
				  ii = add_to_table(last_id);
				  sym_table[ii].typ = proc_id_tok;
				  my_output ("()");
				}
#line 3224 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 263:
#line 1026 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (last_id); }
#line 3230 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1031 "web2c-parser.y" /* yacc.c:1646  */
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
#line 3246 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1057 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("if"); my_output ("("); }
#line 3252 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1062 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (")"); }
#line 3258 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 276:
#line 1067 "web2c-parser.y" /* yacc.c:1646  */
    { new_line (); }
#line 3264 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1070 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("{"); indent++; new_line();
				  my_output ("if"); my_output ("("); }
#line 3271 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1073 "web2c-parser.y" /* yacc.c:1646  */
    { indent--; my_output ("}"); new_line(); }
#line 3277 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1079 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("else"); }
#line 3283 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1084 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("switch"); my_output ("("); }
#line 3289 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1086 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (")"); new_line();
				  my_output ("{"); indent++;
				}
#line 3297 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1090 "web2c-parser.y" /* yacc.c:1646  */
    { indent--; my_output ("}"); new_line(); }
#line 3303 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1098 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("break"); semicolon(); }
#line 3309 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1106 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("case");
				  my_output (temp);
				  my_output (":"); new_line();
				}
#line 3318 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1111 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("default:"); new_line(); }
#line 3324 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 299:
#line 1124 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("while");
				  my_output ("(");
				}
#line 3332 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 300:
#line 1128 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (")"); }
#line 3338 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 302:
#line 1133 "web2c-parser.y" /* yacc.c:1646  */
    { my_output ("do"); my_output ("{"); indent++; }
#line 3344 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 303:
#line 1135 "web2c-parser.y" /* yacc.c:1646  */
    { indent--; my_output ("}");
				  my_output ("while"); my_output ("( ! (");
				}
#line 3352 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 304:
#line 1139 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (") )"); }
#line 3358 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1143 "web2c-parser.y" /* yacc.c:1646  */
    {
				  my_output ("{");
				  my_output ("register");
				  my_output ("integer");
				  if (strict_for)
					my_output ("for_begin,");
				  my_output ("for_end;");
				 }
#line 3371 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1152 "web2c-parser.y" /* yacc.c:1646  */
    { if (strict_for)
					my_output ("for_begin");
				  else
					my_output (control_var);
				  my_output ("="); }
#line 3381 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1158 "web2c-parser.y" /* yacc.c:1646  */
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
#line 3402 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1175 "web2c-parser.y" /* yacc.c:1646  */
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
#line 3422 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 309:
#line 1193 "web2c-parser.y" /* yacc.c:1646  */
    { strcpy(control_var, last_id); }
#line 3428 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 310:
#line 1197 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (";"); }
#line 3434 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 311:
#line 1199 "web2c-parser.y" /* yacc.c:1646  */
    {
				  strcpy(relation, "<=");
				  my_output ("for_end");
				  my_output ("="); }
#line 3443 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 312:
#line 1204 "web2c-parser.y" /* yacc.c:1646  */
    {
				  sprintf(for_stack + strlen(for_stack),
				    "#%s++ < for_end", control_var);
				}
#line 3452 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 313:
#line 1209 "web2c-parser.y" /* yacc.c:1646  */
    { my_output (";"); }
#line 3458 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 314:
#line 1211 "web2c-parser.y" /* yacc.c:1646  */
    {
				  strcpy(relation, ">=");
				  my_output ("for_end");
				  my_output ("="); }
#line 3467 "web2c-parser.c" /* yacc.c:1646  */
    break;

  case 315:
#line 1216 "web2c-parser.y" /* yacc.c:1646  */
    {
				  sprintf(for_stack + strlen(for_stack),
				    "#%s-- > for_end", control_var);
				}
#line 3476 "web2c-parser.c" /* yacc.c:1646  */
    break;


#line 3480 "web2c-parser.c" /* yacc.c:1646  */
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
#line 1221 "web2c-parser.y" /* yacc.c:1906  */


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
