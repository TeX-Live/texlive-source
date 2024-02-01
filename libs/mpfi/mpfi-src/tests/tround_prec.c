/* tround_prec.c -- Test mpfi_round_prec.

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
check (mpfi_ptr i, mpfr_prec_t new_prec,
       mpfr_srcptr expected_left, mpfr_srcptr expected_right,
       int expected_inex)
{
  int inex;

  inex = mpfi_round_prec (i, new_prec);
  if (inex != expected_inex) {
    printf ("Error: mpfi_round_prec (i, p) returns %d instead of %d\n",
            inex, expected_inex);
    printf ("precision of i before function call = %lu\np = %lu\n",
            mpfi_get_prec (i), new_prec);
    exit (1);
  }
  if (!same_mpfr_value (&(i->left), expected_left)
      || !same_mpfr_value (&(i->right), expected_right)) {
    printf ("Error: mpfi_round_prec (i, p) failed.\n");
    printf ("precision of i before function call = %lu\np = %lu\n",
            mpfi_get_prec (i), new_prec);
    printf ("\ngot    i = ");
    mpfi_out_str (stdout, 2, 0, i);
    printf ("\nexpected = [");
    mpfr_out_str (stdout, 2, 0, expected_left, MPFI_RNDD);
    printf (", ");
    mpfr_out_str (stdout, 2, 0, expected_right, MPFI_RNDU);
    printf ("]\n");
    exit (1);
  }
}

void
special (void)
{
  mpfr_t minf, pinf;
  mpfr_t mzero, pzero;
  mpfr_t nan;
  mpfi_t i;

  mpfi_init2 (i, 53);
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

  mpfr_set (&(i->left), minf, MPFI_RNDD);
  mpfr_set (&(i->right), minf, MPFI_RNDU);
  check (i, 2, minf, minf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), minf, MPFI_RNDD);
  mpfr_set (&(i->right), mzero, MPFI_RNDU);
  check (i, 3, minf, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), minf, MPFI_RNDD);
  mpfr_set (&(i->right), pinf, MPFI_RNDU);
  check (i, 4, minf, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), minf, MPFI_RNDD);
  mpfr_set_nan (&(i->right));
  check (i, 5, minf, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), pzero, MPFI_RNDD);
  mpfr_set (&(i->right), mzero, MPFI_RNDU);
  check (i, 6, pzero, mzero, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), pzero, MPFI_RNDD);
  mpfr_set (&(i->right), pinf, MPFI_RNDU);
  check (i, 7, pzero, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), pzero, MPFI_RNDD);
  mpfr_set_nan (&(i->right));
  check (i, 8, pzero, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), pinf, MPFI_RNDD);
  mpfr_set (&(i->right), pinf, MPFI_RNDU);
  check (i, 9, pinf, pinf, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_set_prec (i, 53);
  mpfr_set_nan (&(i->left));
  mpfr_set_nan (&(i->right));
  check (i, 10, nan, nan, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  mpfi_clear (i);
  mpfr_clear (minf);
  mpfr_clear (pinf);
  mpfr_clear (mzero);
  mpfr_clear (pzero);
  mpfr_clear (nan);
}

void
swing (void)
{
  mpfr_prec_t p;
  mpfi_t i;
  mpfr_t left, right, tmp;
  int inex [2] = {
    MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT,
    MPFI_FLAGS_LEFT_ENDPOINT_INEXACT
  };

  mpfi_init2 (i, 1024);
  mpfr_init2 (left, 53);
  mpfr_init2 (right, 53);
  mpfr_init (tmp);

  mpfr_set_str (left, "0x35555555555555p-54", 0, MPFI_RNDD);
  mpfr_set_str (right, "0x35555555555555p-54", 0, MPFI_RNDU);

  mpfi_set_prec (i, 53);
  mpfr_set (&(i->left), left, MPFI_RNDD);
  mpfr_set (&(i->right), right, MPFI_RNDU);

  check (i, 1024, left, right, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);

  for (p = 52; p > 2; --p) {
    mpfr_set_prec (tmp, p);
    mpfr_set (tmp, left, MPFI_RNDD);
    mpfr_swap (tmp, left); /* Warning: precisions are swap too */

    mpfr_set_prec (tmp, p);
    mpfr_set (tmp, right, MPFI_RNDU);
    mpfr_swap (tmp, right);

    check (i, p, left, right, inex[p % 2]);
  }

  mpfi_clear (i);
  mpfr_clear (left);
  mpfr_clear (right);
  mpfr_clear (tmp);
}

int
main (int argc, char **argv)
{
  special ();
  swing ();

  return 0;
}
