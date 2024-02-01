/* tis_strictly_inside.c -- Test mpfi_is_strictly_inside.

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

#define ORDER(min, max)              \
  do {                               \
    if (mpfr_cmp ((min), (max)) > 0) \
      mpfr_swap ((min), (max));      \
  } while (0)

void
print_error (mpfi_srcptr a, mpfi_srcptr b)
{
  printf ("Error: mpfi_is_strictly_inside (A, B) returns %d\nA = ",
          mpfi_is_strictly_inside (a, b));
  mpfi_out_str (stdout, 10, 0, a);
  printf ("\nB = ");
  mpfi_out_str (stdout, 10, 0, b);
  printf ("\n");

  exit (1);
}

void
check ()
{
  mpfi_t i1, i2;
  mpfr_t a, b, c, d;
  int i;

  mpfr_init2 (a, 53);
  mpfr_init2 (b, 53);
  mpfr_init2 (c, 53);
  mpfr_init2 (d, 53);
  mpfi_init2 (i1, 53);
  mpfi_init2 (i2, 53);

  for (i = 0; i <= 1000; ++i) {
    /* random numbers a < b < c */
    random_mpfr (a);
    random_mpfr (b);
    random_mpfr (c);
    random_mpfr (d);
    ORDER (a, b);
    ORDER (a, c);
    ORDER (a, d);
    ORDER (b, c);
    ORDER (b, d);
    ORDER (c, d);

    mpfi_interv_fr (i1, a, b);
    mpfi_interv_fr (i2, c, d);
    if (mpfi_is_strictly_inside (i1, i2)) {
      print_error (i1, i2);
    }
    if (mpfi_is_strictly_inside (i2, i1)) {
      print_error (i2, i1);
    }
    mpfr_set_nan (&(i1->right));
    if (mpfi_is_strictly_inside (i1, i2)) {
      print_error (i1, i2);
    }

    mpfi_interv_fr (i1, a, c);
    mpfi_interv_fr (i2, b, d);
    if (mpfi_is_strictly_inside (i1, i2)) {
      print_error (i1, i2);
    }
    if (mpfi_is_strictly_inside (i2, i1)) {
      print_error (i2, i1);
    }
    mpfr_set_nan (&(i1->right));
    if (mpfi_is_strictly_inside (i1, i2)) {
      print_error (i1, i2);
    }

    mpfi_interv_fr (i1, a, d);
    mpfi_interv_fr (i2, b, c);
    if (mpfi_is_strictly_inside (i1, i2)) {
      print_error (i1, i2);
    }
    if (!mpfi_is_strictly_inside (i2, i1) && !mpfr_equal_p (a, b)
        && !mpfr_equal_p (b, c) && !mpfr_equal_p (c, d)) {
      print_error (i2, i1);
    }
    mpfr_set_nan (&(i1->right));
    if (mpfi_is_strictly_inside (i2, i1)) {
      print_error (i2, i1);
    }
  }

  mpfr_clear (a);
  mpfr_clear (b);
  mpfr_clear (c);
  mpfr_clear (d);
  mpfi_clear (i1);
  mpfi_clear (i2);
}

int
main (int argc, char **argv)
{
  test_start ();

  check ();

  test_end ();

  return 0;
}
