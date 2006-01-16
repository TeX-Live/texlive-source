/*******************************************************************
 *
 *  ftxcmap.h                                                   1.0
 *
 *    API extension for iterating over Cmaps
 *
 *  Copyright 1996-1999 by Juliusz Chroboczek,
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT. By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *
 ******************************************************************/

#ifndef FTXCMAP_H
#define FTXCMAP_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

  /* Find the first entry of a Cmap.  Its glyph index is returned   */
  /* in the "id" field, while the function returns the first valid  */
  /* character code in the Cmap.  It returns -1 in case of failure. */

  EXPORT_DEF
  TT_Long  TT_CharMap_First( TT_CharMap  charMap,
                             TT_UShort*  id );


  /* Find the next entry of Cmap.  Same return conventions. */

  EXPORT_DEF
  TT_Long  TT_CharMap_Next( TT_CharMap  charMap,
                            TT_UShort   startId,
                            TT_UShort*  id );

  /* Find the last entry of a Cmap.  Its glyph index is returned   */
  /* in the "id" field, while the function returns the last valid  */
  /* character code in the Cmap.  It returns -1 in case of failure. */

  EXPORT_DEF
  TT_Long  TT_CharMap_Last( TT_CharMap  charMap,
                            TT_UShort*  id );


#ifdef __cplusplus
}
#endif

#endif /* FTXCMAP_H */


/* END */
