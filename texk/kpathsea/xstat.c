/* xstat.c: stat and (maybe) lstat with error checking.

   Copyright 1992, 1993, 2008 Karl Berry.
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

#include <kpathsea/config.h>

#include <kpathsea/xstat.h>


struct stat
xstat (const_string path)
{
    struct stat s;

    if (stat(path, &s) != 0)
        FATAL_PERROR(path);

    return s;
}


/* If we don't have symbolic links, lstat is the same as stat, and
   a #define is made in the include file.  */

#ifdef S_ISLNK
/*
// We declared lstat to prevent a warning during development.  This
// turns out to be more trouble than it is worth.
// extern int lstat ();
*/
struct stat
xlstat (const_string path)
{
    struct stat s;

    if (lstat(path, &s) != 0)
        FATAL_PERROR(path);
    return s;
}
#endif
