/*******************************************************************
 *
 *  ftxsbit.c
 *
 *    Embedded bitmap API extension
 *
 *  Copyright 1996-1999 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *
 *  This extension is used to load the embedded bitmaps present
 *  in certain TrueType files.
 *
 ******************************************************************/

#include "ftxsbit.h"
#include "ttobjs.h"
#include "ttfile.h"
#include "ttload.h"
#include "ttmemory.h"
#include "tttags.h"
#include "ttextend.h"
#include "ttdebug.h"


#define SBIT_ID  Build_Extension_ID( 's', 'b', 'i', 't' )


/* Required by the tracing mode */
#undef  TT_COMPONENT
#define TT_COMPONENT  trace_bitmap

/* In all functions, the stream is taken from the 'face' object */
#define DEFINE_LOCALS           DEFINE_LOAD_LOCALS( face->stream )
#define DEFINE_LOCALS_WO_FRAME  DEFINE_LOAD_LOCALS_WO_FRAME( face->stream )


/***************************
 *
 * miscellaneous functions
 *
 ***************************/

/*******************************************************************
 *
 *  Function:  Load_BitmapData
 *
 *  Bit-aligned bitmap data -> Byte-aligned bitmap data when pad is 0
 *
 ******************************************************************/

  static
  TT_Error  Load_BitmapData( TT_SBit_Image*  image,
                             Int             image_size,
                             Byte            x_offset,
                             Byte            y_offset,
                             UShort          source_width,
                             UShort          source_height,
                             Bool            byte_padded )
  {
    DEFINE_LOCALS;

    Int     count;   /* number of bits left in rows              */
    Int     loaded;  /* number of bits loaded in the accumulator */
    UShort  buff;    /* accumulator                              */

    PByte   line;     /* target write cursor */
    PByte   limit;


    if ( ( y_offset + source_height > image->map.rows ) ||
         ( x_offset + source_width > image->map.width ) )
      return TT_Err_Invalid_Argument;

    if ( ACCESS_Frame( image_size ) )
      return error;

    buff   = 0;
    loaded = 0;
    line   = (PByte)image->map.bitmap +
               y_offset * image->map.cols;
    limit  = (PByte)image->map.bitmap +
               ( y_offset + source_height ) * image->map.cols;

    for ( ; line < limit; line += image->map.cols )
    {
      PByte  ptr;


      ptr   = line + x_offset / 8;
      count = source_width;

      /* We may assume that `loaded' is less than 8 */
      buff  >>= x_offset % 8;
      loaded += x_offset % 8;

      /* first of all, read all consecutive bytes */
      while ( count >= 8 )
      {
        if ( loaded < 8 )
        {
          buff   |= ((UShort)GET_Byte()) << (8 - loaded);
          loaded += 8;
        }

        *ptr++ |= (Byte)(buff >> 8);
        buff  <<= 8;
        loaded -= 8;
        count  -= 8;
      }

      /* now write remaining bits (i.e. end of line with count < 8) */
      if ( count > 0 )
      {
        if ( loaded < count )
        {
          buff   |= ((UShort)GET_Byte()) << (8 - loaded);
          loaded += 8;
        }

        *ptr   |= ((Byte)(buff >> 8)) & ~(0xFF >> count);
        buff  <<= count;
        loaded -= count;
      }

      if ( byte_padded )
      {
        buff   = 0;
        loaded = 0;
      }
    }

    FORGET_Frame();
    return TT_Err_Ok;
  }


