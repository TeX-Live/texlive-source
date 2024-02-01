/* tdiv_si.c -- Test mpfi_div_si.

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
check_underflow (void)
{
  mpfi_t got, expected;
  long si = 1024;
  int inex;

  mpfi_init2 (got, 128);
  mpfi_init2 (expected, 128);

  mpfi_set_ui (expected, 0);
  mpfr_nextbelow (&(expected->left));
  mpfr_nextabove (&(expected->right));

  inex = mpfi_div_si (got, expected, si);
  if (!MPFI_BOTH_ARE_INEXACT(inex)
      || !same_mpfr_value (&(got->left), &(expected->left))
      || !same_mpfr_value (&(got->right), &(expected->right))) {
    printf ("Error: mpfi_div_si (rop, op, si) does not return correct value\n"
            "op = ");
    mpfi_out_str (stdout, 16, 0, expected);
    printf ("\nsi = %ld\ngot      = ", si);
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nexpected = ");
    mpfi_out_str (stdout, 16, 0, expected);
    printf ("\n");

    if (!MPFI_BOTH_ARE_INEXACT(inex)) {
      printf ("return value = %d\nexpected     = %d\n", inex,
              MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT);
    }

    exit (1);
  }

  mpfi_clear (got);
  mpfi_clear (expected);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_div_si;

  mpfi_fun_init_IIS (&i_div_si, mpfi_div_si, mpfr_div_si);

  test_start ();

  check_data (&i_div_si, "div_si.dat");
  check_random (&i_div_si, 2, 1000, 10);
  check_underflow ();

  test_end ();
  mpfi_fun_clear (&i_div_si);

  return 0;
}
