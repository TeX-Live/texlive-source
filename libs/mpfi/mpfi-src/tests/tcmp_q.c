/* tcmp_q.c -- Test mpfi_cmp_q.

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

#define ORDER(min, max)             \
  do {                              \
    if (mpq_cmp ((min), (max)) > 0) \
      mpq_swap ((min), (max));      \
  } while (0)

void
print_error (mpq_ptr x, mpfi_srcptr i)
{
  printf ("Error: mpfi_cmp_q (x, I) returns %d\nx = ",
          mpfi_cmp_q (i, x));
  mpq_out_str (stdout, 10, x);
  printf ("\nI = ");
  mpfi_out_str (stdout, 10, 0, i);
  printf ("\n");

  exit (1);
}

void
check ()
{
  mpfi_t interval;
  mpq_t a, b, c;
  int cmp;
  int i;

  mpq_init (a);
  mpq_init (b);
  mpq_init (c);
  mpfi_init2 (interval, 53);

  for (i = 0; i <= 1000; ++i) {
    /* random numbers a < b < c */
    random_mpq (a);
    random_mpq (b);
    random_mpq (c);
    ORDER (a, b);
    ORDER (a, c);
    ORDER (b, c);

    mpfi_interv_q (interval, b, c);
    cmp = mpfi_cmp_q (interval, a);
    if (cmp < 0 || (cmp == 0 && mpq_cmp (a, b) != 0)) {
      print_error (a, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_q (interval, a) != 1) {
      print_error (a, interval);
    }

    mpfi_interv_q (interval, a, c);
    if (mpfi_cmp_q (interval, b) != 0) {
      print_error (b, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_q (interval, b) != 1) {
      print_error (b, interval);
    }

    mpfi_interv_q (interval, a, b);
    cmp = mpfi_cmp_q (interval, c);
    if (cmp > 0 || (cmp == 0 && mpq_cmp (c, b) != 0)) {
      print_error (c, interval);
    }
    mpfr_set_nan (&(interval->right));
    if (mpfi_cmp_q (interval, c) != 1) {
      print_error (c, interval);
    }
  }

  mpq_clear (a);
  mpq_clear (b);
  mpq_clear (c);
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
