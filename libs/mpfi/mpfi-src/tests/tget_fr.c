/* tget_fr.c -- Test mpfi_get_fr.

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
error_message (mpfi_srcptr i, mpfr_ptr got, mpfr_ptr expected)
{
  printf ("Error: mpfi_get_fr(x, I) returns wrong value.\nI = ");
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\nGot: ");
  mpfr_out_str (stdout, 10, 0, got, MPFI_RNDD);
  printf ("\nExpected: ");
  mpfr_out_str (stdout, 10, 0, expected, MPFI_RNDD);
  printf ("\n");
  exit (1);
}

void
test_special ()
{
  mpfr_t expected, got;
  mpfi_t i;

  mpfr_init2 (expected, 53);
  mpfr_init2 (got, 53);
  mpfi_init2 (i, 53);

  /* [nan, nan] */
  mpfr_set_nan (expected);
  mpfi_set_fr (i, expected);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  /* [-17, nan] */
  mpfr_set_si (&(i->left), -17, MPFI_RNDD);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  /* [-inf, -inf] */
  mpfr_set_inf (expected, -1);
  mpfi_set_fr (i, expected);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  /* [-inf, 17] */
  mpfr_set_ui (&(i->right), 17, MPFI_RNDD);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  /* [+0, +inf] */
  mpfr_set_ui (&(i->left), 0, MPFI_RNDD);
  mpfr_set_inf (expected, +1);
  mpfr_set (&(i->right), expected, MPFI_RNDU);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  /* [+inf, +inf] */
  mpfi_set_fr (i, expected);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  /* [-inf, +inf] */
  mpfr_set_inf (&(i->left), -1);
  mpfr_set_inf (&(i->right), +1);
  mpfr_set_nan (expected);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  /* [+0, -0] */
  mpfr_set_ui (expected, 0, MPFI_RNDU);
  mpfi_set_fr (i, expected);
  mpfi_get_fr (got, i);
  if (!same_mpfr_value (expected, got))
    error_message (i, got, expected);

  mpfr_clear (expected);
  mpfr_clear (got);
  mpfi_clear (i);
}

void
test_random (mpfr_prec_t prec_min, mpfr_prec_t prec_max)
{
  mpfr_t x;
  mpfi_t i;
  mpfr_prec_t prec;
  unsigned long pi, px;

  mpfr_init2 (x, prec_max);
  mpfi_init2 (i, prec_max);

  for (prec = prec_min; prec < prec_max; prec++) {
    mpfi_set_prec (i, prec);
    mpfr_set_prec (x, prec);
    do {
      random_interval (i);
    } while (MPFI_INF_P (i));
    mpfi_get_fr (x, i);
    if (mpfi_is_inside_fr (x, i) == 0) {
      pi = mpfi_get_prec (i);
      px = mpfr_get_prec (x);
      printf ("Error: mpfi_get_fr (x, I) returns a value x outside the "
              "initial interval I, while the precisions are the same.\n"
              "precision_I: %lu, I = ", pi);
      mpfi_out_str (stdout, 10, 0, i);
      printf ("\nprecision_x: %lu, x = ", px);
      mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
      printf ("\n");
      exit (1);
    }
  }

  mpfr_clear (x);
  mpfi_clear (i);
}

int
main (int argc, char **argv)
{
  test_start ();
  test_special ();
  test_random (2, 1023);
  test_end ();

  return 0;
}
