/* xbasename.c: return the last element in a path.

   Copyright 1992, 1994, 1995, 1996, 2008, 2011, 2016, 2017 Karl Berry.
   Copyright 2005 Olaf Weber.

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

/* Have to include this first to get c-auto.h.  */
#include <kpathsea/config.h>

#include <kpathsea/c-pathch.h>

/* Return NAME with any leading path stripped off.  This returns a
   pointer into NAME.  For example, `basename ("/foo/bar.baz")'
   returns "bar.baz".  */

const_string
xbasename (const_string name)
{
    const_string base = name;
    const_string p;

    if (NAME_BEGINS_WITH_DEVICE(name))
        base += 2;

    else if (IS_UNC_NAME(name)) {
        unsigned limit;

        for (limit = 2; name[limit] && !IS_DIR_SEP (name[limit]); limit++)
            ;
        if (name[limit++] && name[limit] && !IS_DIR_SEP (name[limit])) {
            for (; name[limit] && !IS_DIR_SEP (name[limit]); limit++)
                ;
        } else
            /* malformed UNC name, backup */
            limit = 0;
        base += limit;
    }

    for (p = base; *p; p++) {
        if (IS_DIR_SEP(*p))
            base = p + 1;
#if defined(WIN32) && defined (KPSE_COMPAT_API)
        else if (IS_KANJI(p))
            p++;
#endif
    }

    return base;
}
