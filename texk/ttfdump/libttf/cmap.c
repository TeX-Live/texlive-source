/* cmap.c -- Load and Print the content of cmap table
 * Copyright (C) 1996 Li-Da Lho, All right reserved 
 * 
 * The structure of a cmap is really complex, it depends on which format the 
 * cmap is and loading a cmap is a kind of confusing thing. 
 * The structure of CMAP:
 *   A cmap begins with a general description of the table
 *     USHORT version indicates the version of this cmap.
 *     USHORT numEncodings is the number of encodings in the cmap table.
 *   Then follow 'numEncodings' of encoding tables, each of which contains
 *   the following informations for each subtable. The subtable contains the 
 *   true data to map character code to glyph index
 *     USHORT PlatformID shows which platform the subtable is to be used 
 *            (e.g. Mac, PC)
 *     USHORT Platfrom specific EncodingID shows what kind of encoding scheme
 *            this table uses (e.g. Big5, iso8859, Unicode etc.)
 *     ULONG  offset is the offset from the beginning of the whole cmap table
 *            to the beginning of the corresponding subtable
 * -----------------------         ---         ---
 * | USHORT version      |          ^           ^
 * -----------------------          |           |
 * | USHORT numEncodings |          |           |
 * -----------------------          |           |
 * | Encoding Table 1    |          |           |
 * |  USHORT PlatformID  |          |           |
 * |  USHORT EncodingID  |          |           |
 * |  ULONG  offset 1    |------    |           |
 * -----------------------     |    |  offset 1 | offset 2
 * | Encoding Table 2    |     |    |           |
 * |  USHORT PlatformID  |     |    |           |
 * |  USHORT EncodingID  |     |    |           |
 * |  ULONG  offset 2    |---  |    v           |
 * -----------------------  |  |   ---          |
 * | Subtable 1          |<-----                |
 * | with format 0       |  |                   v
 * -----------------------  |                  ---
 * | Subtable 2          |<--                
 * | with format 2       |
 * ----------------------|
 *
 * Problem with Character Code vs. Byte Endianess.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: cmap.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

typedef union _TwoBytes {
    USHORT u;
    unsigned char c[2];
} TwoBytes;

static void ttfLoadCMAP(FILE *fp,CMAPPtr cmap,ULONG offset);

static void ttfLoadEncodingTable(FILE *fp,SubTablePtr subTable,ULONG offset);
static void ttfLoadSubTable(FILE *fp,SubTablePtr subTable,ULONG offset);
static void ttfPrintSubTable(FILE* fp,SubTablePtr ptable);
static void ttfFreeSubTable(SubTablePtr ptable);

static void ttfLoadCMAP0(FILE *fp,SubTablePtr subTable,ULONG offset);
static void ttfPrintCMAP0(FILE *fp,SubTablePtr subTable);
static USHORT ttfLookUpCMAP0(SubTablePtr subTable,USHORT cc);
static void ttfFreeCMAP0(SubTablePtr subTable);

static void ttfLoadCMAP2(FILE *fp,SubTablePtr subTable,ULONG offset);
static void ttfPrintCMAP2(FILE *fp,SubTablePtr subTable);
static USHORT ttfLookUpCMAP2(SubTablePtr subTable,USHORT cc);
static void ttfFreeCMAP2(SubTablePtr subTable);

static void ttfLoadCMAP4(FILE *fp,SubTablePtr subTable,ULONG offset);
static void ttfPrintCMAP4(FILE *fp,SubTablePtr subTable);
static USHORT ttfLookUpCMAP4(SubTablePtr subTable,USHORT cc);
static void ttfFreeCMAP4(SubTablePtr subTable);

static void ttfLoadCMAP6(FILE *fp,SubTablePtr subTable,ULONG offset);
static void ttfPrintCMAP6(FILE *fp, SubTablePtr sbuTable);
static USHORT ttfLookUpCMAP6(SubTablePtr subTable,USHORT cc);
static void ttfFreeCMAP6(SubTablePtr subTable);

void ttfInitCMAP(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('c', 'm', 'a', 'p');
    TableDirPtr ptd;

    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->cmap = XCALLOC1 (CMAP);
	    ttfLoadCMAP(font->fp,font->cmap,ptd->offset);
	}
}

static void ttfLoadCMAP(FILE *fp,CMAPPtr cmap,ULONG offset)
{
    USHORT i,n;
    ULONG posEnc;            /* beginning of the Encoding Table */
    ULONG baseSub = offset;  /* base of SubTable offset */

    xfseek(fp, offset, SEEK_SET, "ttfLoadCMAP");

    cmap->version = ttfGetUSHORT(fp);
    cmap->numberOfEncodings = n = ttfGetUSHORT(fp);
    cmap->subTables = XCALLOC (cmap->numberOfEncodings, SubTable);

    posEnc = baseSub + sizeof(USHORT)*2; /* step over the beginning of encoding
					  * table */
    
    /* for each encoding scheme, load the encoding table (EncodingTable) and 
     * the real cmap data (SubTable) */
    for (i=0;i<n;i++,posEnc += 8)
	{   /* 8 == ushort*2 + ulong*1 */
	    ttfLoadEncodingTable(fp,cmap->subTables+i,posEnc);
	    ttfLoadSubTable(fp,cmap->subTables+i,baseSub);
	}
}
void ttfPrintCMAP(FILE *fp,CMAPPtr cmap)
{
    USHORT i;

    fprintf(fp,"'cmap' Table - Character to Glyph Index Mapping Table\n");
    fprintf(fp,"-----------------------------------------------------\n");
    fprintf(fp,"\t 'cmap' version: %d\n",cmap->version);
    fprintf(fp,"\t number of encoding: %d\n\n",cmap->numberOfEncodings);

    for (i=0;i<cmap->numberOfEncodings;i++)
	{
	    fprintf(fp,"Subtable %3d.\t",i);
	    ttfPrintSubTable(fp,cmap->subTables+i);
	    fprintf(fp,"\n");
	}
}
USHORT ttfLookUpCMAP(SubTablePtr subTable,USHORT cc)
{
    USHORT idx,format = subTable->format;

    switch (format)
	{
	case 0:
	    idx = ttfLookUpCMAP0(subTable,cc);
	    break;
	case 2:
	    idx = ttfLookUpCMAP2(subTable,cc);
	    break;
	case 4:
	    idx = ttfLookUpCMAP4(subTable,cc);
	    break;
	case 6:
	    idx = ttfLookUpCMAP6(subTable,cc);
	    break;
	default:
	    ttfError("Unrecognized CMAP format\n");
	    return 0;
	}
    return idx;
}
SubTablePtr ttfSelectCMAP(CMAPPtr cmap, USHORT PlatformID, USHORT EncodingID)
{
    USHORT i;
    
    for (i=0;i<cmap->numberOfEncodings;i++)
	{
	    if ((cmap->subTables+i)->PlatformID == PlatformID &&
		(cmap->subTables+i)->EncodingID == EncodingID)
		return cmap->subTables+i;
	}
    return NULL;
}
void ttfFreeCMAP(CMAPPtr cmap)
{
    USHORT i;
    
    for (i=0;i<cmap->numberOfEncodings;i++)
	ttfFreeSubTable(cmap->subTables+i);

    free(cmap->subTables);
    free(cmap);
}

