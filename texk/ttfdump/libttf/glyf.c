/* glyf.c -- Load and print Glyf outline data
 * Copyright (C) 1996 Li-Da Lho, All right reserved
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: glyf.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadSimpleGlyph(FILE *fp,GLYFPtr glyf,ULONG offset);
static void ttfPrintSimpleGlyph(FILE *fp,GLYFPtr glyf);
static void ttfLoadCompositeGlyph(FILE *fp,GLYFPtr glyf,ULONG offset);
static void ttfPrintCompositeGlyph(FILE*fp,GLYFPtr glyf);
static void ttfFreeCompositeGlyph(GLYFPtr glyf);
double fix2dbl(F2Dot14 fixed);

/* Naming convention about GLYF and Glyph:
 * High level functions which are exported should have their name
 * contain GLYF, on the other hand, low level function (or structures)
 * which is not intended to be exported should have their name 
 * contain Glyph
 */
void ttfInitGLYF(TTFontPtr font)
{
    ULONG tag  = FT_MAKE_TAG ('g', 'l', 'y', 'f');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag, font)) != NULL)
	{
	    font->glyphOffset = ptd->offset;
	}
}
/* offset: where the specified glyph really starts 
 * callers should compute this themself form the loca tables */
void ttfLoadGLYF(FILE *fp, GLYFPtr glyf, ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadGLYF");

    glyf->numberOfContours = ttfGetSHORT(fp);
    glyf->xMin = ttfGetFWord(fp);
    glyf->yMin = ttfGetFWord(fp);
    glyf->xMax = ttfGetFWord(fp);
    glyf->yMax = ttfGetFWord(fp);

    offset += sizeof(USHORT) + 4*sizeof(FWord);

    if (glyf->numberOfContours >= 0)
	ttfLoadSimpleGlyph(fp, glyf, offset);
    else 
	ttfLoadCompositeGlyph(fp, glyf, offset); 
}
void ttfPrintGLYF(FILE *fp, GLYFPtr glyf)
{
    fprintf(fp,"\t numberOfContours:\t %d%s\n", glyf->numberOfContours,
	    glyf->numberOfContours == -1 ? "  (Composite)": "");
    fprintf(fp,"\t xMin:\t\t\t %d\n", glyf->xMin);
    fprintf(fp,"\t yMin:\t\t\t %d\n", glyf->yMin);
    fprintf(fp,"\t xMax:\t\t\t %d\n", glyf->xMax);
    fprintf(fp,"\t yMax:\t\t\t %d\n\n", glyf->yMax);
    
    if (glyf->numberOfContours >= 0)
	ttfPrintSimpleGlyph(fp, glyf);
    else
	ttfPrintCompositeGlyph(fp, glyf);
}
void ttfFreeGLYF(GLYFPtr glyf)
{
    if (glyf->numberOfContours < 0)
	ttfFreeCompositeGlyph(glyf);
}
static void ttfLoadSimpleGlyph(FILE *fp, GLYFPtr glyf, ULONG offset)
{
    SHORT nCnts = glyf->numberOfContours;
    USHORT i,nIns,nPts;

    xfseek(fp, offset, SEEK_SET, "ttfLoadSimpleGlyph");
    
    if (nCnts != 0)
	{
	    ttfReadUSHORT (glyf->endPtsOfContours, nCnts, fp);
	    nPts = (glyf->endPtsOfContours)[nCnts-1]+1;
	}
    else
	/* there seems to be something wrong with my interpretation of 
	 * NOGLYPH chars, doing this way in case there is a char that has 
	 * zero contour */
	nPts = 0;

    /* how to deal with zero instruction glyf properly ?? */
    glyf->instructionLength = nIns = ttfGetUSHORT(fp);
    if (nIns != 0)
	{
	    if (fread(glyf->instructions, sizeof(BYTE), nIns, fp) != nIns)
		ttfError("Error when getting instructions\n");
	}

    for (i=0;i<nPts;i++)
	{
	    BYTE j,c;
	    if (((glyf->flags)[i] = c =  ttfGetBYTE(fp)) & FLAGS_REPEAT)
		{ /* if this flag should be repeated */
		    j = ttfGetBYTE(fp); /* j times */
		    while (j--) 
			{
			    i++;
			    (glyf->flags)[i] = c;
			    
			}
		}
	}

    for (i=0;i<nPts;i++)
	{
	    BYTE flag;
	    flag = (glyf->flags)[i];
	    
	    if (flag & FLAGS_X_SHORT_VECTOR)
		{ /* if the coordinate is a BYTE */
		    if (flag & FLAGS_X_SAME)
			(glyf->xCoordinates)[i] = (SHORT) ttfGetBYTE(fp);
		    else
			(glyf->xCoordinates)[i] = (SHORT) -ttfGetBYTE(fp);
		}
	    else
		{ /* the coordiante is a SHORT */
		    if (flag & FLAGS_X_SAME)
			(glyf->xCoordinates)[i] = 0;
		    else
			(glyf->xCoordinates)[i] = ttfGetSHORT(fp);
		}
	}
    for (i=0;i<nPts;i++)
	{
	    BYTE flag;
	    flag = (glyf->flags)[i];
	    
	    if (flag & FLAGS_Y_SHORT_VECTOR)
		{ /* if the coordinate is a BYTE */
		    if (flag & FLAGS_Y_SAME)
			(glyf->yCoordinates)[i] = (SHORT) ttfGetBYTE(fp);
		    else
			(glyf->yCoordinates)[i] = (SHORT) -ttfGetBYTE(fp);
		}
	    else
		{ /* the coordiante is a SHORT */
		    if (flag & FLAGS_Y_SAME)
			(glyf->yCoordinates)[i] = 0;
		    else
			(glyf->yCoordinates)[i] = ttfGetSHORT(fp);
		}
	}
}
static void ttfPrintSimpleGlyph(FILE *fp, GLYFPtr glyf)
{
    USHORT i, nPts, nCnts;
    SHORT x=0, y=0;

    nPts = (glyf->endPtsOfContours)[glyf->numberOfContours-1]+1;
    nCnts = glyf->numberOfContours;

    fprintf(fp,"\t EndPoints\n");
    fprintf(fp,"\t ---------\n");
    for (i=0;i<nCnts;i++)
	fprintf(fp,"\t  %d: %2d\n",i,(glyf->endPtsOfContours)[i]);
    fprintf(fp,"\n");

    fprintf(fp,"\t Length of Instructions: %2d\n\n",glyf->instructionLength);
    ttfPrintInstructions(fp,glyf->instructions);
    
    fprintf(fp,"\t Flags\n");
    fprintf(fp,"\t -----\n");
    for (i=0;i<nPts;i++)
	{
	    BYTE flag;
	    char buf[80];

	    flag =  (glyf->flags)[i];
	    
	    if (flag & FLAGS_Y_SAME)
		sprintf(buf,"YDual  ");
	    else
		sprintf(buf,"       ");
	    if (flag & FLAGS_X_SAME)
		strcat(buf,"XDual   ");
	    else
		strcat(buf,"        ");
	    if (flag & FLAGS_REPEAT)
		strcat(buf,"Repeat  ");
	    else 
		strcat(buf,"        ");
	    if (flag & FLAGS_Y_SHORT_VECTOR)
		strcat(buf,"Y-Short ");
	    else
		strcat(buf,"        ");
	    if (flag & FLAGS_X_SHORT_VECTOR)
		strcat(buf,"X-Short ");
	    else
		strcat(buf,"        ");
	    if (flag & FLAGS_ON_CURVE)
		strcat(buf,"On\n");
	    else
		strcat(buf,"Off\n");
	    		    
	    fprintf(fp,"\t %2d: %s",i,buf);
	}
    fprintf(fp,"\n");
    fprintf(fp,"\t Coordinates\n");
    fprintf(fp,"\t -----------\n");
    for (i=0;i<nPts;i++)
	{
	    x += (glyf->xCoordinates)[i];
	    y += (glyf->yCoordinates)[i];
	    fprintf(fp,"\t %2d Rel ( %6d, %6d) -> Abs ( %6d, %6d)\n", i,
		    (glyf->xCoordinates)[i], (glyf->yCoordinates)[i], x, y);
	}
    fprintf(fp,"\n");
}

