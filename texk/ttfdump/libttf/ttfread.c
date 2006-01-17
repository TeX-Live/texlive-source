/* ttfread.c
 * read data element form ttf file
 * swap byte order carefully for MSBFirst
 * presume that int is 32 bits
 */
#include <stdio.h>

#if defined(__GNUC__) && defined(_IBMR2)
/* Workaround for missing typedef in gcc (egcs-2.90.27 980315 (egcs-1.0.2 release)) */
typedef long		blkcnt_t;	/* number of blocks in the file */
#endif

#include "ttf.h"
#include "ttfutil.h"

/* 	$Id: ttfread.c,v 1.2 1998/07/04 13:17:51 werner Exp $	 */

#ifndef lint
static char vcid[] = "$Id: ttfread.c,v 1.2 1998/07/04 13:17:51 werner Exp $";
#endif /* lint */

inline BYTE ttfGetBYTE(FILE *fp)
{
    int cc;
    if ((cc = fgetc(fp)) == EOF)
	{
	    /*if (feof(fp) != 0)
		ttfError("Unexpected EOF \n");
	    else
		ttfError("Error Getting BYTE\n");*/
	}
    return (BYTE) cc;
}
short ttfGetLSB16(FILE *fp)
{
    int cc;
    cc = ttfGetBYTE(fp);
    cc |= ttfGetBYTE(fp) <<8;
    return cc;
}

int ttfGetLSB32(FILE *fp)
{
    int cc;
    cc = ttfGetBYTE(fp);
    cc |= ttfGetBYTE(fp) <<  8;
    cc |= ttfGetBYTE(fp) << 16;
    cc |= ttfGetBYTE(fp) << 24;
    return cc;
}

CHAR ttfGetCHAR(FILE *fp)
{
    int cc;
    if ((cc = fgetc(fp)) == EOF)
	{
	    if (feof(fp) != 0)
		ttfError("Unexpected EOF \n");
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

USHORT ttfGetSHORT(FILE *fp)
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

