/* euptexextra.c: Hand-coded routines for e-upTeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from euptexd.h here.  */

/* This file defines TeX and eupTeX.  */
#include <euptexd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#define DLLPROC dlleuptexmain
#include <lib/texmfmp.c>
