/* rm-suffix.c: remove any suffix.

   Copyright 2008, 2011, 2012 Karl Berry.
   Copyright 1992, 1993, 1995 Free Software Foundation, Inc.
   Modified for kpathsea by Karl Berry.

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


string
remove_suffix (const_string s)
{
  string ret;
  const_string suffix = find_suffix (s);

  if (suffix)
    {
      /* Back up to before the dot.  */
      suffix--;
      ret = (string) xmalloc (suffix - s + 1);
      strncpy (ret, s, suffix - s);
      ret[suffix - s] = 0;
    }
  else
    ret = xstrdup (s);

  return ret;
}
