/* pixtest.c: Basic test for libpixman
 *
 * Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <pixman.h>

int main (int argc, char **argv)
{
  printf ("%s: using pixman %s\n", argv[0], pixman_version_string ());
  return 0;
}
