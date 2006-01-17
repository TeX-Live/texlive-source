/* vmtx.c -- Vertical Metrics Table
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

/* 	$Id: vmtx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: vmtx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static VMTXPtr ttfAllocVMTX(TTFontPtr font);
static void ttfLoadVMTX(FILE *fp,VMTXPtr vmtx,ULONG offset);

/* Caution: Because of interdependency between tables, 
 * table vhea and maxp should be well initalized before
 * ttfInitVMTX has been called
 */
void ttfInitVMTX(TTFontPtr font)
{
    ULONG tag = 'v' | 'm' << 8 | 't' << 16 | 'x' << 24;
    TableDirPtr ptd;
    
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL) 
	{
	    font->vmtx = ttfAllocVMTX(font);
	    font->vmtx->numOfLongVerMetrics = 
		font->vhea->numOfLongVerMetrics;
	    font->vmtx->numOfTSB = font->maxp->numGlyphs - 
		font->vhea->numOfLongVerMetrics; 
	    ttfLoadVMTX(font->fp,font->vmtx,ptd->offset);
	}
}
static VMTXPtr ttfAllocVMTX(TTFontPtr font)
{
    VMTXPtr vmtx;

    if ((vmtx = (VMTXPtr) calloc(1,sizeof(VMTX))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return vmtx;
}
static void ttfLoadVMTX(FILE *fp,VMTXPtr vmtx,ULONG offset)
{
    USHORT i,n = vmtx->numOfLongVerMetrics,m=vmtx->numOfTSB;

    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLOADVMTX \n");
    
    if ((vmtx->vMetrics = (longVerMetric *) calloc(n,sizeof(longVerMetric))) != NULL)
	{
	    for (i=0;i<n;i++)
		{
		    (vmtx->vMetrics+i)->advanceHeight = ttfGetuFWord(fp);
		    (vmtx->vMetrics+i)->topSideBearing = ttfGetFWord(fp);
		}
	}
    else
	{
	    ttfError("Out Of Memory in __FILE__ : __LINE__\n"); 
	}

    /* codes dealing with leftSideBearing entry */
    if (m && ((vmtx->topSideBearing = (FWord *) calloc(m,sizeof(FWord))) != NULL))
	{
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
