#ifndef __TTF_TTC_H
#define __TTF_TTC_H

/* $Id: ttc.h,v 1.3 1998/07/06 06:07:01 werner Exp $ */

#include "ttf.h"

typedef struct
{
  ULONG TTCTag;
  Fixed version;
  ULONG DirCount;
  ULONG *offset;                /* length = DirCount */
  TTFontPtr font;
}
TTCHeader, *TTCHeaderPtr;

TTCHeaderPtr ttfLoadTTCHeader(char *filename);
void ttfFreeTTCFont(TTCHeaderPtr ttc);

#endif /* __TTF_TTC_H */


/* end of ttc.h */
