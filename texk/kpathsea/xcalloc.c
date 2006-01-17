/* xcalloc.c: calloc with error checking.

    Copyright 2005     Olaf Weber
    Copyright 1992, 93 Karl Berry.

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


void *
xcalloc P2C(unsigned, nelem,  unsigned, elsize)
{
    void *new_mem = (void*)calloc(nelem, elsize);
  
    if (new_mem == NULL) {
        fprintf(stderr,
                "xcalloc: request for %u elements of size %u failed.\n",
                nelem, elsize);
        exit(EXIT_FAILURE);
    }
  
    return new_mem;
}
