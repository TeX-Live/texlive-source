/* head.c -- Header Table
 * Copyright (C) 1996 Li-Da Lho, All right reserved
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: head.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadHEAD(FILE *fp,HEADPtr head,ULONG offset);

void ttfInitHEAD(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('h', 'e', 'a', 'd');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->head = XCALLOC1 (HEAD);
	    ttfLoadHEAD(font->fp,font->head,ptd->offset);    
	}
}

static void ttfLoadHEAD(FILE *fp,HEADPtr head,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadHEAD");

    head->version = ttfGetFixed(fp);
    head->fontRevision = ttfGetFixed(fp);
    head->checkSumAdj = ttfGetULONG(fp);
    head->magicNumber = ttfGetULONG(fp);
    head->flags = ttfGetUSHORT(fp);
    head->unitsPerEm = ttfGetUSHORT(fp);
    
    ttfReadULONG (head->created, 2, fp);
    ttfReadULONG (head->modified, 2, fp);
    
    head->xMin = ttfGetFWord(fp);
    head->yMin = ttfGetFWord(fp);
    head->xMax = ttfGetFWord(fp);
    head->yMax = ttfGetFWord(fp);

    head->macStyle = ttfGetUSHORT(fp);
    head->lowestRecPPEM = ttfGetUSHORT(fp);
 
    head->fontDirectionHint = ttfGetSHORT(fp);
    head->indexToLocFormat = ttfGetSHORT(fp);
    head->glyphDataFormat = ttfGetSHORT(fp);
}

void ttfPrintHEAD(FILE *fp,HEADPtr head)
{
    int b1[2],b2[2];
    
    FixedSplit(head->version,b1);
    FixedSplit(head->fontRevision,b2);

    fprintf(fp,"'head' Table - Font Header\n");
    fprintf(fp,"--------------------------\n");
    fprintf(fp,"\t 'head' version:\t %d.%d\n",b1[1],b1[0]);
    fprintf(fp,"\t fontReversion:\t\t %d.%d\n",b2[1],b2[0]);
    fprintf(fp,"\t checkSumAdjustment:\t 0x%08x\n",head->checkSumAdj);
    fprintf(fp,"\t magicNumber:\t\t 0x%08x\n",head->magicNumber);
    fprintf(fp,"\t flags:\t\t\t 0x%04x\n",head->flags);
    fprintf(fp,"\t unitsPerEm:\t\t %d\n",head->unitsPerEm);

    /* don't know how to compute */
    fprintf(fp,"\t created:\t\t 0x%08x%08x\n", head->created[0], head->created[1]);
    fprintf(fp,"\t modified:\t\t 0x%08x%08x\n", head->modified[0], head->modified[1]);

    fprintf(fp,"\t xMin:\t\t\t %d\n",head->xMin);
    fprintf(fp,"\t yMin:\t\t\t %d\n",head->yMin);
    fprintf(fp,"\t xMax:\t\t\t %d\n",head->xMax);
    fprintf(fp,"\t yMax:\t\t\t %d\n",head->yMax);
    fprintf(fp,"\t macStyle bits:\t\t 0x%04x\n",head->macStyle);
    fprintf(fp,"\t lowestRecPPEM:\t\t %d\n",head->lowestRecPPEM);
    fprintf(fp,"\t fontDirectionHint:\t %d\n",head->fontDirectionHint);
    fprintf(fp,"\t indexToLocFormat:\t %d\n",head->indexToLocFormat);
    fprintf(fp,"\t glyphDataFormat:\t %d\n",head->glyphDataFormat);
    fprintf(fp,"\n");
}

void ttfFreeHEAD(HEADPtr head)
{
    free(head);
}
