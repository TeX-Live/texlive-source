/* tq_sub.c -- Test mpfi_q_sub.

Copyright 2010, 2012,
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
check_overflow ()
{
  mpfi_t interval;
  mpfi_t got;
  mpq_t q;
  int inex;

  mpfi_init2 (interval, 53);
  mpfi_init2 (got, 53);
  mpq_init (q);

  /* right overflow: 1023 - [-Max, 0] = [1023, +oo] */
  mpq_set_ui (q, 1023, 1);
  mpfr_set_inf (&(interval->left), -1);
  mpfr_nextabove (&(interval->left));
  mpfr_set_ui (&(interval->right), 0, MPFI_RNDU);

  inex = mpfi_q_sub (got, q, interval);

  if (MPFI_LEFT_IS_INEXACT (inex) || mpfr_cmp_q (&(got->left), q) != 0
      || !MPFI_RIGHT_IS_INEXACT (inex) || !mpfr_inf_p (&(got->right))) {
    printf ("Error: mpfi_q_sub (rop, q, op) does not correctly handle "
            "overflow.\n  q = ");
    mpq_out_str (stdout, 16, q);
    printf ("\nop = ");
    mpfi_out_str (stdout, 16, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  /* left overflow: -1023 - [0, Max] = [-oo, -1023] */
  mpq_neg (q, q);
  mpfi_neg (interval, interval);

  inex = mpfi_q_sub (got, q, interval);

  if (!MPFI_LEFT_IS_INEXACT (inex)
      || !mpfr_inf_p (&(got->left))
      || MPFI_RIGHT_IS_INEXACT (inex)
      || mpfr_cmp_si (&(got->right), -1023) != 0) {
    printf ("Error: mpfi_q_sub (rop, q, op) does not correctly handle "
            "overflow.\n  q = ");
    mpq_out_str (stdout, 16, q);
    printf ("\nop = ");
    mpfi_out_str (stdout, 16, 0, interval);
    printf ("\nrop = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nreturn value = %d\n", inex);
    exit (1);
  }

  mpq_clear (q);
  mpfi_clear (interval);
  mpfi_clear (got);
}

#ifndef HAVE_MPFR_Q_SUB
/* fake non-existing function */
int
mpfr_q_sub (mpfr_ptr x, mpq_srcptr q, mpfr_srcptr y, mpfr_rnd_t rnd)
{
  int ret;

  /* invert rounding mode */
  if (rnd == MPFI_RNDU)
    rnd = MPFI_RNDD;
  else if (rnd == MPFI_RNDD)
    rnd = MPFI_RNDU;
  ret = mpfr_sub_q (x, y, q, rnd);
  mpfr_neg (x, x, MPFI_RNDU);
  return -ret;
}
#endif /* HAVE_MPFR_Q_SUB */

int
main (int argc, char **argv)
{
  struct mpfi_function_t i_q_sub;

  mpfi_fun_init_IQI (&i_q_sub, mpfi_q_sub, mpfr_q_sub);
  test_start ();

  check_data (&i_q_sub, "q_sub.dat");
  check_overflow ();

  test_end ();
  mpfi_fun_clear (&i_q_sub);

  return 0;
}
