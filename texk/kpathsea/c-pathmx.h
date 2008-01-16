/* c-pathmx.h: define PATH_MAX, the maximum length of a filename.
   Since no such limit may exist, it's preferable to dynamically grow
   filenames as needed.

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

#ifndef KPATHSEA_C_PATH_MX_H
#define KPATHSEA_C_PATH_MX_H

#include <kpathsea/c-limits.h>

/* Cheat and define this as a manifest constant no matter what, instead
   of using pathconf.  I forget why we want to do this.  */

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

#ifndef PATH_MAX
#ifdef MAXPATHLEN
#define PATH_MAX MAXPATHLEN
#else
#define PATH_MAX _POSIX_PATH_MAX
#endif
#endif /* not PATH_MAX */


#endif /* not KPATHSEA_C_PATH_MAX_H */
