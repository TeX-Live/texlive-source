/* absolute.h: Declare absolute filename predicate.

   Copyright 1999, 2005 Olaf Weber.
   Copyright 1993, 94 Karl Berry.

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

#ifndef KPATHSEA_ABSOLUTE_H
#define KPATHSEA_ABSOLUTE_H

#include <kpathsea/types.h>
#include <kpathsea/c-proto.h>


/* True if FILENAME is absolute (/foo) or, if RELATIVE_OK is true,
   explicitly relative (./foo, ../foo), else false (foo).  */

extern KPSEDLL boolean kpse_absolute_p P2H(const_string filename, boolean relative_ok);

#endif /* not KPATHSEA_ABSOLUTE_H */
