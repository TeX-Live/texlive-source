/* tinterv_fr.c -- Test mpfi_interv_fr.

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
check (mpfi_ptr i, mpfr_srcptr a, mpfr_srcptr b,
       mpfr_srcptr expected_left, mpfr_srcptr expected_right,
       int expected_inex)
{
  int inex;

  inex = mpfi_interv_fr (i, a, b);
  if (inex != expected_inex) {
    printf ("Error: mpfi_interv_fr (i, a, b) returns %d instead of %d\n",
            inex, expected_inex);
    printf ("precision(i) = %lu\na =", mpfi_get_prec (i));
    mpfr_out_str (stdout, 10, 0, a, MPFI_RNDD);
    printf ("\nb = ");
    mpfr_out_str (stdout, 10, 0, b, MPFI_RNDU);
    printf ("\n");
    exit (1);
  }
  if (!same_mpfr_value (&(i->left), expected_left)
      || !same_mpfr_value (&(i->right), expected_right)) {
    printf ("Error: mpfi_interv_fr (i, a, b) failed.\n");
    printf ("\na = ");
    mpfr_out_str (stdout, 10, 0, a, MPFI_RNDD);
    printf ("\nb = ");
    mpfr_out_str (stdout, 10, 0, b, MPFI_RNDU);
    printf ("\ngot    i = ");
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
  mpfr_t minf, pinf;
  mpfr_t mzero, pzero;
  mpfr_t nan;
  mpfr_t x, y;
  mpfr_t xx, yy;

  mpfi_t i;

  mpfi_init2 (i, 53);
  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);
  mpfr_init2 (xx, 53);
  mpfr_init2 (yy, 53);
  mpfr_init2 (minf, 53);
  mpfr_init2 (pinf, 53);
  mpfr_init2 (mzero, 53);
  mpfr_init2 (pzero, 53);
  mpfr_init2 (nan, 53);

  mpfr_set_nan (nan);
  mpfr_set_inf (minf, -1);
  mpfr_set_inf (pinf, +1);
  mpfr_set_ui (pzero, 0, MPFI_RNDU);
  mpfr_neg (mzero, pzero, MPFI_RNDD);

  /* special values */
  check (i, minf, minf, minf, minf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, minf, mzero, minf, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, mzero, minf, minf, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, minf, pzero, minf, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pzero, minf, minf, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, minf, pinf, minf, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pinf, minf, minf, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, minf, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, minf, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  check (i, mzero, mzero, pzero, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, mzero, pzero, pzero, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pzero, mzero, pzero, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, mzero, pinf, pzero, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pinf, mzero, pzero, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, mzero, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, mzero, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  check (i, pzero, pzero, pzero, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pzero, pzero, pzero, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pzero, pinf, pzero, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pinf, pzero, pzero, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pzero, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, pzero, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  check (i, pinf, pinf, pinf, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pinf, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, pinf, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  check (i, nan, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  /* regular values */
  mpfr_set_si (x, -456789, MPFI_RNDD);
  mpfr_set_si (y, 123456, MPFI_RNDU);
  check (i, x, y, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, y, x, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, x, minf, minf, x, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, minf, x, minf, x, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, x, mzero, x, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, mzero, x, x, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, y, mzero, pzero, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, mzero, y, pzero, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, x, pzero, x, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pzero, x, x, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, y, pzero, pzero, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pzero, y, pzero, y, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, x, pinf, x, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pinf, x, x, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, y, pinf, y, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, pinf, y, y, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, x, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, x, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, y, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, x, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfr_set_si (x, -400009, MPFI_RNDD);
  mpfr_set_si (y, 100007, MPFI_RNDU);
  mpfi_set_prec (i, 12);
  mpfr_set_prec (xx, 12);
  mpfr_set_prec (yy, 12);
  mpfr_set (xx, x, MPFI_RNDD);
  mpfr_set (yy, y, MPFI_RNDU);
  check (i, x, y, xx, yy, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT);
  check (i, y, x, xx, yy, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT);
  mpfr_set (xx, x, MPFI_RNDU);
  check (i, x, minf, minf, xx, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, minf, x, minf, xx, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, y, minf, minf, yy, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, minf, y, minf, yy, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  mpfr_set (xx, x, MPFI_RNDD);
  check (i, x, mzero, xx, mzero, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, mzero, x, xx, mzero, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, y, mzero, pzero, yy, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, mzero, y, pzero, yy, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, x, pzero, xx, mzero, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, pzero, x, xx, mzero, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, y, pzero, pzero, yy, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, pzero, y, pzero, yy, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, x, pinf, xx, pinf, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, pinf, x, xx, pinf, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  mpfr_set (yy, y, MPFI_RNDD);
  check (i, y, pinf, yy, pinf, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, pinf, y, yy, pinf, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT);
  check (i, x, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, x, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, y, nan, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  check (i, nan, y, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_clear (i);
  mpfr_clear (x);
  mpfr_clear (y);
  mpfr_clear (xx);
  mpfr_clear (yy);
  mpfr_clear (minf);
  mpfr_clear (pinf);
  mpfr_clear (mzero);
  mpfr_clear (pzero);
  mpfr_clear (nan);

  return 0;
}
