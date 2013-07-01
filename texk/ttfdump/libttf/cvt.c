/* cvt.c -- Control Value Table
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: cvt.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadCVT(FILE *fp,FWord *cvt,USHORT lenght,ULONG offset);

void ttfInitCVT(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('c', 'v', 't', ' ');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->cvtLength = ptd->length / sizeof(FWord);
	    font->cvt = XCALLOC (font->cvtLength, FWord);
	    ttfLoadCVT(font->fp,font->cvt,font->cvtLength,ptd->offset);
	}
}

static void ttfLoadCVT(FILE *fp,FWord *cvt,USHORT length,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadCVT");

    ttfReadFWord (cvt, length, fp);
}

void ttfPrintCVT(FILE *fp, FWord *cvt, USHORT cvtLength)
{
    USHORT i;

    fprintf(fp,"'cvt ' Table - Control Value Table\n");
    fprintf(fp,"----------------------------------\n");
    fprintf(fp,"Size = %d bytes, %d entries\n", (int) (cvtLength*sizeof(FWord)),
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
