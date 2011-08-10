/*
mktexlsr
Web2C 2010 (2010/08/22 --ak)
*/

#include <kpathsea/kpathsea.h>

#ifdef __MINGW32__
#include "dirutil.h"
#else
#include <kpathsea/dirent.h>
#endif

#define  MAXTREE  32 /* Max number of texmf trees */

/* magic header */
#define HDL "%% ls-R -- filename database for kpathsea; do not change this line.\n"

#define VARTEXFONTS "c:/var/tex/fonts/ls-R"

FILE *ls_R;

static void
search(const char *name)
{
  DIR *dp;
  struct dirent *de;
  char   buff[256];
  int    len;

  if ((dp = opendir(name))) {
    fprintf(ls_R, "\n%s:\n", name);
    while ((de = readdir(dp))) {
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

    len = strlen (name);
    strcpy (buff, name);
    if (name[len-1] != '/')
      buff[len++] = '/';

    dp = opendir(name);
    while ((de = readdir(dp))) {
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
        if (is_dir (buff)) {
          search (buff);
        }
#else
        if (de->d_isdir) {
          strcpy (buff + len, de->d_name);
          search (buff);
        }
#endif
      }
    }
    closedir(dp);
  }
}

int Quiet = 0;

int main(int ac, char **av)
{
  char *texmfdbs ;
  int cdrive, tdrive;
  char ls_R_name[256];
  int len, i, numtree;
  char *progname;
  char *pathbuff[MAXTREE];
  char *p, *q;

  progname = av[0];
  kpse_set_program_name(progname, NULL);

  for(i = 0; i < MAXTREE; i++) {
    pathbuff[i] = (char *)malloc(384);
    if(!pathbuff[i]) {
      fprintf(stderr, "Memory allocation error.\n");
      exit(100);
    }
  }

  if(ac > 1 && (!strncmp(av[1], "-v", 2) || !strncmp(av[1], "--v", 3))) {
    puts (kpathsea_version_string);
    puts ("Copyright 2010 Karl Berry & Olaf Weber.\n\
There is NO warranty.  You may redistribute this software\n\
under the terms of the GNU General Public License.\n\
For more information about these matters, see the files named GPL and LGPL.");
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
    if(ac > MAXTREE + 1) {
      fprintf(stderr, "Number of trees must be less than %d.\n"
	      "Some trees are discarded.\n", MAXTREE);
      numtree = MAXTREE;
    } else {
      numtree = ac - 1;
    }
    for(i=0; i < numtree; i++) {
      strcpy(pathbuff[i], av[i+1]);
      for(p=pathbuff[i]; *p; p++) {
	if(*p == '\\')
	  *p = '/';
        else if (IS_KANJI(p))
	  p++;
      }
    }
  } else {
    if(!(p = kpse_var_value("TEXMFDBS"))) {
      fprintf(stderr, "TEXMFDBS is not defined in texmf.cnf.\n");
      exit (100);
    }

    texmfdbs = kpse_brace_expand(p);
    free(p);

    for(i=0 ; (i < MAXTREE) && *texmfdbs ; i++) {
      p = strchr(texmfdbs, ';');
      if(!p) {
	strcpy(pathbuff[i], texmfdbs);
	q = pathbuff[i];
	if(*q == '!' && *(q+1) == '!')
	  q += 2;
	strcpy(pathbuff[i], q);
	i++;
	break;
      }
      *p = '\0';
      strcpy(pathbuff[i], texmfdbs);
      q = pathbuff[i];
      if(*q == '!' && *(q+1) == '!')
	q += 2;
      strcpy(pathbuff[i], q);
      p++;
      texmfdbs = p;
    }

    numtree = i;
  }
  
  for(i = 0; i < numtree; i++) {
    strcpy(ls_R_name, pathbuff[i]);
    len = strlen(ls_R_name);
    if(ls_R_name[len-1] != '/') strcat(ls_R_name, "/");
    strcat(ls_R_name, "ls-R");

    ls_R = fopen(ls_R_name, "wb");
    if(!ls_R) {
      fprintf(stderr, "Cannot open %s to write.\n", ls_R_name);
      if(!stricmp(ls_R_name, VARTEXFONTS))
        fprintf(stderr, "       (Don't mind this message.)\n");
      continue;
    }

    cdrive = _getdrive();

    if(ls_R_name[1] == ':') {
      tdrive = tolower(ls_R_name[0]) - 'a' + 1;
      _chdrive(tdrive);
    }

    _chdir(pathbuff[i]);
    if(!Quiet)
      fprintf(stderr, "%s: Updating %s...\n", progname, ls_R_name);
    fprintf(ls_R, HDL);
    search("./");

    fclose(ls_R);
    if(!Quiet)
      fprintf(stderr, "%s: Updated %s.\n", progname, ls_R_name);
    _chdrive(cdrive);
  }

  if(!Quiet)
    fprintf(stderr, "%s: Done.\n", progname);
  
  for(i = 0; i < MAXTREE; i++) {
    free(pathbuff[i]);
  }

  return 0;
}
