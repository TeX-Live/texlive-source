/* ttf.h -- header file that defines the top level abstraction of a True Type font, as 
 * well as the usual typedefs in TTF spec.
 * Copyright (C) 1996-1998 Li-Da Lho, All right reserved.
 */

#ifndef __TTF_H
#define __TTF_H

#include <stdio.h>
#include "config.h"

/* $Id: ttf.h,v 1.3 1998/07/06 06:07:01 werner Exp $ */

typedef unsigned char BYTE;
typedef signed char CHAR;
typedef unsigned short USHORT;
typedef signed short SHORT;


#if SIZEOF_INT==4
typedef unsigned int ULONG;
typedef signed int LONG;
#else
#error "Unsupported size of `int' type!"
#endif

typedef ULONG Fixed;            /* 16.16 fixed point number used for
                                 * version information */
typedef USHORT FUnit;
typedef SHORT FWord;            /* pixel position in the unit of FUnit */
typedef USHORT uFWord;
typedef SHORT F2Dot14;          /* 2.14 fixed point number used for
                                 * unit vectors */
typedef LONG F26Dot6;           /* 26.6 fixed point number to specify
                                 * pixel positions */

BYTE ttfGetBYTE(FILE *fp);
CHAR ttfGetCHAR(FILE *fp);
USHORT ttfGetUSHORT(FILE *fp);
SHORT ttfGetSHORT(FILE *fp);
ULONG ttfGetULONG(FILE *fp);
LONG ttfGetLONG(FILE *fp);
Fixed ttfGetFixed(FILE *fp);
FUnit ttfGetFUnit(FILE *fp);
FWord ttfGetFWord(FILE *fp);
uFWord ttfGetuFWord(FILE *fp);
F2Dot14 ttfGetF2Dot14(FILE *fp);

/* Read arrays.  */
void ttfReadUSHORT(USHORT *array, size_t nelem, FILE *fp);
void ttfReadULONG(ULONG *array, size_t nelem, FILE *fp);
void ttfReadFWord(FWord *array, size_t nelem, FILE *fp);

/* Allocate and read arrays.  */
BYTE *ttfMakeBYTE(size_t nelem, FILE *fp);
USHORT *ttfMakeUSHORT(size_t nelem, FILE *fp);
SHORT *ttfMakeSHORT(size_t nelem, FILE *fp);
ULONG *ttfMakeULONG(size_t nelem, FILE *fp);
LONG *ttfMakeLONG(size_t nelem, FILE *fp);

#include "tables.h"
#include "gcache.h"
#include "gstate.h"

typedef struct
{
  BYTE opcode;
  BYTE *iStream;
}
InstructionDef;

typedef struct
{
  BYTE fun;
  BYTE *iStream;
}
FunctionDef;

typedef struct
{
  /* data for instructions */
  BYTE *iStream;                /* instruction stream */
  USHORT ip;                    /* instruction pointer */
  USHORT insLength;             /* instruction length */
  LONG *StorageArea;            /* storage area */
  LONG *Stack;                  /* stack */
  USHORT sp;                    /* stack pointer */
  USHORT stklimit;              /* stack limit */
  /* data about point size and resolution */
  ULONG ppem;                   /* pixel per em */
  ULONG xres;                   /* x resolution in dpi */
  ULONG yres;                   /* y resolution in dpi */
  F26Dot6 pointsize;            /* the point size of the font */
  /* data for grahpics stat and cvt */
  F26Dot6 *cvt;                 /* Control value table, scaled form cvt in 
                                 * TTFont */
  GraphicsState gstate;         /* graphics state used for instructions */
  GraphicsState gstate_init;    /* initial graphics state after fpgm and cvt 
                                 * programs are executed */
#if 0
  /* user definded instructions and functions */
  InstructionDef iDef;          /* instruction definitions */
  FunctionDef    fDef;          /* function definitions */
#endif

  int Error_State;              /* error state indicate the error */
}
VirtualMachine;

typedef struct
{
  /* misc informations */
  FILE *fp;
  char *ttfname;
  int *refcount;                /* how many instances of this font */
  Fixed version;
  USHORT numTables;             /* it seems not necessary to hold all the 
                                 * OffsetTables */
  ULONG glyphOffset;            /* where the glyph table starts */
  TableDirPtr dir;              /* Pointer to Table Directories */
  EncodingPtr encoding;         /* `encoding' is the currently used encoding 
                                 * table */
  USHORT numCacheElements;      /* number of glyph cache elements */

  VirtualMachine vm;            /* the virtual machine */

  GlyphCachePtr gcache;         /* pointer to glyph cache */

  /* required tables */
  CMAPPtr cmap;
  HEADPtr head;
  HHEAPtr hhea;
  HMTXPtr hmtx;
  LOCAPtr loca;
  MAXPPtr maxp;
  NAMEPtr name;
  POSTPtr post;
  OS_2Ptr os2;

  /* optional tables */
  FWord *cvt;                   /* control value table, not scaled yet */
  USHORT cvtLength;             /* number of FWords of CVT */
  BYTE *fpgm;                   /* font program */
  USHORT fpgmLength;            /* number of bytes of fpgm */
  BYTE *prep;                   /* cvt program */
  USHORT prepLength;            /* number of bytes of prep */

  /* those optional tables seem not to be necessary for a True Type font
   * server */
  GASPPtr gasp;
  HDMXPtr hdmx;
  KERNPtr kern;
  LTSHPtr ltsh;
  PCLTPtr pclt;
  VDMXPtr vdmx;
  VHEAPtr vhea;
  VMTXPtr vmtx;

  /* Additional OpenType Tables */
  GPOSPtr gpos;
  GSUBPtr gsub;
}
TTFont, *TTFontPtr;

TTFontPtr ttfInitFont(char *filename);
void ttfLoadFont(TTFontPtr font, ULONG offset);
void ttfFreeFont(TTFontPtr font);

#include "loadtable.h"

#endif /* __TTF_H */


/* end of ttf.h */
