/* c-systypes.h: include <sys/types.h>.  It's too bad we need this file,
   but some systems don't protect <sys/types.h> from multiple inclusions.

   Copyright 1993, 2008 Karl Berry.

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

#ifndef C_SYSTYPES_H
#define C_SYSTYPES_H

#include <sys/types.h>

/* This is the symbol that X uses to determine if <sys/types.h> has been
   read, so we define it.  */
#define __TYPES__

#endif /* not C_SYSTYPES_H */
