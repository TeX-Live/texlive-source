/* td_sub.c -- Test mpfi_d_sub.

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
  mpfi_t got;
  mpfi_t op;
  double d;
  int inexact;

  mpfi_init2 (got, 53);
  mpfi_init2 (op, 53);

  d = -1024.0;
  mpfr_set_ui (&(op->left), 1, MPFI_RNDD);
  mpfr_set_inf (&(op->right), +1);
  mpfr_nextbelow (&(op->right));

  inexact = mpfi_d_sub (got, d, op);

  if (!MPFI_LEFT_IS_INEXACT (inexact)
      || !mpfr_inf_p (&(got->left))
      || MPFI_RIGHT_IS_INEXACT (inexact)
      || mpfr_cmp_d (&(got->right), d - 1) != 0) {
    printf ("Error: mpfi_d_div (rop, %g, op) does not correctly handle "
            "overflow.\n op = ", d);
    mpfi_out_str (stdout, 10, 0, op);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 10, 0, got);
    printf ("\nreturn value = %d\n", inexact);
    exit (1);
  }

  d = +1024.0;
  mpfi_neg (op, op);

  inexact = mpfi_d_sub (got, d, op);

  if (MPFI_LEFT_IS_INEXACT (inexact)
      || mpfr_cmp_d (&(got->left), d + 1) != 0
      || !MPFI_RIGHT_IS_INEXACT (inexact)
      || !mpfr_inf_p (&(got->right))) {
    printf ("Error: mpfi_d_div (rop, %g, op) does not correctly handle "
            "overflow.\n op = ", d);
    mpfi_out_str (stdout, 10, 0, op);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 10, 0, got);
    printf ("\nreturn value = %d\n", inexact);
    exit (1);
  }

  mpfi_clear (op);
  mpfi_clear (got);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_d_sub;

  mpfi_fun_init_IDI (&i_d_sub, mpfi_d_sub, mpfr_d_sub);
  test_start ();

  check_data (&i_d_sub, "d_sub.dat");
  check_random (&i_d_sub, 2, 1000, 10);
  check_overflow ();

  test_end ();
  mpfi_fun_clear (&i_d_sub);

  return 0;
}
