/* pclt.c -- PCL5 Table
 * Copyright (C) 1997 Li-Da Lho, All right reserved  
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: pclt.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadPCLT(FILE *fp,PCLTPtr pclt,ULONG offset);

void ttfInitPCLT(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('P', 'C', 'L', 'T');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->pclt = XCALLOC1 (PCLT);
	    ttfLoadPCLT(font->fp,font->pclt,ptd->offset);
	}
}

static void ttfLoadPCLT(FILE *fp,PCLTPtr pclt,ULONG offset)
{
    int i;

    xfseek(fp, offset, SEEK_SET, "ttfLoadPCLT");
    
    pclt->version = ttfGetFixed(fp);
    pclt->FontNumber = ttfGetULONG(fp);
    pclt->Pitch = ttfGetUSHORT(fp);
    pclt->xHeight = ttfGetUSHORT(fp);
    pclt->Style = ttfGetUSHORT(fp);
    pclt->TypeFamily = ttfGetUSHORT(fp);
    pclt->CapHeight = ttfGetUSHORT(fp);
    pclt->SymbolSet = ttfGetUSHORT(fp);
    for (i=0;i<16;i++)
	pclt->Typeface[i] = ttfGetCHAR(fp);
    for (i=0;i<8;i++)
	pclt->CharacterComplement[i] = ttfGetCHAR(fp);
    for (i=0;i<6;i++)
	pclt->FileName[i] = ttfGetCHAR(fp);
    pclt->StrokeWeight = ttfGetCHAR(fp);
    pclt->WidthType = ttfGetCHAR(fp);
    pclt->SerifStyle = ttfGetBYTE(fp);
}

void ttfPrintPCLT(FILE *fp,PCLTPtr pclt)
{
    int i;
    int b[2];
    
    FixedSplit(pclt->version,b);

    fprintf(fp,"`PCLT' Table - Printer Command Language Table\n");
    fprintf(fp,"---------------------------------------------\n");
    fprintf(fp,"\t version \t %d.%d\n",b[1],b[0]);
    fprintf(fp,"\t fontNumber \t %d (0x%x)\n",pclt->FontNumber,
	    pclt->FontNumber  );
    fprintf(fp,"\t pitch   \t %d\n",pclt->Pitch);
    fprintf(fp,"\t xHeight \t %d\n",pclt->xHeight);
    fprintf(fp,"\t style   \t %d\n",pclt->Style);
    fprintf(fp,"\t typeFamily \t %x\n",pclt->TypeFamily);
    fprintf(fp,"\t capHeight  \t %d\n",pclt->CapHeight);
    fprintf(fp,"\t symbolSet  \t %d\n",pclt->SymbolSet);
    fprintf(fp,"\t typeFace   \t ");
    for (i=0;i<6;i++)
	fprintf(fp,"%c",pclt->Typeface[i]);
    fprintf(fp,"\n");

    fprintf(fp,"\t characterComplement 0x");
    for (i=0;i<8;i++)
	fprintf(fp,"%02x",(unsigned char)pclt->CharacterComplement[i]);
    fprintf(fp,"\n");

    fprintf(fp,"\t fileName   \t ");
    for (i=0;i<6;i++)
	fprintf(fp,"%c",pclt->FileName[i]);
    fprintf(fp,"\n");

    fprintf(fp,"\t strokeWeight \t %d\n",pclt->StrokeWeight);
    fprintf(fp,"\t widthType  \t %d\n",pclt->WidthType);
    fprintf(fp,"\t serifStyle \t %d\n",pclt->SerifStyle);
}

void ttfFreePCLT(PCLTPtr pclt)
{
    if (pclt != NULL)
	free(pclt);
}
