/* xrealloc.c: realloc with error checking.

   Copyright 1992, 1993, 2008, 2010, 2013 Karl Berry.
   Copyright 2005 Olaf Weber.

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

#include <kpathsea/config.h>

void *
xrealloc (void *old_ptr, size_t size)
{
    void *new_mem;

    if (old_ptr == NULL) {
        new_mem = xmalloc(size);
    } else {
        new_mem = (void *)realloc(old_ptr, size ? size : 1);
        if (new_mem == NULL) {
            /* We used to print OLD_PTR here using %x, and casting its
               value to unsigned, but that lost on the Alpha, where
               pointers and unsigned had different sizes.  Since the info
               is of little or no value anyway, just don't print it.  */
            fprintf(stderr,
                    "fatal: memory exhausted (realloc of %lu bytes).\n",
                    (unsigned long)size);
            exit(EXIT_FAILURE);
        }
    }

    return new_mem;
}
