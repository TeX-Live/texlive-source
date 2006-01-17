/* tex-hush.c: are we suppressing warnings?

    Copyright 1998, 2000, 2005 Olaf Weber.
    Copyright 1996 Karl Berry.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <kpathsea/config.h>

#include <kpathsea/pathsearch.h>
#include <kpathsea/tex-hush.h>
#include <kpathsea/variable.h>

boolean
kpse_tex_hush P1C(const_string, what)
{
  string h;
  string hush = kpse_var_value ("TEX_HUSH");
  if (hush) {
    if (STREQ (hush, "all"))
        return true;
    if (STREQ (hush, "none"))
        return false;
    for (h = kpse_path_element (hush); h; h = kpse_path_element (NULL)) {
      /* Don't do anything special with empty elements.  */
      if (STREQ (h, what))
        return true;
    }
  }
  
  return false;
}
