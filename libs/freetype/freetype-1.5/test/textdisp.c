/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  textdisp.c: Text-mode display component used by some test programs.     */
/*                                                                          */
/*  This file is used to display glyphs using only text, with ' ' and '*'.  */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>

#include "textdisp.h"

  void
  Show_Single_Glyph( const TT_Raster_Map*  map )
  {
    int             y;

    unsigned char*  line = map->bitmap;


    for ( y = 0; y < map->rows; y++, line += map->cols )
    {
      unsigned char*  ptr = line;
      int             x;
      unsigned char   mask = 0x80;


      for ( x = 0; x < map->width; x++ )
      {
        printf( "%c", (ptr[0] & mask) ? '*' : '.' );
        mask >>= 1;
        if ( mask == 0 )
        {
          mask = 0x80;
          ptr++;
        }
      }
      printf( "\n" );
    }
  }

/* End */
