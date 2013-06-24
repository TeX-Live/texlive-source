/* potracetst.c: Basic test for libpotrace
 *
 * Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <potracelib.h>

int main (int argc, char **argv)
{
  printf ("%s: using %s\n", argv[0], potrace_version ());
  return 0;
}
