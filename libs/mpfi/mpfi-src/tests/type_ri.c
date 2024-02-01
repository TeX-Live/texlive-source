/* type_ri.c -- Test functions associated with functions of the type
                mpfi_f (mpfr_t, mpfi_t).

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
read_line_ri (mpfi_function_ptr this, FILE* fp)
{
  test_line_number = line_number;
  /* [1] return value */
  read_sign (fp, &(MPFI_FUN_ARG (*this, 1, i)));
  /* [2] expected value  (mpfr_t)*/
  read_mpfr (fp, MPFI_FUN_ARG (*this, 2, mpfr));
  /* [3] operand */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 3, mpfi));
}

void
check_line_ri (mpfi_function_ptr this)
{
  int ret;

  /* rename operands for better readability */
  RI_fun  f_RI      = MPFI_FUN_GET (*this, RI);
  mpfr_ptr got      = MPFI_FUN_ARG (*this, 0, mpfr);
  int expected_ret  = MPFI_FUN_ARG (*this, 1, i);
  mpfr_ptr expected = MPFI_FUN_ARG (*this, 2, mpfr);
  mpfi_ptr op       = MPFI_FUN_ARG (*this, 3, mpfi);

  mpfr_set_prec (got, mpfr_get_prec (expected));
  ret = f_RI (got, op);
  if ((ret != expected_ret && ret * expected_ret <= 0)
      || !same_mpfr_value (got, expected)) {
    printf ("Failed line %lu.\nop = ", test_line_number);
    mpfi_out_str (stdout, 16, 0, op);
    printf ("\ngot      = ");
    mpfr_out_str (stdout, 16, 0, got, MPFI_RNDD);
    printf ("\nexpected = ");
    mpfr_out_str (stdout, 16, 0, expected, MPFI_RNDD);
    putchar ('\n');
    if (ret != expected_ret || ret * expected_ret < 0) {
      printf ("return value: got = %d, %s expected\n", ret,
              expected_ret < 0 ? "negative"
              : expected_ret > 0 ? "positive" : "zero");
    }

    exit (1);
  }
}

void
set_prec_ri (mpfi_function_ptr this, mpfr_prec_t prec)
{
  mpfr_set_prec (MPFI_FUN_ARG (*this, 0, mpfr), prec);
  mpfr_set_prec (MPFI_FUN_ARG (*this, 2, mpfr), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 3, mpfi), prec);
}

void
clear_ri (mpfi_function_ptr this)
{
  /* [0] result (mpfr_t) */
  mpfr_clear (MPFI_FUN_ARG (*this, 0, mpfr));
  /* [1] return value (int), needs no deallocation */
  /* [2] expected value (mpfr_t) */
  mpfr_clear (MPFI_FUN_ARG (*this, 2, mpfr));
  /* [3] operand (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 3, mpfi));

  free (MPFI_FUN_ARGS (*this));
  MPFI_FUN_ARGS (*this) = NULL;
}

/* In operands array, variables are in the same order as for data in
   '.dat' files plus one additional variable before them. */
void
mpfi_fun_init_RI (mpfi_function_ptr this, RI_fun mpfi_function,
              NULL_fun mpfr_function)
{
  this->type = RI;
  this->func.RI = mpfi_function;
  this->mpfr_func.RI = mpfr_function;
  this->random_domain = NULL;

  MPFI_FUN_ARGS (*this) =
    (mpfi_fun_operand_t*) malloc (4 * sizeof (mpfi_fun_operand_t));

  /* [0] result (mpfr_t) */
  mpfr_init2 (MPFI_FUN_ARG (*this, 0, mpfr), 1024);
  /* [1] return value (int), needs no initialization */
  /* [2] expected value (mpfr_t) */
  mpfr_init2 (MPFI_FUN_ARG (*this, 2, mpfr), 1024);
  /* [3] operand (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 3, mpfi), 1024);

  /* init methods */
  this->set_prec   = set_prec_ri;
  this->read_line  = read_line_ri;
  this->check_line = check_line_ri;
  this->random     = NULL;
  this->clear      = clear_ri;
}
