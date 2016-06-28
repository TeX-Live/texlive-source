/* xdirname.c: return the directory part of a path.

   Copyright 1999, 2008, 2011, 2016 Karl Berry.
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

/* Return directory for NAME.  This is "." if NAME contains no directory
   separators (should never happen for selfdir), else whatever precedes
   the final directory separator, but with multiple separators stripped.
   For example, `xdirname ("/foo//bar.baz")' returns "/foo".  Always
   return a new string.  */

#include <kpathsea/config.h>
#include <kpathsea/c-pathch.h>

string
xdirname (const_string name)
{
    string ret;
    unsigned limit = 0, loc;
#if defined(WIN32)
    string p;
    unsigned i, j;
#endif

    /* Ignore a NULL name. */
    if (!name)
        return NULL;

    if (NAME_BEGINS_WITH_DEVICE(name)) {
        limit = 2;
    } else if (IS_UNC_NAME(name)) {
        for (limit = 2; name[limit] && !IS_DIR_SEP (name[limit]); limit++)
#if defined(WIN32) && defined(KPSE_COMPAT_API)
            if (IS_KANJI(name+limit)) limit++
#endif
            ;
        if (name[limit++] && name[limit] && !IS_DIR_SEP (name[limit])) {
            for (; name[limit] && !IS_DIR_SEP (name[limit]); limit++)
#if defined(WIN32) && defined(KPSE_COMPAT_API)
                if (IS_KANJI(name+limit)) limit++
#endif
                ;
            limit--;
        } else
            /* malformed UNC name, backup */
            limit = 0;
    }

#if defined(WIN32)
    j = loc = limit;
    if (j > 2) j++;
    for (i = j; name[i]; i++) {
        if (IS_DIR_SEP (name[i])) {
            j = i;
            for (i++; IS_DIR_SEP (name[i]); i++)
                ;
            loc = i + 1;
        }
#if defined (KPSE_COMPAT_API)
        else if (IS_KANJI(name+i)) i++;
#endif
    }
#else
    for (loc = strlen (name); loc > limit && !IS_DIR_SEP (name[loc-1]); loc--)
        ;
#endif

    if (loc == limit) {
        if (limit == 0)
            ret = xstrdup (".");
        else if (limit == 2) {
            ret = (string)xmalloc(4);
            ret[0] = name[0];
            ret[1] = name[1];
            ret[2] = '.';
            ret[3] = '\0';
        } else {
            /* UNC name is "//server/share".  */
            ret = xstrdup (name);
        }
    } else {
        /* If have ///a, must return /, so don't strip off everything.  */
#if defined(WIN32)
        loc = j;
        if (loc == limit && IS_DIR_SEP (name[loc])) loc++;
#else
        while (loc > limit+1 && IS_DIR_SEP (name[loc-1])) {
            loc--;
        }
#endif
        ret = (string)xmalloc(loc+1);
        strncpy(ret, name, loc);
        ret[loc] = '\0';
    }

#if defined(WIN32)
    for (p = ret; *p; p++) {
        if (*p == '\\')
            *p = '/';
#if defined (KPSE_COMPAT_API)
        else if (IS_KANJI(p))
            p++;
#endif
    }
#endif

    return ret;
}