static void ttfLoadCompositeGlyph(FILE *fp, GLYFPtr glyf, ULONG offset)
{
    USHORT nIns,flags;   
    Component *cur;

    xfseek(fp, offset, SEEK_SET, "ttfLoadCompositeGlyph");
    
    glyf->comp = cur = XCALLOC1 (Component);
    cur->previous = NULL; /* beginning of a linked list */

    do {
   	cur->flags = flags = ttfGetUSHORT(fp);
	cur->glyphIndex = ttfGetUSHORT(fp);
	if (flags & ARG_1_AND_2_ARE_WORDS)
	    {
		(cur->data).args[0] = ttfGetSHORT(fp);
		(cur->data).args[1] = ttfGetSHORT(fp);
	    }
	else
	    (cur->data).args[0] = ttfGetUSHORT(fp);

	if (flags & WE_HAVE_A_SCALE)
	    {
		(cur->data).transform.scale = ttfGetF2Dot14(fp);
	    }
	else if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
	    {
		(cur->data).transform.vector.xscale = ttfGetF2Dot14(fp);
		(cur->data).transform.vector.yscale = ttfGetF2Dot14(fp);
	    }
	else if (flags & WE_HAVE_A_TWO_BY_TWO)
	    {
		(cur->data).transform.tensor.xscale  = ttfGetF2Dot14(fp);
	        (cur->data).transform.tensor.scale01 = ttfGetF2Dot14(fp);
		(cur->data).transform.tensor.scale10 = ttfGetF2Dot14(fp);
		(cur->data).transform.tensor.yscale  = ttfGetF2Dot14(fp);
	    }	    
	/* allocate next component */
	cur->next = XCALLOC1 (Component);
	cur->next->previous = cur;
	cur = cur->next; /* move to next component */
    } while (flags & MORE_COMPONENT);
    cur->next = NULL;  /* end of the linked list */

    if (flags & WE_HAVE_INSTRUCTIONS)
	{
	    glyf->instructionLength = nIns = ttfGetUSHORT(fp);
	    if (fread(glyf->instructions, sizeof(BYTE), nIns, fp) != nIns)
		ttfError("Error when loading instructions\n");
	}
    else
	{
	    glyf->instructionLength = 0;
	}
}
static void ttfPrintCompositeGlyph(FILE *fp, GLYFPtr glyf)
{
    int i = 0;
    char buf[80];
    USHORT flags;
    Component *cur;

    cur = glyf->comp;
    
    do {
	flags = cur->flags;
	fprintf(fp, "\t %d: Flags:\t 0x%x\n", i, flags);
	fprintf(fp, "\t    Glyf Index:\t %d\n", cur->glyphIndex);
	if (flags & ARGS_ARE_XY_VALUES)
	    {
		if (flags & ARG_1_AND_2_ARE_WORDS)
		    {
			fprintf(fp, "\t    X WOffset:\t %d\n", (cur->data).args[0]);
			fprintf(fp, "\t    Y WOffset:\t %d\n", (cur->data).args[1]);
		    }
		else
		    {
			fprintf(fp, "\t    X BOffset:\t %d\n", 
				(signed char) ((cur->data).args[0] >> 8 & 0xff));
			fprintf(fp, "\t    Y BOffset:\t %d\n",
				(signed char) ((cur->data).args[0] & 0xff));
		    }
	    }
	else
	    {
		/* what the hell are the "patch points" ?? */ 
	    }

	if (flags & WE_HAVE_A_SCALE)
	    {
		fprintf(fp, "\t    X,Y Scale:\t %f\n",
			fix2dbl((cur->data).transform.scale));
	    }
	else if (flags & WE_HAVE_AN_X_AND_Y_SCALE)
	    {
		fprintf(fp, "\t    X Scale:\t %f\n", 
			fix2dbl((cur->data).transform.vector.xscale)); 
		fprintf(fp, "\t    Y Scale:\t %f\n", 
			fix2dbl((cur->data).transform.vector.yscale)); 
	    }
	else if (flags & WE_HAVE_A_TWO_BY_TWO)
	    {
		fprintf(fp, "\t    X Scale:\t %f\n", 
			fix2dbl((cur->data).transform.tensor.xscale)); 
		fprintf(fp, "\t    X,Y Scale:\t %f\n", 
			fix2dbl((cur->data).transform.tensor.scale01)); 
		fprintf(fp, "\t    Y,X Scale:\t %f\n", 
			fix2dbl((cur->data).transform.tensor.scale10)); 
		fprintf(fp, "\t    Y Scale:\t %f\n", 
			fix2dbl((cur->data).transform.tensor.yscale)); 
	    }	    

	if (flags & ROUND_XY_TO_GRID)
	    sprintf(buf, "Round X,Y to Grid   ");
	else
	    sprintf(buf, "                    ");

	if (flags & NO_OVERLAP)
	    strcat(buf, "NO Overlap   ");
	else
	    strcat(buf, "             ");

	if (flags & USE_MY_METRICS)
	    strcat(buf, "Use My Metrics   ");
	else
	    strcat(buf, "                 ");

	fprintf(fp, "\t    Others:\t %s\n\n", buf);

	i++;
	cur = cur->next;
    } while (cur->next != NULL);
    
    fprintf(fp, "\n");

    if (flags & WE_HAVE_INSTRUCTIONS)
	{
	    fprintf(fp,"\t Length of Instructions: %2d\n\n",
		    glyf->instructionLength);
	    ttfPrintInstructions(fp, glyf->instructions);
	}
}
static void ttfFreeCompositeGlyph(GLYFPtr glyf)
{
    Component *cur,*next;
    
    cur = glyf->comp;

    do {
	next = cur->next;
	free(cur);
	cur = next;
    } while (cur != NULL);
}

