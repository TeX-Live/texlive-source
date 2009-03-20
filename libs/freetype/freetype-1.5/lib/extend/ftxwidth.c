/*******************************************************************
 *
 *  ftxwidth.c                                                  1.0
 *
 *    Glyph Widths (and Heights) fast retrieval extension
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
 *
 *  This extension is used to parse the "glyf" table of a TrueType
 *  file in order to extract the bbox of a given range of glyphs.
 *
 *  The bbox is then used to build font unit widths and height
 *  that are returned in two parallel arrays.
 *
 *  This extension is needed by the FreeType/2 OS/2 Font Driver.
 *
 ******************************************************************/


#include "ftxwidth.h"
#include "ttdebug.h"
#include "ttobjs.h"
#include "ttfile.h"
#include "tttags.h"
#include "ttload.h"

/* Required by the tracing mode */

#undef  TT_COMPONENT
#define TT_COMPONENT  trace_any


  /******************************************************************/
  /*                                                                */
  /*  Function:  TT_Get_Face_Widths                                 */
  /*                                                                */
  /*  Description: Returns the widths and/or heights of a given     */
  /*               range of glyphs for a face.                      */
  /*                                                                */
  /*  Input:                                                        */
  /*     face        ::  face handle                                */
  /*                                                                */
  /*     first_glyph ::  first glyph in range                       */
  /*                                                                */
  /*     last_glyph  ::  last glyph in range                        */
  /*                                                                */
  /*     widths      ::  address of table receiving the widths      */
  /*                     expressed in font units (ushorts). Set     */
  /*                     this parameter to NULL if you're not       */
  /*                     interested by these values.                */
  /*                                                                */
  /*     heights     ::  address of table receiving the heights     */
  /*                     expressed in font units (ushorts).  Set    */
  /*                     this parameter to NULL if you're not       */
  /*                     interested by these values.                */
  /*                                                                */
  /*  Returns:                                                      */
  /*       Error code                                               */
  /*                                                                */
  /*                                                                */
  /******************************************************************/

  FT_EXPORT_FUNC( TT_Error )
  TT_Get_Face_Widths( TT_Face     face,
                      TT_UShort   first_glyph,
                      TT_UShort   last_glyph,
                      TT_UShort*  widths,
                      TT_UShort*  heights )
  {
    DEFINE_ALL_LOCALS;

    PFace     faze = HANDLE_Face(face);
    UShort    n;
    Long      table;

    ULong     glyf_offset;      /* offset of glyph table in file */
    UShort    zero_width  = 0;  /* width of glyph 0  */
    UShort    zero_height = 0;  /* height of glyph 0 */

    Bool      zero_loaded = 0;

#ifndef TT_HUGE_PTR
    PStorage  locations;
#else
    Storage TT_HUGE_PTR * locations;
#endif
    TT_BBox   bbox;


    if ( !faze )
      return TT_Err_Invalid_Face_Handle;

    if ( last_glyph >= faze->numGlyphs ||
         first_glyph > last_glyph      )
      return TT_Err_Invalid_Argument;

    /* find "glyf" table */
    table = TT_LookUp_Table( faze, TTAG_glyf );
    if ( table < 0 )
    {
      PERROR(( "ERROR: there is no glyph table in this font file!\n" ));
      return TT_Err_Glyf_Table_Missing;
    }
    glyf_offset = faze->dirTables[table].Offset;

    /* now access stream */
    if ( USE_Stream( faze->stream, stream ) )
      return error;

    locations = faze->glyphLocations + first_glyph;

    /* loop to load each glyph in the range */
    for ( n = first_glyph; n <= last_glyph; n++ )
    {
      if ( n + 1 < faze->numGlyphs &&
           locations[0] == locations[1] )
      {
        /* Note : Glyph 0 is always used to indicate a missing glyph   */
        /*        in a range. We must thus return its width and height */
        /*        where appropriate when we find an undefined glyph.   */
        if ( zero_loaded == 0 )
        {
          if ( FILE_Seek( glyf_offset + faze->glyphLocations[0] ) ||
               ACCESS_Frame( 10L ) )
            goto Fail;

          (void)GET_Short();   /* skip number of contours */

          bbox.xMin = GET_Short();
          bbox.yMin = GET_Short();
          bbox.xMax = GET_Short();
          bbox.yMax = GET_Short();

          FORGET_Frame();

          zero_width  = (UShort)(bbox.xMax - bbox.xMin);
          zero_height = (UShort)(bbox.yMax - bbox.yMin);
          zero_loaded = 1;
        }

        if ( widths )
          *widths++  = zero_width;

        if ( heights )
          *heights++ = zero_height;
      }
      else
      {
        /* normal glyph, read header */
        if ( FILE_Seek( glyf_offset + locations[0] ) ||
             ACCESS_Frame( 10L ) )
          goto Fail;

        (void)GET_Short();  /* skip number of contours */

        bbox.xMin = GET_Short();
        bbox.yMin = GET_Short();
        bbox.xMax = GET_Short();
        bbox.yMax = GET_Short();

        FORGET_Frame();

        if ( widths )
          *widths++  = (UShort)(bbox.xMax - bbox.xMin);

        if ( heights )
          *heights++ = (UShort)(bbox.yMax - bbox.yMin);
      }
    }

  Fail:
    DONE_Stream( stream );
    return error;
  }


/* END */
