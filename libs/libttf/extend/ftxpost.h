/*******************************************************************
 *
 *  ftxpost.h
 *
 *    post table support API extension
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
 *  The post table is not completely loaded by the core engine.  This
 *  file loads the missing PS glyph names and implements an API to
 *  access them.
 *
 ******************************************************************/

#ifndef FTXPOST_H
#define FTXPOST_H

#include "freetype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TT_Err_Invalid_Post_Table_Format  0x0B00
#define TT_Err_Invalid_Post_Table         0x0B01

  /* the 258 standard Mac glyph names, used for format 1.0 and 2.5 */

  extern TT_String*  TT_Post_Default_Names[];


  /* format 2.0 table */

  struct TT_Post_20_
  {
    TT_UShort   numGlyphs;
    TT_UShort*  glyphNameIndex;
    TT_Char**   glyphNames;
  };

  typedef struct TT_Post_20_  TT_Post_20;

  struct TT_Post_25_
  {
    TT_UShort  numGlyphs;
    TT_Char*   offset;
  };

  typedef struct TT_Post_25_  TT_Post_25;

#if 0
  /* format 4.0 table -- not implemented yet */

  struct TT_Post_40_
  {
  };

  typedef struct TT_Post_40_  TT_Post_40;
#endif


  struct TT_Post_
  {
    TT_Long  offset;
    TT_Long  length;
    TT_Bool  loaded;

    union
    {
      TT_Post_20  post20;
      TT_Post_25  post25;
#if 0
      TT_Post_40  post40;
#endif
    } p;
  };

  typedef struct TT_Post_  TT_Post;


  EXPORT_DEF
  TT_Error TT_Init_Post_Extension( TT_Engine  engine );

  EXPORT_DEF
  TT_Error TT_Load_PS_Names( TT_Face   face,
                             TT_Post*  post );
  EXPORT_DEF
  TT_Error TT_Get_PS_Name( TT_Face      face,
                           TT_UShort    index,
                           TT_String**  PSname );

#ifdef __cplusplus
}
#endif

#endif /* FTXPOST_H */


/* END */
