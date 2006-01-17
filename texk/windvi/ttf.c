/*========================================================================*\

Copyright (c) 1999  Fabrice Popineau

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL FABRICE POPINEAU, PAUL VOJTA, OR ANYONE ELSE BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Names:
  fp: Fabrice Popineau (Fabrice.Popineau@supelec.fr)

History:
  24/06/99: initial version

Bugs:
  - Every kpse_find_file call presents a memory leak.  The pathname
    buffer is not freed.

\*========================================================================*/

#define HAVE_BOOLEAN 1

#include "xdvi-config.h"

#ifdef TTF

#define PRIVATE static
#define PUBLIC

#if defined(HAVE_STRING_H)
#include <string.h>
#else
extern char *strtok P2H(char *, const char *);
#endif

#include "dvi.h"
#include <kpathsea/tex-file.h>


/* **************************** GLYPH DRAWING *************************** */


/* Set character# ch */

#ifdef TEXXET
PUBLIC void set_ttf_char P2C(wide_ubyte, cmd, wide_ubyte, ch)
#else
PUBLIC long set_ttf_char P1C(wide_ubyte, ch)
#endif

{
  int xpos = PXL_H - currwin.base_x ;
  int ypos = PXL_V - currwin.base_y ;

}

#endif /* TTF */
