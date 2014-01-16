/* progname.c: TeX Live specific definitions

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
#include <progname.h>

const char *program_name;

void
set_program_name (const char *argv0)
{
  kpse_set_program_name (argv0, "psutils");
  program_name = kpse_program_basename (argv0);
}