/* what I want:
 * outter procedures should not have any ideas about where the glyph starts,
 *
 * it just provide the TTFont structure and the character code or the index
 * of that glyph. Procedures below should do:
 * 1. look up where the glyph data is.
 * 2. provided some glyph cache mechanism for the reason that 
 *    a. it is not necessary to load all glyph into memory, especially for 
 *    eastern languages.
 *    b. if a glyph data has been loaded previously, it is not necessary to 
 *    load it again.
 *    c. malloc and free are SLOW !!
 */
/* Load a glyph by glyph index */
GLYFPtr ttfLoadGlyphIndex(TTFont *font, USHORT idx)
{
    ULONG pos;
    GLYFPtr glyf;
    
    /* compute the actual place where the glyph stored */
    pos = font->glyphOffset + ttfLookUpGlyfLOCA(font->loca, idx);
    glyf = ttfLoadGlyphCached(font, pos);

    return glyf;
}
/* Load a glyph by character code in the current encoding scheme */
GLYFPtr ttfLoadGlyphCode(TTFont *font,USHORT cc)
{
    USHORT index;
    
    index = ttfLookUpCMAP(font->encoding->map,cc);
    return ttfLoadGlyphIndex(font,index);
}

double fix2dbl(F2Dot14 fixed)
{
    double mantissa, fraction;

    mantissa = (double) (fixed >> 14);

    fraction = (double) (double)(fixed & 0x3fff) / 16384.0;

    return mantissa+fraction;
}
