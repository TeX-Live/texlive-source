/* tio_str.c -- Test mpfi_inp_str and mpfi_out_str.

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

#include "mpfi_io.h"
#include "mpfi-tests.h"

#define TMP_FILENAME "io_str.tmp"

void
check_random_interval (mpfr_prec_t prec_min, mpfr_prec_t prec_max, mpfr_prec_t step)
{
  FILE *tmp_file;
  size_t n_written, n_read;
  int base;
  mpfi_t i;
  mpfi_t read;
  mpfr_prec_t p;

  mpfi_init2 (i, prec_max);
  mpfi_init2 (read, prec_max);

  for (p = prec_min; p < prec_max; p += step) {
    mpfi_set_prec (i, p);
    mpfi_set_prec (read, p);
    random_interval (i);

    for (base = 2; base < 63; ++base) {
      tmp_file = fopen (TMP_FILENAME, "w");
      if (tmp_file == NULL) {
        printf ("Internal error: cannot open temporary file.\n");

        exit (1);
      }
      n_written = mpfi_out_str (tmp_file, base, 0, i);
      if (n_written == 0) {
        printf ("Error: mpfi_out_str (\""TMP_FILENAME"\", base, 0, I) "
                "failed.\nI = ");
        mpfi_out_str (stdout, 10, 0, i);
        printf ("\nbase = %d\n", base);

        exit (1);
      }
      if (fclose (tmp_file)) {
        printf ("Internal error: cannot close temporary file.\n");

        exit (1);
      }

      tmp_file = fopen (TMP_FILENAME, "r");
      if (tmp_file == NULL) {
        printf ("Internal error: cannot open temporary file.\n");

        exit (1);
      }
      n_read = mpfi_inp_str (read, tmp_file, base);
      /* Note: there is a bug in mpfr_out_str (used by mpfi_out_str): it does
         not return the correct number of written characters for a special
         values */
      if (mpfi_is_inside (i, read) == 0
          || (!MPFI_NAN_P (i) && !MPFI_INF_P (i) && n_read != n_written)) {
        printf ("Error: mpfi_inp_str (I, \""TMP_FILENAME"\", base) did not "
                "read correctly the output of mpfi_out_str.\n    interval "
                "read: \'");
        mpfi_out_str (stdout, 16, 0, read);
        printf ("\'\ninterval written: \'");
        mpfi_out_str (stdout, 16, 0, i);
        printf ("\'\nbase: %u\n", base);

        if (!MPFI_NAN_P (i) && !MPFI_INF_P (i) && n_read != n_written) {
          printf ("mpfi_inp_str did not read the complete output of "
                  "mpfi_out_str.\nnumber of read characters: %zu\n"
                  "expected: %zu\n", n_read, n_written);
        }

        exit (1);
      }
      if (fclose (tmp_file)) {
        printf ("Internal error: cannot close temporary file.\n");
        exit (1);
      }

    }
  }

  mpfi_clear (i);
  mpfi_clear (read);
}

void
check_file (const char *datafile)
{
  FILE *stream_mpfr;
  FILE *stream_mpfi;
  int c;

  int base;
  mpfr_prec_t p;
  mpfr_t r;
  mpfi_t i;
  size_t n_mpfr, n_mpfi;

  mpfr_init2 (r, 1024);
  mpfi_init2 (i, 1024);

  stream_mpfr = open_file (datafile);
  stream_mpfi = open_file (datafile);

  for (p = 2; p < 1024; p += 17) {
    mpfr_set_prec (r, p);
    mpfi_set_prec (i, p);
    for (base = 2; base < 63 ; ++base) {
      if (fseek (stream_mpfr, 0, SEEK_SET)) {
        printf ("Internal error: cannot reset position to the start of the "
                "file \"%s\"\n", datafile);

        exit (1);
      }
      if (fseek (stream_mpfi, 0, SEEK_SET)) {
        printf ("Internal error: cannot reset position to the start of the "
                "file \"%s\"\n", datafile);

        exit (1);
      }
      n_mpfr = mpfr_inp_str (r, stream_mpfr, base, MPFR_RNDN);
      n_mpfi = mpfi_inp_str (i, stream_mpfi, base);

      if (n_mpfr != n_mpfi) {
        printf ("Error: mpfi_inp_str does not read the same number of "
                "characters as mpfr_inp_str in file \"%s\" line 1.\n"
                "    read: %zu\nexpected: %zu\n", datafile, n_mpfi, n_mpfr);

        exit (1);
      }

      if (!mpfi_is_inside_fr (r, i)) {
        printf ("Error: when reading the file \"%s\" line 1, the number R "
                "read with\nmpfr_inp_str (R, \"%s\", %u, MPFR_RNDN)\nis not "
                "included in the interval I read with\nmpfi_inp_str "
                "(I, \"%s\", %u)\nR = ", datafile, datafile, base, datafile,
                base);
        mpfr_out_str (stdout, base, 0, r, MPFR_RNDN);
        printf ("(in base %u)\nI = ", base);
        mpfi_out_str (stdout, base, 0, i);
        printf ("(in base %u)\n", base);

        exit (1);
      }
    }

    while ((c = getc (stream_mpfr)) != EOF) {
      ungetc (c, stream_mpfr);

      n_mpfr = mpfr_inp_str (r, stream_mpfr, 10, MPFR_RNDN);
      n_mpfi = mpfi_inp_str (i, stream_mpfi, 10);

      if (n_mpfr != n_mpfi) {
        printf ("Error: mpfi_inp_str does not read the same number of "
                "characters as mpfr_inp_str in file \"%s\" line 1.\n"
                "    read: %zu\nexpected: %zu\n", datafile, n_mpfi, n_mpfr);

        exit (1);
      }

      if (!mpfi_is_inside_fr (r, i)) {
        printf ("Error: when reading the file \"%s\" line 1, the number R "
                "read with\nmpfr_inp_str (R, \"%s\", 10, MPFR_RNDN)\nis not "
                "included in the interval I read with\nmpfi_inp_str "
                "(I, \"%s\", 10)\nR = ", datafile, datafile, datafile);
        mpfr_out_str (stdout, 16, 0, r, MPFR_RNDN);
        printf ("(in base 16)\nI = ");
        mpfi_out_str (stdout, 16, 0, i);
        printf ("(in base 16)\n");

        exit (1);
      }
    }
  }

  fclose (stream_mpfr);
  fclose (stream_mpfi);

  mpfr_clear (r);
  mpfi_clear (i);
}

int
main (int argc, char **argv)
{
  test_start ();

  check_random_interval (2, 1000, 43);
  check_file ("inp_str.dat");

  test_end ();

  return 0;
}
