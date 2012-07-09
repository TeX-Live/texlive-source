/* eptexextra.c: Hand-coded routines for e-pTeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from eptexd.h here.  */

/* This file defines TeX and epTeX.  */
#include <eptexd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#define DLLPROC dlleptexmain
#include <lib/texmfmp.c>
