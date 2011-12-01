/* otftables.h -- define data structures for various OpenType Tables 
 * See Also: OpenType Specification
 */

#ifndef __TTF_OTFTABLES_H
#define __TTF_OTFTABLES_H

typedef struct
{
  USHORT reqFeatureIndex;
  USHORT featureCount;
  USHORT *featureIndex;         /* length = featureCount */
}
LangSys, *LangSysPtr;

typedef struct
{
  ULONG tag;
  LangSysPtr langSys;
}
LangSysRecord, *LangSysRecordPtr;

typedef struct
{
  ULONG tag;
  LangSysPtr defaultLangSys;
  USHORT langSysCount;
  LangSysRecord *langSysRecord; /* length = langSysCount */
}
ScriptRecord, *ScriptRecordPtr;

typedef struct
{
  USHORT scriptCount;
  ScriptRecord *scriptRecord;   /* length = scriptCount */
}
ScriptList, *ScriptListPtr;

typedef struct
{
  ULONG tag;
  USHORT featureParams;
  USHORT lookupCount;
  USHORT *lookupListIndex;      /* length = lookupCount */
}
FeatureRecord, *FeatureRecordPtr;

typedef struct
{
  USHORT featureCount;
  FeatureRecord *featureRecord; /* length = featureCount */
}
FeatureList, *FeatureListPtr;

typedef struct
{
  USHORT start;
  USHORT end;
  USHORT startCoverageIndex;
}
RangeRecord, *RangeRecordPtr;

typedef struct
{
  USHORT coverageFormat;  /* = 1 */
  USHORT glyphCount;
  USHORT *glyphArray;           /* length = glyphCount */
}
Coverage1, *Coverage1Ptr;

typedef struct
{
  USHORT coverageFormat;  /* = 2 */
  USHORT rangeCount;
  RangeRecordPtr rangeRecord;   /* length = rangeCount */
}
Coverage2, *Coverage2Ptr;

typedef union
{
  USHORT *format;
  Coverage1Ptr coverage1;
  Coverage2Ptr coverage2;
}
CoveragePtr;

typedef struct
{
  USHORT start;
  USHORT end;
  USHORT classValue;
}
ClassRangeRecord, *ClassRangeRecordPtr;

typedef struct
{
  USHORT classFormat;  /* = 1 */
  USHORT startGlyph;
  USHORT glyphCount;
  USHORT *classValueArray;      /* length = glyphCount */
}
ClassDef1, *ClassDef1Ptr;

typedef struct
{
  USHORT classFormat;  /* = 2 */
  USHORT classRangeCount;
  ClassRangeRecordPtr classRangeRecord;  /* length = classRangeCount */
}
ClassDef2, *ClassDef2Ptr;

typedef union
{
  USHORT *format;
  ClassDef1Ptr classDef1;
  ClassDef2Ptr classDef2;
}
ClassDefPtr;

typedef struct
{
  USHORT startSize;
  USHORT endSize;
  USHORT deltaFormat;  /* = 1, 2, or 3 */
  USHORT deltaValue[1]; /* variable size
                         * (endSize-startSize+1) entries
                         * 8, 4, or 2 entries per array element */
}
Device, *DevicePtr;

typedef struct
{
  USHORT sequenceIndex;
  USHORT lookupListIndex;
}
OtfLookupRecord, *OtfLookupRecordPtr;

typedef struct
{
  USHORT glyphCount;
  USHORT otfCount;
  USHORT *input;                /* length = glyphCount - 1 */
  OtfLookupRecordPtr otf;       /* length = otfCount */
}
OtfRule, *OtfRulePtr;

typedef struct
{
  USHORT otfRuleCount;
  OtfRulePtr otfRule;           /* length = otfRuleCount */
}
OtfRuleSet, *OtfRuleSetPtr;

