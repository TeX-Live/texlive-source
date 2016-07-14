#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: name.c,v 1.1.1.1 1998/06/05 07:47:52 robert Exp $	 */

static void ttfLoadNAME(FILE *fp,NAMEPtr name,ULONG offset);
static void ttfLoadNameRecord(FILE *fp,NameRecordPtr rec,ULONG offset);
static void ttfPrintNameRecord(FILE *fp,NameRecordPtr rec);
static void ttfLoadNameRecordData(FILE *fp,NameRecordPtr rec,ULONG offset);
static void ttfPrintNameRecordData(FILE *fp,NameRecordPtr rec);
static void ttfFreeNameRecordData(NameRecordPtr rec);
static void HexDump(char *p,char *hex,char *asc,int len);

void ttfInitNAME(TTFontPtr font)
{
    ULONG tag = FT_MAKE_TAG ('n', 'a', 'm', 'e');
    TableDirPtr ptd;
    
    if ((ptd = ttfLookUpTableDir(tag,font)) != NULL)
	{
	    font->name = XCALLOC1 (NAME);
	    ttfLoadNAME(font->fp,font->name,ptd->offset);
	}
}

static void ttfLoadNAME(FILE *fp,NAMEPtr name,ULONG offset)
{
    USHORT i,n;
    ULONG pos;

    xfseek(fp, offset, SEEK_SET, "ttfLoadNAME");

    name->format = ttfGetUSHORT(fp);
    name->numberOfRecords = n = ttfGetUSHORT(fp);
    name->offset = ttfGetUSHORT(fp);

    name->NameRecords = XCALLOC (n,  NameRecord);

    pos = offset + sizeof(USHORT)*3;

    for(i=0;i<n;i++,pos+=12 /*sizeof(NameRecord)*/)
	{
	    ttfLoadNameRecord(fp,name->NameRecords+i,pos);
	    ttfLoadNameRecordData(fp,name->NameRecords+i,offset+name->offset);
	}
}
void ttfPrintNAME(FILE *fp,NAMEPtr name)
{
    USHORT i;

    fprintf(fp,"'name' Table - Naming Table\n");
    fprintf(fp,"---------------------------\n");
    fprintf(fp,"\t Format:\t\t %d\n",name->format);
    fprintf(fp,"\t Number of Record:\t %d\n",name->numberOfRecords);
    fprintf(fp,"\t Storage offset:\t %d\n",name->offset);
    
    for (i=0;i<name->numberOfRecords;i++)
	{
	    fprintf(fp,"Name table %3d.\t",i);
	    ttfPrintNameRecord(fp,name->NameRecords+i);
	}
    
}
void ttfFreeNAME(NAMEPtr name)
{
    USHORT i;

    if (!name)
	return;

    for (i=0;i<name->numberOfRecords;i++)
	ttfFreeNameRecordData(name->NameRecords+i);
    free(name->NameRecords);
    free(name);
}
/* offset: address of the beginning of the name record */
static void ttfLoadNameRecord(FILE *fp,NameRecordPtr rec,ULONG offset)
{
    xfseek(fp, offset, SEEK_SET, "ttfLoadNameRecord");

    rec->PlatformID = ttfGetUSHORT(fp);
    rec->EncodingID = ttfGetUSHORT(fp);
    rec->LanguageID = ttfGetUSHORT(fp);
    rec->NameID = ttfGetUSHORT(fp);
    rec->length = ttfGetUSHORT(fp);
    rec->offset = ttfGetUSHORT(fp);
}
static void ttfPrintNameRecord(FILE *fp,NameRecordPtr rec)
{
    fprintf(fp," PlatformID:\t %d\n",rec->PlatformID);
    fprintf(fp,"\t\t EncodingID:\t %d\n",rec->EncodingID);
    fprintf(fp,"\t\t LanguageID:\t %d\n",rec->LanguageID);
    fprintf(fp,"\t\t NameID:\t %d\n",rec->NameID);
    fprintf(fp,"\t\t Length:\t %d\n",rec->length);
    fprintf(fp,"\t\t Offset:\t %d\n",rec->offset);
    ttfPrintNameRecordData(fp,rec);
}
/* ULONG offset: address of the beginning of the actual data (base actually)
 * base: start of data storage, specified by NAME.offset
 * offset: specified by NameRecord.offset */
static void ttfLoadNameRecordData(FILE *fp,NameRecordPtr rec,ULONG offset)
{
    ULONG pos;

    pos =  offset  +  rec->offset;
    xfseek(fp, pos, SEEK_SET, "ttfLoadNameRecordData");

    rec->data = XTALLOC (rec->length, char);
    if (fread(rec->data,sizeof(char),rec->length,fp) != rec->length)
	ttfError("Error when getting Name Record Data\n");
}

static void ttfPrintNameRecordData(FILE *fp,NameRecordPtr rec)
{
    USHORT i,j;
    char hexbuf[100],ascbuf[100],*p;

    p = rec->data;
    for (i=0;i<rec->length/10;i++,p+=10)
	{
	    HexDump(p,hexbuf,ascbuf,10);
	    fprintf(fp,"\t\t %s >  %s\n",hexbuf,ascbuf);
	}
    HexDump(p,hexbuf,ascbuf,rec->length % 10);
    /* i know that this is ugly, but this makes output beautiful */
    i = strlen(hexbuf);
    for (j=i;j<30;j++)
	{
	    hexbuf[j] = ' ';
	}
    fprintf(fp,"\t\t %s > %s\n",hexbuf,ascbuf);

}
static void HexDump(char *p,char *hex,char *asc,int len)
{
    int i;
    unsigned char c;
    char buf[100];
    
    *hex = '\0';
    *asc = '\0';
    for (i=0;i<len;i++)
	{
	    c = *(p+i);
	    sprintf(buf,"%02x ",c);
	    strcat(hex,buf);
	    if (isprint(c))
		*(asc+i) = c;
	    else
		*(asc+i) = '.';
	}
    *(asc+len) = *(hex+len*3) = '\0';
}
static void ttfFreeNameRecordData(NameRecordPtr rec)
{
    free(rec->data);
}
