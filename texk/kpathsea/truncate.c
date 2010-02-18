/* truncate.c: truncate too-long components in a filename.

   Copyright 1993, 1995, 2008, 2010 Karl Berry.

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

#include <kpathsea/c-namemx.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/truncate.h>


/* Truncate any too-long components in NAME, returning the result.  It's
   too bad this is necessary.  See comments in readable.c for why.  */

string
kpathsea_truncate_filename (kpathsea kpse, const_string name)
{
  unsigned c_len = 0;        /* Length of current component.  */
  unsigned ret_len = 0;      /* Length of constructed result.  */
  
  /* Allocate enough space.  */
  string ret = (string) xmalloc (strlen (name) + 1);
  
  (void)kpse; /* currenty not used */

  for (; *name; name++)
    {
      if (IS_DIR_SEP (*name) || IS_DEVICE_SEP (*name))
        { /* At a directory delimiter, reset component length.  */
          c_len = 0;
        }
      else if (c_len > NAME_MAX)
        { /* If past the max for a component, ignore this character.  */
          continue;
        }

      /* Copy this character.  */
      ret[ret_len++] = *name;
      c_len++;
    }
  ret[ret_len] = 0;

  return ret;
}
