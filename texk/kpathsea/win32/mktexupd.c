/* mktexupd.c

   Copyright 2000, 2016 Akira Kakuto.

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

#define  MBUF     512
#define  SBUF     512
#define  LBUF     512

void
mktexupd (char *s)
{
  char fname[MBUF];
  char lsrname[SBUF];
  char path[LBUF];
  char **pathbuff;
  int i, j, numtree;
  char *pa, *pb;
  int existflag = 0;
  FILE *f;

  if (!(pathbuff = mkpaths (&numtree))) {
    fprintf (stderr, "Maybe you are not using ls-R.\n");
    return;
  }

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
  } else
    fprintf(stderr, "mktexupd failed\n");
  for (i = 0; i < numtree; i++)
    free (pathbuff[i]);
  free (pathbuff);
}
