/* proginit.h: declarations for DVI driver initializations.

   Copyright 1994, 1995, 1996, 2008 Karl Berry.
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

#ifndef KPATHSEA_PROGINIT_H
#define KPATHSEA_PROGINIT_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Common initializations for DVI drivers -- check for `PREFIX'SIZES and
   `PREFIX'FONTS environment variables, setenv MAKETEX_MODE to MODE,
   etc., etc.  See the source.  */

extern KPSEDLL void
kpathsea_init_prog (kpathsea kpse, const_string prefix, unsigned dpi,
                    const_string mode, const_string fallback);

#if defined (KPSE_COMPAT_API)
extern KPSEDLL void
kpse_init_prog (const_string prefix,  unsigned dpi,  const_string mode,
                const_string fallback);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_PROGINIT_H */
