/* type_ir.c -- Test functions associated with functions of the type
                mpfi_f (mpfi_t, mpfr_t).

Copyright 2010,
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

#include "mpfi-tests.h"

extern char * pathname;
extern unsigned long line_number;
static unsigned long test_line_number;   /* start line of a test */

void
read_line_ir (mpfi_function_ptr this, FILE* fp)
{
  mpfi_ptr got      = MPFI_FUN_ARG (*this, 0, mpfi);
  mpfi_ptr expected = MPFI_FUN_ARG (*this, 2, mpfi);

  test_line_number = line_number;
  /* [0] initial value */
  read_mpfi (fp, got);
  /* [1] return value */
  read_exactness (fp, &(MPFI_FUN_ARG (*this, 1, i)));
  /* [2] expected value */
  read_mpfi (fp, expected);
  /* [3] integral operand */
  read_mpfr (fp, MPFI_FUN_ARG (*this, 3, mpfr));

  /* data validation */
  if (mpfi_get_prec (got) != mpfi_get_prec (expected)) {
    printf ("Error in data file %s line %lu\nThe precisions of intervals "
            "are different.\n", pathname, test_line_number);
    exit (1);
  }
}

void
check_line_ir (mpfi_function_ptr this)
{
  int inex;

  /* rename operands for better readability */
  IR_fun  f_IR      = MPFI_FUN_GET (*this, IR);
  mpfi_ptr got      = MPFI_FUN_ARG (*this, 0, mpfi);
  int expected_inex = MPFI_FUN_ARG (*this, 1, i);
  mpfi_ptr expected = MPFI_FUN_ARG (*this, 2, mpfi);
  mpfr_ptr op       = MPFI_FUN_ARG (*this, 3, mpfr);

  inex = f_IR (got, op);
  if (inex != expected_inex || !same_value (got, expected)) {
    printf ("Failed line %lu.\nop = ", test_line_number);
    mpfr_out_str (stdout, 16, 0, op, MPFI_RNDD);
    printf ("\ngot      = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nexpected = ");
    mpfi_out_str (stdout, 16, 0, expected);
    putchar ('\n');
    if (inex != expected_inex)
      printf ("inexact flag: got = %u, expected = %u\n", inex, expected_inex);

    exit (1);
  }
}

void
set_prec_ir (mpfi_function_ptr this, mpfr_prec_t prec)
{
  mpfi_set_prec (MPFI_FUN_ARG (*this, 0, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 2, mpfi), prec);
  mpfr_set_prec (MPFI_FUN_ARG (*this, 3, mpfr), prec);
}

void
clear_ir (mpfi_function_ptr this)
{
  /* [0] initial value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 0, mpfi));
  /* [1] return value (int), needs no deallocation */
  /* [2] expected value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 2, mpfi));
  /* [3] operand (mpfr_t) */
  mpfr_clear (MPFI_FUN_ARG (*this, 3, mpfr));

  free (MPFI_FUN_ARGS (*this));
  MPFI_FUN_ARGS (*this) = NULL;
}

/* In operands array, variables are in the same order as for data in
   '.dat' files plus one additional variable before them. */
void
mpfi_fun_init_IR (mpfi_function_ptr this, IR_fun mpfi_function,
              NULL_fun mpfr_function)
{
  this->type = IR;
  this->func.IR = mpfi_function;
  this->mpfr_func.IR = mpfr_function;
  this->random_domain = NULL;

  MPFI_FUN_ARGS (*this) =
    (mpfi_fun_operand_t*) malloc (4 * sizeof (mpfi_fun_operand_t));

  /* [0] initial value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 0, mpfi), 1024);
  /* [1] return value (int), needs no initialization */
  /* [2] expected value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 2, mpfi), 1024);
  /* [3] operand (mpfr_t) */
  mpfr_init2 (MPFI_FUN_ARG (*this, 3, mpfr), 1024);

  /* init methods */
  this->set_prec   = set_prec_ir;
  this->read_line  = read_line_ir;
  this->check_line = check_line_ir;
  this->random     = NULL;
  this->clear      = clear_ir;
}
