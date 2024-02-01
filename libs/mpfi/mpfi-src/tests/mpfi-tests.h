/* mpfi-tests.h -- Include file for mpfi's tests.

Copyright 2009, 2010,
                     Spaces project, Inria Lorraine
                     and Salsa project, INRIA Rocquencourt,
                     and Arenaire project, Inria Rhone-Alpes, France
                     and Lab. ANO, USTL (Univ. of Lille),  France


This file is part of the MPFI Library.

The MPFI Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The MPFI Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPFI Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
MA 02110-1301, USA. */

#ifndef __MPFI_TESTS_H__
#define __MPFI_TESTS_H__

#include <stdlib.h>

#include "mpfi_io.h"
#include "mpfi-impl.h"

#define STR(a) # a
#define QUOTE(a) STR(a)


/** GENERIC TESTS **/

/* When adding a new generic test, one must declare a mpfi_function_t
   variable and initialize it with the MPFI_FUN_SET macro before calling one
   of the check_* functions. */

typedef int (*I_fun)   (mpfi_t);
typedef int (*II_fun)  (mpfi_t, mpfi_srcptr);
typedef int (*IS_fun)  (mpfi_t, long);
typedef int (*IU_fun)  (mpfi_t, unsigned long);
/*
typedef int (*JS_fun)  (mpfi_t, intmax_t);
typedef int (*JU_fun)  (mpfi_t, uintmax_t);
*/
typedef int (*ID_fun)  (mpfi_t, double);
typedef int (*IF_fun)  (mpfi_t, float);
typedef int (*IL_fun)  (mpfi_t, long double);
typedef int (*IZ_fun)  (mpfi_t, mpz_srcptr);
typedef int (*IQ_fun)  (mpfi_t, mpq_srcptr);
typedef int (*IR_fun)  (mpfi_t, mpfr_srcptr);
typedef int (*III_fun) (mpfi_t, mpfi_srcptr, mpfi_srcptr);
/*typedef int (*IIII_fun)(mpfi_t, mpfi_t, mpfi_srcptr, mpfi_srcptr);*/
typedef int (*IIII_fun)(mpfi_t, mpfi_srcptr, mpfi_srcptr, mpfi_srcptr);
typedef int (*IIU_fun) (mpfi_t, mpfi_srcptr, unsigned long);
typedef int (*IIS_fun) (mpfi_t, mpfi_srcptr, long);
typedef int (*IID_fun) (mpfi_t, mpfi_srcptr, double);
typedef int (*IIZ_fun) (mpfi_t, mpfi_srcptr, mpz_srcptr);
typedef int (*IIQ_fun) (mpfi_t, mpfi_srcptr, mpq_srcptr);
typedef int (*IIR_fun) (mpfi_t, mpfi_srcptr, mpfr_srcptr);
typedef int (*IUI_fun) (mpfi_t, unsigned long, mpfi_srcptr);
typedef int (*ISI_fun) (mpfi_t, long, mpfi_srcptr);
typedef int (*IDI_fun) (mpfi_t, double, mpfi_srcptr);
typedef int (*IZI_fun) (mpfi_t, mpz_srcptr, mpfi_srcptr);
typedef int (*IQI_fun) (mpfi_t, mpq_srcptr, mpfi_srcptr);
typedef int (*IRI_fun) (mpfi_t, mpfr_srcptr, mpfi_srcptr);
typedef int (*RI_fun)  (mpfr_t, mpfi_srcptr);

typedef int (*R_fun)   (mpfr_t, mpfr_rnd_t);
typedef int (*RR_fun)  (mpfr_t, mpfr_srcptr, mpfr_rnd_t);
typedef int (*RRU_fun) (mpfr_t, mpfr_srcptr, unsigned long, mpfr_rnd_t);
typedef int (*RRS_fun) (mpfr_t, mpfr_srcptr, long, mpfr_rnd_t);
typedef int (*RRD_fun) (mpfr_t, mpfr_srcptr, double, mpfr_rnd_t);
typedef int (*RRZ_fun) (mpfr_t, mpfr_srcptr, mpz_srcptr, mpfr_rnd_t);
typedef int (*RRQ_fun) (mpfr_t, mpfr_srcptr, mpq_srcptr, mpfr_rnd_t);
typedef int (*RRR_fun) (mpfr_t, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t);
typedef int (*RRRR_fun) (mpfr_t, mpfr_srcptr, mpfr_srcptr, mpfr_srcptr, mpfr_rnd_t);
typedef int (*RUR_fun) (mpfr_t, unsigned long, mpfr_srcptr, mpfr_rnd_t);
typedef int (*RSR_fun) (mpfr_t, long, mpfr_srcptr, mpfr_rnd_t);
typedef int (*RDR_fun) (mpfr_t, double, mpfr_srcptr, mpfr_rnd_t);
typedef int (*RZR_fun) (mpfr_t, mpz_srcptr, mpfr_srcptr, mpfr_rnd_t);
typedef int (*RQR_fun) (mpfr_t, mpq_srcptr, mpfr_srcptr, mpfr_rnd_t);
typedef void *NULL_fun;

