/*******************************************************************
 *
 *  ftxwidth.h
 *
 *    Glyph Widths (and Heights) fast retrieval extension.
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
 *  This extension is used to parse the `glyf' table of a TrueType
 *  file in order to extract the bounding box of a given range of glyphs.
 *
 *  The bounding box is then used to build font unit widths and heights
 *  that are returned in two parallel arrays.
 *
 *  This extension is needed by the FreeType/2 OS/2 Font Driver.
 *
 ******************************************************************/

#ifndef FTXWIDTH_H
#define FTXWIDTH_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

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
  /*                     expressed in font units (ushorts). Set     */
  /*                     this parameter to NULL if you're not       */
  /*                     interested by these values                 */
  /*                                                                */
  /*  Returns:                                                      */
  /*       Error code                                               */
  /*                                                                */
  /*                                                                */
  /******************************************************************/

  EXPORT_DEF
  TT_Error  TT_Get_Face_Widths( TT_Face     face,
                                TT_UShort   first_glyph,
                                TT_UShort   last_glyph,
                                TT_UShort*  widths,
                                TT_UShort*  heights );

#ifdef __cplusplus
}
#endif

#endif /* FTXWIDTH_H */


/* END */
