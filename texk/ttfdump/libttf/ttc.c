/* ttc.c -- True Type Collection Support 
 * Copyright (C) 1997 Li-Da Lho, All right reserved
 */ 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ttf.h"
#include "ttfutil.h"
#include "ttc.h"

/* 	$Id: ttc.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadTTCFont(TTCHeaderPtr ttc, FILE *fp, const char *filename);

TTCHeaderPtr ttfLoadTTCHeader(char * filename)
{
    ULONG tag =  FT_MAKE_TAG ('t', 't', 'c', 'f');
    TTCHeaderPtr ttc;
    FILE *fp;
    
    if ((fp = fopen_truetype (filename)) == NULL)
	{
	    fprintf(stderr,"Can't open ttc file %s\n",filename);
	    return NULL;
	}
    ttc = XCALLOC1 (TTCHeader);

    /* True Type Collection's TTCHeader */
    if ((ttc->TTCTag = ttfGetULONG(fp)) == tag)
	{
	    ttc->version = ttfGetFixed(fp);
	    ttc->DirCount = (USHORT) ttfGetULONG(fp);
	    ttc->offset = ttfMakeULONG (ttc->DirCount, fp);
	    ttfLoadTTCFont(ttc,fp,filename);
	    return ttc;
	}
    else
	/* a file with ttc in name but no ttc tag in it */
	return NULL;
}

static void ttfLoadTTCFont(TTCHeaderPtr ttc,FILE *fp,const char *filename)
{
    unsigned int i;
    
    ttc->font = XCALLOC (ttc->DirCount, TTFont);

    /* currently, we are loading all fonts in a TTC file. 
     * I still can't find a good way to share the data. */
    for (i=0;i<ttc->DirCount;i++)
	{
	    (ttc->font+i) -> fp = fp;
	    (ttc->font+i) -> ttfname = XTALLOC (strlen(filename)+16, char);
	    sprintf ((ttc->font+i)->ttfname, "%s:%u", filename, i);
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
