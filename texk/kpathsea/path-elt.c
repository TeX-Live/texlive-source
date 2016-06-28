/* path-elt.c: return the stuff between colons.

   Copyright 1993, 1996 2008, 2016 Karl Berry.
   Copyright 1997, 2001, 2005 Olaf Weber.

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
#include <kpathsea/pathsearch.h>


/* Upon entry, the static `path' is at the first (and perhaps last)
   character of the return value, or else NULL if we're at the end (or
   haven't been called).  I make no provision for caching the results;
   thus, we parse the same path over and over, on every lookup.  If that
   turns out to be a significant lose, it can be fixed, but I'm guessing
   disk accesses overwhelm everything else.  If ENV_P is true, use
   IS_ENV_SEP; else use IS_DIR_SEP.  */

static string
element (kpathsea kpse, const_string passed_path,  boolean env_p)
{
  const_string p;
  string ret;
  int brace_level;
  unsigned len;

  if (passed_path)
    kpse->path = passed_path;
  /* Check if called with NULL, and no previous path (perhaps we reached
     the end).  */
  else if (!kpse->path)
    return NULL;

  /* OK, we have a non-null `path' if we get here.  */
  assert (kpse->path);
  p = kpse->path;

  /* Find the next colon not enclosed by braces (or the end of the path).  */
  brace_level = 0;
  while (*p != 0  && !(brace_level == 0
                       && (env_p ? IS_ENV_SEP (*p) : IS_DIR_SEP (*p)))) {
    if (*p == '{') ++brace_level;
    else if (*p == '}') --brace_level;
#if defined(WIN32)
    else if (kpathsea_IS_KANJI(kpse, p))
        p++;
#endif
    p++;
  }

  /* Return the substring starting at `path'.  */
  len = p - kpse->path;

  /* Make sure we have enough space (including the null byte).  */
  if (len + 1 > kpse->elt_alloc)
    {
      kpse->elt_alloc = len + 1;
      kpse->elt = (string)xrealloc (kpse->elt, kpse->elt_alloc);
    }

  strncpy (kpse->elt, kpse->path, len);
  kpse->elt[len] = 0;
  ret = kpse->elt;

  /* If we are at the end, return NULL next time.  */
  if (kpse->path[len] == 0)
    kpse->path = NULL;
  else
    kpse->path += len + 1;

  return ret;
}

string
kpathsea_path_element (kpathsea kpse, const_string p)
{
    return element (kpse, p, true);
}

string
kpathsea_filename_component (kpathsea kpse, const_string p)
{
    return element (kpse, p, false);
}

#ifdef TEST

void
print_path_elements (const_string path)
{
  string elt;
  printf ("Elements of `%s':", path ? path : "(null)");

  for (elt = kpathsea_path_element (kpse_def, path); elt != NULL;
       elt = kpathsea_path_element (kpse_def, NULL))
    {
      printf (" %s", *elt ? elt : "`'");
    }

  puts (".");
}

int
main ()
{
  /* All lists end with NULL.  */
  print_path_elements (NULL);   /* */
  print_path_elements ("");     /* "" */
  print_path_elements ("a");    /* a */
  print_path_elements (ENV_SEP_STRING); /* "", "" */
  print_path_elements (ENV_SEP_STRING ENV_SEP_STRING);  /* "", "", "" */
  print_path_elements ("a" ENV_SEP_STRING);     /* a, "" */
  print_path_elements (ENV_SEP_STRING "b");     /* "", b */
  print_path_elements ("a" ENV_SEP_STRING "b"); /* a, b */

  return 0;
}

#endif /* TEST */


/*
Local variables:
standalone-compile-command: "gcc -g -I. -I.. -DTEST path-elt.c kpathsea.a"
End:
*/
