/* type_i.c -- Test functions associated with functions of the type
               mpfi_f (mpfi_t).

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

extern unsigned long line_number;
static unsigned long test_line_number;   /* start line of a test */

void
read_line_i (mpfi_function_ptr this, FILE* fp)
{
  test_line_number = line_number;
  /* [1] return value */
  read_exactness (fp, &(MPFI_FUN_ARG (*this, 1, i)));
  /* [2] expected value */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 2, mpfi));
}

void
check_monotonicity (mpfi_function_ptr this)
{
  mpfr_prec_t precision;
  mpfr_prec_t initial_precision;

  /* rename operands for better readability */
  I_fun  f_I        = MPFI_FUN_GET (*this, I);
  mpfi_ptr got      = MPFI_FUN_ARG (*this, 0, mpfi);
  mpfi_ptr expected = MPFI_FUN_ARG (*this, 2, mpfi);

  initial_precision = mpfi_get_prec (expected);

  for (precision = initial_precision; precision > 1; precision >>= 2) {
    mpfi_set_prec (got, precision);

    f_I (got);

    if (!mpfi_is_inside (expected, got)) {
      printf ("Failed at precision %lu.\n", precision);
      mpfi_out_str (stdout, 2, 0, got);
      printf ("\ndoes not include ");
      mpfi_out_str (stdout, 2, 0, expected);
      putchar ('\n');

      exit (1);
    }
  }
}

void
check_line_i (mpfi_function_ptr this)
{
  int inex;

  /* rename operands for better readability */
  I_fun  f_I        = MPFI_FUN_GET (*this, I);
  mpfi_ptr got      = MPFI_FUN_ARG (*this, 0, mpfi);
  int expected_inex = MPFI_FUN_ARG (*this, 1, i);
  mpfi_ptr expected = MPFI_FUN_ARG (*this, 2, mpfi);

  mpfi_set_prec (got, mpfi_get_prec (expected));

  inex = f_I (got);
  if (inex != expected_inex || !same_value (got, expected)) {
    printf ("Failed line %lu.", test_line_number);
    printf ("\ngot      = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nexpected = ");
    mpfi_out_str (stdout, 16, 0, expected);
    putchar ('\n');
    if (inex != expected_inex)
      printf ("inexact flag: got = %u, expected = %u\n",
              inex, expected_inex);

    exit (1);
  }

  /* monotonic? */
  if (!MPFI_NAN_P (expected)) {
    check_monotonicity (this);
  }
}

void
set_prec_i (mpfi_function_ptr this, mpfr_prec_t prec)
{
  mpfi_set_prec (MPFI_FUN_ARG (*this, 0, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 2, mpfi), prec);
}

void
clear_i (mpfi_function_ptr this)
{
  /* [0] initial value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 0, mpfi));
  /* [1] return value (int), needs no deallocation */
  /* [2] expected value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 2, mpfi));

  free (MPFI_FUN_ARGS (*this));
  MPFI_FUN_ARGS (*this) = NULL;
}

void
mpfi_fun_init_I (mpfi_function_ptr this, I_fun mpfi_function,
        R_fun mpfr_function)
{
  this->type = I;
  this->func.I = mpfi_function;
  this->mpfr_func.I = mpfr_function;
  this->random_domain = NULL;

  /* init operands */
  MPFI_FUN_ARGS (*this) =
    (mpfi_fun_operand_t*) malloc (3 * sizeof (mpfi_fun_operand_t));

  /* [0] initial value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 0, mpfi), 1024);
  /* [1] return value (int), needs no initialization */
  /* [2] expected value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 2, mpfi), 1024);

  /* init methods */
  this->set_prec   = set_prec_i;
  this->read_line  = read_line_i;
  this->check_line = check_line_i;
  this->random     = NULL;
  this->clear      = clear_i;
}
