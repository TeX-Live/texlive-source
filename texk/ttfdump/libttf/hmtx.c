/* hmtx.c -- Horizontal Metrics Table
 * Copyright (C) 1997 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: hmtx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadHMTX(FILE *fp,HMTXPtr hmtx,ULONG offset);

/* Caution: Because of interdependency between tables, 
 * table hhea and maxp should be well initalized before
 * ttfInitHMTX is been called
 */
void ttfInitHMTX(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('h', 'm', 't', 'x');
    TableDirPtr ptd;
    
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL) 
	{
	    font->hmtx = XCALLOC1 (HMTX);
	    font->hmtx->numberOfHMetrics = font->hhea->numberOfHMetrics;
	    font->hmtx->numberOfLSB = font->maxp->numGlyphs - 
		font->hhea->numberOfHMetrics; 
	    ttfLoadHMTX(font->fp,font->hmtx,ptd->offset);
	}
}

static void ttfLoadHMTX(FILE *fp,HMTXPtr hmtx,ULONG offset)
{
    USHORT i,n = hmtx->numberOfHMetrics,m=hmtx->numberOfLSB;

    xfseek(fp, offset, SEEK_SET, "ttfLoadHMTX");
    
    hmtx->hMetrics = XCALLOC (n, longHorMetric);
    for (i=0;i<n;i++)
	{
	    (hmtx->hMetrics+i)->advanceWidth = ttfGetuFWord(fp);
	    (hmtx->hMetrics+i)->lsb = ttfGetFWord(fp);
	}

    /* codes dealing with leftSideBearing entry */
    if (m)
	{
	    hmtx->leftSideBearing = XCALLOC (m, FWord);
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
