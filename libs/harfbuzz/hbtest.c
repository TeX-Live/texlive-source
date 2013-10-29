/* hbtest.c: Basic test for libharfbuzz
 *
 * Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <hb.h>
#include <stdio.h>
#include <unicode/ucnv.h>
#include <graphite2/Font.h>

int main (int argc, char **argv)
{
  UVersionInfo icuVersion;
  char icu_version[U_MAX_VERSION_STRING_LENGTH] = "";
  int nMajor, nMinor, nBugFix;

  printf ("%s: Compiled with HarfBuzz version %s; using %s\n",
          argv[0], HB_VERSION_STRING, hb_version_string ());
  u_getVersion(icuVersion);
  u_versionToString(icuVersion, icu_version);
  printf ("%s: Compiled with ICU version %s; using %s\n",
          argv[0], U_ICU_VERSION, icu_version);
  gr_engine_version(&nMajor, &nMinor, &nBugFix);
  printf ("%s: Compiled with Graphite2 version %d.%d.%d; using %d.%d.%d\n",
          argv[0], GR2_VERSION_MAJOR, GR2_VERSION_MINOR, GR2_VERSION_BUGFIX,
          nMajor, nMinor, nBugFix);
  return 0;
}
