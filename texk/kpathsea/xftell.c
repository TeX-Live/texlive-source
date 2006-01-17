/* xftell.c: ftell with error checking.

    Copyright 2005         Olaf Weber
    Copyright 1992, 93, 95 Karl Berry

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


unsigned long
xftell P2C(FILE *, f,  string, filename)
{
    long where = ftell (f);
    
    if (where < 0)
        FATAL_PERROR(filename);
    
    return where;
}
