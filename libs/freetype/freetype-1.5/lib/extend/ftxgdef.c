/*******************************************************************
 *
 *  ftxgdef.c
 *
 *    TrueType Open GDEF table support.
 *
 *  Copyright 1996-2001 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#include "tttypes.h"
#include "tttags.h"
#include "ttload.h"
#include "ttextend.h"
#include "ttmemory.h"
#include "ttfile.h"

#include "ftxopen.h"
#include "ftxopenf.h"


  static TT_Error  Load_AttachList( TTO_AttachList*  al,
                                    PFace            input );
  static TT_Error  Load_LigCaretList( TTO_LigCaretList*  lcl,
                                      PFace              input );

  static void  Free_AttachList( TTO_AttachList*  al );
  static void  Free_LigCaretList( TTO_LigCaretList*  lcl );

  static void  Free_NewGlyphClasses( TTO_GDEFHeader*  gdef );



  /**********************
   * Extension Functions
   **********************/


  static TT_Error  GDEF_Create( void*  ext,
                                PFace  face )
  {
    DEFINE_LOAD_LOCALS( face->stream );

    TTO_GDEFHeader*  gdef = (TTO_GDEFHeader*)ext;
    Long             table;


    /* by convention */

    if ( !gdef )
      return TT_Err_Ok;

    /* a null offset indicates that there is no GDEF table */

    gdef->offset = 0;

    /* we store the start offset and the size of the subtable */

    table = TT_LookUp_Table( face, TTAG_GDEF );
    if ( table < 0 )
      return TT_Err_Ok;             /* The table is optional */

    if ( FILE_Seek( face->dirTables[table].Offset ) ||
         ACCESS_Frame( 4L ) )
      return error;

    gdef->offset  = FILE_Pos() - 4L;    /* undo ACCESS_Frame() */
    gdef->Version = GET_ULong();

    FORGET_Frame();

    gdef->loaded = FALSE;

    return TT_Err_Ok;
  }


  static TT_Error  GDEF_Destroy( void*  ext,
                                 PFace  face )
  {
    TTO_GDEFHeader*  gdef = (TTO_GDEFHeader*)ext;


    /* by convention */

    if ( !gdef )
      return TT_Err_Ok;

    if ( gdef->loaded )
    {
      Free_LigCaretList( &gdef->LigCaretList );
      Free_AttachList( &gdef->AttachList );
      Free_ClassDefinition( &gdef->GlyphClassDef );
      Free_ClassDefinition( &gdef->MarkAttachClassDef );

      Free_NewGlyphClasses( gdef );
    }

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_Init_GDEF_Extension( TT_Engine  engine )
  {
    PEngine_Instance  _engine = HANDLE_Engine( engine );


    if ( !_engine )
      return TT_Err_Invalid_Engine;

    return  TT_Register_Extension( _engine,
                                   GDEF_ID,
                                   sizeof ( TTO_GDEFHeader ),
                                   GDEF_Create,
                                   GDEF_Destroy );
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_Load_GDEF_Table( TT_Face          face,
                      TTO_GDEFHeader*  retptr )
  {
    ULong            cur_offset, new_offset, base_offset;

    TTO_GDEFHeader*  gdef;

    PFace  faze = HANDLE_Face( face );
    DEFINE_ALL_LOCALS;


    if ( !retptr )
      return TT_Err_Invalid_Argument;

    if ( !faze )
      return TT_Err_Invalid_Face_Handle;

    error = TT_Extension_Get( faze, GDEF_ID, (void**)&gdef );
    if ( error )
      return error;

    if ( gdef->offset == 0 )
      return TT_Err_Table_Missing;      /* no GDEF table; nothing to do */

    /* now access stream */

    if ( USE_Stream( faze->stream, stream ) )
      return error;

    base_offset = gdef->offset;

    /* skip version */

    if ( FILE_Seek( base_offset + 4L ) ||
         ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort();

    FORGET_Frame();

    /* all GDEF subtables are optional */

    if ( new_offset )
    {
      new_offset += base_offset;

      /* only classes 1-4 are allowed here */

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_ClassDefinition( &gdef->GlyphClassDef, 5,
                                           faze ) ) != TT_Err_Ok )
        return error;
      (void)FILE_Seek( cur_offset );
    }
    else
      gdef->GlyphClassDef.loaded = FALSE;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort();

    FORGET_Frame();

    if ( new_offset )
    {
      new_offset += base_offset;

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_AttachList( &gdef->AttachList,
                                      faze ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }
    else
      gdef->AttachList.loaded = FALSE;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort();

    FORGET_Frame();

    if ( new_offset )
    {
      new_offset += base_offset;

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_LigCaretList( &gdef->LigCaretList,
                                        faze ) ) != TT_Err_Ok )
        goto Fail2;
      (void)FILE_Seek( cur_offset );
    }
    else
      gdef->LigCaretList.loaded = FALSE;

    /* OpenType 1.2 has introduced the `MarkAttachClassDef' field.  We
       first have to scan the LookupFlag values to find out whether we
       must load it or not.  Here we only store the current file offset. */

    gdef->MarkAttachClassDef_offset = FILE_Pos();
    gdef->MarkAttachClassDef.loaded = FALSE;

    gdef->LastGlyph       = 0;
    gdef->NewGlyphClasses = NULL;
    gdef->loaded          = TRUE;

    *retptr = *gdef;
    DONE_Stream( stream );

    return TT_Err_Ok;

  Fail2:
    Free_AttachList( &gdef->AttachList );

  Fail1:
    Free_ClassDefinition( &gdef->GlyphClassDef );

    /* release stream */

    DONE_Stream( stream );

    return error;
  }



  /*******************************
   * AttachList related functions
   *******************************/


  /* AttachPoint */

  static TT_Error  Load_AttachPoint( TTO_AttachPoint*  ap,
                                     PFace             input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort   n, count;
    UShort*  pi;


    if ( ACCESS_Frame( 2L ) )
      return error;

    count = ap->PointCount = GET_UShort();

    FORGET_Frame();

    ap->PointIndex = NULL;

    if ( count )
    {
      if ( ALLOC_ARRAY( ap->PointIndex, count, UShort ) )
        return error;

      pi = ap->PointIndex;

      if ( ACCESS_Frame( count * 2L ) )
      {
        FREE( pi );
        return error;
      }

      for ( n = 0; n < count; n++ )
        pi[n] = GET_UShort();

      FORGET_Frame();
    }

    return TT_Err_Ok;
  }


  static void  Free_AttachPoint( TTO_AttachPoint*  ap )
  {
    FREE( ap->PointIndex );
  }


  /* AttachList */

  static TT_Error  Load_AttachList( TTO_AttachList*  al,
                                    PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            n, count;
    ULong             cur_offset, new_offset, base_offset;

    TTO_AttachPoint*  ap;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &al->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = al->GlyphCount = GET_UShort();

    FORGET_Frame();

    al->AttachPoint = NULL;

    if ( ALLOC_ARRAY( al->AttachPoint, count, TTO_AttachPoint ) )
      goto Fail2;

    ap = al->AttachPoint;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_AttachPoint( &ap[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    al->loaded = TRUE;

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_AttachPoint( &ap[n] );

    FREE( ap );

  Fail2:
    Free_Coverage( &al->Coverage );
    return error;
  }


  static void  Free_AttachList( TTO_AttachList*  al )
  {
    UShort            n, count;

    TTO_AttachPoint*  ap;


    if ( !al->loaded )
      return;

    if ( al->AttachPoint )
    {
      count = al->GlyphCount;
      ap    = al->AttachPoint;

      for ( n = 0; n < count; n++ )
        Free_AttachPoint( &ap[n] );

      FREE( ap );
    }

    Free_Coverage( &al->Coverage );
  }



  /*********************************
   * LigCaretList related functions
   *********************************/


  /* CaretValueFormat1 */
  /* CaretValueFormat2 */
  /* CaretValueFormat3 */
  /* CaretValueFormat4 */

  static TT_Error  Load_CaretValue( TTO_CaretValue*  cv,
                                    PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    ULong    cur_offset, new_offset, base_offset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    cv->CaretValueFormat = GET_UShort();

    FORGET_Frame();

    switch ( cv->CaretValueFormat )
    {
    case 1:
      if ( ACCESS_Frame( 2L ) )
        return error;

      cv->cvf.cvf1.Coordinate = GET_Short();

      FORGET_Frame();

      break;

    case 2:
      if ( ACCESS_Frame( 2L ) )
        return error;

      cv->cvf.cvf2.CaretValuePoint = GET_UShort();

      FORGET_Frame();

      break;

    case 3:
      if ( ACCESS_Frame( 4L ) )
        return error;

      cv->cvf.cvf3.Coordinate = GET_Short();

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Device( &cv->cvf.cvf3.Device,
                                  input ) ) != TT_Err_Ok )
        return error;
      (void)FILE_Seek( cur_offset );

      break;

    case 4:
      if ( ACCESS_Frame( 2L ) )
        return error;

      cv->cvf.cvf4.IdCaretValue = GET_UShort();

      FORGET_Frame();
      break;

    default:
      return TTO_Err_Invalid_GDEF_SubTable_Format;
    }

    return TT_Err_Ok;
  }


  static void  Free_CaretValue( TTO_CaretValue*  cv )
  {
    if ( cv->CaretValueFormat == 3 )
      Free_Device( &cv->cvf.cvf3.Device );
  }


  /* LigGlyph */

  static TT_Error  Load_LigGlyph( TTO_LigGlyph*  lg,
                                  PFace          input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort           n, count;
    ULong            cur_offset, new_offset, base_offset;

    TTO_CaretValue*  cv;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = lg->CaretCount = GET_UShort();

    FORGET_Frame();

    lg->CaretValue = NULL;

    if ( ALLOC_ARRAY( lg->CaretValue, count, TTO_CaretValue ) )
      return error;

    cv = lg->CaretValue;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_CaretValue( &cv[n], input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_CaretValue( &cv[n] );

    FREE( cv );
    return error;
  }


  static void  Free_LigGlyph( TTO_LigGlyph*  lg )
  {
    UShort           n, count;

    TTO_CaretValue*  cv;


    if ( lg->CaretValue )
    {
      count = lg->CaretCount;
      cv    = lg->CaretValue;

      for ( n = 0; n < count; n++ )
        Free_CaretValue( &cv[n] );

      FREE( cv );
    }
  }


  /* LigCaretList */

  static TT_Error  Load_LigCaretList( TTO_LigCaretList*  lcl,
                                      PFace              input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort         n, count;
    ULong          cur_offset, new_offset, base_offset;

    TTO_LigGlyph*  lg;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &lcl->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = lcl->LigGlyphCount = GET_UShort();

    FORGET_Frame();

    lcl->LigGlyph = NULL;

    if ( ALLOC_ARRAY( lcl->LigGlyph, count, TTO_LigGlyph ) )
      goto Fail2;

    lg = lcl->LigGlyph;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_LigGlyph( &lg[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    lcl->loaded = TRUE;

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_LigGlyph( &lg[n] );

    FREE( lg );

  Fail2:
    Free_Coverage( &lcl->Coverage );
    return error;
  }


  static void  Free_LigCaretList( TTO_LigCaretList*  lcl )
  {
    UShort         n, count;

    TTO_LigGlyph*  lg;


    if ( !lcl->loaded )
      return;

    if ( lcl->LigGlyph )
    {
      count = lcl->LigGlyphCount;
      lg    = lcl->LigGlyph;

      for ( n = 0; n < count; n++ )
        Free_LigGlyph( &lg[n] );

      FREE( lg );
    }

    Free_Coverage( &lcl->Coverage );
  }



  /***********
   * GDEF API
   ***********/


  static UShort  Get_New_Class( TTO_GDEFHeader*  gdef,
                                UShort           glyphID,
                                UShort           index )
  {
    UShort                 glyph_index, array_index;
    UShort                 byte, bits;

    TTO_ClassRangeRecord*  gcrr;
    UShort**               ngc;


    if ( glyphID >= gdef->LastGlyph )
      return 0;

    gcrr = gdef->GlyphClassDef.cd.cd2.ClassRangeRecord;
    ngc  = gdef->NewGlyphClasses;

    if ( glyphID < gcrr[index].Start )
    {
      array_index = 0;
      if ( index == 0 )
        glyph_index = glyphID;
      else
        glyph_index = glyphID - gcrr[index - 1].End - 1;
    }
    else
    {
      array_index = index + 1;
      glyph_index = glyphID - gcrr[index].End - 1;
    }

    byte = ngc[array_index][glyph_index / 4 + 1];
    bits = byte >> ( 16 - ( glyph_index % 4 + 1 ) * 4 );

    return bits & 0x000F;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GDEF_Get_Glyph_Property( TTO_GDEFHeader*  gdef,
                              TT_UShort        glyphID,
                              TT_UShort*       property )
  {
    UShort    class, index;

    TT_Error  error;


    if ( !gdef || !property )
      return TT_Err_Invalid_Argument;

    /* first, we check for mark attach classes */

    if ( gdef->MarkAttachClassDef.loaded )
    {
      error = Get_Class( &gdef->MarkAttachClassDef, glyphID, &class, &index );
      if ( error && error != TTO_Err_Not_Covered )
        return error;
      if ( !error )
      {
        *property = class << 8;
        return TT_Err_Ok;
      }
    }

    error = Get_Class( &gdef->GlyphClassDef, glyphID, &class, &index );
    if ( error && error != TTO_Err_Not_Covered )
      return error;

    /* if we have a constructed class table, check whether additional
       values have been assigned                                      */

    if ( error == TTO_Err_Not_Covered && gdef->NewGlyphClasses )
      class = Get_New_Class( gdef, glyphID, index );

    switch ( class )
    {
    case UNCLASSIFIED_GLYPH:
      *property = 0;
      break;

    case SIMPLE_GLYPH:
      *property = TTO_BASE_GLYPH;
      break;

    case LIGATURE_GLYPH:
      *property = TTO_LIGATURE;
      break;

    case MARK_GLYPH:
      *property = TTO_MARK;
      break;

    case COMPONENT_GLYPH:
      *property = TTO_COMPONENT;
      break;
    }

    return TT_Err_Ok;
  }


  static TT_Error  Make_ClassRange( TTO_ClassDefinition*  cd,
                                    UShort                start,
                                    UShort                end,
                                    UShort                class )
  {
    TT_Error               error;
    UShort                 index;

    TTO_ClassDefFormat2*   cdf2;
    TTO_ClassRangeRecord*  crr;


    cdf2 = &cd->cd.cd2;

    cdf2->ClassRangeCount++;

    if ( REALLOC_ARRAY( cdf2->ClassRangeRecord, cdf2->ClassRangeCount,
                        TTO_ClassRangeRecord ) )
      return error;

    crr   = cdf2->ClassRangeRecord;
    index = cdf2->ClassRangeCount - 1;

    crr[index].Start = start;
    crr[index].End   = end;
    crr[index].Class = class;

    cd->Defined[class] = TRUE;

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GDEF_Build_ClassDefinition( TTO_GDEFHeader*  gdef,
                                 TT_UShort        num_glyphs,
                                 TT_UShort        glyph_count,
                                 TT_UShort*       glyph_array,
                                 TT_UShort*       class_array )
  {
    UShort                 start, curr_glyph, curr_class;
    UShort                 n, count;
    TT_Error               error;

    TTO_ClassDefinition*   gcd;
    TTO_ClassRangeRecord*  gcrr;
    UShort**               ngc;


    if ( !gdef || !glyph_array || !class_array )
      return TT_Err_Invalid_Argument;

    gcd = &gdef->GlyphClassDef;

    /* We build a format 2 table */

    gcd->ClassFormat = 2;

    /* A GlyphClassDef table contains at most 5 different class values */

    if ( ALLOC_ARRAY( gcd->Defined, 5, Bool ) )
      return error;

    gcd->cd.cd2.ClassRangeCount  = 0;
    gcd->cd.cd2.ClassRangeRecord = NULL;

    start      = glyph_array[0];
    curr_class = class_array[0];
    curr_glyph = start;

    if ( curr_class >= 5 )
    {
      error = TT_Err_Invalid_Argument;
      goto Fail4;
    }

    glyph_count--;

    for ( n = 0; n <= glyph_count; n++ )
    {
      if ( curr_glyph == glyph_array[n] && curr_class == class_array[n] )
      {
        if ( n == glyph_count )
        {
          if ( ( error = Make_ClassRange( gcd, start,
                                          curr_glyph,
                                          curr_class ) ) != TT_Err_Ok )
            goto Fail3;
        }
        else
        {
          if ( curr_glyph == 0xFFFF )
          {
            error = TT_Err_Invalid_Argument;
            goto Fail3;
          }
          else
            curr_glyph++;
        }
      }
      else
      {
        if ( ( error = Make_ClassRange( gcd, start,
                                        curr_glyph - 1,
                                        curr_class ) ) != TT_Err_Ok )
          goto Fail3;

        if ( curr_glyph > glyph_array[n] )
        {
          error = TT_Err_Invalid_Argument;
          goto Fail3;
        }

        start      = glyph_array[n];
        curr_class = class_array[n];
        curr_glyph = start;

        if ( curr_class >= 5 )
        {
          error = TT_Err_Invalid_Argument;
          goto Fail3;
        }

        if ( n == glyph_count )
        {
          if ( ( error = Make_ClassRange( gcd, start,
                                          curr_glyph,
                                          curr_class ) ) != TT_Err_Ok )
            goto Fail3;
        }
        else
        {
          if ( curr_glyph == 0xFFFF )
          {
            error = TT_Err_Invalid_Argument;
            goto Fail3;
          }
          else
            curr_glyph++;
        }
      }
    }

    /* now prepare the arrays for class values assigned during the lookup
       process                                                            */

    if ( ALLOC_ARRAY( gdef->NewGlyphClasses,
                      gcd->cd.cd2.ClassRangeCount + 1, UShort* ) )
      goto Fail2;

    count = gcd->cd.cd2.ClassRangeCount;
    gcrr  = gcd->cd.cd2.ClassRangeRecord;
    ngc   = gdef->NewGlyphClasses;

    /* We allocate arrays for all glyphs not covered by the class range
       records.  Each element holds four class values.                  */

    if ( gcrr[0].Start )
    {
      if ( ALLOC_ARRAY( ngc[0], gcrr[0].Start / 4 + 1, UShort ) )
        goto Fail1;
    }

    for ( n = 1; n < count; n++ )
    {
      if ( gcrr[n].Start - gcrr[n - 1].End > 1 )
        if ( ALLOC_ARRAY( ngc[n],
                          ( gcrr[n].Start - gcrr[n - 1].End - 1 ) / 4 + 1,
                          UShort ) )
          goto Fail1;
    }

    if ( gcrr[count - 1].End != num_glyphs - 1 )
    {
      if ( ALLOC_ARRAY( ngc[count],
                        ( num_glyphs - gcrr[count - 1].End - 1 ) / 4 + 1,
                        UShort ) )
        goto Fail1;
    }

    gdef->LastGlyph = num_glyphs - 1;

    gdef->MarkAttachClassDef_offset = 0L;
    gdef->MarkAttachClassDef.loaded = FALSE;

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      FREE( ngc[n] );

  Fail2:
    FREE( gdef->NewGlyphClasses );

  Fail3:
    FREE( gcd->cd.cd2.ClassRangeRecord );

  Fail4:
    FREE( gcd->Defined );
    return error;
  }


  static void  Free_NewGlyphClasses( TTO_GDEFHeader*  gdef )
  {
    UShort**  ngc;
    UShort    n, count;


    if ( gdef->NewGlyphClasses )
    {
      count = gdef->GlyphClassDef.cd.cd2.ClassRangeCount + 1;
      ngc   = gdef->NewGlyphClasses;

      for ( n = 0; n < count; n++ )
        FREE( ngc[n] );

      FREE( ngc );
    }
  }


  TT_Error  Add_Glyph_Property( TTO_GDEFHeader*  gdef,
                                UShort           glyphID,
                                UShort           property )
  {
    TT_Error               error;
    UShort                 class, new_class, index;
    UShort                 byte, bits, mask;
    UShort                 array_index, glyph_index;

    TTO_ClassRangeRecord*  gcrr;
    UShort**               ngc;


    error = Get_Class( &gdef->GlyphClassDef, glyphID, &class, &index );
    if ( error && error != TTO_Err_Not_Covered )
      return error;

    /* we don't accept glyphs covered in `GlyphClassDef' */

    if ( !error )
      return TTO_Err_Not_Covered;

    switch ( property )
    {
    case 0:
      new_class = UNCLASSIFIED_GLYPH;
      break;

    case TTO_BASE_GLYPH:
      new_class = SIMPLE_GLYPH;
      break;

    case TTO_LIGATURE:
      new_class = LIGATURE_GLYPH;
      break;

    case TTO_MARK:
      new_class = MARK_GLYPH;
      break;

    case TTO_COMPONENT:
      new_class = COMPONENT_GLYPH;
      break;

    default:
      return TT_Err_Invalid_Argument;
    }

    gcrr = gdef->GlyphClassDef.cd.cd2.ClassRangeRecord;
    ngc  = gdef->NewGlyphClasses;

    if ( glyphID < gcrr[index].Start )
    {
      array_index = 0;
      if ( index == 0 )
        glyph_index = glyphID;
      else
        glyph_index = glyphID - gcrr[index - 1].End - 1;
    }
    else
    {
      array_index = index + 1;
      glyph_index = glyphID - gcrr[index].End - 1;
    }

    byte  = ngc[array_index][glyph_index / 4 + 1];
    bits  = byte >> ( 16 - ( glyph_index % 4 + 1 ) * 4 );
    class = bits & 0x000F;

    /* we don't overwrite existing entries */

    if ( !class )
    {
      bits = new_class << ( 16 - ( glyph_index % 4 + 1 ) * 4 );
      mask = ~( 0x000F << ( 16 - ( glyph_index % 4 + 1 ) * 4 ) );

      ngc[array_index][glyph_index / 4 + 1] &= mask;
      ngc[array_index][glyph_index / 4 + 1] |= bits;
    }

    return TT_Err_Ok;
  }


  TT_Error  Check_Property( TTO_GDEFHeader*  gdef,
                            UShort           index,
                            UShort           flags,
                            UShort*          property )
  {
    TT_Error  error;


    if ( gdef )
    {
      error = TT_GDEF_Get_Glyph_Property( gdef, index, property );
      if ( error )
        return error;

      if ( flags & IGNORE_SPECIAL_MARKS )
      {
        /* This is OpenType 1.2 */

        if ( (flags & 0xFF00) != *property )
          return TTO_Err_Not_Covered;
      }
      else {
        if ( flags & *property )
          return TTO_Err_Not_Covered;
      }
    }

    return TT_Err_Ok;
  }


/* END */