/* actually, EncodingTable is a part of SubTable */
static void ttfLoadEncodingTable(FILE *fp,SubTablePtr subTable,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadEncodingTable");
    
    subTable->PlatformID = ttfGetUSHORT(fp);
    subTable->EncodingID = ttfGetUSHORT(fp);
    subTable->offset = ttfGetULONG(fp);
}

/* should this one be static ? */
static void ttfLoadSubTable(FILE *fp,SubTablePtr subTable,ULONG base)
{
    ULONG pos;
    USHORT format;

    /* seek to the actuall position for this subtable
     * base: beginning of cmap
     * offset: offset field of each encoding table */
    pos =  base  +  subTable->offset;
    xfseek(fp, pos, SEEK_SET, "ttfLoadSubTable");

    subTable->format = format = ttfGetUSHORT(fp); 
    subTable->length = ttfGetUSHORT(fp);
    subTable->version = ttfGetUSHORT(fp);
   
    pos += 6;/* step over format independent data,USHORT*3 */

    switch(format)
	{
	case 0:
	    ttfLoadCMAP0(fp,subTable,pos);
	    break;
	case 2:
	    ttfLoadCMAP2(fp,subTable,pos);
	    break;
	case 4:
	    ttfLoadCMAP4(fp,subTable,pos);
	    break;
	case 6:
	    ttfLoadCMAP6(fp,subTable,pos);
	    break;
	default:
	    ttfError("Unrecognized CMAP format\n");
	}
}
static void ttfPrintSubTable(FILE* fp,SubTablePtr ptable)
{
    USHORT format = ptable->format;
    
    /* print encoding table */
    fprintf(fp,    " PlatformID: %2d\n",ptable->PlatformID);
    fprintf(fp,"\t\t EcodingID:  %2d\n",ptable->EncodingID);
    fprintf(fp,"\t\t 'cmap' Offset: 0x%08x\n",ptable->offset);
    
    /* print SubTable part */
    fprintf(fp,"\t\t Length:  %6d\n",ptable->length);
    fprintf(fp,"\t\t Version: %6d\n",ptable->version);

    switch(format)
	{
	case 0:
	    fprintf(fp,"\t\t Format 0 - Byte encoding table\n");
	    ttfPrintCMAP0(fp,ptable);
	    break;
	case 2:
	    fprintf(fp,"\t\t Format 2 - High-byte mapping through table\n");
	    ttfPrintCMAP2(fp,ptable);
	    break;
	case 4:
	    fprintf(fp,"\t\t Format 4 - Segment mapping to delta values\n");
	    ttfPrintCMAP4(fp,ptable);
	    break;
	case 6:
	    fprintf(fp,"\t\t Format 6 - Trimmed table mapping\n");
	    ttfPrintCMAP6(fp,ptable);
	    break;
	default:
	    ttfError("Unrecognized CMAP format\n");
	}
}
static void ttfFreeSubTable(SubTablePtr ptable)
{
    USHORT format = ptable->format;
    
    switch(format)
	{
	case 0:
	    ttfFreeCMAP0(ptable);
	    break;
	case 2:
	    ttfFreeCMAP2(ptable);
	    break;
	case 4:
	    ttfFreeCMAP4(ptable);
 	    break;
	case 6:
	    ttfFreeCMAP6(ptable);
	    break;
	}
}