typedef struct
{
  USHORT lookupType;    /* = 7 (GPOS) or 5 (GSUB) */
  USHORT lookupFormat;	/* = 1 */
  CoveragePtr coverage;
  USHORT otfRuleSetCount;
  OtfRuleSetPtr otfRuleSet;     /* length = otfRuleSetCount */
}
OtfCtx1, *OtfCtx1Ptr;

OtfCtx1Ptr makeOTFCtx1 (FILE *fp, ULONG offset);
void printOTFCtx1 (FILE *fp, OtfCtx1Ptr otf);
void freeOTFCtx1 (OtfCtx1Ptr otf);

typedef struct
{
  USHORT glyphCount;
  USHORT otfCount;
  USHORT *class;                /* length = glyphCount - 1 */
  OtfLookupRecordPtr otf;       /* length = otfCount */
}
OtfClassRule, *OtfClassRulePtr;

typedef struct
{
  USHORT otfClassRuleCnt;
  OtfClassRulePtr otfClassRule; /* length = otfClassRuleCnt */
}
OtfClassSet, *OtfClassSetPtr;

typedef struct
{
  USHORT lookupType;    /* = 7 (GPOS) or 5 (GSUB) */
  USHORT lookupFormat;	/* = 2 */
  CoveragePtr coverage;
  ClassDefPtr classDef;
  USHORT otfClassSetCnt;
  OtfClassSetPtr otfClassSet;   /* length = otfClassSetCnt */
}
OtfCtx2, *OtfCtx2Ptr;

OtfCtx2Ptr makeOTFCtx2 (FILE *fp, ULONG offset);
void printOTFCtx2 (FILE *fp, OtfCtx2Ptr otf);
void freeOTFCtx2 (OtfCtx2Ptr otf);

typedef struct
{
  USHORT lookupType;    /* = 7 (GPOS) or 5 (GSUB) */
  USHORT lookupFormat;	/* = 3 */
  USHORT glyphCount;
  USHORT otfCount;
  CoveragePtr *glyphs;          /* length = glyphCount */
  OtfLookupRecordPtr otf;       /* length = otfCount */
}
OtfCtx3, *OtfCtx3Ptr;

OtfCtx3Ptr makeOTFCtx3 (FILE *fp, ULONG offset);
void printOTFCtx3 (FILE *fp, OtfCtx3Ptr otf);
void freeOTFCtx3 (OtfCtx3Ptr otf);

typedef struct
{
  USHORT backtrackGlyphCount;
  USHORT *backtrack;            /* length = backtrackGlyphCount */
  USHORT inputGlyphCount;
  USHORT *input;                /* length = inputGlyphCount - 1 */
  USHORT lookaheadGlyphCount;
  USHORT *lookahead;            /* length = lookaheadGlyphCount */
  USHORT otfCount;
  OtfLookupRecordPtr otf;       /* length = otfCount */
}
ChainOtfRule, *ChainOtfRulePtr;

typedef struct
{
  USHORT chainOtfRuleCount;
  ChainOtfRulePtr chainOtfRule; /* length = chainOtfRuleCount */
}
ChainOtfRuleSet, *ChainOtfRuleSetPtr;

typedef struct
{
  USHORT lookupType;    /* = 8 (GPOS) or 6 (GSUB) */
  USHORT lookupFormat;	/* = 1 */
  CoveragePtr coverage;
  USHORT chainOtfRuleSetCount;
  ChainOtfRuleSetPtr chainOtfRuleSet; /* length = chainOtfRuleSetCount */
}
OtfChn1, *OtfChn1Ptr;

OtfChn1Ptr makeOTFChn1 (FILE *fp, ULONG offset);
void printOTFChn1 (FILE *fp, OtfChn1Ptr otf);
void freeOTFChn1 (OtfChn1Ptr otf);

typedef struct
{
  USHORT backtrackGlyphCount;
  USHORT *backtrack;            /* length = backtrackGlyphCount */
  USHORT inputGlyphCount;
  USHORT *input;                /* length = inputGlyphCount - 1 */
  USHORT lookaheadGlyphCount;
  USHORT *lookahead;            /* length = lookaheadGlyphCount */
  USHORT otfCount;
  OtfLookupRecordPtr otf;       /* length = otfCount */
}
ChainOtfClassRule, *ChainOtfClassRulePtr;

