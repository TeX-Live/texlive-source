#include <kpathsea/kpathsea.h>

#include "mktexupd.h"

int main(int argc, char **argv)
{
  char dir[256];
  char file[256];
  char path[256];
  char *p;
  int i;

  kpse_set_program_name (argv[0], NULL);
  if(argc != 3) {
    fprintf(stderr, "%s:: usage: %s DIR FILE\n", argv[0], argv[0]);
    return 1;
  }
  strcpy(dir, argv[1]);
  strcpy(file, argv[2]);
  for(p = dir; *p; ++p) {
    if(*p == '\\') *p = '/';
    else if (IS_KANJI(p)) p++;
  }
  i = strlen(dir);
  while(dir[i-1] == '/')
    i--;
  dir[i] = '\0';
  strcpy(path, dir);
  strcat(path, "/");
  strcat(path, file);
  mktexupd(path);
  return 0;
}
