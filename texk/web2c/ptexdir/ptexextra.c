/* ptexextra.c: Hand-coded routines for pTeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from ptexd.h here.  */

/* This file defines TeX and pTeX.  */
#include <ptexd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#define DLLPROC dllptexmain
#include <lib/texmfmp.c>
