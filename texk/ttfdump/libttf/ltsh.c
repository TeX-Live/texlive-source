/* ltsh.c -- Linear Threshold table
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

/* 	$Id: ltsh.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: ltsh.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static LTSHPtr ttfAllocLTSH(TTFontPtr font);
static void ttfLoadLTSH(FILE *fp,LTSHPtr ltsh,ULONG offset);

void ttfInitLTSH(TTFontPtr font)
{
    ULONG tag = 'L' | 'T' << 8 | 'S' << 16 | 'H' << 24;
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->ltsh = ttfAllocLTSH(font);
	    ttfLoadLTSH(font->fp,font->ltsh,ptd->offset);
	}
}
static LTSHPtr ttfAllocLTSH(TTFontPtr font)
{
    LTSHPtr ltsh;
    
    if ((ltsh = (LTSHPtr) calloc(1,sizeof(LTSH))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return ltsh;
}
static void ttfLoadLTSH (FILE *fp,LTSHPtr ltsh,ULONG offset)
{
    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLoadLTSH \n");	
    
    ltsh->version = ttfGetUSHORT(fp);
    ltsh->numGlyphs = ttfGetUSHORT(fp);
    
    ltsh->yPels = (BYTE *) calloc(ltsh->numGlyphs, sizeof(BYTE));

    if(fread(ltsh->yPels, sizeof(BYTE), ltsh->numGlyphs, fp) != ltsh->numGlyphs)
	ttfError("Error when getting yPels\n");
}

void ttfPrintLTSH(FILE *fp,LTSHPtr ltsh)
{
    int i;

    fprintf(fp,"'LTSH' Table - Linear Threshold Table\n");
    fprintf(fp,"-------------------------------------\n");
    fprintf(fp,"'LTSH' Version:\t %d\n",ltsh->version);
    fprintf(fp,"Number of Glyphs:\t %d\n",ltsh->numGlyphs);
    fprintf(fp,"\t Glyph # \t Threshold\n");
    
    for (i=0;i<ltsh->numGlyphs;i++)
	{
	    fprintf(fp,"\t %d. \t\t %d\n",i,ltsh->yPels[i]);
	}
}

void ttfFreeLTSH(LTSHPtr ltsh)
{    
    if (ltsh != NULL)
	{
	   free(ltsh->yPels);
	   free(ltsh);
	}
}
