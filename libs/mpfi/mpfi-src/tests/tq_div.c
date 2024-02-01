/* tq_div.c -- Test mpfi_q_div.

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

void
error_message (int inex, int expected_inex, mpfi_ptr got,
               mpq_ptr op1, mpfi_ptr op2, mpfi_ptr expected)
{
  printf ("Failed when checking overflow.\nop1 = ");
  mpq_out_str (stdout, 10, op1);
  printf ("\nop2 = ");
  mpfi_out_str (stdout, 10, 0, op2);
  printf ("\ngot      = ");
  mpfi_out_str (stdout, 10, 0, got);
  printf ("\nexpected = ");
  mpfi_out_str (stdout, 10, 0, expected);
  putchar ('\n');
  if (inex != expected_inex)
    printf ("inexact flag: got = %u, expected = %u\n", inex, expected_inex);
  exit (1);
}

void
check_overflow (struct mpfi_function_t *i_q_div)
{
  mpfi_ptr got      = MPFI_FUN_ARG (*i_q_div, 0, mpfi);
  mpfi_ptr expected = MPFI_FUN_ARG (*i_q_div, 2, mpfi);
  mpq_ptr op1       = MPFI_FUN_ARG (*i_q_div, 3, mpq);
  mpfi_ptr op2      = MPFI_FUN_ARG (*i_q_div, 4, mpfi);
  int inex;
  mpfr_exp_t emin;
  mpfr_exp_t emax;

  emin = mpfr_get_emin ();
  emax = mpfr_get_emax ();
  if (-emax < emin - 1)
    return;

  mpfi_set_prec (got, 53);
  mpfi_set_prec (expected, 53);
  mpfi_set_prec (op2, 53);

  /* op2 = [2^-emax, 1] */
  mpfr_set_ui_2exp (&(op2->left), 1, -emax, MPFI_RNDD);
  mpfr_set_ui (&(op2->right), 1, MPFI_RNDD);
 /* op1 = 6 */
  mpq_set_ui (op1, 6, 1);
  inex = mpfi_q_div (got, op1, op2);
  mpfr_set_ui (&(expected->left), 6, MPFI_RNDD);
  mpfr_set_inf (&(expected->right), +1);
  if (inex != 2 || !same_value (got, expected)) {
    error_message (inex, 2, got, op1, op2, expected);
  }

 /* op1 = 17/3 ~ 6 */
  mpq_set_ui (op1, 17, 3);
  inex = mpfi_q_div (got, op1, op2);
  mpfr_set_q (&(expected->left), op1, MPFI_RNDD);
  if (inex != 3 || !same_value (got, expected)) {
    error_message (inex, 3, got, op1, op2, expected);
  }

  /* op2 = [-1, -2^-emax] */
  mpfi_neg (op2, op2);
  inex = mpfi_q_div (got, op1, op2);
  mpfr_set_inf (&(expected->left), -1);
  mpq_neg (op1, op1);
  mpfr_set_q (&(expected->right), op1, MPFI_RNDU);
  if (inex != 3 || !same_value (got, expected)) {
    error_message (inex, 3, got, op1, op2, expected);
  }

  /* op1 = 6 */
  mpq_set_ui (op1, 6, 1);
  inex = mpfi_q_div (got, op1, op2);
  mpfr_set_si (&(expected->right), -6, MPFI_RNDU);
  if (inex != 1 || !same_value (got, expected)) {
    error_message (inex, 1, got, op1, op2, expected);
  }
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_q_div;

  mpfi_fun_init_IQI (&i_q_div, mpfi_q_div, NULL);

  check_data (&i_q_div, "q_div.dat");
  check_overflow (&i_q_div);

  mpfi_fun_clear (&i_q_div);

  return 0;
}
