/* tdiv_si.c -- Test mpfi_si_div.

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
  long si = 1024.0;
  int inex;

  mpfi_init2 (interval, 53);
  mpfi_init2 (got, 53);

  /* right overflow: 1024 / [epsilon, 1] = [1024, +oo] */
  mpfr_set_ui (&(interval->left), 0, MPFI_RNDD);
  mpfr_nextabove (&(interval->left)); /* tiny left endpoint x0 */
  mpfr_set_ui (&(interval->right), 1, MPFI_RNDU);

  inex = mpfi_si_div (got, si, interval);
  if (MPFI_LEFT_IS_INEXACT (inex) || mpfr_cmp_si (&(got->left), si) != 0
      || !MPFI_RIGHT_IS_INEXACT (inex) || !mpfr_inf_p (&(got->right))) {
    printf ("Error: mpfi_si_div (rop, %ld, op) does not correctly handle "
            "overflow.\nop = ", si);
    mpfi_out_str (stdout, 10, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 10, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  /* left overflow: 1024.0 / [-1, -epsilon] = [-oo, -1024] */
  mpfi_neg (interval, interval);

  inex = mpfi_si_div (got, si, interval);
  if (!MPFI_LEFT_IS_INEXACT (inex)
      || !mpfr_inf_p (&(got->left))
      || MPFI_RIGHT_IS_INEXACT (inex)
      || mpfr_cmp_si (&(got->right), -si) != 0) {
    printf ("Error: mpfi_si_div (rop, %ld, op) does not correctly handle "
            "overflow.\nop = ", si);
    mpfi_out_str (stdout, 10, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 10, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  mpfi_clear (interval);
  mpfi_clear (got);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_si_div;

  mpfi_fun_init_ISI (&i_si_div, mpfi_si_div, mpfr_si_div);

  test_start ();

  check_data (&i_si_div, "si_div.dat");
  check_random (&i_si_div, 2, 1000, 10);
  check_overflow ();

  test_end ();
  mpfi_fun_clear (&i_si_div);

  return 0;
}
