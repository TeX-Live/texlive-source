/* tbisect.c -- Test mpfi_bisect.

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

extern int nextchar;
extern unsigned long line_number;
unsigned long test_line_number;   /* start line of a test */

static void
check (mpfi_ptr left, mpfi_ptr right, mpfi_srcptr interval,
       mpfi_srcptr expected_left, mpfi_srcptr expected_right,
       int expected_inex)
{
  int inex;

  inex = mpfi_bisect (left, right, interval);
  if (inex != expected_inex || !same_value (left, expected_left)
      || !same_value (right, expected_right))
    {
      printf ("Failed line %lu.\n      interval: ", test_line_number);
      mpfi_out_str (stdout, 16, 0, interval);
      printf ("\n returned left: ");
      mpfi_out_str (stdout, 16, 0, left);
      printf ("\nreturned right: ");
      mpfi_out_str (stdout, 16, 0, right);
      printf ("\n expected left: ");
      mpfi_out_str (stdout, 16, 0, expected_left);
      printf ("\nexpected right: ");
      mpfi_out_str (stdout, 16, 0, expected_right);
      printf ("\n");
      if (inex != expected_inex) {
        printf ("inexact flag: got = %u, expected = %u\n",
                inex, expected_inex);
      }

      exit (1);
    }


  /* reuse variable tests */

  if (mpfi_get_prec (interval) == mpfi_get_prec (expected_left))
    {
      mpfi_set (left, interval);
      inex = mpfi_bisect (left, right, left);
      if (inex != expected_inex || !same_value (left, expected_left)
          || !same_value (right, expected_right))
        {
          printf ("Error when reusing input argument as first output "
                  "(line %lu).\n      interval: ", test_line_number);
          mpfi_out_str (stdout, 16, 0, interval);
          printf ("\n returned left: ");
          mpfi_out_str (stdout, 16, 0, left);
          printf ("\nreturned right: ");
          mpfi_out_str (stdout, 16, 0, right);
          printf ("\n expected left: ");
          mpfi_out_str (stdout, 16, 0, expected_left);
          printf ("\nexpected right: ");
          mpfi_out_str (stdout, 16, 0, expected_right);
          printf ("\n");
          if (inex != expected_inex) {
            printf ("inexact flag: got = %u, expected = %u\n",
                    inex, expected_inex);
          }

          exit (1);
        }
    }

  if (mpfi_get_prec (interval) == mpfi_get_prec (expected_right))
    {
      mpfi_set (right, interval);
      inex = mpfi_bisect (left, right, right);
      if (inex != expected_inex || !same_value (left, expected_left)
          || !same_value (right, expected_right))
        {
          printf ("Error when reusing input argument as second output "
                  "(line %lu).\n      interval: ", test_line_number);
          mpfi_out_str (stdout, 16, 0, interval);
          printf ("\n returned left: ");
          mpfi_out_str (stdout, 16, 0, left);
          printf ("\nreturned right: ");
          mpfi_out_str (stdout, 16, 0, right);
          printf ("\n expected left: ");
          mpfi_out_str (stdout, 16, 0, expected_left);
          printf ("\nexpected right: ");
          mpfi_out_str (stdout, 16, 0, expected_right);
          printf ("\n");
          if (inex != expected_inex) {
            printf ("inexact flag: got = %u, expected = %u\n",
                    inex, expected_inex);
          }

          exit (1);
        }
    }
}

int
main (int argc, char **argv)
{
  FILE *stream;
  mpfi_t interval;
  mpfi_t left;
  mpfi_t right;
  mpfi_t expected_left;
  mpfi_t expected_right;

  int retval;

  mpfi_init2 (interval, 1024);
  mpfi_init2 (left, 1024);
  mpfi_init2 (right, 1024);
  mpfi_init2 (expected_left, 1024);
  mpfi_init2 (expected_right, 1024);

  stream = open_file ("bisect.dat");
  init_reading (stream);

  while (nextchar != EOF) {
    test_line_number = line_number;
    read_sign (stream, &retval);
    read_mpfi (stream, expected_left);
    mpfi_set_prec (left, mpfi_get_prec (expected_left));
    read_mpfi (stream, expected_right);
    mpfi_set_prec (right, mpfi_get_prec (expected_right));
    read_mpfi (stream, interval);

    check (left, right, interval, expected_left, expected_right, retval);
  }

  close_file (stream);
  mpfi_clear (interval);
  mpfi_clear (left);
  mpfi_clear (right);
  mpfi_clear (expected_left);
  mpfi_clear (expected_right);

  return 0;
}
