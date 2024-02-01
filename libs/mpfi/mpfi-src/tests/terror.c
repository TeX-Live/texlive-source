/* terror.c -- Frightening test of error handling.

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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

int
read_stderr (FILE *stream)
{
  int ret = 0;

  /* something to read? */
  ret = (fgetc (stream) != EOF);
  if (fseek (stream, 0, SEEK_END) != 0) {
    printf ("Internal error: cannot reset stream.\n");
    exit (1);
  }

  return ret;
}

int
main (int argc, char **argv)
{
  FILE *stream;

  if (freopen ("error.tmp", "w", stderr) == NULL) {
    printf ("Internal error: cannot redirect stderr to a file.\n");
    exit (1);
  }
  stream = fopen ("error.tmp", "r");
  if (stream == NULL) {
    printf ("Internal error: cannot open the file \"error.tmp\".\n");
    exit (1);
  }

  mpfi_reset_error ();
  if (mpfi_is_error () == 1) {
    printf ("Error: mpfi_is_error() returns 1 after a call to "
            "mpfi_reset_error.\n");
    exit (1);
  }

  MPFI_ERROR ("[1] this should be printed!");
  fflush (stderr);
  if (!read_stderr (stream)) {
    printf ("Error: MPFI_ERROR does not print message after a call to "
            "mpfi_reset_error.\n");
    exit (1);
  }
  if (mpfi_is_error () != 1) {
    printf ("Error: mpfi_is_error does not set return 1 after a call to "
            "MPFI_ERROR.\n");
    exit (1);
  }

  mpfi_set_error (2);
  if (mpfi_is_error () == 1) {
    printf ("Error: mpfi_is_error() returns 1 after calling "
            "mpfi_set_error(2).\n");
    exit (1);
  }
  MPFI_ERROR ("[2] this should not be printed!");
  fflush (stderr);
  if (read_stderr (stream)) {
    printf ("Error: MPFI_ERROR prints a message while error number is "
            "set.\n");
    exit (1);
  }
  if (mpfi_is_error () == 1) {
    printf ("Error: MPFI_ERROR set error number to 1 while it was already "
            "set.\n");
    exit (1);
  }

  mpfi_set_error (1);
  if (mpfi_is_error () != 1) {
    printf ("Error: mpfi_is_error() does not return 1 after calling "
            "mpfi_set_error(1).\n");
    exit (1);
  }
  MPFI_ERROR ("[3] this should not be printed!");
  fflush (stderr);
  if (read_stderr (stream)) {
    printf ("Error: MPFI_ERROR prints a message while error number is "
            "set.\n");
    exit (1);
  }

  mpfi_reset_error ();
  if (mpfi_is_error () == 1) {
    printf ("Error: mpfi_is_error() returns 1 after a call to "
            "mpfi_reset_error.\n");
    exit (1);
  }
  MPFI_ERROR ("[4] this should be printed!");
  fflush (stderr);
  if (!read_stderr (stream)) {
    printf ("Error: MPFI_ERROR does not print message after a call to "
            "mpfi_reset_error.\n");
    exit (1);
  }
  if (mpfi_is_error () != 1) {
    printf ("Error: MPFI_ERROR dos not set error number to 1 after a call to "
            "mpfi_reset_error.\n");
    exit (1);
  }

  fclose (stream);
  fclose (stderr);
  unlink ("error.tmp");

  return 0;
}
