/* fpgm.c -- Font Program
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: fpgm.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadFPGM(FILE *fp,BYTE *fpgm,USHORT lenght,ULONG offset);

void ttfInitFPGM(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('f', 'p', 'g', 'm');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->fpgmLength = ptd->length;
	    font->fpgm = XCALLOC (font->fpgmLength, BYTE);
	    ttfLoadFPGM(font->fp,font->fpgm,font->fpgmLength,ptd->offset);
	}
}

static void ttfLoadFPGM(FILE *fp,BYTE *fpgm,USHORT length,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadFPGM");

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
