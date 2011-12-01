/* gsub.h -- define data structures for Glyph Substitution Table
 */

#ifndef __TTF_GSUB_H
#define __TTF_GSUB_H

/* Substitution LookupType Values */
#define SubLookup_Single                       1
#define SubLookup_Multiple                     2
#define SubLookup_Alternate                    3
#define SubLookup_Ligature                     4
#define SubLookup_Context                      5
#define SubLookup_ChainingContext              6
#define SubLookup_ExtensionSubstitution        7
#define SubLookup_ReverseChainingContextSingle 8
#define SubLookup_Max                          8

typedef struct
{
  USHORT lookupType;    /* = 1 */
  USHORT lookupFormat;  /* = 1 */
  CoveragePtr coverage;
  SHORT deltaGlyphID; 
}
Sub11, *Sub11Ptr;

typedef struct
{
  USHORT lookupType;    /* = 1 */
  USHORT lookupFormat;  /* = 2 */
  CoveragePtr coverage;
  USHORT glyphCount;
  USHORT *substitute;           /* length = glyphCount */
}
Sub12, *Sub12Ptr;

typedef struct
{
  USHORT glyphCount;
  USHORT *substitute;           /* length = glyphCount */
}
Sequence, *SequencePtr;

typedef struct
{
  USHORT lookupType;    /* = 2 */
  USHORT lookupFormat;  /* = 1 */
  CoveragePtr coverage;
  USHORT sequenceCount;
  SequencePtr sequence;         /* length = sequenceCount */
}
Sub21, *Sub21Ptr;

typedef struct
{
  USHORT glyphCount;
  USHORT *alternate;            /* length = glyphCount */
}
AlternateSet, *AlternateSetPtr;

typedef struct
{
  USHORT lookupType;    /* = 3 */
  USHORT lookupFormat;  /* = 1 */
  CoveragePtr coverage;
  USHORT alternateSetCount;
  AlternateSetPtr alternateSet; /* length = alternateSetCount */
}
Sub31, *Sub31Ptr;

typedef struct
{
  USHORT ligGlyph;
  USHORT compCount;
  USHORT *component;            /* length = compCount - 1 */
}
Ligature, *LigaturePtr;

typedef struct
{
  USHORT ligatureCount;
  LigaturePtr ligature;         /* length = ligatureCount */
}
LigatureSet, *LigatureSetPtr;

typedef struct
{
  USHORT lookupType;    /* = 4 */
  USHORT lookupFormat;  /* = 1 */
  CoveragePtr coverage;
  USHORT ligSetCount;
  LigatureSetPtr ligatureSet;   /* length = ligSetCount */
}
Sub41, *Sub41Ptr;

#define Sub51Ptr OtfCtx1Ptr
#define Sub52Ptr OtfCtx2Ptr
#define Sub53Ptr OtfCtx3Ptr
#define Sub61Ptr OtfChn1Ptr
#define Sub62Ptr OtfChn2Ptr
#define Sub63Ptr OtfChn3Ptr

typedef struct
{
  USHORT lookupType;    /* = 8 */
  USHORT lookupFormat;  /* = 1 */
  CoveragePtr coverage;
  USHORT backtrackGlyphCount;
  CoveragePtr *backtrack;       /* length = backtrackGlyphCount */
  USHORT lookaheadGlyphCount;
  CoveragePtr *lookahead;       /* length = lookaheadGlyphCount */
  USHORT glyphCount;
  USHORT *substitute;           /* length = glyphCount */
}
Sub81, *Sub81Ptr;

typedef union
{
  Sub11Ptr sub11;
  Sub12Ptr sub12;
  Sub21Ptr sub21;
  Sub31Ptr sub31;
  Sub41Ptr sub41;
  Sub51Ptr sub51;
  Sub52Ptr sub52;
  Sub53Ptr sub53;
  Sub61Ptr sub61;
  Sub62Ptr sub62;
  Sub63Ptr sub63;
  Sub81Ptr sub81;
}
SubLookupPtr;

#endif /* __TTF_SUB_H */
