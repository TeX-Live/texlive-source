/* read_data.c -- Helper functions to read data files.

Copyright 2009, 2010, 2011,
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

#include <string.h>
#include "mpfi-tests.h"

char *pathname;
unsigned long line_number; /* file name with complete path and currently read
                              line; kept globally to simplify parameter
                              passing */
int nextchar; /* character appearing next in the file, may be EOF */

FILE*
open_file (const char* file_name)
{
  FILE *fp;
  char *src_dir;
  char default_srcdir[] = ".";

  src_dir = getenv ("srcdir");
  if (src_dir == NULL)
    src_dir = default_srcdir;

  pathname = (char *) malloc ((strlen (src_dir)) + strlen (file_name) + 2);
  if (pathname == NULL) {
    printf ("Cannot allocate memory\n");
    exit (1);
  }
  sprintf (pathname, "%s/%s", src_dir, file_name);
  fp = fopen (pathname, "r");
  if (fp == NULL) {
    fprintf (stderr, "Unable to open %s\n", pathname);
    exit (1);
  }

  return fp;
}

void
close_file (FILE *f)
{
  free (pathname);
  fclose (f);
}

void
init_reading (FILE* f)
{
  line_number = 1;
  nextchar = getc (f);
  skip_whitespace_comments (f);
}

/* comparisons: return true when arguments have the same value (even if both
   are NaN) */
int
same_mpfr_value (mpfr_srcptr got, mpfr_srcptr ref)
{
  if (mpfr_nan_p (got) || mpfr_nan_p (ref))
    return mpfr_nan_p (ref) && mpfr_nan_p (got);
  if (mpfr_inf_p (got))
    return mpfr_inf_p (ref) && mpfr_signbit (got) == mpfr_signbit (ref);
  if (mpfr_zero_p (got))
    return mpfr_zero_p (ref) && mpfr_signbit (got) == mpfr_signbit (ref);
  return mpfr_cmp (got, ref) == 0;
}

int
same_value (mpfi_srcptr a, mpfi_srcptr b)
{
  return same_mpfr_value (&(a->left), &(b->left))
    && same_mpfr_value (&(a->right), &(b->right));
}


/* read primitives */

/* skips characters until reaching '\n' or EOF;
   '\n' is skipped as well  */
static void
skip_line (FILE *f)
{
  while (nextchar != EOF && nextchar != '\n')
    nextchar = getc (f);
  if (nextchar != EOF) {
    line_number++;
    nextchar = getc (f);
  }
}

/* skips over whitespace if any until reaching EOF or non-whitespace  */
static void
skip_whitespace (FILE *f)
{
  while (isspace (nextchar)) {
    if (nextchar == '\n')
      line_number++;
    nextchar = getc (f);
  }
}

/* skips over all whitespace and comments, if any */
void
skip_whitespace_comments (FILE *f)
{
  skip_whitespace (f);
  while (nextchar == '#') {
    skip_line (f);
    if (nextchar != EOF)
      skip_whitespace (f);
  }
}


/* Read operand in file, and sometimes check its validity */

