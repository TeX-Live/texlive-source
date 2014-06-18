/* gmptst.c: Basic test for libgmp
 *
 * Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <gmp.h>

int main (int argc, char **argv)
{
  printf ("%s: Compiled with gmp version %d.%d.%d; using %s\n",
          argv[0], __GNU_MP_VERSION, __GNU_MP_VERSION_MINOR, __GNU_MP_VERSION_PATCHLEVEL,
          gmp_version);
  return 0;
}
