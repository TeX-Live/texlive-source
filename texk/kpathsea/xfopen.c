/* xfopen.c: fopen and fclose with error checking.

    Copyright 2005         Olaf Weber
    Copyright 1992, 93, 95 Karl Berry.

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

#include <kpathsea/config.h>


/* These routines just check the return status from standard library
   routines and abort if an error happens.  */

FILE *
xfopen P2C(const_string, filename,  const_string, mode)
{
    FILE *f;
    
    assert(filename && mode);
  
    f = fopen(filename, mode);
    if (f == NULL)
        FATAL_PERROR(filename);

    return f;
}


void
xfclose P2C(FILE *, f,  const_string, filename)
{
    assert(f);
  
    if (fclose(f) == EOF)
        FATAL_PERROR(filename);
}
