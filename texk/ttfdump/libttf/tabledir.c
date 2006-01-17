/* loadtable.c
 * Load the tables of the Table Directory of a True Type font file
 * Copyright (C) 1996 Li-Da Lho, All right reserved
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

#ifdef MEMCHECK
#include <dmalloc.h>
#endif

/* 	$Id: tabledir.c,v 1.2 1998/07/04 13:17:51 werner Exp $	 */

#ifndef lint
static char vcid[] = "$Id: tabledir.c,v 1.2 1998/07/04 13:17:51 werner Exp $";
#endif /* lint */

static TableDirPtr ttfAllocTableDir(TTFontPtr font);
static void ttfLoadTableDir(FILE *fp,TableDirPtr p,ULONG offset);

#define Offset 12 /* start point of table dir */

void ttfInitTableDir(TTFontPtr font, ULONG offset)
{
    int i,pos; /* table directory starts form position 12 */
    
    font->dir = ttfAllocTableDir(font);
    pos = Offset + offset;
    for (i=0;i<font->numTables;i++)
	{
	    ttfLoadTableDir(font->fp,font->dir+i,pos);
	    pos += sizeof(TableDir);
	}
}

static TableDirPtr ttfAllocTableDir(TTFontPtr font)
{
    TableDirPtr ptable;

    if ((ptable = (TableDirPtr) calloc(font->numTables,sizeof(TableDir))) == NULL)
	{
	    ttfError("Out Of memory\n in __FILE__:__LINE__");
	    return NULL;
	}
    return ptable;
}

static void ttfLoadTableDir(FILE *fp,TableDirPtr p,ULONG offset)
{
    if (fseek(fp,offset,SEEK_SET) != 0)
	ttfError("Fseek Failed\n");

    p -> tag = (ULONG) ttfGetLSB32(fp);
    p -> checksum = ttfGetULONG(fp);
    p -> offset = ttfGetULONG(fp);
    p -> length = ttfGetULONG(fp);
}

void ttfPrintTableDir(FILE *fp,TableDirPtr p)
{
    char tagname[5];

    /* make ULONG tag names more like a string */
    bcopy(&(p->tag),tagname,4);
    tagname[4] = '\0';
#ifdef WORDS_BIGENDIAN
    FourByteSwap((unsigned char *)tagname,4);
#endif

    fprintf(fp,"'%s' - chechsum = 0x%08x, offset = 0x%08x, len = %9u \n",
	    tagname,p->checksum,p->offset,p->length);
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

/* calculate table check sum */
/* can't be done until the data abstraction have been finished */
/* not finished yet */    
ULONG ttfCalcTableCheckSum(ULONG tagname,TTFontPtr font)
{    
    ULONG sum = 0L,Length;
    TableDirPtr ptable;

    Length = ((ptable->length+3) & ~3) / sizeof(ULONG);

    ptable = ttfLookUpTableDir(tagname, font);    
    fseek(font->fp,ptable->offset,SEEK_SET);

    while (Length--)
	{
	    sum += ttfGetULONG(font->fp);
	}

    return sum;
}