static void ttfLoadCMAP0(FILE *fp,SubTablePtr subTable,ULONG offset)
{
    BYTE * array;
    
    subTable->map.cmap0 = XCALLOC1 (CMAP0);
    array = subTable->map.cmap0->glyphIndexArray; 

    xfseek(fp, offset, SEEK_SET, "ttfLoadCMAP0");

    /* Attention: we get lots of bytes at once as a work around of the
     * usual ttfGet*, this cause byte sex trouble as will be seen
     * in the fellowing procedures */
    if (fread(array,sizeof(BYTE),256,fp) != 256)
	ttfError("Error when getting glyphIndexArray\n");
}
static void ttfPrintCMAP0(FILE *fp,SubTablePtr subTable)
{
    USHORT index;
    int i;
    
    for (i=0;i<256;i++)
	{
	    index = ttfLookUpCMAP(subTable,i);
	    fprintf(fp,"\t\t Char %3d -> Index %4d\n",i,index);
	}
}
static USHORT ttfLookUpCMAP0(SubTablePtr subTable,USHORT cc)
{
    return subTable->map.cmap0->glyphIndexArray[cc & 0x00ff];
}
static void ttfFreeCMAP0(SubTablePtr subTable)
{
    free(subTable->map.cmap0);
}

static void ttfLoadCMAP2(FILE *fp,SubTablePtr subTable,ULONG offset)
{
    USHORT * array,i,n = 0;
    USHORT numGlyphId;
    SubHeaderPtr header;

    xfseek(fp, offset, SEEK_SET, "ttfLoadCMAP2");

    subTable->map.cmap2 = XCALLOC1 (CMAP2);
    array = subTable->map.cmap2->subHeaderKeys;
    
    ttfReadUSHORT (array, 256, fp);

    for (i=0;i<256;i++)
	{
	    array[i] /= 8;
	    if (n< array[i])
		n = array[i]; /* find the max of subHeaderKeys */
	}
    n += 1; /* the number of subHeaders is one plus the max of subHeaderKeys */

    subTable->map.cmap2->subHeaders = header = XCALLOC (n, SubHeader);
    for (i=0;i<n;i++)
	{
	    (header+i)->firstCode = ttfGetUSHORT(fp);
	    (header+i)->entryCount = ttfGetUSHORT(fp);
	    (header+i)->idDelta = ttfGetSHORT(fp);
	    (header+i)->idRangeOffset = ttfGetUSHORT(fp);
	    
	    /* it makes things easier to let the offset starts from
	     * the beginning of glyphIndexArray */
	    if ((header+i)->idRangeOffset != 0)
		(header+i)->idRangeOffset -= (sizeof(USHORT) +
					      (n-i-1) * sizeof(SubHeader)) ;
	}

    /* caculate the length of glyphIndexArray, this is ugly, there should be
     * a better way to get this information. */
    numGlyphId = 
	subTable->length - (256 + 3) * sizeof(USHORT) -  n * sizeof(SubHeader);
    numGlyphId /= sizeof(USHORT);
    subTable->map.cmap2->glyphIndexArray = XCALLOC (numGlyphId, USHORT);
    for (i=0;i<numGlyphId;i++)
	{
	    subTable->map.cmap2->glyphIndexArray[i] = ttfGetUSHORT(fp);
	}	
}
static void ttfPrintCMAP2(FILE *fp,SubTablePtr subTable)
{
    USHORT i,j,numGlyphId;
    USHORT *array,n=0,index;
    SubHeaderPtr header;
    TwoBytes tb;

    array = subTable->map.cmap2->subHeaderKeys;
    header = subTable->map.cmap2->subHeaders;
    
    for (i=0;i<256;i++)
	{
	    /* find the number of subHeader */
	    if (n< array[i])
		n = array[i]; 
	    fprintf(fp,"\t\t subHeaderKeys[%d] = %d\n",i,array[i]);
	}
    n += 1; /* the number of subHeaders is one plus the max of subHeaderKeys */
    fprintf(fp,"\t\t Number of SubHeaders is %d\n",n);

    for (i=0;i<=n;i++)
	{
	    fprintf(fp,"\t\t SubHeader[%d]\n",i);
	    fprintf(fp,"\t\t firstCode \t 0x%04x\n",(header+i)->firstCode);
	    fprintf(fp,"\t\t entryCount \t %d\n",(header+i)->entryCount);
	    fprintf(fp,"\t\t idDelta \t %d\n",(header+i)->idDelta);
	    fprintf(fp,"\t\t idRangeOffset \t 0x%04x\n\n",
		    (header+i)->idRangeOffset);
	}

    /* caculate the length of glyphIndexArray, this is ugly, there should be
     * a better way to get this information. */
    numGlyphId = 
	subTable->length - (256 + 3) * sizeof(USHORT) -  n * sizeof(SubHeader);
    numGlyphId /= sizeof(USHORT);
    fprintf(fp,"Number of glyphIndex: %d\n", numGlyphId);
    for (i=0;i<numGlyphId;i++)
	{
	    fprintf(fp,"\t\t glyphIdArray[%d] = %4d\n",i,
		    subTable->map.cmap2->glyphIndexArray[i]);
	}

    i = 0;
    fprintf(fp,"\t\t First Byte:\t %2x\n",i);
    tb.c[1] = i;
    for(j=0;j<=255;j++)
	{
	    tb.c[0] = j;
	    index = ttfLookUpCMAP2(subTable,tb.u);
	    fprintf(fp,"\t\t   Char %2x -> Index %d\n",j,index);
	}
    for (i=128;i<=255;i++)
	{
	    fprintf(fp,"\t\t First Byte:\t %2x\n",i);
	    tb.c[1] = i;
	    for(j=0;j<=255;j++)
		{
		    tb.c[0] = j;
		    index = ttfLookUpCMAP2(subTable,tb.u);
		    fprintf(fp,"\t\t   Char %2x -> Index %d\n",j,index);
		}
	}
}
static USHORT ttfLookUpCMAP2(SubTablePtr subTable,USHORT cc)
{
    USHORT index,idx = 0;
    USHORT *array = subTable->map.cmap2->subHeaderKeys;
    SubHeaderPtr headers = subTable->map.cmap2->subHeaders;
    SHORT idDelta;
    USHORT firstCode, entryCount, idRangeOffset;
    TwoBytes tb;
    unsigned char first,second;

   
    /* On little endian platforms "low byte" is the "first byte".
     * It determines if it is necessary to use the second byte to 
     * fully interprete the character code; for example, if the first byte 
     * is obviously an ASCII character then it is not necessary to 
     * interpret the second byte, on the other hand, if the first byte is 
     * zero then the second byte is a ASCII char, when the first byte
     * is not an ASCII char nor zero, those two bytes together determine
     * the meanning of the character code
     */
     tb.u = cc;
     first = tb.c[1];
     second = tb.c[0];

    /* select which subHeader to use */
    idx = array[first];

    firstCode = (headers+idx)->firstCode;
    entryCount = (headers+idx)->entryCount;
    idDelta = (headers+idx)->idDelta;
    idRangeOffset = (headers+idx)->idRangeOffset / sizeof (USHORT);

    if (second >= firstCode && second < firstCode+entryCount)
	{
	    /* use idRangeOffset to find where in the glyphIndexArray
	     * the correspinding index is */
	    idRangeOffset += (second - firstCode);
	    index = subTable->map.cmap2->glyphIndexArray[idRangeOffset];
	    if (index != 0)
		/* if the character is not a missing character then
		 * add idDelta to it */
		index += idDelta;
	}
    else
	/* The second code is out ranged then return the 
	 * missing glyph character */
	index = 0;
	    
    return index;
}
static void ttfFreeCMAP2(SubTablePtr subTable)
{
    free(subTable->map.cmap2->subHeaders);
    free(subTable->map.cmap2->glyphIndexArray);
    free(subTable->map.cmap2);
}

