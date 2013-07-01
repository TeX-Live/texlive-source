/* loadtable.c
 * Load the tables of the Table Directory of a True Type font file
 * Copyright (C) 1996 Li-Da Lho, All right reserved
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: tabledir.c,v 1.2 1998/07/04 13:17:51 werner Exp $	 */

static void ttfLoadTableDir(FILE *fp,TableDirPtr p,ULONG offset);

#define Offset 12 /* start point of table dir */

void ttfInitTableDir(TTFontPtr font, ULONG offset)
{
    int i,pos; /* table directory starts form position 12 */
    
    font->dir = XCALLOC (font->numTables, TableDir);
    pos = Offset + offset;
    for (i=0;i<font->numTables;i++)
	{
	    ttfLoadTableDir(font->fp,font->dir+i,pos);
	    pos += sizeof(TableDir);
	}
}

static void ttfLoadTableDir(FILE *fp,TableDirPtr p,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadTableDir");

    p -> tag = ttfGetULONG(fp);
    p -> checksum = ttfGetULONG(fp);
    p -> offset = ttfGetULONG(fp);
    p -> length = ttfGetULONG(fp);
}

void ttfPrintTableDir(FILE *fp,TableDirPtr p)
{
    fprintf(fp,"'%s' - checksum = 0x%08x, offset = 0x%08x, len = %9u\n",
	    TagToStr(p->tag),p->checksum,p->offset,p->length);
}

void ttfFreeTableDir(TableDirPtr p)
{
    free(p);
}

/* ttfLookUpTableDir
 * lookup the specified table in an array of TableDir
 * Linear search at present, should change to binary search in the
 * future to improve effcience
 * The tag name are sorted in ascent order in ttf file.
 */
TableDirPtr ttfLookUpTableDir(ULONG tagname,TTFontPtr font)
{
    USHORT i,n = font->numTables;
    TableDirPtr ptable = font->dir;
    
    for (i=0;i<n;i++,ptable++)
	{
	    if (ptable->tag == tagname)
		return ptable;
	}
    return NULL;
}

#if 0
/* Not used */
/* calculate table check sum */
/* can't be done until the data abstraction have been finished */
/* not finished yet */    
ULONG ttfCalcTableCheckSum(ULONG tagname,TTFontPtr font)
{    
    ULONG sum = 0L,Length;
    TableDirPtr ptable;

    Length = ((ptable->length+3) & ~3) / sizeof(ULONG);

    ptable = ttfLookUpTableDir(tagname, font);    
    xfseek(font->fp, ptable->offset, SEEK_SET, "ttfCalcTableCheckSum");

    while (Length--)
	{
	    sum += ttfGetULONG(font->fp);
	}

    return sum;
}
#endif
