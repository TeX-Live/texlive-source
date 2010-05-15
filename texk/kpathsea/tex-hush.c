/* tex-hush.c: suppressing warnings?

   Copyright 1996, 2008 Karl Berry.
   Copyright 1998, 2000, 2005 Olaf Weber.

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

#include <kpathsea/pathsearch.h>
#include <kpathsea/tex-hush.h>
#include <kpathsea/variable.h>

boolean
kpathsea_tex_hush (kpathsea kpse, const_string what)
{
  string h;
  string hush = kpathsea_var_value (kpse, "TEX_HUSH");
  if (hush) {
    if (STREQ (hush, "all"))
        return true;
    if (STREQ (hush, "none"))
        return false;
    for (h = kpathsea_path_element (kpse, hush); h;
         h = kpathsea_path_element (kpse, NULL)) {
      /* Don't do anything special with empty elements.  */
      if (STREQ (h, what))
        return true;
    }
  }

  return false;
}

#if defined (KPSE_COMPAT_API)
boolean
kpse_tex_hush (const_string what)
{
    return kpathsea_tex_hush (kpse_def, what);
}
#endif
