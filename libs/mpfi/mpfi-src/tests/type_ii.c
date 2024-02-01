/* type_ii.c -- Test functions associated with functions of the type
                mpfi_f (mpfi_t, mpfi_t).

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

extern unsigned long line_number;
static unsigned long test_line_number;   /* start line of a test */

void
read_line_ii (mpfi_function_ptr this, FILE* fp)
{
  test_line_number = line_number;
  /* [1] return value */
  read_exactness (fp, &(MPFI_FUN_ARG (*this, 1, i)));
  /* [2] expected value */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 2, mpfi));
  /* [3] operand */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 3, mpfi));
}

/* check_with_different_prec used by both check_line_ii and check_line_iii */
/* when one (at least) endpoint of the result is exact, it can be
   used to check the function at a different precision. */
void
check_with_different_prec (mpfi_function_ptr function, mpfr_prec_t prec)
{
  mpfr_t x;

  /* rename operands for better readability */
  int type_II   = MPFI_FUN_TYPE (*function) == II;
  II_fun  f_II  = MPFI_FUN_GET (*function, II);
  III_fun f_III = MPFI_FUN_GET (*function, III);
  mpfi_ptr got      = MPFI_FUN_ARG (*function, 0, mpfi);
  int expected_inex = MPFI_FUN_ARG (*function, 1, i);
  mpfi_ptr expected = MPFI_FUN_ARG (*function, 2, mpfi);
  mpfi_ptr op1      = MPFI_FUN_ARG (*function, 3, mpfi);
  mpfi_ptr op2      = type_II ? NULL : MPFI_FUN_ARG (*function, 4, mpfi);


  mpfr_init2 (x, prec);
  mpfi_set_prec (got, prec);

  if (type_II)
    f_II (got, op1);
  else
    f_III (got, op1, op2);

  if (!MPFI_LEFT_IS_INEXACT (expected_inex)) {
    mpfr_set (x, &(expected->left), MPFI_RNDD);
    if (!same_mpfr_value (x, &(got->left))) {
      printf ("Error at precision = %lu (line %lu).\n",
              (unsigned long)prec, test_line_number);
      if (type_II) {
        printf ("op = ");
        mpfi_out_str (stdout, 16, 0, op1);
      }
      else {
        printf ("op1 = ");
        mpfi_out_str (stdout, 16, 0, op1);
        printf ("\nop2 = ");
        mpfi_out_str (stdout, 16, 0, op2);
      }
      printf ("\nleft endpoint: got = ");
      mpfr_out_str (stdout,  2, 0, &(got->left), MPFI_RNDD);
      printf ("\n          expected = ");
      mpfr_out_str (stdout,  2, 0, x, MPFI_RNDD);
      putchar ('\n');

      exit (1);
    }
  }

  if (!MPFI_RIGHT_IS_INEXACT (expected_inex)) {
    mpfr_set (x, &(expected->right), MPFI_RNDU);
    if (!same_mpfr_value (x, &(got->right))) {
      printf ("Error at precision = %lu (line %lu).\n",
              (unsigned long)prec, test_line_number);
      if (type_II) {
        printf ("op = ");
        mpfi_out_str (stdout, 16, 0, op1);
      }
      else {
        printf ("op1 = ");
        mpfi_out_str (stdout, 16, 0, op1);
        printf ("\nop2 = ");
        mpfi_out_str (stdout, 16, 0, op2);
      }
      printf ("\nright endpoint: got = ");
      mpfr_out_str (stdout,  2, 0, &(got->right), MPFI_RNDU);
      printf ("\n           expected = ");
      mpfr_out_str (stdout,  2, 0, x, MPFI_RNDU);
      putchar ('\n');

      exit (1);
    }
  }

  mpfr_clear (x);
}


/* check function against data at different precisions and test if an
   input variable can be reused as output */