static void ttfLoadCMAP4(FILE *fp,SubTablePtr subTable,ULONG offset)
{
    USHORT segCount;
    USHORT len;

    xfseek(fp, offset, SEEK_SET, "ttfLoadCMAP4");
    
    subTable->map.cmap4 = XCALLOC1 (CMAP4);

    subTable->map.cmap4->segCountX2 = segCount = ttfGetUSHORT(fp);
    subTable->map.cmap4->searchRange = ttfGetUSHORT(fp);
    subTable->map.cmap4->entrySelector = ttfGetUSHORT(fp);
    subTable->map.cmap4->rangeShift = ttfGetUSHORT(fp);

    segCount /= 2;
    subTable->map.cmap4->endCount = ttfMakeUSHORT (segCount, fp);

    subTable->map.cmap4->reservedPad = ttfGetUSHORT(fp);

    subTable->map.cmap4->startCount = ttfMakeUSHORT (segCount, fp);

    subTable->map.cmap4->idDelta = ttfMakeUSHORT (segCount, fp);

    subTable->map.cmap4->idRangeOffset = ttfMakeUSHORT (segCount, fp);

    /* caculate the length of glyphIndexArray, this is ugly, there should be
     * a better way to get this information. */
    len = subTable->length - 8*sizeof(USHORT) - 4*segCount*sizeof(USHORT);
    len /= sizeof(USHORT);
    subTable->map.cmap4->glyphIndexArray = ttfMakeUSHORT (len, fp);
}
static void ttfPrintCMAP4(FILE *fp,SubTablePtr subTable)
{
    USHORT i;
    USHORT segCount,len;

    segCount = subTable->map.cmap4->segCountX2/2;
    fprintf(fp, "\t\t segCount:\t %d\n", segCount);
    fprintf(fp, "\t\t searchRange:\t %d\n", subTable->map.cmap4->searchRange);
    fprintf(fp, "\t\t entrySelector:\t %d\n", subTable->map.cmap4->entrySelector);
    fprintf(fp, "\t\t rangeShift:\t %d\n", subTable->map.cmap4->rangeShift);

    for (i=0;i<segCount;i++)
	{
	    fprintf(fp, "\t\t Seg   %3d :", i);
	    fprintf(fp, " St = %04x,", subTable->map.cmap4->startCount[i]);
	    fprintf(fp, " En = %04x,", subTable->map.cmap4->endCount[i]);
	    /* should this filed be SHORT or USHORT ?? */
	    fprintf(fp, " D = %6d,"  , 
		    (subTable->map.cmap4->idDelta[i]));
	    fprintf(fp, " RO = %6d," , subTable->map.cmap4->idRangeOffset[i]);

	    /* find the glyphIndex correpsonding to this segment */
	    if (subTable->map.cmap4->idRangeOffset[i] != 0)
		{
		    USHORT j;
		    j =  subTable->map.cmap4->segCountX2/2 - i;
		    j = subTable->map.cmap4->idRangeOffset[i] - 
			j*sizeof(USHORT);
		    fprintf(fp, " gId# = %d\n", (int) (j/sizeof(USHORT)));
		}
	    else
		fprintf(fp, " gId# = N/A\n");
	}

    /* caculate the length of glyphIndexArray, this is ugly, there should be
     * a better way to get this information. */
    len = subTable->length - 8*sizeof(USHORT) - 4*segCount*sizeof(USHORT);
    len /= sizeof(USHORT);    
    fprintf(fp,"\t\t Number of glyphIndex %d\n",len);
    for (i=0;i<len;i++)
	{
	    fprintf(fp,"\t\t glyphIdArray[%d] =  %d\n",i,
		    subTable->map.cmap4->glyphIndexArray[i]);
	}

    for (i=0;i<segCount;i++)
	{
	    int j,index;
	    fprintf(fp,"Segment %d:\n",i);
	    for (j=subTable->map.cmap4->startCount[i];
		 j<=subTable->map.cmap4->endCount[i];j++)
		{
		    index = ttfLookUpCMAP4(subTable,j);
		    fprintf(fp,"\t\tChar 0x%04x -> Index %d\n",j,index);
		}
	}
}
static USHORT ttfLookUpCMAP4(SubTablePtr subTable,USHORT cc)
{
    USHORT i;
    USHORT index=0, segCount = subTable->map.cmap4->segCountX2/2;

    for (i=0;i<segCount;i++)
	{
	    if (cc <= subTable->map.cmap4->endCount[i] &&
		cc >= subTable->map.cmap4->startCount[i])
		{
		    USHORT j;
		    if (subTable->map.cmap4->idRangeOffset[i] != 0)
			{
			    j = subTable->map.cmap4->idRangeOffset[i] - 
				(segCount - i)*sizeof(USHORT);
			    j = cc - subTable->map.cmap4->startCount[i] + j/2;
			    index = subTable->map.cmap4->glyphIndexArray[j];
			    if (index != 0)
				index += subTable->map.cmap4->idDelta[i];
			}
		    else
			{
			    index = cc + subTable->map.cmap4->idDelta[i];
			}
		    break;
		}
	}

    return index;
}
static void ttfFreeCMAP4(SubTablePtr subTable)
{
    free(subTable->map.cmap4->endCount);
    free(subTable->map.cmap4->startCount);
    free(subTable->map.cmap4->idDelta);
    free(subTable->map.cmap4->idRangeOffset);
    free(subTable->map.cmap4->glyphIndexArray);
    free(subTable->map.cmap4);
}

