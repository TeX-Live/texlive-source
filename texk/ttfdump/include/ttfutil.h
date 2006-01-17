#ifndef __TTF_UTIL_H
#define __TTF_UTIL_H

#include "ttf.h"

/* $Id: ttfutil.h,v 1.2 1998/07/06 06:07:01 werner Exp $ */

void FixedSplit(Fixed f, int b[]);
void TwoByteSwap(unsigned char *buf, int nbytes);
void FourByteSwap(unsigned char *buf, int nbytes);
void FourByteSwap(unsigned char *buf, int nbytes);
void ttfError(char *msg);

#include "protos.h"

#endif /* __TTF_UTIL_H */


/* end of ttfutil.h */
