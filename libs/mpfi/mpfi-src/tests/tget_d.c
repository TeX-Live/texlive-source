/* tget_d.c -- Test mpfi_get_d.

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

#include <math.h>
#include "mpfi-tests.h"

#ifndef isnan
# define isnan(x) ((x) != (x))
#endif

#ifndef isinf
# define isinf(x) (!isnan (x) && isnan ((x) - (x)))
#endif

void
error_message (mpfi_srcptr i, double got, const char *expected)
{
  printf ("Error: mpfi_get_d(x, I) returns wrong value.\nI = ");
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\ngot     : %f\nexpected: %s\n", got, expected);
  exit (1);
}

void
test_special ()
{
  double got;
  mpfi_t i;

  mpfi_init2 (i, 53);

  /* [nan, nan] */
  mpfr_set_nan (&(i->left));
  mpfr_set_nan (&(i->right));
  got = mpfi_get_d (i);
  if (!isnan (got))
    error_message (i, got, "NaN");

  /* [-17, nan] */
  mpfr_set_si (&(i->left), -17, MPFI_RNDD);
  got = mpfi_get_d (i);
  if (!isnan (got))
    error_message (i, got, "NaN");

  /* [-inf, -inf] */
  mpfr_set_inf (&(i->left), -1);
  mpfr_set_inf (&(i->right), -1);
  got = mpfi_get_d (i);
  if (!isinf (got) || got > 0)
    error_message (i, got, "-infinity");

  /* [-inf, 17] */
  mpfr_set_ui (&(i->right), 17, MPFI_RNDD);
  got = mpfi_get_d (i);
  if (!isinf (got) || got > 0)
    error_message (i, got, "-infinity");

  /* [+0, +inf] */
  mpfr_set_ui (&(i->left), 0, MPFI_RNDD);
  mpfr_set_inf (&(i->right), +1);
  got = mpfi_get_d (i);
  if (!isinf (got) || got < 0)
    error_message (i, got, "+infinity");

  /* [+inf, +inf] */
  mpfr_set_inf (&(i->left), +1);
  got = mpfi_get_d (i);
  if (!isinf (got) || got < 0)
    error_message (i, got, "+infinity");

  /* [-inf, +inf] */
  mpfr_set_inf (&(i->left), -1);
  mpfr_set_inf (&(i->right), +1);
  got = mpfi_get_d (i);
  if (!isnan (got))
    error_message (i, got, "NaN");

  /* [+0, -0] */
  mpfr_set_ui (&(i->left), 0, MPFI_RNDU);
  mpfr_neg (&(i->right), &(i->left), MPFI_RNDD);
  got = mpfi_get_d (i);
  if (got != 0)
    error_message (i, got, "0");

  mpfi_clear (i);
}

void
test_regular ()
{
  double x;
  mpfi_t i;

  /* assume IEEE-754 double */
  mpfi_init2 (i, 100);

  mpfr_set_ui_2exp (&(i->left), 1, -1, MPFI_RNDD);
  mpfr_set_ui_2exp (&(i->right), 1, 53, MPFI_RNDU);
  mpfr_add_ui (&(i->right), &(i->right), 1, MPFI_RNDU);
  x = mpfi_get_d (i);

  if (x != 0x10000000000001P+0)
    error_message (i, x, "4503599627370497");

  mpfi_clear (i);
}

int
main (int argc, char **argv)
{
  test_special ();
  test_regular ();

  return 0;
}
