/* mpfrtst.c: Basic test for libmpfr
 *
 * Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <gmp.h>
#include <mpfr.h>

int main (int argc, char **argv)
{
  printf ("%s: Compiled with mpfr version %s; using %s\n",
          argv[0], MPFR_VERSION_STRING, mpfr_get_version());
  printf ("%s: Compiled with gmp version %d.%d.%d; using %s\n",
          argv[0], __GNU_MP_VERSION, __GNU_MP_VERSION_MINOR, __GNU_MP_VERSION_PATCHLEVEL,
          gmp_version);
  return 0;
}
