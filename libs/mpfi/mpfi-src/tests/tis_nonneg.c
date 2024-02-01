/* tis_nonneg.c -- Test mpfi_is_nonneg.

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

extern int nextchar;

void
print_error (mpfi_srcptr a)
{
  printf ("Error: mpfi_is_nonneg (A) returns %d\nA = ",
          mpfi_is_nonneg (a));
  mpfi_out_str (stdout, 10, 0, a);
  printf ("\n");

  exit (1);
}

int
main (int argc, char **argv)
{
  FILE *stream;
  mpfi_t interval;
  int expected;
  int got;

  mpfi_init2 (interval, 1024);

  stream = open_file ("is_nonneg.dat");

  init_reading (stream);

  while (nextchar != EOF) {
    read_sign (stream, &expected);
    read_mpfi (stream, interval);

    got = mpfi_is_nonneg (interval);
    if (got != expected || got * expected < 0)
      print_error (interval);
  }

  close_file (stream);
  mpfi_clear (interval);

  return 0;
}
