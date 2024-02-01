/* type_iiu.c -- Test functions associated with functions of the type
                mpfi_f (mpfi_t, mpfi_t, unsigned long).

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
read_line_iiu (mpfi_function_ptr this, FILE* fp)
{
  test_line_number = line_number;
  /* [1] return value */
  read_exactness (fp, &(MPFI_FUN_ARG (*this, 1, i)));
  /* [2] expected value */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 2, mpfi));
  /* [3] mpfi_t operand */
  read_mpfi (fp, MPFI_FUN_ARG (*this, 3, mpfi));
  /* [4] unsigned integral operand */
  read_ui (fp, &(MPFI_FUN_ARG (*this, 4, ui)));
}

/* check function against data at different precisions and test if an
   input variable can be reused as output */

void
check_line_iiu (mpfi_function_ptr this)
{
  int inex;

  /* rename operands for better readability */
  IIU_fun  f_IIU    = MPFI_FUN_GET (*this, IIU);
  mpfi_ptr got      = MPFI_FUN_ARG (*this, 0, mpfi);
  int expected_inex = MPFI_FUN_ARG (*this, 1, i);
  mpfi_ptr expected = MPFI_FUN_ARG (*this, 2, mpfi);
  mpfi_ptr op1      = MPFI_FUN_ARG (*this, 3, mpfi);
  unsigned long op2 = MPFI_FUN_ARG (*this, 4, ui);

  mpfi_set_prec (got, mpfi_get_prec (expected));

  inex = f_IIU (got, op1, op2);

  if (inex != expected_inex || !same_value (got, expected)) {
    printf ("Failed line %lu.\nop1 = ", test_line_number);
    mpfi_out_str (stdout, 16, 0, op1);
    printf ("\nop2 = %lu", op2);
    printf ("\ngot      = ");
    mpfi_out_str (stdout, 16, 0, got);
    printf ("\nexpected = ");
    mpfi_out_str (stdout, 16, 0, expected);
    putchar ('\n');
    if (inex != expected_inex)
      printf ("inexact flag: got = %u, expected = %u\n", inex, expected_inex);

    exit (1);
  }

  /* reuse input variable as output */
  if (mpfi_get_prec (got) == mpfi_get_prec (op1)) {
    mpfi_set (got, op1);

    inex = f_IIU (got, got, op2);

    if (inex != expected_inex || !same_value (got, expected)) {
      printf ("Error when reusing first input argument as output (line %lu)."
              "\nop1 = ", test_line_number);
      mpfi_out_str (stdout, 16, 0, op1);
      printf ("\nop2 = %lu", op2);
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
random_iiu (mpfi_function_ptr this)
{
  /* rename operands for better readability */
  IIU_fun f_IIU = MPFI_FUN_GET (*this, IIU);
  RRU_fun f_RRU = MPFI_FUN_MPFR_FUNCTION (*this, IIU);
  mpfi_ptr b    = MPFI_FUN_ARG (*this, 2, mpfi);
  mpfi_ptr a    = MPFI_FUN_ARG (*this, 3, mpfi);
  unsigned long ui;
  /* reuse endpoint as mpfr_t */
  mpfi_ptr i    = MPFI_FUN_ARG (*this, 0, mpfi);
  mpfr_ptr x    = &(i->left);
  mpfr_ptr y    = &(i->right);

  ui = random_ui ();
  random_interval (a);
  if (this->random_domain != NULL) {
    /* restrict the range of random interval to speed up tests */
    this->random_domain (a);
  }
  mpfi_alea (x, a);
  f_IIU (b, a, ui);
  f_RRU (y, x, ui, MPFI_RNDD);
  if (!mpfr_nan_p (y) && !MPFI_NAN_P (b) && !mpfi_is_inside_fr (y, b)) {
    printf ("Error: the interval b, image of (a, n), does not contain "
            "the point y, image of (x, n) where x is in a.\na = ");
    mpfi_out_str (stdout, 10, 0, a);
    printf ("\nn = %lu\nb = ", ui);
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
set_prec_iiu (mpfi_function_ptr this, mpfr_prec_t prec)
{
  mpfi_set_prec (MPFI_FUN_ARG (*this, 0, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 2, mpfi), prec);
  mpfi_set_prec (MPFI_FUN_ARG (*this, 3, mpfi), prec);
}

void
clear_iiu (mpfi_function_ptr this)
{
  /* [0] initial value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 0, mpfi));
  /* [1] return value (int), needs no deallocation */
  /* [2] expected value (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 2, mpfi));
  /* [3] operand (mpfi_t) */
  mpfi_clear (MPFI_FUN_ARG (*this, 3, mpfi));
  /* [4] operand (unsigned long), needs no deallocation */

  free (MPFI_FUN_ARGS (*this));
  MPFI_FUN_ARGS (*this) = NULL;
}

/* In operands array, variables are in the same order as for data in
   '.dat' files plus one additional variable before them. */
void
mpfi_fun_init_IIU (mpfi_function_ptr this, IIU_fun mpfi_function,
                   RRU_fun mpfr_function)
{
  this->type = IIU;
  this->func.IIU = mpfi_function;
  this->mpfr_func.IIU = mpfr_function;
  this->random_domain = NULL;

  /* init operands */
  MPFI_FUN_ARGS (*this) =
    (mpfi_fun_operand_t*) malloc (5 * sizeof (mpfi_fun_operand_t));

  /* [0] initial value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 0, mpfi), 1024);
  /* [1] return value (int), needs no initialization */
  /* [2] expected value (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 2, mpfi), 1024);
  /* [3] first operand (mpfi_t) */
  mpfi_init2 (MPFI_FUN_ARG (*this, 3, mpfi), 1024);
  /* [4] second operand (unsigned long), needs no initialization */

  /* init methods */
  this->set_prec   = set_prec_iiu;
  this->read_line  = read_line_iiu;
  this->check_line = check_line_iiu;
  this->random     = random_iiu;
  this->clear      = clear_iiu;
}
