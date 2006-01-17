/* fpgm.c -- Font Program
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

/* 	$Id: fpgm.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: fpgm.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */
static void ttfLoadFPGM(FILE *fp,BYTE *fpgm,USHORT lenght,ULONG offset);

void ttfInitFPGM(TTFontPtr font)
{
    ULONG tag = 'f' | 'p' << 8 | 'g' << 16 | 'm' << 24;
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->fpgmLength = ptd->length;
	    font->fpgm = (BYTE *) calloc(font->fpgmLength, sizeof(BYTE));
	    if (font->fpgm != NULL)
		ttfLoadFPGM(font->fp,font->fpgm,font->fpgmLength,ptd->offset);
	}
}

static void ttfLoadFPGM(FILE *fp,BYTE *fpgm,USHORT length,ULONG offset)
{
    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLoadCVT \n");

    if (fread(fpgm, sizeof(BYTE), length, fp) != length)
	ttfError("Error when getting CVT\n");
}

void ttfPrintFPGM(FILE *fp, BYTE *fpgm, USHORT length)
{
    ttfPrintInstructions(fp, fpgm);
}

void ttfFreeFPGM(BYTE *fpgm)
{
    if (fpgm != NULL)
	free(fpgm);
}
