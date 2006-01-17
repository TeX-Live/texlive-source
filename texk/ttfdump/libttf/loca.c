#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: loca.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: loca.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static LOCAPtr ttfAllocLOCA(TTFontPtr font);
static void ttfLoadLOCA(FILE *fp,LOCAPtr loca,ULONG offset);

void ttfInitLOCA(TTFontPtr font)
{
    ULONG tag = 'l' | 'o' << 8 | 'c' << 16 | 'a' <<24;
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->loca = (LOCAPtr) ttfAllocLOCA(font);
	    ttfLoadLOCA(font->fp,font->loca,ptd->offset);
	}
}
static LOCAPtr ttfAllocLOCA(TTFontPtr font)
{
    USHORT n=0;
    LOCAPtr loca;

    if ((loca = (LOCAPtr) calloc(1,sizeof(LOCA))) == NULL)
	 {
	     ttfError("Out of Memory\n");
	 }
    else
	{
	    loca->indexToLocFormat = font->head->indexToLocFormat;
	    loca->numGlyphs = n = font->maxp->numGlyphs;
	}

    n += 1;/* the number of loca entry is numberOfGlyph+1 */
    if ((loca->offset = (ULONG *) calloc(n,sizeof(ULONG))) == NULL)
	{
	    ttfError("Out Of Memory\n");
	    free (loca);
	    return NULL;
	}
    
    return loca; 
}
static void ttfLoadLOCA(FILE *fp,LOCAPtr loca,ULONG offset)
{
    /* warrning: the number of loca entry is numberOfGlyph+1 !! */
    USHORT i,n = loca->numGlyphs+1;
    
    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLOADLOCA \n");
 
    switch (loca->indexToLocFormat)
	{
	case LOCA_OFFSET_SHORT:
	    for (i=0;i<n;i++)
		{
		    (loca->offset)[i] = (ULONG) ttfGetUSHORT(fp)*2;
		}
	    break;
	case LOCA_OFFSET_LONG:
	    if (fread(loca->offset,sizeof(ULONG),n,fp) != n)
		ttfError("Error reading loca data\n");
#ifndef WORDS_BIGENDIAN
	    FourByteSwap((unsigned char *) loca->offset,n*sizeof(ULONG));
#endif
	    break;
	}
}

void ttfPrintLOCA(FILE *fp,LOCAPtr loca)
{
    USHORT i;
    
    fprintf(fp,"'loca' Table - Index to Location\n");
    fprintf(fp,"--------------------------------\n");
    for (i=0;i<loca->numGlyphs;i++)
	{
	    fprintf(fp,"\t Idx %6d -> GlyphOffset 0x%08x\n",i,
		    (loca->offset)[i]);
	}
    fprintf (fp,"\t Ended at 0x%08x\n",(loca->offset)[loca->numGlyphs]);
}

void ttfFreeLOCA(LOCAPtr loca)
{
    free (loca->offset);
    free (loca);
}

ULONG ttfLookUpGlyfLOCA(LOCAPtr loca,USHORT idx)
{
    /* out of bound or it is a non-glyph character */
    if (idx >= loca->numGlyphs ||
	loca->offset[idx] == loca->offset[idx+1]) 
	return (loca->offset)[0];

    return (loca->offset)[idx];
}
