/* tis_empty.c -- Test mpfi_is_empty.

Copyright 2009, 2010,
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
  mpfr_prec_t prec_min = 2;
  mpfr_prec_t prec_max = 1000;
  mpfr_prec_t prec;
  int step = 10;

  mpfi_t i;

  test_start ();
  mpfi_init2 (i, 1024);

  for (prec = prec_min; prec <= prec_max; prec += step) {
    mpfi_set_prec (i, prec);
    random_interval (i);
    if (mpfi_is_empty (i)) {
      printf ("Error: mpfi_is_empty claims that the interval I = ");
      mpfi_out_str (stdout, 10, 0, i);
      printf (" is empty.\n");
      exit (1);
    }
    mpfr_swap (&(i->left), &(i->right));
    if (!mpfi_is_empty (i) && !mpfr_equal_p (&(i->left), &(i->right))) {
      printf ("Error: mpfi_is_empty claims that the interval I = ");
      mpfi_out_str (stdout, 10, 0, i);
      printf (" is not empty.\n");
      exit (1);
    }
  }

  mpfi_clear (i);
  test_end ();

  return 0;
}
