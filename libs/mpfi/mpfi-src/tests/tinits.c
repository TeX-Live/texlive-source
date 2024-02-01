/* Test file for mpfi_init2, mpfi_inits, mpfi_inits2 and mpfi_clears.

Copyright 2018 Free Software Foundation, Inc.
Contributed by the AriC project, INRIA.

This file is part of the GNU MPFI Library.

The GNU MPFI Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The GNU MPFI Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MPFI Library; see the file COPYING.LESSER.  If not, see
http://www.gnu.org/licenses/ or write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA. */

#include "mpfi-tests.h"

int
main (void)
{
  mpfi_t a, b, c;
  long large_prec;


  mpfi_inits (a, b, c, (mpfi_ptr) 0);
  mpfi_clears (a, b, c, (mpfi_ptr) 0);
  mpfi_inits2 (200, a, b, c, (mpfi_ptr) 0);
  mpfi_clears (a, b, c, (mpfi_ptr) 0);

  /* test for precision 2^31-1, see
     https://gforge.inria.fr/tracker/index.php?func=detail&aid=13918 */
/* this belongs to MPFR, no need to test this here? XXX
  large_prec = 2147483647;
  if (getenv ("MPFR_CHECK_LARGEMEM") != NULL)
    {
      size_t min_memory_limit;
*/

      /* We assume that the precision won't be increased internally. */
/* this belongs to MPFR, no need to test this here? XXX
      if (large_prec > MPFR_PREC_MAX)
        large_prec = MPFR_PREC_MAX;
*/

      /* Increase tests_memory_limit if need be in order to avoid an
         obvious failure due to insufficient memory, by choosing a bit
         more than the memory used for the variables a and b. Note
         that such an increase is necessary, but is not guaranteed to
         be sufficient in all cases (e.g. with logging activated). */
/* this belongs to MPFR, no need to test this here? XXX
      min_memory_limit = 2 * (large_prec / MPFR_BYTES_PER_MP_LIMB) + 65536;
      if (tests_memory_limit > 0 && tests_memory_limit < min_memory_limit)
        tests_memory_limit = min_memory_limit;

      mpfr_inits2 (large_prec, a, b, (mpfr_ptr) 0);
      mpfr_set_ui (a, 17, MPFR_RNDN);
      mpfr_set (b, a, MPFR_RNDN);
      if (mpfr_get_ui (a, MPFR_RNDN) != 17)
        {
          printf ("Error in mpfr_init2 with precision 2^31-1\n");
          exit (1);
        }
      mpfr_clears (a, b, (mpfr_ptr) 0);
    }
*/


  return 0;
}
