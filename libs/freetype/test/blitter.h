/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  blitter.h: Support for blitting of bitmaps with various depth.          */
/*                                                                          */
/****************************************************************************/

#ifndef BLITTER_H
#define BLITTER_H

#include "freetype.h"

  /* <Function>                                                          */
  /*    Blit_Bitmap                                                      */
  /*                                                                     */
  /* <Description>                                                       */
  /*    blit a source bitmap to a target bitmap or pixmap                */
  /*                                                                     */
  /* <Input>                                                             */
  /*    target       :: target bitmap or pixmap                          */
  /*    source       :: source bitmap (depth must be 1)                  */
  /*    target_depth :: pixel bit depth of target map                    */
  /*    x_offset     :: horizontal offset of source in target            */
  /*    y_offset     :: vertical offset of source in target              */
  /*    color        :: color to use when blitting to color pixmap       */
  /*                                                                     */
  /* <Return>                                                            */
  /*    error code. 0 means success                                      */
  /*                                                                     */
  /* <Note>                                                              */
  /*    an error occurs when the target bit depth isn't supported, or    */
  /*    if the source's bit depth isn't 1.                               */
  /*                                                                     */
  /*    the offsets are relative to the top-left corner of the target    */
  /*    map. Positive y are downwards.                                   */
  /*                                                                     */
  extern
  int  Blit_Bitmap( TT_Raster_Map*  target,
                    TT_Raster_Map*  source,
                    int             target_depth,
                    int             x_offset,
                    int             y_offset,
                    int             color );

#endif


/* End */
