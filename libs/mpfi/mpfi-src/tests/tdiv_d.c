/* tdiv_d.c -- Test mpfi_div_d.

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
check_overflow ()
{
  mpfr_t max;
  mpfi_t a;
  int inexact;

  mpfi_init2 (a, 53);
  mpfr_init2 (max, 53);
  mpfr_set_ui (&(a->left), 1, MPFI_RNDD);
  mpfr_set_inf (max, +1);
  mpfr_nextbelow (max);
  mpfr_set (&(a->right), max, MPFI_RNDU);

  inexact = mpfi_div_d (a, a, 0.5);

  if (!mpfr_inf_p (&(a->right))) {
    printf ("Error: mpfi_div_d does not correctly handle positive "
            "overflow.\n");
    exit (1);
  }

  if (!MPFI_RIGHT_IS_INEXACT (inexact)) {
    printf ("Error: mpfi_div_d does not return correct value when positive "
            "overflow.\n");
    exit (1);
  }

  mpfr_set_inf (max, -1);
  mpfr_nextabove (max);
  mpfr_set (&(a->left), max, MPFI_RNDD);
  mpfr_set_ui (&(a->right), 1, MPFI_RNDU);

  inexact = mpfi_div_d (a, a, 0.5);

  if (!mpfr_inf_p (&(a->left))) {
    printf ("Error: mpfi_div_d does not correctly handle negative "
            "overflow.\n");
    exit (1);
  }

  if (!MPFI_LEFT_IS_INEXACT (inexact)) {
    printf ("Error: mpfi_div_d does not return correct value when negative "
            "overflow.\n");
    exit (1);
  }

  mpfi_clear (a);
  mpfr_clear (max);
}

void
check_underflow (void)
{
  mpfi_t got, expected;
  double d = 0x1p+100;
  int inex;

  mpfi_init2 (got, 128);
  mpfi_init2 (expected, 128);

  mpfi_set_ui (expected, 0);
  mpfr_nextbelow (&(expected->left));
  mpfr_nextabove (&(expected->right));

  inex = mpfi_div_d (got, expected, d);
  if (!MPFI_BOTH_ARE_INEXACT(inex)
      || !same_mpfr_value (&(got->left), &(expected->left))
      || !same_mpfr_value (&(got->right), &(expected->right))) {
    printf ("Error: mpfi_div_d (rop, op, d) does not return correct value\n"
            "op = ");
    mpfi_out_str (stdout, 16, 0, expected);
    printf ("\nd  = %a\ngot      = ", d);
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nexpected = ");
    mpfi_out_str (stdout, 16, 0, expected);
    printf ("\n");

    if (!MPFI_BOTH_ARE_INEXACT(inex)) {
      printf ("return value = %d\nexpected     = %d\n", inex,
              MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT);
    }

    exit (1);
  }

  mpfi_clear (got);
  mpfi_clear (expected);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_div_d;

  mpfi_fun_init_IID (&i_div_d, mpfi_div_d, mpfr_div_d);
  test_start ();

  check_data (&i_div_d, "div_d.dat");
  check_random (&i_div_d, 2, 1000, 10);
  check_overflow ();
  check_underflow ();

  test_end ();
  mpfi_fun_clear (&i_div_d);

  return 0;
}
