/* gpos.h -- define data structures for Glyph Positioning Table
 */

#ifndef __TTF_GPOS_H
#define __TTF_GPOS_H

/* ValueFormat Bits */
#define ValueFormat_XPlacement 0x0001 << 0
#define ValueFormat_YPlacement 0x0001 << 1
#define ValueFormat_XAdvance   0x0001 << 2
#define ValueFormat_YAdvance   0x0001 << 3
#define ValueFormat_AllDesign  0x000f
#define ValueFormat_XPlaDevice 0x0010 << 0
#define ValueFormat_YPlaDevice 0x0010 << 1
#define ValueFormat_XAdvDevice 0x0010 << 2
#define ValueFormat_YAdvDevice 0x0010 << 3
#define ValueFormat_AllDevice  0x00f0
#define ValueFormat_Reserved   0xff00

/* Positioning LookupType Values */
#define PosLookup_SingleAdjustment          1
#define PosLookup_PairAdjustment            2
#define PosLookup_CursiveAttachment         3
#define PosLookup_MarkToBaseAttachment      4
#define PosLookup_MarkToLigatureAttachment  5
#define PosLookup_MarkToMarkAttachment      6
#define PosLookup_ContextPositioning        7
#define PosLookup_ChainedContextPositioning 8
#define PosLookup_ExtensionPositioning      9
#define PosLookup_Max                       9

typedef struct
{
  SHORT valDesign[4];
  union
  {
    USHORT offset;
    DevicePtr device;
  }
  valDevice[4];
}
ValueRecord, *ValueRecordPtr;

typedef struct
{
  USHORT anchorFormat;  /* = 1 */
  SHORT xCoordinate;
  SHORT yCoordinate;
}
Anchor1, *Anchor1Ptr;

typedef struct
{
  USHORT anchorFormat;  /* = 2 */
  SHORT xCoordinate;
  SHORT yCoordinate;
  USHORT anchorPoint;
}
Anchor2, *Anchor2Ptr;

typedef struct
{
  USHORT anchorFormat;  /* = 3 */
  SHORT xCoordinate;
  SHORT yCoordinate;
  DevicePtr xDevice;
  DevicePtr yDevice;
}
Anchor3, *Anchor3Ptr;

typedef union
{
  Anchor1Ptr anchor1;
  Anchor2Ptr anchor2;
  Anchor3Ptr anchor3;
}
AnchorPtr;

typedef struct
{
  USHORT class;
  AnchorPtr markAnchor;
}
MarkRecord, *MarkRecordPtr;

typedef struct
{
  USHORT lookupType;    /* = 1 */
  USHORT lookupFormat;	/* = 1 */
  CoveragePtr coverage;
  USHORT valueFormat;
  ValueRecordPtr value;
}
Pos11, *Pos11Ptr;

typedef struct
{
  USHORT lookupType;    /* = 1 */
  USHORT lookupFormat;	/* = 2 */
  CoveragePtr coverage;
  USHORT valueFormat;
  USHORT valueCount;
  ValueRecordPtr *value;        /* length = valueCount */
}
Pos12, *Pos12Ptr;

typedef struct
{
  USHORT secondGlyph;
  ValueRecordPtr value1;
  ValueRecordPtr value2;
}
PairValueRecord, *PairValueRecordPtr;

typedef struct
{
  USHORT pairValueCount;
  PairValueRecordPtr pairValue; /* length = pairValueCount */
}
PairSet, *PairSetPtr;

typedef struct
{
  USHORT lookupType;    /* = 2 */
  USHORT lookupFormat;	/* = 1 */
  CoveragePtr coverage;
  USHORT valueFormat1;
  USHORT valueFormat2;
  USHORT pairSetCount;
  PairSetPtr pairSet;           /* length = pairSetCount */
}
Pos21, *Pos21Ptr;

typedef struct
{
  USHORT lookupType;    /* = 2 */
  USHORT lookupFormat;	/* = 2 */
  CoveragePtr coverage;
  USHORT valueFormat1;
  USHORT valueFormat2;
  ClassDefPtr classDef1;
  ClassDefPtr classDef2;
  USHORT class1Count;
  USHORT class2Count;
  ValueRecordPtr *values;       /* length = 2 * class1Count * class2Count */
}
Pos22, *Pos22Ptr;

typedef struct
{
  USHORT lookupType;    /* = 3 */
  USHORT lookupFormat;	/* = 1 */
  CoveragePtr coverage;
  USHORT entryExitCount;
  AnchorPtr *entryExit;         /* length = 2 * entryExitCount */
}
Pos31, *Pos31Ptr;

typedef struct
{
  USHORT lookupType;    /* = 4 */
  USHORT lookupFormat;	/* = 1 */
  CoveragePtr markCoverage;
  CoveragePtr baseCoverage;
  USHORT markCount;
  USHORT baseCount;
  USHORT classCount;
  MarkRecordPtr markArray;      /* length = markCount */
  AnchorPtr *baseArray;         /* length = baseCount * classCount */
}
Pos41, *Pos41Ptr;

typedef struct
{
  USHORT componentCount;
  AnchorPtr *componentRecord;   /* length = componentCount * classCount */
}
LigatureAttach, *LigatureAttachPtr;

typedef struct
{
  USHORT lookupType;    /* = 5 */
  USHORT lookupFormat;	/* = 1 */
  CoveragePtr markCoverage;
  CoveragePtr ligatureCoverage;
  USHORT markCount;
  USHORT ligatureCount;
  USHORT classCount;
  MarkRecordPtr markArray;      /* length = markCount */
  LigatureAttachPtr ligatureArray; /* length = ligatureCount */
}
Pos51, *Pos51Ptr;

typedef struct
{
  USHORT lookupType;    /* = 6 */
  USHORT lookupFormat;	/* = 1 */
   CoveragePtr mark1Coverage;
  CoveragePtr mark2Coverage;
  USHORT mark1Count;
  USHORT mark2Count;
  USHORT classCount;
  MarkRecordPtr mark1Array;     /* length = mark1Count */
  AnchorPtr *mark2Array;        /* length = mark2Count * classCount */
}
Pos61, *Pos61Ptr;

#define Pos71Ptr OtfCtx1Ptr
#define Pos72Ptr OtfCtx2Ptr
#define Pos73Ptr OtfCtx3Ptr
#define Pos81Ptr OtfChn1Ptr
#define Pos82Ptr OtfChn2Ptr
#define Pos83Ptr OtfChn3Ptr

typedef union
{
  Pos11Ptr pos11;
  Pos12Ptr pos12;
  Pos21Ptr pos21;
  Pos22Ptr pos22;
  Pos31Ptr pos31;
  Pos41Ptr pos41;
  Pos51Ptr pos51;
  Pos61Ptr pos61;
  Pos71Ptr pos71;
  Pos72Ptr pos72;
  Pos73Ptr pos73;
  Pos81Ptr pos81;
  Pos82Ptr pos82;
  Pos83Ptr pos83;
}
PosLookupPtr;

#endif /* __TTF_GPOS_H */
