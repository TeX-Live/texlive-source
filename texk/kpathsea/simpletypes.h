/* simpletypes.h: basic string, boolean, etc., that we use in kpathsea.

   This is needed to avoid a loop between types.h and str-llist.h.
   Otherwise, types.h would have to be included before str-llist.h,
   because types.h itself also includes str-llist.h, and its following
   use of str_llist_type would not be defined.  So instead we have
   str-llist.h #include this, instead of the full types.h.  All follows
   from types.h including nearly everything, so that MetaPost can be a
   threaded library.
   
   Copyright 1993, 1994, 2008, 2010, 2014 Karl Berry.

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

#ifndef KPATHSEA_SIMPLETYPES_H
#define KPATHSEA_SIMPLETYPES_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_TYPES_H */
