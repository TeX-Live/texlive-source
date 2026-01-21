/* c-proto.h: macros to include or discard prototypes.

   Copyright 1992, 1993, 1995, 1996, 2008, 2009 Karl Berry.
   Copyright 1999, 2000, 2001, 2003, 2004, 2005 Olaf Weber.

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

#ifndef KPATHSEA_C_PROTO_H
#define KPATHSEA_C_PROTO_H

#if defined (WIN32) && !defined (__MINGW32__) && !defined (NO_KPSE_DLL)
#define KPSE_DLL 1
#endif /* WIN32 && ! __MINGW32__ */

#if defined (KPSE_DLL) && (defined (WIN32) || defined (__CYGWIN__))
#ifdef MAKE_KPSE_DLL
#define KPSEDLL __declspec(dllexport)
#else /* ! MAKE_KPSE_DLL */
#define KPSEDLL __declspec(dllimport)
#endif
#else /* ! (KPSE_DLL && (WIN32 || __CYGWIN__)) */
#define KPSEDLL
#endif

#endif /* not KPATHSEA_C_PROTO_H */
