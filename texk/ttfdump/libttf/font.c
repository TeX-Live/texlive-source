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

/* 	$Id: font.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfInitInterpreter(TTFontPtr font);

TTFontPtr ttfInitFont(char *filename)
{
    TTFontPtr font;

    font = XCALLOC1 (TTFont);

    font->ttfname = filename;
    if ((font->fp = fopen_truetype (filename)) == NULL)
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
    xfseek(font->fp, offset, SEEK_SET, "ttfLoadFont");

    /* offset table */
    font->version = ttfGetFixed(font->fp);
    font->numTables = ttfGetUSHORT(font->fp);

    ttfInitTableDir(font,offset);
    
    ttfLoadRequiredTables(font);
    ttfLoadOptionalTables(font);
    ttfLoadOpenTypeTables(font);

    ttfInitInterpreter(font);

    /* initialize the reference count to 1 */
    font->refcount = XCALLOC1 (int);
    *(font->refcount) = 1;
}
void ttfFreeFont(TTFontPtr font)
{
    ttfFreeRequiredTables(font);
    
    ttfFreeOptionalTables(font);
    
    ttfFreeOpenTypeTables(font);
    
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
    ttfInitLOCA(font);
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
    ttfFreeLOCA(font->loca);
    ttfFreePREP(font->prep);
    ttfFreeLTSH(font->ltsh);
    ttfFreePCLT(font->pclt);
    ttfFreeVDMX(font->vdmx);
    ttfFreeVHEA(font->vhea);
    ttfFreeVMTX(font->vmtx);
}

void ttfLoadOpenTypeTables(TTFontPtr font)
{
    ttfInitGPOS(font);
    ttfInitGSUB(font);
}
void ttfFreeOpenTypeTables(TTFontPtr font)
{
    ttfFreeGPOS(font->gpos);
    ttfFreeGSUB(font->gsub);
}

#if 0
/* Not used */
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

    newfont = XTALLOC1 (TTFont);
    
    memcpy(newfont, font, sizeof(TTFont));
    newfont->refcount += 1;

    return newfont;
}
#endif

static void ttfInitInterpreter(TTFontPtr font)
{
    ttfInitStorageArea(font);
    ttfInitStack(font);
    ttfInitGraphicsState(font);    
}
