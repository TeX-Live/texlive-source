/* tables.h -- define data structures for various ttf file internal tables 
 * See Also: True Type Font Specification
 */

#ifndef __TTF_TABLES_H
#define __TTF_TABLES_H

/* $Id: tables.h,v 1.2 1998/07/06 06:07:01 werner Exp $ */

/* Offset Table:
 * Into the beginning of a True Type font file
 */
typedef struct
{
  Fixed version;
  USHORT numTables;
  USHORT searchRange;
  USHORT entrySelector;
  USHORT rangeShift;
}
OffsetTable, *OffsetTablePtr;

/* Table Directory:
 * The directory to find each table in a True Type font file
 */
typedef struct
{
  ULONG tag;
  ULONG checksum;
  ULONG offset;
  ULONG length;
}
TableDir, *TableDirPtr;


/* cmap: Character to Glyph Index Mapping Table
 * There are nine kinds of cmap, format 0, 2, 4, 6, 8, 10, 12,
 * 13, and 14.  They are defined as follows
 */
typedef struct
{
  USHORT format;  /* = 0 */
  USHORT length;
  USHORT version;
  BYTE glyphIndexArray[256];
}
CMAP0;

typedef struct
{
  USHORT firstCode;
  USHORT entryCount;
  SHORT idDelta;
  USHORT idRangeOffset;
}
SubHeader, *SubHeaderPtr;

typedef struct
{
  USHORT format;  /* = 2 */
  USHORT length;
  USHORT version;
  USHORT subHeaderKeys[256];
  SubHeaderPtr subHeaders;
  USHORT *glyphIndexArray;
}
CMAP2;

typedef struct
{
  USHORT format;  /* = 4 */
  USHORT length;
  USHORT version;
  USHORT segCountX2;
  USHORT searchRange;
  USHORT entrySelector;
  USHORT rangeShift;
  USHORT *endCount;
  USHORT reservedPad;
  USHORT *startCount;
  USHORT *idDelta;
  USHORT *idRangeOffset;
  USHORT *glyphIndexArray;
}
CMAP4;

typedef struct
{
  USHORT format;  /* = 6 */
  USHORT length;
  USHORT version;
  USHORT firstCode;
  USHORT entryCount;
  USHORT *glyphIndexArray;
}
CMAP6;

typedef struct
{
  ULONG startCharCode;
  ULONG endCharCode;
  ULONG startGlyphID;
}
CharGroup, *CharGroupPtr;

typedef struct
{
  USHORT format;  /* = 8 */
  ULONG length;
  ULONG version;
  BYTE is32[8192];
  ULONG nGroups;
  CharGroupPtr charGroup;       /* size = nGroups */
}
CMAP8;

typedef struct
{
  USHORT format;  /* = 10 */
  ULONG length;
  ULONG version;
  ULONG startCharCode;
  ULONG numChars;
  USHORT *glyphs;
}
CMAP10;

typedef struct
{
  USHORT format;  /* = 12 */
  ULONG length;
  ULONG version;
  ULONG nGroups;
  CharGroupPtr charGroup;       /* size = nGroups */
}
CMAP12;

typedef struct
{
  USHORT format;  /* = 13 */
  ULONG length;
  ULONG version;
  ULONG nGroups;
  CharGroupPtr charGroup;       /* size = nGroups */
}
CMAP13;

typedef struct
{
  int notYet;
}
VarSelRec, *VarSelRecPtr;

typedef struct
{
  USHORT format;  /* = 14 */
  ULONG length;
  ULONG numVarSelRec;
  VarSelRecPtr varSelRec;       /* size = numVarSelRec */
}
CMAP14;

typedef union
{
  USHORT *format;
  CMAP0 *cmap0;
  CMAP2 *cmap2;
  CMAP4 *cmap4;
  CMAP6 *cmap6;
  CMAP8 *cmap8;
  CMAP10 *cmap10;
  CMAP12 *cmap12;
  CMAP13 *cmap13;
  CMAP14 *cmap14;
}
MapPtr;

/* Encoding: one for each encoding scheme */
typedef struct
{
  /* encoding table */
  USHORT PlatformID;
  USHORT EncodingID;
  ULONG offset;
  USHORT mapindex;
  MapPtr map;
}
Encoding, *EncodingPtr;

