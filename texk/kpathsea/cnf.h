/* cnf.h: runtime config files.

   Copyright 1994, 1995, 2008, 2012, 2019 Karl Berry.
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

#ifndef KPATHSEA_CNF_H
#define KPATHSEA_CNF_H

#include <kpathsea/c-proto.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Return the value in the last-read cnf file for VAR, or NULL if none.
   On the first call, also read all the `texmf.cnf' files in the path
   for kpse_cnf_format (and initialize the path).  Any error messages are
   written to stderr.  */

extern KPSEDLL const_string kpathsea_cnf_get (kpathsea kpse, const_string var);


/* Parse L as a texmf.cnf configuration line for the KPSE instance,
   and insert any <variable> assignment in the environment, both as-is and
   with the value (if all non-NULL) of <variable> "_" KPSE->program_name. 
   If <variable>[_<progname>] is already set in the environment, it is
   overwritten.
   
   All of this behavior is because if the user runs tex with
   --cnf-line=TEXINPUTS=/foo:, it's not unreasonable to expect that
   cmdline option to override both environment variables TEXINPUTS and
   TEXINPUTS_tex, and also both configuration values TEXINPUTS and
   TEXINPUTS.tex -- even though ordinarily <var>.<progname> overrides
   plain <var>. 
   
   Any error message is written to stderr.  */

extern KPSEDLL void kpathsea_cnf_line_env_progname (kpathsea kpse,
                                                    string l);

#if defined(KPSE_COMPAT_API)
extern KPSEDLL const_string kpse_cnf_get (const_string var);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_CNF_H */
