/* mpfi_tests.c -- Test constant functions
                   and test non-constant functions with random values.

Copyright (C) 2001, 2002, 2009, 2010,
                     Spaces project, Inria Lorraine
                     Arenaire project, Inria Rhone-Alpes, France
                     and Lab. ANO, USTL (Univ. of Lille),  France


This file is part of the MPFI Library, based on the MPFR Library.

The MPFI Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFI Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
MA 02110-1301, USA. */

#include "mpfi-tests.h"

void
mpfi_restrict_random (mpfi_function_ptr func, I_fun restriction)
{
  func->random_domain = restriction;
}

void
mpfi_fun_clear (mpfi_function_ptr func)
{
  func->clear (func);
}

void
check_random (mpfi_function_ptr function,
              mpfr_prec_t prec_min, mpfr_prec_t prec_max, int nb_tests)
{
  mpfr_prec_t prec;
  int i;

  if (!rands_initialized)
    {
      printf ("Put test_start at the beginning of your test function.\n");
      printf ("There is a bug in the test suite itself, "
              "please report to the MPFI mailing list.\n");
      exit (1);
    }

  if (function->random == NULL) {
    printf ("Error: no random function for this type.\n");
    printf ("There is a bug in the test suite itself, "
	    "please report to the MPFI mailing list.\n");
    exit (1);
  }

  for (prec = prec_min; prec <= prec_max; ++prec) {
    function->set_prec (function, prec);
    for (i = 0; i < nb_tests; ++i) {
      function->random (function);
    }
  }
}
