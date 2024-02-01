/* tmig.c -- Test mpfi_mig.

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
test_random (mpfr_prec_t prec_min, mpfr_prec_t prec_max)
{
  mpfr_t x;
  mpfi_t i;
  mpfr_prec_t prec;
  int dl, dr, d0;
  int ret;

  mpfr_init2 (x, prec_max);
  mpfi_init2 (i, prec_max);

  for (prec = prec_min; prec < prec_max; prec++) {
    mpfi_set_prec (i, prec);
    mpfr_set_prec (x, prec);
    random_interval (i);
    ret = mpfi_mig (x, i);
    dl = mpfr_cmp_abs (x, &(i->left));
    dr = mpfr_cmp_abs (x, &(i->right));
    if (dl > 0 || dr > 0) {
      printf ("Error: mpfi_mig (x, I) returns a value x greater than some "
              "elements in I.\nI = ");
      mpfi_out_str (stdout, 10, 0, i);
      printf ("\nx = ");
      mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
      printf ("\n");
      exit (1);
    }
    d0 = mpfr_cmp_ui (x, 0);
    if (d0 < 0) {
      printf ("Error: mpfi_mig (x, I) returns a negative value.\nI = ");
      mpfi_out_str (stdout, 10, 0, i);
      printf ("\nx = ");
      mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
      printf ("\n");
      exit (1);
    }
    if (dl != 0 && dr != 0 && d0 != 0) {
      printf ("Error: mpfi_mig(x, I) returns a value x that is not an "
              "endpoint of abs(I).\nI = ");
      mpfi_out_str (stdout, 10, 0, i);
      printf ("\nx = ");
      mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
      printf ("\n");
      exit (1);
    }
    if (ret != 0) {
      printf ("Error: mpfi_mig(x, I) returns a nonexact value x while the "
              "precisions of x and I are equal.\nprecision(I) = %lu, I = ",
              mpfi_get_prec (i));
      mpfi_out_str (stdout, 10, 0, i);
      printf ("\nprecision(x) = %lu, x = ", mpfr_get_prec (x));
      mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
      printf ("\n");
      exit (1);
    }
  }

  mpfr_clear (x);
  mpfi_clear (i);
}

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_mig;

  mpfi_fun_init_RI (&i_mig, mpfi_mig, NULL);

  test_start ();

  check_data (&i_mig, "mig.dat");
  test_random (2, 1023);

  test_end ();
  mpfi_fun_clear (&i_mig);

  return 0;
}
