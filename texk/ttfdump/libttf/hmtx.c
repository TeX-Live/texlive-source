/* hmtx.c -- Horizontal Metrics Table
 * Copyright (C) 1997 Li-Da Lho, All right reserved 
 */
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: hmtx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: hmtx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static HMTXPtr ttfAllocHMTX(TTFontPtr font);
static void ttfLoadHMTX(FILE *fp,HMTXPtr hmtx,ULONG offset);

/* Caution: Because of interdependency between tables, 
 * table hhea and maxp should be well initalized before
 * ttfInitHMTX is been called
 */
void ttfInitHMTX(TTFontPtr font)
{
    ULONG tag = 'h' | 'm' << 8 | 't' << 16 | 'x' << 24;
    TableDirPtr ptd;
    
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL) 
	{
	    font->hmtx = ttfAllocHMTX(font);
	    font->hmtx->numberOfHMetrics = font->hhea->numberOfHMetrics;
	    font->hmtx->numberOfLSB = font->maxp->numGlyphs - 
		font->hhea->numberOfHMetrics; 
	    ttfLoadHMTX(font->fp,font->hmtx,ptd->offset);
	}
}
static HMTXPtr ttfAllocHMTX(TTFontPtr font)
{
    HMTXPtr hmtx;

    if ((hmtx = (HMTXPtr) calloc(1,sizeof(HMTX))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return hmtx;
}
static void ttfLoadHMTX(FILE *fp,HMTXPtr hmtx,ULONG offset)
{
    USHORT i,n = hmtx->numberOfHMetrics,m=hmtx->numberOfLSB;

    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLOADHMTX \n");
    
    if ((hmtx->hMetrics = (longHorMetric *) calloc(n,sizeof(longHorMetric))) != NULL)
	{
	    for (i=0;i<n;i++)
		{
		    (hmtx->hMetrics+i)->advanceWidth = ttfGetuFWord(fp);
		    (hmtx->hMetrics+i)->lsb = ttfGetFWord(fp);
		}
	}
    else
	{
	    ttfError("Out Of Memory in __FILE__ : __LINE__\n"); 
	}

    /* codes dealing with leftSideBearing entry */
    if (m && ((hmtx->leftSideBearing = (FWord *) calloc(m,sizeof(FWord))) != NULL))
	{
	    for (i=0;i<m;i++)
		{
		    (hmtx->leftSideBearing)[i] = ttfGetFWord(fp);
		}
	}
}

void ttfPrintHMTX(FILE *fp,HMTXPtr hmtx)
{
    int i;
    
    fprintf(fp,"'hmtx' Table - Horizontal Metrics\n");
    fprintf(fp,"---------------------------------\n");
    
    for (i=0;i<hmtx->numberOfHMetrics;i++)
	{
	    fprintf(fp,"\t %4d. advWid: %4d, LSBear: %4d\n",i,
		    (hmtx->hMetrics+i)->advanceWidth,
		    (hmtx->hMetrics+i)->lsb);
	}

    for (i=0;i<hmtx->numberOfLSB;i++)
	{
	    fprintf(fp,"\t %4d. LSbear: %4d\n",i+hmtx->numberOfHMetrics,
		    (hmtx->leftSideBearing)[i]);
	}
}

void ttfFreeHMTX(HMTXPtr hmtx)
{
    free(hmtx->hMetrics);

    if (hmtx->numberOfLSB)
	free(hmtx->leftSideBearing);

    free(hmtx);
}
