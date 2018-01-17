/* luatest.c: Basic test for liblua53
 *
 * Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
 * You may freely use, modify and/or distribute this file.
 */

#include <stdio.h>
#include <lua.h>

int main (int argc, char **argv)
{
  printf ("%s: Compiled with %s\n",
          argv[0], LUA_VERSION);
  return 0;
}
