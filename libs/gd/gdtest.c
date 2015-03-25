/* pixtest.c: Basic test for libpixman
 *
 * Copyright (C) 2015 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <gd.h>

int main (int argc, char **argv)
{
  printf ("%s: Compiled with libgd version %s; using %s\n",
          argv[0], GD_VERSION_STRING, gdVersionString ());
  return 0;
}
