#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: disasm.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: disasm.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

void ttfPrintInstructions(FILE *fp, BYTE * ins)
{
   /* not implemented yet */ 
}

/* Pushing data onto the interpreter stack */
