/* default.h: declare default path expander.

   Copyright 1993, 1994, 2008, 2011, 2017 Karl Berry.

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

#ifndef KPATHSEA_DEFAULT_H
#define KPATHSEA_DEFAULT_H

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

#include <kpathsea/types.h>
#include <kpathsea/c-proto.h>


/* Replace a leading or trailing or doubled : in PATH with DFLT.  If
   no extra colons, return PATH.  Only one extra colon is replaced.
   If PATH is NULL or the empty string, DFLT is returned.
   DFLT may not be NULL.  The result is always in new memory.  */

extern string kpathsea_expand_default (kpathsea kpse, const_string path,
                                       const_string dflt);

#endif /* MAKE_KPSE_DLL */

#endif /* not KPATHSEA_DEFAULT_H */
