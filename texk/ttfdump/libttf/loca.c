#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: loca.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static LOCAPtr ttfAllocLOCA(TTFontPtr font);
static void ttfLoadLOCA(FILE *fp,LOCAPtr loca,ULONG offset);

void ttfInitLOCA(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('l', 'o', 'c', 'a');
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

    loca = XCALLOC1 (LOCA);
    loca->indexToLocFormat = font->head->indexToLocFormat;
    loca->numGlyphs = n = font->maxp->numGlyphs;

    n += 1;/* the number of loca entry is numberOfGlyph+1 */
    loca->offset = XCALLOC (n, ULONG);
    
    return loca; 
}
static void ttfLoadLOCA(FILE *fp,LOCAPtr loca,ULONG offset)
{
    /* warning: the number of loca entry is numberOfGlyph+1 !! */
    USHORT i,n = loca->numGlyphs+1;
    
    xfseek(fp, offset, SEEK_SET, "ttfLoadLOCA");
 
    switch (loca->indexToLocFormat)
	{
	case LOCA_OFFSET_SHORT:
	    for (i=0;i<n;i++)
		{
		    (loca->offset)[i] = (ULONG) ttfGetUSHORT(fp)*2;
		}
	    break;
	case LOCA_OFFSET_LONG:
	    ttfReadULONG (loca->offset, n, fp);
	    break;
	}
}

void ttfPrintLOCA(FILE *fp,LOCAPtr loca)
{
    USHORT i;
    
    if (loca)
        {
            fprintf(fp,"'loca' Table - Index to Location\n");
            fprintf(fp,"--------------------------------\n");
            for (i=0;i<loca->numGlyphs;i++)
        	{
        	    fprintf(fp,"\t Idx %6d -> GlyphOffset 0x%08x\n",i,
        		    (loca->offset)[i]);
        	}
            fprintf (fp,"\t Ended at 0x%08x\n",(loca->offset)[loca->numGlyphs]);
        }
}

void ttfFreeLOCA(LOCAPtr loca)
{
    if (loca)
        {
            free (loca->offset);
            free (loca);
        }
}

ULONG ttfLookUpGlyfLOCA(LOCAPtr loca,USHORT idx)
{
    /* out of bound or it is a non-glyph character */
    if (idx >= loca->numGlyphs ||
	loca->offset[idx] == loca->offset[idx+1]) 
	return (loca->offset)[0];

    return (loca->offset)[idx];
}
