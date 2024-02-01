/* tinit_set.c -- Test file for mpfi_init_set functions.

Copyright 2009,
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

#include <math.h>
#include "mpfi-tests.h"

void
check_endpoints (mpfi_ptr i, const char *left, const char *right,
                 const char *function_name)
{
  mpfr_t l,r;

  mpfr_inits2 (mpfi_get_prec (i), l, r, (mpfr_ptr)0);

  mpfr_set_str (l, left, 0, MPFI_RNDD);
  mpfr_set_str (r, right, 0, MPFI_RNDU);

  if (!mpfr_equal_p (&(i->left), l) || !mpfr_equal_p (&(i->right), r)) {
    printf ("Error in %s\nexpected [%s, %s]\n     got ",
            function_name, left, right);
    mpfi_out_str (stdout, 16, 0, i);
    putchar ('\n');
    exit (1);
  }

  mpfr_clears (l, r, (mpfr_ptr)0);
}

void
check_ui (unsigned long ui, int expected_inex,
          const char *left, const char *right)
{
  int inex;
  mpfi_t fi;

  inex = mpfi_init_set_ui (fi, ui);
  if (inex != expected_inex) {
    printf ("Error: mpfi_init_set_ui returns unexpected value with input=%lu and "
            "output precision=%lu\nexpected return value: %u, got: %u\n",
            ui, mpfi_get_prec (fi), expected_inex, inex);
    exit (1);
  }
  check_endpoints (fi, left, right, "mpfi_init_set_ui");
  if (ui == 0 && (mpfr_signbit (&(fi->left)) || !mpfr_signbit (&(fi->right)))) {
    printf ("Error: mpfi_init_set_ui does not handle signed zeros correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  mpfi_clear (fi);
}

void
check_si (long si, int expected_inex,
          const char *left, const char *right)
{
  mpfi_t fi;
  int inex;
  inex = mpfi_init_set_si (fi, si);
  if (inex != expected_inex) {
    printf ("Error: mpfi_init_set_si returns unexpected value with input=%lu and "
            "output precision=%lu\nexpected return value: %u, got: %u\n",
            si, mpfi_get_prec (fi), expected_inex, inex);
    exit (1);
  }
  check_endpoints (fi, left, right, "mpfi_init_set_si");
  if (si == 0 && (mpfr_signbit (&(fi->left)) || !mpfr_signbit (&(fi->right)))) {
    printf ("Error: mpfi_init_set_si does not handle signed zeros correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  mpfi_clear(fi);
}

void
check_d (double d, int expected_inex,
         const char *left, const char *right)
{
  mpfi_t fi;
  int inex;
  inex = mpfi_init_set_d (fi, d);
  if (inex != expected_inex) {
    printf ("Error: mpfi_init_set_d returns unexpected value with input=%g and "
            "output precision=%lu\nexpected return value: %u, got: %u\n",
            d, mpfi_get_prec (fi), expected_inex, inex);
    exit (1);
  }
  check_endpoints (fi, left, right, "mpfi_init_set_d");
  if (d == 0.0
      && (mpfr_signbit (&(fi->left)) || !mpfr_signbit (&(fi->right)))) {
    printf ("Error: mpfi_init_set_d does not handle signed zeros correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  mpfi_clear(fi);
}

void
check_z (const char *value, int expected_inex,
         const char *left, const char *right)
{
  mpfi_t fi;
  int inex;
  mpz_t z;
  mpz_init (z);

  mpz_set_str (z, value, 0);
  inex = mpfi_init_set_z (fi, z);
  if (inex != expected_inex) {
    printf ("Error: mpfi_init_set_z returns unexpected value with input=");
    mpz_out_str (stdout, 10, z);
    printf (" and output precision=%lu\nexpected return value: %u, got: %u\n",
            mpfi_get_prec (fi), expected_inex, inex);
    exit (1);
  }
  check_endpoints (fi, left, right, "mpfi_init_set_z");

  if (mpz_cmp_ui (z,0) == 0
      && (mpfr_signbit (&(fi->left)) || !mpfr_signbit (&(fi->right)))) {
    printf ("Error: mpfi_init_set_z does not handle signed zeros correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }

  mpz_clear (z);
  mpfi_clear(fi);
}

void
check_q (const char *value, int expected_inex,
         const char *left, const char *right)
{
  mpfi_t fi;
  int inex;
  mpq_t q;
  mpq_init (q);

  mpq_set_str (q, value, 0);
  inex = mpfi_init_set_q (fi, q);
  if (inex != expected_inex) {
    printf ("Error: mpfi_init_set_q returns unexpected value with input=");
    mpq_out_str (stdout, 10, q);
    printf (" and output precision=%lu\nexpected return value: %u, got: %u\n",
            mpfi_get_prec (fi), expected_inex, inex);
    exit (1);
  }
  check_endpoints (fi, left, right, "mpfi_init_set_q");

  if (mpq_cmp_ui (q, 0, 1) == 0
      && (mpfr_signbit (&(fi->left)) || !mpfr_signbit (&(fi->right)))) {
    printf ("Error: mpfi_init_set_q does not handle signed zeros correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }

  mpq_clear (q);
  mpfi_clear(fi);
}

void
check_fr (mpfr_ptr fr, int expected_inex,
         const char *left, const char *right)
{
  mpfi_t fi;
  int inex;

  inex = mpfi_init_set_fr (fi, fr);
  if (inex != expected_inex) {
    printf ("Error: mpfi_init_set_fr returns unexpected value with input=");
    mpfr_out_str (stdout, 10, 0, fr, MPFI_RNDD);
    printf (" and output precision=%lu\nexpected return value: %u, got: %u\n",
            mpfi_get_prec (fi), expected_inex, inex);
    exit (1);
  }
  check_endpoints (fi, left, right, "mpfi_init_set_fr");

  if (mpfr_zero_p (fr)
      && (mpfr_signbit (&(fi->left)) || !mpfr_signbit (&(fi->right)))) {
    printf ("Error: mpfi_init_set_fr does not handle signed zeros correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }

  mpfi_clear(fi);
}

void
check_fi (mpfi_ptr in, int expected_inex,
          const char *left, const char *right)
{
  mpfi_t out;
  int inex;

  inex = mpfi_init_set (out, in);
  if (inex != expected_inex) {
    printf ("Error: mpfi_init_set returns unexpected value with input=");
    mpfi_out_str (stdout, 10, 0, in);
    printf (" and output precision=%lu\nexpected return value: %u, got: %u\n",
            mpfi_get_prec (out), expected_inex, inex);
    exit (1);
  }
  if (!MPFI_NAN_P (in)) {
    check_endpoints (out, left, right, "mpfi_init_set");
  }
  else if (!mpfr_nan_p (&(out->left)) || !mpfr_nan_p (&(out->right))) {
    printf ("Error: mpfi_init_set_fr does not accept NAN\ngot: ");
    mpfi_out_str (stdout, 10, 0, out);
    putchar ('\n');
    exit (1);
  }

  mpfi_clear(out);
}

int
main (int argc, char **argv)
{
  int inex;
  double d;
  mpfr_t fr;
  mpfi_t fi;

  /* check mpfi_init_set_ui */
  mpfr_set_default_prec(2);
  check_ui (0x87654321L,
            MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0b1p+31", "0b11p+30");
  mpfr_set_default_prec(16);
  check_ui (0x87654321L,
            MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0x8765@+4", "0x8766@+4");
  mpfr_set_default_prec(64);
  check_ui (0x87654321L,
            MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "0x87654321", "0x87654321");
  check_ui (0,
            MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");


  /* check mpfi_init_set_si */
  mpfr_set_default_prec(2);
  check_si (0x80001L,
            MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0b10p+18", "0b11p+18");
  check_si (-0x80001L,
            MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "-0b11p+18", "-0b10p+18");
  mpfr_set_default_prec(16);
  check_si (0x80001L,
            MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0x8000@+1", "0x8001@+1");
  check_si (-0x80001L,
            MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "-0x8001@+1", "-0x8000@+1");
  mpfr_set_default_prec(20);
  check_si (0x80001L,
            MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "0x80001", "0x80001");
  check_si (-0x80001L,
            MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "-0x80001", "-0x80001");
  check_si (0,
            MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");


  /* check mpfi_init_set_d */
  mpfr_set_default_prec(53);
  check_d (1.0e6,
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "1.0e6", "1.0e6");
  mpfr_set_default_prec(12);
  check_d (0.1,
           MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0x1999@-4", "0x199A@-4");
  check_d (-0.1,
           MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "-0x199A@-4", "-0x1999@-4");
  check_d (+0.0,
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");
  check_d (-0.0,
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");

  /* NAN, INFINITY, and DBL_MANT_DIG are symbols defined in math.h or float.h
     in C99 mode */
#ifdef NAN
  d = NAN;
  mpfi_init_set_d (fi, d);
  if (!mpfr_nan_p (&(fi->left)) || !mpfr_nan_p (&(fi->right))) {
    printf ("Error: mpfi_init_set_d does not accept NAN\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  mpfi_clear (fi);
#endif /* NAN */

#ifdef INFINITY
  d = INFINITY;
  inex = mpfi_init_set_d (fi, d);
  if (!MPFI_INF_P (fi)) {
    printf ("Error: mpfi_init_set_d does not accept INFINITY\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  if (!MPFI_BOTH_ARE_EXACT (inex)) {
    printf ("Error: mpfi_init_set_d(INF) returns unexpected value, "
            "expected: %u, got: %u\n", MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, inex);
    exit (1);
  }
  mpfi_clear (fi);
#endif /* INFINITY */

#ifdef DBL_MANT_DIG
  d = 0.1;
  mpfr_set_default_prec (DBL_MANT_DIG);
  inex = mpfi_init_set_d (fi, d);
  if (!MPFI_BOTH_ARE_EXACT (inex)) {
    printf ("Error: mpfi_init_set_d(%g) returns unexpected value, expected: %u, "
            "got: %u\n", d, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, inex);
    exit (1);
  }

# if DBL_MANT_DIG == 53
  check_endpoints (fi, "0x1999999999999A@-14", "0x1999999999999A@-14", "mpfi_init_set_d");
# endif
  mpfi_clear (fi);
#endif /* DBL_MANT_DIG */


  /* check mpfi_init_set_z */
  mpfr_set_default_prec(12);
  check_z ("0xffffffffffffff",
           MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0xffff@+10", "0x10000@+10");
  check_z ("-0xfeffffffffffff",
           MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "-0xff00@+10", "-0xfeff@+10");
  mpfr_set_default_prec(56);
  check_z ("0xffffffffffffff",
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "0xffffffffffffff", "0xffffffffffffff");
  check_z ("-0xfeffffffffffff",
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "-0xfeffffffffffff", "-0xfeffffffffffff");
  check_z ("0",
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");


  /* check mpfi_init_set_q */
  mpfr_set_default_prec(2);
  check_q ("1/2",
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "0x1p-1", "0x1p-1");
  check_q ("5/2",
           MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "2", "3");
  mpfr_set_default_prec(53);
  check_q ("-1/2",
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "-0x1p-1", "-0x1p-1");
  check_q ("-1/3",
           MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "-0xAAAAAAAAAAAABP-53", "-0x15555555555555P-54");
  check_q ("0/1",
           MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");


  /* check mpfi_init_set_fr */
  mpfr_init2 (fr, 53);

  mpfr_set_default_prec(2);
  mpfr_set_str (fr, "0.5", 10, MPFI_RNDD);
  check_fr (fr, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "0x8@-1", "0x8@-1");
  mpfr_set_str (fr, "0.1", 10, MPFI_RNDU);
  check_fr (fr, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0b11p-5", "0b10p-4");

  mpfr_set_default_prec(53);
  check_fr (fr, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT,
            "0x1999999999999A@-14", "0x1999999999999A@-14");

  mpfr_set_ui (fr, 0, MPFI_RNDD);
  check_fr (fr, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");
  mpfr_neg (fr, fr, MPFI_RNDD);
  check_fr (fr, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");
  mpfr_set_inf (fr, -1);
  check_fr (fr, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "-@inf@", "-@inf@");

  mpfr_set_nan (fr);
  mpfi_init_set_fr (fi, fr);
  if (!mpfr_nan_p (&(fi->left)) || !mpfr_nan_p (&(fi->right))) {
    printf ("Error: mpfi_init_set_fr does not accept NAN\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  mpfi_clear (fi);
  mpfr_clear (fr);


  /* check mpfi_init_set */
  mpfi_init2 (fi, 53);

  mpfr_set_default_prec(2);
  mpfi_set_str (fi, "0.5", 10);
  check_fi (fi, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "0x8@-1", "0x8@-1");
  mpfi_set_str (fi, "0.1", 10);
  check_fi (fi, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT, "0b11p-5", "0b10p-4");

  mpfr_set_default_prec(53);
  check_fi (fi,
            MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "0x19999999999999@-14", "0x1999999999999A@-14");

  mpfr_set_inf (&(fi->left), -1);
  mpfr_set_inf (&(fi->right), +1);
  check_fi (fi, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "-@inf@", "+@inf@");

  mpfr_set_ui (&(fi->left), 0, MPFI_RNDD);
  mpfr_neg (&(fi->right), &(fi->left), MPFI_RNDU);
  check_fi (fi, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "+0", "-0");

  mpfr_set_nan (&(fi->left));
  mpfr_set_nan (&(fi->right));
  check_fi (fi, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT, "@nan@", "@nan@");

  mpfi_clear (fi);

  return 0;
}
