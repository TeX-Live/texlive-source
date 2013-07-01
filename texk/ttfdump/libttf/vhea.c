/* vhea.c -- Vertical Header Table
 * Copyright (C) 1997 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: vhea.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadVHEA(FILE *fp,VHEAPtr vhea,ULONG offset);

void ttfInitVHEA(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('v', 'h', 'e', 'a');
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->vhea = XCALLOC1 (VHEA);
	    ttfLoadVHEA(font->fp,font->vhea,ptd->offset);
	}
}

static void ttfLoadVHEA (FILE *fp,VHEAPtr vhea,ULONG offset)
{
    int i;

    xfseek(fp, offset, SEEK_SET, "ttfLoadVHEA");
    
    vhea->version = ttfGetFixed(fp);
    vhea->ascent  = ttfGetSHORT(fp);
    vhea->descent = ttfGetSHORT(fp);
    vhea->lineGap = ttfGetSHORT(fp);
    vhea->advanceHeightMax = ttfGetSHORT(fp);
    vhea->minTopSideBearing = ttfGetSHORT(fp);
    vhea->minBottomSideBearing = ttfGetSHORT(fp);
    vhea->yMaxExtent = ttfGetSHORT(fp);
    vhea->caretSlopeRise = ttfGetSHORT(fp);
    vhea->caretSlopeRun = ttfGetSHORT(fp);
    vhea->caretOffset = ttfGetSHORT(fp);
    for(i=0;i<4;i++)
	(vhea->reserved)[i] = ttfGetSHORT(fp);
    vhea->metricDataFormat = ttfGetSHORT(fp);
    vhea->numOfLongVerMetrics = ttfGetUSHORT(fp);
}

void ttfPrintVHEA(FILE *fp,VHEAPtr vhea)
{
    int i,b[2];

    fprintf(fp,"'VHEA' - Vertical Header Table\n");
    fprintf(fp,"------------------------------\n");

    FixedSplit(vhea->version,b);
    
    fprintf(fp,"\t version:\t %d.%d\n",b[1],b[0]);
    fprintf(fp,"\t ascent:\t %d\n",vhea->ascent);
    fprintf(fp,"\t descent:\t %d\n",vhea->descent);
    fprintf(fp,"\t lineGap:\t %d\n",vhea->lineGap);
    fprintf(fp,"\t advanceHeightMax: %d\n",vhea->advanceHeightMax);
    fprintf(fp,"\t minTopSideBearing: %d\n",vhea->minTopSideBearing);
    fprintf(fp,"\t minBottomBearing: %d\n",vhea->minBottomSideBearing);
    fprintf(fp,"\t yMaxExtent:\t %d\n",vhea->yMaxExtent);
    fprintf(fp,"\t caretSlopeRise: %d\n", vhea->caretSlopeRise);
    fprintf(fp,"\t caretSlopeRun: %d\n", vhea->caretSlopeRun);
    fprintf(fp,"\t caretOffset:\t %d\n", vhea->caretOffset);
    for(i=0;i<4;i++)
	fprintf(fp,"\t reserved %d:\t %d\n",i,(vhea->reserved)[i]);
    fprintf(fp,"\t metricDataFormat:\t %d\n", vhea->metricDataFormat);
    fprintf(fp,"\t numOfLongVerMetrics: %d\n",vhea->numOfLongVerMetrics);
}

void ttfFreeVHEA(VHEAPtr vhea)
{
    if (vhea != NULL)
	{
	    free(vhea);
	}
}
