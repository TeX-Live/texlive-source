/* tz_div.c -- Test mpfi_z_div.

Copyright 2010, 2011,
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
  mpz_t z;
  int inex;

  mpz_init (z);
  mpfi_init2 (interval, 53);
  mpfi_init2 (got, 53);

  /* right overflow: 1024 / [epsilon, 1] = [1024, +oo] */
  mpz_set_ui (z, 1024);
  mpfr_set_ui (&(interval->left), 0, MPFI_RNDD);
  mpfr_nextabove (&(interval->left)); /* tiny left endpoint x0 */
  mpfr_set_ui (&(interval->right), 1, MPFI_RNDU);

  inex = mpfi_z_div (got, z, interval);
  if (MPFI_LEFT_IS_INEXACT (inex) || mpfr_cmp_z (&(got->left), z) != 0
      || !MPFI_RIGHT_IS_INEXACT (inex) || !mpfr_inf_p (&(got->right))) {
    printf ("Error: mpfi_z_div (rop, z, op) does not correctly handle "
            "overflow.\n  z = ");
    mpz_out_str (stdout, 10, z);
    printf ("\n op = ");
    mpfi_out_str (stdout, 10, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 10, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  /* left overflow: 1024.0 / [-1, -epsilon] = [-oo, -1024] */
  mpfi_neg (interval, interval);

  inex = mpfi_z_div (got, z, interval);
  if (!MPFI_LEFT_IS_INEXACT (inex)
      || !mpfr_inf_p (&(got->left))
      || MPFI_RIGHT_IS_INEXACT (inex)
      || mpfr_cmp_si (&(got->right), -1024) != 0) {
    printf ("Error: mpfi_z_div (rop, z, op) does not correctly handle "
            "overflow.\n  z = ");
    mpz_out_str (stdout, 10, z);
    printf ("\n op = ");
    mpfi_out_str (stdout, 10, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 10, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  mpz_clear (z);
  mpfi_clear (interval);
  mpfi_clear (got);
}

#ifndef HAVE_MPFR_Z_DIV
/* fake non-existing function */
int
mpfr_z_div (mpfr_ptr x, mpz_srcptr z, mpfr_srcptr y, mpfr_rnd_t rnd)
{
  int ret;
  mpfr_t zz;

  mpfr_init2 (zz, mpz_sizeinbase (z, 2));
  mpfr_set_z (zz, z, MPFI_RNDD); /* exact */
  ret = mpfr_div (x, zz, y, rnd);
  mpfr_clear (zz);

  return ret;
}
#endif /*HAVE_MPFR_Z_DIV */

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_z_div;

  mpfi_fun_init_IZI (&i_z_div, mpfi_z_div, mpfr_z_div);
  test_start ();

  check_data (&i_z_div, "z_div.dat");
  check_random (&i_z_div, 2, 1000, 10);
  check_overflow ();

  test_end ();
  mpfi_fun_clear (&i_z_div);

  return 0;
}
