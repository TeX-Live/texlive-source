/* tget_swap.c -- Test mpfi_swap.

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

int
main (int argc, char **argv)
{
  mpfi_t a, b;
  unsigned long pa, pb;

  mpfi_init2 (a, 53);
  mpfi_init2 (b, 2);

  mpfr_set_ui (&(a->left), 4, MPFI_RNDD);
  mpfr_set_ui (&(a->right), 7, MPFI_RNDU);
  mpfr_set_ui (&(b->left), 0, MPFI_RNDD);
  mpfr_set_ui (&(b->right), 2, MPFI_RNDU);
  mpfi_swap (a, b);
  if (mpfr_cmp_ui (&(b->left), 4) != 0
      || mpfr_cmp_ui (&(b->right), 7) != 0
      || mpfr_cmp_ui (&(a->left), 0) != 0
      || mpfr_cmp_ui (&(a->right), 2) != 0) {
    printf ("Error: mpfi_swap (a, b) does not swap values.\na = ");
    mpfi_out_str (stdout, 10, 0, a);
    printf ("\nb = ");
    mpfi_out_str (stdout, 10, 0, b);
    printf ("\n");
    exit (1);
  }
  if (mpfi_get_prec (a) != 2 || mpfi_get_prec (b) != 53) {
    pa = mpfi_get_prec (a);
    pb = mpfi_get_prec (b);
    printf ("Error: mpfi_swap(a, b) does not swap precisions.\n"
            "precision of a = %lu\nprecision of b = %lu", pa, pb);
    exit (1);
  }

  mpfi_clear (a);
  mpfi_clear (b);
  return 0;
}
