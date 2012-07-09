/* etexextra.c: Hand-coded routines for e-TeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from etexd.h here.  */

/* This file defines TeX and eTeX.  */
#include <etexd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#define DLLPROC dlletexmain
#include <lib/texmfmp.c>
