#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: maxp.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadMAXP(FILE *fp,MAXPPtr maxp,ULONG offset);

void ttfInitMAXP(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('m', 'a', 'x', 'p');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->maxp = XCALLOC1 (MAXP);
	    ttfLoadMAXP(font->fp,font->maxp,ptd->offset);
	}
}

static void ttfLoadMAXP(FILE *fp,MAXPPtr maxp,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadMAXP");

    maxp->version = ttfGetFixed(fp);
    maxp->numGlyphs = ttfGetUSHORT(fp);
    maxp->maxPoints = ttfGetUSHORT(fp);
    maxp->maxContours = ttfGetUSHORT(fp);
    maxp->maxCompositePoints = ttfGetUSHORT(fp);
    maxp->maxCompositeContours = ttfGetUSHORT(fp);
    maxp->maxZones = ttfGetUSHORT(fp);
    maxp->maxTwilightPoints = ttfGetUSHORT(fp);
    maxp->maxStorage = ttfGetUSHORT(fp);
    maxp->maxFunctionDefs = ttfGetUSHORT(fp);
    maxp->maxInstructionDefs = ttfGetUSHORT(fp);
    maxp->maxStackElements = ttfGetUSHORT(fp);
    maxp->maxSizeOfInstructions = ttfGetUSHORT(fp);
    maxp->maxComponentElements = ttfGetUSHORT(fp);
    maxp->maxComponentDepth = ttfGetUSHORT(fp);
}

void ttfPrintMAXP(FILE *fp,MAXPPtr maxp)
{
    int b[2];
    
    FixedSplit(maxp->version,b);

    fprintf(fp,"'maxp' Table - Maximum Profile\n");
    fprintf(fp,"------------------------------\n");
    fprintf(fp,"\t 'maxp' version:\t %d.%d\n",b[1],b[0]);
    fprintf(fp,"\t numGlyphs:\t\t %d\n",maxp->numGlyphs);
    fprintf(fp,"\t maxPoints:\t\t %d\n",maxp->maxPoints);
    fprintf(fp,"\t maxContours:\t\t %d\n",maxp->maxContours);
    fprintf(fp,"\t maxCompositePoints:\t %d\n",maxp->maxCompositePoints);
    fprintf(fp,"\t maxCompositeContours:\t %d\n",maxp->maxCompositeContours);
    fprintf(fp,"\t maxZones:\t\t %d\n",maxp->maxZones);
    fprintf(fp,"\t maxTwilightPoints:\t %d\n",maxp->maxTwilightPoints);
    fprintf(fp,"\t maxStorage:\t\t %d\n",maxp->maxStorage);
    fprintf(fp,"\t maxFunctionDefs:\t %d\n",maxp->maxFunctionDefs);
    fprintf(fp,"\t maxInstructionDefs:\t %d\n",maxp->maxInstructionDefs);
    fprintf(fp,"\t maxStackElements:\t %d\n",maxp->maxStackElements);
    fprintf(fp,"\t maxSizeOfInstructions:\t %d\n",maxp->maxSizeOfInstructions);
    fprintf(fp,"\t maxComponentElements:\t %d\n",maxp->maxComponentElements);
    fprintf(fp,"\t maxCompoenetDepth:\t %d\n",maxp->maxComponentDepth);
}

void ttfFreeMAXP(MAXPPtr maxp)
{
    free(maxp);
}
