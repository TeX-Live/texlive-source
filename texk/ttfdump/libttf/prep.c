/* prep.c -- Control Value Program
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: prep.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadPREP(FILE *fp,BYTE *prep,USHORT lenght,ULONG offset);

void ttfInitPREP(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('p', 'r', 'e', 'p');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->prepLength = ptd->length;
	    font->prep = XCALLOC (font->prepLength, BYTE);
	    ttfLoadPREP(font->fp,font->prep,font->prepLength,ptd->offset);
	}
}

static void ttfLoadPREP(FILE *fp,BYTE *prep,USHORT length,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadPREP");

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
