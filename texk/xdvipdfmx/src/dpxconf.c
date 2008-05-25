/*  $Header: /home/cvsroot/dvipdfmx/src/dpxconf.c,v 1.2 2008/05/22 10:08:02 matthias Exp $
    
    This is DVIPDFMx, an eXtended version of DVIPDFM by Mark A. Wicks.

    Copyright (C) 2002-2003 by Jin-Hwan Cho and Shunsaku Hirata,
    the DVIPDFMx project team <dvipdfmx@project.ktug.or.kr>
    
    Copyright (C) 1998, 1999 by Mark A. Wicks <mwicks@kettering.edu>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include "error.h"
#include "mem.h"

#include "dpxconf.h"

#ifndef  HAVE_LIBPAPER
const struct paper paperspecs[] = {
  {"letter",  612.00,  792.00},
  {"legal" ,  612.00, 1008.00},
  {"ledger", 1224.00,  792.00},
  {"tabloid", 792.00, 1224.00},
  {"a6",      297.64,  420.91},
  {"a5",      420.91,  595.27},
  {"a4",      595.27,  841.82},
  {"a3",      841.82, 1190.16},
  /*
   * The B series paper formats were first introduced in Japanese
   * patch (jpatch). The size of b6, ..., b5var paper is JIS paper
   * size for this reason. Please modify the following line or use
   * libpaper if you need ISO paper sizes.
   */
#if defined(USE_ISO_PAPERSIZE) || defined(ISO_PAPERSIZE)
  {"b6",         354,     499},
  {"b5",         501,     709},
  {"b4",         709,    1002},
  {"b3",        1002,    1418},
#else
  {"b6",      364.25,  515.91},
  {"b5",      515.91,  728.50},
  {"b4",      728.50, 1031.81},
  {"b3",     1031.81, 1457.00},
  {"b5var",   515.91,  651.97},
#endif /* USE_ISO_PAPERSIZE */
  /* Alias for JIS and ISO */
  {"jisb6",   364.25,  515.91},
  {"jisb5",   515.91,  728.50},
  {"jisb4",   728.50, 1031.81},
  {"jisb3",  1031.81, 1457.00},
  {"isob6",      354,     499},
  {"isob5",      501,     709},
  {"isob4",      709,    1002},
  {"isob3",     1002,    1418},
  /* -- */
  {NULL, 0, 0},
};

const struct paper *
paperinfo (const char *ppformat)
{
  const struct paper *ppinfo;

  if (!ppformat)
    return  NULL;

  ppinfo = paperfirst();
  while (ppinfo && papername(ppinfo)) {
    if (!strcmp(ppformat, ppinfo->name))
      /* ppinfo->name == papername(ppinfo), but gcc doesn't like the latter */
      break;
    ppinfo = papernext(ppinfo);
  }
  
  return ((ppinfo && papername(ppinfo)) ? ppinfo : NULL);
}

#endif /* HAVE_LIBPAPER */

void
dumppaperinfo (void)
{
  const struct paper *ppinfo;
  ppinfo = paperfirst();
  while (ppinfo && papername(ppinfo)) {
    double  wd, ht;
    wd = paperpswidth (ppinfo);
    ht = paperpsheight(ppinfo);
    fprintf(stdout, "%s: %.2f %.2f (%.2fmm %.2fmm)\n",
            papername(ppinfo), wd, ht, 25.4 * wd / 72.0, 25.4 * ht / 72.0); 
    ppinfo = papernext(ppinfo);
  }  
}