typedef struct
{
  ULONG offset;
  MapPtr map;
}
SubTable, *SubTablePtr;

typedef struct
{
  USHORT version;
  USHORT numberOfEncodings;
  USHORT numberOfMaps;
  EncodingPtr encoding;         /* size = numberOfEncodings */
  SubTablePtr subTable;         /* size = numberOfMaps */
}
CMAP, *CMAPPtr;

/* glyf: Glyph Data table 
 * It is necessary to load "loca" table first to know where to load a specific
 * glyph; the "loca" table itself depends on "head" and "maxp" tables
 */
typedef F2Dot14 SCALE;

typedef struct
{
  F2Dot14 xscale;
  F2Dot14 yscale;
}
VECTOR;

typedef struct
{
  F2Dot14 xscale;
  F2Dot14 scale01;
  F2Dot14 scale10;
  F2Dot14 yscale;
}
TENSOR;

typedef struct
{
  SHORT args[2];                /* need reconsideration */
  union
  {
    SCALE scale;
    VECTOR vector;
    TENSOR tensor;
  }
  transform;
}
CompositeData;

typedef struct _component
{
  USHORT flags;
  USHORT glyphIndex;
  CompositeData data;
  struct _component *next, *previous;   /* linked list used, no way to know 
                                         * how many components in advance */
}
Component;

typedef struct
{
  SHORT numberOfContours;
  FWord xMin;
  FWord yMin;
  FWord xMax;
  FWord yMax;
  /* simple glyph data */
  USHORT *endPtsOfContours;     /* size = numberOfContours */
  USHORT instructionLength;
  BYTE *instructions;           /* size = instructionLength */
  BYTE *flags;                  /* size = the total # of x,y 
                                 * coordinates, i.e. last number
                                 * of endPtsOfContour */
  SHORT *xCoordinates;
  SHORT *yCoordinates;
  Component *comp;              /* a pointer to a linked list of 
                                 * composite components */
}
GLYF, *GLYFPtr;

/* Flags for Coordinates */
#define FLAGS_ON_CURVE       1
#define FLAGS_X_SHORT_VECTOR 2
#define FLAGS_Y_SHORT_VECTOR 4
#define FLAGS_REPEAT         8
#define FLAGS_X_SAME      0x10
#define FLAGS_Y_SAME      0x20

/* Flags for Composite Glyph */
#define ARG_1_AND_2_ARE_WORDS    0x001
#define ARGS_ARE_XY_VALUES       0x002
#define ROUND_XY_TO_GRID         0x004
#define WE_HAVE_A_SCALE          0x008
#define RESERVE                  0x010
#define NO_OVERLAP               0x010
#define MORE_COMPONENT           0x020
#define WE_HAVE_AN_X_AND_Y_SCALE 0x040
#define WE_HAVE_A_TWO_BY_TWO     0x080
#define WE_HAVE_INSTRUCTIONS     0x100
#define USE_MY_METRICS           0x200
#define OVERLAP_COMPOUND         0x400  /* from Apple's TTF specs */

typedef struct
{
  Fixed version;
  Fixed fontRevision;
  ULONG checkSumAdj;
  ULONG magicNumber;
  USHORT flags;
  USHORT unitsPerEm;
  ULONG created[2];
  ULONG modified[2];
  FWord xMin;
  FWord yMin;
  FWord xMax;
  FWord yMax;
  USHORT macStyle;
  USHORT lowestRecPPEM;
  SHORT fontDirectionHint;
  SHORT indexToLocFormat;       /* 0 => ushort, 1 => ulong */
  SHORT glyphDataFormat;
}
HEAD, *HEADPtr;

#define FLAGS_Y_0 1
#define FLAGS_X_0 2
#define FLAGS_SIZE_DEP 4
#define FLAGS_INT_PPEM 8
#define FLAGS_ALT_WIDTH 0x10
#define MAC_STYLE_BOLD 1
#define MAC_STYLE_ITALIC 2
#define FONT_DIR_MIX 0
#define FONT_DIR_L2R 1
#define FONT_DIR_L2R_NEUTRALS 2
#define FONT_DIR_R2L -1
#define FONT_DIR_R2L_NEUTRALS -2
#define LOCA_OFFSET_SHORT 0
#define LOCA_OFFSET_LONG 1

