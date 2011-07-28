/* xdirname.c: return the directory part of a path.

   Copyright 1999, 2008, 2011 Karl Berry.
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
#endif

    /* Ignore a NULL name. */
    if (!name)
        return NULL;

    if (NAME_BEGINS_WITH_DEVICE(name)) {
        limit = 2;
#if defined(WIN32) || defined(__CYGWIN__)
    } else if (IS_UNC_NAME(name)) {
        for (limit = 2; name[limit] && !IS_DIR_SEP(name[limit]); limit++)
            ;
        if (name[limit]) {
            for (limit++ ; name[limit] && !IS_DIR_SEP(name[limit]); limit++)
                ;
            limit--;
        } else {
            /* malformed UNC name, backup */
            limit = 2;
        }
#endif
    }

    for (loc = strlen (name); loc > limit && !IS_DIR_SEP (name[loc-1]); loc--)
        ;

    if (loc == limit && limit > 0) {
        if (limit == 2) {
            ret = (string)xmalloc(limit + 2);
            ret[0] = name[0];
            ret[1] = name[1];
            ret[2] = '.';
            ret[3] = '\0';
        } else {
            ret = (string)xmalloc(limit + 2);
            strcpy(ret, name);
        }
    } else {
        /* If have ///a, must return /, so don't strip off everything.  */
        while (loc > limit+1 && IS_DIR_SEP (name[loc-1])) {
            loc--;
        }
        ret = (string)xmalloc(loc+1);
        strncpy(ret, name, loc);
        ret[loc] = '\0';
    }

#if defined(WIN32)
    for (p = ret; *p; p++) {
        if (*p == '\\')
            *p = '/';
        else if (IS_KANJI(p))
            p++;
    }
#endif

    return ret;
}

#ifdef TEST

char *tab[] = {
    "\\\\neuromancer\\fptex\\bin\\win32\\kpsewhich.exe",
    "\\\\neuromancer\\fptex\\win32\\kpsewhich.exe",
    "\\\\neuromancer\\fptex\\kpsewhich.exe",
    "\\\\neuromancer\\kpsewhich.exe",
    "p:\\bin\\win32\\kpsewhich.exe",
    "p:\\win32\\kpsewhich.exe",
    "p:\\kpsewhich.exe",
    "p:bin\\win32\\kpsewhich.exe",
    "p:win32\\kpsewhich.exe",
    "p:kpsewhich.exe",
    "p:///kpsewhich.exe",
    "/usr/bin/win32/kpsewhich.exe",
    "/usr/bin/kpsewhich.exe",
    "/usr/kpsewhich.exe",
    "///usr/kpsewhich.exe",
    "///kpsewhich.exe",
    NULL
};

int main()
{
    char **p;
    for (p = tab; *p; p++)
        printf("name %s, dirname %s\n", *p, xdirname(*p));
    return 0;
}
#endif /* TEST */


/*
Local variables:
standalone-compile-command: "gcc -g -I. -I.. -DTEST xdirname.c kpathsea.a"
End:
*/
