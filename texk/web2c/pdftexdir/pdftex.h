/*
Copyright (c) 1996-2002 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: //depot/Build/source.development/TeX/texk/web2c/pdftexdir/pdftex.h#4 $
*/

/* Additions to texmfmp.h for pdfTeX */

/* mark a char in font */
#define pdfmarkchar(f, c) pdfcharused[f][c/8] |= (1<<(c%8))

/* test whether a char in font is marked */
#define pdfcharmarked(f, c) (boolean)(pdfcharused[f][c/8] & (1<<(c%8)))

/* writepdf() always writes by fwrite() */
#define       writepdf(a, b) \
  (void) fwrite ((char *) &pdfbuf[a], sizeof (pdfbuf[a]), \
                 (int) ((b) - (a) + 1), pdffile)

#define getlpcode(f, c) \
    (pdffontlpbase[f] == 0 ? 0 : pdfmem[pdffontlpbase[f] + c])

#define getrpcode(f, c) \
    (pdffontrpbase[f] == 0 ? 0 : pdfmem[pdffontrpbase[f] + c])

#define getefcode(f, c) \
    (pdffontefbase[f] == 0 ? 1000 : pdfmem[pdffontefbase[f] + c])

#define texbopenin(f) \
    open_input (&(f), kpse_tex_format, FOPEN_RBIN_MODE)
#define vfbopenin(f) \
    open_input (&(f), kpse_vf_format, FOPEN_RBIN_MODE)

#include <pdftexdir/ptexlib.h>
