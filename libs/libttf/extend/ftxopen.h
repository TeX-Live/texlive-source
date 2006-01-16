/*******************************************************************
 *
 *  ftxopen.h
 *
 *    TrueType Open support.
 *
 *  Copyright 1996-2000 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *  This file should be included by the application.  Nevertheless,
 *  the table specific APIs (and structures) are located in files like
 *  ftxgsub.h or ftxgpos.h; these header files are read by ftxopen.h .
 *
 ******************************************************************/

#ifndef FTXOPEN_H
#define FTXOPEN_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TTO_MAX_NESTING_LEVEL  100

#define TTO_Err_Invalid_SubTable_Format   0x1000
#define TTO_Err_Invalid_SubTable          0x1001
#define TTO_Err_Not_Covered               0x1002
#define TTO_Err_Too_Many_Nested_Contexts  0x1003
#define TTO_Err_No_MM_Interpreter         0x1004


  /* Script list related structures */

  struct  TTO_LangSys_
  {
    TT_UShort   LookupOrderOffset;      /* always 0 for TT Open 1.0  */
    TT_UShort   ReqFeatureIndex;        /* required FeatureIndex     */
    TT_UShort   FeatureCount;           /* number of Feature indices */
    TT_UShort*  FeatureIndex;           /* array of Feature indices  */
  };

  typedef struct TTO_LangSys_  TTO_LangSys;


  struct  TTO_LangSysRecord_
  {
    TT_ULong     LangSysTag;            /* LangSysTag identifier */
    TTO_LangSys  LangSys;               /* LangSys table         */
  };

  typedef struct TTO_LangSysRecord_  TTO_LangSysRecord;


  struct  TTO_Script_
  {
    TTO_LangSys         DefaultLangSys; /* DefaultLangSys table     */
    TT_UShort           LangSysCount;   /* number of LangSysRecords */
    TTO_LangSysRecord*  LangSysRecord;  /* array of LangSysRecords  */
  };

  typedef struct TTO_Script_  TTO_Script;


  struct  TTO_ScriptRecord_
  {
    TT_ULong    ScriptTag;              /* ScriptTag identifier */
    TTO_Script  Script;                 /* Script table         */
  };

  typedef struct TTO_ScriptRecord_  TTO_ScriptRecord;


  struct  TTO_ScriptList_
  {
    TT_UShort          ScriptCount;     /* number of ScriptRecords */
    TTO_ScriptRecord*  ScriptRecord;    /* array of ScriptRecords  */
  };

  typedef struct TTO_ScriptList_  TTO_ScriptList;


  /* Feature list related structures */

  struct TTO_Feature_
  {
    TT_UShort   FeatureParams;          /* always 0 for TT Open 1.0     */
    TT_UShort   LookupListCount;        /* number of LookupList indices */
    TT_UShort*  LookupListIndex;        /* array of LookupList indices  */
  };

  typedef struct TTO_Feature_  TTO_Feature;


  struct  TTO_FeatureRecord_
  {
    TT_ULong     FeatureTag;            /* FeatureTag identifier */
    TTO_Feature  Feature;               /* Feature table         */
  };

  typedef struct TTO_FeatureRecord_  TTO_FeatureRecord;


  struct  TTO_FeatureList_
  {
    TT_UShort           FeatureCount;   /* number of FeatureRecords */
    TTO_FeatureRecord*  FeatureRecord;  /* array of FeatureRecords  */
  };

  typedef struct TTO_FeatureList_  TTO_FeatureList;


  /* Lookup list related structures */

  struct  TTO_SubTable_;                /* defined below after inclusion
                                           of ftxgsub.h and ftxgpos.h    */
  typedef struct TTO_SubTable_  TTO_SubTable;


  struct  TTO_Lookup_
  {
    TT_UShort      LookupType;          /* Lookup type         */
    TT_UShort      LookupFlag;          /* Lookup qualifiers   */
    TT_UShort      SubTableCount;       /* number of SubTables */
    TTO_SubTable*  SubTable;            /* array of SubTables  */
  };

  typedef struct TTO_Lookup_  TTO_Lookup;


  /* The `Properties' field is not defined in the TTO specification but
     is needed for processing lookups.  If properties[n] is > 0, the
     functions TT_GSUB_Apply_String() resp. TT_GPOS_Apply_String() will
     process Lookup[n] for glyphs which have the specific bit not set in
     the `properties' field of the input string object.                  */

  struct  TTO_LookupList_
  {
    TT_UShort    LookupCount;           /* number of Lookups       */
    TTO_Lookup*  Lookup;                /* array of Lookup records */
    TT_UShort*   Properties;            /* array of flags          */
  };

  typedef struct TTO_LookupList_  TTO_LookupList;


