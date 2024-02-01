/* tunion.c -- Test mpfi_union.

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

/* the middle of two points is in their convex hull */
int
middle (mpfr_ptr m, mpfr_srcptr a, mpfr_srcptr b, mpfr_rnd_t rnd)
{
  mpfr_t min, max;

  if (mpfr_cmp (a, b) < 0) {
    min[0] = a[0];
    max[0] = b[0];
  }
  else {
    min[0] = b[0];
    max[0] = a[0];
  }
  mpfr_sub (m, max, min, rnd);
  mpfr_div_2exp (m, m, 1, rnd);
  mpfr_add (m, min, m, rnd);

  return 0;
}


int
main (int argc, char **argv)
{
  struct mpfi_function_t i_union;

  mpfi_fun_init_III (&i_union, mpfi_union, middle);

  test_start ();


  check_data (&i_union, "union.dat");
  check_random (&i_union, 2, 1000, 10);

  test_end ();
  mpfi_fun_clear (&i_union);

  return 0;
}