void
check_line_ii (mpfi_function_ptr function)
{
  int inex;

  /* rename operands for better readability
     [0]: value set by function
     [1]: return value (inexact flag)
     [2]: expected value
     [3]: first operand */
  II_fun  f_II      = MPFI_FUN_GET (*function, II);
  mpfi_ptr got      = MPFI_FUN_ARG (*function, 0, mpfi);
  int expected_inex = MPFI_FUN_ARG (*function, 1, i);
  mpfi_ptr expected = MPFI_FUN_ARG (*function, 2, mpfi);
  mpfi_ptr op1      = MPFI_FUN_ARG (*function, 3, mpfi);

  mpfi_set_prec (got, mpfi_get_prec (expected));
  inex = f_II (got, op1);
  if (inex != expected_inex || !same_value (got, expected)) {
    printf ("Failed line %lu.\n", test_line_number);
    printf ("op = ");
    mpfi_out_str (stdout, 16, 0, op1);
    printf ("\ngot      = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nexpected = ");
    mpfi_out_str (stdout, 16, 0, expected);
    putchar ('\n');
    if (inex != expected_inex)
      printf ("inexact flag: got = %u, expected = %u\n",
              inex, expected_inex);
    exit (1);
  }

  /* when one endpoint is exact, compute function at lower and higher
     precision */
  if (!MPFI_NAN_P (expected) && !MPFI_BOTH_ARE_INEXACT (inex)) {
    check_with_different_prec (function, 2);
    check_with_different_prec (function, 2 * mpfi_get_prec (expected));
    mpfi_set_prec (got, mpfi_get_prec (expected));
  }

  /* reuse input variable as output (when they have the same precision) */
  if (mpfi_get_prec (got) == mpfi_get_prec (op1)) {
    mpfi_set (got, op1);
    inex = f_II (got, got);
    if (inex != expected_inex || !same_value (got, expected)) {
      printf ("Error when reusing input argument as output (line %lu)."
              "\nop1 = ", test_line_number);
      mpfi_out_str (stdout, 16, 0, op1);
      printf ("\ngot      = ");
      mpfi_out_str (stdout, 16, 0, got);
      printf ("\nexpected = ");
      mpfi_out_str (stdout, 16, 0, expected);
      putchar ('\n');
      if (inex != expected_inex)
        printf ("inexact flag: got = %u, expected = %u\n",
                inex, expected_inex);

      exit (1);
    }
  }
}

/* Check if the image of a random point chosen in the given interval is in the
   image of this interval. */
void
random_ii (mpfi_function_ptr this)
{
  /* rename operands for better readability */
  II_fun f_II = MPFI_FUN_GET (*this, II);
  RR_fun f_RR = MPFI_FUN_MPFR_FUNCTION (*this, II);
  mpfi_ptr b  = MPFI_FUN_ARG (*this, 2, mpfi);
  mpfi_ptr a  = MPFI_FUN_ARG (*this, 3, mpfi);
  /* reuse endpoint as mpfr_t */
  mpfi_ptr i  = MPFI_FUN_ARG (*this, 0, mpfi);
  mpfr_ptr x  = &(i->left);
  mpfr_ptr y  = &(i->right);

  random_interval (a);
  if (this->random_domain != NULL) {
    /* restrict the range of random interval to speed up tests */
    this->random_domain (a);
  }
  mpfi_alea (x, a);
  f_II (b, a);
  f_RR (y, x, MPFI_RNDD);
  if (!mpfr_nan_p (y) && !MPFI_NAN_P (b) && !mpfi_is_inside_fr (y, b)) {
    printf ("Error: the image b of a does not contain the image y "
            "of the point x of a.\na = ");
    mpfi_out_str (stdout, 10, 0, a);
    printf ("\nb = ");
    mpfi_out_str (stdout, 10, 0, b);
    printf ("\nx = ");
    mpfr_out_str (stdout, 10, 0, x, MPFI_RNDU);
    printf ("\ny = ");
    mpfr_out_str (stdout, 10, 0, y, MPFI_RNDU);
    putchar ('\n');

    exit (1);
  }
}

void
set_prec_ii (mpfi_function_ptr this, mpfr_prec_t prec)
{
  mpfi_set_prec (MPFI_FUN_ARG (*this, 0, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 2, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 3, mpfi), prec);
}

void
clear_ii (mpfi_function_ptr this)
{
  /* [0] auxiliary variable (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 0, mpfi));
  /* [1] return value (int), needs no deallocation */
  /* [2] expected value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 2, mpfi));
  /* [3] operand (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 3, mpfi));

  free (MPFI_FUN_ARGS (*this));
  MPFI_FUN_ARGS (*this) = NULL;
}

/* In operands array, variables are in the same order as for data in
   '.dat' files plus one additional variable before them. */
void
mpfi_fun_init_II (mpfi_function_ptr this, II_fun mpfi_function,
                  RR_fun mpfr_function)
{
  this->type = II;
  this->func.II = mpfi_function;
  this->mpfr_func.II = mpfr_function;
  this->random_domain = NULL;

  /* init operands */
  MPFI_FUN_ARGS (*this) =
    (mpfi_fun_operand_t*) malloc (4 * sizeof (mpfi_fun_operand_t));

  /* [0] auxiliary variable (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 0, mpfi), 1024);
  /* [1] return value (int), needs no initialization */
  /* [2] expected value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 2, mpfi), 1024);
  /* [3] operand (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 3, mpfi), 1024);

  /* init methods */
  this->set_prec   = set_prec_ii;
  this->read_line  = read_line_ii;
  this->check_line = check_line_ii;
  this->random     = random_ii;
  this->clear      = clear_ii;
}
