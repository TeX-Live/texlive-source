/* c-limits.h: include the system parameter file.

   Copyright 1992, 1993, 1996, 2008 Karl Berry.
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

#ifndef C_LIMITS_H
#define C_LIMITS_H

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#include <kpathsea/systypes.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#endif /* not HAVE_LIMITS_H */

/* Some systems may have the floating-point limits in the above.  */
#if defined (HAVE_FLOAT_H) && !defined (FLT_MAX)
#include <float.h>
#endif

#endif /* not C_LIMITS_H */
