/* type_iiii.c -- Test functions associated with functions of the type
                mpfi_f (mpfi_t, mpfi_t, mpfi_t, mpfi_t).

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
read_line_iiii (mpfi_function_ptr this, FILE* fp)
{
  test_line_number = line_number;
  /* [1] return value */
  read_exactness (fp, &(MPFI_FUN_ARG (*this, 1, i)));
  /* [2] expected value */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 2, mpfi));
  /* [3] first operand */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 3, mpfi));
  /* [4] second operand */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 4, mpfi));
  /* [5] second operand */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 5, mpfi));
}

/* check function against data at different precisions and test if an
   input variable can be reused as output */

void
check_line_iiii (mpfi_function_ptr function)
{
  int inex;

  /* rename operands for better readability
     [0]: value set by function
     [1]: return value (inexact flag)
     [2]: expected value
     [3]: first operand
     [4]: second operand
     [5]: third operand */
  IIII_fun f_IIII = MPFI_FUN_GET (*function, IIII);
  mpfi_ptr got      = MPFI_FUN_ARG (*function, 0, mpfi);
  int expected_inex = MPFI_FUN_ARG (*function, 1, i);
  mpfi_ptr expected = MPFI_FUN_ARG (*function, 2, mpfi);
  mpfi_ptr op1      = MPFI_FUN_ARG (*function, 3, mpfi);
  mpfi_ptr op2      = MPFI_FUN_ARG (*function, 4, mpfi);
  mpfi_ptr op3      = MPFI_FUN_ARG (*function, 5, mpfi);

  mpfi_set_prec (got, mpfi_get_prec (expected));

  inex = f_IIII (got, op1, op2, op3);
  if (inex != expected_inex || !same_value (got, expected)) {
    printf ("Failed line %lu.\n", test_line_number);
    printf ("op1 = ");
    mpfi_out_str (stdout, 16, 0, op1);
    printf ("\nop2 = ");
    mpfi_out_str (stdout, 16, 0, op2);
    printf ("\ngot      = ");
    mpfi_out_str (stdout, 16, 0, op3);
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

    inex = f_IIII (got, got, op2, op3);
    if (inex != expected_inex || !same_value (got, expected)) {
      printf ("Error when reusing first input argument as output (line %lu)."
              "\nop1 = ", test_line_number);
      mpfi_out_str (stdout, 16, 0, op1);
      printf ("\nop2 = ");
      mpfi_out_str (stdout, 16, 0, op2);
      printf ("\ngot      = ");
      mpfi_out_str (stdout, 16, 0, op3);
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

  if (mpfi_get_prec (got) == mpfi_get_prec (op2)) {
    mpfi_set (got, op2);
    inex = f_IIII (got, op1, got, op3);

    if (inex != expected_inex || !same_value (got, expected)) {
      printf ("Error when reusing second argument as output (line %lu)."
              "\nop1 = ", test_line_number);
      mpfi_out_str (stdout, 16, 0, op1);
      printf ("\nop2 = ");
      mpfi_out_str (stdout, 16, 0, op2);
      printf ("\ngot      = ");
      mpfi_out_str (stdout, 16, 0, op3);
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

  if (mpfi_get_prec (got) == mpfi_get_prec (op3)) {
    mpfi_set (got, op3);
    inex = f_IIII (got, op1, op2, got);

    if (inex != expected_inex || !same_value (got, expected)) {
      printf ("Error when reusing second argument as output (line %lu)."
              "\nop1 = ", test_line_number);
      mpfi_out_str (stdout, 16, 0, op1);
      printf ("\nop2 = ");
      mpfi_out_str (stdout, 16, 0, op2);
      printf ("\ngot      = ");
      mpfi_out_str (stdout, 16, 0, op3);
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

/* Check if the image of random points chosen in the given intervals is in the
   image of these intervals. */
void
random_iiii (mpfi_function_ptr this)
{
  /* rename operands for better readability */
  IIII_fun f_IIII = MPFI_FUN_GET (*this, IIII);
  RRRR_fun f_RRRR = MPFI_FUN_MPFR_FUNCTION (*this, IIII);
  mpfi_ptr d    = MPFI_FUN_ARG (*this, 2, mpfi);
  mpfi_ptr c    = MPFI_FUN_ARG (*this, 3, mpfi);
  mpfi_ptr b    = MPFI_FUN_ARG (*this, 4, mpfi);
  mpfi_ptr a    = MPFI_FUN_ARG (*this, 5, mpfi);
  /* reuse endpoint as mpfr_t */
  mpfi_ptr i  = MPFI_FUN_ARG (*this, 0, mpfi);
  mpfr_ptr x  = &(i->left);
  mpfr_ptr y  = &(i->right);
  mpfr_t z;
  mpfr_t t;

  mpfr_init2 (z, mpfi_get_prec (c));
  mpfr_init2 (t, mpfi_get_prec (c));

  random_interval (a);
  if (this->random_domain != NULL) {
    /* restrict the range of random interval to speed up tests */
    this->random_domain (a);
  }
  mpfi_alea (x, a);
  random_interval (b);
  if (this->random_domain != NULL) {
    /* restrict the range of random interval to speed up tests */
    this->random_domain (b);
  }
  mpfi_alea (y, b);
  random_interval (c);
  if (this->random_domain != NULL) {
    /* restrict the range of random interval to speed up tests */
    this->random_domain (c);
  }
  mpfi_alea (z, c);
  f_IIII (d, a, b, c);
  f_RRRR (t, x, y, z, MPFI_RNDD);
  if (!mpfr_nan_p (t) && !MPFI_NAN_P (d) && !mpfi_is_inside_fr (t, d)) {
    printf ("Error: the image d of (a, b, c) does not contain the image t "
            "of (x, y, z) where x (resp. y (resp. z)) is in a (resp. b (resp. c)).\na = ");
    mpfi_out_str (stdout, 10, 0, a);
    printf ("\nb = ");
    mpfi_out_str (stdout, 10, 0, b);
    printf ("\nc = ");
    mpfi_out_str (stdout, 10, 0, c);
    printf ("\nx = ");
    mpfi_out_str (stdout, 10, 0, d);
    printf ("\nx = ");
    mpfr_out_str (stdout, 10, 0, x, MPFI_RNDU);
    printf ("\ny = ");
    mpfr_out_str (stdout, 10, 0, y, MPFI_RNDU);
    printf ("\nz = ");
    mpfr_out_str (stdout, 10, 0, z, MPFI_RNDU);
    putchar ('\n');
    mpfr_out_str (stdout, 10, 0, t, MPFI_RNDU);
    putchar ('\n');

    exit (1);
  }

  mpfr_clear (z);
  mpfr_clear (t);
}

void
set_prec_iiii (mpfi_function_ptr this, mpfr_prec_t prec)
{
  mpfi_set_prec (MPFI_FUN_ARG (*this, 0, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 2, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 3, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 4, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 5, mpfi), prec);
}

void
clear_iiii (mpfi_function_ptr this)
{
  /* [0] auxiliary variable (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 0, mpfi));
  /* [1] return value (int), needs no deallocation */
  /* [2] expected value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 2, mpfi));
  /* [3] first operand (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 3, mpfi));
  /* [4] second operand (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 4, mpfi));
  /* [5] third operand (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 5, mpfi));

  free (MPFI_FUN_ARGS (*this));
  MPFI_FUN_ARGS (*this) = NULL;
}

/* In operands array, variables are in the same order as for data in
   '.dat' files plus one additional variable before them. */
void
mpfi_fun_init_IIII (mpfi_function_ptr this, IIII_fun mpfi_function,
               RRRR_fun mpfr_function)
{
  this->type = IIII;
  this->func.IIII = mpfi_function;
  this->mpfr_func.IIII = mpfr_function;
  this->random_domain = NULL;

  /* init operands */
  MPFI_FUN_ARGS (*this) =
    (mpfi_fun_operand_t*) malloc (6 * sizeof (mpfi_fun_operand_t));

  /* [0] auxiliary variable (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 0, mpfi), 1024);
  /* [1] return value (int), needs no initialization */
  /* [2] expected value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 2, mpfi), 1024);
  /* [3] first operand (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 3, mpfi), 1024);
  /* [4] second operand (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 4, mpfi), 1024);
  /* [5] second operand (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 5, mpfi), 1024);

  /* init methods */
  this->set_prec   = set_prec_iiii;
  this->read_line  = read_line_iiii;
  this->check_line = check_line_iiii;
  this->random     = random_iiii;
  this->clear      = clear_iiii;
}
