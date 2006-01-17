/* cvt.c -- Control Value Table
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

/* 	$Id: cvt.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: cvt.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */
static void ttfLoadCVT(FILE *fp,FWord *cvt,USHORT lenght,ULONG offset);

void ttfInitCVT(TTFontPtr font)
{
    ULONG tag = 'c' | 'v' << 8 | 't' << 16 | ' ' << 24;
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->cvtLength = ptd->length / sizeof(FWord);
	    font->cvt = (FWord *) calloc(font->cvtLength, sizeof(FWord));
	    if (font->cvt != NULL)
		ttfLoadCVT(font->fp,font->cvt,font->cvtLength,ptd->offset);
	}
}

static void ttfLoadCVT(FILE *fp,FWord *cvt,USHORT length,ULONG offset)
{
    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLoadCVT \n");

    if (fread(cvt, sizeof(FWord), length, fp) != length)
	ttfError("Error when getting CVT\n");

#ifndef WORDS_BIGENDIAN
    TwoByteSwap((unsigned char *) cvt,length*sizeof(FWord));
#endif
}

void ttfPrintCVT(FILE *fp, FWord *cvt, USHORT cvtLength)
{
    USHORT i;

    fprintf(fp,"'cvt ' Table - Control Value Table\n");
    fprintf(fp,"----------------------------------\n");
    fprintf(fp,"Size = %d bytes, %d entries\n",cvtLength*sizeof(FWord),
	    cvtLength);

    for (i=0;i<cvtLength;i++)
	{
	    fprintf(fp,"\t %4d. \t %d\n",i,cvt[i]);
	}
    fprintf(fp,"\n");
}

void ttfFreeCVT(FWord *cvt)
{
    if (cvt != NULL)
	free(cvt);
}
