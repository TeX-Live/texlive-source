/*
mkocp.c
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <process.h>

/* kpathsea functions
 */
#include <kpathsea/kpathsea.h>

int main(int ac, char **av)
{
  int  savo, savi;
  FILE *fnul;
  char name[256];
  char fullname[256];
  char *p;

  kpse_set_program_name(av[0], "mkocp");

  if(ac != 2) {
    fprintf(stderr,"%s : Usage %s ocpname\n", av[0], av[0]);
    return 1;
  }

  p = strrchr(av[1], '.');
  if(p)
    *p = '\0';
  strcpy(name, av[1]);
  strcpy(fullname, name);
  strcat(fullname, ".ocp");

/* save stdout and stdin */
  savo = _dup(fileno(stdout));
  savi = _dup(fileno(stdin));

/* connect stdout to stderr */
  _dup2(fileno(stderr), fileno(stdout));

/* connect stdin to nul device */
  if(!(fnul = fopen("nul", "r"))) {
    fprintf(stderr, "Cannot open nul device to read\n");
    exit(100);
  }
  _dup2(fileno(fnul), fileno(stdin));

/* COMMAND */

  spawnlp(P_WAIT, "otp2ocp", "otp2ocp", name, NULL);

/* END COMMAND */
  
/* return to original stdout and stdin */
  _dup2(savo, fileno(stdout));
  close(savo);
  _dup2(savi, fileno(stdin));
  close(savi);

/* close nul device */
  fclose(fnul);

  if(_access(fullname, 0) == 0)
    printf("%s\n", fullname);

  return 0;
}
