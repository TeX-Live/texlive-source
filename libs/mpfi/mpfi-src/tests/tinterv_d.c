/* tinterv_d.c -- Test mpfi_interv_d.

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
check (mpfi_ptr i, double a, double b,
       mpfr_srcptr expected_left, mpfr_srcptr expected_right,
       int expected_inex)
{
  int inex;

  inex = mpfi_interv_d (i, a, b);
  if (inex != expected_inex) {
    printf ("Error: mpfi_interv_d (i, a, b) returns %d instead of %d\n",
            inex, expected_inex);
    printf ("precision(i) = %lu\na = %g\nb = %g\n", mpfi_get_prec (i), a, b);
    exit (1);
  }
  if (!same_mpfr_value (&(i->left), expected_left)
      || !same_mpfr_value (&(i->right), expected_right)) {
    printf ("Error: mpfi_interv_d (i, a, b) failed.\n");
    printf ("\na = %g\nb = %g\ngot    i = ", a, b);
    mpfi_out_str (stdout, 10, 0, i);
    printf ("\nexpected = [");
    mpfr_out_str (stdout, 10, 0, expected_left, MPFI_RNDD);
    printf (", ");
    mpfr_out_str (stdout, 10, 0, expected_right, MPFI_RNDU);
    printf ("]\n");
    exit (1);
  }
}

int
main (int argc, char **argv)
{
  mpfr_t x, y;

  double a, b;
  mpfi_t i;

  mpfi_init2 (i, 53);
  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);

  a = -1;
  b = +1;
  mpfr_set_d (x, -1, MPFI_RNDD);
  mpfr_set_d (y, +1, MPFI_RNDU);
  check (i, a, b, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, b, a, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, a, a, x, x, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 4);
  mpfr_set_prec (x, 4);
  mpfr_set_prec (y, 4);
  a = -1;
  b = +40;
  mpfr_set_d (x, -1, MPFI_RNDD);
  mpfr_set_d (y, +40, MPFI_RNDU);
  check (i, a, b, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, b, a, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, a, a, x, x, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  a = -1;
  b = +42.35;
  mpfr_set_d (x, -1, MPFI_RNDD);
  mpfr_set_d (y, +42.35, MPFI_RNDU);
  check (i, a, b, x, y, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, b, a, x, y, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);

  a = -0.1;
  mpfr_set_d (x, -0.1, MPFI_RNDD);
  mpfr_set_d (y, +42.35, MPFI_RNDU);
  check (i, a, b, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT);
  check (i, b, a, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT);

  a = -1e-4;
  b = +16;
  mpfr_set_d (x, -1e-4, MPFI_RNDD);
  mpfr_set_d (y, +16, MPFI_RNDU);
  check (i, a, b, x, y, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, b, a, x, y, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);

  mpfr_set_nan (x);
  a = mpfr_get_d (x, MPFI_RNDD); /* a = NaN, if supported */
  check (i, a, b, y, x, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, b, a, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, a, a, x, x, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfr_set_ui (x, 0, MPFI_RNDD);
  mpfr_neg (x, x, MPFI_RNDD);
  a = mpfr_get_d (x, MPFI_RNDD); /* a = -0, if supported */
  mpfr_neg (x, x, MPFI_RNDU);
  check (i, a, b, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, b, a, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_clear (i);
  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}
