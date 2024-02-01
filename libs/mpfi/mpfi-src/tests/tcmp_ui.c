/* tcmp_ui.c -- Test mpfi_cmp_ui.

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

#ifndef ULONG_MAX
# define ULONG_MAX 4294967295
#endif

#define ORDER(min, max)                         \
  do {                                          \
    if (min > max) {                            \
      unsigned long tmp;                        \
      tmp = min;                                \
      min = max;                                \
      max = tmp;}                               \
  } while (0)

void
print_error (unsigned long x, mpfi_srcptr i)
{
  printf ("Error: mpfi_cmp_ui (x, I) returns %d\nx = %lu\nI = ",
          mpfi_cmp_ui (i, x), x);
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\n");

  exit (1);
}

void
check ()
{
  mpfi_t interval;
  unsigned long a, b, c;
  int cmp;
  int i;

  i = 1;
  a = ULONG_MAX;
  while (a >>= 1) i++;

  mpfi_init2 (interval, i);

  for (i = 0; i <= 1000; ++i) {
    /* random numbers a < b < c */
    a = random_ui ();
    b = random_ui ();
    c = random_ui ();
    ORDER (a, b);
    ORDER (a, c);
    ORDER (b, c);

    mpfi_interv_ui (interval, b, c);
    cmp = mpfi_cmp_ui (interval, a);
    if (cmp < 0 || (cmp == 0 && a != b)) {
      print_error (a, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_ui (interval, a) != 1) {
      print_error (a, interval);
    }

    mpfi_interv_ui (interval, a, c);
    if (mpfi_cmp_ui (interval, b) != 0) {
      print_error (b, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_ui (interval, b) != 1) {
      print_error (b, interval);
    }

    mpfi_interv_ui (interval, a, b);
    cmp = mpfi_cmp_ui (interval, c);
    if (cmp > 0 || (cmp == 0 && c != b)) {
      print_error (c, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_ui (interval, c) != 1) {
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
