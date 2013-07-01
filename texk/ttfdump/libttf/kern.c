/* kern.c -- Kerning Table
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: kern.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadKERN(FILE *fp,KERNPtr kern,ULONG offset);

void ttfInitKERN(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('k', 'e', 'r', 'n');
    TableDirPtr ptd;
     
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->kern = XCALLOC1 (KERN);
	    ttfLoadKERN(font->fp,font->kern,ptd->offset);
	}
}

static void ttfLoadKERN (FILE *fp,KERNPtr kern,ULONG offset)
{
    int i;

    xfseek(fp, offset, SEEK_SET, "ttfLoadKERN");
    
    kern->version = ttfGetUSHORT(fp);
    kern->nTables = ttfGetUSHORT(fp);

    kern->subtable = XCALLOC (kern->nTables, KernSubtable);

    for (i=0;i<kern->nTables;i++)
	{
	    struct kernpair *pairs;
	    int j,n;
	    (kern->subtable+i)->version = ttfGetUSHORT(fp);
	    (kern->subtable+i)->length = ttfGetUSHORT(fp);
	    (kern->subtable+i)->coverage = ttfGetUSHORT(fp);

	    switch ((kern->subtable+i)->coverage >> 8)
		{
		case 0:
		    (kern->subtable+i)->kern.kern0.nPairs = n = 
			ttfGetUSHORT(fp);
		    (kern->subtable+i)->kern.kern0.searchRange =
			ttfGetUSHORT(fp);
		    (kern->subtable+i)->kern.kern0.entrySelector =
			ttfGetUSHORT(fp);
		    (kern->subtable+i)->kern.kern0.rangeShift =
			ttfGetUSHORT(fp);
		    (kern->subtable+i)->kern.kern0.pairs = pairs =
			XCALLOC (n, struct kernpair);

		    for (j=0;j<n;j++)
			{
			    (pairs+j)->left = ttfGetUSHORT(fp);
			    (pairs+j)->right = ttfGetUSHORT(fp);
			    (pairs+j)->value = ttfGetFWord(fp);
			}
		    break;
		case 2:
		    /* not implemented yet */
		    break;
		default:
		    /* do nothing */
		    break;
		}
	}
}

void ttfPrintKERN(FILE *fp,KERNPtr kern)
{
    int i;
    
    fprintf(fp,"'kern' Table - Kerning Data\n");
    fprintf(fp,"---------------------------\n");
    fprintf(fp,"'kern' Version:\t %d\n", kern->version);
    fprintf(fp,"Number of subtables:\t %d\n\n",kern->nTables);

    for (i=0;i<kern->nTables;i++)
	{
	    struct kernpair *pairs;
	    int j,n;

	    fprintf(fp,"\t Subtable format \t %d\n",
		    (kern->subtable+i)->coverage >> 8);
	    fprintf(fp,"\t Subtable version \t %d\n",
		    (kern->subtable+i)->version);
	    fprintf(fp,"\t Bytes in subtable \t %d\n",
		    (kern->subtable+i)->length);
	    fprintf(fp,"\t Coverage bits \t 0x%x\n",
		    (kern->subtable+i)->coverage);
	    switch ((kern->subtable+i)->coverage >> 8)
		{
		case 0:
		    n = (kern->subtable+i)->kern.kern0.nPairs;
		    pairs = (kern->subtable+i)->kern.kern0.pairs;

		    fprintf(fp,"\t Number of pairs %d\n",n);
		    fprintf(fp,"\t Search Range    %d\n",
			    (kern->subtable+i)->kern.kern0.searchRange);
		    fprintf(fp,"\t Entry Selector  %d\n",
			    (kern->subtable+i)->kern.kern0.entrySelector);
		    fprintf(fp,"\t Range Shift     %d\n",
			    (kern->subtable+i)->kern.kern0.rangeShift);

		    fprintf(fp,"\t Left Glyph \t Right Glyph \t Kern Move\n");
		    fprintf(fp,"\t ---------- \t ----------- \t ---------\n");

		    for (j=0;j<n;j++)
			{
			    fprintf(fp,"\t\t %d \t\t %d \t\t %d\n",
				    (pairs+j)->left,(pairs+j)->right,
				    (pairs+j)->value);
			}
		    break;
		case 2:
		    /* not implemented yet */
		    break;
		default:
		    /* do nothing */
		    break;
		}
	    fprintf(fp,"\n");
	}

}

void ttfFreeKERN(KERNPtr kern)
{
    int i;
    
    if (kern != NULL)
	{
	    for (i=0;i<kern->nTables;i++)
		{
		    switch ((kern->subtable+i)->coverage >> 8)
			{
			case 0:
			    free((kern->subtable+i)->kern.kern0.pairs);
			     break;
			case 2:
			    /* not implemented yet */
			    break;
			default:
			    /* do nothing */
			    break;
			}
		}
	    free(kern->subtable);
	    free(kern);
	}
}
