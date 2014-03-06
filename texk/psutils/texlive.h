/* texlive.h: TeX Live specific declarations - included from config.h

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

#include <kpathsea/kpathsea.h>

#undef _Noreturn  /* Cygwin defines this */
#define _Noreturn
#define _GL_ATTRIBUTE_CONST

#ifdef WIN32
# define set_binary_mode(file, mode) _setmode(file, mode)
# ifndef __MINGW32__
#  define ftello _ftelli64
#  define fseeko _fseeki64
# endif
#else
# define set_binary_mode(file, mode) 0
#endif