typedef struct
{
  USHORT chainOtfClassRuleCnt;
  ChainOtfClassRulePtr chainOtfClassRule; /* length = chainOtfClassRuleCnt */
}
ChainOtfClassSet, *ChainOtfClassSetPtr;

typedef struct
{
  USHORT lookupType;    /* = 8 (GPOS) or 6 (GSUB) */
  USHORT lookupFormat;	/* = 2 */
  CoveragePtr coverage;
  ClassDefPtr backtrackClassDef;
  ClassDefPtr inputClassDef;
  ClassDefPtr lookaheadClassDef;
  USHORT chainOtfClassSetCnt;
  ChainOtfClassSetPtr chainOtfClassSet; /* length = chainOtfClassSetCnt */
}
OtfChn2, *OtfChn2Ptr;

OtfChn2Ptr makeOTFChn2 (FILE *fp, ULONG offset);
void printOTFChn2 (FILE *fp, OtfChn2Ptr otf);
void freeOTFChn2 (OtfChn2Ptr otf);

typedef struct
{
  USHORT lookupType;    /* = 8 (GPOS) or 6 (GSUB) */
  USHORT lookupFormat;	/* = 3 */
  USHORT backtrackGlyphCount;
  CoveragePtr *backtrack;       /* length = backtrackGlyphCount */
  USHORT inputGlyphCount;
  CoveragePtr *input;           /* length = inputGlyphCount */
  USHORT lookaheadGlyphCount;
  CoveragePtr *lookahead;       /* length = lookaheadGlyphCount */
  USHORT otfCount;
  OtfLookupRecordPtr otf;       /* length = otfCount */
}
OtfChn3, *OtfChn3Ptr;

OtfChn3Ptr makeOTFChn3 (FILE *fp, ULONG offset);
void printOTFChn3 (FILE *fp, OtfChn3Ptr otf);
void freeOTFChn3 (OtfChn3Ptr otf);

#include "gpos.h"
#include "gsub.h"

typedef struct
{
  USHORT lookupType;
  USHORT lookupFormat;
}
OtfLookup, *OtfLookupPtr;

typedef union
{
  OtfLookupPtr otf;
  PosLookupPtr pos;
  SubLookupPtr sub;
}
LookupPtr;

typedef struct
{
  USHORT lookupFlag;
  USHORT subTableCount;
  USHORT markFilteringSet;
  LookupPtr *lookup;            /* length = subTableCount */
} LookupRecord, *LookupRecordPtr;

/* LookupFlag Bits */
#define LookupFlag_RightToLeft         0x0001
#define lookupFlag_IgnoreBaseGlyphs    0x0002
#define lookupFlag_IgnoreLigatures     0x0004
#define lookupFlag_IgnoreMarks         0x0008
#define lookupFlag_UseMarkFilteringSet 0x0010
#define lookupFlag_Reserved            0x00e0
#define lookupFlag_MarkAttachmentType  0xff00

typedef struct
{
  USHORT lookupCount;
  LookupRecord *lookupRecord;
}
LookupList, *LookupListPtr;

typedef LookupPtr (MakeLookupFunc) (FILE *fp, USHORT lookupType, ULONG offset);
typedef void (PrintLookupFunc) (FILE *fp, LookupPtr lookup);
typedef void (FreeLookupFunc) (LookupPtr lookup);

typedef struct
{
  Fixed version;
  ScriptListPtr scriptList;
  FeatureListPtr featureList;
  LookupListPtr lookupList;
}
GPOS, *GPOSPtr;

typedef struct
{
  Fixed version;
  ScriptListPtr scriptList;
  FeatureListPtr featureList;
  LookupListPtr lookupList;
}
GSUB, *GSUBPtr;

#endif /* __TTF_OTFTABLES_H */
