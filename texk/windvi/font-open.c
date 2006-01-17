/* 
   font_open.c: find font filenames.  This bears no relation (but the
   interface) to the original font_open.c.

Copyright (c) 1999  The texk project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL PAUL VOJTA OR ANYONE ELSE BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   

 */

#define WIN32_LEAN_AND_MEAN 1
#include "xdvi-config.h"

#include <kpathsea/c-fopen.h>
#include <kpathsea/tex-glyph.h>


/* We try for a VF first because that's what dvips does.  Also, it's
   easier to avoid running mktexpk if we have a VF this way.  */

FILE *
#if T1
font_open P7C(char *,font, char **,font_ret, double, dpi, int *,dpi_ret, 
			  int, dummy, char **,filename_ret, int *,t1id)
#else
font_open P6C(char *,font, char **,font_ret, double, dpi, int *,dpi_ret, 
			  int, dummy, char **,filename_ret, int *,t1id)

#endif
{
  FILE *ret;
  char *name;
#ifdef WIN32
  extern void UpdateStatusBar(LPSTR lpszStatusString, 
			      WORD partNumber, WORD displayFlags);
  char szBuf[80];

  wsprintf(szBuf, "Finding font %s", font);
  UpdateStatusBar(szBuf, 0, 0);
#endif
  
#ifdef Omega
  name = kpse_find_ovf (font);
  if (!name) name = kpse_find_vf(font);
#else
  name = kpse_find_vf (font);
#endif
#if TTF
  *ttfid=-1
#endif
#if T1
  *t1id=-1;
#endif
  if (name) {
    /* VF fonts don't have a resolution, but load_font will complain if
       we don't return what it asked for.  */
    *dpi_ret = dpi;
    *font_ret = NULL;
  } else {
    kpse_glyph_file_type file_ret;

#if TTF
    /* First atempt to load it as TrueType font */

    *dpi_ret = dpi;
    *font_ret = NULL;
    *filename_ret = NULL;
    *ttfid=find_TTF_font(font);
    if (*ttfid>=0) return NULL;
#endif
#if T1
    /* First atempt to load it as T1 font */

    *dpi_ret = dpi;
    *font_ret = NULL;
    *filename_ret = NULL;
    *t1id=find_T1_font(font);
    if (*t1id>=0) return NULL;
#endif

    name = kpse_find_glyph (font, (unsigned) (dpi + .5),
			    kpse_any_glyph_format, &file_ret);

    if (name) {
      /* If we got it normally, from an alias, or from mktexpk,
	 don't fill in FONT_RET.  That tells load_font to complain.  */
      *font_ret
	= file_ret.source == kpse_glyph_source_fallback ? file_ret.name
	: NULL; /* tell load_font we found something good */
          
      *dpi_ret = file_ret.dpi;
    }
    /* If no VF and no PK, FONT_RET is irrelevant? */
  }
  
  /* If we found a name, return the stream. */
  ret = name ? xfopen_local (name, FOPEN_RBIN_MODE) : NULL;
  *filename_ret = name;

  return ret;
}
