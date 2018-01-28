/* readable.h: is a file readable?

   Copyright 1993, 2008, 2009, 2018 Karl Berry.
   Copyright 1999, 2005 Olaf Weber.

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

#ifndef KPATHSEA_READABLE_H
#define KPATHSEA_READABLE_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* If NAME is readable and is a regular file, return it (as is).  If the
   error is ENAMETOOLONG, truncate any too-long path components, and if
   the result is a readable file, return that (in the same memory, since
   it only got shorter).  Otherwise return NULL.  */
extern KPSEDLL string kpathsea_readable_file (kpathsea kpse, string name);

#if defined (KPSE_COMPAT_API)
extern KPSEDLL string kpse_readable_file (string name);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_READABLE_H */
