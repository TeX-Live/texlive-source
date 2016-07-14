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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: cmap.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

typedef union _TwoBytes {
    USHORT u;
    unsigned char c[2];
} TwoBytes;

static void ttfLoadCMAP(FILE *fp, CMAPPtr cmap, ULONG offset);

static void ttfLoadSubTable(FILE *fp, SubTablePtr subTable, ULONG offset);
static void ttfPrintSubTable(FILE* fp, SubTablePtr subTable);
static void ttfFreeSubTable(SubTablePtr subTable);

static void ttfLoadCMAP0(FILE *fp, MapPtr *map);
static void ttfPrintCMAP0(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP0(MapPtr map, USHORT cc);
static void ttfFreeCMAP0(MapPtr map);

static void ttfLoadCMAP2(FILE *fp, MapPtr *map);
static void ttfPrintCMAP2(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP2(MapPtr map, USHORT cc);
static void ttfFreeCMAP2(MapPtr map);

static void ttfLoadCMAP4(FILE *fp, MapPtr *map);
static void ttfPrintCMAP4(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP4(MapPtr map, USHORT cc);
static void ttfFreeCMAP4(MapPtr map);

static void ttfLoadCMAP6(FILE *fp, MapPtr *map);
static void ttfPrintCMAP6(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP6(MapPtr map, USHORT cc);
static void ttfFreeCMAP6(MapPtr map);

static void ttfLoadCMAP8(FILE *fp, MapPtr *map);
static void ttfPrintCMAP8(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP8(MapPtr map, ULONG cc);
static void ttfFreeCMAP8(MapPtr map);

static void ttfLoadCMAP10(FILE *fp, MapPtr *map);
static void ttfPrintCMAP10(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP10(MapPtr map, ULONG cc);
static void ttfFreeCMAP10(MapPtr map);

static void ttfLoadCMAP12(FILE *fp, MapPtr *map);
static void ttfPrintCMAP12(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP12(MapPtr map, ULONG cc);
static void ttfFreeCMAP12(MapPtr map);

static void ttfLoadCMAP13(FILE *fp, MapPtr *map);
static void ttfPrintCMAP13(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP13(MapPtr map, ULONG cc);
static void ttfFreeCMAP13(MapPtr map);

static void ttfLoadCMAP14(FILE *fp, MapPtr *map);
static void ttfPrintCMAP14(FILE *fp, MapPtr map);
static USHORT ttfLookUpCMAP14(MapPtr map, ULONG cc);
static void ttfFreeCMAP14(MapPtr map);

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
    int k = 0;

    xfseek(fp, offset, SEEK_SET, "ttfLoadCMAP");

    cmap->version = ttfGetUSHORT(fp);
    cmap->numberOfEncodings = n = ttfGetUSHORT(fp);
    cmap->encoding = XCALLOC (cmap->numberOfEncodings, Encoding);

    for (i = 0; i < n; i++)
        {
            int j;

            cmap->encoding[i].PlatformID = ttfGetUSHORT(fp);
            cmap->encoding[i].EncodingID = ttfGetUSHORT(fp);
            cmap->encoding[i].offset = ttfGetULONG(fp);
            cmap->encoding[i].mapindex = k++;
            for (j = 0; j < i; j++) {
                if (cmap->encoding[i].offset == cmap->encoding[j].offset)
                    {
                        cmap->encoding[i].mapindex = cmap->encoding[j].mapindex;
                        k--;
                        break;
                
                    }
            }
        }
    cmap->numberOfMaps = k;
    cmap->subTable = XCALLOC (cmap->numberOfMaps, SubTable);
    for (i = 0; i < n; i++)
        cmap->subTable[cmap->encoding[i].mapindex].offset = cmap->encoding[i].offset;
    for (i = 0; i < k; i++)
        ttfLoadSubTable(fp, cmap->subTable+i, offset);
}
void ttfPrintCMAP(FILE *fp,CMAPPtr cmap)
{
    USHORT i;

    fprintf(fp,"'cmap' Table - Character to Glyph Index Mapping Table\n");
    fprintf(fp,"-----------------------------------------------------\n");
    fprintf(fp,"\t 'cmap' version: %d\n", cmap->version);
    fprintf(fp,"\t number of encodings: %d\n", cmap->numberOfEncodings);
    fprintf(fp,"\t number of subtables: %d\n\n", cmap->numberOfMaps);

    for (i = 0; i < cmap->numberOfEncodings; i++)
        {
            fprintf(fp, "Encoding %3d.\t PlatformID: %2d\n", i,
                        cmap->encoding[i].PlatformID);
            fprintf(fp, "\t\t EcodingID:  %2d\n",
                        cmap->encoding[i].EncodingID);
            fprintf(fp, "\t\t SubTable: %d, Offset: 0x%08x\n\n",
                        cmap->encoding[i].mapindex, cmap->encoding[i].offset);
        }
    for (i = 0; i < cmap->numberOfMaps; i++)
        {
            fprintf(fp,"SubTable %3d.\t",i);
            ttfPrintSubTable(fp, cmap->subTable+i);
            fprintf(fp,"\n");
        }
}
USHORT ttfLookUpCMAP(MapPtr map, USHORT cc)
{
    USHORT idx,format = *map.format;

    switch (format)
        {
        case 0:
            idx = ttfLookUpCMAP0(map, cc);
            break;
        case 2:
            idx = ttfLookUpCMAP2(map, cc);
            break;
        case 4:
            idx = ttfLookUpCMAP4(map, cc);
            break;
        case 6:
            idx = ttfLookUpCMAP6(map, cc);
            break;
        case 8:
            idx = ttfLookUpCMAP8(map, cc);
            break;
        case 10:
            idx = ttfLookUpCMAP10(map, cc);
            break;
        case 12:
            idx = ttfLookUpCMAP12(map, cc);
            break;
        case 13:
            idx = ttfLookUpCMAP13(map, cc);
            break;
        case 14:
            idx = ttfLookUpCMAP14(map, cc);
            break;
        default:
            ttfError("Unrecognized CMAP format\n");
            return 0;
        }
    return idx;
}
void ttfFreeCMAP(CMAPPtr cmap)
{
    USHORT i;
    
    if (!cmap)
	return;

    free(cmap->encoding);

    for (i = 0; i < cmap->numberOfMaps; i++)
        ttfFreeSubTable(cmap->subTable+i);
    free(cmap->subTable);

    free(cmap);
}

static void ttfLoadSubTable(FILE *fp, SubTablePtr subTable, ULONG offset)
{
    ULONG pos;
    USHORT format;

    /* seek to the actuall position for this subtable
     * base: beginning of cmap
     * offset: offset field of each encoding table */
    pos =  offset  +  subTable->offset;
    xfseek(fp, pos, SEEK_SET, "ttfLoadEncoding");

    format = ttfGetUSHORT(fp); 
   
    switch(format)
        {
        case 0:
            ttfLoadCMAP0(fp, &subTable->map);
            break;
        case 2:
            ttfLoadCMAP2(fp, &subTable->map);
            break;
        case 4:
            ttfLoadCMAP4(fp, &subTable->map);
            break;
        case 6:
            ttfLoadCMAP6(fp, &subTable->map);
            break;
        case 8:
            ttfLoadCMAP8(fp, &subTable->map);
            break;
        case 10:
            ttfLoadCMAP10(fp, &subTable->map);
            break;
        case 12:
            ttfLoadCMAP12(fp, &subTable->map);
            break;
        case 13:
            ttfLoadCMAP13(fp, &subTable->map);
            break;
        case 14:
            ttfLoadCMAP14(fp, &subTable->map);
            break;
        default:
            ttfError("Unrecognized CMAP format\n");
        }

    *subTable->map.format = format; 
}
static void ttfPrintSubTable(FILE* fp, SubTablePtr subTable)
{
    USHORT format = *subTable->map.format;
    
    switch(format)
        {
        case 0:
            ttfPrintCMAP0(fp, subTable->map);
            break;
        case 2:
            ttfPrintCMAP2(fp, subTable->map);
            break;
        case 4:
            ttfPrintCMAP4(fp, subTable->map);
            break;
        case 6:
            ttfPrintCMAP6(fp, subTable->map);
            break;
        case 8:
            ttfPrintCMAP8(fp, subTable->map);
            break;
        case 10:
            ttfPrintCMAP10(fp, subTable->map);
            break;
        case 12:
            ttfPrintCMAP12(fp, subTable->map);
            break;
        case 13:
            ttfPrintCMAP13(fp, subTable->map);
            break;
        case 14:
            ttfPrintCMAP14(fp, subTable->map);
            break;
        default:
            ttfError("Unrecognized CMAP format\n");
        }
}
static void ttfFreeSubTable(SubTablePtr subTable)
{
    USHORT format = *subTable->map.format;
    
    switch(format)
        {
        case 0:
            ttfFreeCMAP0(subTable->map);
            break;
        case 2:
            ttfFreeCMAP2(subTable->map);
            break;
        case 4:
            ttfFreeCMAP4(subTable->map);
             break;
        case 6:
            ttfFreeCMAP6(subTable->map);
            break;
        case 8:
            ttfFreeCMAP8(subTable->map);
            break;
        case 10:
            ttfFreeCMAP10(subTable->map);
            break;
        case 12:
            ttfFreeCMAP12(subTable->map);
            break;
        case 13:
            ttfFreeCMAP13(subTable->map);
            break;
        case 14:
            ttfFreeCMAP14(subTable->map);
            break;
        }
}

static void ttfLoadCMAP0(FILE *fp, MapPtr *map)
{
    BYTE * array;
    
    map->cmap0 = XCALLOC1 (CMAP0);
    map->cmap0->length = ttfGetUSHORT(fp);
    map->cmap0->version = ttfGetUSHORT(fp);
    array = map->cmap0->glyphIndexArray; 

    /* Attention: we get lots of bytes at once as a work around of the
     * usual ttfGet*, this cause byte sex trouble as will be seen
     * in the following procedures */
    if (fread(array,sizeof(BYTE),256,fp) != 256)
        ttfError("Error when getting glyphIndexArray\n");
}
static void ttfPrintCMAP0(FILE *fp, MapPtr map)
{
    USHORT index;
    int i;
    
    fprintf(fp,    " Format 0 - Byte encoding table\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap0->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap0->version);
    for (i=0;i<256;i++)
        {
            index = ttfLookUpCMAP(map, i);
            fprintf(fp,"\t\t Char %3d -> Index %4d\n",i,index);
        }
}
static USHORT ttfLookUpCMAP0(MapPtr map, USHORT cc)
{
    return map.cmap0->glyphIndexArray[cc & 0x00ff];
}
static void ttfFreeCMAP0(MapPtr map)
{
    free(map.cmap0);
}

static void ttfLoadCMAP2(FILE *fp, MapPtr *map)
{
    USHORT * array,i,n = 0;
    USHORT numGlyphId;
    SubHeaderPtr header;

    map->cmap2 = XCALLOC1 (CMAP2);
    map->cmap2->length = ttfGetUSHORT(fp);
    map->cmap2->version = ttfGetUSHORT(fp);
    array = map->cmap2->subHeaderKeys;
    
    ttfReadUSHORT (array, 256, fp);

    for (i=0;i<256;i++)
        {
            array[i] /= 8;
            if (n< array[i])
                n = array[i]; /* find the max of subHeaderKeys */
        }
    n += 1; /* the number of subHeaders is one plus the max of subHeaderKeys */

    map->cmap2->subHeaders = header = XCALLOC (n, SubHeader);
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
        map->cmap2->length - (256 + 3) * sizeof(USHORT) -  n * sizeof(SubHeader);
    numGlyphId /= sizeof(USHORT);
    map->cmap2->glyphIndexArray = XCALLOC (numGlyphId, USHORT);
    for (i=0;i<numGlyphId;i++)
        {
            map->cmap2->glyphIndexArray[i] = ttfGetUSHORT(fp);
        }
}
static void ttfPrintCMAP2(FILE *fp, MapPtr map)
{
    USHORT i,j,numGlyphId;
    USHORT *array,n=0,index;
    SubHeaderPtr header;
    TwoBytes tb;

    fprintf(fp,    " Format 2 - High-byte mapping through table\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap2->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap2->version);
    array = map.cmap2->subHeaderKeys;
    header = map.cmap2->subHeaders;
    
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
        map.cmap2->length - (256 + 3) * sizeof(USHORT) -  n * sizeof(SubHeader);
    numGlyphId /= sizeof(USHORT);
    fprintf(fp,"Number of glyphIndex: %d\n", numGlyphId);
    for (i=0;i<numGlyphId;i++)
        {
            fprintf(fp,"\t\t glyphIdArray[%d] = %4d\n",i,
                    map.cmap2->glyphIndexArray[i]);
        }

    i = 0;
    fprintf(fp,"\t\t First Byte:\t %2x\n",i);
    tb.c[1] = i;
    for(j=0;j<=255;j++)
        {
            tb.c[0] = j;
            index = ttfLookUpCMAP2(map, tb.u);
            fprintf(fp,"\t\t   Char %2x -> Index %d\n",j,index);
        }
    for (i=128;i<=255;i++)
        {
            fprintf(fp,"\t\t First Byte:\t %2x\n",i);
            tb.c[1] = i;
            for(j=0;j<=255;j++)
                {
                    tb.c[0] = j;
                    index = ttfLookUpCMAP2(map, tb.u);
                    fprintf(fp,"\t\t   Char %2x -> Index %d\n",j,index);
                }
        }
}
static USHORT ttfLookUpCMAP2(MapPtr map, USHORT cc)
{
    USHORT index,idx = 0;
    USHORT *array = map.cmap2->subHeaderKeys;
    SubHeaderPtr headers = map.cmap2->subHeaders;
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
            index = map.cmap2->glyphIndexArray[idRangeOffset];
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
static void ttfFreeCMAP2(MapPtr map)
{
    free(map.cmap2->subHeaders);
    free(map.cmap2->glyphIndexArray);
    free(map.cmap2);
}

static void ttfLoadCMAP4(FILE *fp, MapPtr *map)
{
    USHORT segCount;
    USHORT len;

    map->cmap4 = XCALLOC1 (CMAP4);
    map->cmap4->length = ttfGetUSHORT(fp);
    map->cmap4->version = ttfGetUSHORT(fp);

    map->cmap4->segCountX2 = segCount = ttfGetUSHORT(fp);
    map->cmap4->searchRange = ttfGetUSHORT(fp);
    map->cmap4->entrySelector = ttfGetUSHORT(fp);
    map->cmap4->rangeShift = ttfGetUSHORT(fp);

    segCount /= 2;
    map->cmap4->endCount = ttfMakeUSHORT (segCount, fp);

    map->cmap4->reservedPad = ttfGetUSHORT(fp);

    map->cmap4->startCount = ttfMakeUSHORT (segCount, fp);

    map->cmap4->idDelta = ttfMakeUSHORT (segCount, fp);

    map->cmap4->idRangeOffset = ttfMakeUSHORT (segCount, fp);

    /* caculate the length of glyphIndexArray, this is ugly, there should be
     * a better way to get this information. */
    len = map->cmap4->length - 8*sizeof(USHORT) - 4*segCount*sizeof(USHORT);
    len /= sizeof(USHORT);
    map->cmap4->glyphIndexArray = ttfMakeUSHORT (len, fp);
}
static void ttfPrintCMAP4(FILE *fp, MapPtr map)
{
    USHORT i;
    USHORT segCount,len;

    fprintf(fp,    " Format 4 - Segment mapping to delta values\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap4->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap4->version);
    segCount = map.cmap4->segCountX2/2;
    fprintf(fp, "\t\t segCount:\t %d\n", segCount);
    fprintf(fp, "\t\t searchRange:\t %d\n", map.cmap4->searchRange);
    fprintf(fp, "\t\t entrySelector:\t %d\n", map.cmap4->entrySelector);
    fprintf(fp, "\t\t rangeShift:\t %d\n", map.cmap4->rangeShift);

    for (i=0;i<segCount;i++)
        {
            fprintf(fp, "\t\t Seg   %3d :", i);
            fprintf(fp, " St = %04x,", map.cmap4->startCount[i]);
            fprintf(fp, " En = %04x,", map.cmap4->endCount[i]);
            /* should this filed be SHORT or USHORT ?? */
            fprintf(fp, " D = %6d,"  , 
                    (map.cmap4->idDelta[i]));
            fprintf(fp, " RO = %6d," , map.cmap4->idRangeOffset[i]);

            /* find the glyphIndex correpsonding to this segment */
            if (map.cmap4->idRangeOffset[i] != 0)
                {
                    USHORT j;
                    j =  map.cmap4->segCountX2/2 - i;
                    j = map.cmap4->idRangeOffset[i] - 
                        j*sizeof(USHORT);
                    fprintf(fp, " gId# = %d\n", (int) (j/sizeof(USHORT)));
                }
            else
                fprintf(fp, " gId# = N/A\n");
        }

    /* caculate the length of glyphIndexArray, this is ugly, there should be
     * a better way to get this information. */
    len = map.cmap4->length - 8*sizeof(USHORT) - 4*segCount*sizeof(USHORT);
    len /= sizeof(USHORT);    
    fprintf(fp,"\t\t Number of glyphIndex %d\n",len);
    for (i=0;i<len;i++)
        {
            fprintf(fp,"\t\t glyphIdArray[%d] =  %d\n",i,
                    map.cmap4->glyphIndexArray[i]);
        }

    for (i=0;i<segCount;i++)
        {
            int j,index;
            fprintf(fp,"Segment %d:\n",i);
            for (j=map.cmap4->startCount[i];
                 j<=map.cmap4->endCount[i];j++)
                {
                    index = ttfLookUpCMAP4(map, j);
                    fprintf(fp,"\t\tChar 0x%04x -> Index %d\n",j,index);
                }
        }
}
static USHORT ttfLookUpCMAP4(MapPtr map, USHORT cc)
{
    USHORT i;
    USHORT index=0, segCount = map.cmap4->segCountX2/2;

    for (i=0;i<segCount;i++)
        {
            if (cc <= map.cmap4->endCount[i] &&
                cc >= map.cmap4->startCount[i])
                {
                    USHORT j;
                    if (map.cmap4->idRangeOffset[i] != 0)
                        {
                            j = map.cmap4->idRangeOffset[i] - 
                                (segCount - i)*sizeof(USHORT);
                            j = cc - map.cmap4->startCount[i] + j/2;
                            index = map.cmap4->glyphIndexArray[j];
                            if (index != 0)
                                index += map.cmap4->idDelta[i];
                        }
                    else
                        {
                            index = cc + map.cmap4->idDelta[i];
                        }
                    break;
                }
        }

    return index;
}
static void ttfFreeCMAP4(MapPtr map)
{
    free(map.cmap4->endCount);
    free(map.cmap4->startCount);
    free(map.cmap4->idDelta);
    free(map.cmap4->idRangeOffset);
    free(map.cmap4->glyphIndexArray);
    free(map.cmap4);
}

static void ttfLoadCMAP6(FILE *fp, MapPtr *map)
{
    USHORT len;

    map->cmap6 = XCALLOC1 (CMAP6);
    map->cmap6->length = ttfGetUSHORT(fp);
    map->cmap6->version = ttfGetUSHORT(fp);
    map->cmap6->firstCode = ttfGetUSHORT(fp);
    map->cmap6->entryCount = len = ttfGetUSHORT(fp);
    map->cmap6->glyphIndexArray = ttfMakeUSHORT (len, fp);
}
static void ttfPrintCMAP6(FILE *fp, MapPtr map)
{
    USHORT i;

    fprintf(fp,    " Format 6 - Trimmed table mapping\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap6->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap6->version);
    fprintf(fp,"\t\t First Code: 0x%04x\n", map.cmap6->firstCode);
    fprintf(fp,"\t\t Entry Count: %d\n", map.cmap6->entryCount);
    
    for (i=0;i<map.cmap6->entryCount;i++)
        {
            fprintf(fp,"\t\t glyphIdArray[%d] =  %d\n",i,
                    map.cmap6->glyphIndexArray[i]);
        }
}
static USHORT ttfLookUpCMAP6(MapPtr map, USHORT cc)
{
    USHORT index;
    
    index = cc - map.cmap6->firstCode; 
    if (index < map.cmap6->entryCount)
        return  map.cmap6->glyphIndexArray[index];
    else
        /* index out of range, return missing glyph */
        return 0;
}
static void ttfFreeCMAP6(MapPtr map)
{
    free(map.cmap6->glyphIndexArray);
    free(map.cmap6);
}

static void ttfLoadCMAP8(FILE *fp, MapPtr *map)
{
    int i;

    map->cmap8 = XCALLOC1 (CMAP8);
    ttfGetUSHORT(fp);
    map->cmap8->length = ttfGetULONG(fp);
    map->cmap8->version = ttfGetULONG(fp);
    if (fread(map->cmap8->is32, sizeof(BYTE), 8192, fp) != 8192)
        ttfError("Error when getting is32\n");
    map->cmap8->nGroups = ttfGetULONG(fp);
    map->cmap8->charGroup = XCALLOC (map->cmap8->nGroups, CharGroup);
    for (i = 0; i < map->cmap8->nGroups; i++) {
        map->cmap8->charGroup[i].startCharCode = ttfGetULONG(fp);
        map->cmap8->charGroup[i].endCharCode = ttfGetULONG(fp);
        map->cmap8->charGroup[i].startGlyphID = ttfGetULONG(fp);
    }
}
static void ttfPrintCMAP8(FILE *fp, MapPtr map)
{
    int i;

    fprintf(fp,    " Format 8 - Mixed 16-bit and 32-bit coverage\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap8->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap8->version);
    fprintf(fp,"\t\t nGroups: %6d\n", map.cmap8->nGroups);
    for (i = 0; i < 256; i++) {
        int j;

        fprintf(fp, "\t is32 %2x", i);
        for (j = 0; j < 32; j++)
            fprintf(fp, j % 4 ? "%2x" : " %2x", map.cmap8->is32[i << 5 | j]); 
        fprintf(fp, "\n");
    }
    for (i = 0; i < map.cmap8->nGroups; i++)
        fprintf(fp, "\t\t Group %4d : startCharCode = %d, endCharCode = %d, startGlyphID = %d\n", i,
                    map.cmap8->charGroup[i].startCharCode,
                    map.cmap8->charGroup[i].endCharCode,
                    map.cmap8->charGroup[i].startGlyphID);
}
static USHORT ttfLookUpCMAP8(MapPtr map, ULONG cc)
{
    int i;

    for (i = 0; i < map.cmap8->nGroups; i++)
        if (cc > map.cmap8->charGroup[i].endCharCode)
            continue;
        else if (cc < map.cmap8->charGroup[i].startCharCode)
            break;
        else
            return map.cmap8->charGroup[i].startGlyphID +
                   (cc - map.cmap8->charGroup[i].startCharCode);

    /* index out of range, return missing glyph */
    return 0;
}
static void ttfFreeCMAP8(MapPtr map)
{
    free(map.cmap8->charGroup);
    free(map.cmap8);
}

static void ttfLoadCMAP10(FILE *fp, MapPtr *map)
{
    ULONG len;

    map->cmap10 = XCALLOC1 (CMAP10);
    ttfGetUSHORT(fp);
    map->cmap10->length = ttfGetULONG(fp);
    map->cmap10->version = ttfGetULONG(fp);
    map->cmap10->startCharCode = ttfGetULONG(fp);
    map->cmap10->numChars = len = ttfGetULONG(fp);
    map->cmap10->glyphs = ttfMakeUSHORT (len, fp);
}
static void ttfPrintCMAP10(FILE *fp, MapPtr map)
{
    int i;

    fprintf(fp,    " Format 10 - Trimmed array\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap10->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap10->version);
    fprintf(fp,"\t\t Start Char Code: 0x%04x\n", map.cmap10->startCharCode);
    fprintf(fp,"\t\t Num Chars: %d\n", map.cmap10->numChars);
    
    for (i = 0; i < map.cmap10->numChars; i++)
        {
            fprintf(fp, "\t\t glyphs[%d] =  %d\n", i, map.cmap10->glyphs[i]);
        }
}
static USHORT ttfLookUpCMAP10(MapPtr map, ULONG cc)
{
    ULONG index;
    
    index = cc - map.cmap10->startCharCode; 
    if (index < map.cmap10->numChars)
        return  map.cmap10->glyphs[index];
    else
        /* index out of range, return missing glyph */
        return 0;
}
static void ttfFreeCMAP10(MapPtr map)
{
    free(map.cmap10->glyphs);
    free(map.cmap10);
}

static void ttfLoadCMAP12(FILE *fp, MapPtr *map)
{
    int i;

    map->cmap12 = XCALLOC1 (CMAP12);
    ttfGetUSHORT(fp);
    map->cmap12->length = ttfGetULONG(fp);
    map->cmap12->version = ttfGetULONG(fp);
    map->cmap12->nGroups = ttfGetULONG(fp);
    map->cmap12->charGroup = XCALLOC (map->cmap12->nGroups, CharGroup);
    for (i = 0; i < map->cmap12->nGroups; i++) {
        map->cmap12->charGroup[i].startCharCode = ttfGetULONG(fp);
        map->cmap12->charGroup[i].endCharCode = ttfGetULONG(fp);
        map->cmap12->charGroup[i].startGlyphID = ttfGetULONG(fp);
    }
}
static void ttfPrintCMAP12(FILE *fp, MapPtr map)
{
    int i;

    fprintf(fp,    " Format 12 - Segmented coverage\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap12->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap12->version);
    fprintf(fp,"\t\t nGroups: %6d\n", map.cmap12->nGroups);
    for (i = 0; i < map.cmap12->nGroups; i++)
        fprintf(fp, "\t\t Group %4d : startCharCode = %d, endCharCode = %d, startGlyphID = %d\n", i,
                    map.cmap12->charGroup[i].startCharCode,
                    map.cmap12->charGroup[i].endCharCode,
                    map.cmap12->charGroup[i].startGlyphID);
}
static USHORT ttfLookUpCMAP12(MapPtr map, ULONG cc)
{
    int i;

    for (i = 0; i < map.cmap12->nGroups; i++)
        if (cc > map.cmap12->charGroup[i].endCharCode)
            continue;
        else if (cc < map.cmap12->charGroup[i].startCharCode)
            break;
        else
            return map.cmap12->charGroup[i].startGlyphID +
                   (cc - map.cmap12->charGroup[i].startCharCode);

    /* index out of range, return missing glyph */
    return 0;
}
static void ttfFreeCMAP12(MapPtr map)
{
    free(map.cmap12->charGroup);
    free(map.cmap12);
}

static void ttfLoadCMAP13(FILE *fp, MapPtr *map)
{
    int i;

    map->cmap13 = XCALLOC1 (CMAP13);
    ttfGetUSHORT(fp);
    map->cmap13->length = ttfGetULONG(fp);
    map->cmap13->version = ttfGetULONG(fp);
    map->cmap13->nGroups = ttfGetULONG(fp);
    map->cmap13->charGroup = XCALLOC (map->cmap13->nGroups, CharGroup);
    for (i = 0; i < map->cmap13->nGroups; i++) {
        map->cmap13->charGroup[i].startCharCode = ttfGetULONG(fp);
        map->cmap13->charGroup[i].endCharCode = ttfGetULONG(fp);
        map->cmap13->charGroup[i].startGlyphID = ttfGetULONG(fp);
    }
}
static void ttfPrintCMAP13(FILE *fp, MapPtr map)
{
    int i;

    fprintf(fp,    " Format 13 - Many-to-one range mappings\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap13->length);
    fprintf(fp,"\t\t Version: %6d\n", map.cmap13->version);
    fprintf(fp,"\t\t nGroups: %6d\n", map.cmap13->nGroups);
    for (i = 0; i < map.cmap13->nGroups; i++)
        fprintf(fp, "\t\t Group %4d : startCharCode = %d, endCharCode = %d, glyphID = %d\n", i,
                    map.cmap13->charGroup[i].startCharCode,
                    map.cmap13->charGroup[i].endCharCode,
                    map.cmap13->charGroup[i].startGlyphID);
}
static USHORT ttfLookUpCMAP13(MapPtr map, ULONG cc)
{
    int i;

    for (i = 0; i < map.cmap13->nGroups; i++)
        if (cc > map.cmap13->charGroup[i].endCharCode)
            continue;
        else if (cc < map.cmap13->charGroup[i].startCharCode)
            break;
        else
            return map.cmap12->charGroup[i].startGlyphID;

    /* index out of range, return missing glyph */
    return 0;
}
static void ttfFreeCMAP13(MapPtr map)
{
    free(map.cmap13->charGroup);
    free(map.cmap13);
}

static void ttfLoadCMAP14(FILE *fp, MapPtr *map)
{
    map->cmap14 = XCALLOC1 (CMAP14);
    map->cmap14->length = ttfGetULONG(fp);
}
static void ttfPrintCMAP14(FILE *fp, MapPtr map)
{
    fprintf(fp,    " Format 14 - Unicode variation sequences\n");
    fprintf(fp,"\t\t Length:  %6d\n", map.cmap14->length);
    fprintf(fp,"\t\t *** NOT YET IMPLEMENTED ***\n");
}
static USHORT ttfLookUpCMAP14(MapPtr map, ULONG cc)
{
    return 0;
}
static void ttfFreeCMAP14(MapPtr map)
{
    free(map.cmap14);
}

