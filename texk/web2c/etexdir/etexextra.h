/* etexextra.h: banner etc. for e-TeX.

   This is included by e-TeX, from etexextra.c.

Copyright (C) 1995, 1996, 2009, 2014, 2016 Karl Berry.
Copyright (C) 2004 Olaf Weber.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <etexdir/etex_version.h> /* for ETEX_VERSION */

#define BANNER "This is e-TeX, Version 3.141592653-" ETEX_VERSION
#define COPYRIGHT_HOLDER "Peter Breitenlohner et al"
#define AUTHOR NULL
#define PROGRAM_HELP ETEXHELP
#define BUG_ADDRESS "tex-k@tug.org"
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "fmt"
#define DUMP_EXT ".fmt"
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "einitex"
#define VIR_PROGRAM "evirtex"
