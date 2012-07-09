/* texextra.c: Hand-coded routines for TeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from texd.h here.  */

/* This file defines TeX and onlyTeX.  */
#include <texd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#define DLLPROC dlltexmain
#include <lib/texmfmp.c>
