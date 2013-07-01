/* vmtx.c -- Vertical Metrics Table
 * Copyright (C) 1997 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: vmtx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadVMTX(FILE *fp,VMTXPtr vmtx,ULONG offset);

/* Caution: Because of interdependency between tables, 
 * table vhea and maxp should be well initalized before
 * ttfInitVMTX has been called
 */
void ttfInitVMTX(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('v', 'm', 't', 'x');
    TableDirPtr ptd;
    
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL) 
	{
	    font->vmtx = XCALLOC1 (VMTX);
	    font->vmtx->numOfLongVerMetrics = 
		font->vhea->numOfLongVerMetrics;
	    font->vmtx->numOfTSB = font->maxp->numGlyphs - 
		font->vhea->numOfLongVerMetrics; 
	    ttfLoadVMTX(font->fp,font->vmtx,ptd->offset);
	}
}

static void ttfLoadVMTX(FILE *fp,VMTXPtr vmtx,ULONG offset)
{
    USHORT i,n = vmtx->numOfLongVerMetrics,m=vmtx->numOfTSB;

    xfseek(fp, offset, SEEK_SET, "ttfLoadVMTX");
    
    vmtx->vMetrics = XCALLOC (n, longVerMetric);
    for (i=0;i<n;i++)
	{
	    (vmtx->vMetrics+i)->advanceHeight = ttfGetuFWord(fp);
	    (vmtx->vMetrics+i)->topSideBearing = ttfGetFWord(fp);
	}

    /* codes dealing with topSideBearing entry */
    if (m)
	{
	    vmtx->topSideBearing = XCALLOC (m, FWord);
	    for (i=0;i<m;i++)
		{
		    (vmtx->topSideBearing)[i] = ttfGetFWord(fp);
		}
	}
}

void ttfPrintVMTX(FILE *fp,VMTXPtr vmtx)
{
    int i;
    
    fprintf(fp,"'vmtx' Table - Vertical Metrics\n");
    fprintf(fp,"---------------------------------\n");
    
    for (i=0;i<vmtx->numOfLongVerMetrics;i++)
	{
	    fprintf(fp,"\t %4d. advWid: %4d, TSBear: %4d\n",i,
		    (vmtx->vMetrics+i)->advanceHeight,
		    (vmtx->vMetrics+i)->topSideBearing);
	}

    for (i=0;i<vmtx->numOfTSB;i++)
	{
	    fprintf(fp,"\t %4d. TSBear: %4d\n",i+vmtx->numOfLongVerMetrics,
		    (vmtx->topSideBearing)[i]);
	}
}

void ttfFreeVMTX(VMTXPtr vmtx)
{
    if (vmtx != NULL)
	{
	    free(vmtx->vMetrics);

	    if (vmtx->numOfTSB)
		free(vmtx->topSideBearing);

	    free(vmtx);
	}
}