/*******************************************************************
 *
 *  Function:  Crop_Bitmap
 *
 ******************************************************************/

  static
  void  Crop_Bitmap( TT_SBit_Image*  image )
  {
    /*******************************************************/
    /* In the following situation, some bounding boxes of  */
    /* embedded bitmaps are too large.  We need to crop it */
    /* to a reasonable size.                               */
    /*                                                     */
    /*      ---------                                      */
    /*      |       |                -----                 */
    /*      |  ***  |                |***|                 */
    /*      |   *   |    ----->      | * |                 */
    /*      |   *   |                | * |                 */
    /*      |   *   |                | * |                 */
    /*      |   *   |                | * |                 */
    /*      |  ***  |                |***|                 */
    /*      ---------                -----                 */
    /*                                                     */
    /*******************************************************/

    Int    rows, count;
    Long   line_len;
    PByte  line;


    /********************************************************************/
    /*                                                                  */
    /* first of all, check the top-most lines of the bitmap and remove  */
    /* them if they're empty.                                           */
    /*                                                                  */
    {
      line     = (PByte)image->map.bitmap;
      rows     = image->map.rows;
      line_len = image->map.cols;

      for ( count = 0; count < rows; count++ )
      {
        PByte  cur   = line;
        PByte  limit = line + line_len;


        for ( ; cur < limit; cur++ )
          if ( cur[0] )
            goto Found_Top;

        /* the current line was empty -- skip to next one */
        line = limit;
      }

    Found_Top:
      /* check that we have at least one filled line */
      if ( count >= rows )
        goto Empty_Bitmap;

      /* now, crop the empty upper lines */
      if ( count > 0 )
      {
        line = (PByte)image->map.bitmap;

        MEM_Move( line, line + count*line_len, (rows-count) * line_len );

        image->metrics.bbox.yMax    -= count;
        image->metrics.vertBearingY -= count;
        image->metrics.horiBearingY -= count;
        image->map.rows             -= count;
        rows                        -= count;
      }
    }

    /*******************************************************************/
    /*                                                                 */
    /* second, crop the lower lines                                    */
    /*                                                                 */
    {
      line = (PByte)image->map.bitmap + (rows-1) * line_len;

      for ( count = 0; count < rows; count++ )
      {
        PByte  cur   = line;
        PByte  limit = line + line_len;


        for ( ; cur < limit; cur++ )
          if ( cur[0] )
            goto Found_Bottom;

        /* the current line was empty -- skip to previous one */
        line -= line_len;
      }

    Found_Bottom:
      if ( count > 0 )
      {
        image->metrics.bbox.yMin += count;
        image->map.rows          -= count;
        rows                     -= count;
      }
    }

    /*******************************************************************/
    /*                                                                 */
    /* third, get rid of the space on the left side of the glyph       */
    /*                                                                 */
    do
    {
      PByte  limit;


      line  = (PByte)image->map.bitmap;
      limit = line + rows * line_len;

      for ( ; line < limit; line += line_len )
        if ( line[0] & 0x80 )
          goto Found_Left;

      /* shift the whole glyph one pixel to the left */
      line  = (PByte)image->map.bitmap;
      limit = line + rows * line_len;

      for ( ; line < limit; line += line_len )
      {
        Int    n, width = image->map.width;
        Byte   old;
        PByte  cur = line;


        old = cur[0] << 1;

        for ( n = 8; n < width; n += 8 )
        {
          Byte  val;


          val    = cur[1];
          cur[0] = old | (val >> 7);
          old    = val << 1;
          cur++;
        }
        cur[0] = old;
      }

      image->map.width--;
      image->metrics.horiBearingX++;
      image->metrics.vertBearingX++;
      image->metrics.bbox.xMin++;

    } while ( image->map.width > 0 );

  Found_Left:

    /*********************************************************************/
    /*                                                                   */
    /* finally, crop the bitmap width to get rid of the space on the     */
    /* right side of the glyph.                                          */
    /*                                                                   */
    do
    {
      Int    right = image->map.width-1;
      PByte  limit;
      Byte   mask;


      line  = (PByte)image->map.bitmap + (right >> 3);
      limit = line + rows*line_len;
      mask  = 0x80 >> (right & 7);

      for ( ; line < limit; line += line_len )
        if ( line[0] & mask )
          goto Found_Right;

      /* crop the whole glyph on the right */
      image->map.width--;
      image->metrics.bbox.xMax--;

    } while ( image->map.width > 0 );

  Found_Right:
    /* all right, the bitmap was cropped */
    return;

  Empty_Bitmap:
    image->map.width = 0;
    image->map.rows  = 0;
    image->map.cols  = 0;
    image->map.size  = 0;
  }

