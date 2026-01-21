/* xopendir.h: declare checked directory operations.

   Copyright 1994, 1996, 2008, 2010 Karl Berry.

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

#if !defined (KPATHSEA_XOPENDIR_H) && (!defined (WIN32) || defined(__MINGW32__))
#define KPATHSEA_XOPENDIR_H

#include <kpathsea/c-dir.h>
#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

/* Like opendir and closedir, but abort on error.  */
extern DIR *xopendir (const_string dirname);
extern void xclosedir (DIR *);

#endif /* MAKE_KPSE_DLL */

#endif /* not (KPATHSEA_XOPENDIR_H or WIN32) */