typedef struct
{
  Fixed version;
  FWord Ascender;
  FWord Descender;
  FWord LineGap;
  uFWord advanceWidthMax;
  FWord minLeftSideBearing;
  FWord minRightSideBearing;
  FWord xMaxExtent;
  SHORT caretSlopeRise;
  SHORT caretSlopeRun;
  SHORT reserved[5];
  SHORT metricDataFormat;
  USHORT numberOfHMetrics;
}
HHEA, *HHEAPtr;

/* for a new created HMTX, one have to fill numberofHMetrics manually for
 * hhea */
typedef struct
{
  uFWord advanceWidth;
  FWord lsb;
}
longHorMetric;

typedef struct
{                               /* depends on hhea */
  USHORT numberOfHMetrics;      /* defined in hhea */
  USHORT numberOfLSB;           /* numGlyph - numberOfHMetrics */
  longHorMetric *hMetrics;
  FWord *leftSideBearing;
}
HMTX, *HMTXPtr;

/* for a newly created LOCA, one have to fill indexToLocFormat from head and
 * numGlyphs from maxp manually */
typedef struct
{                               /* depends on head and maxp */
  SHORT indexToLocFormat;       /* defined in head */
  USHORT numGlyphs;             /* defined in maxp */
  ULONG *offset;
}
LOCA, *LOCAPtr;

typedef struct
{
  Fixed version;
  USHORT numGlyphs;
  USHORT maxPoints;
  USHORT maxContours;
  USHORT maxCompositePoints;
  USHORT maxCompositeContours;
  USHORT maxZones;
  USHORT maxTwilightPoints;
  USHORT maxStorage;
  USHORT maxFunctionDefs;
  USHORT maxInstructionDefs;
  USHORT maxStackElements;
  USHORT maxSizeOfInstructions;
  USHORT maxComponentElements;
  USHORT maxComponentDepth;
}
MAXP, *MAXPPtr;

typedef struct
{
  USHORT PlatformID;
  USHORT EncodingID;
  USHORT LanguageID;
  USHORT NameID;
  USHORT length;
  USHORT offset;
  char *data;
}
NameRecord, *NameRecordPtr;

typedef struct
{
  USHORT format;
  USHORT numberOfRecords;
  USHORT offset;
  NameRecordPtr NameRecords;
}
NAME, *NAMEPtr;

typedef struct
{
  USHORT numGlyphs;     /* Should be the same as numGlyphs defined in maxp */
  USHORT *glyphNameIndex;
  CHAR **GlyphName;
}
Format20;

typedef struct
{
  char *offset;
}
Format25;

typedef struct
{
  Fixed format;
  Fixed italicAngle;
  FWord underlinePosition;
  FWord underlineThickness;
  ULONG isFixedPitch;
  ULONG minMemType42;
  ULONG maxMemType42;
  ULONG minMemType1;
  ULONG maxMemType1;
  /* the elements below exist only for 2.0 and 2.5 format */
  union
  {
    Format20 *format20;
    Format25 *format25;
  }
  name;
}
POST, *POSTPtr;

typedef struct
{
  USHORT version;
  SHORT xAvgCharWidth;
  USHORT usWeightClass;
  USHORT usWidthClass;
  USHORT fsType;
  SHORT ySubscriptXSize;
  SHORT ySubscriptYSize;
  SHORT ySubscriptXOffset;
  SHORT ySubscriptYOffset;
  SHORT ySuperscriptXSize;
  SHORT ySuperscriptYSize;
  SHORT ySuperscriptXOffset;
  SHORT ySuperscriptYOffset;
  SHORT yStrikeoutSize;
  SHORT yStrikeoutPosition;
  SHORT sFamilyClass;
  CHAR panose[10];
  ULONG ulUnicodeRange1;
  ULONG ulUnicodeRange2;
  ULONG ulUnicodeRange3;
  ULONG ulUnicodeRange4;
  CHAR achVendID[5];
  USHORT fsSelection;
  USHORT usFirstCharIndex;
  USHORT usLastCharIndex;
  SHORT sTypoAscender;
  SHORT sTypoDescender;
  SHORT sTypoLineGap;
  USHORT usWinAscent;
  USHORT usWinDescent;
  /* only version 1 tables */
  ULONG ulCodePageRange1;
  ULONG ulCodePageRange2;
  /* only version 2 tables */
  SHORT sxHeight;
  SHORT sCapHeight;
  USHORT usDefaultChar;
  USHORT usBreakChar;
  USHORT usMaxContext;
}
OS_2, *OS_2Ptr;

