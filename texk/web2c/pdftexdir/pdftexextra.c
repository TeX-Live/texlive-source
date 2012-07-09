/* pdftexextra.c: Hand-coded routines for pdfTeX.

   This file is public domain.  */

#define	EXTERN /* Instantiate data from pdftexd.h here.  */

/* This file defines TeX and pdfTeX.  */
#include <pdftexd.h>

/* Hand-coded routines for TeX or Metafont in C.  */
#define DLLPROC dllpdftexmain
#include <lib/texmfmp.c>
