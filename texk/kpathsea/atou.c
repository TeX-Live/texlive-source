/* atou.c: like atoi, but if the number is negative, abort.

   Copyright 1992, 1995, 2008 Karl Berry.

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

#include "config.h"

unsigned
atou (const_string s)
{
  int i = atoi (s);

  if (i < 0)
    FATAL1 ("I expected a positive number, not %d", i);
  return i;
}
