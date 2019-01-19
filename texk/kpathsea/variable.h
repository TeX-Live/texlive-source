/* variable.h: declare variable expander.

   Copyright 1993, 1995, 2008, 2019 Karl Berry.

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

#ifndef KPATHSEA_VARIABLE_H
#define KPATHSEA_VARIABLE_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Return the (variable-expanded) environment variable value or config
   file value, or NULL.  */
extern KPSEDLL string kpathsea_var_value (kpathsea kpse, const_string var);

/* Expand $VAR, ${VAR}, and ~ references in SRC, returning the (always newly
   dynamically-allocated) result.  An unterminated ${ or any other
   character following $ produce error messages, and that part of SRC is
   ignored.  In the $VAR form, the variable name consists of consecutive
   letters, digits, and underscores.  In the ${VAR} form, the variable
   name consists of whatever is between the braces.

   In any case, ``expansion'' means calling `getenv'; if the variable is not
   set, look in texmf.cnf files for a definition.  If not set there, either,
   the expansion of a $FOO construct is its literal text ("$FOO"), while
   the expansion of a ${BAR} construct is the empty string.  This is so
   at least some file names containing $ characters will work.  */
extern KPSEDLL string kpathsea_var_expand (kpathsea kpse, const_string src);

#if defined (KPSE_COMPAT_API)
extern KPSEDLL string kpse_var_value (const_string var);
extern KPSEDLL string kpse_var_expand (const_string src);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_VARIABLE_H */
