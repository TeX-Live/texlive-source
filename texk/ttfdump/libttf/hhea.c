/* hhea.c -- Horizontal header
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 */
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: hhea.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: hhea.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static HHEAPtr ttfAllocHHEA(TTFontPtr font);
static void ttfLoadHHEA(FILE *fp,HHEAPtr hhea,ULONG offset);

void ttfInitHHEA(TTFontPtr font)
{
    ULONG tag = 'h' | 'h' << 8 | 'e' << 16 | 'a' << 24;
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->hhea = ttfAllocHHEA(font);
	    ttfLoadHHEA(font->fp,font->hhea,ptd->offset);
	}
}
static HHEAPtr ttfAllocHHEA(TTFontPtr font)
{
    HHEAPtr hhea;
    
    if ((hhea = (HHEAPtr) calloc(1,sizeof(HHEA))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return hhea;
}
static void ttfLoadHHEA (FILE *fp,HHEAPtr hhea,ULONG offset)
{
    int i;

    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLOADCMAP \n");	

    hhea->version = ttfGetFixed(fp);
    hhea->Ascender = ttfGetFWord(fp);
    hhea->Descender = ttfGetFWord(fp);
    hhea->LineGap = ttfGetFWord(fp);
    hhea->advanceWidthMax = ttfGetuFWord(fp);
    hhea->minLeftSideBearing = ttfGetFWord(fp);
    hhea->minRightSideBearing = ttfGetFWord(fp);
    hhea->xMaxExtent = ttfGetFWord(fp);
    hhea->caretSlopeRise = ttfGetSHORT(fp);
    hhea->caretSlopeRun = ttfGetSHORT(fp);
    for(i=0;i<5;i++)
	(hhea->reserved)[i] = ttfGetSHORT(fp);
    hhea->metricDataFormat = ttfGetSHORT(fp);
    hhea->numberOfHMetrics = ttfGetUSHORT(fp);
}

void ttfPrintHHEA(FILE *fp,HHEAPtr hhea)
{
    int i,b[2];
    
    FixedSplit(hhea->version,b);

    fprintf(fp,"'hhea' Table - Horizontal Header\n");
    fprintf(fp,"--------------------------\n");
    fprintf(fp,"\t 'hhea' version:\t %2d.%2d\n",b[1],b[0]);
    fprintf(fp,"\t yAscender:\t\t %d\n",hhea->Ascender);
    fprintf(fp,"\t yDescender:\t\t %d\n",hhea->Descender);
    fprintf(fp,"\t yLineGap:\t\t %d\n",hhea->LineGap);
    fprintf(fp,"\t advanceWidthMax:\t %d\n",hhea->advanceWidthMax);
    fprintf(fp,"\t minLeftSideBearing:\t %d\n",hhea->minLeftSideBearing);
    fprintf(fp,"\t minRightSideBearing:\t %d\n",hhea->minRightSideBearing);
    fprintf(fp,"\t xMaxExtent:\t\t %d\n",hhea->xMaxExtent);
    fprintf(fp,"\t caretSlopeRise:\t %d\n",hhea->caretSlopeRise);
    fprintf(fp,"\t caretSlopeRun:\t\t %d\n",hhea->caretSlopeRun);
    for (i=0;i<5;i++)
	fprintf(fp,"\t reserved%d:\t\t %d\n",i,(hhea->reserved)[i]);
    fprintf(fp,"\t metricDataFormat:\t %d\n",hhea->metricDataFormat);
    fprintf(fp,"\t numberOfHMetrics:\t %d\n",hhea->numberOfHMetrics);
}

void ttfFreeHHEA(HHEAPtr hhea)
{
    free(hhea);
}
