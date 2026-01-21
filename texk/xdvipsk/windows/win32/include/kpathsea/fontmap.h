/* fontmap.h: declarations for reading a file to define additional font names.

   Copyright 1993, 1994, 1995, 2008, 2010, 2011 Karl Berry.

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

#ifndef FONTMAP_H
#define FONTMAP_H

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

#include <kpathsea/c-proto.h>
#include <kpathsea/hash.h>
#include <kpathsea/types.h>

/* Look up KEY in all texfonts.map's in the glyph_format path, and
   return a null-terminated list of all matching entries, or NULL.  */
extern const_string *kpathsea_fontmap_lookup (kpathsea kpse, const_string key);

#endif /* MAKE_KPSE_DLL */

#endif /* not FONTMAP_H */
