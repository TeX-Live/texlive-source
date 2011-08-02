/* truncate.h: truncate too-long components in a filename.

   Copyright 1993, 2008, 2010 Karl Berry.

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

#ifndef KPATHSEA_TRUNCATE_H
#define KPATHSEA_TRUNCATE_H

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

/* Truncate any component in NAME longer than the system NAME_MAX, and
   return the result as a malloced string.  If none, return a copy of
   NAME.  */
extern string kpathsea_truncate_filename (kpathsea kpse, const_string name);

#endif /* MAKE_KPSE_DLL */

#endif /* not KPATHSEA_TRUNCATE_H */
