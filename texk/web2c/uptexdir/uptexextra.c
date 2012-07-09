/* uptexextra.c: Hand-coded routines for upTeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from uptexd.h here.  */

/* This file defines TeX and upTeX.  */
#include <uptexd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#define DLLPROC dlluptexmain
#include <lib/texmfmp.c>
