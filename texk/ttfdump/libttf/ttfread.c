/* ttfread.c
 * read data element form ttf file
 * swap byte order carefully for MSBFirst
 * presume that int is 32 bits
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#if defined(__GNUC__) && defined(_IBMR2)
/* Workaround for missing typedef in gcc (egcs-2.90.27 980315 (egcs-1.0.2 release)) */
typedef long		blkcnt_t;	/* number of blocks in the file */
#endif

#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: ttfread.c,v 1.2 1998/07/04 13:17:51 werner Exp $	 */

BYTE ttfGetBYTE(FILE *fp)
{
    int cc;
    if ((cc = fgetc(fp)) == EOF)
	{
	    if (feof(fp) != 0)
		ttfError("Unexpected EOF\n");
	    else
		ttfError("Error Getting BYTE\n");
	}
    return (BYTE) cc;
}

CHAR ttfGetCHAR(FILE *fp)
{
    int cc;
    if ((cc = fgetc(fp)) == EOF)
	{
	    if (feof(fp) != 0)
		ttfError("Unexpected EOF\n");
	    else
		ttfError("Error Getting CHAR\n");
	}
    return (CHAR) cc;
}

USHORT ttfGetUSHORT(FILE *fp)
{
    int cc;
    cc = ttfGetBYTE(fp) << 8;
    cc |= ttfGetBYTE(fp);
    
    return (USHORT) cc;
}

SHORT ttfGetSHORT(FILE *fp)
{
    int cc;
    cc = ttfGetBYTE(fp) << 8;
    cc |= ttfGetBYTE(fp);
    
    return (SHORT) cc;
}

ULONG ttfGetULONG(FILE *fp)
{
    int cc;
    cc = ttfGetBYTE(fp) << 24;
    cc |= ttfGetBYTE(fp) << 16;
    cc |= ttfGetBYTE(fp) << 8;
    cc |= ttfGetBYTE(fp);

    return (ULONG) cc;
}

LONG ttfGetLONG(FILE *fp)
{
    int cc;
    cc = ttfGetBYTE(fp) << 24;
    cc |= ttfGetBYTE(fp) << 16;
    cc |= ttfGetBYTE(fp) << 8;
    cc |= ttfGetBYTE(fp);

    return (LONG) cc;
}

Fixed ttfGetFixed(FILE *fp)
{
    return (Fixed) ttfGetULONG(fp);
}

FUnit ttfGetFUnit(FILE *fp)
{
    return (FUnit) ttfGetUSHORT(fp);
}

FWord ttfGetFWord(FILE *fp)
{
    return (FWord) ttfGetSHORT(fp);
}

uFWord ttfGetuFWord(FILE *fp)
{
    return (uFWord) ttfGetUSHORT(fp);
}

F2Dot14 ttfGetF2Dot14(FILE *fp)
{
    return (F2Dot14) ttfGetUSHORT(fp);
}

/* Read arrays.  */
void ttfReadUSHORT(USHORT *array, size_t nelem, FILE *fp)
{
    int i;
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetUSHORT (fp);
}

void ttfReadULONG(ULONG *array, size_t nelem, FILE *fp)
{
    int i;
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetULONG (fp);
}

void ttfReadFWord(FWord *array, size_t nelem, FILE *fp)
{
    int i;
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetFWord (fp);
}

/* Allocate and read arrays.  */
BYTE *ttfMakeBYTE(size_t nelem, FILE *fp)
{
    int i;
    BYTE *array = XTALLOC (nelem, BYTE);
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetBYTE (fp);
    return array;
}

USHORT *ttfMakeUSHORT(size_t nelem, FILE *fp)
{
    int i;
    USHORT *array = XTALLOC (nelem, USHORT);
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetUSHORT (fp);
    return array;
}

SHORT *ttfMakeSHORT(size_t nelem, FILE *fp)
{
    int i;
    SHORT *array = XTALLOC (nelem, SHORT);
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetSHORT (fp);
    return array;
}

ULONG *ttfMakeULONG(size_t nelem, FILE *fp)
{
    int i;
    ULONG *array = XTALLOC (nelem, ULONG);
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetULONG (fp);
    return array;
}

LONG *ttfMakeLONG(size_t nelem, FILE *fp)
{
    int i;
    LONG *array = XTALLOC (nelem, LONG);
    for (i = 0; i < nelem; i++)
        array[i] = ttfGetLONG (fp);
    return array;
}

