/* mktexupd.c

   Copyright 2000, 2015 Akira Kakuto.

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

#include "mktexupd.h"

#define  MBUF     512
#define  SBUF     512
#define  LBUF     512
#define  DBS      "TEXMFDBS"

void
mktexupd (char *s)
{
  char fname[MBUF];
  char lsrname[SBUF];
  char path[LBUF];
  char **pathbuff = NULL;
  int i, j, numtree;
  char *pa, *pb, *pc;
  int existflag = 0;
  FILE *f;

  pa = kpse_var_value (DBS);
  if (pa == NULL) {
    fprintf (stderr, "No definition of TEXMFDBS.\n");
    fprintf (stderr, "Maybe you are not using ls-R.\n");
    return;
  }

  pb = kpse_brace_expand (pa);
  free (pa);
  if (pb == NULL) {
    fprintf (stderr, "I cannot expand braces in TEXMFDBS.\n");
    fprintf (stderr, "Maybe you are not using ls-R.\n");
    return;
  }

  pa = pb;
  i = 0;

  while(*pa) {
    if(*pa == '!' && *(pa+1) == '!')
      pa += 2;
    if(*pa == ';')
      while(*pa == ';')
        pa++;
    if(*pa && *pa != '!') {
      while(*pa != ';' && *pa)
        pa++;
      if(*pa == ';') {
        i++;
        while(*pa == ';')
          pa++;
      } else if(*pa == '\0') {
        i++;
        break;
      }
    }
  }

  numtree = i;

  pathbuff = xmalloc(numtree * sizeof(char *));

  pa = pb;
  i = 0;

  while (*pa) {
    if (*pa == '!' && *(pa + 1) == '!')
      pa += 2;
    if (*pa == ';') {
      while (*pa == ';')
        pa++;
    }
    if(*pa && *pa != '!') {
      pathbuff[i] = malloc(strlen(pa) + 1);
      pc = pathbuff[i];
      while (*pa != ';' && *pa)
        *pc++ = *pa++;
      *pc = '\0';
      if (*pa == ';') {
        while (*pa == ';')
          pa++;
        i++;
      } else if (*pa == '\0') {
        i++;
        break;
      }
    }
  }

  free (pb);

  for (i = 0; i < numtree; i++) {
    j = (int)strlen (pathbuff[i]);
    if (pathbuff[i][j - 1] == '/')
      pathbuff[i][j - 1] = '\0';
  }

  strcpy (path, s);
  pa = strrchr (path, '/');
  if (pa == NULL) {
    fprintf (stderr, "Path name of the file may be incorrect.\n");
    for (i = 0; i < numtree; i++)
      free (pathbuff[i]);
    free (pathbuff);
    return;
  }

  *pa = '\0';
  pa++;
  strcpy (fname, pa);

  for (i = 0; i < numtree; i++) {
    j = (int)strlen (pathbuff[i]);
    if (j && strnicmp (path, pathbuff[i], j) == 0) {
      existflag = 1;
      break;
    }
  }

  if (existflag) {
    strcpy (lsrname, pathbuff[i]);
    strcat (lsrname, "/ls-R");
    if (_access (lsrname, 0) != 0) {
      for (j = 0; j < numtree; j++)
        free (pathbuff[j]);
      free (pathbuff);
      return;
    }
    pa = path;
    pb = pathbuff[i];
    while (tolower (*pa) == tolower (*pb) && *pb) {
      pa++;
      pb++;
    }
    f = fopen (lsrname, "ab");
    fprintf (f, "\n.%s:\n%s\n", pa, fname);
    fclose (f);
  } else {
    fprintf(stderr, "mktexupd failed\n");
  }
  for (i = 0; i < numtree; i++)
    free (pathbuff[i]);
  free (pathbuff);
}