void
read_sign (FILE *f, int *sign)
{
  switch (nextchar) {
  case '-':
    *sign = -1;
    break;
  case '0':
    *sign = 0;
    break;
  case '+':
    *sign = +1;
    break;
  default:
    printf ("Error: unexpected signedness '%c' in file '%s' line %lu\n",
            nextchar, pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  if (!isspace(nextchar)) {
    printf ("Error: unexpected character '%c' after exactness flag"
            " in file '%s' line %lu\n", nextchar, pathname, line_number);
    exit (1);
  }

  skip_whitespace_comments (f);
}

void
read_exactness (FILE *f, int *exactness)
{
  if (!isdigit (nextchar) || nextchar < '0' || nextchar > '4') {
    printf ("Error: unexpected exactness flag '%c' in file '%s' line %lu\n",
            nextchar, pathname, line_number);
    exit (1);
  }

  *exactness = nextchar - '0';
  nextchar = getc (f);
  if (!isspace(nextchar)) {
    printf ("Error: unexpected character '%c' after exactness flag"
            " in file '%s' line %lu\n", nextchar, pathname, line_number);
    exit (1);
  }

  skip_whitespace_comments (f);
}

void
read_ui (FILE *f, unsigned long *i)
{
  mpfr_t x;

  mpfr_init2 (x, 32);

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading integer "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFI_RNDD) == 0) {
    printf ("Error: Impossible to read integer in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }


  if (mpfr_fits_ulong_p (x, MPFI_RNDD))
    *i = mpfr_get_ui (x, MPFI_RNDD);
  else {
    printf ("Error: the number ");
    mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
    printf (" read in file '%s' line %lu does not fit "
            "in an unsigned long int\n",
            pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  skip_whitespace_comments (f);

  mpfr_clear (x);
}

/* problem with uintmax on some archi
void
read_uj (FILE *f, uintmax_t *i)
{
  mpfr_t x;
*/

  /* unknown constant...
  mpfr_init2 (x, INTMAX_WIDTH);
  */
/*
  mpfr_init2 (x, sizeof(uintmax_t));

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading integer "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFI_RNDD) == 0) {
    printf ("Error: Impossible to read integer in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }


  if (mpfr_fits_uintmax_p (x, MPFI_RNDD))
    *i = mpfr_get_uj (x, MPFI_RNDD);
  else {
    printf ("Error: the number ");
    mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
    printf (" read in file '%s' line %lu does not fit "
            "in an unsigned long int\n",
            pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  skip_whitespace_comments (f);

  mpfr_clear (x);
}
*/

void
read_si (FILE *f, long *i)
{
  mpfr_t x;

  mpfr_init2 (x, 32);

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading integer "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFI_RNDD) == 0) {
    printf ("Error: Impossible to read integer in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  if (mpfr_fits_slong_p (x, MPFI_RNDD))
    *i = mpfr_get_si (x, MPFI_RNDD);
  else {
    printf ("Error: the number ");
    mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
    printf (" read in file '%s' line %lu does not fit in a long int\n",
            pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  skip_whitespace_comments (f);

  mpfr_clear (x);
}

/* problem with intmax on some archi
void
read_sj (FILE *f, intmax_t *i)
{
  mpfr_t x;
*/
  /* unknown constant...
  mpfr_init2 (x, INTMAX_WIDTH);
  */
/*
  mpfr_init2 (x, sizeof(intmax_t));

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading integer "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFI_RNDD) == 0) {
    printf ("Error: Impossible to read integer in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  if (mpfr_fits_intmax_p (x, MPFI_RNDD))
    *i = mpfr_get_sj (x, MPFI_RNDD);
  else {
    printf ("Error: the number ");
    mpfr_out_str (stdout, 10, 0, x, MPFI_RNDD);
    printf (" read in file '%s' line %lu does not fit in a huge int\n",
            pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  skip_whitespace_comments (f);

  mpfr_clear (x);
}
*/
/* WARNING: when reading a double value, all roundings are towards
   minus infinity and the precision depends on the host system. */
int
read_double (FILE *f, double *d)
{
  int ret;
  mpfr_t x;

#if defined(DBL_MANT_DIG) && DBL_MANT_DIG > 53
  mpfr_init2 (x, DBL_MANT_DIG);
#else
  mpfr_init2 (x, 53);
#endif

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading double"
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFI_RNDD) == 0) {
    printf ("Error: Impossible to read double in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  *d = mpfr_get_d (x, MPFI_RNDD);
  ret = mpfr_cmp_d (x, *d); /* verify conversion */

  nextchar = getc (f);
  skip_whitespace_comments (f);

  mpfr_clear (x);

  return ret;
}

int
read_float (FILE *f, float *d)
{
  int ret;
  mpfr_t x;

#if defined(FLT_MANT_DIG) && FLT_MANT_DIG > 24
  mpfr_init2 (x, FLT_MANT_DIG);
#else
  mpfr_init2 (x, 24);
#endif

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading float"
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFI_RNDD) == 0) {
    printf ("Error: Impossible to read float in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  *d = mpfr_get_flt (x, MPFI_RNDD);
  ret = mpfr_cmp_d (x, *d); /* verify conversion */

  nextchar = getc (f);
  skip_whitespace_comments (f);

  mpfr_clear (x);

  return ret;
}

int
read_long_double (FILE *f, long double *d)
{
  int ret;
  mpfr_t x;

#if defined(LDBL_MANT_DIG) && LDBL_MANT_DIG > 64
  mpfr_init2 (x, LDBL_MANT_DIG);
#else
  mpfr_init2 (x, 64);
#endif

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading long double"
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFI_RNDD) == 0) {
    printf ("Error: Impossible to read long double in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  *d = mpfr_get_ld (x, MPFI_RNDD);
  ret = mpfr_cmp_ld (x, *d); /* verify conversion */

  nextchar = getc (f);
  skip_whitespace_comments (f);

  mpfr_clear (x);

  return ret;
}

void
read_mpz (FILE *f, mpz_ptr x)
{
  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading mpz number"
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpz_inp_str (x, f, 0) == 0) {
    printf ("Error: Impossible to read mpz number "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  skip_whitespace_comments (f);
}

void
read_mpq (FILE *f, mpq_ptr x)
{
  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading mpq number"
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpq_inp_str (x, f, 0) == 0) {
    printf ("Error: Impossible to read mpq number "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  skip_whitespace_comments (f);
}

static mpfr_prec_t
read_prec (FILE *f)
{
  unsigned long prec;
  int n;

  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading mpfr precision "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  n = fscanf (f, "%lu", &prec);
  if (ferror (f)) /* then also n == EOF */
    perror ("Error when reading precision");
  if (n == 0 || n == EOF || prec < MPFR_PREC_MIN || prec > MPFR_PREC_MAX) {
    printf ("Error: Impossible precision in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  nextchar = getc (f);
  skip_whitespace_comments (f);
  return (mpfr_prec_t) prec;
}

static void
read_mpfr_number (FILE *f, mpfr_ptr x)
{
  if (nextchar == EOF) {
    printf ("Error: Unexpected EOF when reading mpfr number"
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }
  ungetc (nextchar, f);
  if (mpfr_inp_str (x, f, 0, MPFR_RNDN) == 0) {
    printf ("Error: Impossible to read mpfr number "
            "in file '%s' line %lu\n",
            pathname, line_number);
    exit (1);
  }

  nextchar = getc (f);
  skip_whitespace_comments (f);
}

void
read_mpfr (FILE *f, mpfr_ptr x)
{
  mpfr_set_prec (x, read_prec (f));
  read_mpfr_number (f, x);
}

void
read_mpfi (FILE *f, mpfi_ptr a)
{
  mpfi_set_prec (a, read_prec (f));
  read_mpfr_number (f, &(a->left));
  read_mpfr_number (f, &(a->right));

#ifdef WARN_IF_REVERTED_ENDPOINTS
  if (mpfr_cmp (&(a->left), &(a->right)) > 0)
    printf ("Warning: reverted endpoints line %lu\n", line_number);
  if (mpfr_zero_p (&(a->left)) && mpfr_signbit (&(a->left)))
    printf ("Warning: -0 as lower bound line %lu\n", line_number);
  if (mpfr_zero_p (&(a->right)) && !mpfr_signbit (&(a->right)))
    printf ("Warning: +0 in upper bound line %lu\n", line_number);
#endif
}
