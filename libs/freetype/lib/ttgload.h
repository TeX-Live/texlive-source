/*******************************************************************
 *
 *  ttgload.h                                                   1.0
 *
 *    TrueType Glyph Loader.
 *
 *  Copyright 1996-2000 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#ifndef TTGLOAD_H
#define TTGLOAD_H

#include "ttconfig.h"
#include "tttypes.h"
#include "ttobjs.h"

#ifdef __cplusplus
  extern "C" {
#endif


  FT_INTERNAL_DEF( void )
  TT_Get_Metrics( TT_Horizontal_Header*  header,
                  UShort                 index,
                  Short*                 bearing,
                  UShort*                advance );


  FT_INTERNAL_DEF( TT_Error )
  Load_TrueType_Glyph( PInstance  instance,
                       PGlyph     glyph,
                       UShort     glyph_index,
                       UShort     load_flags );

#ifdef __cplusplus
  }
#endif


#endif /* TTGLOAD_H */


/* END */
