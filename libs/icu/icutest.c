/* icutest.c: Basic test for libicu*
 *
 * Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <unicode/ucnv.h>

int main (int argc, char **argv)
{
  UVersionInfo icuVersion;
  char icu_version[U_MAX_VERSION_STRING_LENGTH] = "";

  u_getVersion(icuVersion);
  u_versionToString(icuVersion, icu_version);
  printf ("%s: Compiled with ICU version %s; using %s\n",
          argv[0], U_ICU_VERSION, icu_version);
  return 0;
}