/*************
 *
 * Main body
 *
 *************/


  static
  TT_Error  Load_Range_Codes( TT_SBit_Range*  range,
                              PFace           face,
                              Bool            load_offsets )
  {
    DEFINE_LOCALS;

    ULong  count, n, size;


    (void)face;

    /* read glyph count */
    if ( ACCESS_Frame( 4L ) )
      goto Exit;
    count = GET_ULong();
    FORGET_Frame();

    range->num_glyphs = count;

    /* Allocate glyph offsets table if needed */
    if ( load_offsets )
    {
      if ( ALLOC_ARRAY( range->glyph_offsets, count, ULong ) )
        goto Exit;

      size = count * 4L;
    }
    else
      size = count * 2L;

    /* Allocate glyph codes table and access frame */
    if ( ALLOC_ARRAY ( range->glyph_codes, count, UShort ) ||
         ACCESS_Frame( size )                              )
      goto Exit;

    for ( n = 0; n < count; n++ )
    {
      range->glyph_codes[n] = GET_UShort();

      if ( load_offsets )
        range->glyph_offsets[n] = (ULong)range->image_offset + GET_UShort();
    }

    FORGET_Frame();

  Exit:
    return error;
  }


  static
  TT_Error  Load_SBit_Range( TT_SBit_Strike*  strike,
                             TT_SBit_Range*   range,
                             PFace            face )
  {
    DEFINE_LOCALS;

    UShort  format;


    (void)face;
    (void)strike;

    format = range->index_format;
    PTRACE6(( "Index Format: %d\n", format ));

    switch( format )
    {
    case 1:   /* variable metrics with 4-byte offsets */
    case 3:   /* variable metrics with 2-byte offsets */
      {
        UShort  num_glyphs, size_elem;
        Bool    large = (format == 1);
        ULong*  cur;

        num_glyphs = range->last_glyph - range->first_glyph + 1;
        PTRACE5(( "  num glyphs: %hu\n", num_glyphs ));

        range->num_glyphs = num_glyphs;

        num_glyphs++;  /* BEWARE */

        size_elem = large ? 4 : 2;

        if ( ALLOC_ARRAY( range->glyph_offsets, num_glyphs, ULong ) ||
             ACCESS_Frame( num_glyphs * size_elem )                 )
          return error;

        cur = range->glyph_offsets;

        while ( num_glyphs > 0 )
        {
          cur[0] = (TT_ULong)( range->image_offset +
                               (large ? GET_ULong() : GET_UShort()) );
          PTRACE7(( "  offset: %d\n", cur[0] ));
          cur++;
          num_glyphs--;
        }

        FORGET_Frame();
      }
      break;

    case 2:   /* all glyphs have identical metrics */
    case 4:
    case 5:
      {
        error = 0;

        if ( format != 4 )  /* read constant metrics, formats 2 and 5 */
        {
          TT_SBit_Metrics*  metrics;


          if ( ACCESS_Frame( 12L ) )
            return error;

          range->image_size = GET_ULong();
          metrics           = &range->metrics;

          metrics->height = GET_Byte();
          metrics->width  = GET_Byte();

          metrics->horiBearingX = GET_Char();
          metrics->horiBearingY = GET_Char();
          metrics->horiAdvance  = GET_Byte();

          metrics->vertBearingX = GET_Char();
          metrics->vertBearingY = GET_Char();
          metrics->vertAdvance  = GET_Byte();

          FORGET_Frame();
        }

        if ( format != 2 )   /* load range codes, formats 4 and 5 */
          error = Load_Range_Codes( range, face, (format == 4) );
      }
      break;

    default:
      error = TT_Err_Invalid_File_Format;
    }

    PTRACE3(( "Embedded Bitmap Location Tables loaded.\n" ));

    return error;
  }


