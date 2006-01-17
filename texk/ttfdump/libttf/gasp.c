/* gasp.c -- Grid-fitting And Scan-conversion Procedure
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

/* 	$Id: gasp.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

#ifndef lint
static char vcid[] = "$Id: gasp.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $";
#endif /* lint */

static GASPPtr ttfAllocGASP(TTFontPtr font);
static void ttfLoadGASP(FILE *fp,GASPPtr gasp,ULONG offset);

void ttfInitGASP(TTFontPtr font)
{
    ULONG tag = 'g' | 'a' << 8 | 's' << 16 | 'p' << 24;
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->gasp = ttfAllocGASP(font);
	    ttfLoadGASP(font->fp,font->gasp,ptd->offset);
	}
}
static GASPPtr ttfAllocGASP(TTFontPtr font)
{
    GASPPtr gasp;
    
    if ((gasp = (GASPPtr) calloc(1,sizeof(GASP))) == NULL)
	{
	    ttfError("Out of Memory in __FILE__:__LINE__\n");
	    return NULL;
	}
    return gasp;
}
static void ttfLoadGASP (FILE *fp,GASPPtr gasp,ULONG offset)
{
    int i;

    if (fseek(fp,offset,SEEK_SET) !=0)
	ttfError("Fseek Failed in ttfLoadGASP \n");	
    
    gasp->version = ttfGetUSHORT(fp);
    gasp->numRanges = ttfGetUSHORT(fp);
    
    gasp->gaspRange = (GASPRANGE *) calloc(gasp->numRanges, sizeof(GASPRANGE));
    
    if (gasp->gaspRange == NULL)
	ttfError("Out of Memory in __FILE__:__LINE__\n");
    else
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
