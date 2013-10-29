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
  printf ("%s: Compiled with cairo version %s; using %s\n",
          argv[0], CAIRO_VERSION_STRING, cairo_version_string ());
  printf ("%s: Compiled with pixman version %s; using %s\n",
          argv[0], PIXMAN_VERSION_STRING, pixman_version_string ());
  return 0;
}