/*******************************************************************
 *
 *  Function    :  Load_TrueType_Eblc
 *
 *  Description :  Loads the Eblc table directory into face table.
 *
 *  Input  :  face    face record to look for
 *
 *  Output :  Error code.
 *
 ******************************************************************/

  static
  TT_Error  Load_TrueType_Eblc( PFace     face,
                                TT_EBLC*  eblc )
  {
    DEFINE_LOCALS;

    ULong   eblc_offset;
    UShort  i;
    Long    table;

    TT_SBit_Strike*  strike;


    PTRACE2(( "Load_EBLC_Table( %08lx )\n", (long)face ));

    eblc->version = 0;

    /* Try to find the `EBLC' or `bloc' table in the font files. */
    /* Both tags describe the same table; `EBLC' is for OpenType */
    /* fonts while `bloc' is for TrueType GX fonts.  Many fonts  */
    /* contain both tags pointing to the same table.             */

    table = TT_LookUp_Table( face, TTAG_EBLC );
    if ( table < 0 )
      table = TT_LookUp_Table( face, TTAG_bloc );

    if ( table < 0 )
      /* This table is optional */
      return TT_Err_Ok;

    eblc_offset = face->dirTables[table].Offset;

    if ( FILE_Seek( eblc_offset ) ||
         ACCESS_Frame( 8L )       )
      return error;

    eblc->version     = GET_ULong();
    eblc->num_strikes = GET_ULong();

    FORGET_Frame();

    PTRACE2(( "-- Tables count:   %12u\n",  eblc->num_strikes ));
    PTRACE2(( "-- Format version: %08lx\n", eblc->version     ));

    if ( eblc->version != 0x00020000 )
    {
      PERROR(( "Invalid file format!\n" ));
      return TT_Err_Invalid_File_Format;
    }

    if ( ALLOC_ARRAY( eblc->strikes, eblc->num_strikes, TT_SBit_Strike ) ||
         ACCESS_Frame( 48L * eblc->num_strikes )                         )
      return error;

    strike = eblc->strikes;

    for ( i = 0; i < eblc->num_strikes; i++, strike++ )
    {          /* loop through the tables and get all entries */
      ULong                  indexTablesSize;
      TT_SBit_Line_Metrics*  metrics;
      Int                    count;


      strike->ranges_offset = GET_ULong();
      indexTablesSize       = GET_ULong();  /* dont' save */

      strike->num_ranges    = GET_ULong();
      strike->color_ref     = GET_ULong();

      /* load horizontal and vertical metrics */
      metrics = &strike->hori;
      for ( count = 2; count > 0; count-- )
      {
        metrics->ascender  = GET_Char();
        metrics->descender = GET_Char();
        metrics->max_width = GET_Byte();

        metrics->caret_slope_numerator   = GET_Char();
        metrics->caret_slope_denominator = GET_Char();
        metrics->caret_offset            = GET_Char();

        metrics->min_origin_SB  = GET_Char();
        metrics->min_advance_SB = GET_Char();
        metrics->max_before_BL  = GET_Char();
        metrics->min_after_BL   = GET_Char();
        metrics->pads[0]        = GET_Char();
        metrics->pads[1]        = GET_Char();

        metrics = &strike->vert;
      }

      strike->start_glyph = GET_UShort();
      strike->end_glyph   = GET_UShort();
      strike->x_ppem      = GET_Byte();
      strike->y_ppem      = GET_Byte();
      strike->bit_depth   = GET_Byte();
      strike->flags       = GET_Char();

      PTRACE4(( " start - end - ppemX - ppemY\n" ));
      PTRACE4(( " %04d - %04d - %3u - %3u\n",
                  strike->start_glyph,
                  strike->end_glyph,
                  strike->x_ppem,
                  strike->y_ppem ));
    }

    FORGET_Frame();

    /* Load EBLC index ranges */
    strike = eblc->strikes;

    for ( i = 0; i < eblc->num_strikes; i++, strike++ )
    {
      TT_SBit_Range*  range;
      UShort          count = strike->num_ranges;


      /* loop through the tables and get all entries */
      if ( ALLOC_ARRAY( strike->sbit_ranges,
                        strike->num_ranges,
                        TT_SBit_Range )                     ||
           FILE_Seek( eblc_offset + strike->ranges_offset ) ||
           ACCESS_Frame( strike->num_ranges * 8L )          )
        return error;

      for ( range = strike->sbit_ranges; count > 0; count--, range++ )
      {
        range->first_glyph   = GET_UShort();
        range->last_glyph    = GET_UShort();
        range->table_offset  = eblc_offset + strike->ranges_offset +
                                 GET_ULong();
      }
      FORGET_Frame();

      /* Now, read each index table */
      range = strike->sbit_ranges;
      for ( count = strike->num_ranges; count > 0; count--, range++ )
      {
        /* Read the header */
        if ( FILE_Seek( range->table_offset ) ||
             ACCESS_Frame( 8L )              )
          return error;;

        range->index_format = GET_UShort();
        range->image_format = GET_UShort();
        range->image_offset = GET_ULong();

        FORGET_Frame();

        error = Load_SBit_Range( strike, range, face );
        if (error) return error;
      }
    }

    return TT_Err_Ok;
  }


  static
  void  Free_TrueType_Eblc( TT_EBLC*  eblc )
  {
    if ( eblc )
    {
      ULong            i;
      TT_SBit_Strike*  strike = eblc->strikes;


      strike = eblc->strikes;

      for ( i = eblc->num_strikes; i > 0; i--, strike++ )
      {
        /* for each strike, release all glyph ranges */
        TT_SBit_Range*  range = strike->sbit_ranges;
        Int             n;


        for ( n = strike->num_ranges; n > 0; n--, range++ )
        {
          /* release a range */
          FREE( range->glyph_offsets );
          FREE( range->glyph_codes );
        }
        FREE( strike->sbit_ranges );
        strike->num_ranges = 0;
      }
      FREE( eblc->strikes );
      eblc->num_strikes = 0;
      eblc->version     = 0;
    }
  }


  static
  TT_Error Load_SBit_Metrics( TT_Big_Glyph_Metrics*  metrics,
                              TT_SBit_Range*         range,
                              ULong                  ebdt_offset )
  {
    TT_Error  error;
    Byte      height, width;


    /* copy bitmap metrics for formats 2 and 5 */
    if ( ( ( range->index_format == 2 ) || ( range->index_format == 5 ) ) &&
         ( range->image_format == 5 ) )
                         /* metrics are taken from current image bitmap */
                         /* i.e. from `image.metrics'                   */
    {
      TT_SBit_Metrics*  rmetrics = &range->metrics;


      metrics->bbox.xMin = rmetrics->horiBearingX;
      metrics->bbox.xMax = metrics->bbox.xMin + rmetrics->width;

      metrics->bbox.yMax = rmetrics->horiBearingY;
      metrics->bbox.yMin = metrics->bbox.yMax - rmetrics->height;

      metrics->horiBearingX = rmetrics->horiBearingX;
      metrics->horiBearingY = metrics->bbox.yMax;
      metrics->horiAdvance  = rmetrics->horiAdvance;

      metrics->vertBearingX = rmetrics->vertBearingX;
      metrics->vertBearingY = rmetrics->vertBearingY;
      metrics->vertAdvance  = rmetrics->vertAdvance;

      return TT_Err_Ok;
    }

    switch ( range->image_format )
    {
    case 1:
    case 2:
    case 6:
    case 7:
    case 8:
    case 9:
      {
        Long  length = 5L;


        if ( range->image_format == 8 )
          length++;

        /* read the small metrics */
        if ( ACCESS_Frame( length ) )
          return error;

        height = GET_Byte();
        width  = GET_Byte();

        metrics->horiBearingX = GET_Char();
        metrics->horiBearingY = GET_Char();
        metrics->horiAdvance  = GET_Byte();

        FORGET_Frame();

        metrics->bbox.xMin = metrics->horiBearingX;
        metrics->bbox.yMax = metrics->horiBearingY;
        metrics->bbox.xMax = metrics->bbox.xMin + width;
        metrics->bbox.yMin = metrics->bbox.yMax - height;

        /* read the rest of the big metrics for the formats */
        /* that support it.                                 */
        if ( ( range->image_format >= 6 ) && ( range->image_format != 8 ) )
        {
          if ( ACCESS_Frame( 3L ) )
            return error;

          metrics->vertBearingX = (Int)GET_Char();
          metrics->vertBearingY = (Int)GET_Char();
          metrics->vertAdvance  = (Int)GET_Char();

          FORGET_Frame();
        }
        else
        {
          /* XXX: How can we fill these when the information isn't */
          /*      available?                                       */
          metrics->vertBearingX = 0;
          metrics->vertBearingY = 0;
          metrics->vertAdvance  = 0;
        }
      }
      break;

    case 5:  /* metrics are taken from current image bitmap */
             /* i.e. from 'image.metrics'                   */
      break;

    default:
      PERROR(( "Unsupported embedded bitmap format!\n" ));
      return TT_Err_Invalid_File_Format;
    }

    return TT_Err_Ok;
  }


  static
  TT_Error  Load_SBit_Image( TT_SBit_Strike  strike,
                             UShort          glyph_index,
                             Byte            x_offset,
                             Byte            y_offset,
                             ULong           ebdt_offset,
                             TT_SBit_Image*  image,
                             UShort          component_depth )
  {
    TT_Error              error;
    Byte                  height, width;

    ULong                 bitmap_offset;

    TT_SBit_Range*        range = 0;

    TT_Big_Glyph_Metrics  metrics;

    /********************************************************************/
    /*                                                                  */
    /* Scan the strike's range for the position/metrics of the source   */
    /* glyph.                                                           */
    {
      UShort          count = strike.num_ranges;
      TT_SBit_Range*  cur   = strike.sbit_ranges;

      for ( ; count > 0; count--, cur++ )
      {
        /* look for the glyph in the current range */
        switch ( cur->index_format )
        {
        case 1:
        case 2:
        case 3:
          if ( glyph_index >= cur->first_glyph &&
               glyph_index <= cur->last_glyph  )
          {
            UShort  delta = glyph_index - cur->first_glyph;


            range         = cur;
            bitmap_offset = cur->index_format == 2
                           ? cur->image_offset + cur->image_size * delta
                           : cur->glyph_offsets[delta];
            goto Found;
          }
          break;

        case 4:
        case 5:
          {
            UShort  n;


            for ( n = 0; n < cur->num_glyphs; n++ )
              if ( cur->glyph_codes[n] == glyph_index )
              {
                range         = cur;
                bitmap_offset = cur->index_format == 4
                               ? cur->glyph_offsets[n]
                               : cur->image_offset + cur->image_size * n;
                goto Found;
              }
          }
          break;

        default:
          return TT_Err_Invalid_Glyph_Index;
        }
      }
      /* Not found */
      return TT_Err_Invalid_Glyph_Index;
    }

  Found:
    if ( FILE_Seek( ebdt_offset + bitmap_offset ) )
      return error;

    /* First of all, load the metrics if needed */
    error = Load_SBit_Metrics( &metrics, range, ebdt_offset );
    if ( error )
      return error;

    width  = metrics.bbox.xMax - metrics.bbox.xMin;
    height = metrics.bbox.yMax - metrics.bbox.yMin;

    if ( !component_depth )
    {
      image->metrics   = metrics;

      image->map.width = width;
      image->map.rows  = height;

      image->map.cols  = (width + 7) >> 3;
      image->map.size  = height * image->map.cols;

      if ( REALLOC( image->map.bitmap, image->map.size ) )
        return error;

      MEM_Set( image->map.bitmap, 0, image->map.size );
    }

    /* Now, load the data as needed */
    switch ( range->image_format )
    {
    case 1:
    case 6: /* byte-aligned data */
      error = Load_BitmapData( image,
                               height * (( width + 7 ) >> 3),
                               x_offset, y_offset,
                               width, height,
                               1 );
      if ( error )
        return error;
      break;

    case 2:
    case 5:
    case 7:
      error = Load_BitmapData( image,
                               (width * height + 7) >> 3,
                               x_offset, y_offset,
                               width, height, 0 );
      if ( error )
        return error;
      break;

    case 8:
    case 9:
      {
        /* Now, load composite sbit glyphs */
        /* This code is not sophisticated */

        TT_SBit_Component*  component_array;
        UShort              num_components;

        Int i = 0;


        if ( ACCESS_Frame( 2L ) )
          return error;
        num_components = GET_UShort();
        FORGET_Frame();

        MEM_Alloc( component_array,
                   sizeof ( TT_SBit_Component ) * num_components );

        if ( ACCESS_Frame( 4L * num_components ) )
          return error;

        for ( i = 0; i < num_components; i++ )
        {
          component_array[i].glyph_code = GET_UShort();
          component_array[i].x_offset   = GET_Char();
          component_array[i].y_offset   = GET_Char();
        }

        FORGET_Frame();

        component_depth++;

        for ( i = 0; i < num_components; i++ )
        {
          error = Load_SBit_Image( strike, component_array[i].glyph_code,
                                   component_array[i].x_offset,
                                   component_array[i].y_offset,
                                   ebdt_offset,
                                   image,
                                   component_depth );
          if ( error )
            return error;
        }
        FREE( component_array );
        break;

      default:
        return TT_Err_Invalid_File_Format;
      }
    }

    return TT_Err_Ok;
  }


