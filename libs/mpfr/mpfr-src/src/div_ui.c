/* mpfr_div_ui -- divide a floating-point number by a machine integer

Copyright 1999-2017 Free Software Foundation, Inc.
Contributed by the AriC and Caramba projects, INRIA.

This file is part of the GNU MPFR Library.

The GNU MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MPFR Library; see the file COPYING.LESSER.  If not, see
http://www.gnu.org/licenses/ or write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA. */

#define MPFR_NEED_LONGLONG_H
#include "mpfr-impl.h"

/* returns 0 if result exact, non-zero otherwise */
#undef mpfr_div_ui
MPFR_HOT_FUNCTION_ATTR int
mpfr_div_ui (mpfr_ptr y, mpfr_srcptr x, unsigned long int u, mpfr_rnd_t rnd_mode)
{
  long i;
  int sh;
  mp_size_t xn, yn, dif;
  mp_limb_t *xp, *yp, *tmp, c, d;
  mpfr_exp_t exp;
  int inexact, nexttoinf;
  int middle = 1; /* middle = 0 if the next bit after {yp, yn} is 1 and others are
                     zero, middle = -1 if the next bit after {yp, yn} is 0, and
                     middle = 1 if the next bit after {yp, yn} is 1, and next bits
                     are not all zero */
  MPFR_TMP_DECL(marker);

  MPFR_LOG_FUNC
    (("x[%Pu]=%.*Rg u=%lu rnd=%d",
      mpfr_get_prec(x), mpfr_log_prec, x, u, rnd_mode),
     ("y[%Pu]=%.*Rg inexact=%d",
      mpfr_get_prec(y), mpfr_log_prec, y, inexact));

  if (MPFR_UNLIKELY (MPFR_IS_SINGULAR (x)))
    {
      if (MPFR_IS_NAN (x))
        {
          MPFR_SET_NAN (y);
          MPFR_RET_NAN;
        }
      else if (MPFR_IS_INF (x))
        {
          MPFR_SET_INF (y);
          MPFR_SET_SAME_SIGN (y, x);
          MPFR_RET (0);
        }
      else
        {
          MPFR_ASSERTD (MPFR_IS_ZERO(x));
          if (u == 0) /* 0/0 is NaN */
            {
              MPFR_SET_NAN(y);
              MPFR_RET_NAN;
            }
          else
            {
              MPFR_SET_ZERO(y);
              MPFR_SET_SAME_SIGN (y, x);
              MPFR_RET(0);
            }
        }
    }
  else if (MPFR_UNLIKELY (u <= 1))
    {
      if (u < 1)
        {
          /* x/0 is Inf since x != 0*/
          MPFR_SET_INF (y);
          MPFR_SET_SAME_SIGN (y, x);
          MPFR_SET_DIVBY0 ();
          MPFR_RET (0);
        }
      else /* y = x/1 = x */
        return mpfr_set (y, x, rnd_mode);
    }
  else if (MPFR_UNLIKELY (IS_POW2 (u)))
    return mpfr_div_2si (y, x, MPFR_INT_CEIL_LOG2 (u), rnd_mode);

  MPFR_SET_SAME_SIGN (y, x);

  MPFR_TMP_MARK (marker);
  xn = MPFR_LIMB_SIZE (x);
  yn = MPFR_LIMB_SIZE (y);

  xp = MPFR_MANT (x);
  yp = MPFR_MANT (y);
  exp = MPFR_GET_EXP (x);

  dif = yn + 1 - xn;

  /* we need to store yn+1 = xn + dif limbs of the quotient */
  /* don't use tmp=yp since the mpn_lshift call below requires yp >= tmp+1 */
  tmp = MPFR_TMP_LIMBS_ALLOC (yn + 1);

  MPFR_STAT_STATIC_ASSERT (MPFR_LIMB_MAX >= ULONG_MAX);
  if (dif >= 0)
    c = mpn_divrem_1 (tmp, dif, xp, xn, u); /* used all the dividend */
  else /* dif < 0 i.e. xn > yn, don't use the (-dif) low limbs from x */
    c = mpn_divrem_1 (tmp, 0, xp - dif, yn + 1, u);

  /* the quotient x/u is formed by {tmp, yn+1}
     + (c + {xp, dif}/B^dif) / u, where B = 2^GMP_NUMB_BITS */

  inexact = (c != 0);

  /* First pass in estimating next bit of the quotient, in case of RNDN    *
   * In case we just have the right number of bits (postpone this ?),      *
   * we need to check whether the remainder is more or less than half      *
   * the divisor. The test must be performed with a subtraction, so as     *
   * to prevent carries.                                                   */

  if (MPFR_LIKELY (rnd_mode == MPFR_RNDN))
    {
      if (c < (mp_limb_t) u - c) /* We have u > c */
        middle = -1;
      else if (c > (mp_limb_t) u - c)
        middle = 1;
      else
        middle = 0; /* exactly in the middle */
    }

  /* If we believe that we are right in the middle or exact, we should check
     that we did not neglect any word of x (division large / 1 -> small). */

  for (i = 0; (inexact == 0 || middle == 0) && i < -dif; i++)
    if (xp[i])
      inexact = middle = 1; /* larger than middle */

  /*
     If the high limb of the result is 0 (xp[xn-1] < u), remove it.
     Otherwise, compute the left shift to be performed to normalize.
     In the latter case, we discard some low bits computed. They
     contain information useful for the rounding, hence the updating
     of middle and inexact.
  */

  if (tmp[yn] == 0)
    {
      MPN_COPY(yp, tmp, yn);
      exp -= GMP_NUMB_BITS;
    }
  else
    {
      int shlz;

      count_leading_zeros (shlz, tmp[yn]);

      /* shift left to normalize */
      if (MPFR_LIKELY (shlz != 0))
        {
          mp_limb_t w = tmp[0] << shlz;

          mpn_lshift (yp, tmp + 1, yn, shlz);
          yp[0] |= tmp[0] >> (GMP_NUMB_BITS - shlz);
          /* now {yp, yn} is the approximate quotient, w is the next limb */

          if (w > MPFR_LIMB_HIGHBIT)
            { middle = 1; }
          else if (w < MPFR_LIMB_HIGHBIT)
            { middle = -1; }
          else
            { middle = (c != 0); }

          inexact = inexact || (w != 0);
          exp -= shlz;
        }
      else
        { /* this happens only if u == 1 and xp[xn-1] >=
             MPFR_LIMB_ONE << (GMP_NUMB_BITS-1). It might be better to
             handle the u == 1 case separately?
          */
             MPN_COPY (yp, tmp + 1, yn);
        }
    }

  MPFR_UNSIGNED_MINUS_MODULO (sh, MPFR_PREC (y));
  /* it remains sh bits in less significant limb of y */

  d = yp[0] & MPFR_LIMB_MASK (sh);
  yp[0] ^= d; /* set to zero lowest sh bits */

  MPFR_TMP_FREE (marker);

  if (MPFR_UNLIKELY (exp < __gmpfr_emin - 1))
    return mpfr_underflow (y, rnd_mode == MPFR_RNDN ? MPFR_RNDZ : rnd_mode,
                           MPFR_SIGN (y));

  if (MPFR_UNLIKELY (d == 0 && inexact == 0))
    nexttoinf = 0;  /* result is exact */
  else
    {
      MPFR_UPDATE2_RND_MODE(rnd_mode, MPFR_SIGN (y));
      switch (rnd_mode)
        {
        case MPFR_RNDZ:
        case MPFR_RNDF:
          inexact = - MPFR_INT_SIGN (y);  /* result is inexact */
          nexttoinf = 0;
          break;

        case MPFR_RNDA:
          inexact = MPFR_INT_SIGN (y);
          nexttoinf = 1;
          break;

        default: /* should be MPFR_RNDN */
          MPFR_ASSERTD (rnd_mode == MPFR_RNDN);
          /* We have one more significant bit in yn. */
          if (sh && d < (MPFR_LIMB_ONE << (sh - 1)))
            {
              inexact = - MPFR_INT_SIGN (y);
              nexttoinf = 0;
            }
          else if (sh && d > (MPFR_LIMB_ONE << (sh - 1)))
            {
              inexact = MPFR_INT_SIGN (y);
              nexttoinf = 1;
            }
          else /* sh = 0 or d = 1 << (sh-1) */
            {
              /* The first case is "false" even rounding (significant bits
                 indicate even rounding, but the result is inexact, so up) ;
                 The second case is the case where middle should be used to
                 decide the direction of rounding (no further bit computed) ;
                 The third is the true even rounding:
                 (a) either sh > 0 and inexact = 0
                 (a) or sh = 0 and middle = 0
              */
              if ((sh && inexact) || (!sh && middle > 0) ||
                  (((sh && !inexact) || (!sh && middle == 0))
                   && (yp[0] & (MPFR_LIMB_ONE << sh))))
                {
                  inexact = MPFR_INT_SIGN (y);
                  nexttoinf = 1;
                }
              else
                {
                  inexact = - MPFR_INT_SIGN (y);
                  nexttoinf = 0;
                }
            }
        }
    }

  if (nexttoinf &&
      MPFR_UNLIKELY (mpn_add_1 (yp, yp, yn, MPFR_LIMB_ONE << sh)))
    {
      exp++;
      yp[yn-1] = MPFR_LIMB_HIGHBIT;
    }

  /* Set the exponent. Warning! One may still have an underflow. */
  MPFR_EXP (y) = exp;

  return mpfr_check_range (y, inexact, rnd_mode);
}
