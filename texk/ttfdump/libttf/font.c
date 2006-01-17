/* font.c -- general font init and clean up codes
 * Copyright (C) 1996 Li-Da Lho, All right reserved  
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"
#include "protos.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: font.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: font.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static void ttfInitInterpreter(TTFontPtr font);

TTFontPtr ttfInitFont(char *filename)
{
    TTFontPtr font;

    font = (TTFontPtr) calloc(1,sizeof(TTFont));

    font->ttfname = filename;
    if ((font->fp = fopen (filename,"r")) == NULL)
	{
	    fprintf(stderr,"Can't open ttf file %s\n",filename);
	    free(font);
	    return NULL;
	}
    
    ttfLoadFont(font, 0);
    return font;
}
void ttfLoadFont(TTFontPtr font, ULONG offset)
{
    if (fseek(font->fp,offset,SEEK_SET) != 0)
	ttfError("Fseek Failed\n");

    /* offset table */
    font->version = ttfGetFixed(font->fp);
    font->numTables = ttfGetUSHORT(font->fp);

    ttfInitTableDir(font,offset);
    
    ttfLoadRequiredTables(font);
    ttfLoadOptionalTables(font);

    ttfInitInterpreter(font);

    /* initialize the reference count to 1 */
    font->refcount = (int *) calloc(1, sizeof(int));
    *(font->refcount) = 1;
}
void ttfFreeFont(TTFontPtr font)
{
    ttfFreeRequiredTables(font);
    
    ttfFreeOptionalTables(font);
    
    ttfFreeTableDir(font->dir);
    free(font->refcount);
    free(font);
}

void ttfLoadRequiredTables(TTFontPtr font)
{
    ttfInitCMAP(font);
    ttfInitNAME(font);

    ttfInitMAXP(font);
    ttfInitHEAD(font);
    ttfInitLOCA(font);

    ttfInitGlyphCache(font);
    ttfInitGLYF(font);

    ttfInitHHEA(font);
    ttfInitHMTX(font);

    ttfInitPOST(font);
    ttfInitOS2(font);
}

void ttfFreeRequiredTables(TTFontPtr font)
{
    ttfFreeCMAP(font->cmap);
    ttfFreeNAME(font->name);

    ttfCleanUpGlyphCache(font);
    ttfFreeLOCA(font->loca);
    ttfFreeHEAD(font->head);
    ttfFreeMAXP(font->maxp);

    ttfFreeHMTX(font->hmtx);
    ttfFreeHHEA(font->hhea);
     
    ttfFreePOST(font->post);
    ttfFreeOS2(font->os2);
}

void ttfLoadOptionalTables(TTFontPtr font)
{
    ttfInitCVT(font);
    ttfInitFPGM(font);
    ttfInitGASP(font);
    ttfInitHDMX(font);
    ttfInitKERN(font);
    ttfInitPREP(font);
    ttfInitLTSH(font);
    ttfInitPCLT(font);
    ttfInitVDMX(font);
    ttfInitVHEA(font);
    ttfInitVMTX(font);
}
void ttfFreeOptionalTables(TTFontPtr font)
{
    ttfFreeCVT(font->cvt);
    ttfFreeFPGM(font->fpgm);
    ttfFreeGASP(font->gasp);
    ttfFreeHDMX(font->hdmx);
    ttfFreeKERN(font->kern);
    ttfFreePREP(font->prep);
    ttfFreeLTSH(font->ltsh);
    ttfFreePCLT(font->pclt);
    ttfFreeVDMX(font->vdmx);
    ttfFreeVHEA(font->vhea);
    ttfFreeVMTX(font->vmtx);
}

/* make a clone of the origional font
 * This is use for fonts that support more than one encoding scheme
 *
 * Problems: 
 *       1. which of those members can be shared between two instances
 *       2. How can we free a font safely
 */
TTFontPtr ttfCloneFont(TTFontPtr font)
{
    TTFontPtr newfont;

    newfont = (TTFontPtr) calloc(1,sizeof(TTFont));
    
    memcpy(newfont, font, sizeof(TTFont));
    newfont->refcount += 1;

    return newfont;
}
static void ttfInitInterpreter(TTFontPtr font)
{
    ttfInitStorageArea(font);
    ttfInitStack(font);
    ttfInitGraphicsState(font);    
}
