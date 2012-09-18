/* main.c -- the main program for everything but TeX & MF.  */
/* Public domain. */

#include <w2c/config.h>
#include "lib.h"

/* These variables are referenced from the change files.  */
char **argv;
int argc;

/* The entry point for all the programs except TeX and Metafont, which
   have more to do.  We just have to set up the command line.  web2c
   transforms Pascal's main block into a procedure `main_body'.  */
int
main (int  ac,  string* av)
{
#ifdef __EMX__
  _wildcard (&ac, &av);
  _response (&ac, &av);
#endif

#ifdef WIN32
  _setmaxstdio(2048);
#endif

  argc = ac;
  argv = av;
  mainbody ();
  return EXIT_SUCCESS;
}


/* Return the Nth (counted as in C) argument from the command line.  */

string 
cmdline (int n)
{
  if (n >= argc)
    { /* This error message should never happen, because the callers
         should always check they've got the arguments they expect.  */
      fprintf (stderr, "%s: Oops; not enough arguments.\n", argv[0]);
      uexit (1);
    }
  return argv[n];
}
