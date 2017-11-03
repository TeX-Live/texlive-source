/* kdefault.c: expand extra colons.
   (This is not named default.c because then the OSF/1 make tries to
   make a program `default' from it, since we have a target `default';
   and OSF/1 make doesn't understand .PHONY.)

   Copyright 1993, 1994, 1996, 2008, 2009, 2011, 2017 Karl Berry.
   Copyright 2002, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>

#include <kpathsea/c-pathch.h>
#include <kpathsea/default.h>


/* Check for leading colon first, then trailing, then doubled, since
   that is fastest.  Usually it will be leading or trailing.  */

string
kpathsea_expand_default (kpathsea kpse, const_string path,
                         const_string fallback)
{
  unsigned path_length;
  string expansion;

  (void) kpse; /* unused */

  /* The default path better not be null.  */
  assert (fallback);

  if (path == NULL || *path == 0)
    expansion = xstrdup (fallback);

  /* Solitary or leading :?  */
  else if (IS_ENV_SEP (*path))
    {
      expansion = path[1] == 0 ? xstrdup (fallback) : concat (fallback, path);
    }

  /* Sorry about the assignment in the middle of the expression, but
     conventions were made to be flouted and all that.  I don't see the
     point of calling strlen twice or complicating the logic just to
     avoid the assignment (especially now that I've pointed it out at
     such great length).  */
  else if (path[(path_length = strlen (path)) - 1] == ENV_SEP)
    expansion = concat (path, fallback);

  /* OK, not leading or trailing.  Check for doubled.  */
  else
    {
      const_string loc;

      for (loc = path; *loc; loc++)
        if (IS_ENV_SEP (loc[0]) && IS_ENV_SEP (loc[1]))
          break;
      if (*loc)
        { /* We have a doubled colon.  */
          expansion = (string)xmalloc (path_length + strlen(fallback) + 1);

          /* Copy stuff up to and including the first colon.  */
          strncpy (expansion, path, loc - path + 1);
          expansion[loc - path + 1] = 0;

          /* Copy in FALLBACK, and then the rest of PATH.  */
          strcat (expansion, fallback);
          strcat (expansion, loc + 1);
        }
      else
        { /* No doubled colon. */
          expansion = xstrdup(path);
        }
    }

  return expansion;
}

#ifdef TEST

void
test_expand_default (const_string path, const_string def)
{
  string answer;

  printf ("Expansion of `%s':\t", path ? path : "(nil)");
  answer = kpathsea_expand_default (kpse_def, path, def);
  puts (answer);
}

int
main ()
{
  string default_path = "default";

  test_expand_default (NULL, default_path);
  test_expand_default ("", default_path);
  test_expand_default ("none", default_path);
  test_expand_default (ENV_SEP_STRING, default_path);
  test_expand_default (ENV_SEP_STRING "first", default_path);
  test_expand_default ("last" ENV_SEP_STRING, default_path);
  test_expand_default ("middle" ENV_SEP_STRING ENV_SEP_STRING "elddim",
                       default_path);

  return 0;
}

#endif /* TEST */


/*
Local variables:
standalone-compile-command: "gcc -g -I. -I.. -DTEST kdefault.c kpathsea.a"
End:
*/
