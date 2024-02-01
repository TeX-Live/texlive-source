/* tfr_sub.c -- Test mpfi_fr_sub.

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
check_overflow (void)
{
  mpfi_t interval;
  mpfi_t got;
  mpfr_t x;
  int inex;

  mpfi_init2 (interval, 53);
  mpfi_init2 (got, 53);
  mpfr_init2 (x, 53);

  /* right overflow: 1023 - [-Max, 0] = [1023, +oo] */
  mpfr_set_ui (x, 1023, MPFI_RNDD);
  mpfr_set_inf (&(interval->left), -1);
  mpfr_nextabove (&(interval->left));
  mpfr_set_ui (&(interval->right), 0, MPFI_RNDU);

  inex = mpfi_fr_sub (got, x, interval);

  if (MPFI_LEFT_IS_INEXACT (inex) || mpfr_cmp (&(got->left), x) != 0
      || !MPFI_RIGHT_IS_INEXACT (inex) || !mpfr_inf_p (&(got->right))) {
    printf ("Error: mpfi_fr_sub (rop, x, op) does not correctly handle "
            "overflow.\n  x = ");
    mpfr_out_str (stdout, 16, 0, x, MPFI_RNDD);
    printf ("\nop = ");
    mpfi_out_str (stdout, 16, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  /* left overflow: -1023 - [0, Max] = [-oo, -1023] */
  mpfr_neg (x, x, MPFI_RNDD);
  mpfi_neg (interval, interval);

  inex = mpfi_fr_sub (got, x, interval);

  if (!MPFI_LEFT_IS_INEXACT (inex)
      || !mpfr_inf_p (&(got->left))
      || MPFI_RIGHT_IS_INEXACT (inex)
      || mpfr_cmp_si (&(got->right), -1023) != 0) {
    printf ("Error: mpfi_fr_sub (rop, x, op) does not correctly handle "
            "overflow.\n  x = ");
    mpfr_out_str (stdout, 16, 0, x, MPFI_RNDD);
    printf ("\nop = ");
    mpfi_out_str (stdout, 16, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  mpfr_clear (x);
  mpfi_clear (interval);
  mpfi_clear (got);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_fr_sub;

  mpfi_fun_init_IRI (&i_fr_sub, mpfi_fr_sub, mpfr_sub);
  test_start ();

  check_data (&i_fr_sub, "fr_sub.dat");
  check_random (&i_fr_sub, 2, 1000, 10);
  check_overflow ();

  test_end ();
  mpfi_fun_clear (&i_fr_sub);

  return 0;
}
