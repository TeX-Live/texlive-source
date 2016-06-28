/* make-suffix.c: unconditionally add a filename suffix.

   Copyright 1992, 1993, 1995, 2008, 2011, 2016 Karl Berry.
   Copyright 2001, 2005 Olaf Weber.

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

/* Return a new string: S suffixed with SUFFIX, regardless of what it
   was before. This returns a newly allocated string.  */

string
make_suffix (const_string s,  const_string suffix)
{
  string new_s;
  const_string dot_pos = strrchr (s, '.');
  const_string p;
#if defined(WIN32)
  string q;
#endif

  if (dot_pos)
    for (p = dot_pos + 1; *p; p++) {
      if (IS_DIR_SEP (*p)) {
        dot_pos = NULL;
        break;
      }
#if defined(WIN32) && defined(KPSE_COMPAT_API)
      else if (IS_KANJI(p))
        p++;
#endif
    }

  if (dot_pos == NULL)
    new_s = concat3 (s, ".", suffix);
  else
    {
      unsigned past_dot_index = dot_pos + 1 - s;

      new_s = (string)xmalloc (past_dot_index + strlen (suffix) + 1);
      strncpy (new_s, s, past_dot_index);
      strcpy (new_s + past_dot_index, suffix);
    }

#if defined(WIN32)
  for (q = new_s; *q; q++) {
    if (*q == '\\')
      *q = '/';
#if defined(KPSE_COMPAT_API)
    else if (IS_KANJI(q))
      q++;
#endif
  }
#endif

  return new_s;
}
