/* mkpaths.c

   Copyright 2015 Akira Kakuto.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#include <kpathsea/kpathsea.h>
#include "mktex.h"

char **
mkpaths (int *numptr)
{
  char **pathbuff;
  char *texmfdbs;
  char *p, *pa, *pc;
  int i;
  
  if (!(p = kpse_var_value ("TEXMFDBS"))) {
    fprintf (stderr, "No definition of TEXMFDBS.\n");
    return NULL;
  }

  texmfdbs = kpse_brace_expand (p);
  free (p);
  if (!texmfdbs) {
    fprintf (stderr, "I cannot expand braces in TEXMFDBS.\n");
    return NULL;
  }

  p = texmfdbs;
  i = 0;

  while(*p) {
    if(*p == '!' && *(p+1) == '!')
      p += 2;
    if(*p == ';')
      while(*p == ';')
        p++;
    if(*p && *p != '!') {
      while(*p != ';' && *p)
        p++;
      i++;
      while(*p == ';')
        p++;
    }
  }

  if (!i) {
    free (texmfdbs);
    fprintf (stderr, "No database paths in TEXMFDBS.\n");
    return NULL;
  }

  *numptr = i;
  pathbuff = xmalloc(i * sizeof(char *));

  p = texmfdbs;
  i = 0;

  while (*p) {
    if (*p == '!' && *(p + 1) == '!')
      p += 2;
    if (*p == ';') {
      while (*p == ';')
        p++;
    }
    if(*p && *p != '!') {
      pa = p;
      while(*p != ';' && *p)
        p++;
      pc = p;
      pathbuff[i] = xmalloc(pc - pa + 1);
      strncpy(pathbuff[i], pa, pc - pa);
      pathbuff[i][pc - pa] = 0;
      i++;
      while(*p == ';')
        p++;
    }
  }

  free(texmfdbs);
  return pathbuff;
}