typedef union
{
  I_fun   I;       /* output: mpfi_t, no input */
  II_fun  II;      /* output: mpfi_t, input: mpfi_t */
  IS_fun  IS;      /* output: mpfi_t, input: long */
  IU_fun  IU;      /* output: mpfi_t, input: unsigned long */
  /*JS_fun  JS;*/      /* output: mpfi_t, input: intmax_t */
  /*JU_fun  JU;*/      /* output: mpfi_t, input: uintmax_t */
  ID_fun  ID;      /* output: mpfi_t, input: double */
  IF_fun  IF;      /* output: mpfi_t, input: float */
  IL_fun  IL;      /* output: mpfi_t, input: long double */
  IZ_fun  IZ;      /* output: mpfi_t, input: mpz_t */
  IQ_fun  IQ;      /* output: mpfi_t, input: mpq_t */
  IR_fun  IR;      /* output: mpfi_t, input: mpfr_t */
  III_fun III;     /* output: mpfi_t, inputs: mpfi_t, mpfi_t */
  IIII_fun IIII;   /* output: mpfi_t, mpfi_t, inputs: mpfi_t, mpfi_t */
  IIU_fun IIU;     /* output: mpfi_t, inputs: mpfi_t, unsigned long */
  IIS_fun IIS;     /* output: mpfi_t, inputs: mpfi_t, signed long */
  IID_fun IID;     /* output: mpfi_t, inputs: mpfi_t, double */
  IIZ_fun IIZ;     /* output: mpfi_t, inputs: mpfi_t, mpz_t */
  IIQ_fun IIQ;     /* output: mpfi_t, inputs: mpfi_t, mpq_t */
  IIR_fun IIR;     /* output: mpfi_t, inputs: mpfi_t, mpfr_t */
  IUI_fun IUI;     /* output: mpfi_t, inputs: unsigned long, mpfi_t */
  ISI_fun ISI;     /* output: mpfi_t, inputs: signed long, mpfi_t */
  IDI_fun IDI;     /* output: mpfi_t, inputs: double, mpfi_t */
  IZI_fun IZI;     /* output: mpfi_t, inputs: mpz_t, mpfi_t */
  IQI_fun IQI;     /* output: mpfi_t, inputs: mpq_t, mpfi_t */
  IRI_fun IRI;     /* output: mpfi_t, inputs: mpfr_t, mpfi_t */
  RI_fun  RI;      /* output: mpfr_t, input: mpfi_t */
} mpfi_fun_ptr;

typedef union
{
  R_fun    I;      /* output: mpfr_t, no input */
  RR_fun   II;     /* output: mpfr_t, input: mpfr_t */
  RRR_fun  III;    /* output: mpfr_t, inputs: mpfr_t, mpfr_t */
  RRRR_fun  IIII;  /* output: mpfr_t, inputs: mpfr_t, mpfr_t, mpfr_t */
  NULL_fun IS;     /* dummy, no corresponding mpfr function */
  NULL_fun IU;     /* dummy, no corresponding mpfr function */
  /*NULL_fun JS;*/     /* dummy, no corresponding mpfr function */
  /*NULL_fun JU;*/     /* dummy, no corresponding mpfr function */
  NULL_fun ID;     /* dummy, no corresponding mpfr function */
  NULL_fun IF;     /* dummy, no corresponding mpfr function */
  NULL_fun IL;     /* dummy, no corresponding mpfr function */
  NULL_fun IZ;     /* dummy, no corresponding mpfr function */
  NULL_fun IQ;     /* dummy, no corresponding mpfr function */
  NULL_fun IR;     /* dummy, no corresponding mpfr function */
  RRU_fun  IIU;    /* output: mpfr_t, inputs: mpfr_t, unsigned long */
  RRS_fun  IIS;    /* output: mpfr_t, inputs: mpfr_t, signed long */
  RRD_fun  IID;    /* output: mpfr_t, inputs: mpfr_t, double */
  RRZ_fun  IIZ;    /* output: mpfr_t, inputs: mpfr_t, mpz_t */
  RRQ_fun  IIQ;    /* output: mpfr_t, inputs: mpfr_t, mpq_t */
  RRR_fun  IIR;    /* output: mpfr_t, inputs: mpfr_t, mpfr_t */
  RUR_fun  IUI;    /* output: mpfr_t, inputs: unsigned long, mpfr_t */
  RSR_fun  ISI;    /* output: mpfr_t, inputs: signed long, mpfr_t */
  RDR_fun  IDI;    /* output: mpfr_t, inputs: double, mpfr_t */
  RZR_fun  IZI;    /* output: mpfr_t, inputs: mpz_t, mpfr_t */
  RQR_fun  IQI;    /* output: mpfr_t, inputs: mpq_t, mpfr_t */
  RRR_fun  IRI;    /* output: mpfr_t, inputs: mpfr_t, mpfr_t */
  NULL_fun RI;     /* dummy, no corresponding mpfr function */
} mpfi_fun_mpfr_ptr;

