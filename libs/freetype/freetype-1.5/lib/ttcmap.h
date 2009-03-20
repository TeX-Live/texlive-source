/*******************************************************************
 *
 *  ttcmap.h
 *
 *    TrueType Character Mappings
 *
 *  Copyright 1996-2002 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *
 ******************************************************************/

#ifndef TTCMAP_H
#define TTCMAP_H

#include "ttconfig.h"
#include "tttypes.h"


#ifdef __cplusplus
  extern "C" {
#endif

  /* format 0 */

  struct  TCMap0_
  {
    PByte  glyphIdArray;
  };

  typedef struct TCMap0_  TCMap0;
  typedef TCMap0*         PCMap0;


  /* format 2 */

  struct  TCMap2SubHeader_
  {
    UShort  firstCode;      /* first valid low byte         */
    UShort  entryCount;     /* number of valid low bytes    */
    Short   idDelta;        /* delta value to glyphIndex    */
    UShort  idRangeOffset;  /* offset from here to 1st code */
  };

  typedef struct TCMap2SubHeader_  TCMap2SubHeader;
  typedef TCMap2SubHeader*         PCMap2SubHeader;

  struct  TCMap2_
  {
    PUShort subHeaderKeys;
    /* high byte mapping table     */
    /* value = subHeader index * 8 */

    PCMap2SubHeader  subHeaders;
    PUShort          glyphIdArray;
    UShort           numGlyphId;        /* control value */
  };

  typedef struct TCMap2_  TCMap2;
  typedef TCMap2*         PCMap2;


  /* format 4 */

  struct  TCMap4Segment_
  {
    UShort  endCount;
    UShort  startCount;
    Short   idDelta;        /* in the specs defined as UShort but the
                               example there gives negative values... */
    UShort  idRangeOffset;
  };

  typedef struct TCMap4Segment_  TCMap4Segment;
  typedef TCMap4Segment*         PCMap4Segment;

  struct  TCMap4_
  {
    UShort  segCountX2;     /* number of segments * 2       */
    UShort  searchRange;    /* these parameters can be used */
    UShort  entrySelector;  /* for a binary search          */
    UShort  rangeShift;

    PCMap4Segment  segments;
    PUShort        glyphIdArray;
    UShort         numGlyphId;          /* control value */
  };

  typedef struct TCMap4_  TCMap4;
  typedef TCMap4*         PCMap4;


  /* format 6 */

  struct  TCMap6_
  {
    UShort   firstCode;      /* first character code of subrange      */
    UShort   entryCount;     /* number of character codes in subrange */

    PUShort  glyphIdArray;
  };

  typedef struct TCMap6_  TCMap6;
  typedef TCMap6*         PCMap6;


  /* formats 8 & 12 */

  /* auxiliary table for format 8 and 12 */

  struct  TCMapGroup_
  {
    ULong  startCharCode;
    ULong  endCharCode;
    ULong  startGlyphID;
  };

  typedef struct TCMapGroup_  TCMapGroup;
  typedef TCMapGroup*         PCMapGroup;

  /* FreeType handles format 8 and 12 identically.  It is not necessary to
     cover mixed 16bit and 32bit codes since FreeType always uses ULong
     for input character codes -- converting Unicode surrogates to 32bit
     character codes must be done by the application.                      */

  struct  TCMap8_12_
  {
    ULong       nGroups;
    PCMapGroup  groups;

    PCMapGroup  last_group;      /* last used group; this is a small    */
                                 /* cache to potentially increase speed */
  };
    
  typedef struct TCMap8_12_  TCMap8_12;
  typedef TCMap8_12*         PCMap8_12;


  /* charmap table */

  struct  TCMapTable_
  {
    UShort  platformID;
    UShort  platformEncodingID;
    UShort  format;
    UShort  length;
    UShort  version;

    Bool    loaded;
    ULong   offset;

    union
    {
      TCMap0     cmap0;
      TCMap2     cmap2;
      TCMap4     cmap4;
      TCMap6     cmap6;
      TCMap8_12  cmap8_12;
    } c;
  };

  typedef struct TCMapTable_  TCMapTable;
  typedef TCMapTable*         PCMapTable;



  /* Load character mappings directory when face is loaded. */
  /* The mappings themselves are only loaded on demand.     */

  FT_INTERNAL_DEF( TT_Error )
  CharMap_Load( PCMapTable  table,
                TT_Stream  input );


  /* Destroy one character mapping table */

  FT_INTERNAL_DEF( TT_Error )
  CharMap_Free( PCMapTable  table );


  /* Use character mapping table to perform mapping */

  FT_INTERNAL_DEF( UShort )
  CharMap_Index( PCMapTable  cmap,
                 ULong       charCode );

  /* NOTE: The PFace type isn't defined at this point */

#ifdef __cplusplus
  }
#endif

#endif /* TTCMAP_H */


/* END */
