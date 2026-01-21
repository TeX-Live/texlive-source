/* c-namemx.h: define NAME_MAX, the maximum length of a single
   component in a filename.  No such limit may exist, or may vary
   depending on the filesystem.

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

#ifndef KPATHSEA_C_NAME_MX_H
#define KPATHSEA_C_NAME_MX_H

#include <kpathsea/c-limits.h>

/* Most likely the system will truncate filenames if it is not POSIX,
   and so we can use the BSD value here.  */
#ifndef _POSIX_NAME_MAX
#define _POSIX_NAME_MAX 255
#endif

#ifndef NAME_MAX
#define NAME_MAX _POSIX_NAME_MAX
#endif

#endif /* not KPATHSEA_C_NAME_MX_H */
