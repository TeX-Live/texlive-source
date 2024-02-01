/* tset_str.c -- Test file for mpfi_set_str.

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
check_endpoints (mpfi_ptr i, const char *left, const char *right)
{
  mpfr_t l,r;

  mpfr_inits2 (mpfi_get_prec (i), l, r, (mpfr_ptr)0);

  mpfr_set_str (l, left, 0, MPFI_RNDD);
  mpfr_set_str (r, right, 0, MPFI_RNDU);

  if (!mpfr_equal_p (&(i->left), l) || !mpfr_equal_p (&(i->right), r)) {
    printf ("Error in mpfi_set_str\nexpected [%s, %s]\n     got ",
            left, right);
    mpfi_out_str (stdout, 16, 0, i);
    putchar ('\n');
    exit (1);
  }

  mpfr_clears (l, r, (mpfr_ptr)0);
}

void
check_str (mpfi_ptr fi, const char *value, const char *left, const char *right)
{
  int ret;
  ret = mpfi_set_str (fi, value, 0);
  if (ret) {
    printf ("Error when parsing \"%s\"\n", value);
    exit (1);
  }
  check_endpoints (fi, left, right);
}

void
check_base (mpfi_ptr fi, const char *value, int base,
	    const char *left, const char *right)
{
  int ret;
  ret = mpfi_set_str (fi, value, base);
  if (ret) {
    printf ("Error when parsing \"%s\"\n", value);
    exit (1);
  }
  check_endpoints (fi, left, right);
}

void
check_invalid_string (mpfi_ptr fi, const char* s, int base)
{
  int ret;

  ret = mpfi_set_str (fi, s, base);
  if (ret != 1) {
    printf ("Error when parsing \"%s\":"
	    " the input string should be considered invalid\n", s);
    exit (1);
  }
}

int
main (int argc, char **argv)
{
  int ret;
  mpfi_t fi;

  mpfi_init2 (fi, 1024);

  fclose (stderr); /* mute (internal) error messages */

  /* invalid inputs */
  check_invalid_string (fi, "",         10);
  check_invalid_string (fi, "     ",    10);
  check_invalid_string (fi, ",",        10);
  check_invalid_string (fi, "[]",       10);
  check_invalid_string (fi, "[,]",      10);
  check_invalid_string (fi, "[  ",      10);
  check_invalid_string (fi, "[0  ",     10);
  check_invalid_string (fi, "[1  , ",   10);
  check_invalid_string (fi, "[ 2,",     10);
  check_invalid_string (fi, "[3,3.21",  10);
  check_invalid_string (fi, "[+4, 44 ", 10);
  check_invalid_string (fi, "[-5,5[",   10);
  check_invalid_string (fi, "[-6,6 [",  10);

  check_invalid_string (fi, "[0]",      10);
  check_invalid_string (fi, "[-1,1,1]", 10);
  check_invalid_string (fi, "[2e+2,]",  10);
  check_invalid_string (fi, "[,300]",   10);
  check_invalid_string (fi, "[+-4,4]",  10);
  check_invalid_string (fi, "[5e+,-5]", 10);
  check_invalid_string (fi, "[+,6]",    10);
  check_invalid_string (fi, "7[1,1]",   10);
  check_invalid_string (fi, "[8 8]",    10);
  check_invalid_string (fi, "[-0x9,9]", 10);
  check_invalid_string (fi, "17 invalid-string", 10);

  check_invalid_string (fi, "[1, 2]", 2);
  check_invalid_string (fi, "[1, 3]", 3);
  check_invalid_string (fi, "[1, 4]", 4);
  check_invalid_string (fi, "[1, 5]", 5);
  check_invalid_string (fi, "[1, 6]", 6);
  check_invalid_string (fi, "[1, 7]", 7);
  check_invalid_string (fi, "[1, 8]", 8);
  check_invalid_string (fi, "[1, 9]", 9);
  check_invalid_string (fi, "[1, a]", 10);
  check_invalid_string (fi, "[1, b]", 11);
  check_invalid_string (fi, "[1, c]", 12);
  check_invalid_string (fi, "[1, d]", 13);
  check_invalid_string (fi, "[1, e]", 14);
  check_invalid_string (fi, "[1, f]", 15);
  check_invalid_string (fi, "[1, g]", 16);
  check_invalid_string (fi, "[1, h]", 17);
  check_invalid_string (fi, "[1, i]", 18);
  check_invalid_string (fi, "[1, j]", 19);
  check_invalid_string (fi, "[1, k]", 20);
  check_invalid_string (fi, "[1, l]", 21);
  check_invalid_string (fi, "[1, m]", 22);
  check_invalid_string (fi, "[1, n]", 23);
  check_invalid_string (fi, "[1, o]", 24);
  check_invalid_string (fi, "[1, p]", 25);
  check_invalid_string (fi, "[1, q]", 26);
  check_invalid_string (fi, "[1, r]", 27);
  check_invalid_string (fi, "[1, s]", 28);
  check_invalid_string (fi, "[1, t]", 29);
  check_invalid_string (fi, "[1, u]", 30);
  check_invalid_string (fi, "[1, v]", 31);
  check_invalid_string (fi, "[1, w]", 32);
  check_invalid_string (fi, "[1, x]", 33);
  check_invalid_string (fi, "[1, y]", 34);
  check_invalid_string (fi, "[1, z]", 35);


  /* real number inputs */
  check_str (fi, "0", "0", "0");
  check_str (fi, "-1", "-1", "-1");
  check_str (fi, "2e-1", "2e-1", "2e-1");

  /* special values */
  ret = mpfi_set_str (fi, "   [@nan@,@nan@]", 0);
  if (ret) {
    printf ("Error when parsing \"[@nan@,@nan@]\"\n");
    exit (1);
  }
  if (!mpfr_nan_p (&(fi->left)) || !mpfr_nan_p (&(fi->right))) {
    printf ("Error: mpfi_set_str does not accept NAN\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  ret = mpfi_set_str (fi, "[ 10 , @nan@ ] ", 0);
  if (ret) {
    printf ("Error when parsing \" [ 10 , @nan@ ] \"\n");
    exit (1);
  }
  if (!mpfr_nan_p (&(fi->right))) {
    printf ("Error: mpfi_set_str does not accept NAN\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  ret = mpfi_set_str (fi, "[@nan@, 20]", 0);
  if (ret) {
    printf ("Error when parsing \"[ @nan@, 20]\"\n");
    exit (1);
  }
  if (!mpfr_nan_p (&(fi->left))) {
    printf ("Error: mpfi_set_str does not accept NAN\ngot: ");
    mpfi_out_str (stdout, 10, 0, fi);
    putchar ('\n');
    exit (1);
  }
  check_str (fi, "[-@inf@, +@inf@]", "-@inf@", "+@inf@");
  check_str (fi, "[+@inf@, -@inf@]", "+@inf@", "-@inf@");
  check_str (fi, "[+0, -0]", "+0", "-0");
  /* no guarantee on the validity of the returned interval */
  check_str (fi, "[-0,+0]", "-0", "+0");


  /* check regular values */
  mpfi_set_prec (fi, 2);
  check_str (fi, "[0x80001,0x80001]", "0b10p+18", "0b11p+18");
  check_str (fi, "[-0x80001, 0x80001]", "-0b11p+18", "0b11p+18");
  mpfi_set_prec (fi, 16);
  check_str (fi, " [0x80001,0xA2310]", "0x8000@+1", "0xa2310");
  check_str (fi, "[  -0xfffff  ,  -0x80001  ] ", "-0x100000", "-0x80000");
  mpfi_set_prec (fi, 20);
  check_str (fi, "[-0x80001, 0x80001]", "-0x80001", "0x80001");


  /* check bases */
  mpfi_set_prec (fi, 53);
  check_base (fi, "101010101010101010101010101010101010101010101010101010101",
	      2, "0x155555555555550", "0x155555555555560");
  check_base (fi, "[-10, 10] -ignored-", 3, "-0x3", "+0x3");
  check_base (fi, "[-1@4, 1@40]", 4, "-0x100", "0x1@20");
  check_base (fi, "[-1@53, -1@10]", 5, "-0x85A36366EB71F8@17", "-0x9502F9");
  check_base (fi, "[-1@53, 10]", 6, "-0x20111FB4CE3C18@21", "0x6");
  check_base (fi, "[-1@53, 10]", 7, "-0x1BA95C5079317E@24", "0x7");
  check_base (fi, "[-1@53, 17]", 8, "-0x8@39", "0xf");
  check_base (fi, "[-1@53, 17]", 9, "-0x10112449B5FECD@29", "0x10");
  check_base (fi, "[-1@53, 17]", 16, "-0x1@53", "0x17");
  check_base (fi, "[-baba, zaza]", 36, "-0x808f6", "0x192256");

  mpfi_clear (fi);

  return 0;
}
