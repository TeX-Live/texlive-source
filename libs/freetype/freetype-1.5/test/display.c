/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2000 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  display.c: Display component used by all test programs.                 */
/*                                                                          */
/*  This file is used to display glyphs and strings in a target window      */
/*  using the graphics drivers provided by gmain.c, gevents.h, etc.         */
/*                                                                          */
/*  Its role is to be shared and heavely commented to let people understand */
/*  how we do the job...                                                    */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>  /* malloc() and free() */
#include <string.h>  /* memset()            */

#include "display.h"
#include "freetype.h"
#include "gmain.h"

  /* The target bitmap or pixmap -- covering the full display window/screen */
  TT_Raster_Map  Bit;

  /* A smaller intermediate bitmap used to render individual glyphs when    */
  /* font smoothing mode is activated.  It is then or-ed to `Bit'.          */
  TT_Raster_Map  Small_Bit;

/* The magic of or-ing gray-levels:                                         */
/*                                                                          */
/* When gray-level mode (a.k.a. font-smoothing) is on, `Bit' is an 8-bit    */
/* pixmap of the size of the display window or screen.                      */
/*                                                                          */
/* The gray-level palette to use for display is system-dependent, and       */
/* given by the "gray_palette" variable defined in 'gmain.c'.  It is        */
/* set up by the graphics driver, and we cannot assume anything about its   */
/* values.                                                                  */
/*                                                                          */
/* The function TT_Get_Glyph_Pixmap() can use any palette to render each    */
/* individual glyph, however we'll later need to "or" the glyph to          */
/* the display pixmap `Bit' to be able to form strings of text by           */
/* juxtaposing several glyphs together.                                     */
/*                                                                          */
/* If we use the gray_palette directly, we'll encounter trouble doing       */
/* the "or".  Example:                                                      */
/*                                                                          */
/*    Suppose that gray_palette = { 0, 32, 64, 128, 255 }                   */
/*                                                                          */
/* Let's render a glyph with this palette and "or" it to                    */
/* the `Bit' pixmap.  If, by chance, we superpose two distinct non-zero     */
/* colors, we will get strange results, like 32+64 = 96, which isn't in     */
/* our gray palette!                                                        */
/*                                                                          */
/* There are two ways to solve this problem:                                */
/*                                                                          */
/*   - perform a "slow or" where we check all possible combinations         */
/*     and solve conflicts.                                                 */
/*                                                                          */
/*   - render all pixmaps using a special "virtual" palette that eases      */
/*     the "oring" process, then convert the whole display pixmap to        */
/*     "display" colors at once.                                            */
/*                                                                          */
/* We choose the second solution, of course; this means that:               */
/*                                                                          */
/*  - the virtual palette used is simply = { 0, 1, 2, 3, 4 }, defined in    */
/*    the variable "palette" below.  The `Bit' and `Small_Bit' pixmaps will */
/*    always contain pixels within these values, with the exception of      */
/*    post-render display, where `Bit' will be converted to display values  */
/*    by the Convert_To_Display_Palette() function.                         */
/*                                                                          */
/*  - as or-ing values between 0 and 4 will give us values between          */
/*    0 and 7, we use a second palette, called "bounding_palette"           */
/*    to maintain all values within the virtual palette.                    */
/*                                                                          */
/*    in effect bounding_palette = { 0, 1, 2, 3, 4, 4, 4, 4 }               */
/*                                                                          */
/*    which means that (3|4) == 7 => 4 after bounding                       */
/*                                                                          */

  /* the virtual palette */
  unsigned char  virtual_palette[5] = { 0, 1, 2, 3, 4 };

  /* Or-ing the possible palette values gets us from 0 to 7 */
  /* We must bound check these...                           */
  unsigned char  bounded_palette[8] = { 0, 1, 2, 3, 4, 4, 4, 4 };


  /* Clears the Bit bitmap/pixmap */
  void  Clear_Display( void )
  {
    memset( Bit.bitmap, 0, Bit.size );
  }


  /* Clears the Small_Bit pixmap */
  void  Clear_Small( void )
  {
    memset( Small_Bit.bitmap, 0, Small_Bit.size );
  }


  /* Initialize the display bitmap named Bit */
  int  Init_Display( int  font_smoothing )
  {
    Bit.rows   = vio_Height;  /* the whole window */
    Bit.width  = vio_Width;
    Bit.flow   = TT_Flow_Up;

    if ( font_smoothing )
      Bit.cols = (Bit.width+3) & -4;  /* must be 32-bits aligned */
    else
      Bit.cols = (Bit.width+7) >> 3;

    Bit.size   = (long)Bit.cols * Bit.rows;

    if ( Bit.bitmap )
      free( Bit.bitmap );
    Bit.bitmap = malloc( (int)Bit.size );
    if ( !Bit.bitmap )
      return -1;

    Clear_Display();
    return 0;
  }


  /* Convert the display pixmap from virtual to display palette */
  void  Convert_To_Display_Palette( void )
  {
    unsigned char*  p;
    long            i;

    p = Bit.bitmap;
    for ( i = 0; i < Bit.size; i++ )
    {
      *p = gray_palette[(int)*p];
      p++;
    }
  }


  /* Init Small Bitmap */
  int  Init_Small( int  x_ppem, int  y_ppem )
  {
    if ( Small_Bit.bitmap )
      free( Small_Bit.bitmap );

    Small_Bit.rows  = y_ppem + 32;
    Small_Bit.width = x_ppem + 32;
    Small_Bit.cols  = ( Small_Bit.width+3 ) & -4;  /* pad to 32-bits */
    Small_Bit.flow  = TT_Flow_Up;
    Small_Bit.size  = (long)Small_Bit.rows * Small_Bit.cols;

    Small_Bit.bitmap = malloc( (int)Small_Bit.size );
    if ( Small_Bit.bitmap )
      return -1;

    Clear_Small();
    return 0;
  }


  /* Render a single glyph into the display bit/pixmap                    */
  /*                                                                      */
  /* Note that in b/w mode, we simply render the glyph directly into      */
  /* the display map, as the scan-line converter or-es the glyph into     */
  /* the target bitmap.                                                   */
  /*                                                                      */
  /* In gray mode, however, the glyph is first rendered indivdually in    */
  /* the Small_Bit map, then 'or-ed' with bounding into the display       */
  /* pixmap.                                                              */
  /*                                                                      */

  TT_Error  Render_Single_Glyph( int       font_smoothing,
                                 TT_Glyph  glyph,
                                 int       x_offset,
                                 int       y_offset )
  {
    if ( !font_smoothing )
      return TT_Get_Glyph_Bitmap( glyph, &Bit,
                                  (long)x_offset*64, (long)y_offset*64 );
    else
    {
      TT_Glyph_Metrics  metrics;

      TT_Error    error;
      TT_F26Dot6  x, y, xmin, ymin, xmax, ymax;
      int         ioff, iread;
      char        *off, *read, *_off, *_read;


      /* font-smoothing mode */

      /* we begin by grid-fitting the bounding box */
      TT_Get_Glyph_Metrics( glyph, &metrics );

      xmin = metrics.bbox.xMin & -64;
      ymin = metrics.bbox.yMin & -64;
      xmax = (metrics.bbox.xMax+63) & -64;
      ymax = (metrics.bbox.yMax+63) & -64;

      /* now render the glyph in the small pixmap */

      /* IMPORTANT NOTE: the offset parameters passed to the function     */
      /* TT_Get_Glyph_Bitmap() must be integer pixel values, i.e.,        */
      /* multiples of 64.  HINTING WILL BE RUINED IF THIS ISN'T THE CASE! */
      /* This is why we _did_ grid-fit the bounding box, especially xmin  */
      /* and ymin.                                                        */

      Clear_Small();
      error = TT_Get_Glyph_Pixmap( glyph, &Small_Bit, -xmin, -ymin );
      if ( error )
        return error;

      /* Blit-or the resulting small pixmap into the biggest one */
      /* We do that by hand, and provide also clipping.          */

      xmin = (xmin >> 6) + x_offset;
      ymin = (ymin >> 6) + y_offset;
      xmax = (xmax >> 6) + x_offset;
      ymax = (ymax >> 6) + y_offset;

      /* Take care of comparing xmin and ymin with signed values!  */
      /* This was the cause of strange misplacements when Bit.rows */
      /* was unsigned.                                             */

      if ( xmin >= (int)Bit.width   ||
           ymin >= (int)Bit.rows    ||
           xmax < 0                 ||
           ymax < 0 )
        return TT_Err_Ok;  /* nothing to do */

      /* Note that the clipping check is performed _after_ rendering */
      /* the glyph in the small bitmap to let this function return   */
      /* potential error codes for all glyphs, even hidden ones.     */

      /* In exotic glyphs, the bounding box may be larger than the   */
      /* size of the small pixmap.  Take care of that here.          */

      if ( xmax-xmin + 1 > Small_Bit.width )
        xmax = xmin + Small_Bit.width - 1;

      if ( ymax-ymin + 1 > Small_Bit.rows )
        ymax = ymin + Small_Bit.rows - 1;

      /* set up clipping and cursors */

      iread = 0;
      if ( ymin < 0 )
      {
        iread -= ymin * Small_Bit.cols;
        ioff   = 0;
        ymin   = 0;
      }
      else
        ioff = ymin * Bit.cols;

      if ( ymax >= Bit.rows )
        ymax = Bit.rows-1;

      if ( xmin < 0 )
      {
        iread -= xmin;
        xmin   = 0;
      }
      else
        ioff += xmin;

      if ( xmax >= Bit.width )
        xmax = Bit.width - 1;

      _read = (char*)Small_Bit.bitmap + iread;
      _off  = (char*)Bit.bitmap       + ioff;

      for ( y = ymin; y <= ymax; y++ )
      {
        read = _read;
        off  = _off;

        for ( x = xmin; x <= xmax; x++ )
        {
          *off = bounded_palette[*off | *read];
          off++;
          read++;
        }
        _read += Small_Bit.cols;
        _off  += Bit.cols;
      }

      return TT_Err_Ok;
    }
  }



/* End */
