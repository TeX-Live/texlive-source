/* tcmp_fr.c -- Test mpfi_cmp_fr.

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
print_error (mpfr_ptr x, mpfi_srcptr i)
{
  printf ("Error: mpfi_cmp_fr (x, I) returns %d\nx = ",
          mpfi_cmp_fr (i, x));
  mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
  printf ("\nI = ");
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\n");

  exit (1);
}

void
check ()
{
  mpfi_t interval;
  mpfr_t a, b, c;
  int i;
  int cmp;

  mpfr_init2 (a, 53);
  mpfr_init2 (b, 53);
  mpfr_init2 (c, 53);
  mpfi_init2 (interval, 53);

  for (i = 0; i <= 1000; ++i) {
    /* random numbers a < b < c */
    random_interval (interval);
    mpfi_alea (b, interval);
    mpfr_set (a, &(interval->left), MPFI_RNDD);
    mpfr_set (c, &(interval->right), MPFI_RNDU);

    mpfi_interv_fr (interval, b, c);
    cmp = mpfi_cmp_fr (interval, a);
    if (cmp < 0 || (cmp == 0 && !mpfr_equal_p (a, b))) {
      print_error (a, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_fr (interval, a) != 1) {
      print_error (a, interval);
    }

    mpfi_interv_fr (interval, a, c);
    if (mpfi_cmp_fr (interval, b) != 0) {
      print_error (b, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_fr (interval, b) != 1) {
      print_error (b, interval);
    }

    mpfi_interv_fr (interval, a, b);
    cmp = mpfi_cmp_fr (interval, c);
    if (cmp > 0 || (cmp == 0 && !mpfr_equal_p (c, b))) {
      print_error (c, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_fr (interval, c) != 1) {
      print_error (c, interval);
    }
  }

  mpfr_clear (a);
  mpfr_clear (b);
  mpfr_clear (c);
  mpfi_clear (interval);
}

int
main (int argc, char **argv)
{
  test_start ();

  check ();

  test_end ();

  return 0;
}
