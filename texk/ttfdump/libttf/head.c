/* head.c -- Header Table
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

/* 	$Id: head.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: head.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static HEADPtr ttfAllocHEAD(TTFontPtr font);
static void ttfLoadHEAD(FILE *fp,HEADPtr head,ULONG offset);

void ttfInitHEAD(TTFontPtr font)
{
    ULONG tag = 'h' | 'e' << 8 | 'a' << 16 | 'd' << 24;
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->head = ttfAllocHEAD(font);
	    ttfLoadHEAD(font->fp,font->head,ptd->offset);    
	}
}
static HEADPtr ttfAllocHEAD(TTFontPtr font)
{
    HEADPtr head;

    if ((head = (HEADPtr) calloc(1,sizeof(HEAD))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return head;
}
static void ttfLoadHEAD(FILE *fp,HEADPtr head,ULONG offset)
{
    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLoadHEAD \n");

    head->version = ttfGetFixed(fp);
    head->fontRevision = ttfGetFixed(fp);
    head->checkSumAdj = ttfGetULONG(fp);
    head->magicNumber = ttfGetULONG(fp);
    head->flags = ttfGetUSHORT(fp);
    head->unitsPerEm = ttfGetUSHORT(fp);
    
    fread(head->created,sizeof(char),8,fp);
    fread(head->modified,sizeof(char),8,fp);
    
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
    fprintf(fp,"\t 'head' version:\t %2d.%2d\n",b1[1],b1[0]);
    fprintf(fp,"\t fontReversion:\t\t %2d.%2d\n",b2[1],b2[0]);
    fprintf(fp,"\t checkSumAdjustment:\t 0x%08x\n",head->checkSumAdj);
    fprintf(fp,"\t magicNumber:\t\t 0x%08x\n",head->magicNumber);
    fprintf(fp,"\t flags:\t\t\t 0x%04x\n",head->flags);
    fprintf(fp,"\t unitsPerEm:\t\t %d\n",head->unitsPerEm);

    /* don't know how to compute */
    fprintf(fp,"\t created:\n");
    fprintf(fp,"\t modified:\n");

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
