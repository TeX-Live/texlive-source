/* random.c -- Handle random seed in tests.

Copyright (C) 2009, 2010,
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

/* Put test_start at the beginning of your test function and
   test_end at the end.
   These are an adaptation of those of MPFR. */

#include "mpfi-tests.h"

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifndef ULONG_MAX
# define ULONG_MAX 4294967295
#endif

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif
#ifndef DBL_MANT_DIG
# define DBL_MANT_DIG 53
#endif
#ifndef DBL_MAX_EXP
# define  DBL_MAX_EXP +1024
#endif

gmp_randstate_t  rands;
char             rands_initialized;

void
test_start (void)
{
  char *environment_seed;
  unsigned long seed;

  if (rands_initialized) {
    printf ("Put test_start ONCE at the beginning of your test function.\n");
    exit (1);
  }

  gmp_randinit_default (rands);
  rands_initialized = 1;

  environment_seed = getenv ("MPFI_CHECK_RANDOMIZE");
  if (environment_seed == NULL)
    gmp_randseed_ui (rands, 0xfac11e);
  else {
    seed = atoi (environment_seed);
    if (seed == 0 || seed == 1) {
#if HAVE_GETTIMEOFDAY
      struct timeval  tv;
      gettimeofday (&tv, NULL);
      seed = tv.tv_sec + tv.tv_usec;
#else
      time_t  tv;
      time (&tv);
      seed = tv;
#endif
      gmp_randseed_ui (rands, seed);
      printf ("Seed MPFI_CHECK_RANDOMIZE=%lu "
              "(include this in bug reports)\n", seed);
    }
    else {
      printf ("Re-seeding with MPFI_CHECK_RANDOMIZE=%lu\n", seed);
      gmp_randseed_ui (rands, seed);
    }
  }
}

void
test_end (void)
{
  if (rands_initialized) {
    rands_initialized = 0;
    gmp_randclear (rands);
  }
  else {
    printf ("Put test_start at the beginning of your test function.\n");
    exit (1);
  }

  mpfr_free_cache ();
}

unsigned long
random_ui ()
{
  return gmp_urandomm_ui (rands, ULONG_MAX);
}

long
random_si ()
{
  return (long)gmp_urandomm_ui (rands, ULONG_MAX);
}

double
random_double ()
{
  double d;
  mpfr_t x;

  mpfr_init2 (x, DBL_MANT_DIG + 1);
  mpfr_urandomb (x, rands);
  mpfr_sub_d (x, x, 0.5, MPFI_RNDD);
  mpfr_mul_2ui (x, x, DBL_MAX_EXP + 1, MPFI_RNDD);
  d = mpfr_get_d (x, MPFI_RNDD);
  mpfr_clear (x);

  return d;
}

void
random_mpz (mpz_ptr z, unsigned long n)
{
  return mpz_urandomb (z, rands, n);
}

void
random_mpq (mpq_ptr q)
{
  mpq_set_si (q, random_si (), random_ui ());
}

/* random endpoint with non-uniform distribution:
   Prob(x == -oo)     = 1/8
   Prob(-oo < x < -1) = 1/4
   Prob(-1  < x < 0)  = 1/8
   Prob(0   < x < +1) = 1/8
   Prob(+1 < x < +oo) = 1/4
   Prob(x == +oo)     = 1/8
 */
void
random_mpfr (mpfr_ptr x)
{
  unsigned long r;

  r = gmp_urandomb_ui (rands, 3);
  if (r < 1) {
    mpfr_set_inf (x, +1);
    return;
  }
  if (r < 2) {
    mpfr_set_inf (x, -1);
    return;
  }
  mpfr_urandomb (x, rands);
  if (r < 3)
    return;
  if (r < 4) {
    mpfr_neg (x, x, MPFI_RNDD);
    return;
  }
  mpfr_ui_div (x, 1, x, MPFI_RNDD);
  if (r < 6)
    return;
  mpfr_neg (x, x, MPFI_RNDD);
}

/* random interval with non-uniform distribution:
   Prob(inf == -oo)     = Prob(sup == +oo)     = 1/8 + 7/64
   Prob(-oo < inf < -1) = Prob(+1 < sup < +oo) = 3/8
   Prob(-1  < inf < 0)  = Prob(0  < sup < +1)  = 1/16 + 5/64
   Prob(0   < inf < +1) = Prob(-1 < sup < 0)   = 1/16 + 3/64
   Prob(+1 < inf < +oo) = Prob(-oo < sup < -1) = 1/8
   Prob(inf == +oo)     = Prob(sup == -oo)     = 1/64
   where inf = left endpoint, sup = right endpoint
 */
void
random_interval (mpfi_ptr i)
{
  mpfr_t x;

  mpfr_init2 (x, mpfi_get_prec (i));

  random_mpfr (x);
  mpfi_set_fr (i, x);
  random_mpfr (x);
  mpfi_put_fr (i, x);

  mpfr_clear (x);
}
