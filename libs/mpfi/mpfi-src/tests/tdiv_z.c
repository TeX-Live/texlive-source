/* tdiv_z.c -- Test mpfi_div_z.

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
  mpz_t z;
  int inex;

  mpz_init (z);
  mpfi_init2 (got, 128);
  mpfi_init2 (expected, 128);

  mpz_set_ui (z, 1024);
  mpfi_set_ui (expected, 0);
  mpfr_nextbelow (&(expected->left));
  mpfr_nextabove (&(expected->right));

  inex = mpfi_div_z (got, expected, z);
  if (!MPFI_BOTH_ARE_INEXACT(inex)
      || !same_mpfr_value (&(got->left), &(expected->left))
      || !same_mpfr_value (&(got->right), &(expected->right))) {
    printf ("Error: mpfi_div_z (rop, op, z) does not return correct value\n"
            "op = ");
    mpfi_out_str (stdout, 16, 0, expected);
    printf ("\nz = ");
    mpz_out_str (stdout, 16, z);
    printf ("\ngot      = ");
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

  mpz_clear (z);
  mpfi_clear (got);
  mpfi_clear (expected);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_div_z;

  mpfi_fun_init_IIZ (&i_div_z, mpfi_div_z, mpfr_div_z);
  test_start ();

  check_data (&i_div_z, "div_z.dat");
  check_random (&i_div_z, 2, 1000, 10);

  test_end ();
  mpfi_fun_clear (&i_div_z);

  return 0;
}