static void ttfLoadCMAP6(FILE *fp,SubTablePtr subTable,ULONG offset)
{
    USHORT len;

    xfseek(fp, offset, SEEK_SET, "ttfLoadCMAP6");

    subTable->map.cmap6 = XCALLOC1 (CMAP6);
    subTable->map.cmap6->firstCode = ttfGetUSHORT(fp);
    subTable->map.cmap6->entryCount = len = ttfGetUSHORT(fp);
    subTable->map.cmap6->glyphIndexArray = ttfMakeUSHORT (len, fp);
}
static void ttfPrintCMAP6(FILE *fp, SubTablePtr subTable)
{
    USHORT i;
    fprintf(fp,"\t\t First Code: 0x%04x\n",subTable->map.cmap6->firstCode);
    fprintf(fp,"\t\t Entry Count: %d\n",subTable->map.cmap6->entryCount);
    
    for (i=0;i<subTable->map.cmap6->entryCount;i++)
	{
	    fprintf(fp,"\t\t glyphIdArray[%d] =  %d\n",i,
		    subTable->map.cmap6->glyphIndexArray[i]);
	}
}
static USHORT ttfLookUpCMAP6(SubTablePtr subTable,USHORT cc)
{
    USHORT index;
    
    index = cc - subTable->map.cmap6->firstCode; 
    if (index < subTable->map.cmap6->entryCount)
	return  subTable->map.cmap6->glyphIndexArray[index];
    else
	/* index out of range, return missing glyph */
	return 0;
}
static void ttfFreeCMAP6(SubTablePtr subTable)
{
    free(subTable->map.cmap6->glyphIndexArray);
    free(subTable->map.cmap6);
}

