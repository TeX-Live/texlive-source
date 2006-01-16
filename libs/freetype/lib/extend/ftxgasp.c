/*******************************************************************
 *
 *  ftxgasp.c                                                   1.0
 *
 *    Gasp table support API extension body
 *
 *  Copyright 1996-2001 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT. By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 *
 *  The gasp table is currently loaded by the core engine, but the
 *  standard API doesn't give access to it.  This file is used to
 *  demonstrate the use of a simple API extension.
 *
 ******************************************************************/

#include "ftxgasp.h"

#include "tttypes.h"
#include "ttobjs.h"
#include "tttables.h"


  FT_EXPORT_FUNC( TT_Error )
  TT_Get_Face_Gasp_Flags( TT_Face    face,
                          TT_UShort  point_size,
                          TT_Bool*   grid_fit,
                          TT_Bool*   smooth_font )
  {
    PFace   faze = HANDLE_Face( face );
    UShort  i, flag;


    if ( !faze )
      return TT_Err_Invalid_Face_Handle;

    if ( faze->gasp.numRanges == 0 || !faze->gasp.gaspRanges )
      return TT_Err_Table_Missing;

    for ( i = 0; i < faze->gasp.numRanges; i++ )
    {
      if ( point_size <= faze->gasp.gaspRanges[i].maxPPEM )
      {
        flag = faze->gasp.gaspRanges[i].gaspFlag;

        *grid_fit    = ( (flag & GASP_GRIDFIT) != 0 );
        *smooth_font = ( (flag & GASP_DOGRAY ) != 0 );

        return TT_Err_Ok;
      }
    }

    /* for very large fonts we enable font smoothing and discard */
    /* grid fitting                                              */

    *grid_fit    = 0;
    *smooth_font = 1;

    return TT_Err_Ok;
  }


/* END */
