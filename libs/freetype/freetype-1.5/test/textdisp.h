/****************************************************************************/
/*                                                                          */
/*  The FreeType project -- a free and portable quality TrueType renderer.  */
/*                                                                          */
/*  Copyright 1996-2001 by                                                  */
/*  D. Turner, R.Wilhelm, and W. Lemberg                                    */
/*                                                                          */
/*  textdisp.h: Text-mode display component used by some test programs.     */
/*                                                                          */
/*  This file is used to display glyphs using only text, with ' ' and '*'.  */
/*                                                                          */
/****************************************************************************/

#ifndef TEXTDISP_H
#define TEXTDISP_H

#include "freetype.h" /* TT_Raster_Map */

#ifdef __cplusplus
  extern "C" {
#endif

  void  Show_Single_Glyph( const TT_Raster_Map*  map );

#ifdef __cplusplus
  }
#endif

#endif /* TEXTDISP_H */


/* End */
