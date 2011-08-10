#include <kpathsea/kpathsea.h>

#include "mktexupd.h"

#define  MBUF     128
#define  SBUF     256
#define  LBUF     512
#define  DBS      "TEXMFDBS"
#define  MAXTREE  16

void
mktexupd (char *s)
{
  char fname[MBUF];
  char lsrname[SBUF];
  char path[LBUF];
  char *rootdir[MAXTREE];
  int i, j, treenum;
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

  for (i = 0; i < MAXTREE; i++)
    rootdir[i] = (char *) malloc (MBUF);

  pa = pb;
  i = 0;

  while (*pa && i < MAXTREE) {
    if (*pa == '!' && *(pa + 1) == '!') {
      pa++;
      pa++;
    }
    pc = rootdir[i];
    while (*pa != ';' && *pa)
      *pc++ = *pa++;
    *pc = '\0';
    if (*pa == ';') {
      pa++;
      i++;
    }
  }

  i++;
  treenum = i;
  free (pb);

  for (i = 0; i < treenum; i++) {
    j = strlen (rootdir[i]);
    if (rootdir[i][j - 1] == '/')
      rootdir[i][j - 1] = '\0';
  }

  strcpy (path, s);
  pa = strrchr (path, '/');
  if (pa == NULL) {
    fprintf (stderr, "Path name of the file may be incorrect.\n");
    for (i = 0; i < MAXTREE; i++)
      free (rootdir[i]);
    return;
  }

  *pa = '\0';
  pa++;
  strcpy (fname, pa);

  for (i = 0; i < treenum; i++) {
    j = strlen (rootdir[i]);
    if (j && strnicmp (path, rootdir[i], j) == 0) {
      existflag = 1;
      break;
    }
  }

  if (existflag) {
    strcpy (lsrname, rootdir[i]);
    strcat (lsrname, "/ls-R");
    if (_access (lsrname, 0) != 0) {
      for (j = 0; j < MAXTREE; j++)
        free (rootdir[j]);
      return;
    }
    pa = path;
    pb = rootdir[i];
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
  for (i = 0; i < MAXTREE; i++)
    free (rootdir[i]);
}
