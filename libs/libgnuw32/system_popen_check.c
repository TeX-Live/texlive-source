#include "win32lib.h"

main(int argc, char *argv[])
{
  int ret;
  FILE *f;
  int c;

  if (argc != 3) {
    exit(1);
  }
  if (strncmp(argv[1], "syst", 4) == 0) {
    printf("Running cmd `%s'\n", argv[2]);
    ret = system(argv[2]);
    printf("return code = %d\n");
  }
  else if (strncmp(argv[1], "popr", 4) == 0) {
    f = popen(argv[2], "r");
    while ((c = fgetc(f)) != EOF) fputc(c, stdout);
    pclose(f);
  }
  else if (strncmp(argv[1], "popw", 4) == 0) {
    f = popen(argv[2], "w");
    while ((c = fgetc(stdin)) != EOF) fputc(c, f);
    pclose(f);
  }
}
