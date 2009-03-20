/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2000 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  display.h: Display component interface used by test programs            */
/*                                                                          */
/*  This file is used to display glyphs and strings in a target window      */
/*  using the graphics drivers provided by gmain.c, gevents.h, etc.         */
/*                                                                          */
/*  Its role is to be shared and heavely commented to let people understand */
/*  how we do the job...                                                    */
/*                                                                          */
/*  See comments in display.c for a full description!                       */
/*                                                                          */
/****************************************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "freetype.h" /* TT_Raster_Map */

  /* The target bitmap or pixmap -- covering the full display window/screen */
  extern  TT_Raster_Map  Bit;

  /* A smaller intermediate bitmap used to render individual glyphs when    */
  /* font smoothing mode is activated.  It is then or-ed to `Bit'.          */
  extern  TT_Raster_Map  Small_Bit;

  /* the virtual palette */
  extern unsigned char   virtual_palette[5];

  /* Or-ing the possible palette values gets us from 0 to 7 */
  /* We must bound check these...                           */
  extern unsigned char   bounded_palette[8];


  /* Clears the Bit bitmap/pixmap */
  void  Clear_Display( void );

  /* Clears the Small_Bit pixmap */
  void  Clear_Small( void );

  /* Initialize the display bitmap named Bit */
  int  Init_Display( int  font_smoothing );

  /* Initialize Small Bitmap */
  int  Init_Small( int  x_ppem, int  y_ppem );

  /* Convert the display pixmap from virtual to display palette */
  void  Convert_To_Display_Palette( void );

  /* Render a single glyph into the display bit/pixmap.                   */
  /*                                                                      */
  /* Note that in b/w mode, we simply render the glyph directly into      */
  /* the display map, as the scan-line converter or-es the glyph into     */
  /* the target bitmap.                                                   */
  /*                                                                      */
  /* In gray mode, however, the glyph is first rendered individually in   */
  /* the Small_Bit map, then 'or-ed' with bounding into the display       */
  /* pixmap.                                                              */
  /*                                                                      */
  TT_Error  Render_Single_Glyph( int       font_smoothing,
                                 TT_Glyph  glyph,
                                 int       x_offset,
                                 int       y_offset );

#endif /* DISPLAY_H */


/* End */
