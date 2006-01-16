/*******************************************************************
 *
 *  ftxsbit.h
 *
 *    embedded bitmap support API extension
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

#ifndef FTXSBIT_H
#define FTXSBIT_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Metrics                                  */
  /*                                                           */
  /* <Description>                                             */
  /*    A structure used to hold the big metrics of a given    */
  /*    glyph bitmap in a TrueType or OpenType font.  These    */
  /*    are usually found in the `EBDT' table.                 */
  /*                                                           */
  /* <Fields>                                                  */
  /*    height       :: glyph height in pixels                 */
  /*    width        :: glyph width in pixels                  */
  /*                                                           */
  /*    horiBearingX :: horizontal left bearing                */
  /*    horiBearingY :: horizontal top bearing                 */
  /*    horiAdvance  :: horizontal advance                     */
  /*                                                           */
  /*    vertBearingX :: vertical left bearing                  */
  /*    vertBearingY :: vertical top bearing                   */
  /*    vertAdvance  :: vertical advance                       */
  /*                                                           */
  typedef struct  TT_SBit_Metrics_
  {
    TT_Byte  height;
    TT_Byte  width;

    TT_Char  horiBearingX;
    TT_Char  horiBearingY;
    TT_Byte  horiAdvance;

    TT_Char  vertBearingX;
    TT_Char  vertBearingY;
    TT_Byte  vertAdvance;

  } TT_SBit_Metrics;


  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Small_Metrics                            */
  /*                                                           */
  /* <Description>                                             */
  /*    A structure used to hold the small metrics of a given  */
  /*    glyph bitmap in a TrueType or OpenType font.  These    */
  /*    are usually found in the `EBDT' table.                 */
  /*                                                           */
  /* <Fields>                                                  */
  /*    height    :: glyph height in pixels                    */
  /*    width     :: glyph width in pixels                     */
  /*                                                           */
  /*    bearingX  :: left-side bearing                         */
  /*    bearingY  :: top-side bearing                          */
  /*    advance   :: advance width or height                   */
  /*                                                           */
  typedef struct  TT_SBit_Small_Metrics_
  {
    TT_Byte  height;
    TT_Byte  width;

    TT_Char  bearingX;
    TT_Char  bearingY;
    TT_Byte  advance;

  } TT_SBit_Small_Metrics;


  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Line_Metrics                             */
  /*                                                           */
  /* <Description>                                             */
  /*    A structure used to describe the text line metrics of  */
  /*    a given bitmap strike, for either horizontal or        */
  /*    vertical layout.                                       */
  /*                                                           */
  /* <Fields>                                                  */
  /*    ascender  :: ascender in pixels                        */
  /*    descender :: descender in pixels                       */
  /*    max_width :: maximum glyph width in pixels             */
  /*                                                           */
  /*    caret_slope_enumerator  :: Rise of the caret slope,    */
  /*                 typically set to 1 for non-italic fonts.  */
  /*    caret_slope_denominator :: Rise of the caret slope,    */
  /*                 typically set to 0 for non-italic fonts.  */
  /*    caret_offset            :: Offset in pixels            */
  /*            to move the caret for proper positioning.      */
  /*                                                           */
  /*    min_origin_SB  :: Minimum of horiBearingX              */
  /*                      (resp. vertBearingY)                 */
  /*    min_advance_SB :: Minimum of                           */
  /*               (hori. advance - ( horiBearingX + width ))  */
  /*         (resp. vert. advance - ( vertBearingY + height )) */
  /*    max_before_BL  :: Maximum of horiBearingY              */
  /*                      (resp. Maximum of vertBearingY)      */
  /*    min_after_BL   :: Minimum of ( horiBearingY - height ) */
  /*                      (resp. vertBearingX - width )        */
  /*                                                           */
  typedef struct  TT_SBit_Line_Metrics_
  {
    TT_Char  ascender;
    TT_Char  descender;
    TT_Byte  max_width;
    TT_Char  caret_slope_numerator;
    TT_Char  caret_slope_denominator;
    TT_Char  caret_offset;
    TT_Char  min_origin_SB;
    TT_Char  min_advance_SB;
    TT_Char  max_before_BL;
    TT_Char  min_after_BL;
    TT_Char  pads[2];

  } TT_SBit_Line_Metrics;


  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Range                                    */
  /*                                                           */
  /* <Description>                                             */
  /*    A TrueType/OpenType subIndexTable as defined in the    */
  /*    `EBLC' or `bloc' tables.                               */
  /*                                                           */
  /* <Fields>                                                  */
  /*                                                           */
  /*    first_glyph  :: first glyph index in range             */
  /*    last_glyph   :: last glyph index in range              */
  /*                                                           */
  /*    index_format :: format of index table. valid           */
  /*                    values are 1 to 5.                     */
  /*                                                           */
  /*    image_format :: format of `EBDT' image data            */
  /*    image_offset :: offset to image data in `EBDT'         */
  /*                                                           */
  /*    image_size   :: for index formats 2 and 5.  This is    */
  /*                    the size in bytes of each glyph bitmap */
  /*                    glyph bitmap                           */
  /*                                                           */
  /*    big_metrics  :: for index formats 2 and 5.  This is    */
  /*                    the big metrics for each glyph bitmap  */
  /*                                                           */
  /*    num_glyphs   :: for index formats 4 and 5.  This is    */
  /*                    the number of glyphs in the code       */
  /*                    array.                                 */
  /*                                                           */
  /*    glyph_offsets :: for index formats 1 and 3.            */
  /*    glyph_codes   :: for index formats 4 and 5.            */
  /*                                                           */
  /*    table_offset  :: offset of index table in `EBLC' table */
  /*                     -- only used during strike loading.   */
  /*                                                           */
  typedef struct  TT_SBit_Range
  {
    TT_UShort        first_glyph;
    TT_UShort        last_glyph;

    TT_UShort        index_format;
    TT_UShort        image_format;
    TT_ULong         image_offset;

    TT_ULong         image_size;
    TT_SBit_Metrics  metrics;
    TT_ULong         num_glyphs;

    TT_ULong*        glyph_offsets;
    TT_UShort*       glyph_codes;

    TT_ULong         table_offset;

  } TT_SBit_Range;


  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Strike                                   */
  /*                                                           */
  /* <Description>                                             */
  /*    A structure used describe a given bitmap strike in the */
  /*    `EBLC' or `bloc' tables.                               */
  /*                                                           */
  /* <Fields>                                                  */
  /*                                                           */
  /*   num_index_ranges :: number of index ranges              */
  /*   index_ranges     :: array of glyph index ranges         */
  /*                                                           */
  /*   color_ref   :: unused. color reference?                 */
  /*   hori        :: line metrics for horizontal layouts.     */
  /*   vert        :: line metrics for vertical layouts.       */
  /*                                                           */
  /*   start_glyph :: lowest glyph index for this strike.      */
  /*   end_glyph   :: higher glyph index for this strike.      */
  /*                                                           */
  /*   x_ppem      :: horizontal pixels per EM                 */
  /*   y_ppem      :: vertical pixels per EM                   */
  /*   bit_depth   :: bit depth. valid values are 1, 2, 4 & 8  */
  /*   flags       :: vertical or horizontal?                  */
  /*                                                           */
  typedef struct  TT_SBit_Strike_
  {
    TT_Int                num_ranges;
    TT_SBit_Range*        sbit_ranges;
    TT_ULong              ranges_offset;

    TT_ULong              color_ref;

    TT_SBit_Line_Metrics  hori;
    TT_SBit_Line_Metrics  vert;

    TT_UShort             start_glyph;
    TT_UShort             end_glyph;

    TT_Byte               x_ppem;
    TT_Byte               y_ppem;
    TT_Byte               bit_depth;
    TT_Char               flags;

  } TT_SBit_Strike;

  
  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Component                                */
  /*                                                           */
  /* <Description>                                             */
  /*    A simple structure to describe a compound sbit element */
  /*                                                           */
  /* <Fields>                                                  */
  /*    glyph_code :: element's glyph index                    */
  /*    x_offset   :: element's left bearing                   */
  /*    y_offset   :: element's top bearing                    */
  /*                                                           */
  typedef struct  TT_SBit_Component_
  {
    TT_UShort  glyph_code;
    TT_Char    x_offset;
    TT_Char    y_offset;

  } TT_SBit_Component;


  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Scale                                    */
  /*                                                           */
  /* <Description>                                             */
  /*    A structure used describe a given bitmap scaling       */
  /*    table, as defined for the `EBSC' table.                */
  /*                                                           */
  /* <Fields>                                                  */
  /*    hori   :: horizontal line metrics                      */
  /*    vert   :: vertical line metrics                        */
  /*                                                           */
  /*    x_ppem :: horizontal pixels per EM                     */
  /*    y_ppem :: vertical pixels per EM                       */
  /*                                                           */
  /*    x_ppem_substitute :: substitution x_ppem               */
  /*    y_ppem_substitute :: substitution y_ppem               */
  /*                                                           */
  typedef struct  TT_SBit_Scale_
  {
    TT_SBit_Line_Metrics  hori;
    TT_SBit_Line_Metrics  vert;

    TT_Byte               x_ppem;
    TT_Byte               y_ppem;

    TT_Byte               x_ppem_substitute;
    TT_Byte               y_ppem_substitute;

  } TT_SBit_Scale;


  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_SBit_Image                                    */
  /*                                                           */
  /* <Description>                                             */
  /*    A structure used to describe a given embedded bitmap   */
  /*    image.                                                 */
  /*                                                           */
  /* <Fields>                                                  */
  /*    map       :: bitmap descriptor                         */
  /*    bit_depth :: pixel bit depth                           */
  /*    metrics   :: glyph metrics for the bitmap              */
  /*                                                           */
  typedef struct  TT_SBit_Image_
  {
    TT_Raster_Map         map;
    int                   bit_depth;
    TT_Big_Glyph_Metrics  metrics;

  } TT_SBit_Image;


  /*************************************************************/
  /*                                                           */
  /* <Struct> TT_EBLC                                          */
  /*                                                           */
  /* <Description>                                             */
  /*    A structure used to describe the `EBLC' table from     */
  /*    a TrueType font.                                       */
  /*                                                           */
  /* <Fields>                                                  */
  /*    version     :: version number of the EBLC table        */
  /*                                                           */
  /*    num_strikes :: the number of strikes, i.e. bitmap      */
  /*                   sizes, present in this font             */
  /*                                                           */
  /*    strikes     :: array of strikes                        */
  /*                                                           */
  typedef struct  TT_EBLC_
  {
    TT_ULong         version;
    TT_ULong         num_strikes;
    TT_SBit_Strike*  strikes;

  } TT_EBLC;




  /*************************************************************/
  /*                                                           */
  /* <Function>                                                */
  /*    TT_Init_SBit_Extension                                 */
  /*                                                           */
  /* <Description>                                             */
  /*    Initializes the embedded bitmap extension for the      */
  /*    FreeType engine.                                       */
  /*                                                           */
  /* <Input>                                                   */
  /*    engine :: handle to current FreeType library instance  */
  /*                                                           */
  /* <Return>                                                  */
  /*    Error code.  0 means success.                          */
  /*                                                           */
  EXPORT_DEF
  TT_Error  TT_Init_SBit_Extension( TT_Engine  engine );


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
  /*    Error code. 0 means success.                           */
  /*                                                           */
  /* <Note>                                                    */
  /*    This function returns TT_Err_Table_Missing if the      */
  /*    font contains no embedded bitmaps.  All fields in      */
  /*    `eblc_table' will then be set to 0.                    */
  /*                                                           */
  EXPORT_DEF
  TT_Error  TT_Get_Face_Bitmaps( TT_Face   face,
                                 TT_EBLC*  eblc_table );


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
  EXPORT_DEF
  TT_Error  TT_New_SBit_Image( TT_SBit_Image**  image );


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
  EXPORT_DEF
  void  TT_Done_SBit_Image( TT_SBit_Image*  image );


  /*************************************************************/
  /*                                                           */
  /*  <Function> TT_Get_SBit_Strike                            */
  /*                                                           */
  /*  <Description>                                            */
  /*     Loads a suitable strike (bitmap sizetable) for the    */
  /*     given instance.  This strike includes                 */
  /*     sbitLineMetrics.                                      */
  /*                                                           */
  /*  <Input>                                                  */
  /*     face     :: the source face                           */
  /*     instance :: the current size instance                 */
  /*                                                           */
  /*  <Output>                                                 */
  /*     strike   :: the bitmap strike descriptor              */
  /*                                                           */
  /*  <Return>                                                 */
  /*     Error code.  0 means success.                         */
  /*                                                           */
  EXPORT_DEF
  TT_Error  TT_Get_SBit_Strike( TT_Face          face,
                                TT_Instance      instance,
                                TT_SBit_Strike*  strike );


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
  EXPORT_DEF
  TT_Error  TT_Load_Glyph_Bitmap( TT_Face         face,
                                  TT_Instance     instance,
                                  TT_UShort       glyph_index,
                                  TT_SBit_Image*  bitmap );

#ifdef __cplusplus
}
#endif

#endif /* FTXSBIT_H */


/* END */