typedef enum
  {
    I,     /* no input */
    II,    /* one input: interval */
    IS,    /* one input: long */
    IU,    /* one input: unsigned long */
    /*JS,*/    /* one input: intmax_t */
    /*JU,*/    /* one input: uintmax_t */
    ID,    /* one input: double */
    IF,    /* one input: float */
    IL,    /* one input: long double */
    IZ,    /* one input: mpz_t */
    IQ,    /* one input: mpq_t */
    IR,    /* one input: mpfr_t */
    III,   /* two inputs: intervals */
    IIII,  /* three inputs: intervals */ /* may be modified into 2 inputs and 2 outputs */
    IIU,   /* two inputs: interval, unsigned long */
    IIS,   /* two inputs: interval, signed long */
    IID,   /* two inputs: interval, double */
    IIZ,   /* two inputs: interval, mpz_t */
    IIQ,   /* two inputs: interval, mpq_t */
    IIR,   /* two inputs: interval, mpfr_t */
    IUI,   /* two inputs: unsigned long, interval */
    ISI,   /* two inputs: signed long, interval */
    IDI,   /* two inputs: double, interval */
    IZI,   /* two inputs: mpz_t, interval */
    IQI,   /* two inputs: mpq_t, interval */
    IRI,   /* two inputs: mpfr_t, interval */
    RI,    /* one input: interval */
  } mpfi_fun_type;

typedef union {
  int           i;
  unsigned long ui;
  signed long   si;
/*
  uintmax_t 	uj;
  intmax_t 	sj;
*/
  double        d;
  float         f;
  long double   ld;
  mpz_t         mpz;
  mpq_t         mpq;
  mpfr_t        mpfr;
  mpfi_t        mpfi;
} mpfi_fun_operand_t;

struct mpfi_function_t;
typedef struct mpfi_function_t* mpfi_function_ptr;

struct mpfi_function_t
{
  mpfi_fun_type       type;
  mpfi_fun_ptr        func;
  mpfi_fun_mpfr_ptr   mpfr_func; /* associated MPFR function */
  mpfi_fun_operand_t* operands;
  I_fun               random_domain;

  void (*set_prec)   (mpfi_function_ptr, mpfr_prec_t);
  void (*read_line)  (mpfi_function_ptr, FILE *);
  void (*check_line) (mpfi_function_ptr);
  void (*random)     (mpfi_function_ptr);
  void (*clear)      (mpfi_function_ptr);
};


/* helper macro to abstract (to mask) mpfi_function_t type */

#define MPFI_FUN_TYPE(_mpfi_function)       (_mpfi_function).type
#define MPFI_FUN_GET(_mpfi_function, _type) (_mpfi_function).func._type
#define MPFI_FUN_MPFR_FUNCTION(_mpfi_function, _type)   \
  (_mpfi_function).mpfr_func._type
#define MPFI_FUN_ARGS(_mpfi_function)           \
  ((_mpfi_function).operands)
#define MPFI_FUN_ARG(_mpfi_function, _arg_no, _arg_type)        \
  ((_mpfi_function).operands[(_arg_no)]._arg_type)


/* Helper functions */

