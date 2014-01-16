/* verror.c: TeX Live specific definition

   Copyright 2014 Peter Breitenlohner.
 
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

#include <config.h>
#include <verror.h>

void
verror (int status, int errnum, const char *format, va_list args)
{
  (void) errnum;
  fflush (stdout);
  vfprintf (stderr, format, args);
  fputc ('\n', stderr);
  exit (status);
}