/*******************************************************************
 *
 *  Function:  Load_TrueType_Ebdt
 *
 ******************************************************************/

  static
  TT_Error  Load_TrueType_Ebdt( PFace           face,
                                TT_SBit_Strike  strike,
                                ULong           glyph_index,
                                TT_SBit_Image*  image )
  {
    DEFINE_LOCALS;

    ULong   ebdt_offset;
    ULong   version;
    Long    i;


    /* Try to find the `EBDT' or `bdat' table in the font files. */
    /* Both tags describe the same table, `EBDT' is for OpenType */
    /* fonts, while `bdat' is for TrueType GX fonts.  Many fonts */
    /* contain both tags pointing to the same table              */

    i = TT_LookUp_Table( face, TTAG_EBDT );
    if ( i < 0 )
      i = TT_LookUp_Table( face, TTAG_bdat );

    if ( i < 0 )
      return TT_Err_Table_Missing;

    ebdt_offset = face->dirTables[i].Offset;

    if ( FILE_Seek( ebdt_offset ) ||
         ACCESS_Frame( 4L )       )  /* read into frame */
      return error;

    version  = GET_ULong();
    FORGET_Frame();

    PTRACE2(( "-- Format version : %08lx\n", version ));
    if ( version != 0x00020000 )
    {
      PERROR(( "Invalid file format!\n" ));
      return TT_Err_Invalid_File_Format;
    }

 /* This doesn't compile, I simply commented it out ?? - David */
 /*    PTRACE4(( "-- Format: %d\n", range->image_format ));    */

    error = Load_SBit_Image( strike,
                             glyph_index,
                             0, 0,
                             ebdt_offset,
                             image,
                             0 );
    if ( error )
      return error;

    return TT_Err_Ok;
  }


  static TT_Error  EBLC_Create( void*  ext,
                                PFace  face )
  {
    TT_EBLC*  eblc = (TT_EBLC*)ext;


    /* by convention */
    if ( !eblc )
      return TT_Err_Ok;

    return Load_TrueType_Eblc( face, eblc );
  }


  static TT_Error  EBLC_Destroy( void*  ext,
                                 PFace  face )
  {
    TT_EBLC*  eblc = (TT_EBLC*)ext;


    (void)face;

    if ( eblc )
      Free_TrueType_Eblc( eblc );

    return TT_Err_Ok;
  }


  /*************************************************************/
  /*                                                           */
  /* <Function>                                                */
  /*    TT_Init_SBit_Extension                                 */
  /*                                                           */
  /* <Description>                                             */
  /*    Initialize the embedded bitmaps extension for the      */
  /*    FreeType engine.                                       */
  /*                                                           */
  /* <Input>                                                   */
  /*    engine :: handle to current FreeType library instance  */
  /*                                                           */
  /* <Return>                                                  */
  /*    Error code. 0 means success.                           */
  /*                                                           */
  EXPORT_FUNC
  TT_Error  TT_Init_SBit_Extension( TT_Engine  engine )
  {
    PEngine_Instance  _engine = HANDLE_Engine( engine );

    TT_Error  error;


    if ( !_engine )
      return TT_Err_Invalid_Engine;

    error = TT_Register_Extension( _engine,
                                   SBIT_ID,
                                   sizeof ( TT_EBLC ),
                                   EBLC_Create,
                                   EBLC_Destroy );

    return error;
  }


  /*************************************************************/
  /*                                                           */
  /* <Function>                                                */
  /*    TT_Get_Face_Bitmaps                                    */
  /*                                                           */
  /* <Description>                                             */
  /*    Loads the `EBLC' table from a font file, if any.       */
  /*                                                           */
  /* <Input>                                                   */
  /*    face :: handle to the source TrueType font/face        */
  /*                                                           */
  /* <Output>                                                  */
  /*    eblc_table :: a descriptor for the EBLC table          */
  /*                                                           */
  /* <Return>                                                  */
  /*    Error code.  0 means success.                          */
  /*                                                           */
  /* <Note>                                                    */
  /*    This function returns TT_Err_Table_Missing if the      */
  /*    font contains no embedded bitmaps.  All fields in      */
  /*    `eblc_table' will then be set to 0.                    */
  /*                                                           */
  EXPORT_FUNC
  TT_Error  TT_Get_Face_Bitmaps( TT_Face   face,
                                 TT_EBLC*  eblc_table )
  {
    PFace     faze = HANDLE_Face( face );
    TT_EBLC*  eblc;
    TT_Error  error;


    error = TT_Extension_Get( faze, SBIT_ID, (void**)&eblc );
    if ( !error )
    {
      if ( eblc->version )
      {
        *eblc_table = *eblc;
        return TT_Err_Ok;
      }
      error = TT_Err_Table_Missing;
    }

    eblc_table->version     = 0;
    eblc_table->num_strikes = 0;
    eblc_table->strikes     = 0;

    return error;
  }


