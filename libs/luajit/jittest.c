/* jittest.c: Basic test for libluajit
 *
 * Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <luajit.h>

int main (int argc, char **argv)
{
  printf ("%s: Compiled with %s (%s)\n",
          argv[0], LUAJIT_VERSION, LUA_VERSION);
  return 0;
}
