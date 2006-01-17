/* xfseek.c: fseek with error checking.

    Copyright 2005     Olaf Weber
    Copyright 1992, 95 Karl Berry

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


void
xfseek P4C(FILE *, f,  long, offset,  int, wherefrom,  string, filename)
{
    if (fseek(f, offset, wherefrom) < 0)
        FATAL_PERROR(filename);
}
