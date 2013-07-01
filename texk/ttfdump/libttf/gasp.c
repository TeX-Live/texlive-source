/* gasp.c -- Grid-fitting And Scan-conversion Procedure
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: gasp.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadGASP(FILE *fp,GASPPtr gasp,ULONG offset);

void ttfInitGASP(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('g', 'a', 's', 'p');
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->gasp = XCALLOC1 (GASP);
	    ttfLoadGASP(font->fp,font->gasp,ptd->offset);
	}
}

static void ttfLoadGASP (FILE *fp,GASPPtr gasp,ULONG offset)
{
    int i;

    xfseek(fp, offset, SEEK_SET, "ttfLoadGASP");
    
    gasp->version = ttfGetUSHORT(fp);
    gasp->numRanges = ttfGetUSHORT(fp);
    
    gasp->gaspRange = XCALLOC (gasp->numRanges, GASPRANGE);
    
    for (i=0;i<gasp->numRanges;i++)
	{
	    gasp->gaspRange[i].rangeMaxPPEM = ttfGetUSHORT(fp);
	    gasp->gaspRange[i].rangeGaspBehavior = ttfGetUSHORT(fp);
	}
}

void ttfPrintGASP(FILE *fp,GASPPtr gasp)
{
    int i;

    fprintf(fp,"'gasp' Table - Grid-fitting And Scan-conversion Procedure\n"); 
    fprintf(fp,"---------------------------------------------------------\n");

    fprintf(fp,"'gasp' version:\t %d\n",gasp->version);
    fprintf(fp,"numRanges: \t %d\n\n",gasp->numRanges);

    for (i=0;i<gasp->numRanges;i++)
	{
	    fprintf(fp,"\t gasp Range %d\n",i);
	    fprintf(fp,"\t rangeMaxPPEM:\t %d\n",
		    gasp->gaspRange[i].rangeMaxPPEM);
	    fprintf(fp,"\t rangeGaspBehavior:\t 0x%04x\n\n",
		    gasp->gaspRange[i].rangeGaspBehavior);
	}
    fprintf(fp,"\n");
}

void ttfFreeGASP(GASPPtr gasp)
{
    if (gasp != NULL)
	{
	    free(gasp->gaspRange);
	    free(gasp);
	}
}
