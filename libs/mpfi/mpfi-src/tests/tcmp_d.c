/* tcmp_d.c -- Test mpfi_cmp_d.

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

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

/* Warning: DBL_MANT_DIG is not necessary a number of bits */
#ifndef DBL_MANT_DIG
#define DBL_MANT_DIG 53
#endif

#define ORDER(min, max) \
  do {                  \
    if (min > max) {    \
      double tmp;       \
      tmp = min;        \
      min = max;        \
      max = tmp;}       \
  } while (0)

void
print_error (double x, mpfi_srcptr i)
{
  printf ("Error: mpfi_cmp_d (x, I) returns %d\nx = %g\nI = ",
          mpfi_cmp_d (i, x), x);
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\n");

  exit (1);
}

void
check ()
{
  mpfi_t interval;
  double a, b, c;
  int cmp;
  int i;

  mpfi_init2 (interval, DBL_MANT_DIG);

  for (i = 0; i <= 1000; ++i) {
    /* random numbers a < b < c */
    a = random_double ();
    b = random_double ();
    c = random_double ();
    ORDER (a, b);
    ORDER (a, c);
    ORDER (b, c);

    mpfi_interv_d (interval, b, c);
    cmp = mpfi_cmp_d (interval, a);
    if (cmp < 0 || (cmp == 0 && a != b)) {
      print_error (a, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_d (interval, a) != 1) {
      print_error (a, interval);
    }

    mpfi_interv_d (interval, a, c);
    if (mpfi_cmp_d (interval, b) != 0) {
      print_error (b, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_d (interval, b) != 1) {
      print_error (b, interval);
    }

    mpfi_interv_d (interval, a, b);
    cmp = mpfi_cmp_d (interval, c);
    if (cmp > 0 || (cmp == 0 && c != b)) {
      print_error (c, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_d (interval, c) != 1) {
      print_error (c, interval);
    }
  }

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
