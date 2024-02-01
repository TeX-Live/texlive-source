/* tset.c -- Test file for mpfi_set functions.

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

void
check_endpoints (mpfi_t i, const char *left, const char *right,
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
check_fi (mpfi_ptr out, mpfi_ptr in, int expected_inex,
	  const char *left, const char *right)
{
  int inex;

  inex = mpfi_set (out, in);
  if (inex != expected_inex) {
    printf ("Error: mpfi_set returns unexpected value with input=");
    mpfi_out_str (stdout, 10, 0, in);
    printf (" and output precision=%lu\nexpected return value: %u, got: %u\n",
	    mpfi_get_prec (out), expected_inex, inex);
    exit (1);
  }
  check_endpoints (out, left, right, "mpfi_set");
}

int
main (int argc, char **argv)
{
  mpfi_t fi1;
  mpfi_t fi2;

  mpfi_init2 (fi1, 1024);
  mpfi_init2 (fi2, 1024);

  mpfi_set_prec (fi1, 2);
  mpfi_set_prec (fi2, 53);
  mpfr_set_str (&(fi1->left),  "0.1", 0, MPFI_RNDD);
  mpfr_set_str (&(fi1->right), "0.1", 0, MPFI_RNDU);
  check_fi (fi2, fi1, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT,
	    "0b11p-5", "0b10p-4");

  mpfr_set_nan (&(fi1->left));
  mpfr_set_nan (&(fi1->right));
  mpfi_set (fi2, fi1);
  if (!mpfr_nan_p (&(fi2->left)) || !mpfr_nan_p (&(fi2->right))) {
    printf ("Error: mpfi_set does not handle NAN correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi2);
    putchar ('\n');
    exit (1);
  }

  mpfi_set_prec (fi1, 53);
  mpfi_set_prec (fi2, 2);
  mpfr_set_str (&(fi1->left),  "0.1", 0, MPFI_RNDD);
  mpfr_set_str (&(fi1->right), "0.1", 0, MPFI_RNDU);
  check_fi (fi2, fi1, MPFI_FLAGS_BOTH_ENDPOINTS_INEXACT,
	    "0x18@-2", "0x2@-1");

  mpfr_set_str (&(fi1->left),  "0.1", 0, MPFI_RNDD);
  mpfr_set_str (&(fi1->right), "0.5", 0, MPFI_RNDU);
  check_fi (fi2, fi1, MPFI_FLAGS_LEFT_ENDPOINT_INEXACT,
	    "0x18@-2", "0x8@-1");

  mpfr_set_str (&(fi1->left),  "-0.5", 0, MPFI_RNDD);
  mpfr_set_str (&(fi1->right), "-0.1", 0, MPFI_RNDU);
  check_fi (fi2, fi1, MPFI_FLAGS_RIGHT_ENDPOINT_INEXACT,
	    "-0x8@-1", "-0x18@-2");

  mpfr_set_inf (&(fi1->left),  -1);
  mpfr_set_inf (&(fi1->right), -1);
  check_fi (fi2, fi1, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT,
	    "-@inf@", "-@inf@");

  /* signed zeros */
  mpfr_set_ui (&(fi1->left), 0, MPFI_RNDU);
  mpfr_neg (&(fi1->right), &(fi1->left), MPFI_RNDD);
  check_fi (fi2, fi1, MPFI_FLAGS_BOTH_ENDPOINTS_EXACT,
	    "+0", "-0");
  if (mpfr_signbit (&(fi2->left)) || !mpfr_signbit (&(fi2->right))) {
    printf ("Error: mpfi_set does not handle signed zeros correctly\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi2);
    putchar ('\n');
    exit (1);
  }

  mpfi_clear (fi1);
  mpfi_clear (fi2);

  return 0;
}
