/* c-unistd.h: ensure we have constants from <unistd.h>.  Included from
   c-std.h.

   Copyright 1992, 1993, 2008, 2012 Karl Berry.
   Copyright 2005 Olaf Weber.

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

#ifndef KPATHSEA_C_UNISTD_H
#define KPATHSEA_C_UNISTD_H

/* <unistd.h> is allowed to depend on <sys/types.h>.  */
#include <kpathsea/systypes.h>

#if HAVE_UNISTD_H
#if defined (WIN32) || defined (__CYGWIN__)
# define __need_getopt
#include <kpathsea/getopt.h>
#endif
#include <unistd.h>
#endif

#include <kpathsea/c-std.h>

/* For fseek.  */
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif /* not SEEK_SET */

/* For access.  */
#ifndef F_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif /* not F_OK */

#ifndef STDIN_FILENO
#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2
#endif /* not STDIN_FILENO */

#endif /* not KPATHSEA_C_UNISTD_H */
