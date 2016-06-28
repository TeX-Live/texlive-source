/* find-suffix.c: return the stuff after a dot.

   Copyright 1992, 1993, 1995, 2008, 2011, 2016 Karl Berry.

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


/* Return pointer to first character after `.' in last directory element
   of NAME.  If the name is `foo' or `/foo.bar/baz', we have no extension.  */

/* The the result of strrchr(NAME, '.'), when not NULL, is a non-const
   pointer into the string NAME.  However, this is cheating (motivated
   by limitations of the C language) when the argument NAME is a
   const string, because in that case the (technically non-const) result
   from strrchr() is certainly not modifiable.

   We do not want to repeat this kind of cheating for find_suffix() and
   therefore declare find_suffix(NAME) as const.  When find_suffix(NAME)
   is non-NULL and the argument NAME is modifiable (i.e., non-const)
   then NAME+(find_suffix(NAME)-NAME) is an equivalent modifiable string
   and the pointer arithmetic is optimized away by modern compilers.  */

const_string
find_suffix (const_string name)
{
  const_string dot_pos = strrchr (name, '.');
  const_string p;

  if (dot_pos == NULL)
    return NULL;

  for (p = dot_pos + 1; *p; p++) {
    if (IS_DIR_SEP (*p))
      return NULL;
  }

  return dot_pos + 1;
}
