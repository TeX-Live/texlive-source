/*
Copyright (c) 1996-2006 Han The Thanh, <thanh@pdftex.org>

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

$Id: writefont.c,v 1.3 2005/12/27 19:04:42 hahe Exp $
*/

#include "mplib.h"

static const char perforce_id[] =
    "$Id: writefont.c,v 1.3 2005/12/27 19:04:42 hahe Exp $";

boolean fontfile_found;
boolean is_otf_font;
char fontname_buf[FONTNAME_BUF_SIZE];

boolean fontisreencoded (int f) {
  fm_entry *fm;
  if (fontsizes[f]!=0 && hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL 
	&& (fm->ps_name != NULL)
	&& is_reencoded (fm))
      return 1;
  }
  return 0;
}

boolean fontisincluded (int f) {
  fm_entry *fm;
  if (fontsizes[f]!=0 && hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL 
	&& (fm->ps_name != NULL && fm->ff_name != NULL) 
	&& is_included (fm))
      return 1;
  }
  return 0;
}

boolean fontissubsetted (int f) {
  fm_entry *fm;
  if (fontsizes[f]!=0 && hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL 
	&& (fm->ps_name != NULL && fm->ff_name != NULL) 
	&& is_included (fm) && is_subsetted (fm))
      return 1;
  }
  return 0;
}


strnumber fmencodingname (int f) {
  enc_entry *e;
  fm_entry *fm;
  if (hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL && (fm->ps_name != NULL)) {
      if (is_reencoded (fm)) {
	e = fm->encoding;
	if (e->encname!=NULL) {
	  return maketexstring(e->encname);
	} 
      } else {
	return 0;
      }
    }
  }
  pdftex_fail ("fontmap encoding problems for font %s",makecstring (fontname[f]));
  return 0;
}

strnumber fmfontname (int f) {
  fm_entry *fm;
  if (hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL && (fm->ps_name != NULL)) {
	  if (fontisincluded(f) && !fontpsnamefixed[f]) {
		/* find the real fontname, and update ps_name and subset_tag if needed */
	    if(t1_updatefm(f,fm)) {
	      fontpsnamefixed[f] = true;
	    } else {
	      pdftex_fail ("font loading problems for font %s",makecstring (fontname[f]));
	    }
	  }
	  return maketexstring(fm->ps_name);
    }
  }
  pdftex_fail ("fontmap name problems for font %s",makecstring (fontname[f]));
  return 0;
}

strnumber fmfontsubsetname (int f) {
  fm_entry *fm;
  char *s;
  if (hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL && (fm->ps_name != NULL)) {
      if (is_subsetted(fm)) {
		s = xmalloc(strlen(fm->ps_name)+8);
		snprintf(s,strlen(fm->ps_name)+8,"%s-%s",fm->subset_tag,fm->ps_name);
		return maketexstring(s);
      } else {
		return maketexstring(fm->ps_name);
	  }
    }
  }
  pdftex_fail ("fontmap name problems for font %s",makecstring (fontname[f]));
  return 0;
}



integer fmfontslant (int f) {
  fm_entry *fm;
  if (hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL && (fm->ps_name != NULL)) {
      return fm->slant;
    }
  }
  return 0;
}

integer fmfontextend (int f) {
  fm_entry *fm;
  if (hasfmentry (f)) { 
    fm = (fm_entry *) mpfontmap[f];
    if (fm != NULL && (fm->ps_name != NULL)) {
      return fm->extend;
    }
  }
  return 0;
}



void mploadencodings (int lastfnum) {
  int nullfont;
  int f;
  enc_entry *e;
  fm_entry *fm_cur;
  nullfont = getnullfont();
  for (f=nullfont+1;f<=lastfnum;f++) {
    if (fontsizes[f]!=0 && hasfmentry (f)) { 
      fm_cur = (fm_entry *) mpfontmap[f];
      if (fm_cur != NULL && 
	  fm_cur->ps_name != NULL &&
	  is_reencoded (fm_cur)) {
		e = fm_cur->encoding;
		read_enc (e);
      }
    }
  }
}

void mpfontencodings (int lastfnum, int encodings_only) {
  int nullfont;
  int f,ff;
  enc_entry *e;
  fm_entry *fm;
  nullfont = getnullfont();
  for (f=nullfont+1;f<=lastfnum;f++) {
    if (fontsizes[f]!=0 && hasfmentry (f)) { 
      fm = (fm_entry *) mpfontmap[f];
      if (fm != NULL && (fm->ps_name != NULL)) {
	if (is_reencoded (fm)) {
	  if (encodings_only || (!is_subsetted (fm))) {
	    e = fm->encoding;
	    write_enc (NULL, e, 0);
	  }
	}
      }
    }
  }
  for (f=nullfont+1;f<=lastfnum;f++) {
    if (fontsizes[f]!=0 && hasfmentry (f)) { 
      fm = (fm_entry *) mpfontmap[f];
      if (fm != NULL && (fm->ps_name != NULL)) {
	if (is_reencoded (fm)) {
	  if (encodings_only || (!is_subsetted (fm))) {
            /* clear for next run */
            e->objnum = 0;
	  }
	}
      }
    }
  }
}

boolean dopsfont (fontnumber f)
{
    int i;
  fm_entry *fm_cur;
    if (mpfontmap[f] == NULL)
        pdftex_fail ("pdffontmap not initialized for font %s",
                     makecstring (fontname[f]));
    if (hasfmentry (f))
        fm_cur = (fm_entry *) mpfontmap[f];
    else
        fm_cur = NULL;
    if (fm_cur == NULL)
      return 1;
    if (is_truetype(fm_cur) ||
	(fm_cur->ps_name == NULL && fm_cur->ff_name == NULL)) {
        return 0;
    }
    
    if (is_included(fm_cur)) {
      printnl(maketexstring("%%BeginResource: font "));
      flushstr (last_tex_string);
      if (is_subsetted(fm_cur)) {
	    print(maketexstring(fm_cur->subset_tag));
        flushstr (last_tex_string);
     	print(maketexstring("-"));
        flushstr (last_tex_string);
      }
      print(maketexstring(fm_cur->ps_name));
      flushstr (last_tex_string);
      println();
      writet1 (f,fm_cur);
      printnl(maketexstring("%%EndResource"));
      flushstr (last_tex_string);
      println();
    }
    return 1;
}

