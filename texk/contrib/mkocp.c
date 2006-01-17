#include <kpathsea/kpathsea.h>
#include "stackenv.h"

char *progname = NULL;

void 
usage()
{
  fprintf(stderr, "usage: %s ocp_filename\n", progname);
  exit(1);
}

int
main (int argc, char *argv[]) 
{
  char cmd[4096];
  int newfd[3], ret;
  FILE *f = fopen("nul", "r");
  char *basename;

  progname = xbasename(argv[0]);

  if (argc != 2) {
    usage();
  }
  
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)mt_exit, TRUE);

  basename = xbasename(argv[1]);
  if (strlen(basename) > strlen(".ocp")
      && _stricmp(basename + strlen(basename) - strlen(".ocp"), ".ocp") == 0) {
    basename[strlen(basename) - strlen(".ocp")] = '\0';
  }
  newfd[0] = fileno(f);
  newfd[1] = fileno(stderr);
  newfd[2] = fileno(stderr);
  push_fd(newfd);
  sprintf(cmd, "otp2ocp %s", basename);
  ret = system(cmd);
  pop_fd();
  fclose(f);
  if (ret == 0) {
    printf("%s.ocp\n", basename);
  }
  return ret;
}
