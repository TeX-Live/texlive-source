/* ltsh.c -- Linear Threshold table
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: ltsh.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadLTSH(FILE *fp,LTSHPtr ltsh,ULONG offset);

void ttfInitLTSH(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('L', 'T', 'S', 'H');
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->ltsh = XCALLOC1 (LTSH);
	    ttfLoadLTSH(font->fp,font->ltsh,ptd->offset);
	}
}

static void ttfLoadLTSH (FILE *fp,LTSHPtr ltsh,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadLTSH");
    
    ltsh->version = ttfGetUSHORT(fp);
    ltsh->numGlyphs = ttfGetUSHORT(fp);
    
    ltsh->yPels = ttfMakeBYTE (ltsh->numGlyphs, fp);
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
