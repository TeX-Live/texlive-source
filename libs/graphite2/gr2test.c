/* gr2test.c: Basic test for libgraphite2
 *
 * Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <graphite2/Font.h>

int main (int argc, char **argv)
{
  int nMajor, nMinor, nBugFix;

  gr_engine_version(&nMajor, &nMinor, &nBugFix);
  printf ("%s: Compiled with Graphite2 version %d.%d.%d; using %d.%d.%d\n",
          argv[0], GR2_VERSION_MAJOR, GR2_VERSION_MINOR, GR2_VERSION_BUGFIX,
          nMajor, nMinor, nBugFix);
  return 0;
}
