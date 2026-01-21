/* expand.h: general expansion.

   Copyright 1993, 1994, 1996, 2008, 2011, 2018 Karl Berry.
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

#ifndef KPATHSEA_EXPAND_H
#define KPATHSEA_EXPAND_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

/* Call kpse_var_expand and kpse_tilde_expand (in that order).  Result
   is always in fresh memory, even if no expansions were done.  */
extern string kpathsea_expand (kpathsea kpse, const_string s);

#endif /* MAKE_KPSE_DLL */

/* Do brace expansion and call `kpathsea_expand' on each element of the
   result; return the final expansion (always in fresh memory, even if
   no expansions were done).  We don't call `kpse_expand_default'
   because there is a whole sequence of defaults to run through; see
   `kpse_init_format'.  */
extern KPSEDLL string kpathsea_brace_expand (kpathsea kpse, const_string path);

/* Do brace expansion and call `kpse_expand' on each argument of the
   result, then expand any `//' constructs.  The final expansion (always
   in fresh memory) is a path of all the existing directories that match
   the pattern. */
extern KPSEDLL string kpathsea_path_expand (kpathsea kpse, const_string path);

#if defined(KPSE_COMPAT_API)
extern KPSEDLL string kpse_brace_expand (const_string path);
extern KPSEDLL string kpse_path_expand (const_string path);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_EXPAND_H */