/*******************************************************************
 *
 *  <Function> TT_Get_SBit_Strike
 *
 *  <Description>
 *     Loads suitable strike (bitmap sizetable) for given instance.
 *     This strike includes sbitLineMetrics.
 *
 *  <Input>
 *     face     :: the source face
 *     instance :: the current size instance
 *
 *  <Output>
 *     strike   :: the bitmap strike descriptor
 *
 *  <Return>
 *     TrueType error code.  0 means success.
 *
 ******************************************************************/

  EXPORT_FUNC
  TT_Error  TT_Get_SBit_Strike( TT_Face          face,
                                TT_Instance      instance,
                                TT_SBit_Strike*  strike )
  {
    TT_Error   error;
    PFace      faze = HANDLE_Face( face );
    PInstance  ins  = HANDLE_Instance( instance );

    TT_EBLC*   eblc;
    TT_Int     x_ppem, y_ppem;


    if ( !strike || !ins || ins->owner != faze )
      return TT_Err_Invalid_Argument;

    error = TT_Extension_Get( faze, SBIT_ID, (void**)&eblc );
    if ( error )
      goto Exit;

    /********************************************************************/
    /*                                                                  */
    /* Look for an sbit strike that matches the current x and y ppms    */
    /*                                                                  */
    {
      UShort           count = eblc->num_strikes;
      TT_SBit_Strike*  cur   = eblc->strikes;


      x_ppem = ins->metrics.x_ppem;
      y_ppem = ins->metrics.y_ppem;

      MEM_Set( strike, 0, sizeof ( TT_SBit_Strike ) );

      for ( ; count > 0; count--, cur++ )
        if ( cur->x_ppem == x_ppem &&
             cur->y_ppem == y_ppem )
        {
          *strike = *cur;
          break;
        }

      /* return immediately if we didn't find an appropriate strike */
      if ( !strike->num_ranges )
        error = TT_Err_Invalid_PPem;
    }

  Exit:
    return error;
  }


  /*************************************************************/
  /*                                                           */
  /* <Function>                                                */
  /*    TT_Load_Glyph_Bitmap                                   */
  /*                                                           */
  /* <Description>                                             */
  /*    Loads a given glyph embedded bitmap.                   */
  /*                                                           */
  /* <Input>                                                   */
  /*    face        :: handle to the source TrueType font/face */
  /*    instance    :: current size/transform instance         */
  /*    glyph_index :: index of source glyph                   */
  /*    bitmap      :: target embedded bitmap descriptor       */
  /*                                                           */
  /* <Return>                                                  */
  /*    Error code.  0 means success.                          */
  /*                                                           */
  /* <Note>                                                    */
  /*    This function returns an error if there is no          */
  /*    embedded bitmap for the glyph at the given             */
  /*    instance.                                              */
  /*                                                           */
  EXPORT_FUNC
  TT_Error  TT_Load_Glyph_Bitmap( TT_Face         face,
                                  TT_Instance     instance,
                                  TT_UShort       glyph_index,
                                  TT_SBit_Image*  image )
  {
    TT_Stream  stream;
    TT_Error   error;

    PFace      faze = HANDLE_Face( face );
    PInstance  ins  = HANDLE_Instance( instance );

    TT_SBit_Strike  strike;


    if ( ins->owner != faze )
    {
      error = TT_Err_Invalid_Argument;
      goto Fail;
    }

    /********************************************************************/
    /*                                                                  */
    /* Look for an sbit strike that matches the current x and y ppms    */
    /*                                                                  */
    error = TT_Get_SBit_Strike( face, instance, &strike );
    if ( error )
      goto Fail;

    /* return immediately if the glyph index isn't in the strike extent */
    if ( glyph_index < strike.start_glyph ||
         glyph_index > strike.end_glyph   )
    {
      error = TT_Err_Invalid_Glyph_Index;
      goto Fail;
    }

    {
      image->bit_depth = 1;

      if ( !USE_Stream( faze->stream, stream ) )
      {
        error = Load_TrueType_Ebdt( faze, strike, glyph_index, image );

        DONE_Stream( stream );

        /* exit successfully if we can */
        if ( !error )
        {
          image->map.flow = TT_Flow_Down;

          Crop_Bitmap( image );

          /* correct sbit metrics */
          {
            TT_Big_Glyph_Metrics*  metrics = &image->metrics;


            metrics->bbox.xMin *= 64;
            metrics->bbox.xMax *= 64;

            metrics->bbox.yMax *= 64;
            metrics->bbox.yMin *= 64;

            metrics->horiBearingX *= 64;
            metrics->horiBearingY *= 64;
            metrics->horiAdvance  *= 64;

            metrics->vertBearingX *= 64;
            metrics->vertBearingY *= 64;
            metrics->vertAdvance  *= 64;
          }

          goto Exit;
        }
      }
    }

  Fail:
    image->map.width  = 0;
    image->map.rows   = 0;
    image->map.cols   = 0;
    image->map.size   = 0;
    image->map.bitmap = 0;
    image->map.flow   = 0;
    image->bit_depth  = 0;

  Exit:
    return error;
  }


  /*************************************************************/
  /*                                                           */
  /* <Function>                                                */
  /*    TT_New_SBit_Image                                      */
  /*                                                           */
  /* <Description>                                             */
  /*    Allocates a new embedded bitmap container.             */
  /*                                                           */
  /* <Output>                                                  */
  /*    image :: sbit image                                    */
  /*                                                           */
  /* <Return>                                                  */
  /*    Error code.  0 means success.                          */
  /*                                                           */
  EXPORT_FUNC
  TT_Error  TT_New_SBit_Image( TT_SBit_Image**  image )
  {
    return MEM_Alloc( *image, sizeof ( **image ) );
  }


  /*************************************************************/
  /*                                                           */
  /* <Function>                                                */
  /*    TT_Done_SBit_Image                                     */
  /*                                                           */
  /* <Description>                                             */
  /*    Releases an embedded bitmap container.                 */
  /*                                                           */
  /* <Input>                                                   */
  /*    image :: sbit image                                    */
  /*                                                           */
  EXPORT_FUNC
  void  TT_Done_SBit_Image( TT_SBit_Image*  image )
  {
    FREE( image->map.bitmap );
    FREE( image );
  }


/* END */
