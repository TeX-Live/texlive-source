/* mktexfmt.c
 */

#include <kpathsea/kpathsea.h>

int main(int ac, char **av)
{
  int  savo, savi;
  FILE *fnul;
  char *fmtname;
  char *p;
  char orgname[256];

  char texbindir[256];
  char fullbin[256];

  kpse_set_program_name(av[0], NULL);

  if(ac != 2) {
    fprintf(stderr,"%s : Usage %s formatname\n", av[0], av[0]);
    fprintf(stderr,"formatname :  (foo.fmt, foo.nfmt, foo.mem, foo.base)\n");
    return 1;
  }

  if(strlen(av[1]) > 127) {
    fprintf(stderr, "\nToo long a format name.\n");
    return 100;
  }

  p = kpse_var_value("SELFAUTOLOC");
  if(p == 0) {
     fprintf(stderr, "I cannot get SELFAUTOLOC\n");
     exit(1);
  }
  strcpy(texbindir, p);
  free(p);
  for(p=texbindir; *p; p++) {
     if(*p == '/') *p = '\\';
  }
  strcat(texbindir, "\\");

  strcpy(orgname, av[1]);

  p = strrchr(av[1], '.');
  if(!p) {
    fprintf(stderr, "formatname needs a suffix.\n");
    return 1;
  }

  if(stricmp(p, ".fmt") && stricmp(p, ".nfmt") &&
     stricmp(p, ".base") && stricmp(p, ".mem")) {
    fprintf(stderr, "%s : unknown format type.\n", av[1]);
    return 1;
  }

  *p = '\0';

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
  strcpy(fullbin, texbindir);
  strcat(fullbin, "fmtutil.exe");
  fprintf(stderr, "Running the command %s\n", fullbin);
  _spawnlp(_P_WAIT, fullbin, "fmtutil", "--byfmt", av[1], NULL);

/* END COMMAND */
  
/* return to original stdout and stdin */
  _dup2(savo, fileno(stdout));
  close(savo);
  _dup2(savi, fileno(stdin));
  close(savi);

/* close nul device */
  fclose(fnul);

  fmtname = kpse_find_file(orgname , kpse_fmt_format, 0);
  if(fmtname) {
    printf("%s\n", fmtname);
    free(fmtname);
  }
  return 0;
}
