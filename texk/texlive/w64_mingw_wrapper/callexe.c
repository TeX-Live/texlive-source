/* Public domain.
 * Originally written by Akira Kakuto  <kakuto@fuk.kindai.ac.jp>
 *
 * WIN32 wrapper program replacing Unix symlinks such as,
 * e.g., ofm2opl -> omfonts.
 *
 * EXEPROG must be defined in the Makefile as,
 * e.g., -DEXEPROG=\"omfonts.exe\".
 */
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <malloc.h>

static int is_include_space(char *s)
{
  char *p;
  p = strchr(s, ' ');
  if(p) return 1;
  p = strchr(s, '\t');
  if(p) return 1;
  return 0;
}

int main(int argc, char *argv[])
{
  int i;
  char *p;

  for(i = 0; i < argc; i++) {
    if(is_include_space(argv[i])) {
      p = (char *)malloc(strlen(argv[i])+3);
      strcpy(p, "\"");
      strcat(p, argv[i]);
      strcat(p, "\"");
      free(argv[i]);
      argv[i] = p;
    }
  }
  argv[argc] = NULL;
  return _spawnvp(_P_WAIT, EXEPROG, (const char * const *)argv);
}
