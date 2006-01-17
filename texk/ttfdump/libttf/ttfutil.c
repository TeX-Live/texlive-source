#include <stdio.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: ttfutil.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: ttfutil.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

/* FixedSplit: split Fixed in to two interger (16.16) */
void FixedSplit(Fixed f,int b[])
{
    b[0] = f & 0xff00;
    b[1] = f >> 16;
}

/*
 *      Invert byte order within each 16-bits of an array.
 */
void
TwoByteSwap(unsigned char *buf, int nbytes)
{
    register unsigned char c;
    
    for (; nbytes > 0; nbytes -= 2, buf += 2)
	{
	    c = buf[0];
	    buf[0] = buf[1];
	    buf[1] = c;
	}
}

/*
 *      Invert byte order within each 32-bits of an array.
 */

void
FourByteSwap(unsigned char *buf, int nbytes)
{
    register unsigned char c;

    for (; nbytes > 0; nbytes -= 4, buf += 4)
	{
	    c = buf[0];
	    buf[0] = buf[3];
	    buf[3] = c;
	    c = buf[1];
	    buf[1] = buf[2];
	    buf[2] = c;
	}
}

void ttfError(char * msg)
{
    fprintf(stderr,"%s",msg);
}
