/* tmul_ui.c -- Test mpfi_mul_ui.

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

  inexact = mpfi_mul_ui (a, a, 1024);

  if (!mpfr_inf_p (&(a->right))) {
    printf ("Error: mpfi_mul_ui does not correctly handle positive "
            "overflow.\n");
    exit (1);
  }

  if (!MPFI_RIGHT_IS_INEXACT (inexact)) {
    printf ("Error: mpfi_mul_ui does not return correct value when positive "
            "overflow.\n");
    exit (1);
  }

  mpfr_set_inf (max, -1);
  mpfr_nextabove (max);
  mpfr_set (&(a->left), max, MPFI_RNDD);
  mpfr_set_ui (&(a->right), 1, MPFI_RNDU);

  inexact = mpfi_mul_ui (a, a, 1024);

  if (!mpfr_inf_p (&(a->left))) {
    printf ("Error: mpfi_mul_ui does not correctly handle negative "
            "overflow.\n");
    exit (1);
  }

  if (!MPFI_LEFT_IS_INEXACT (inexact)) {
    printf ("Error: mpfi_mul_ui does not return correct value when negative "
            "overflow.\n");
    exit (1);
  }

  mpfi_clear (a);
  mpfr_clear (max);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_mul_ui;

  mpfi_fun_init_IIU (&i_mul_ui, mpfi_mul_ui, mpfr_mul_ui);

  test_start ();

  check_data (&i_mul_ui, "mul_ui.dat");
  check_random (&i_mul_ui, 2, 1000, 10);
  check_overflow ();

  test_end ();
  mpfi_fun_clear (&i_mul_ui);

  return 0;
}
