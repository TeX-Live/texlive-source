/* tex-make.h: declarations for executing external scripts.

   Copyright 1993, 1994, 2008, 2010 Karl Berry.
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

#ifndef KPATHSEA_TEX_MAKE_H
#define KPATHSEA_TEX_MAKE_H

#include <kpathsea/c-proto.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Run a program to create a file named by BASE_FILE in format FORMAT.
   Return the full filename to it, or NULL.  Any other information about
   the file is passed through environment variables.  See the mktexpk
   stuff in `tex-make.c' for an example. */

extern KPSEDLL string kpathsea_make_tex (kpathsea kpse,
                                 kpse_file_format_type format,
                                 const_string base_file);

#if defined (KPSE_COMPAT_API)
extern KPSEDLL string kpse_make_tex (kpse_file_format_type format,
                             const_string base_file);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_TEX_MAKE_H */
