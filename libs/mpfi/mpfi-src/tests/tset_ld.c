/* tset_ld.c -- Test mpfi_set_ld.

Copyright 2018
                     AriC project, Inria Grenoble - Rhone-Alpes, France


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
  struct mpfi_function_t i_set_ld;

  mpfi_fun_init_IL (&i_set_ld, mpfi_set_ld, NULL);

  check_data (&i_set_ld, "set_ld.dat");

  mpfi_fun_clear (&i_set_ld);

  return 0;
}
