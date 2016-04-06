/* mktexlsr.c

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

   Web2C 2010 (2010/08/22 --ak)
*/

#include <kpathsea/kpathsea.h>
#ifndef __MINGW32__
#include <kpathsea/dirent.h>
#endif
#include "mktex.h"

/* magic header */
#define HDL "%% ls-R -- filename database for kpathsea; do not change this line.\n"

#define VARTEXFONTS "c:/var/tex/fonts/ls-R"

FILE *ls_R;

static void
search(char *name)
{
  DIR *dp;
  struct dirent *de;
  char   buff[256];
  int    len;

  normalize (name);

  if((dp = opendir(name))) {
    fprintf(ls_R, "\n%s:\n", name);
    while((de = readdir(dp))) {
      if(stricmp(de->d_name, ".") &&
         stricmp(de->d_name, "..") &&
         stricmp(de->d_name, ".bzr") &&
         stricmp(de->d_name, ".git") &&
         stricmp(de->d_name, ".hg") &&
         stricmp(de->d_name, ".svn") &&
         stricmp(de->d_name, "_bzr") &&
         stricmp(de->d_name, "_git") &&
         stricmp(de->d_name, "_hg") &&
         stricmp(de->d_name, "_svn") &&
         stricmp(de->d_name, "_darcs")) {
        fprintf(ls_R, "%s\n", de->d_name);
      }
    }
    closedir(dp);

    len = (int)strlen(name);
    strcpy(buff, name);
    if(name[len-1] != '/') {
      strcat(buff, "/");
      len++;
    }

    dp = opendir(name);
    while((de = readdir(dp))) {
      if(stricmp(de->d_name, ".") &&
         stricmp(de->d_name, "..") &&
         stricmp(de->d_name, ".bzr") &&
         stricmp(de->d_name, ".git") &&
         stricmp(de->d_name, ".hg") &&
         stricmp(de->d_name, ".svn") &&
         stricmp(de->d_name, "_bzr") &&
         stricmp(de->d_name, "_git") &&
         stricmp(de->d_name, "_hg") &&
         stricmp(de->d_name, "_svn") &&
         stricmp(de->d_name, "_darcs")) {
#ifdef __MINGW32__
        strcpy(buff + len, de->d_name);
        if(!is_dir(buff))
          continue;
#else
        if(!de->d_isdir)
          continue;
        strcpy(buff + len, de->d_name);
#endif          
        search(buff);
      }
    }
    closedir(dp);
  }
}

int Quiet = 0;

char first_name[] = "./";

int main(int ac, char **av)
{
  int cdrive, tdrive;
  char ls_R_name[512];
  int i, numtree;
  size_t len;
  char *progname;
  char **pathbuff;

  kpse_set_program_name(av[0], NULL);
  progname = kpse_program_name;

  if(ac > 1 && (!strncmp(av[1], "-v", 2) || !strncmp(av[1], "--v", 3))) {
    puts (kpathsea_version_string);
    puts ("mktexlsr: (C version 1.1 --ak 2002-2015)");
    exit (0);
  }

  if(ac > 1 && (!strncmp(av[1], "-h", 2) || !strncmp(av[1], "--h", 3))) {
    printf("Usage: %s [--quiet|--silent] [DIRS ...]\n\n"
"Rebuild all necessary ls-R filename databases completely. If one or\n"
"more arguments DIRS are given, these are used as texmf directories to\n"
"build ls-R for. Else all directories in the search path for ls-R files\n"
"($TEXMFDBS) are used.", av[0]);
    exit (0);
  }

  if(ac > 1) {
    if(!strncmp(av[1], "-q", 2) || !strncmp(av[1], "--q", 3) ||
       !strncmp(av[1], "-s", 2) || !strncmp(av[1], "--s", 3)) {
      Quiet = 1;
      ac--;
      av++;
    }
  }

  if(ac > 1) {
    numtree = ac - 1;
    pathbuff = xmalloc(numtree * sizeof(char *));
    for(i=0; i < numtree; i++) {
      pathbuff[i] = xstrdup(av[i+1]);
      normalize (pathbuff[i]);
    }
  } else {
    if (!(pathbuff = mkpaths (&numtree))) {
      fprintf (stderr, "Maybe you are not using ls-R.\n");
      exit (100);
    }
  }

  for(i = 0; i < numtree; i++) {
    strcpy(ls_R_name, pathbuff[i]);
    len = strlen(ls_R_name);
    if(ls_R_name[len-1] != '/') strcat(ls_R_name, "/");
    strcat(ls_R_name, "ls-R");

    ls_R = fopen(ls_R_name, "wb");
    if(!ls_R) {
      fprintf(stdout, "Cannot open %s to write.\n", ls_R_name);
      if(!stricmp(ls_R_name, VARTEXFONTS))
        fprintf(stdout, "       (Don't mind this message.)\n");
      continue;
    }

    cdrive = _getdrive();

    if(ls_R_name[1] == ':') {
      tdrive = tolower(ls_R_name[0]) - 'a' + 1;
      _chdrive(tdrive);
    }

    _chdir(pathbuff[i]);
    if(!Quiet)
      fprintf(stdout, "%s: Updating %s...\n", progname, ls_R_name);
    fprintf(ls_R, HDL);
    search(first_name);

    fclose(ls_R);
    if(!Quiet)
      fprintf(stdout, "%s: Updated %s.\n", progname, ls_R_name);
    _chdrive(cdrive);
  }

  if(!Quiet)
    fprintf(stdout, "%s: Done.\n", progname);
  
  for(i = 0; i < numtree; i++) {
    free(pathbuff[i]);
  }

  free (pathbuff);

  return 0;
}
