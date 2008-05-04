/* types.h: general types for kpathsea.

   Copyright 1993, 1995, 1996, 2005, 2008 Karl Berry.

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

#ifndef KPATHSEA_TYPES_H
#define KPATHSEA_TYPES_H

/* Booleans.  */
/* NeXT wants to define their own boolean type.  */
#ifndef HAVE_BOOLEAN
#define HAVE_BOOLEAN
typedef int boolean;
/* `true' and `false' are reserved words in C++.  */
#ifndef __cplusplus
#ifndef true
#define true 1
#define false 0
#endif /* not true */
#endif /* not __cplusplus */
#endif /* not HAVE_BOOLEAN */

/* The X library (among other things) defines `FALSE' and `TRUE', and so
   we only want to define them if necessary, for use by application code.  */
#ifndef FALSE
#define FALSE false
#define TRUE true
#endif /* FALSE */

/* The usual null-terminated string.  */
typedef char *string;

/* A pointer to constant data.  (ANSI says `const string' is
   `char * const', which is a constant pointer to non-constant data.)  */
typedef const char *const_string;

/* A generic pointer.  */
typedef void *address;

#endif /* not KPATHSEA_TYPES_H */
