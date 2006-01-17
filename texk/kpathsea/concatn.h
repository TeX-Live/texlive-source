/* concatn.h: concatenate a variable number of strings.
   This is a separate include file only because I don't see the point of
   having every source file include <c-vararg.h>.  The declarations for
   the other concat routines are in lib.h.

   Copyright 1999, 2005 Olaf Weber.
   Copyright 1993, 96 Karl Berry.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#ifndef KPATHSEA_CONCATN_H
#define KPATHSEA_CONCATN_H

#include <kpathsea/c-proto.h>
#include <kpathsea/c-vararg.h>
#include <kpathsea/types.h>

/* Concatenate a null-terminated list of strings and return the result
   in malloc-allocated memory.  */
extern KPSEDLL string concatn PVAR1H(const_string str1);

#endif /* not KPATHSEA_CONCATN_H */