#ifdef __cplusplus
extern "C" {
#endif

/* public functions.
   when adding a generic test, use the following functions: */
void test_start         (void);
void test_end           (void);
void check_data         (mpfi_function_ptr, const char *);
void check_random       (mpfi_function_ptr, mpfr_prec_t, mpfr_prec_t, int);

void mpfi_fun_init_I    (mpfi_function_ptr, I_fun, R_fun);
void mpfi_fun_init_II   (mpfi_function_ptr, II_fun, RR_fun);
void mpfi_fun_init_IU   (mpfi_function_ptr, IU_fun, NULL_fun);
void mpfi_fun_init_IS   (mpfi_function_ptr, IS_fun, NULL_fun);
/*
void mpfi_fun_init_JU   (mpfi_function_ptr, JU_fun, NULL_fun);
void mpfi_fun_init_JS   (mpfi_function_ptr, JS_fun, NULL_fun);
*/
void mpfi_fun_init_ID   (mpfi_function_ptr, ID_fun, NULL_fun);
void mpfi_fun_init_IF   (mpfi_function_ptr, IF_fun, NULL_fun);
void mpfi_fun_init_IL   (mpfi_function_ptr, IL_fun, NULL_fun);
void mpfi_fun_init_IZ   (mpfi_function_ptr, IZ_fun, NULL_fun);
void mpfi_fun_init_IQ   (mpfi_function_ptr, IQ_fun, NULL_fun);
void mpfi_fun_init_IR   (mpfi_function_ptr, IR_fun, NULL_fun);
void mpfi_fun_init_III  (mpfi_function_ptr, III_fun, RRR_fun);
void mpfi_fun_init_IIII (mpfi_function_ptr, IIII_fun, RRRR_fun);
void mpfi_fun_init_IIU  (mpfi_function_ptr, IIU_fun, RRU_fun);
void mpfi_fun_init_IIS  (mpfi_function_ptr, IIS_fun, RRS_fun);
void mpfi_fun_init_IID  (mpfi_function_ptr, IID_fun, RRD_fun);
void mpfi_fun_init_IIZ  (mpfi_function_ptr, IIZ_fun, RRZ_fun);
void mpfi_fun_init_IIQ  (mpfi_function_ptr, IIQ_fun, RRQ_fun);
void mpfi_fun_init_IIR  (mpfi_function_ptr, IIR_fun, RRR_fun);
void mpfi_fun_init_IUI  (mpfi_function_ptr, IUI_fun, RUR_fun);
void mpfi_fun_init_ISI  (mpfi_function_ptr, ISI_fun, RSR_fun);
void mpfi_fun_init_IDI  (mpfi_function_ptr, IDI_fun, RDR_fun);
void mpfi_fun_init_IZI  (mpfi_function_ptr, IZI_fun, RZR_fun);
void mpfi_fun_init_IQI  (mpfi_function_ptr, IQI_fun, RQR_fun);
void mpfi_fun_init_IRI  (mpfi_function_ptr, IRI_fun, RRR_fun);
void mpfi_fun_init_RI   (mpfi_function_ptr, RI_fun, NULL_fun);
void mpfi_fun_clear     (mpfi_function_ptr);

void mpfi_restrict_random (mpfi_function_ptr, I_fun);

/* internal functions */

extern gmp_randstate_t  rands;
extern char             rands_initialized;
void   random_interval  (mpfi_ptr);
unsigned long random_ui ();
long   random_si        ();
double random_double    ();
void   random_mpz       (mpz_ptr, unsigned long);
void   random_mpq       (mpq_ptr);
void   random_mpfr      (mpfr_ptr);

int  same_mpfr_value    (mpfr_srcptr, mpfr_srcptr);
int  same_value         (mpfi_srcptr, mpfi_srcptr);

FILE* open_file         (const char *);
void init_reading       (FILE*);
void close_file         (FILE*);
void skip_whitespace_comments (FILE*);
void read_sign          (FILE*, int*);
void read_exactness     (FILE*, int*);
void read_ui            (FILE*, unsigned long*);
void read_si            (FILE*, long*);
/*
void read_uj            (FILE*, uintmax_t*);
void read_sj            (FILE*, intmax_t*);
*/
int  read_double        (FILE*, double*);
int  read_float         (FILE*, float*);
int  read_long_double   (FILE*, long double*);
void read_mpz           (FILE*, mpz_ptr);
void read_mpq           (FILE*, mpq_ptr);
void read_mpfr          (FILE*, mpfr_ptr);
void read_mpfi          (FILE*, mpfi_ptr);

void check_with_different_prec (mpfi_function_ptr, mpfr_prec_t);

#ifdef __cplusplus
}
#endif

#endif /*  __MPFI_TESTS_H__ */
