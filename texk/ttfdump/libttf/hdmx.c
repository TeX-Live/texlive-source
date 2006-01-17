/* hdmx.c -- Horizontal Device Metrics
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

/* 	$Id: hdmx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: hdmx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static HDMXPtr ttfAllocHDMX(TTFontPtr font);
static void ttfLoadHDMX(FILE *fp,HDMXPtr hdmx,ULONG offset);

void ttfInitHDMX(TTFontPtr font)
{
    ULONG tag = 'h' | 'd' << 8 | 'm' << 16 | 'x' << 24;
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->hdmx = ttfAllocHDMX(font);
	    font->hdmx->numGlyphs = font->maxp->numGlyphs;
	    ttfLoadHDMX(font->fp,font->hdmx,ptd->offset);
	}
}
static HDMXPtr ttfAllocHDMX(TTFontPtr font)
{
    HDMXPtr hdmx;
    
    if ((hdmx = (HDMXPtr) calloc(1,sizeof(HDMX))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return hdmx;
}
static void ttfLoadHDMX (FILE *fp,HDMXPtr hdmx,ULONG offset)
{
    int i;

    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLoadHDMX \n");	
    
    hdmx->version = ttfGetUSHORT(fp);
    hdmx->numDevices = ttfGetUSHORT(fp);
    hdmx->size = ttfGetLONG(fp);

    hdmx->Records = (DeviceRecord  *) calloc(hdmx->numDevices, 
					     sizeof(DeviceRecord));
    
    if (hdmx->Records == NULL)
	ttfError("Out of Memory in __FILE__:__LINE__\n");
    else
	for (i=0;i<hdmx->numDevices;i++)
	    {
		hdmx->Records[i].PixelSize = ttfGetBYTE(fp);
		hdmx->Records[i].MaxWidth = ttfGetBYTE(fp);
		hdmx->Records[i].Width = (BYTE *) calloc(hdmx->size,
							 sizeof(BYTE));
		fread ((hdmx->Records+i)->Width, sizeof(BYTE), 
		       hdmx->numGlyphs+1,fp);
	    }
}

void ttfPrintHDMX(FILE *fp,HDMXPtr hdmx)
{
    int i;

    fprintf(fp,"'hdmx' Table - Horizontal Device Metrics\n");
    fprintf(fp,"----------------------------------------\n");

    fprintf(fp,"\t 'hdmx' version:\t %d\n",hdmx->version);
    fprintf(fp,"\t # device records:\t %d\n",hdmx->numDevices);
    fprintf(fp,"\t Record length:\t %d\n",hdmx->size);
    
    for (i=0;i<hdmx->numDevices;i++)
	{
	    int j;
	    fprintf(fp,"\t DevRec  %d: ppem =  %d, maxWid =  %d\n",
		    i,hdmx->Records[i].PixelSize,hdmx->Records[i].MaxWidth);

	    for (j=0;j<hdmx->numGlyphs;j++)
		fprintf(fp,"\t %d. \t %d\n",j,(hdmx->Records+i)->Width[j]);
	    fprintf(fp,"\n");
	}
    fprintf(fp,"\n");
}

void ttfFreeHDMX(HDMXPtr hdmx)
{
    int i;
    
    if (hdmx != NULL)
	{
	    for (i=0;i<hdmx->numDevices;i++)
		free(hdmx->Records[i].Width);
	    
	    free(hdmx->Records);
	    free(hdmx);
	}
}
