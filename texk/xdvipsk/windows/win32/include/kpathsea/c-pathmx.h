/* c-pathmx.h: define PATH_MAX, the maximum length of a filename.
   Since no such limit may exist (pace GNU Hurd), these constants should
   not actually be used; filenames should be dynamically grown as
   needed.  Most of Kpathsea does this now, but not all, and not all
   programs using it.

   Copyright 1992, 1993, 2008, 2010 Karl Berry.
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
   of using pathconf.  Maybe it is too big otherwise on Hurd?  */

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

#ifndef PATH_MAX
#ifdef MAXPATHLEN
#define PATH_MAX MAXPATHLEN
#elif defined (MAX_PATH)
#define PATH_MAX MAX_PATH
#else
#define PATH_MAX _POSIX_PATH_MAX
#endif
#endif /* not PATH_MAX */

#endif /* not KPATHSEA_C_PATH_MAX_H */
