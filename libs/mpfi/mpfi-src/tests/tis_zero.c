/* tis_zero.c -- Test mpfi_is_zero.

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

extern int nextchar;

void
print_error (mpfi_srcptr a)
{
  printf ("Error: mpfi_is_zero (A) returns %d\nA = ",
          mpfi_is_zero (a));
  mpfi_out_str (stdout, 10, 0, a);
  printf ("\n");

  exit (1);
}

int
main (int argc, char **argv)
{
  mpfr_t x;
  mpfi_t interval;
  mpfr_prec_t p;

  test_start ();

  mpfr_init2 (x, 1024);
  mpfi_init2 (interval, 1024);

  mpfi_set_ui (interval, 0);
  if (!mpfi_is_zero (interval)) {
    print_error (interval);
  }

  for (p = 2; p < 1024; ++p) {
    mpfr_set_prec (x, p);
    mpfi_set_prec (interval, p);

    random_mpfr (x);
    mpfi_set_fr (interval, x);
    if (mpfi_is_zero (interval) && !mpfr_zero_p (x)) {
      print_error (interval);
    }

    random_mpfr (x);
    if (mpfr_zero_p (x))
      mpfr_nextabove (x);
    /* x != 0*/
    mpfi_put_fr (interval, x);
    if (mpfi_is_zero (interval)) {
      print_error (interval);
    }

    mpfr_set_nan (&(interval->left));
    if (mpfi_is_zero (interval)) {
      print_error (interval);
    }
  }

  mpfr_clear (x);
  mpfi_clear (interval);

  test_end ();

  return 0;
}