typedef struct
{
  USHORT rangeMaxPPEM;
  USHORT rangeGaspBehavior;
}
GASPRANGE;

typedef struct
{
  USHORT version;
  USHORT numRanges;
  GASPRANGE *gaspRange;         /* length = numRanges */
}
GASP, *GASPPtr;

/* flags for gasp */
#define GASP_GRIDFIT 0x0001
#define GASP_DOGRAY  0x0002

typedef struct
{
  BYTE PixelSize;
  BYTE MaxWidth;
  BYTE *Width;                  /* length = numGlyphs */
}
DeviceRecord;

typedef struct
{
  USHORT numGlyphs;             /* defined in maxp, artifitial */
  USHORT version;
  SHORT numDevices;
  LONG size;
  DeviceRecord *Records;
}
HDMX, *HDMXPtr;

typedef struct
{
  USHORT version;
  USHORT length;
  USHORT coverage;
  union
  {
    struct
    {
      USHORT nPairs;
      USHORT searchRange;
      USHORT entrySelector;
      USHORT rangeShift;
      struct kernpair
      {
        USHORT left;
        USHORT right;
        FWord value;
      }
      *pairs;
    }
    kern0;

    struct
    {
      USHORT rowWidth;
      USHORT leftClassTable;
      USHORT rightClassTable;
      USHORT array;
    }
    kern2;
  }
  kern;
}
KernSubtable;

typedef struct
{
  USHORT version;
  USHORT nTables;
  KernSubtable *subtable;
}
KERN, *KERNPtr;

typedef struct
{
  USHORT version;
  USHORT numGlyphs;
  BYTE *yPels;                  /* length = numGlyphs */
}
LTSH, *LTSHPtr;

typedef struct
{
  Fixed version;
  ULONG FontNumber;
  USHORT Pitch;
  USHORT xHeight;
  USHORT Style;
  USHORT TypeFamily;
  USHORT CapHeight;
  USHORT SymbolSet;
  CHAR Typeface[16];
  CHAR CharacterComplement[8];
  CHAR FileName[6];
  CHAR StrokeWeight;
  CHAR WidthType;
  BYTE SerifStyle;
  BYTE reserved;
}
PCLT, *PCLTPtr;

typedef struct
{
  BYTE CharSet;
  BYTE xRatio;
  BYTE yStartRatio;
  BYTE yEndRatio;
}
Ratios;

typedef struct
{
  USHORT yPelHeight;
  SHORT yMax;
  SHORT yMin;
}
vTable;

typedef struct
{
  USHORT recs;
  BYTE startsz;
  BYTE endsz;
  vTable *entry;
}
Vdmx;

typedef struct
{
  USHORT version;
  USHORT numRecs;
  USHORT numRatios;
  Ratios *ratRange;             /* length = numRatios */
  USHORT *offset;               /* length = numRatios */
  Vdmx *groups;
}
VDMX, *VDMXPtr;

typedef struct
{
  Fixed version;
  SHORT ascent;
  SHORT descent;
  SHORT lineGap;
  SHORT advanceHeightMax;
  SHORT minTopSideBearing;
  SHORT minBottomSideBearing;
  SHORT yMaxExtent;
  SHORT caretSlopeRise;
  SHORT caretSlopeRun;
  SHORT caretOffset;
  SHORT reserved[4];
  SHORT metricDataFormat;
  USHORT numOfLongVerMetrics;
}
VHEA, *VHEAPtr;

typedef struct
{
  uFWord advanceHeight;
  FWord topSideBearing;
}
longVerMetric;

typedef struct
{
  /* depends on vhea */
  USHORT numOfLongVerMetrics;   /* defined in vhea */
  USHORT numOfTSB;              /* numGlyph - numberOfHMetrics */
  longVerMetric *vMetrics;
  FWord *topSideBearing;
}
VMTX, *VMTXPtr;

/* Additional OpenType Tables */
#include "otftables.h"

#endif /* __TTF_TABLES_H */


/* end of tables.h */
