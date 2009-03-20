/*******************************************************************
 *
 *  ftxgpos.c
 *
 *    TrueType Open GPOS table support.
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

/* XXX There is *a lot* of duplicated code (cf. formats 7 and 8), but
       I don't care currently.  I believe that it would be possible to
       save about 50% of TTO code by carefully designing the structures,
       sharing as much as possible with extensive use of macros.  This
       is something for a volunteer :-)                                  */

#include "tttypes.h"
#include "tttags.h"
#include "ttload.h"
#include "ttextend.h"
#include "ttmemory.h"
#include "ttfile.h"

#include "ftxopen.h"
#include "ftxopenf.h"


  struct  GPOS_Instance_
  {
    TTO_GPOSHeader*  gpos;
    TT_Instance      instance;
    TT_Glyph         glyph;
    UShort           load_flags;  /* how the glyph should be loaded */
    TT_Bool          r2l;

    UShort           first;       /* the first glyph in a chain of
                                     cursive connections           */
    UShort           last;        /* the last valid glyph -- used
                                     with cursive positioning     */
    TT_Pos           anchor_x;    /* the coordinates of the anchor point */
    TT_Pos           anchor_y;    /* of the last valid glyph             */
  };

  typedef struct GPOS_Instance_  GPOS_Instance;


  static TT_Error  Do_Glyph_Lookup( GPOS_Instance*    gpi,
                                    UShort            lookup_index,
                                    TTO_GSUB_String*  in,
                                    TTO_GPOS_Data*    out,
                                    UShort            context_length,
                                    int               nesting_level );



  /**********************
   * Extension Functions
   **********************/

  /* the client application must replace this with something more
     meaningful if multiple master fonts are to be supported.     */

  static TT_Error  default_mmfunc( TT_Instance  instance,
                                   TT_UShort    metric_id,
                                   TT_Pos*      metric_value,
                                   void*        data )
  {
    return TTO_Err_No_MM_Interpreter;
  }


  static TT_Error  GPOS_Create( void*  ext,
                                PFace  face )
  {
    DEFINE_LOAD_LOCALS( face->stream );

    TTO_GPOSHeader*  gpos = (TTO_GPOSHeader*)ext;
    Long             table;


    /* by convention */

    if ( !gpos )
      return TT_Err_Ok;

    /* a null offset indicates that there is no GPOS table */

    gpos->offset = 0;

    /* we store the start offset and the size of the subtable */

    table = TT_LookUp_Table( face, TTAG_GPOS );
    if ( table < 0 )
      return TT_Err_Ok;             /* The table is optional */

    if ( FILE_Seek( face->dirTables[table].Offset ) ||
         ACCESS_Frame( 4L ) )
      return error;

    gpos->offset  = FILE_Pos() - 4L;    /* undo ACCESS_Frame() */
    gpos->Version = GET_ULong();

    FORGET_Frame();

    /* a default mmfunc() handler which just returns an error */

    gpos->mmfunc = default_mmfunc;

    /* the default glyph function is TT_Load_Glyph() */

    gpos->gfunc = TT_Load_Glyph;

    gpos->loaded = FALSE;

    return TT_Err_Ok;
  }


  static TT_Error  GPOS_Destroy( void*  ext,
                                 PFace  face )
  {
    TTO_GPOSHeader*  gpos = (TTO_GPOSHeader*)ext;


    /* by convention */

    if ( !gpos )
      return TT_Err_Ok;

    if ( gpos->loaded )
    {
      Free_LookupList( &gpos->LookupList, GPOS );
      Free_FeatureList( &gpos->FeatureList );
      Free_ScriptList( &gpos->ScriptList );
    }

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_Init_GPOS_Extension( TT_Engine  engine )
  {
    PEngine_Instance  _engine = HANDLE_Engine( engine );


    if ( !_engine )
      return TT_Err_Invalid_Engine;

    return  TT_Register_Extension( _engine,
                                   GPOS_ID,
                                   sizeof ( TTO_GPOSHeader ),
                                   GPOS_Create,
                                   GPOS_Destroy );
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_Load_GPOS_Table( TT_Face          face,
                      TTO_GPOSHeader*  retptr,
                      TTO_GDEFHeader*  gdef )
  {
    ULong            cur_offset, new_offset, base_offset;

    UShort           i, num_lookups;
    TTO_GPOSHeader*  gpos;
    TTO_GDEFHeader*  gdef_reg;
    TTO_Lookup*      lo;

    PFace  faze = HANDLE_Face( face );
    DEFINE_ALL_LOCALS;


    if ( !retptr )
      return TT_Err_Invalid_Argument;

    if ( !faze )
      return TT_Err_Invalid_Face_Handle;

    error = TT_Extension_Get( faze, GPOS_ID, (void**)&gpos );
    if ( error )
      return error;

    if ( gpos->offset == 0 )
      return TT_Err_Table_Missing;      /* no GPOS table; nothing to do */

    /* now access stream */

    if ( USE_Stream( faze->stream, stream ) )
      return error;

    base_offset = gpos->offset;

    /* skip version */

    if ( FILE_Seek( base_offset + 4L ) ||
         ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_ScriptList( &gpos->ScriptList,
                                    faze ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_FeatureList( &gpos->FeatureList,
                                     faze ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_LookupList( &gpos->LookupList,
                                    faze, GPOS ) ) != TT_Err_Ok )
      goto Fail2;

    gpos->gdef = gdef;      /* can be NULL */

    /* We now check the LookupFlags for values larger than 0xFF to find
       out whether we need to load the `MarkAttachClassDef' field of the
       GDEF table -- this hack is necessary for OpenType 1.2 tables since
       the version field of the GDEF table hasn't been incremented.

       For constructed GDEF tables, we only load it if
       `MarkAttachClassDef_offset' is not zero (nevertheless, a build of
       a constructed mark attach table is not supported currently).       */

    if ( gdef &&
         gdef->MarkAttachClassDef_offset && !gdef->MarkAttachClassDef.loaded )
    {
      lo          = gpos->LookupList.Lookup;
      num_lookups = gpos->LookupList.LookupCount;

      for ( i = 0; i < num_lookups; i++ )
      {
        if ( lo[i].LookupFlag & IGNORE_SPECIAL_MARKS )
        {
          if ( FILE_Seek( gdef->MarkAttachClassDef_offset ) ||
               ACCESS_Frame( 2L ) )
            goto Fail1;

          new_offset = GET_UShort();

          FORGET_Frame();

          if ( !new_offset )
            return TTO_Err_Invalid_GDEF_SubTable;

          new_offset += gdef->offset;

          if ( FILE_Seek( new_offset ) ||
               ( error = Load_ClassDefinition( &gdef->MarkAttachClassDef,
                                               256, faze ) ) != TT_Err_Ok )
            goto Fail1;

          /* copy the class definition pointer into the extension structure */

          error = TT_Extension_Get( faze, GDEF_ID, (void**)&gdef_reg );
          if ( error )
            return error;

          *gdef_reg = *gdef;

          break;
        }
      }
    }

    gpos->loaded = TRUE;
    *retptr = *gpos;
    DONE_Stream( stream );

    return TT_Err_Ok;

  Fail1:
    Free_LookupList( &gpos->LookupList, GPOS );

  Fail2:
    Free_FeatureList( &gpos->FeatureList );

  Fail3:
    Free_ScriptList( &gpos->ScriptList );

    /* release stream */

    DONE_Stream( stream );

    return error;
  }



  /*****************************
   * SubTable related functions
   *****************************/

  /* shared tables */

  /* ValueRecord */

  /* There is a subtle difference in the specs between a `table' and a
     `record' -- offsets for device tables in ValueRecords are taken from
     the parent table and not the parent record.                          */

  static TT_Error  Load_ValueRecord( TTO_ValueRecord*  vr,
                                     UShort            format,
                                     ULong             base_offset,
                                     PFace             input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    ULong    cur_offset, new_offset;


    if ( format & HAVE_X_PLACEMENT )
    {
      if ( ACCESS_Frame( 2L ) )
        return error;

      vr->XPlacement = GET_Short();

      FORGET_Frame();
    }
    else
      vr->XPlacement = 0;

    if ( format & HAVE_Y_PLACEMENT )
    {
      if ( ACCESS_Frame( 2L ) )
        return error;

      vr->YPlacement = GET_Short();

      FORGET_Frame();
    }
    else
      vr->YPlacement = 0;

    if ( format & HAVE_X_ADVANCE )
    {
      if ( ACCESS_Frame( 2L ) )
        return error;

      vr->XAdvance = GET_Short();

      FORGET_Frame();
    }
    else
      vr->XAdvance = 0;

    if ( format & HAVE_Y_ADVANCE )
    {
      if ( ACCESS_Frame( 2L ) )
        return error;

      vr->YAdvance = GET_Short();

      FORGET_Frame();
    }
    else
      vr->YAdvance = 0;

    if ( format & HAVE_X_PLACEMENT_DEVICE )
    {
      if ( ACCESS_Frame( 2L ) )
        return error;

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Device( &vr->XPlacementDevice,
                                    input ) ) != TT_Err_Ok )
          return error;
        (void)FILE_Seek( cur_offset );
      }
      else
        goto empty1;
    }
    else
    {
    empty1:
      vr->XPlacementDevice.StartSize  = 0;
      vr->XPlacementDevice.EndSize    = 0;
      vr->XPlacementDevice.DeltaValue = NULL;
    }

    if ( format & HAVE_Y_PLACEMENT_DEVICE )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail3;

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Device( &vr->YPlacementDevice,
                                    input ) ) != TT_Err_Ok )
          goto Fail3;
        (void)FILE_Seek( cur_offset );
      }
      else
        goto empty2;
    }
    else
    {
    empty2:
      vr->YPlacementDevice.StartSize  = 0;
      vr->YPlacementDevice.EndSize    = 0;
      vr->YPlacementDevice.DeltaValue = NULL;
    }

    if ( format & HAVE_X_ADVANCE_DEVICE )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Device( &vr->XAdvanceDevice,
                                    input ) ) != TT_Err_Ok )
          goto Fail2;
        (void)FILE_Seek( cur_offset );
      }
      else
        goto empty3;
    }
    else
    {
    empty3:
      vr->XAdvanceDevice.StartSize  = 0;
      vr->XAdvanceDevice.EndSize    = 0;
      vr->XAdvanceDevice.DeltaValue = NULL;
    }

    if ( format & HAVE_Y_ADVANCE_DEVICE )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Device( &vr->YAdvanceDevice,
                                    input ) ) != TT_Err_Ok )
          goto Fail1;
        (void)FILE_Seek( cur_offset );
      }
      else
        goto empty4;
    }
    else
    {
    empty4:
      vr->YAdvanceDevice.StartSize  = 0;
      vr->YAdvanceDevice.EndSize    = 0;
      vr->YAdvanceDevice.DeltaValue = NULL;
    }

    if ( format & HAVE_X_ID_PLACEMENT )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      vr->XIdPlacement = GET_UShort();

      FORGET_Frame();
    }
    else
      vr->XIdPlacement = 0;

    if ( format & HAVE_Y_ID_PLACEMENT )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      vr->YIdPlacement = GET_UShort();

      FORGET_Frame();
    }
    else
      vr->YIdPlacement = 0;

    if ( format & HAVE_X_ID_ADVANCE )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      vr->XIdAdvance = GET_UShort();

      FORGET_Frame();
    }
    else
      vr->XIdAdvance = 0;

    if ( format & HAVE_Y_ID_ADVANCE )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      vr->YIdAdvance = GET_UShort();

      FORGET_Frame();
    }
    else
      vr->YIdAdvance = 0;

    return TT_Err_Ok;

  Fail1:
    Free_Device( &vr->YAdvanceDevice );

  Fail2:
    Free_Device( &vr->XAdvanceDevice );

  Fail3:
    Free_Device( &vr->YPlacementDevice );
    return error;
  }


  static void  Free_ValueRecord( TTO_ValueRecord*  vr,
                                 UShort            format )
  {
    if ( format & HAVE_Y_ADVANCE_DEVICE )
      Free_Device( &vr->YAdvanceDevice );
    if ( format & HAVE_X_ADVANCE_DEVICE )
      Free_Device( &vr->XAdvanceDevice );
    if ( format & HAVE_Y_PLACEMENT_DEVICE )
      Free_Device( &vr->YPlacementDevice );
    if ( format & HAVE_X_PLACEMENT_DEVICE )
      Free_Device( &vr->XPlacementDevice );
  }


  static TT_Error  Get_ValueRecord( GPOS_Instance*    gpi,
                                    TTO_ValueRecord*  vr,
                                    UShort            format,
                                    TTO_GPOS_Data*    gd )
  {
    TT_Pos           value;
    Short            pixel_value;
    TT_Error         error = TT_Err_Ok;
    TTO_GPOSHeader*  gpos = gpi->gpos;
    PInstance        ins;

    UShort     x_ppem, y_ppem;
    Fixed      x_scale, y_scale;


    if ( !format )
      return TT_Err_Ok;

    ins = HANDLE_Instance( gpi->instance );

    x_ppem  = ins->metrics.x_ppem;
    y_ppem  = ins->metrics.y_ppem;
    x_scale = TT_MulDiv( 0x10000,
                         ins->metrics.x_scale1,
                         ins->metrics.x_scale2 );
    y_scale = TT_MulDiv( 0x10000,
                         ins->metrics.y_scale1,
                         ins->metrics.y_scale2 );

    /* design units -> fractional pixel */

    if ( format & HAVE_X_PLACEMENT )
      gd->x_pos += x_scale * vr->XPlacement / 0x10000;
    if ( format & HAVE_Y_PLACEMENT )
      gd->y_pos += y_scale * vr->YPlacement / 0x10000;
    if ( format & HAVE_X_ADVANCE )
      gd->x_advance += x_scale * vr->XAdvance / 0x10000;
    if ( format & HAVE_Y_ADVANCE )
      gd->y_advance += y_scale * vr->YAdvance / 0x10000;

    /* we use the device tables only if gpi->glyph.z is not NULL */

    if ( gpi->glyph.z )
    {
      /* pixel -> fractional pixel */

      if ( format & HAVE_X_PLACEMENT_DEVICE )
      {
        Get_Device( &vr->XPlacementDevice, x_ppem, &pixel_value );
        gd->x_pos += pixel_value << 6;
      }
      if ( format & HAVE_Y_PLACEMENT_DEVICE )
      {
        Get_Device( &vr->YPlacementDevice, y_ppem, &pixel_value );
        gd->y_pos += pixel_value << 6;
      }
      if ( format & HAVE_X_ADVANCE_DEVICE )
      {
        Get_Device( &vr->XAdvanceDevice, x_ppem, &pixel_value );
        gd->x_advance += pixel_value << 6;
      }
      if ( format & HAVE_Y_ADVANCE_DEVICE )
      {
        Get_Device( &vr->YAdvanceDevice, y_ppem, &pixel_value );
        gd->y_advance += pixel_value << 6;
      }
    }

    /* values returned from mmfunc() are already in fractional pixels */

    if ( format & HAVE_X_ID_PLACEMENT )
    {
      error = (gpos->mmfunc)( gpi->instance, vr->XIdPlacement,
                              &value, gpos->data );
      if ( error )
        return error;
      gd->x_pos += value;
    }
    if ( format & HAVE_Y_ID_PLACEMENT )
    {
      error = (gpos->mmfunc)( gpi->instance, vr->YIdPlacement,
                              &value, gpos->data );
      if ( error )
        return error;
      gd->y_pos += value;
    }
    if ( format & HAVE_X_ID_ADVANCE )
    {
      error = (gpos->mmfunc)( gpi->instance, vr->XIdAdvance,
                              &value, gpos->data );
      if ( error )
        return error;
      gd->x_advance += value;
    }
    if ( format & HAVE_Y_ID_ADVANCE )
    {
      error = (gpos->mmfunc)( gpi->instance, vr->YIdAdvance,
                              &value, gpos->data );
      if ( error )
        return error;
      gd->y_advance += value;
    }

    return error;
  }


  /* AnchorFormat1 */
  /* AnchorFormat2 */
  /* AnchorFormat3 */
  /* AnchorFormat4 */

  static TT_Error  Load_Anchor( TTO_Anchor*  an,
                                PFace        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    ULong    cur_offset, new_offset, base_offset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    an->PosFormat = GET_UShort();

    FORGET_Frame();

    switch ( an->PosFormat )
    {
    case 1:
      if ( ACCESS_Frame( 4L ) )
        return error;

      an->af.af1.XCoordinate = GET_Short();
      an->af.af1.YCoordinate = GET_Short();

      FORGET_Frame();
      break;

    case 2:
      if ( ACCESS_Frame( 6L ) )
        return error;

      an->af.af2.XCoordinate = GET_Short();
      an->af.af2.YCoordinate = GET_Short();
      an->af.af2.AnchorPoint = GET_UShort();

      FORGET_Frame();
      break;

    case 3:
      if ( ACCESS_Frame( 6L ) )
        return error;

      an->af.af3.XCoordinate = GET_Short();
      an->af.af3.YCoordinate = GET_Short();

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Device( &an->af.af3.XDeviceTable,
                                    input ) ) != TT_Err_Ok )
          return error;
        (void)FILE_Seek( cur_offset );
      }
      else
      {
        an->af.af3.XDeviceTable.StartSize  = 0;
        an->af.af3.XDeviceTable.EndSize    = 0;
        an->af.af3.XDeviceTable.DeltaValue = 0;
      }

      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Device( &an->af.af3.YDeviceTable,
                                    input ) ) != TT_Err_Ok )
          goto Fail;
        (void)FILE_Seek( cur_offset );
      }
      else
      {
        an->af.af3.YDeviceTable.StartSize  = 0;
        an->af.af3.YDeviceTable.EndSize    = 0;
        an->af.af3.YDeviceTable.DeltaValue = 0;
      }
      break;

    case 4:
      if ( ACCESS_Frame( 4L ) )
        return error;

      an->af.af4.XIdAnchor = GET_UShort();
      an->af.af4.YIdAnchor = GET_UShort();

      FORGET_Frame();
      break;

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    return TT_Err_Ok;

  Fail:
    Free_Device( &an->af.af3.XDeviceTable );
    return error;
  }


  static void  Free_Anchor( TTO_Anchor*  an )
  {
    if ( an->PosFormat == 3 )
    {
      Free_Device( &an->af.af3.YDeviceTable );
      Free_Device( &an->af.af3.XDeviceTable );
    }
  }


  static TT_Error  Get_Anchor( GPOS_Instance*   gpi,
                               TTO_Anchor*      an,
                               TT_UShort        glyph_index,
                               TT_Pos*          x_value,
                               TT_Pos*          y_value )
  {
    TT_Error  error = TT_Err_Ok;

    PInstance        ins;
    PGlyph           glyph;
    TTO_GPOSHeader*  gpos = gpi->gpos;
    UShort           ap;

    Short            pixel_value;
    UShort           load_flags;

    UShort           x_ppem, y_ppem;
    Fixed            x_scale, y_scale;


    ins = HANDLE_Instance( gpi->instance );

    x_ppem  = ins->metrics.x_ppem;
    y_ppem  = ins->metrics.y_ppem;
    x_scale = TT_MulDiv( 0x10000,
                         ins->metrics.x_scale1,
                         ins->metrics.x_scale2 );
    y_scale = TT_MulDiv( 0x10000,
                         ins->metrics.y_scale1,
                         ins->metrics.y_scale2 );

    switch ( an->PosFormat )
    {
    case 0:
      /* The special case of an empty AnchorTable */

      return TTO_Err_Not_Covered;

    case 1:
      *x_value = x_scale * an->af.af1.XCoordinate / 0x10000;
      *y_value = y_scale * an->af.af1.YCoordinate / 0x10000;
      break;

    case 2:
      /* glyphs must be scaled */

      load_flags = gpi->load_flags | TTLOAD_SCALE_GLYPH;

      /* we use the glyph contour point only if gpi->glyph.z is not NULL */

      if ( gpi->glyph.z )
      {
        error = (gpos->gfunc)( gpi->instance, gpi->glyph,
                               glyph_index, load_flags );
        if ( error )
          return error;

        glyph = HANDLE_Glyph( gpi->glyph );
        ap    = an->af.af2.AnchorPoint;

        /* if outline.n_points is set to zero by gfunc(), we use the
           design coordinate value pair.  This can happen e.g. for
           sbit glyphs                                               */

        if ( !glyph->outline.n_points )
          goto no_contour_point;

        if ( ap >= glyph->outline.n_points )
          return TTO_Err_Invalid_GPOS_SubTable;

        *x_value = glyph->outline.points[ap].x;
        *y_value = glyph->outline.points[ap].y;
      }
      else
      {
      no_contour_point:
        *x_value = x_scale * an->af.af3.XCoordinate / 0x10000;
        *y_value = y_scale * an->af.af3.YCoordinate / 0x10000;
      }
      break;

    case 3:
      /* we use the device tables only if gpi->glyph.z is not NULL */

      if ( gpi->glyph.z )
      {
        Get_Device( &an->af.af3.XDeviceTable, x_ppem, &pixel_value );
        *x_value = pixel_value << 6;
        Get_Device( &an->af.af3.YDeviceTable, y_ppem, &pixel_value );
        *y_value = pixel_value << 6;
      }
      else
        *x_value = *y_value = 0;

      *x_value += x_scale * an->af.af3.XCoordinate / 0x10000;
      *y_value += y_scale * an->af.af3.YCoordinate / 0x10000;
      break;

    case 4:
      error = (gpos->mmfunc)( gpi->instance, an->af.af4.XIdAnchor,
                              x_value, gpos->data );
      if ( error )
        return error;

      error = (gpos->mmfunc)( gpi->instance, an->af.af4.YIdAnchor,
                              y_value, gpos->data );
      if ( error )
        return error;
      break;
    }

    return error;
  }


  /* MarkArray */

  static TT_Error  Load_MarkArray ( TTO_MarkArray*  ma,
                                    PFace           input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort           n, count;
    ULong            cur_offset, new_offset, base_offset;

    TTO_MarkRecord*  mr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = ma->MarkCount = GET_UShort();
    
    FORGET_Frame();

    ma->MarkRecord = NULL;

    if ( ALLOC_ARRAY( ma->MarkRecord, count, TTO_MarkRecord ) )
      return error;

    mr = ma->MarkRecord;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 4L ) )
        goto Fail;

      mr[n].Class = GET_UShort();
      new_offset  = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Anchor( &mr[n].MarkAnchor, input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_Anchor( &mr[n].MarkAnchor );

    FREE( mr );
    return error;
  }


  static void  Free_MarkArray( TTO_MarkArray*  ma )
  {
    UShort           n, count;

    TTO_MarkRecord*  mr;


    if ( ma->MarkRecord )
    {
      count = ma->MarkCount;
      mr    = ma->MarkRecord;

      for ( n = 0; n < count; n++ )
        Free_Anchor( &mr[n].MarkAnchor );

      FREE( mr );
    }
  }


  /* LookupType 1 */

  /* SinglePosFormat1 */
  /* SinglePosFormat2 */

  TT_Error  Load_SinglePos( TTO_SinglePos*  sp,
                            PFace           input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            n, count, format;
    ULong             cur_offset, new_offset, base_offset;

    TTO_ValueRecord*  vr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 6L ) )
      return error;

    sp->PosFormat = GET_UShort();
    new_offset    = GET_UShort() + base_offset;

    format = sp->ValueFormat = GET_UShort();

    FORGET_Frame();

    if ( !format )
      return TTO_Err_Invalid_GPOS_SubTable;

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &sp->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    switch ( sp->PosFormat )
    {
    case 1:
      error = Load_ValueRecord( &sp->spf.spf1.Value, format,
                                base_offset, input );
      if ( error )
        goto Fail2;
      break;

    case 2:
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      count = sp->spf.spf2.ValueCount = GET_UShort();

      FORGET_Frame();

      sp->spf.spf2.Value = NULL;

      if ( ALLOC_ARRAY( sp->spf.spf2.Value, count, TTO_ValueRecord ) )
        goto Fail2;

      vr = sp->spf.spf2.Value;

      for ( n = 0; n < count; n++ )
      {
        error = Load_ValueRecord( &vr[n], format, base_offset, input );
        if ( error )
          goto Fail1;
      }
      break;

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_ValueRecord( &vr[n], format );

    FREE( vr );

  Fail2:
    Free_Coverage( &sp->Coverage );
    return error;
  }


  void  Free_SinglePos( TTO_SinglePos*  sp )
  {
    UShort            n, count, format;

    TTO_ValueRecord*  v;


    format = sp->ValueFormat;

    switch ( sp->PosFormat )
    {
    case 1:
      Free_ValueRecord( &sp->spf.spf1.Value, format );
      break;

    case 2:
      if ( sp->spf.spf2.Value )
      {
        count = sp->spf.spf2.ValueCount;
        v     = sp->spf.spf2.Value;

        for ( n = 0; n < count; n++ )
          Free_ValueRecord( &v[n], format );

        FREE( v );
      }
      break;
    }

    Free_Coverage( &sp->Coverage );
  }


  static TT_Error  Lookup_SinglePos( GPOS_Instance*    gpi,
                                     TTO_SinglePos*    sp,
                                     TTO_GSUB_String*  in,
                                     TTO_GPOS_Data*    out,
                                     UShort            flags,
                                     UShort            context_length )
  {
    UShort           index, property;
    TT_Error         error;
    TTO_GPOSHeader*  gpos = gpi->gpos;


    if ( context_length != 0xFFFF && context_length < 1 )
      return TTO_Err_Not_Covered;

    if ( CHECK_Property( gpos->gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &sp->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    switch ( sp->PosFormat )
    {
    case 1:
      error = Get_ValueRecord( gpi, &sp->spf.spf1.Value,
                               sp->ValueFormat, &out[in->pos] );
      if ( error )
        return error;
      break;

    case 2:
      if ( index >= sp->spf.spf2.ValueCount )
        return TTO_Err_Invalid_GPOS_SubTable;
      error = Get_ValueRecord( gpi, &sp->spf.spf2.Value[index],
                               sp->ValueFormat, &out[in->pos] );
      if ( error )
        return error;
      break;

    default:
      return TTO_Err_Invalid_GPOS_SubTable;
    }

    (in->pos)++;

    return TT_Err_Ok;
  }


  /* LookupType 2 */

  /* PairSet */

  static TT_Error  Load_PairSet ( TTO_PairSet*  ps,
                                  UShort        format1,
                                  UShort        format2,
                                  PFace         input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    ULong                 base_offset;

    TTO_PairValueRecord*  pvr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = ps->PairValueCount = GET_UShort();
    
    FORGET_Frame();

    ps->PairValueRecord = NULL;

    if ( ALLOC_ARRAY( ps->PairValueRecord, count, TTO_PairValueRecord ) )
      return error;

    pvr = ps->PairValueRecord;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      pvr[n].SecondGlyph = GET_UShort();

      FORGET_Frame();

      if ( format1 )
      {
        error = Load_ValueRecord( &pvr[n].Value1, format1,
                                  base_offset, input );
        if ( error )
          goto Fail;
      }
      if ( format2 )
      {
        error = Load_ValueRecord( &pvr[n].Value2, format2,
                                  base_offset, input );
        if ( error )
          goto Fail;
      }
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
    {
      if ( format1 )
        Free_ValueRecord( &pvr[n].Value1, format1 );
      if ( format2 )
        Free_ValueRecord( &pvr[n].Value2, format2 );
    }

    FREE( pvr );
    return error;
  }


  static void  Free_PairSet( TTO_PairSet*  ps,
                             UShort        format1,
                             UShort        format2 )
  {
    UShort                n, count;

    TTO_PairValueRecord*  pvr;


    if ( ps->PairValueRecord )
    {
      count = ps->PairValueCount;
      pvr   = ps->PairValueRecord;

      for ( n = 0; n < count; n++ )
      {
        if ( format1 )
          Free_ValueRecord( &pvr[n].Value1, format1 );
        if ( format2 )
          Free_ValueRecord( &pvr[n].Value2, format2 );
      }

      FREE( pvr );
    }
  }


  /* PairPosFormat1 */

  static TT_Error  Load_PairPos1( TTO_PairPosFormat1*  ppf1,
                                  UShort               format1,
                                  UShort               format2,
                                  PFace                input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort        n, count;
    ULong         cur_offset, new_offset, base_offset;

    TTO_PairSet*  ps;


    base_offset = FILE_Pos() - 8L;

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = ppf1->PairSetCount = GET_UShort();

    FORGET_Frame();

    ppf1->PairSet = NULL;

    if ( ALLOC_ARRAY( ppf1->PairSet, count, TTO_PairSet ) )
      goto Fail;

    ps = ppf1->PairSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_PairSet( &ps[n], format1,
                                   format2, input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_PairSet( &ps[n], format1, format2 );

    FREE( ps );
    return error;
  }


  static void  Free_PairPos1( TTO_PairPosFormat1*  ppf1,
                              UShort               format1,
                              UShort               format2 )
  {
    UShort        n, count;

    TTO_PairSet*  ps;


    if ( ppf1->PairSet )
    {
      count = ppf1->PairSetCount;
      ps    = ppf1->PairSet;

      for ( n = 0; n < count; n++ )
        Free_PairSet( &ps[n], format1, format2 );

      FREE( ps );
    }
  }


  /* PairPosFormat2 */

  static TT_Error  Load_PairPos2( TTO_PairPosFormat2*  ppf2,
                                  UShort               format1,
                                  UShort               format2,
                                  PFace                input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort             m, n, count1, count2;
    ULong              cur_offset, new_offset1, new_offset2, base_offset;

    TTO_Class1Record*  c1r;
    TTO_Class2Record*  c2r;


    base_offset = FILE_Pos() - 8L;

    if ( ACCESS_Frame( 8L ) )
      return error;

    new_offset1 = GET_UShort() + base_offset;
    new_offset2 = GET_UShort() + base_offset;

    /* `Class1Count' and `Class2Count' are the upper limits for class
       values, thus we read it now to make additional safety checks.  */

    count1 = ppf2->Class1Count = GET_UShort();
    count2 = ppf2->Class2Count = GET_UShort();

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset1 ) ||
         ( error = Load_ClassDefinition( &ppf2->ClassDef1, count1,
                                         input ) ) != TT_Err_Ok )
      return error;
    if ( FILE_Seek( new_offset2 ) ||
         ( error = Load_ClassDefinition( &ppf2->ClassDef2, count2,
                                         input ) ) != TT_Err_Ok )
      goto Fail2;
    (void)FILE_Seek( cur_offset );

    ppf2->Class1Record = NULL;

    if ( ALLOC_ARRAY( ppf2->Class1Record, count1, TTO_Class1Record ) )
      goto Fail1;

    c1r = ppf2->Class1Record;

    for ( m = 0; m < count1; m++ )
    {
      c1r[m].Class2Record = NULL;

      if ( ALLOC_ARRAY( c1r[m].Class2Record, count2, TTO_Class2Record ) )
        goto Fail1;

      c2r = c1r[m].Class2Record;

      for ( n = 0; n < count2; n++ )
      {
        if ( format1 )
        {
          error = Load_ValueRecord( &c2r[n].Value1, format1,
                                    base_offset, input );
          if ( error )
            goto Fail1;
        }
        if ( format2 )
        {
          error = Load_ValueRecord( &c2r[n].Value2, format2,
                                    base_offset, input );
          if ( error )
            goto Fail1;
        }
      }
    }

    return TT_Err_Ok;

  Fail1:
    for ( m = 0; m < count1; m++ )
    {
      c2r = c1r[m].Class2Record;

      for ( n = 0; n < count2; n++ )
      {
        if ( format1 )
          Free_ValueRecord( &c2r[n].Value1, format1 );
        if ( format2 )
          Free_ValueRecord( &c2r[n].Value2, format2 );
      }

      FREE( c2r );
    }

    FREE( c1r );

    Free_ClassDefinition( &ppf2->ClassDef2 );

  Fail2:
    Free_ClassDefinition( &ppf2->ClassDef1 );
    return error;
  }


  static void  Free_PairPos2( TTO_PairPosFormat2*  ppf2,
                              UShort               format1,
                              UShort               format2 )
  {
    UShort             m, n, count1, count2;

    TTO_Class1Record*  c1r;
    TTO_Class2Record*  c2r;


    if ( ppf2->Class1Record )
    {
      c1r    = ppf2->Class1Record;
      count1 = ppf2->Class1Count;
      count2 = ppf2->Class2Count;

      for ( m = 0; m < count1; m++ )
      {
        c2r = c1r[m].Class2Record;

        for ( n = 0; n < count2; n++ )
        {
          if ( format1 )
            Free_ValueRecord( &c2r[n].Value1, format1 );
          if ( format2 )
            Free_ValueRecord( &c2r[n].Value2, format2 );
        }

        FREE( c2r );
      }

      FREE( c1r );

      Free_ClassDefinition( &ppf2->ClassDef2 );
      Free_ClassDefinition( &ppf2->ClassDef1 );
    }
  }


  TT_Error  Load_PairPos( TTO_PairPos*  pp,
                          PFace         input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            format1, format2;
    ULong             cur_offset, new_offset, base_offset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 8L ) )
      return error;

    pp->PosFormat = GET_UShort();
    new_offset    = GET_UShort() + base_offset;

    format1 = pp->ValueFormat1 = GET_UShort();
    format2 = pp->ValueFormat2 = GET_UShort();

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &pp->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    switch ( pp->PosFormat )
    {
    case 1:
      error = Load_PairPos1( &pp->ppf.ppf1, format1, format2, input );
      if ( error )
        goto Fail;
      break;

    case 2:
      error = Load_PairPos2( &pp->ppf.ppf2, format1, format2, input );
      if ( error )
        goto Fail;
      break;

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    return TT_Err_Ok;

  Fail:
    Free_Coverage( &pp->Coverage );
    return error;
  }


  void  Free_PairPos( TTO_PairPos*  pp )
  {
    UShort  format1, format2;


    format1 = pp->ValueFormat1;
    format2 = pp->ValueFormat2;

    switch ( pp->PosFormat )
    {
    case 1:
      Free_PairPos1( &pp->ppf.ppf1, format1, format2 );
      break;

    case 2:
      Free_PairPos2( &pp->ppf.ppf2, format1, format2 );
      break;
    }

    Free_Coverage( &pp->Coverage );
  }


  static TT_Error  Lookup_PairPos1( GPOS_Instance*       gpi,
                                    TTO_PairPosFormat1*  ppf1,
                                    TTO_GSUB_String*     in,
                                    TTO_GPOS_Data*       out,
                                    UShort               first_pos,
                                    UShort               index,
                                    UShort               format1,
                                    UShort               format2 )
  {
    TT_Error              error;
    UShort                numpvr, glyph2;

    TTO_PairValueRecord*  pvr;


    if ( index >= ppf1->PairSetCount )
       return TTO_Err_Invalid_GPOS_SubTable;

    pvr = ppf1->PairSet[index].PairValueRecord;
    if ( !pvr )
      return TTO_Err_Invalid_GPOS_SubTable;

    glyph2 = in->string[in->pos];

    for ( numpvr = ppf1->PairSet[index].PairValueCount;
          numpvr;
          numpvr--, pvr++ )
    {
      if ( glyph2 == pvr->SecondGlyph )
      {
        error = Get_ValueRecord( gpi, &pvr->Value1, format1,
                                 &out[first_pos] );
        if ( error )
          return error;
        return Get_ValueRecord( gpi, &pvr->Value2, format2,
                                &out[in->pos] );
      }
    }

    return TTO_Err_Not_Covered;
  }


  static TT_Error  Lookup_PairPos2( GPOS_Instance*       gpi,
                                    TTO_PairPosFormat2*  ppf2,
                                    TTO_GSUB_String*     in,
                                    TTO_GPOS_Data*       out,
                                    UShort               first_pos,
                                    UShort               format1,
                                    UShort               format2 )
  {
    TT_Error           error;
    UShort             cl1, cl2;

    TTO_Class1Record*  c1r;
    TTO_Class2Record*  c2r;


    error = Get_Class( &ppf2->ClassDef1, in->string[first_pos],
                       &cl1, NULL );
    if ( error )
      return error;
    error = Get_Class( &ppf2->ClassDef2, in->string[in->pos],
                       &cl2, NULL );
    if ( error )
      return error;

    c1r = &ppf2->Class1Record[cl1];
    if ( !c1r )
      return TTO_Err_Invalid_GPOS_SubTable;
    c2r = &c1r->Class2Record[cl2];

    error = Get_ValueRecord( gpi, &c2r->Value1, format1, &out[first_pos] );
    if ( error )
      return error;
    return Get_ValueRecord( gpi, &c2r->Value2, format2, &out[in->pos] );
  }


  static TT_Error  Lookup_PairPos( GPOS_Instance*    gpi,
                                   TTO_PairPos*      pp,
                                   TTO_GSUB_String*  in,
                                   TTO_GPOS_Data*    out,
                                   UShort            flags,
                                   UShort            context_length )
  {
    TT_Error         error;
    UShort           index, property, first_pos;
    TTO_GPOSHeader*  gpos = gpi->gpos;


    if ( in->pos >= in->length - 1 )
      return TTO_Err_Not_Covered;           /* Not enough glyphs in input */

    if ( context_length != 0xFFFF && context_length < 2 )
      return TTO_Err_Not_Covered;

    if ( CHECK_Property( gpos->gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &pp->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    /* second glyph */

    first_pos = in->pos;
    (in->pos)++;

    while ( CHECK_Property( gpos->gdef, in->string[in->pos],
                            flags, &property ) )
    {
      if ( error && error != TTO_Err_Not_Covered )
        return error;

      if ( in->pos < in->length )
        (in->pos)++;
      else
        break;
    }

    switch ( pp->PosFormat )
    {
    case 1:
      error = Lookup_PairPos1( gpi, &pp->ppf.ppf1, in, out,
                               first_pos, index,
                               pp->ValueFormat1, pp->ValueFormat2 );
      break;

    case 2:
      error = Lookup_PairPos2( gpi, &pp->ppf.ppf2, in, out, first_pos,
                               pp->ValueFormat1, pp->ValueFormat2 );
      break;

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    /* adjusting the `next' glyph */

    if ( pp->ValueFormat2 )
      (in->pos)++;

    return error;
  }


  /* LookupType 3 */

  /* CursivePosFormat1 */

  TT_Error  Load_CursivePos( TTO_CursivePos*  cp,
                             PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    ULong                 cur_offset, new_offset, base_offset;

    TTO_EntryExitRecord*  eer;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    cp->PosFormat = GET_UShort();
    new_offset    = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &cp->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = cp->EntryExitCount = GET_UShort();

    FORGET_Frame();

    cp->EntryExitRecord = NULL;

    if ( ALLOC_ARRAY( cp->EntryExitRecord, count, TTO_EntryExitRecord ) )
      goto Fail2;

    eer = cp->EntryExitRecord;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        return error;

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Anchor( &eer[n].EntryAnchor,
                                    input ) ) != TT_Err_Ok )
          goto Fail1;
        (void)FILE_Seek( cur_offset );
      }
      else
        eer[n].EntryAnchor.PosFormat   = 0;

      if ( ACCESS_Frame( 2L ) )
        return error;

      new_offset = GET_UShort();

      FORGET_Frame();

      if ( new_offset )
      {
        new_offset += base_offset;

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Anchor( &eer[n].ExitAnchor,
                                    input ) ) != TT_Err_Ok )
          goto Fail1;
        (void)FILE_Seek( cur_offset );
      }
      else
        eer[n].ExitAnchor.PosFormat   = 0;
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
    {
      Free_Anchor( &eer[n].EntryAnchor );
      Free_Anchor( &eer[n].ExitAnchor );
    }

    FREE( eer );

  Fail2:
    Free_Coverage( &cp->Coverage );
    return error;
  }


  void  Free_CursivePos( TTO_CursivePos*  cp )
  {
    UShort                n, count;

    TTO_EntryExitRecord*  eer;


    if ( cp->EntryExitRecord )
    {
      count = cp->EntryExitCount;
      eer   = cp->EntryExitRecord;

      for ( n = 0; n < count; n++ )
      {
        Free_Anchor( &eer[n].EntryAnchor );
        Free_Anchor( &eer[n].ExitAnchor );
      }

      FREE( eer );
    }

    Free_Coverage( &cp->Coverage );
  }


  static TT_Error  Lookup_CursivePos( GPOS_Instance*    gpi,
                                      TTO_CursivePos*   cp,
                                      TTO_GSUB_String*  in,
                                      TTO_GPOS_Data*    out,
                                      UShort            flags,
                                      UShort            context_length )
  {
    UShort           index, property;
    TT_Error         error;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    TTO_EntryExitRecord*  eer;
    TT_Pos                entry_x, entry_y;
    TT_Pos                exit_x, exit_y;


    if ( context_length != 0xFFFF && context_length < 1 )
    {
      gpi->last = 0xFFFF;
      return TTO_Err_Not_Covered;
    }

    /* Glyphs not having the right GDEF properties will be ignored, i.e.,
       gpi->last won't be reset (contrary to user defined properties). */

    if ( CHECK_Property( gpos->gdef, in->string[in->pos], flags, &property ) )
      return error;

    /* We don't handle mark glyphs here.  According to Andrei, this isn't
       possible, but who knows...                                         */

    if ( property == MARK_GLYPH )
    {
      gpi->last = 0xFFFF;
      return TTO_Err_Not_Covered;
    }

    error = Coverage_Index( &cp->Coverage, in->string[in->pos], &index );
    if ( error )
    {
      gpi->last = 0xFFFF;
      return error;
    }

    if ( index >= cp->EntryExitCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    eer = &cp->EntryExitRecord[index];

    /* Now comes the messiest part of the whole OpenType
       specification.  At first glance, cursive connections seem easy
       to understand, but there are pitfalls!  The reason is that
       the specs don't mention how to compute the advance values
       resp. glyph offsets.  I was told it would be an omission, to
       be fixed in the next OpenType version...  Again many thanks to
       Andrei Burago <andreib@microsoft.com> for clarifications.

       Consider the following example:

                        |  xadv1    |
                         +---------+
                         |         |
                   +-----+--+ 1    |
                   |     | .|      |
                   |    0+--+------+
                   |   2    |
                   |        |
                  0+--------+
                  |  xadv2   |

         glyph1: advance width = 12
                 anchor point = (3,1)

         glyph2: advance width = 11
                 anchor point = (9,4)

         LSB is 1 for both glyphs (so the boxes drawn above are glyph
         bboxes).  Writing direction is R2L; `0' denotes the glyph's
         coordinate origin.

       Now the surprising part: The advance width of the *left* glyph
       (resp. of the *bottom* glyph) will be modified, no matter
       whether the writing direction is L2R or R2L (resp. T2B or
       B2T)!  This assymetry is caused by the fact that the glyph's
       coordinate origin is always the lower left corner for all
       writing directions.

       Continuing the above example, we can compute the new
       (horizontal) advance width of glyph2 as

         9 - 3 = 6  ,

       and the new vertical offset of glyph2 as

         1 - 4 = -3  .


       Vertical writing direction is far more complicated:

       a) Assuming that we recompute the advance height of the lower glyph:

                                    --
                         +---------+
                --       |         |
                   +-----+--+ 1    | yadv1
                   |     | .|      |
             yadv2 |    0+--+------+        -- BSB1  --
                   |   2    |       --      --        y_offset
                   |        |
     BSB2 --      0+--------+                        --
          --    --

         glyph1: advance height = 6
                 anchor point = (3,1)

         glyph2: advance height = 7
                 anchor point = (9,4)

         TSB is 1 for both glyphs; writing direction is T2B.


           BSB1     = yadv1 - (TSB1 + ymax1)
           BSB2     = yadv2 - (TSB2 + ymax2)
           y_offset = y2 - y1

         vertical advance width of glyph2
           = y_offset + BSB2 - BSB1
           = (y2 - y1) + (yadv2 - (TSB2 + ymax2)) - (yadv1 - (TSB1 + ymax1))
           = y2 - y1 + yadv2 - TSB2 - ymax2 - (yadv1 - TSB1 - ymax1)
           = y2 - y1 + yadv2 - TSB2 - ymax2 - yadv1 + TSB1 + ymax1


       b) Assuming that we recompute the advance height of the upper glyph:

                                    --      --
                         +---------+        -- TSB1
          --    --       |         |
     TSB2 --       +-----+--+ 1    | yadv1   ymax1
                   |     | .|      |
             yadv2 |    0+--+------+        --       --
      ymax2        |   2    |       --                y_offset
                   |        |
          --      0+--------+                        --
                --

         glyph1: advance height = 6
                 anchor point = (3,1)

         glyph2: advance height = 7
                 anchor point = (9,4)

         TSB is 1 for both glyphs; writing direction is T2B.

         y_offset = y2 - y1

         vertical advance width of glyph2
           = TSB1 + ymax1 + y_offset - (TSB2 + ymax2)
           = TSB1 + ymax1 + y2 - y1 - TSB2 - ymax2


       Comparing a) with b) shows that b) is easier to compute.  I'll wait
       for a reply from Andrei to see what should really be implemented...

       Since horizontal advance widths or vertical advance heights
       can be used alone but not together, no ambiguity occurs.        */

    if ( gpi->last == 0xFFFF )
      goto end;

    /* Get_Anchor() returns TTO_Err_Not_Covered if there is no anchor
       table.                                                         */

    error = Get_Anchor( gpi, &eer->EntryAnchor, in->string[in->pos],
                        &entry_x, &entry_y );
    if ( error == TTO_Err_Not_Covered )
      goto end;
    if ( error )
      return error;

    if ( gpi->r2l )
    {
      out[in->pos].x_advance   = entry_x - gpi->anchor_x;
      out[in->pos].new_advance = TRUE;
    }
    else
    {
      out[gpi->last].x_advance   = gpi->anchor_x - entry_x;
      out[gpi->last].new_advance = TRUE;
    }

    out[in->pos].y_pos = gpi->anchor_y - entry_y + out[gpi->last].y_pos;

  end:
    error = Get_Anchor( gpi, &eer->ExitAnchor, in->string[in->pos],
                        &exit_x, &exit_y );
    if ( error == TTO_Err_Not_Covered )
      gpi->last = 0xFFFF;
    else
    {
      if ( gpi->first == 0xFFFF )
        gpi->first  = in->pos;
      gpi->last     = in->pos;
      gpi->anchor_x = exit_x;
      gpi->anchor_y = exit_y;
    }
    if ( error )
      return error;

    (in->pos)++;

    return TT_Err_Ok;
  }


  /* LookupType 4 */

  /* BaseArray */

  static TT_Error  Load_BaseArray( TTO_BaseArray*  ba,
                                   UShort          num_classes,
                                   PFace           input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort           m, n, count;
    ULong            cur_offset, new_offset, base_offset;

    TTO_BaseRecord*  br;
    TTO_Anchor*      ban;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = ba->BaseCount = GET_UShort();
    
    FORGET_Frame();

    ba->BaseRecord = NULL;

    if ( ALLOC_ARRAY( ba->BaseRecord, count, TTO_BaseRecord ) )
      return error;

    br = ba->BaseRecord;

    for ( m = 0; m < count; m++ )
    {
      br[m].BaseAnchor = NULL;

      if ( ALLOC_ARRAY( br[m].BaseAnchor, num_classes, TTO_Anchor ) )
        goto Fail;

      ban = br[m].BaseAnchor;

      for ( n = 0; n < num_classes; n++ )
      {
        if ( ACCESS_Frame( 2L ) )
          goto Fail;

        new_offset = GET_UShort() + base_offset;

        FORGET_Frame();

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Anchor( &ban[n], input ) ) != TT_Err_Ok )
          goto Fail;
        (void)FILE_Seek( cur_offset );
      }
    }

    return TT_Err_Ok;

  Fail:
    for ( m = 0; m < count; m++ )
    {
      ban = br[m].BaseAnchor;
      
      for ( n = 0; n < num_classes; n++ )
        Free_Anchor( &ban[n] );

      FREE( ban );
    }

    FREE( br );
    return error;
  }


  static void  Free_BaseArray( TTO_BaseArray*  ba,
                               UShort          num_classes )
  {
    UShort           m, n, count;

    TTO_BaseRecord*  br;
    TTO_Anchor*      ban;


    if ( ba->BaseRecord )
    {
      count = ba->BaseCount;
      br    = ba->BaseRecord;

      for ( m = 0; m < count; m++ )
      {
        ban = br[m].BaseAnchor;

        for ( n = 0; n < num_classes; n++ )
          Free_Anchor( &ban[n] );

        FREE( ban );
      }

      FREE( br );
    }
  }


  /* MarkBasePosFormat1 */

  TT_Error  Load_MarkBasePos( TTO_MarkBasePos*  mbp,
                              PFace             input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    ULong  cur_offset, new_offset, base_offset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    mbp->PosFormat = GET_UShort();
    new_offset     = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &mbp->MarkCoverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &mbp->BaseCoverage, input ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 4L ) )
      goto Fail2;

    mbp->ClassCount = GET_UShort();
    new_offset      = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_MarkArray( &mbp->MarkArray, input ) ) != TT_Err_Ok )
      goto Fail2;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail1;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_BaseArray( &mbp->BaseArray, mbp->ClassCount,
                                   input ) ) != TT_Err_Ok )
      goto Fail1;

    return TT_Err_Ok;

  Fail1:
    Free_MarkArray( &mbp->MarkArray );

  Fail2:
    Free_Coverage( &mbp->BaseCoverage );

  Fail3:
    Free_Coverage( &mbp->MarkCoverage );
    return error;
  }


  void  Free_MarkBasePos( TTO_MarkBasePos*  mbp )
  {
    Free_BaseArray( &mbp->BaseArray, mbp->ClassCount );
    Free_MarkArray( &mbp->MarkArray );
    Free_Coverage( &mbp->BaseCoverage );
    Free_Coverage( &mbp->MarkCoverage );
  }


  static TT_Error  Lookup_MarkBasePos( GPOS_Instance*    gpi,
                                       TTO_MarkBasePos*  mbp,
                                       TTO_GSUB_String*  in,
                                       TTO_GPOS_Data*    out,
                                       UShort            flags,
                                       UShort            context_length )
  {
    TT_UInt          i, j;
    UShort           mark_index, base_index, property, class;
    TT_Pos           x_mark_value, y_mark_value, x_base_value, y_base_value;
    TT_Error         error;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    TTO_MarkArray*   ma;
    TTO_BaseArray*   ba;
    TTO_BaseRecord*  br;
    TTO_Anchor*      mark_anchor;
    TTO_Anchor*      base_anchor;

    TTO_GPOS_Data*   o;


    if ( context_length != 0xFFFF && context_length < 1 )
      return TTO_Err_Not_Covered;

    if ( flags & IGNORE_BASE_GLYPHS )
      return TTO_Err_Not_Covered;

    if ( CHECK_Property( gpos->gdef, in->string[in->pos],
                         flags, &property ) )
      return error;

    error = Coverage_Index( &mbp->MarkCoverage, in->string[in->pos],
                            &mark_index );
    if ( error )
      return error;

    /* now we search backwards for a non-mark glyph */

    i = 1;
    j = in->pos - 1;

    while ( i <= in->pos )
    {
      error = TT_GDEF_Get_Glyph_Property( gpos->gdef, in->string[j],
                                          &property );
      if ( error )
        return error;

      if ( !( property == TTO_MARK || property & IGNORE_SPECIAL_MARKS ) )
        break;

      i++;
      j--;
    }

    /* The following assertion is too strong -- at least for mangal.ttf. */
#if 0
    if ( property != TTO_BASE_GLYPH )
      return TTO_Err_Not_Covered;
#endif

    if ( i > in->pos )
      return TTO_Err_Not_Covered;

    error = Coverage_Index( &mbp->BaseCoverage, in->string[j],
                            &base_index );
    if ( error )
      return error;

    ma = &mbp->MarkArray;

    if ( mark_index >= ma->MarkCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    class       = ma->MarkRecord[mark_index].Class;
    mark_anchor = &ma->MarkRecord[mark_index].MarkAnchor;

    if ( class >= mbp->ClassCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    ba = &mbp->BaseArray;

    if ( base_index >= ba->BaseCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    br          = &ba->BaseRecord[base_index];
    base_anchor = &br->BaseAnchor[class];

    error = Get_Anchor( gpi, mark_anchor, in->string[in->pos],
                        &x_mark_value, &y_mark_value );
    if ( error )
      return error;
    error = Get_Anchor( gpi, base_anchor, in->string[j],
                        &x_base_value, &y_base_value );
    if ( error )
      return error;

    /* anchor points are not cumulative */

    o = &out[in->pos];

    o->x_pos     = x_base_value - x_mark_value;
    o->y_pos     = y_base_value - y_mark_value;
    o->x_advance = 0;
    o->y_advance = 0;
    o->back      = i;

    (in->pos)++;

    return TT_Err_Ok;
  }


  /* LookupType 5 */

  /* LigatureAttach */

  static TT_Error  Load_LigatureAttach( TTO_LigatureAttach*  lat,
                                        UShort               num_classes,
                                        PFace                input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                m, n, count;
    ULong                 cur_offset, new_offset, base_offset;

    TTO_ComponentRecord*  cr;
    TTO_Anchor*           lan;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = lat->ComponentCount = GET_UShort();
    
    FORGET_Frame();

    lat->ComponentRecord = NULL;

    if ( ALLOC_ARRAY( lat->ComponentRecord, count, TTO_ComponentRecord ) )
      return error;

    cr = lat->ComponentRecord;

    for ( m = 0; m < count; m++ )
    {
      cr[m].LigatureAnchor = NULL;

      if ( ALLOC_ARRAY( cr[m].LigatureAnchor, num_classes, TTO_Anchor ) )
        goto Fail;

      lan = cr[m].LigatureAnchor;

      for ( n = 0; n < num_classes; n++ )
      {
        if ( ACCESS_Frame( 2L ) )
          goto Fail;

        new_offset = GET_UShort();

        FORGET_Frame();

        if ( new_offset )
        {
          new_offset += base_offset;

          cur_offset = FILE_Pos();
          if ( FILE_Seek( new_offset ) ||
               ( error = Load_Anchor( &lan[n], input ) ) != TT_Err_Ok )
            goto Fail;
          (void)FILE_Seek( cur_offset );
        }
        else
          lan[n].PosFormat = 0;
      }
    }

    return TT_Err_Ok;

  Fail:
    for ( m = 0; m < count; m++ )
    {
      lan = cr[m].LigatureAnchor;
      
      for ( n = 0; n < num_classes; n++ )
        Free_Anchor( &lan[n] );

      FREE( lan );
    }

    FREE( cr );
    return error;
  }


  static void  Free_LigatureAttach( TTO_LigatureAttach*  lat,
                                    UShort               num_classes )
  {
    UShort           m, n, count;

    TTO_ComponentRecord*  cr;
    TTO_Anchor*           lan;


    if ( lat->ComponentRecord )
    {
      count = lat->ComponentCount;
      cr    = lat->ComponentRecord;

      for ( m = 0; m < count; m++ )
      {
        lan = cr[m].LigatureAnchor;

        for ( n = 0; n < num_classes; n++ )
          Free_Anchor( &lan[n] );

        FREE( lan );
      }

      FREE( cr );
    }
  }


  /* LigatureArray */

  static TT_Error  Load_LigatureArray( TTO_LigatureArray*  la,
                                       UShort              num_classes,
                                       PFace               input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort               n, count;
    ULong                cur_offset, new_offset, base_offset;

    TTO_LigatureAttach*  lat;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = la->LigatureCount = GET_UShort();

    FORGET_Frame();

    la->LigatureAttach = NULL;

    if ( ALLOC_ARRAY( la->LigatureAttach, count, TTO_LigatureAttach ) )
      return error;

    lat = la->LigatureAttach;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_LigatureAttach( &lat[n], num_classes,
                                          input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_LigatureAttach( &lat[n], num_classes );

    FREE( lat );
    return error;
  }


  static void  Free_LigatureArray( TTO_LigatureArray*  la,
                                   UShort              num_classes )
  {
    UShort               n, count;

    TTO_LigatureAttach*  lat;


    if ( la->LigatureAttach )
    {
      count = la->LigatureCount;
      lat   = la->LigatureAttach;

      for ( n = 0; n < count; n++ )
        Free_LigatureAttach( &lat[n], num_classes );

      FREE( lat );
    }
  }


  /* MarkLigPosFormat1 */

  TT_Error  Load_MarkLigPos( TTO_MarkLigPos*  mlp,
                             PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    ULong  cur_offset, new_offset, base_offset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    mlp->PosFormat = GET_UShort();
    new_offset     = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &mlp->MarkCoverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &mlp->LigatureCoverage,
                                  input ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 4L ) )
      goto Fail2;

    mlp->ClassCount = GET_UShort();
    new_offset      = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_MarkArray( &mlp->MarkArray, input ) ) != TT_Err_Ok )
      goto Fail2;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail1;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_LigatureArray( &mlp->LigatureArray, mlp->ClassCount,
                                       input ) ) != TT_Err_Ok )
      goto Fail1;

    return TT_Err_Ok;

  Fail1:
    Free_MarkArray( &mlp->MarkArray );

  Fail2:
    Free_Coverage( &mlp->LigatureCoverage );

  Fail3:
    Free_Coverage( &mlp->MarkCoverage );
    return error;
  }


  void  Free_MarkLigPos( TTO_MarkLigPos*  mlp )
  {
    Free_LigatureArray( &mlp->LigatureArray, mlp->ClassCount );
    Free_MarkArray( &mlp->MarkArray );
    Free_Coverage( &mlp->LigatureCoverage );
    Free_Coverage( &mlp->MarkCoverage );
  }


  static TT_Error  Lookup_MarkLigPos( GPOS_Instance*    gpi,
                                      TTO_MarkLigPos*   mlp,
                                      TTO_GSUB_String*  in,
                                      TTO_GPOS_Data*    out,
                                      UShort            flags,
                                      UShort            context_length )
  {
    TT_UInt          i, j;
    UShort           mark_index, lig_index, property, class;
    UShort           mark_glyph;
    TT_Pos           x_mark_value, y_mark_value, x_lig_value, y_lig_value;
    TT_Error         error;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    TTO_MarkArray*        ma;
    TTO_LigatureArray*    la;
    TTO_LigatureAttach*   lat;
    TTO_ComponentRecord*  cr;
    UShort                comp_index;
    TTO_Anchor*           mark_anchor;
    TTO_Anchor*           lig_anchor;

    TTO_GPOS_Data*  o;


    if ( context_length != 0xFFFF && context_length < 1 )
      return TTO_Err_Not_Covered;

    if ( flags & IGNORE_LIGATURES )
      return TTO_Err_Not_Covered;

    mark_glyph = in->string[in->pos];

    if ( CHECK_Property( gpos->gdef, mark_glyph, flags, &property ) )
      return error;

    error = Coverage_Index( &mlp->MarkCoverage, mark_glyph, &mark_index );
    if ( error )
      return error;

    /* now we search backwards for a non-mark glyph */

    i = 1;
    j = in->pos - 1;

    while ( i <= in->pos )
    {
      error = TT_GDEF_Get_Glyph_Property( gpos->gdef, in->string[j],
                                          &property );
      if ( error )
        return error;

      if ( !( property == TTO_MARK || property & IGNORE_SPECIAL_MARKS ) )
        break;

      i++;
      j--;
    }

    /* Similar to Lookup_MarkBasePos(), I suspect that this assertion is
       too strong, thus it is commented out.                             */
#if 0
    if ( property != TTO_LIGATURE )
      return TTO_Err_Not_Covered;
#endif

    if ( i > in->pos )
      return TTO_Err_Not_Covered;

    error = Coverage_Index( &mlp->LigatureCoverage, in->string[j],
                            &lig_index );
    if ( error )
      return error;

    ma = &mlp->MarkArray;

    if ( mark_index >= ma->MarkCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    class       = ma->MarkRecord[mark_index].Class;
    mark_anchor = &ma->MarkRecord[mark_index].MarkAnchor;

    if ( class >= mlp->ClassCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    la = &mlp->LigatureArray;

    if ( lig_index >= la->LigatureCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    lat = &la->LigatureAttach[lig_index];

    /* We must now check whether the ligature ID of the current mark glyph
       is identical to the ligature ID of the found ligature.  If yes, we
       can directly use the component index.  If not, we attach the mark
       glyph to the last component of the ligature.                        */

    if ( in->ligIDs && in->components &&
         in->ligIDs[j] == in->ligIDs[in->pos] )
    {
      comp_index = in->components[in->pos];
      if ( comp_index >= lat->ComponentCount )
        return TTO_Err_Not_Covered;
    }
    else
      comp_index = lat->ComponentCount - 1;

    cr         = &lat->ComponentRecord[comp_index];
    lig_anchor = &cr->LigatureAnchor[class];

    error = Get_Anchor( gpi, mark_anchor, in->string[in->pos],
                        &x_mark_value, &y_mark_value );
    if ( error )
      return error;
    error = Get_Anchor( gpi, lig_anchor, in->string[j],
                        &x_lig_value, &y_lig_value );
    if ( error )
      return error;

    /* anchor points are not cumulative */

    o = &out[in->pos];

    o->x_pos     = x_lig_value - x_mark_value;
    o->y_pos     = y_lig_value - y_mark_value;
    o->x_advance = 0;
    o->y_advance = 0;
    o->back      = i;

    (in->pos)++;

    return TT_Err_Ok;
  }


  /* LookupType 6 */

  /* Mark2Array */

  static TT_Error  Load_Mark2Array( TTO_Mark2Array*  m2a,
                                    UShort           num_classes,
                                    PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            m, n, count;
    ULong             cur_offset, new_offset, base_offset;

    TTO_Mark2Record*  m2r;
    TTO_Anchor*       m2an;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = m2a->Mark2Count = GET_UShort();
    
    FORGET_Frame();

    m2a->Mark2Record = NULL;

    if ( ALLOC_ARRAY( m2a->Mark2Record, count, TTO_Mark2Record ) )
      return error;

    m2r = m2a->Mark2Record;

    for ( m = 0; m < count; m++ )
    {
      m2r[m].Mark2Anchor = NULL;

      if ( ALLOC_ARRAY( m2r[m].Mark2Anchor, num_classes, TTO_Anchor ) )
        goto Fail;

      m2an = m2r[m].Mark2Anchor;

      for ( n = 0; n < num_classes; n++ )
      {
        if ( ACCESS_Frame( 2L ) )
          goto Fail;

        new_offset = GET_UShort() + base_offset;

        FORGET_Frame();

        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_Anchor( &m2an[n], input ) ) != TT_Err_Ok )
          goto Fail;
        (void)FILE_Seek( cur_offset );
      }
    }

    return TT_Err_Ok;

  Fail:
    for ( m = 0; m < count; m++ )
    {
      m2an = m2r[m].Mark2Anchor;
      
      for ( n = 0; n < num_classes; n++ )
        Free_Anchor( &m2an[n] );

      FREE( m2an );
    }

    FREE( m2r );
    return error;
  }


  static void  Free_Mark2Array( TTO_Mark2Array*  m2a,
                                UShort           num_classes )
  {
    UShort            m, n, count;

    TTO_Mark2Record*  m2r;
    TTO_Anchor*       m2an;


    if ( m2a->Mark2Record )
    {
      count = m2a->Mark2Count;
      m2r   = m2a->Mark2Record;

      for ( m = 0; m < count; m++ )
      {
        m2an = m2r[m].Mark2Anchor;

        for ( n = 0; n < num_classes; n++ )
          Free_Anchor( &m2an[n] );

        FREE( m2an );
      }

      FREE( m2r );
    }
  }


  /* MarkMarkPosFormat1 */

  TT_Error  Load_MarkMarkPos( TTO_MarkMarkPos*  mmp,
                              PFace             input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    ULong  cur_offset, new_offset, base_offset;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 4L ) )
      return error;

    mmp->PosFormat = GET_UShort();
    new_offset     = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &mmp->Mark1Coverage,
                                  input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &mmp->Mark2Coverage,
                                  input ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 4L ) )
      goto Fail2;

    mmp->ClassCount = GET_UShort();
    new_offset      = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_MarkArray( &mmp->Mark1Array, input ) ) != TT_Err_Ok )
      goto Fail2;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail1;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Mark2Array( &mmp->Mark2Array, mmp->ClassCount,
                                    input ) ) != TT_Err_Ok )
      goto Fail1;

    return TT_Err_Ok;

  Fail1:
    Free_MarkArray( &mmp->Mark1Array );

  Fail2:
    Free_Coverage( &mmp->Mark2Coverage );

  Fail3:
    Free_Coverage( &mmp->Mark1Coverage );
    return error;
  }


  void  Free_MarkMarkPos( TTO_MarkMarkPos*  mmp )
  {
    Free_Mark2Array( &mmp->Mark2Array, mmp->ClassCount );
    Free_MarkArray( &mmp->Mark1Array );
    Free_Coverage( &mmp->Mark2Coverage );
    Free_Coverage( &mmp->Mark1Coverage );
  }


  static TT_Error  Lookup_MarkMarkPos( GPOS_Instance*    gpi,
                                       TTO_MarkMarkPos*  mmp,
                                       TTO_GSUB_String*  in,
                                       TTO_GPOS_Data*    out,
                                       UShort            flags,
                                       UShort            context_length )
  {
    UShort           j, mark1_index, mark2_index, property, class;
    TT_Pos           x_mark1_value, y_mark1_value,
                     x_mark2_value, y_mark2_value;
    TT_Error         error;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    TTO_MarkArray*    ma1;
    TTO_Mark2Array*   ma2;
    TTO_Mark2Record*  m2r;
    TTO_Anchor*       mark1_anchor;
    TTO_Anchor*       mark2_anchor;

    TTO_GPOS_Data*  o;


    if ( context_length != 0xFFFF && context_length < 1 )
      return TTO_Err_Not_Covered;

    if ( flags & IGNORE_MARKS )
      return TTO_Err_Not_Covered;

    if ( CHECK_Property( gpos->gdef, in->string[in->pos],
                         flags, &property ) )
      return error;

    error = Coverage_Index( &mmp->Mark1Coverage, in->string[in->pos],
                            &mark1_index );
    if ( error )
      return error;

    /* now we check the preceding glyph whether it is a suitable
       mark glyph                                                */

    if ( in->pos == 0 )
      return TTO_Err_Not_Covered;

    j = in->pos - 1;
    error = TT_GDEF_Get_Glyph_Property( gpos->gdef, in->string[j],
                                        &property );
    if ( error )
      return error;

    if ( flags & IGNORE_SPECIAL_MARKS )
    {
      if ( property != (flags & 0xFF00) )
        return TTO_Err_Not_Covered;
    }
    else
    {
      if ( property != TTO_MARK )
        return TTO_Err_Not_Covered;
    }

    error = Coverage_Index( &mmp->Mark2Coverage, in->string[j],
                            &mark2_index );
    if ( error )
      return error;

    ma1 = &mmp->Mark1Array;

    if ( mark1_index >= ma1->MarkCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    class        = ma1->MarkRecord[mark1_index].Class;
    mark1_anchor = &ma1->MarkRecord[mark1_index].MarkAnchor;

    if ( class >= mmp->ClassCount )
      return TTO_Err_Invalid_GPOS_SubTable;

    ma2 = &mmp->Mark2Array;

    if ( mark2_index >= ma2->Mark2Count )
      return TTO_Err_Invalid_GPOS_SubTable;

    m2r          = &ma2->Mark2Record[mark2_index];
    mark2_anchor = &m2r->Mark2Anchor[class];

    error = Get_Anchor( gpi, mark1_anchor, in->string[in->pos],
                        &x_mark1_value, &y_mark1_value );
    if ( error )
      return error;
    error = Get_Anchor( gpi, mark2_anchor, in->string[j],
                        &x_mark2_value, &y_mark2_value );
    if ( error )
      return error;

    /* anchor points are not cumulative */

    o = &out[in->pos];

    o->x_pos     = x_mark2_value - x_mark1_value;
    o->y_pos     = y_mark2_value - y_mark1_value;
    o->x_advance = 0;
    o->y_advance = 0;
    o->back      = 1;

    (in->pos)++;

    return TT_Err_Ok;
  }


  /* Do the actual positioning for a context positioning (either format
     7 or 8).  This is only called after we've determined that the input
     matches the subrule.                                                 */

  static TT_Error  Do_ContextPos( GPOS_Instance*        gpi,
                                  UShort                GlyphCount,
                                  UShort                PosCount,
                                  TTO_PosLookupRecord*  pos,
                                  TTO_GSUB_String*      in,
                                  TTO_GPOS_Data*        out,
                                  int                   nesting_level )
  {
    TT_Error  error;
    UShort    i, old_pos;


    i = 0;

    while ( i < GlyphCount )
    {
      if ( PosCount && i == pos->SequenceIndex )
      {
        old_pos = in->pos;

        /* Do a positioning */

        error = Do_Glyph_Lookup( gpi, pos->LookupListIndex, in, out,
                                 GlyphCount, nesting_level );

        if ( error )
          return error;

        pos++;
        PosCount--;
        i += in->pos - old_pos;
      }
      else
      {
        i++;
        (in->pos)++;
      }
    }

    return TT_Err_Ok;
  }


  /* LookupType 7 */

  /* PosRule */

  static TT_Error  Load_PosRule( TTO_PosRule*  pr,
                                 PFace         input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    UShort*               i;

    TTO_PosLookupRecord*  plr;


    if ( ACCESS_Frame( 4L ) )
      return error;

    pr->GlyphCount = GET_UShort();
    pr->PosCount   = GET_UShort();

    FORGET_Frame();

    pr->Input = NULL;

    count = pr->GlyphCount - 1;         /* only GlyphCount - 1 elements */

    if ( ALLOC_ARRAY( pr->Input, count, UShort ) )
      return error;

    i = pr->Input;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
      i[n] = GET_UShort();

    FORGET_Frame();

    pr->PosLookupRecord = NULL;

    count = pr->PosCount;

    if ( ALLOC_ARRAY( pr->PosLookupRecord, count, TTO_PosLookupRecord ) )
      goto Fail2;

    plr = pr->PosLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      plr[n].SequenceIndex   = GET_UShort();
      plr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( plr );

  Fail2:
    FREE( i );
    return error;
  }


  static void  Free_PosRule( TTO_PosRule*  pr )
  {
    FREE( pr->PosLookupRecord );
    FREE( pr->Input );
  }


  /* PosRuleSet */

  static TT_Error  Load_PosRuleSet( TTO_PosRuleSet*  prs,
                                    PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort        n, count;
    ULong         cur_offset, new_offset, base_offset;

    TTO_PosRule*  pr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = prs->PosRuleCount = GET_UShort();

    FORGET_Frame();

    prs->PosRule = NULL;

    if ( ALLOC_ARRAY( prs->PosRule, count, TTO_PosRule ) )
      return error;

    pr = prs->PosRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_PosRule( &pr[n], input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_PosRule( &pr[n] );

    FREE( pr );
    return error;
  }


  static void  Free_PosRuleSet( TTO_PosRuleSet*  prs )
  {
    UShort        n, count;

    TTO_PosRule*  pr;


    if ( prs->PosRule )
    {
      count = prs->PosRuleCount;
      pr    = prs->PosRule;

      for ( n = 0; n < count; n++ )
        Free_PosRule( &pr[n] );

      FREE( pr );
    }
  }


  /* ContextPosFormat1 */

  static TT_Error  Load_ContextPos1( TTO_ContextPosFormat1*  cpf1,
                                     PFace                   input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort           n, count;
    ULong            cur_offset, new_offset, base_offset;

    TTO_PosRuleSet*  prs;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &cpf1->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = cpf1->PosRuleSetCount = GET_UShort();

    FORGET_Frame();

    cpf1->PosRuleSet = NULL;

    if ( ALLOC_ARRAY( cpf1->PosRuleSet, count, TTO_PosRuleSet ) )
      goto Fail2;

    prs = cpf1->PosRuleSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_PosRuleSet( &prs[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_PosRuleSet( &prs[n] );

    FREE( prs );

  Fail2:
    Free_Coverage( &cpf1->Coverage );
    return error;
  }


  static void  Free_Context1( TTO_ContextPosFormat1*  cpf1 )
  {
    UShort           n, count;

    TTO_PosRuleSet*  prs;


    if ( cpf1->PosRuleSet )
    {
      count = cpf1->PosRuleSetCount;
      prs   = cpf1->PosRuleSet;

      for ( n = 0; n < count; n++ )
        Free_PosRuleSet( &prs[n] );

      FREE( prs );
    }

    Free_Coverage( &cpf1->Coverage );
  }


  /* PosClassRule */

  static TT_Error  Load_PosClassRule( TTO_ContextPosFormat2*  cpf2,
                                      TTO_PosClassRule*       pcr,
                                      PFace                   input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;

    UShort*               c;
    TTO_PosLookupRecord*  plr;
    Bool*                 d;


    if ( ACCESS_Frame( 4L ) )
      return error;

    pcr->GlyphCount = GET_UShort();
    pcr->PosCount   = GET_UShort();

    FORGET_Frame();

    if ( pcr->GlyphCount > cpf2->MaxContextLength )
      cpf2->MaxContextLength = pcr->GlyphCount;

    pcr->Class = NULL;

    count = pcr->GlyphCount - 1;        /* only GlyphCount - 1 elements */

    if ( ALLOC_ARRAY( pcr->Class, count, UShort ) )
      return error;

    c = pcr->Class;
    d = cpf2->ClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
    {
      c[n] = GET_UShort();

      /* We check whether the specific class is used at all.  If not,
         class 0 is used instead.                                     */

      if ( !d[c[n]] )
        c[n] = 0;
    }

    FORGET_Frame();

    pcr->PosLookupRecord = NULL;

    count = pcr->PosCount;

    if ( ALLOC_ARRAY( pcr->PosLookupRecord, count, TTO_PosLookupRecord ) )
      goto Fail2;

    plr = pcr->PosLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      plr[n].SequenceIndex   = GET_UShort();
      plr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( plr );

  Fail2:
    FREE( c );
    return error;
  }


  static void  Free_PosClassRule( TTO_PosClassRule*  pcr )
  {
    FREE( pcr->PosLookupRecord );
    FREE( pcr->Class );
  }


  /* PosClassSet */

  static TT_Error  Load_PosClassSet( TTO_ContextPosFormat2*  cpf2,
                                     TTO_PosClassSet*        pcs,
                                     PFace                   input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort             n, count;
    ULong              cur_offset, new_offset, base_offset;

    TTO_PosClassRule*  pcr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = pcs->PosClassRuleCount = GET_UShort();

    FORGET_Frame();

    pcs->PosClassRule = NULL;

    if ( ALLOC_ARRAY( pcs->PosClassRule, count, TTO_PosClassRule ) )
      return error;

    pcr = pcs->PosClassRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_PosClassRule( cpf2, &pcr[n],
                                        input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_PosClassRule( &pcr[n] );

    FREE( pcr );
    return error;
  }


  static void  Free_PosClassSet( TTO_PosClassSet*  pcs )
  {
    UShort             n, count;

    TTO_PosClassRule*  pcr;


    if ( pcs->PosClassRule )
    {
      count = pcs->PosClassRuleCount;
      pcr   = pcs->PosClassRule;

      for ( n = 0; n < count; n++ )
        Free_PosClassRule( &pcr[n] );

      FREE( pcr );
    }
  }


  /* ContextPosFormat2 */

  static TT_Error  Load_ContextPos2( TTO_ContextPosFormat2*  cpf2,
                                     PFace                   input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort            n, count;
    ULong             cur_offset, new_offset, base_offset;

    TTO_PosClassSet*  pcs;


    base_offset = FILE_Pos() - 2;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &cpf2->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 4L ) )
      goto Fail3;

    new_offset = GET_UShort() + base_offset;

    /* `PosClassSetCount' is the upper limit for class values, thus we
       read it now to make an additional safety check.                 */

    count = cpf2->PosClassSetCount = GET_UShort();

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_ClassDefinition( &cpf2->ClassDef, count,
                                         input ) ) != TT_Err_Ok )
      goto Fail3;
    (void)FILE_Seek( cur_offset );

    cpf2->PosClassSet      = NULL;
    cpf2->MaxContextLength = 0;

    if ( ALLOC_ARRAY( cpf2->PosClassSet, count, TTO_PosClassSet ) )
      goto Fail2;

    pcs = cpf2->PosClassSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      if ( new_offset != base_offset )      /* not a NULL offset */
      {
        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_PosClassSet( cpf2, &pcs[n],
                                         input ) ) != TT_Err_Ok )
          goto Fail1;
        (void)FILE_Seek( cur_offset );
      }
      else
      {
        /* we create a PosClassSet table with no entries */

        cpf2->PosClassSet[n].PosClassRuleCount = 0;
        cpf2->PosClassSet[n].PosClassRule      = NULL;
      }
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_PosClassSet( &pcs[n] );

    FREE( pcs );

  Fail2:
    Free_ClassDefinition( &cpf2->ClassDef );

  Fail3:
    Free_Coverage( &cpf2->Coverage );
    return error;
  }


  static void  Free_Context2( TTO_ContextPosFormat2*  cpf2 )
  {
    UShort            n, count;

    TTO_PosClassSet*  pcs;


    if ( cpf2->PosClassSet )
    {
      count = cpf2->PosClassSetCount;
      pcs   = cpf2->PosClassSet;

      for ( n = 0; n < count; n++ )
        Free_PosClassSet( &pcs[n] );

      FREE( pcs );
    }

    Free_ClassDefinition( &cpf2->ClassDef );
    Free_Coverage( &cpf2->Coverage );
  }


  /* ContextPosFormat3 */

  static TT_Error  Load_ContextPos3( TTO_ContextPosFormat3*  cpf3,
                                     PFace                   input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    ULong                 cur_offset, new_offset, base_offset;

    TTO_Coverage*         c;
    TTO_PosLookupRecord*  plr;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 4L ) )
      return error;

    cpf3->GlyphCount = GET_UShort();
    cpf3->PosCount   = GET_UShort();

    FORGET_Frame();

    cpf3->Coverage = NULL;

    count = cpf3->GlyphCount;

    if ( ALLOC_ARRAY( cpf3->Coverage, count, TTO_Coverage ) )
      return error;

    c = cpf3->Coverage;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &c[n], input ) ) != TT_Err_Ok )
        goto Fail2;
      (void)FILE_Seek( cur_offset );
    }

    cpf3->PosLookupRecord = NULL;

    count = cpf3->PosCount;

    if ( ALLOC_ARRAY( cpf3->PosLookupRecord, count, TTO_PosLookupRecord ) )
      goto Fail2;

    plr = cpf3->PosLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      plr[n].SequenceIndex   = GET_UShort();
      plr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( plr );

  Fail2:
    for ( n = 0; n < count; n++ )
      Free_Coverage( &c[n] );

    FREE( c );
    return error;
  }


  static void  Free_Context3( TTO_ContextPosFormat3*  cpf3 )
  {
    UShort         n, count;

    TTO_Coverage*  c;


    FREE( cpf3->PosLookupRecord );

    if ( cpf3->Coverage )
    {
      count = cpf3->GlyphCount;
      c     = cpf3->Coverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }
  }


  /* ContextPos */

  TT_Error  Load_ContextPos( TTO_ContextPos*  cp,
                             PFace            input )
  {
    DEFINE_LOAD_LOCALS( input->stream );


    if ( ACCESS_Frame( 2L ) )
      return error;

    cp->PosFormat = GET_UShort();

    FORGET_Frame();

    switch ( cp->PosFormat )
    {
    case 1:
      return Load_ContextPos1( &cp->cpf.cpf1, input );

    case 2:
      return Load_ContextPos2( &cp->cpf.cpf2, input );

    case 3:
      return Load_ContextPos3( &cp->cpf.cpf3, input );

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }


  void  Free_ContextPos( TTO_ContextPos*  cp )
  {
    switch ( cp->PosFormat )
    {
    case 1:
      Free_Context1( &cp->cpf.cpf1 );
      break;

    case 2:
      Free_Context2( &cp->cpf.cpf2 );
      break;

    case 3:
      Free_Context3( &cp->cpf.cpf3 );
      break;
    }
  }


  static TT_Error  Lookup_ContextPos1( GPOS_Instance*          gpi,
                                       TTO_ContextPosFormat1*  cpf1,
                                       TTO_GSUB_String*        in,
                                       TTO_GPOS_Data*          out,
                                       UShort                  flags,
                                       UShort                  context_length,
                                       int                     nesting_level )
  {
    UShort           index, property;
    UShort           i, j, k, numpr;
    TT_Error         error;
    UShort*          s_in;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    TTO_PosRule*     pr;
    TTO_GDEFHeader*  gdef;


    gdef = gpos->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &cpf1->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    pr    = cpf1->PosRuleSet[index].PosRule;
    numpr = cpf1->PosRuleSet[index].PosRuleCount;

    for ( k = 0; k < numpr; k++ )
    {
      if ( context_length != 0xFFFF && context_length < pr[k].GlyphCount )
        continue;

      if ( in->pos + pr[k].GlyphCount > in->length )
        continue;                           /* context is too long */

      s_in = &in->string[in->pos];

      for ( i = 1, j = 1; i < pr[k].GlyphCount; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( in->pos + j < in->length )
            j++;
          else
            break;
        }

        if ( s_in[j] != pr[k].Input[i - 1] )
          break;
      }

      if ( i == pr[k].GlyphCount )
        return Do_ContextPos( gpi, pr[k].GlyphCount,
                              pr[k].PosCount, pr[k].PosLookupRecord,
                              in, out,
                              nesting_level );
    }

    return TTO_Err_Not_Covered;
  }


  static TT_Error  Lookup_ContextPos2( GPOS_Instance*          gpi,
                                       TTO_ContextPosFormat2*  cpf2,
                                       TTO_GSUB_String*        in,
                                       TTO_GPOS_Data*          out,
                                       UShort                  flags,
                                       UShort                  context_length,
                                       int                     nesting_level )
  {
    UShort             index, property;
    TT_Error           error;
    UShort             i, j, k, known_classes;

    UShort*            classes;
    UShort*            s_in;
    UShort*            cl;
    TTO_GPOSHeader*    gpos = gpi->gpos;

    TTO_PosClassSet*   pcs;
    TTO_PosClassRule*  pr;
    TTO_GDEFHeader*    gdef;


    gdef = gpos->gdef;

    if ( ALLOC_ARRAY( classes, cpf2->MaxContextLength, UShort ) )
      return error;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    /* Note: The coverage table in format 2 doesn't give an index into
             anything.  It just lets us know whether or not we need to
             do any lookup at all.                                     */

    error = Coverage_Index( &cpf2->Coverage, in->string[in->pos], &index );
    if ( error )
      goto End;

    error = Get_Class( &cpf2->ClassDef, in->string[in->pos],
                       &classes[0], NULL );
    if ( error )
      goto End;
    known_classes = 0;

    pcs = &cpf2->PosClassSet[classes[0]];
    if ( !pcs )
    {
      error = TTO_Err_Invalid_GPOS_SubTable;
      goto End;
    }

    for ( k = 0; k < pcs->PosClassRuleCount; k++ )
    {
      pr = &pcs->PosClassRule[k];

      if ( context_length != 0xFFFF && context_length < pr->GlyphCount )
        continue;

      if ( in->pos + pr->GlyphCount > in->length )
        continue;                           /* context is too long */

      s_in = &in->string[in->pos];
      cl   = pr->Class;

      /* Start at 1 because [0] is implied */

      for ( i = 1, j = 1; i < pr->GlyphCount; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( in->pos + j < in->length )
            j++;
          else
            break;
        }

        if ( i > known_classes )
        {
          /* Keeps us from having to do this for each rule */

          error = Get_Class( &cpf2->ClassDef, s_in[j], &classes[i], NULL );
          if ( error && error != TTO_Err_Not_Covered )
            return error;
          known_classes = i;
        }

        if ( cl[i - 1] != classes[i] )
          break;
      }

      if ( i == pr->GlyphCount )
      {
        error = Do_ContextPos( gpi, pr->GlyphCount,
                               pr->PosCount, pr->PosLookupRecord,
                               in, out,
                               nesting_level );
        goto End;
      }
    }

    error = TTO_Err_Not_Covered;

  End:
    FREE( classes );
    return error;
  }


  static TT_Error  Lookup_ContextPos3( GPOS_Instance*          gpi,
                                       TTO_ContextPosFormat3*  cpf3,
                                       TTO_GSUB_String*        in,
                                       TTO_GPOS_Data*          out,
                                       UShort                  flags,
                                       UShort                  context_length,
                                       int                     nesting_level )
  {
    TT_Error         error;
    UShort           index, i, j, property;
    UShort*          s_in;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    TTO_Coverage*    c;
    TTO_GDEFHeader*  gdef;


    gdef = gpos->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    if ( context_length != 0xFFFF && context_length < cpf3->GlyphCount )
      return TTO_Err_Not_Covered;

    if ( in->pos + cpf3->GlyphCount > in->length )
      return TTO_Err_Not_Covered;         /* context is too long */

    s_in = &in->string[in->pos];
    c    = cpf3->Coverage;

    for ( i = 1, j = 1; i < cpf3->GlyphCount; i++, j++ )
    {
      while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
      {
        if ( error && error != TTO_Err_Not_Covered )
          return error;

        if ( in->pos + j < in->length )
          j++;
        else
          return TTO_Err_Not_Covered;
      }

      error = Coverage_Index( &c[i], s_in[j], &index );
      if ( error )
        return error;
    }

    return Do_ContextPos( gpi, cpf3->GlyphCount,
                          cpf3->PosCount, cpf3->PosLookupRecord,
                          in, out,
                          nesting_level );
  }


  static TT_Error  Lookup_ContextPos( GPOS_Instance*    gpi,
                                      TTO_ContextPos*   cp,
                                      TTO_GSUB_String*  in,
                                      TTO_GPOS_Data*    out,
                                      UShort            flags,
                                      UShort            context_length,
                                      int               nesting_level )
  {
    switch ( cp->PosFormat )
    {
    case 1:
      return Lookup_ContextPos1( gpi, &cp->cpf.cpf1, in, out,
                                 flags, context_length, nesting_level );

    case 2:
      return Lookup_ContextPos2( gpi, &cp->cpf.cpf2, in, out,
                                 flags, context_length, nesting_level );

    case 3:
      return Lookup_ContextPos3( gpi, &cp->cpf.cpf3, in, out,
                                 flags, context_length, nesting_level );

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }


  /* LookupType 8 */

  /* ChainPosRule */

  static TT_Error  Load_ChainPosRule( TTO_ChainPosRule*  cpr,
                                      PFace              input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    UShort*               b;
    UShort*               i;
    UShort*               l;

    TTO_PosLookupRecord*  plr;


    if ( ACCESS_Frame( 2L ) )
      return error;

    cpr->BacktrackGlyphCount = GET_UShort();

    FORGET_Frame();

    cpr->Backtrack = NULL;

    count = cpr->BacktrackGlyphCount;

    if ( ALLOC_ARRAY( cpr->Backtrack, count, UShort ) )
      return error;

    b = cpr->Backtrack;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail4;

    for ( n = 0; n < count; n++ )
      b[n] = GET_UShort();

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail4;

    cpr->InputGlyphCount = GET_UShort();

    FORGET_Frame();

    cpr->Input = NULL;

    count = cpr->InputGlyphCount - 1;  /* only InputGlyphCount - 1 elements */

    if ( ALLOC_ARRAY( cpr->Input, count, UShort ) )
      goto Fail4;

    i = cpr->Input;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail3;

    for ( n = 0; n < count; n++ )
      i[n] = GET_UShort();

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    cpr->LookaheadGlyphCount = GET_UShort();

    FORGET_Frame();

    cpr->Lookahead = NULL;

    count = cpr->LookaheadGlyphCount;

    if ( ALLOC_ARRAY( cpr->Lookahead, count, UShort ) )
      goto Fail3;

    l = cpr->Lookahead;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
      l[n] = GET_UShort();

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    cpr->PosCount = GET_UShort();

    FORGET_Frame();

    cpr->PosLookupRecord = NULL;

    count = cpr->PosCount;

    if ( ALLOC_ARRAY( cpr->PosLookupRecord, count, TTO_PosLookupRecord ) )
      goto Fail2;

    plr = cpr->PosLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      plr[n].SequenceIndex   = GET_UShort();
      plr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( plr );

  Fail2:
    FREE( l );

  Fail3:
    FREE( i );

  Fail4:
    FREE( b );
    return error;
  }


  static void  Free_ChainPosRule( TTO_ChainPosRule*  cpr )
  {
    FREE( cpr->PosLookupRecord );
    FREE( cpr->Lookahead );
    FREE( cpr->Input );
    FREE( cpr->Backtrack );
  }


  /* ChainPosRuleSet */

  static TT_Error  Load_ChainPosRuleSet( TTO_ChainPosRuleSet*  cprs,
                                         PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort             n, count;
    ULong              cur_offset, new_offset, base_offset;

    TTO_ChainPosRule*  cpr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = cprs->ChainPosRuleCount = GET_UShort();

    FORGET_Frame();

    cprs->ChainPosRule = NULL;

    if ( ALLOC_ARRAY( cprs->ChainPosRule, count, TTO_ChainPosRule ) )
      return error;

    cpr = cprs->ChainPosRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_ChainPosRule( &cpr[n], input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_ChainPosRule( &cpr[n] );

    FREE( cpr );
    return error;
  }


  static void  Free_ChainPosRuleSet( TTO_ChainPosRuleSet*  cprs )
  {
    UShort             n, count;

    TTO_ChainPosRule*  cpr;


    if ( cprs->ChainPosRule )
    {
      count = cprs->ChainPosRuleCount;
      cpr   = cprs->ChainPosRule;

      for ( n = 0; n < count; n++ )
        Free_ChainPosRule( &cpr[n] );

      FREE( cpr );
    }
  }


  /* ChainContextPosFormat1 */

  static TT_Error  Load_ChainContextPos1( TTO_ChainContextPosFormat1*  ccpf1,
                                          PFace                        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    ULong                 cur_offset, new_offset, base_offset;

    TTO_ChainPosRuleSet*  cprs;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &ccpf1->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    count = ccpf1->ChainPosRuleSetCount = GET_UShort();

    FORGET_Frame();

    ccpf1->ChainPosRuleSet = NULL;

    if ( ALLOC_ARRAY( ccpf1->ChainPosRuleSet, count, TTO_ChainPosRuleSet ) )
      goto Fail2;

    cprs = ccpf1->ChainPosRuleSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_ChainPosRuleSet( &cprs[n], input ) ) != TT_Err_Ok )
        goto Fail1;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_ChainPosRuleSet( &cprs[n] );

    FREE( cprs );

  Fail2:
    Free_Coverage( &ccpf1->Coverage );
    return error;
  }


  static void  Free_ChainContext1( TTO_ChainContextPosFormat1*  ccpf1 )
  {
    UShort                n, count;

    TTO_ChainPosRuleSet*  cprs;


    if ( ccpf1->ChainPosRuleSet )
    {
      count = ccpf1->ChainPosRuleSetCount;
      cprs  = ccpf1->ChainPosRuleSet;

      for ( n = 0; n < count; n++ )
        Free_ChainPosRuleSet( &cprs[n] );

      FREE( cprs );
    }

    Free_Coverage( &ccpf1->Coverage );
  }


  /* ChainPosClassRule */

  static TT_Error  Load_ChainPosClassRule(
                     TTO_ChainContextPosFormat2*  ccpf2,
                     TTO_ChainPosClassRule*       cpcr,
                     PFace                        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;

    UShort*               b;
    UShort*               i;
    UShort*               l;
    TTO_PosLookupRecord*  plr;
    Bool*                 d;


    if ( ACCESS_Frame( 2L ) )
      return error;

    cpcr->BacktrackGlyphCount = GET_UShort();

    FORGET_Frame();

    if ( cpcr->BacktrackGlyphCount > ccpf2->MaxBacktrackLength )
      ccpf2->MaxBacktrackLength = cpcr->BacktrackGlyphCount;

    cpcr->Backtrack = NULL;

    count = cpcr->BacktrackGlyphCount;

    if ( ALLOC_ARRAY( cpcr->Backtrack, count, UShort ) )
      return error;

    b = cpcr->Backtrack;
    d = ccpf2->BacktrackClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail4;

    for ( n = 0; n < count; n++ )
    {
      b[n] = GET_UShort();

      /* We check whether the specific class is used at all.  If not,
         class 0 is used instead.                                     */

      if ( !d[b[n]] )
        b[n] = 0;
    }

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail4;

    cpcr->InputGlyphCount = GET_UShort();

    if ( cpcr->InputGlyphCount > ccpf2->MaxInputLength )
      ccpf2->MaxInputLength = cpcr->InputGlyphCount;

    FORGET_Frame();

    cpcr->Input = NULL;

    count = cpcr->InputGlyphCount - 1; /* only InputGlyphCount - 1 elements */

    if ( ALLOC_ARRAY( cpcr->Input, count, UShort ) )
      goto Fail4;

    i = cpcr->Input;
    d = ccpf2->InputClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail3;

    for ( n = 0; n < count; n++ )
    {
      i[n] = GET_UShort();

      if ( !d[i[n]] )
        i[n] = 0;
    }

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    cpcr->LookaheadGlyphCount = GET_UShort();

    FORGET_Frame();

    if ( cpcr->LookaheadGlyphCount > ccpf2->MaxLookaheadLength )
      ccpf2->MaxLookaheadLength = cpcr->LookaheadGlyphCount;

    cpcr->Lookahead = NULL;

    count = cpcr->LookaheadGlyphCount;

    if ( ALLOC_ARRAY( cpcr->Lookahead, count, UShort ) )
      goto Fail3;

    l = cpcr->Lookahead;
    d = ccpf2->LookaheadClassDef.Defined;

    if ( ACCESS_Frame( count * 2L ) )
      goto Fail2;

    for ( n = 0; n < count; n++ )
    {
      l[n] = GET_UShort();

      if ( !d[l[n]] )
        l[n] = 0;
    }

    FORGET_Frame();

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    cpcr->PosCount = GET_UShort();

    FORGET_Frame();

    cpcr->PosLookupRecord = NULL;

    count = cpcr->PosCount;

    if ( ALLOC_ARRAY( cpcr->PosLookupRecord, count, TTO_PosLookupRecord ) )
      goto Fail2;

    plr = cpcr->PosLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      plr[n].SequenceIndex   = GET_UShort();
      plr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( plr );

  Fail2:
    FREE( l );

  Fail3:
    FREE( i );

  Fail4:
    FREE( b );
    return error;
  }


  static void  Free_ChainPosClassRule( TTO_ChainPosClassRule*  cpcr )
  {
    FREE( cpcr->PosLookupRecord );
    FREE( cpcr->Lookahead );
    FREE( cpcr->Input );
    FREE( cpcr->Backtrack );
  }


  /* PosClassSet */

  static TT_Error  Load_ChainPosClassSet(
                     TTO_ChainContextPosFormat2*  ccpf2,
                     TTO_ChainPosClassSet*        cpcs,
                     PFace                        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                  n, count;
    ULong                   cur_offset, new_offset, base_offset;

    TTO_ChainPosClassRule*  cpcr;


    base_offset = FILE_Pos();

    if ( ACCESS_Frame( 2L ) )
      return error;

    count = cpcs->ChainPosClassRuleCount = GET_UShort();

    FORGET_Frame();

    cpcs->ChainPosClassRule = NULL;

    if ( ALLOC_ARRAY( cpcs->ChainPosClassRule, count,
                      TTO_ChainPosClassRule ) )
      return error;

    cpcr = cpcs->ChainPosClassRule;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_ChainPosClassRule( ccpf2, &cpcr[n],
                                             input ) ) != TT_Err_Ok )
        goto Fail;
      (void)FILE_Seek( cur_offset );
    }

    return TT_Err_Ok;

  Fail:
    for ( n = 0; n < count; n++ )
      Free_ChainPosClassRule( &cpcr[n] );

    FREE( cpcr );
    return error;
  }


  static void  Free_ChainPosClassSet( TTO_ChainPosClassSet*  cpcs )
  {
    UShort                  n, count;

    TTO_ChainPosClassRule*  cpcr;


    if ( cpcs->ChainPosClassRule )
    {
      count = cpcs->ChainPosClassRuleCount;
      cpcr  = cpcs->ChainPosClassRule;

      for ( n = 0; n < count; n++ )
        Free_ChainPosClassRule( &cpcr[n] );

      FREE( cpcr );
    }
  }


  /* ChainContextPosFormat2 */

  static TT_Error  Load_ChainContextPos2( TTO_ChainContextPosFormat2*  ccpf2,
                                          PFace                        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                 n, count;
    ULong                  cur_offset, new_offset, base_offset;
    ULong                  backtrack_offset, input_offset, lookahead_offset;

    TTO_ChainPosClassSet*  cpcs;


    base_offset = FILE_Pos() - 2;

    if ( ACCESS_Frame( 2L ) )
      return error;

    new_offset = GET_UShort() + base_offset;

    FORGET_Frame();

    cur_offset = FILE_Pos();
    if ( FILE_Seek( new_offset ) ||
         ( error = Load_Coverage( &ccpf2->Coverage, input ) ) != TT_Err_Ok )
      return error;
    (void)FILE_Seek( cur_offset );

    if ( ACCESS_Frame( 8L ) )
      goto Fail5;

    backtrack_offset = GET_UShort() + base_offset;
    input_offset     = GET_UShort() + base_offset;
    lookahead_offset = GET_UShort() + base_offset;

    /* `ChainPosClassSetCount' is the upper limit for input class values,
       thus we read it now to make an additional safety check.            */

    count = ccpf2->ChainPosClassSetCount = GET_UShort();

    FORGET_Frame();

    /* backtrack and lookahead data can be optional */

    cur_offset = FILE_Pos();
    if ( backtrack_offset != base_offset )
    {
      if ( FILE_Seek( backtrack_offset ) ||
           ( error = Load_ClassDefinition( &ccpf2->BacktrackClassDef, count,
                                           input ) ) != TT_Err_Ok )
        goto Fail5;
    }
    if ( FILE_Seek( input_offset ) ||
         ( error = Load_ClassDefinition( &ccpf2->InputClassDef, count,
                                         input ) ) != TT_Err_Ok )
      goto Fail4;
    if ( lookahead_offset != base_offset )
    {
      if ( FILE_Seek( lookahead_offset ) ||
           ( error = Load_ClassDefinition( &ccpf2->LookaheadClassDef, count,
                                           input ) ) != TT_Err_Ok )
        goto Fail3;
    }
    (void)FILE_Seek( cur_offset );

    ccpf2->ChainPosClassSet   = NULL;
    ccpf2->MaxBacktrackLength = 0;
    ccpf2->MaxInputLength     = 0;
    ccpf2->MaxLookaheadLength = 0;

    if ( ALLOC_ARRAY( ccpf2->ChainPosClassSet, count, TTO_ChainPosClassSet ) )
      goto Fail2;

    cpcs = ccpf2->ChainPosClassSet;

    for ( n = 0; n < count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail1;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      if ( new_offset != base_offset )      /* not a NULL offset */
      {
        cur_offset = FILE_Pos();
        if ( FILE_Seek( new_offset ) ||
             ( error = Load_ChainPosClassSet( ccpf2, &cpcs[n],
                                              input ) ) != TT_Err_Ok )
          goto Fail1;
        (void)FILE_Seek( cur_offset );
      }
      else
      {
        /* we create a ChainPosClassSet table with no entries */

        ccpf2->ChainPosClassSet[n].ChainPosClassRuleCount = 0;
        ccpf2->ChainPosClassSet[n].ChainPosClassRule      = NULL;
      }
    }

    return TT_Err_Ok;

  Fail1:
    for ( n = 0; n < count; n++ )
      Free_ChainPosClassSet( &cpcs[n] );

    FREE( cpcs );

  Fail2:
    Free_ClassDefinition( &ccpf2->LookaheadClassDef );

  Fail3:
    Free_ClassDefinition( &ccpf2->InputClassDef );

  Fail4:
    Free_ClassDefinition( &ccpf2->BacktrackClassDef );

  Fail5:
    Free_Coverage( &ccpf2->Coverage );
    return error;
  }


  static void  Free_ChainContext2( TTO_ChainContextPosFormat2*  ccpf2 )
  {
    UShort                 n, count;

    TTO_ChainPosClassSet*  cpcs;


    if ( ccpf2->ChainPosClassSet )
    {
      count = ccpf2->ChainPosClassSetCount;
      cpcs  = ccpf2->ChainPosClassSet;

      for ( n = 0; n < count; n++ )
        Free_ChainPosClassSet( &cpcs[n] );

      FREE( cpcs );
    }

    Free_ClassDefinition( &ccpf2->LookaheadClassDef );
    Free_ClassDefinition( &ccpf2->InputClassDef );
    Free_ClassDefinition( &ccpf2->BacktrackClassDef );

    Free_Coverage( &ccpf2->Coverage );
  }


  /* ChainContextPosFormat3 */

  static TT_Error  Load_ChainContextPos3( TTO_ChainContextPosFormat3*  ccpf3,
                                          PFace                        input )
  {
    DEFINE_LOAD_LOCALS( input->stream );

    UShort                n, count;
    UShort                backtrack_count, input_count, lookahead_count;
    ULong                 cur_offset, new_offset, base_offset;

    TTO_Coverage*         b;
    TTO_Coverage*         i;
    TTO_Coverage*         l;
    TTO_PosLookupRecord*  plr;


    base_offset = FILE_Pos() - 2L;

    if ( ACCESS_Frame( 2L ) )
      return error;

    ccpf3->BacktrackGlyphCount = GET_UShort();

    FORGET_Frame();

    ccpf3->BacktrackCoverage = NULL;

    backtrack_count = ccpf3->BacktrackGlyphCount;

    if ( ALLOC_ARRAY( ccpf3->BacktrackCoverage, backtrack_count,
                      TTO_Coverage ) )
      return error;

    b = ccpf3->BacktrackCoverage;

    for ( n = 0; n < backtrack_count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail4;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &b[n], input ) ) != TT_Err_Ok )
        goto Fail4;
      (void)FILE_Seek( cur_offset );
    }

    if ( ACCESS_Frame( 2L ) )
      goto Fail4;

    ccpf3->InputGlyphCount = GET_UShort();

    FORGET_Frame();

    ccpf3->InputCoverage = NULL;

    input_count = ccpf3->InputGlyphCount;

    if ( ALLOC_ARRAY( ccpf3->InputCoverage, input_count, TTO_Coverage ) )
      goto Fail4;

    i = ccpf3->InputCoverage;

    for ( n = 0; n < input_count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail3;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &i[n], input ) ) != TT_Err_Ok )
        goto Fail3;
      (void)FILE_Seek( cur_offset );
    }

    if ( ACCESS_Frame( 2L ) )
      goto Fail3;

    ccpf3->LookaheadGlyphCount = GET_UShort();

    FORGET_Frame();

    ccpf3->LookaheadCoverage = NULL;

    lookahead_count = ccpf3->LookaheadGlyphCount;

    if ( ALLOC_ARRAY( ccpf3->LookaheadCoverage, lookahead_count,
                      TTO_Coverage ) )
      goto Fail3;

    l = ccpf3->LookaheadCoverage;

    for ( n = 0; n < lookahead_count; n++ )
    {
      if ( ACCESS_Frame( 2L ) )
        goto Fail2;

      new_offset = GET_UShort() + base_offset;

      FORGET_Frame();

      cur_offset = FILE_Pos();
      if ( FILE_Seek( new_offset ) ||
           ( error = Load_Coverage( &l[n], input ) ) != TT_Err_Ok )
        goto Fail2;
      (void)FILE_Seek( cur_offset );
    }

    if ( ACCESS_Frame( 2L ) )
      goto Fail2;

    ccpf3->PosCount = GET_UShort();

    FORGET_Frame();

    ccpf3->PosLookupRecord = NULL;

    count = ccpf3->PosCount;

    if ( ALLOC_ARRAY( ccpf3->PosLookupRecord, count, TTO_PosLookupRecord ) )
      goto Fail2;

    plr = ccpf3->PosLookupRecord;

    if ( ACCESS_Frame( count * 4L ) )
      goto Fail1;

    for ( n = 0; n < count; n++ )
    {
      plr[n].SequenceIndex   = GET_UShort();
      plr[n].LookupListIndex = GET_UShort();
    }

    FORGET_Frame();

    return TT_Err_Ok;

  Fail1:
    FREE( plr );

  Fail2:
    for ( n = 0; n < lookahead_count; n++ )
      Free_Coverage( &l[n] );

    FREE( l );

  Fail3:
    for ( n = 0; n < input_count; n++ )
      Free_Coverage( &i[n] );

    FREE( i );

  Fail4:
    for ( n = 0; n < backtrack_count; n++ )
      Free_Coverage( &b[n] );

    FREE( b );
    return error;
  }


  static void  Free_ChainContext3( TTO_ChainContextPosFormat3*  ccpf3 )
  {
    UShort         n, count;

    TTO_Coverage*  c;


    FREE( ccpf3->PosLookupRecord );

    if ( ccpf3->LookaheadCoverage )
    {
      count = ccpf3->LookaheadGlyphCount;
      c     = ccpf3->LookaheadCoverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }

    if ( ccpf3->InputCoverage )
    {
      count = ccpf3->InputGlyphCount;
      c     = ccpf3->InputCoverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }

    if ( ccpf3->BacktrackCoverage )
    {
      count = ccpf3->BacktrackGlyphCount;
      c     = ccpf3->BacktrackCoverage;

      for ( n = 0; n < count; n++ )
        Free_Coverage( &c[n] );

      FREE( c );
    }
  }


  /* ChainContextPos */

  TT_Error  Load_ChainContextPos( TTO_ChainContextPos*  ccp,
                                  PFace                 input )
  {
    DEFINE_LOAD_LOCALS( input->stream );


    if ( ACCESS_Frame( 2L ) )
      return error;

    ccp->PosFormat = GET_UShort();

    FORGET_Frame();

    switch ( ccp->PosFormat )
    {
    case 1:
      return Load_ChainContextPos1( &ccp->ccpf.ccpf1, input );

    case 2:
      return Load_ChainContextPos2( &ccp->ccpf.ccpf2, input );

    case 3:
      return Load_ChainContextPos3( &ccp->ccpf.ccpf3, input );

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }


  void  Free_ChainContextPos( TTO_ChainContextPos*  ccp )
  {
    switch ( ccp->PosFormat )
    {
    case 1:
      Free_ChainContext1( &ccp->ccpf.ccpf1 );
      break;

    case 2:
      Free_ChainContext2( &ccp->ccpf.ccpf2 );
      break;

    case 3:
      Free_ChainContext3( &ccp->ccpf.ccpf3 );
      break;
    }
  }


  static TT_Error  Lookup_ChainContextPos1(
                     GPOS_Instance*               gpi,
                     TTO_ChainContextPosFormat1*  ccpf1,
                     TTO_GSUB_String*             in,
                     TTO_GPOS_Data*               out,
                     TT_UInt                      flags,
                     TT_UInt                      context_length,
                     int                          nesting_level )
  {
    UShort             index, property;
    TT_UInt            i, j, k, num_cpr, curr_pos;
    TT_UInt            bgc, igc, lgc;
    TT_Error           error;
    UShort*            s_in;
    TTO_GPOSHeader*    gpos = gpi->gpos;

    TTO_ChainPosRule*  cpr;
    TTO_ChainPosRule   curr_cpr;
    TTO_GDEFHeader*    gdef;


    gdef = gpos->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    error = Coverage_Index( &ccpf1->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    cpr     = ccpf1->ChainPosRuleSet[index].ChainPosRule;
    num_cpr = ccpf1->ChainPosRuleSet[index].ChainPosRuleCount;

    for ( k = 0; k < num_cpr; k++ )
    {
      curr_cpr = cpr[k];
      bgc      = curr_cpr.BacktrackGlyphCount;
      igc      = curr_cpr.InputGlyphCount;
      lgc      = curr_cpr.LookaheadGlyphCount;

      if ( context_length != 0xFFFF && context_length < igc )
        continue;

      /* check whether context is too long; it is a first guess only */

      if ( bgc > in->pos || in->pos + igc + lgc > in->length )
        continue;

      if ( bgc )
      {
        /* Since we don't know in advance the number of glyphs to inspect,
           we search backwards for matches in the backtrack glyph array    */

        curr_pos = 0;
        s_in     = &in->string[curr_pos];

        for ( i = 0, j = in->pos - 1; i < bgc; i++, j-- )
        {
          while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
          {
            if ( error && error != TTO_Err_Not_Covered )
              return error;

            if ( j > curr_pos )
              j--;
            else
              break;
          }

          /* In OpenType 1.3, it is undefined whether the offsets of
             backtrack glyphs is in logical order or not.  Version 1.4
             will clarify this:

               Logical order -      a  b  c  d  e  f  g  h  i  j
                                                i
               Input offsets -                  0  1
               Backtrack offsets -  3  2  1  0
               Lookahead offsets -                    0  1  2  3           */

          if ( s_in[j] != curr_cpr.Backtrack[i] )
            break;
        }

        if ( i != bgc )
          continue;
      }

      curr_pos = in->pos;
      s_in     = &in->string[curr_pos];

      /* Start at 1 because [0] is implied */

      for ( i = 1, j = 1; i < igc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( s_in[j] != curr_cpr.Input[i - 1] )
          break;
      }

      if ( i != igc )
        continue;

      /* we are starting to check for lookahead glyphs right after the
         last context glyph                                            */

      curr_pos = j;
      s_in     = &in->string[curr_pos];

      for ( i = 0, j = 0; i < lgc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( s_in[j] != curr_cpr.Lookahead[i] )
          break;
      }

      if ( i == lgc )
        return Do_ContextPos( gpi, igc,
                              curr_cpr.PosCount,
                              curr_cpr.PosLookupRecord,
                              in, out,
                              nesting_level );
    }

    return TTO_Err_Not_Covered;
  }


  static TT_Error  Lookup_ChainContextPos2(
                     GPOS_Instance*               gpi,
                     TTO_ChainContextPosFormat2*  ccpf2,
                     TTO_GSUB_String*             in,
                     TTO_GPOS_Data*               out,
                     TT_UInt                      flags,
                     TT_UInt                      context_length,
                     int                          nesting_level )
  {
    UShort                 index, property;
    TT_Error               error;
    TT_UInt                i, j, k, curr_pos;
    TT_UInt                bgc, igc, lgc;
    UShort                 known_backtrack_classes,
                           known_input_classes,
                           known_lookahead_classes;

    UShort*                backtrack_classes;
    UShort*                input_classes;
    UShort*                lookahead_classes;

    UShort*                s_in;

    UShort*                bc;
    UShort*                ic;
    UShort*                lc;
    TTO_GPOSHeader*        gpos = gpi->gpos;

    TTO_ChainPosClassSet*  cpcs;
    TTO_ChainPosClassRule  cpcr;
    TTO_GDEFHeader*        gdef;


    gdef = gpos->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    /* Note: The coverage table in format 2 doesn't give an index into
             anything.  It just lets us know whether or not we need to
             do any lookup at all.                                     */

    error = Coverage_Index( &ccpf2->Coverage, in->string[in->pos], &index );
    if ( error )
      return error;

    if ( ALLOC_ARRAY( backtrack_classes, ccpf2->MaxBacktrackLength, UShort ) )
      return error;
    known_backtrack_classes = 0;

    if ( ALLOC_ARRAY( input_classes, ccpf2->MaxInputLength, UShort ) )
      goto End3;
    known_input_classes = 1;

    if ( ALLOC_ARRAY( lookahead_classes, ccpf2->MaxLookaheadLength, UShort ) )
      goto End2;
    known_lookahead_classes = 0;

    error = Get_Class( &ccpf2->InputClassDef, in->string[in->pos],
                       &input_classes[0], NULL );
    if ( error )
      goto End1;

    cpcs = &ccpf2->ChainPosClassSet[input_classes[0]];
    if ( !cpcs )
    {
      error = TTO_Err_Invalid_GPOS_SubTable;
      goto End1;
    }

    for ( k = 0; k < (TT_UInt)cpcs->ChainPosClassRuleCount; k++ )
    {
      cpcr = cpcs->ChainPosClassRule[k];
      bgc  = cpcr.BacktrackGlyphCount;
      igc  = cpcr.InputGlyphCount;
      lgc  = cpcr.LookaheadGlyphCount;

      if ( context_length != 0xFFFF && context_length < igc )
        continue;

      /* check whether context is too long; it is a first guess only */

      if ( bgc > in->pos || in->pos + igc + lgc > in->length )
        continue;

      if ( bgc )
      {
        /* Since we don't know in advance the number of glyphs to inspect,
           we search backwards for matches in the backtrack glyph array.
           Note that `known_backtrack_classes' starts at index 0.         */

        curr_pos = 0;
        s_in     = &in->string[curr_pos];
        bc       = cpcr.Backtrack;

        for ( i = 0, j = in->pos - 1; i < bgc; i++, j-- )
        {
          while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
          {
            if ( error && error != TTO_Err_Not_Covered )
              return error;

            if ( j > curr_pos )
              j--;
            else
              break;
          }

          if ( i >= (TT_UInt)known_backtrack_classes )
          {
            /* Keeps us from having to do this for each rule */

            error = Get_Class( &ccpf2->BacktrackClassDef, s_in[j],
                               &backtrack_classes[i], NULL );
            if ( error && error != TTO_Err_Not_Covered )
              goto End1;
            known_backtrack_classes = i;
          }

          if ( bc[i] != backtrack_classes[i] )
            break;
        }

        if ( i != bgc )
          continue;
      }

      curr_pos = in->pos;
      s_in     = &in->string[curr_pos];
      ic       = cpcr.Input;

      /* Start at 1 because [0] is implied */

      for ( i = 1, j = 1; i < igc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            goto End1;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( i >= (TT_UInt)known_input_classes )
        {
          error = Get_Class( &ccpf2->InputClassDef, s_in[j],
                             &input_classes[i], NULL );
          if ( error && error != TTO_Err_Not_Covered )
            goto End1;
          known_input_classes = i;
        }

        if ( ic[i - 1] != input_classes[i] )
          break;
      }

      if ( i != igc )
        continue;

      /* we are starting to check for lookahead glyphs right after the
         last context glyph                                            */

      curr_pos = j;
      s_in     = &in->string[curr_pos];
      lc       = cpcr.Lookahead;

      for ( i = 0, j = 0; i < lgc; i++, j++ )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( curr_pos + j < in->length )
            j++;
          else
            break;
        }

        if ( i >= (TT_UInt)known_lookahead_classes )
        {
          error = Get_Class( &ccpf2->LookaheadClassDef, s_in[j],
                             &lookahead_classes[i], NULL );
          if ( error && error != TTO_Err_Not_Covered )
            goto End1;
          known_lookahead_classes = i;
        }

        if ( lc[i] != lookahead_classes[i] )
          break;
      }

      if ( i == lgc )
      {
        error = Do_ContextPos( gpi, igc,
                               cpcr.PosCount,
                               cpcr.PosLookupRecord,
                               in, out,
                               nesting_level );
        goto End1;
      }
    }

    error = TTO_Err_Not_Covered;

  End1:
    FREE( lookahead_classes );

  End2:
    FREE( input_classes );

  End3:
    FREE( backtrack_classes );
    return error;
  }


  static TT_Error  Lookup_ChainContextPos3(
                     GPOS_Instance*               gpi,
                     TTO_ChainContextPosFormat3*  ccpf3,
                     TTO_GSUB_String*             in,
                     TTO_GPOS_Data*               out,
                     TT_UInt                      flags,
                     TT_UInt                      context_length,
                     int                          nesting_level )
  {
    UShort           index, property;
    TT_UInt          i, j, curr_pos, bgc, igc, lgc;
    TT_Error         error;
    UShort*          s_in;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    TTO_Coverage*    bc;
    TTO_Coverage*    ic;
    TTO_Coverage*    lc;
    TTO_GDEFHeader*  gdef;


    gdef = gpos->gdef;

    if ( CHECK_Property( gdef, in->string[in->pos], flags, &property ) )
      return error;

    bgc = ccpf3->BacktrackGlyphCount;
    igc = ccpf3->InputGlyphCount;
    lgc = ccpf3->LookaheadGlyphCount;

    if ( context_length != 0xFFFF && context_length < igc )
      return TTO_Err_Not_Covered;

    /* check whether context is too long; it is a first guess only */

    if ( bgc > in->pos || in->pos + igc + lgc > in->length )
      return TTO_Err_Not_Covered;

    if ( bgc )
    {
      /* Since we don't know in advance the number of glyphs to inspect,
         we search backwards for matches in the backtrack glyph array    */

      curr_pos = 0;
      s_in     = &in->string[curr_pos];
      bc       = ccpf3->BacktrackCoverage;

      for ( i = 0, j = in->pos - 1; i < bgc; i++, j-- )
      {
        while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
        {
          if ( error && error != TTO_Err_Not_Covered )
            return error;

          if ( j > curr_pos )
            j--;
          else
            return TTO_Err_Not_Covered;
        }

        error = Coverage_Index( &bc[i], s_in[j], &index );
        if ( error )
          return error;
      }
    }

    curr_pos = in->pos;
    s_in     = &in->string[curr_pos];
    ic       = ccpf3->InputCoverage;

    /* Start at 1 because [0] is implied */

    for ( i = 1, j = 1; i < igc; i++, j++ )
    {
      while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
      {
        if ( error && error != TTO_Err_Not_Covered )
          return error;

        if ( curr_pos + j < in->length )
          j++;
        else
          return TTO_Err_Not_Covered;
      }

      error = Coverage_Index( &ic[i], s_in[j], &index );
      if ( error )
        return error;
    }

    /* we are starting to check for lookahead glyphs right after the
       last context glyph                                            */

    curr_pos = j;
    s_in     = &in->string[curr_pos];
    lc       = ccpf3->LookaheadCoverage;

    for ( i = 0, j = 0; i < lgc; i++, j++ )
    {
      while ( CHECK_Property( gdef, s_in[j], flags, &property ) )
      {
        if ( error && error != TTO_Err_Not_Covered )
          return error;

        if ( curr_pos + j < in->length )
          j++;
        else
          return TTO_Err_Not_Covered;
      }

      error = Coverage_Index( &lc[i], s_in[j], &index );
      if ( error )
        return error;
    }

    return Do_ContextPos( gpi, igc,
                          ccpf3->PosCount,
                          ccpf3->PosLookupRecord,
                          in, out,
                          nesting_level );
  }


  static TT_Error  Lookup_ChainContextPos(
                     GPOS_Instance*        gpi,
                     TTO_ChainContextPos*  ccp,
                     TTO_GSUB_String*      in,
                     TTO_GPOS_Data*        out,
                     UShort                flags,
                     UShort                context_length,
                     int                   nesting_level )
  {
    switch ( ccp->PosFormat )
    {
    case 1:
      return Lookup_ChainContextPos1( gpi, &ccp->ccpf.ccpf1, in, out,
                                      flags, context_length,
                                      nesting_level );

    case 2:
      return Lookup_ChainContextPos2( gpi, &ccp->ccpf.ccpf2, in, out,
                                      flags, context_length,
                                      nesting_level );

    case 3:
      return Lookup_ChainContextPos3( gpi, &ccp->ccpf.ccpf3, in, out,
                                      flags, context_length,
                                      nesting_level );

    default:
      return TTO_Err_Invalid_GPOS_SubTable_Format;
    }

    return TT_Err_Ok;               /* never reached */
  }



  /***********
   * GPOS API
   ***********/


  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Select_Script( TTO_GPOSHeader*  gpos,
                         TT_ULong         script_tag,
                         TT_UShort*       script_index )
  {
    UShort             n;

    TTO_ScriptList*    sl;
    TTO_ScriptRecord*  sr;


    if ( !gpos || !script_index )
      return TT_Err_Invalid_Argument;

    sl = &gpos->ScriptList;
    sr = sl->ScriptRecord;

    for ( n = 0; n < sl->ScriptCount; n++ )
      if ( script_tag == sr[n].ScriptTag )
      {
        *script_index = n;

        return TT_Err_Ok;
      }

    return TTO_Err_Not_Covered;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Select_Language( TTO_GPOSHeader*  gpos,
                           TT_ULong         language_tag,
                           TT_UShort        script_index,
                           TT_UShort*       language_index,
                           TT_UShort*       req_feature_index )
  {
    UShort              n;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;


    if ( !gpos || !language_index || !req_feature_index )
      return TT_Err_Invalid_Argument;

    sl = &gpos->ScriptList;
    sr = sl->ScriptRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    for ( n = 0; n < s->LangSysCount; n++ )
      if ( language_tag == lsr[n].LangSysTag )
      {
        *language_index = n;
        *req_feature_index = lsr[n].LangSys.ReqFeatureIndex;

        return TT_Err_Ok;
      }

    return TTO_Err_Not_Covered;
  }


  /* selecting 0xFFFF for language_index asks for the values of the
     default language (DefaultLangSys)                              */

  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Select_Feature( TTO_GPOSHeader*  gpos,
                          TT_ULong         feature_tag,
                          TT_UShort        script_index,
                          TT_UShort        language_index,
                          TT_UShort*       feature_index )
  {
    UShort              n;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;
    TTO_LangSys*        ls;
    UShort*             fi;

    TTO_FeatureList*    fl;
    TTO_FeatureRecord*  fr;


    if ( !gpos || !feature_index )
      return TT_Err_Invalid_Argument;

    sl = &gpos->ScriptList;
    sr = sl->ScriptRecord;

    fl = &gpos->FeatureList;
    fr = fl->FeatureRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    if ( language_index == 0xFFFF )
      ls = &s->DefaultLangSys;
    else
    {
      if ( language_index >= s->LangSysCount )
        return TT_Err_Invalid_Argument;

      ls = &lsr[language_index].LangSys;
    }

    fi = ls->FeatureIndex;

    for ( n = 0; n < ls->FeatureCount; n++ )
    {
      if ( fi[n] >= fl->FeatureCount )
        return TTO_Err_Invalid_GPOS_SubTable_Format;

      if ( feature_tag == fr[fi[n]].FeatureTag )
      {
        *feature_index = fi[n];

        return TT_Err_Ok;
      }
    }

    return TTO_Err_Not_Covered;
  }


  /* The next three functions return a null-terminated list */

  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Query_Scripts( TTO_GPOSHeader*  gpos,
                         TT_ULong**       script_tag_list )
  {
    UShort             n;
    TT_Error           error;
    ULong*             stl;

    TTO_ScriptList*    sl;
    TTO_ScriptRecord*  sr;


    if ( !gpos || !script_tag_list )
      return TT_Err_Invalid_Argument;

    sl = &gpos->ScriptList;
    sr = sl->ScriptRecord;

    if ( ALLOC_ARRAY( stl, sl->ScriptCount + 1, ULong ) )
      return error;

    for ( n = 0; n < sl->ScriptCount; n++ )
      stl[n] = sr[n].ScriptTag;
    stl[n] = 0;

    *script_tag_list = stl;

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Query_Languages( TTO_GPOSHeader*  gpos,
                           TT_UShort        script_index,
                           TT_ULong**       language_tag_list )
  {
    UShort              n;
    TT_Error            error;
    ULong*              ltl;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;


    if ( !gpos || !language_tag_list )
      return TT_Err_Invalid_Argument;

    sl = &gpos->ScriptList;
    sr = sl->ScriptRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    if ( ALLOC_ARRAY( ltl, s->LangSysCount + 1, ULong ) )
      return error;

    for ( n = 0; n < s->LangSysCount; n++ )
      ltl[n] = lsr[n].LangSysTag;
    ltl[n] = 0;

    *language_tag_list = ltl;

    return TT_Err_Ok;
  }


  /* selecting 0xFFFF for language_index asks for the values of the
     default language (DefaultLangSys)                              */

  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Query_Features( TTO_GPOSHeader*  gpos,
                          TT_UShort        script_index,
                          TT_UShort        language_index,
                          TT_ULong**       feature_tag_list )
  {
    UShort              n;
    TT_Error            error;
    ULong*              ftl;

    TTO_ScriptList*     sl;
    TTO_ScriptRecord*   sr;
    TTO_Script*         s;
    TTO_LangSysRecord*  lsr;
    TTO_LangSys*        ls;
    UShort*             fi;

    TTO_FeatureList*    fl;
    TTO_FeatureRecord*  fr;


    if ( !gpos || !feature_tag_list )
      return TT_Err_Invalid_Argument;

    sl = &gpos->ScriptList;
    sr = sl->ScriptRecord;

    fl = &gpos->FeatureList;
    fr = fl->FeatureRecord;

    if ( script_index >= sl->ScriptCount )
      return TT_Err_Invalid_Argument;

    s   = &sr[script_index].Script;
    lsr = s->LangSysRecord;

    if ( language_index == 0xFFFF )
      ls = &s->DefaultLangSys;
    else
    {
      if ( language_index >= s->LangSysCount )
        return TT_Err_Invalid_Argument;

      ls = &lsr[language_index].LangSys;
    }

    fi = ls->FeatureIndex;

    if ( ALLOC_ARRAY( ftl, ls->FeatureCount + 1, ULong ) )
      return error;

    for ( n = 0; n < ls->FeatureCount; n++ )
    {
      if ( fi[n] >= fl->FeatureCount )
      {
        FREE( ftl );
        return TTO_Err_Invalid_GPOS_SubTable_Format;
      }
      ftl[n] = fr[fi[n]].FeatureTag;
    }
    ftl[n] = 0;

    *feature_tag_list = ftl;

    return TT_Err_Ok;
  }


  /* Do an individual subtable lookup.  Returns TT_Err_Ok if positioning
     has been done, or TTO_Err_Not_Covered if not.                        */

  static TT_Error  Do_Glyph_Lookup( GPOS_Instance*    gpi,
                                    UShort            lookup_index,
                                    TTO_GSUB_String*  in,
                                    TTO_GPOS_Data*    out,
                                    UShort            context_length,
                                    int               nesting_level )
  {
    TT_Error         error = TT_Err_Ok;
    UShort           i, flags;
    TTO_GPOSHeader*  gpos = gpi->gpos;
    TTO_Lookup*      lo;


    nesting_level++;

    if ( nesting_level > TTO_MAX_NESTING_LEVEL )
      return TTO_Err_Too_Many_Nested_Contexts;

    lo    = &gpos->LookupList.Lookup[lookup_index];
    flags = lo->LookupFlag;

    for ( i = 0; i < lo->SubTableCount; i++ )
    {
      switch ( lo->LookupType )
      {
      case GPOS_LOOKUP_SINGLE:
        error = Lookup_SinglePos( gpi,
                                  &lo->SubTable[i].st.gpos.single,
                                  in, out,
                                  flags, context_length );
        break;

      case GPOS_LOOKUP_PAIR:
        error = Lookup_PairPos( gpi,
                                &lo->SubTable[i].st.gpos.pair,
                                in, out,
                                flags, context_length );
        break;

      case GPOS_LOOKUP_CURSIVE:
        error = Lookup_CursivePos( gpi,
                                   &lo->SubTable[i].st.gpos.cursive,
                                   in, out,
                                   flags, context_length );
        break;

      case GPOS_LOOKUP_MARKBASE:
        error = Lookup_MarkBasePos( gpi,
                                    &lo->SubTable[i].st.gpos.markbase,
                                    in, out,
                                    flags, context_length );
        break;

      case GPOS_LOOKUP_MARKLIG:
        error = Lookup_MarkLigPos( gpi,
                                   &lo->SubTable[i].st.gpos.marklig,
                                   in, out,
                                   flags, context_length );
        break;

      case GPOS_LOOKUP_MARKMARK:
        error = Lookup_MarkMarkPos( gpi,
                                    &lo->SubTable[i].st.gpos.markmark,
                                    in, out,
                                    flags, context_length );
        break;

      case GPOS_LOOKUP_CONTEXT:
        error = Lookup_ContextPos( gpi,
                                   &lo->SubTable[i].st.gpos.context,
                                   in, out,
                                   flags, context_length,
                                   nesting_level );
        break;

      case GPOS_LOOKUP_CHAIN:
        error = Lookup_ChainContextPos( gpi,
                                        &lo->SubTable[i].st.gpos.chain,
                                        in, out,
                                        flags, context_length,
                                        nesting_level );
        break;
      }

      /* Check whether we have a successful positioning or an error other
         than TTO_Err_Not_Covered                                         */

      if ( error != TTO_Err_Not_Covered )
        return error;
    }

    return TTO_Err_Not_Covered;
  }


  /* apply one lookup to the input string object */

  static TT_Error  Do_String_Lookup( GPOS_Instance*    gpi,
                                     UShort            lookup_index,
                                     TTO_GSUB_String*  in,
                                     TTO_GPOS_Data*    out )
  {
    TT_Error         error = TTO_Err_Not_Covered;
    TTO_GPOSHeader*  gpos = gpi->gpos;

    UShort*  properties = gpos->LookupList.Properties;
    UShort*  p_in       = in->properties;

    int      nesting_level = 0;
    TT_UInt  i;
    TT_Pos   offset;


    gpi->first = 0xFFFF;
    gpi->last  = 0xFFFF;     /* no last valid glyph for cursive pos. */

    in->pos = 0;

    while ( in->pos < in->length )
    {
      if ( ~p_in[in->pos] & properties[lookup_index] )
      {
        /* 0xFFFF indicates that we don't have a context length yet. */

        /* Note that the connection between mark and base glyphs hold
           exactly one (string) lookup.  For example, it would be possible
           that in the first lookup, mark glyph X is attached to base
           glyph A, and in the next lookup it is attached to base glyph B.
           It is up to the font designer to provide meaningful lookups and
           lookup order.                                                   */

        error = Do_Glyph_Lookup( gpi, lookup_index, in, out,
                                 0xFFFF, nesting_level );
        if ( error && error != TTO_Err_Not_Covered )
          return error;
      }
      else
      {
        /* Contrary to properties defined in GDEF, user-defined properties
           will always stop a possible cursive positioning.                */
        gpi->last = 0xFFFF;

        error = TTO_Err_Not_Covered;
      }

      /* test whether we have to adjust the offsets for cursive connections */

      if ( gpi->first != 0xFFFF && gpi->last == 0xFFFF &&
           gpos->LookupList.Lookup[lookup_index].LookupFlag & RIGHT_TO_LEFT )
      {
        offset = out[in->pos].y_pos;

        /* no horizontal offsets (for vertical writing direction)
           supported yet                                          */

        for ( i = gpi->first; i <= (TT_UInt)in->pos; i++ )
          out[i].y_pos -= offset;

        gpi->first = 0xFFFF;
      }

      if ( error == TTO_Err_Not_Covered )
        (in->pos)++;
    }

    return error;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Add_Feature( TTO_GPOSHeader*  gpos,
                       TT_UShort        feature_index,
                       TT_UShort        property )
  {
    UShort       i;

    TTO_Feature  feature;
    UShort*      properties;
    UShort*      index;


    if ( !gpos ||
         feature_index >= gpos->FeatureList.FeatureCount )
      return TT_Err_Invalid_Argument;

    properties = gpos->LookupList.Properties;

    feature = gpos->FeatureList.FeatureRecord[feature_index].Feature;
    index   = feature.LookupListIndex;

    for ( i = 0; i < feature.LookupListCount; i++ )
      properties[index[i]] |= property;

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Clear_Features( TTO_GPOSHeader*  gpos )
  {
    UShort   i;

    UShort*  properties;


    if ( !gpos )
      return TT_Err_Invalid_Argument;

    properties = gpos->LookupList.Properties;

    for ( i = 0; i < gpos->LookupList.LookupCount; i++ )
      properties[i] = 0;

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Register_Glyph_Function( TTO_GPOSHeader*    gpos,
                                   TTO_GlyphFunction  gfunc )
  {
    if ( !gpos )
      return TT_Err_Invalid_Argument;

    gpos->gfunc = gfunc;

    return TT_Err_Ok;
  }


  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Register_MM_Function( TTO_GPOSHeader*  gpos,
                                TTO_MMFunction   mmfunc,
                                void*            data )
  {
    if ( !gpos )
      return TT_Err_Invalid_Argument;

    gpos->mmfunc = mmfunc;
    gpos->data   = data;

    return TT_Err_Ok;
  }


  /* If `dvi' is TRUE, glyph contour points for anchor points and device
     tables are ignored -- you will get device independent values.         */

  FT_EXPORT_FUNC( TT_Error )
  TT_GPOS_Apply_String( TT_Instance        instance,
                        TT_Glyph           glyph,
                        TTO_GPOSHeader*    gpos,
                        TT_UShort          load_flags,
                        TTO_GSUB_String*   in,
                        TTO_GPOS_Data**    out,
                        TT_Bool            dvi,
                        TT_Bool            r2l )
  {
    TT_Error       error = TTO_Err_Not_Covered;
    GPOS_Instance  gpi;

    UShort    j;

    UShort*   properties;


    if ( !instance.z || !gpos ||
         !in || in->length == 0 || in->pos >= in->length )
      return TT_Err_Invalid_Argument;

    properties = gpos->LookupList.Properties;

    gpi.instance   = instance;
    if ( dvi )
      gpi.glyph.z  = NULL;
    else
      gpi.glyph    = glyph;
    gpi.gpos       = gpos;
    gpi.load_flags = load_flags;
    gpi.r2l        = r2l;

    if ( *out )
      FREE( *out );
    if ( ALLOC_ARRAY( *out, in->length, TTO_GPOS_Data ) )
      return error;

    for ( j = 0; j < gpos->LookupList.LookupCount; j++ )
      if ( !properties || properties[j] )
      {
        error = Do_String_Lookup( &gpi, j, in, *out );
        if ( error && error != TTO_Err_Not_Covered )
          return error;
      }

    return error;
  }


/* END */
