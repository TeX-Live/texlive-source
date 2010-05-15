/* xopendir.c: opendir and closedir with error checking.

   Copyright 1992, 1993, 1994, 1995, 1996, 2008, 2010 Karl Berry.
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
#include <kpathsea/xopendir.h>


#if !defined(WIN32) || defined(__MINGW32__)
DIR *
xopendir (const_string dirname)
{
    DIR *d = opendir(dirname);

    if (d == NULL)
        FATAL_PERROR(dirname);

    return d;
}

void
xclosedir (DIR *d)
{
#ifdef CLOSEDIR_VOID
    closedir (d);
#else
    int ret = closedir(d);

    if (ret != 0)
        FATAL("closedir failed");
#endif
}
#endif /* not WIN32 || __MINGW32__ */
