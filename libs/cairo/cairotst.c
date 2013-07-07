/* cairotst.c: Basic test for libcairo
 *
 * Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <pixman.h>
#include <cairo.h>

int main (int argc, char **argv)
{
  printf ("%s: using pixman version %s\n", argv[0], pixman_version_string ());
  printf ("%s: using cairo version %s\n", argv[0], cairo_version_string ());
  return 0;
}
