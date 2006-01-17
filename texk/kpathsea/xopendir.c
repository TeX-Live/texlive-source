/* xopendir.c: opendir and closedir with error checking.

    Copyright 2005 Olaf Weber.
    Copyright 1992, 93, 94, 95, 96 Karl Berry.

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
#include <kpathsea/xopendir.h>


#ifndef WIN32
DIR *
xopendir P1C(string, dirname)
{
    DIR *d = opendir(dirname);

    if (d == NULL)
        FATAL_PERROR(dirname);

    return d;
}
#endif /* not WIN32 */

void
xclosedir P1C(DIR *, d)
{
#ifdef CLOSEDIR_VOID
    closedir (d);
#else
    int ret = closedir(d);
  
    if (ret != 0)
        FATAL("closedir failed");
#endif
}

