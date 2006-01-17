/* readable.h: Is a file readable?

   Copyright 1999, 2005 Olaf Weber.
   Copyright 1993 Karl Berry.

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

#ifndef KPATHSEA_READABLE_H
#define KPATHSEA_READABLE_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

/* If NAME is readable and is a regular file, return it.  If the error is
   ENAMETOOLONG, truncate any too-long path components, and if the
   result is a readable file, return that.  Otherwise return NULL.  */
   
extern KPSEDLL string kpse_readable_file P1H(const_string name);

#endif /* not KPATHSEA_READABLE_H */
