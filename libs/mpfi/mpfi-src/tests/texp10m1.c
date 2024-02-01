/* texp10m1.c -- Test mpfi_exp10m1.

Copyright 2009, 2010, 2022
                     Spaces project, Inria Lorraine
                     and Salsa project, INRIA Rocquencourt,
                     and Arenaire project, Inria Rhone-Alpes, France
                     and AriC project, Inria Rhone-Alpes and Inria Lyon, France
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
  struct mpfi_function_t i_exp10m1;

/* awaiting for exp10m1 in MPFR
  mpfi_fun_init_II (&i_exp10m1, mpfi_exp10m1, mpfr_exp10m1);
*/
  mpfi_fun_init_II (&i_exp10m1, mpfi_exp10m1, mpfr_exp10);
  test_start ();

  check_data (&i_exp10m1, "exp10m1.dat");
/* awaiting for exp10m1 in MPFR
  check_random (&i_exp10m1, 2, 1000, 10);
*/

  test_end ();
  mpfi_fun_clear (&i_exp10m1);

  return 0;
}
