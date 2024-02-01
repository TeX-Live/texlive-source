/* tget_endpoints.c -- Test mpfi_get_left and mpfi_get_right.

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
error_message (int side, mpfi_srcptr i, mpfr_ptr got)
{
  unsigned long pi, pgot;
  pi = mpfi_get_prec (i);
  pgot = mpfr_get_prec (got);

  printf ("Error: mpfi_get_%s(x, I) returns wrong value.\n"
          "Precision_I   = %lu. I = ", side ? "right" : "left", pi);
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\nPrecision_got = %lu. Got: ", pgot);
  mpfr_out_str (stdout, 10, 0, got, MPFI_RNDD);
  printf ("\n");
  exit (1);
}

void
test_special ()
{
  mpfi_t i;
  mpfr_t l, r;
  mpfr_t expected;

  mpfi_init2 (i, 53);
  mpfr_init2 (l, 53);
  mpfr_init2 (r, 53);
  mpfr_init2 (expected, 53);

  /* [nan, nan] */
  mpfr_set_nan (expected);
  mpfi_set_fr (i, expected);
  mpfi_get_left (l, i);
  if (!same_mpfr_value (l, expected))
    error_message (0, i, l);
  mpfi_get_right (r, i);
  if (!same_mpfr_value (r, expected))
    error_message (1, i, r);

  /* [-17, nan] */
  mpfr_set_si (expected, -17, MPFI_RNDD);
  mpfr_set (&(i->left), expected, MPFI_RNDD);
  mpfi_get_right (r, i);
  if (!same_mpfr_value (r, l))
    error_message (1, i, r);
  mpfi_get_left (l, i);
  if (!same_mpfr_value (l, expected))
    error_message (0, i, l);

  /* [-inf, -inf] */
  mpfr_set_inf (expected, -1);
  mpfi_set_fr (i, expected);
  mpfi_get_left (l, i);
  if (!same_mpfr_value (l, expected))
    error_message (0, i, l);
  mpfi_get_right (r, i);
  if (!same_mpfr_value (r, expected))
    error_message (1, i, r);

  /* [-inf, 17] */
  mpfr_set_ui (expected, 17, MPFI_RNDU);
  mpfr_set (&(i->right), expected, MPFI_RNDU);
  mpfi_get_left (l, i);
  if (!same_mpfr_value (l, r))
    error_message (0, i, l);
  mpfi_get_right (r, i);
  if (!same_mpfr_value (r, expected))
    error_message (1, i, r);

  /* [+0, +inf] */
  mpfr_set_ui (&(i->left), 0, MPFI_RNDD);
  mpfr_set_inf (expected, +1);
  mpfr_set (&(i->right), expected, MPFI_RNDU);
  mpfi_get_left (l, i);
  if (mpfr_cmp_ui (l, 0) != 0)
    error_message (0, i, l);
  mpfi_get_right (r, i);
  if (!same_mpfr_value (r, expected))
    error_message (1, i, r);

  /* [+inf, +inf] */
  mpfi_set_fr (i, expected);
  mpfi_get_left (l, i);
  if (!same_mpfr_value (l, expected))
    error_message (0, i, l);
  mpfi_get_right (r, i);
  if (!same_mpfr_value (r, expected))
    error_message (1, i, r);

  /* [-inf, +inf] */
  mpfr_set_inf (&(i->left), -1);
  mpfr_set_inf (&(i->right), +1);
  mpfi_get_left (l, i);
  mpfr_set_inf (expected, -1);
  if (!same_mpfr_value (l, expected))
    error_message (0, i, l);
  mpfi_get_right (r, i);
  mpfr_set_inf (expected, +1);
  if (!same_mpfr_value (r, expected))
    error_message (1, i, r);

  /* [+0, -0] */
  mpfr_set_ui (expected, 0, MPFI_RNDU);
  mpfi_set_fr (i, expected);
  mpfi_get_left (l, i);
  if (!same_mpfr_value (l, expected))
    error_message (0, i, l);
  mpfi_get_right (r, i);
  mpfr_neg (expected, expected, MPFI_RNDD);
  if (!same_mpfr_value (r, expected))
    error_message (1, i, r);

  mpfi_clear (i);
  mpfr_clear (l);
  mpfr_clear (r);
  mpfr_clear (expected);
}

void
test_regular ()
{
  mpfi_t i;
  mpfr_t got, expected;

  mpfi_init2 (i, 53);
  mpfr_init2 (got, 500);
  mpfr_init2 (expected, 53);

  mpfi_interv_ui (i, 3, 5);
  mpfi_inv (i, i);

  mpfr_set_ui (expected, 5, MPFI_RNDU);
  mpfr_ui_div (expected, 1, expected, MPFI_RNDD);
  mpfi_get_left (got, i);
  if (!same_mpfr_value (got, expected))
    error_message (0, i, got);

  mpfr_set_prec (got, 53);
  mpfi_get_left (got, i);
  if (!same_mpfr_value (got, expected))
    error_message (0, i, got);

  mpfr_set_prec (expected, 3);
  mpfr_set_prec (got, 3);
  mpfr_set_ui (expected, 5, MPFI_RNDU);
  mpfr_ui_div (expected, 1, expected, MPFI_RNDD);
  mpfi_get_left (got, i);
  if (!same_mpfr_value (got, expected))
    error_message (0, i, got);

  mpfr_set_ui (expected, 3, MPFI_RNDD);
  mpfr_ui_div (expected, 1, expected, MPFI_RNDU);
  mpfi_get_right (got, i);
  if (!same_mpfr_value (got, expected))
    error_message (1, i, got);

  mpfr_set_prec (expected, 53);
  mpfr_set_prec (got, 53);
  mpfr_set_ui (expected, 3, MPFI_RNDD);
  mpfr_ui_div (expected, 1, expected, MPFI_RNDU);
  mpfi_get_right (got, i);
  if (!same_mpfr_value (got, expected))
    error_message (1, i, got);

  mpfr_set_prec (got, 500);
  mpfi_get_right (got, i);
  if (!same_mpfr_value (got, expected))
    error_message (1, i, got);

  mpfi_clear (i);
  mpfr_clear (got);
  mpfr_clear (expected);
}

int
main (int argc, char **argv)
{
  test_special ();
  test_regular ();

  return 0;
}
