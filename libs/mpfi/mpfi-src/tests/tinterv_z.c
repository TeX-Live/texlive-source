/* tinterv_z.c -- Test mpfi_interv_z.

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
check (mpfi_ptr i, mpz_srcptr a, mpz_srcptr b,
       mpfr_srcptr expected_left, mpfr_srcptr expected_right,
       int expected_inex)
{
  int inex;

  inex = mpfi_interv_z (i, a, b);
  if (inex != expected_inex) {
    printf ("Error: mpfi_interv_z (i, a, b) returns %d instead of %d\n",
            inex, expected_inex);
    printf ("precision(i) = %lu\na =", mpfi_get_prec (i));
    mpz_out_str (stdout, 10, a);
    printf ("\nb = ");
    mpz_out_str (stdout, 10, b);
    printf ("\n");
    exit (1);
  }
  if (!same_mpfr_value (&(i->left), expected_left)
      || !same_mpfr_value (&(i->right), expected_right)) {
    printf ("Error: mpfi_interv_z (i, a, b) failed.\n");
    printf ("\na = ");
    mpz_out_str (stdout, 10, a);
    printf ("\nb = ");
    mpz_out_str (stdout, 10, b);
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
  mpfr_t x, y;

  mpz_t a, b;
  mpfi_t i;

  mpz_init (a);
  mpz_init (b);
  mpfi_init2 (i, 53);
  mpfr_init2 (x, 53);
  mpfr_init2 (y, 53);

  mpz_set_si (a, -10000);
  mpz_fac_ui (b, 53);
  mpfr_set_z (x, a, MPFI_RNDD);
  mpfr_set_z (y, b, MPFI_RNDU);
  check (i, a, b, x, y, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, b, a, x, y, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT);
  check (i, a, a, x, x, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT);
  mpfr_set_z (x, b, MPFI_RNDD);
  check (i, b, b, x, y, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT);

  mpz_clear (a);
  mpz_clear (b);
  mpfi_clear (i);
  mpfr_clear (x);
  mpfr_clear (y);

  return 0;
}
