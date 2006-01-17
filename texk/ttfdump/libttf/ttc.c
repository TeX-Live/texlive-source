/* ttc.c -- True Type Collection Support 
 * Copyright (C) 1997 Li-Da Lho, All right reserved
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "ttf.h"
#include "ttfutil.h"
#include "ttc.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: ttc.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: ttc.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

TTCHeaderPtr ttfLoadTTCHeader(char * filename)
{
    ULONG tag =  't' | 't' << 8 | 'c' << 16 | 'f' << 24;
    TTCHeaderPtr ttc;
    FILE *fp;
    
    if ((fp = fopen (filename,"r")) == NULL)
	{
	    fprintf(stderr,"Can't open ttc file %s\n",filename);
	    return NULL;
	}
    ttc = (TTCHeaderPtr) calloc(1, sizeof(TTCHeader));

    /* True Type Collection's TTCHeader */
    if ((ttc->TTCTag = (ULONG) ttfGetLSB32(fp)) == tag)
	{
	    ttc->version = ttfGetFixed(fp);
	    ttc->DirCount = (USHORT) ttfGetULONG(fp);
	    ttc->offset = (ULONG *) calloc(ttc->DirCount,
					   sizeof(ULONG));
	    if (fread(ttc->offset, sizeof(ULONG), 
		      ttc->DirCount,fp) != ttc->DirCount)
		{
		    ttfError("Error when reading Table Direcotry Offset/n");
		}
#ifndef WORDS_BIGENDIAN
	    FourByteSwap((unsigned char *) ttc->offset,
			 ttc->DirCount*sizeof(ULONG));
#endif
	    ttfLoadTTCFont(ttc,fp);
	    return ttc;
	}
    else
	/* a file with ttc in name but no ttc tag in it */
	return NULL;
}

void ttfLoadTTCFont(TTCHeaderPtr ttc,FILE *fp)
{
    int i;
    
    ttc->font = (TTFontPtr) calloc(ttc->DirCount, sizeof(TTFont));

    if (ttc->font == NULL)
	{
	    ttfError("Out of Memory");
	    return;
	}
    /* currently, we are loading all fonts in a TTC file. 
     * I still can't find a good way to share the data. */
    for (i=0;i<ttc->DirCount;i++)
	{
	    (ttc->font+i) -> fp = fp;
	    ttfLoadFont((ttc->font+i),(ttc->offset)[i]);
	}
}

void ttfFreeTTCFont(TTCHeaderPtr ttc)
{
   int i;
   
   for (i=0;i<ttc->DirCount;i++)
	{
	    ttfFreeRequiredTables(ttc->font+i);
    
	    ttfFreeOptionalTables(ttc->font+i);
	    
	    ttfFreeTableDir((ttc->font+i)->dir);
	    free((ttc->font+i)->refcount);
	}

   free(ttc->offset);
   free(ttc->font);
   free(ttc);
}
