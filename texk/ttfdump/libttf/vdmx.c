/* vdmx.c -- Vertical Device Metrics
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

/* 	$Id: vdmx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: vdmx.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static VDMXPtr ttfAllocVDMX(TTFontPtr font);
static void ttfLoadVDMX(FILE *fp,VDMXPtr vdmx,ULONG offset);

void ttfInitVDMX(TTFontPtr font)
{
    ULONG tag = 'V' | 'D' << 8 | 'M' << 16 | 'X' << 24;
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->vdmx = ttfAllocVDMX(font);
	    ttfLoadVDMX(font->fp,font->vdmx,ptd->offset);
	}
}
static VDMXPtr ttfAllocVDMX(TTFontPtr font)
{
    VDMXPtr vdmx;
    
    if ((vdmx = (VDMXPtr) calloc(1,sizeof(VDMX))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return vdmx;
}
static void ttfLoadVDMX (FILE *fp,VDMXPtr vdmx,ULONG offset)
{
    int i;

    if (fseek(fp,offset,SEEK_SET) != 0)
	ttfError("Fseek Failed in ttfLoadVDMX \n");	
    
    vdmx->version = ttfGetUSHORT(fp);
    vdmx->numRecs = ttfGetUSHORT(fp);
    vdmx->numRatios = ttfGetUSHORT(fp);
    vdmx->ratRange = (Ratios *) calloc(vdmx->numRatios, sizeof(Ratios));
    vdmx->offset = (USHORT *) calloc(vdmx->numRatios, sizeof(USHORT));
    vdmx->groups = (Vdmx *) calloc(vdmx->numRecs, sizeof(Vdmx));

    for (i=0;i<vdmx->numRatios;i++)
	{
	    (vdmx->ratRange+i)->CharSet = ttfGetBYTE(fp);
	    (vdmx->ratRange+i)->xRatio = ttfGetBYTE(fp);
	    (vdmx->ratRange+i)->yStartRatio = ttfGetBYTE(fp);
	    (vdmx->ratRange+i)->yEndRatio = ttfGetBYTE(fp);
	    
	}
    if (fread(vdmx->offset, sizeof(USHORT), vdmx->numRatios,fp) != 
	vdmx->numRatios)
	ttfError("Error when getting Offset\n");
#ifndef WORDS_BIGENDIAN
    TwoByteSwap((unsigned char *) vdmx->offset,
		vdmx->numRatios*sizeof(USHORT));
#endif

    for (i=0;i<vdmx->numRecs;i++)
	{
	    int j;
	    (vdmx->groups+i)->recs = ttfGetUSHORT(fp);
	    (vdmx->groups+i)->startsz = ttfGetBYTE(fp);
	    (vdmx->groups+i)->endsz = ttfGetBYTE(fp);
    
	    (vdmx->groups+i)->entry = 
		(vTable *) calloc((vdmx->groups+i)->recs, sizeof(vTable));

	    for (j=0;j<(vdmx->groups+i)->recs;j++)
		{
		    ((vdmx->groups+i)->entry+j)->yPelHeight = ttfGetUSHORT(fp);
		    ((vdmx->groups+i)->entry+j)->yMax = ttfGetSHORT(fp);
		    ((vdmx->groups+i)->entry+j)->yMin = ttfGetSHORT(fp);
		}
	}
}

void ttfPrintVDMX(FILE *fp,VDMXPtr vdmx)
{
    int i;

    fprintf(fp,"'VDMX' Table - Precomputed Vertical Device Metrics\n");
    fprintf(fp,"--------------------------------------------------\n");


    fprintf(fp,"Version:\t %d\n", vdmx->version);
    fprintf(fp,"Number of Hgt Records:\t %d\n",vdmx->numRecs);
    fprintf(fp,"Number of Ratio Records:\t %d\n",vdmx->numRecs);

    for (i=0;i<vdmx->numRatios;i++)
	{ 
	    fprintf(fp,"\t Ratio Record #%d\n",i+1);
	    fprintf(fp,"\t\t CharSetId \t %d\n",(vdmx->ratRange+i)->CharSet);
	    fprintf(fp,"\t\t xRatio \t %d\n",(vdmx->ratRange+i)->xRatio);
	    fprintf(fp,"\t\t yStartRatio \t %d\n",(vdmx->ratRange+i)->yStartRatio);
	    fprintf(fp,"\t\t yEndRatio \t %d\n",(vdmx->ratRange+i)->yEndRatio);
	    fprintf(fp,"\t\t Record Offset %d (group #%d)\n\n",vdmx->offset[i],i);
	}
    
    fprintf(fp,"\t VDMX Height Record Groups\n");
    fprintf(fp,"\t -------------------------\n\n");

    for (i=0;i<vdmx->numRecs;i++)
	{
	    int j;
	    	    
	    fprintf(fp,"\t %d.  Number of Hgt Records %d\n",i,
		    (vdmx->groups+i)->recs);
	    fprintf(fp,"\t Starting Y Pel Height  %d\n",
		    (vdmx->groups+i)->startsz);
	    fprintf(fp,"\t Ending Y Pel Height %d\n",
		    (vdmx->groups+i)->endsz); 
    
	    for (j=0;j<(vdmx->groups+i)->recs;j++)
		{
		    fprintf(fp,"\t\t %d. Pel Height= %d\n",j+1,
			    ((vdmx->groups+i)->entry+j)->yPelHeight);
		    fprintf(fp,"\t\t yMax= \t %d\n",
			    ((vdmx->groups+i)->entry+j)->yMax);
		    fprintf(fp,"\t\t yMin= \t %d\n\n",
			    ((vdmx->groups+i)->entry+j)->yMin);		   
		}
	}

}

void ttfFreeVDMX(VDMXPtr vdmx)
{
    int i;
    if (vdmx != NULL)
	{
	    for (i=0;i<vdmx->numRecs;i++)
		{
		    free((vdmx->groups+i)->entry);
		}
	    free(vdmx->ratRange);
	    free(vdmx->offset);
	    free(vdmx->groups);
	    free(vdmx);
	}
}
