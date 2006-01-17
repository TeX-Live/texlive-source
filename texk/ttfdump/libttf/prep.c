/* prep.c -- Control Value Program
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

/* 	$Id: prep.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: prep.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */
static void ttfLoadPREP(FILE *fp,BYTE *prep,USHORT lenght,ULONG offset);

void ttfInitPREP(TTFontPtr font)
{
    ULONG tag = 'p' | 'r' << 8 | 'e' << 16 | 'p' << 24;
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->prepLength = ptd->length;
	    font->prep = (BYTE *) calloc(font->prepLength, sizeof(BYTE));
	    if (font->prep != NULL)
		ttfLoadPREP(font->fp,font->prep,font->prepLength,ptd->offset);
	}
}

static void ttfLoadPREP(FILE *fp,BYTE *prep,USHORT length,ULONG offset)
{
    if (fseek(fp,offset,SEEK_SET) != 0)
	ttfError("Fseek Failed in ttfLoadPREP \n");

    if (fread(prep, sizeof(BYTE), length, fp) != length)
	ttfError("Error when getting PREP\n");
}

void ttfPrintPREP(FILE *fp, BYTE *prep, USHORT length)
{
    ttfPrintInstructions(fp, prep);
}

void ttfFreePREP(BYTE *prep)
{
    if (prep != NULL)
	free(prep);
}