/* Possible LookupFlag bit masks.  `IGNORE_SPECIAL_MARKS' comes from the
   OpenType 1.2 specification.                                           */

#define IGNORE_BASE_GLYPHS    0x0002
#define IGNORE_LIGATURES      0x0004
#define IGNORE_MARKS          0x0008
#define IGNORE_SPECIAL_MARKS  0xFF00


  struct  TTO_CoverageFormat1_
  {
    TT_UShort   GlyphCount;             /* number of glyphs in GlyphArray */
    TT_UShort*  GlyphArray;             /* array of glyph IDs             */
  };

  typedef struct TTO_CoverageFormat1_  TTO_CoverageFormat1;


  struct TTO_RangeRecord_
  {
    TT_UShort  Start;                   /* first glyph ID in the range */
    TT_UShort  End;                     /* last glyph ID in the range  */
    TT_UShort  StartCoverageIndex;      /* coverage index of first
                                           glyph ID in the range       */
  };

  typedef struct TTO_RangeRecord_  TTO_RangeRecord;


  struct  TTO_CoverageFormat2_
  {
    TT_UShort         RangeCount;       /* number of RangeRecords */
    TTO_RangeRecord*  RangeRecord;      /* array of RangeRecords  */
  };

  typedef struct TTO_CoverageFormat2_  TTO_CoverageFormat2;


  struct  TTO_Coverage_
  {
    TT_UShort  CoverageFormat;          /* 1 or 2 */

    union
    {
      TTO_CoverageFormat1  cf1;
      TTO_CoverageFormat2  cf2;
    } cf;
  };

  typedef struct TTO_Coverage_  TTO_Coverage;


  struct  TTO_ClassDefFormat1_
  {
    TT_UShort   StartGlyph;             /* first glyph ID of the
                                           ClassValueArray             */
    TT_UShort   GlyphCount;             /* size of the ClassValueArray */
    TT_UShort*  ClassValueArray;        /* array of class values       */
  };

  typedef struct TTO_ClassDefFormat1_  TTO_ClassDefFormat1;


  struct  TTO_ClassRangeRecord_
  {
    TT_UShort  Start;                   /* first glyph ID in the range    */
    TT_UShort  End;                     /* last glyph ID in the range     */
    TT_UShort  Class;                   /* applied to all glyphs in range */
  };

  typedef struct TTO_ClassRangeRecord_  TTO_ClassRangeRecord;


  struct  TTO_ClassDefFormat2_
  {
    TT_UShort              ClassRangeCount;
                                        /* number of ClassRangeRecords */
    TTO_ClassRangeRecord*  ClassRangeRecord;
                                        /* array of ClassRangeRecords  */
  };

  typedef struct TTO_ClassDefFormat2_  TTO_ClassDefFormat2;


  /* The `Defined' field is not defined in the TTO specification but
     apparently needed for processing fonts like trado.ttf: This font
     refers to a class which contains not a single element.  We map such
     classes to class 0.                                                 */

  struct  TTO_ClassDefinition_
  {
    TT_Bool    loaded;

    TT_Bool*   Defined;                 /* array of Booleans.
                                           If Defined[n] is FALSE,
                                           class n contains no glyphs. */
    TT_UShort  ClassFormat;             /* 1 or 2                      */

    union
    {
      TTO_ClassDefFormat1  cd1;
      TTO_ClassDefFormat2  cd2;
    } cd;
  };

  typedef struct TTO_ClassDefinition_  TTO_ClassDefinition;


  struct TTO_Device_
  {
    TT_UShort   StartSize;              /* smallest size to correct      */
    TT_UShort   EndSize;                /* largest size to correct       */
    TT_UShort   DeltaFormat;            /* DeltaValue array data format:
                                           1, 2, or 3                    */
    TT_UShort*  DeltaValue;             /* array of compressed data      */
  };

  typedef struct TTO_Device_  TTO_Device;


#include "ftxgdef.h"
#include "ftxgsub.h"
#include "ftxgpos.h"


  struct  TTO_SubTable_
  {
    union
    {
      TTO_GSUB_SubTable  gsub;
      TTO_GPOS_SubTable  gpos;
    } st;
  };


  enum  TTO_Type_
  {
    GSUB,
    GPOS
  };

  typedef enum TTO_Type_  TTO_Type;


#ifdef __cplusplus
}
#endif

#endif /* FTXOPEN_H */


/* END */
