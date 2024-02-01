/* tis_inside_si.c -- Test mpfi_is_inside_si.

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

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifndef LONG_MAX
# define LONG_MAX +2147483647
#endif

#define ORDER(min, max)  \
  do {                   \
    if (min > max) {     \
      long tmp;          \
      tmp = min;         \
      min = max;         \
      max = tmp;}        \
  } while (0)

void
print_error (long x, mpfi_srcptr i)
{
  printf ("Error: mpfi_is_inside_si (x, I) returns %d\nx = %ld\nI = ",
          mpfi_is_inside_si (x, i), x);
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\n");

  exit (1);
}

void
check ()
{
  mpfi_t interval;
  long a, b, c;
  int i;

  i = 1;
  a = LONG_MAX;
  while (a >>= 1) i++;

  mpfi_init2 (interval, i);

  for (i = 0; i <= 1000; ++i) {
    /* random numbers a < b < c */
    a = random_si ();
    b = random_si ();
    c = random_si ();
    ORDER (a, b);
    ORDER (a, c);
    ORDER (b, c);

    mpfi_interv_si (interval, b, c);
    if (a != b && mpfi_is_inside_si (a, interval)) {
      print_error (a, interval);
    }

    mpfr_set_nan (&(interval->left));
    if (mpfi_is_inside_si (a, interval)) {
      print_error (a, interval);
    }

    mpfi_interv_si (interval, a, c);
    if (!mpfi_is_inside_si (b, interval)) {
      print_error (b, interval);
    }

    mpfr_set_nan (&(interval->right));
    if (mpfi_is_inside_si (b, interval)) {
      print_error (b, interval);
    }

    mpfi_interv_si (interval, a, b);
    if (c != b && mpfi_is_inside_si (c, interval)) {
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
