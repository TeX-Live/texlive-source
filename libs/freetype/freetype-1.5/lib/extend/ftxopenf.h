/*******************************************************************
 *
 *  ftxopenf.h
 *
 *    internal TrueType Open functions
 *
 *  Copyright 1996-2001 by
 *  David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 *  This file is part of the FreeType project, and may only be used
 *  modified and distributed under the terms of the FreeType project
 *  license, LICENSE.TXT.  By continuing to use, modify, or distribute
 *  this file you indicate that you have read the license and
 *  understand and accept it fully.
 *
 ******************************************************************/

#ifndef FTXOPENF_H
#define FTXOPENF_H

#include "ftxopen.h"

#ifdef __cplusplus
extern "C" {
#endif

  /* functions from ftxopen.c */

  TT_Error  Load_ScriptList( TTO_ScriptList*  sl,
                             PFace            input );
  TT_Error  Load_FeatureList( TTO_FeatureList*  fl,
                              PFace             input );
  TT_Error  Load_LookupList( TTO_LookupList*  ll,
                             PFace            input,
                             TTO_Type         type );

  TT_Error  Load_Coverage( TTO_Coverage*  c,
                           PFace          input );
  TT_Error  Load_ClassDefinition( TTO_ClassDefinition*  cd,
                                  UShort                limit,
                                  PFace                 input );
  TT_Error  Load_Device( TTO_Device*  d,
                         PFace        input );

  void  Free_ScriptList( TTO_ScriptList*  sl );
  void  Free_FeatureList( TTO_FeatureList*  fl );
  void  Free_LookupList( TTO_LookupList*  ll,
                         TTO_Type         type );

  void  Free_Coverage( TTO_Coverage*  c );
  void  Free_ClassDefinition( TTO_ClassDefinition*  cd );
  void  Free_Device( TTO_Device*  d );


  /* functions from ftxgsub.c */

  TT_Error  Load_SingleSubst( TTO_SingleSubst*  ss,
                              PFace             input );
  TT_Error  Load_MultipleSubst( TTO_MultipleSubst*  ms,
                                PFace               input );
  TT_Error  Load_AlternateSubst( TTO_AlternateSubst*  as,
                                 PFace                input );
  TT_Error  Load_LigatureSubst( TTO_LigatureSubst*  ls,
                                PFace               input );
  TT_Error  Load_ContextSubst( TTO_ContextSubst*  cs,
                               PFace              input );
  TT_Error  Load_ChainContextSubst( TTO_ChainContextSubst*  ccs,
                                    PFace                   input );

  void  Free_SingleSubst( TTO_SingleSubst*  ss );
  void  Free_MultipleSubst( TTO_MultipleSubst*  ms );
  void  Free_AlternateSubst( TTO_AlternateSubst*  as );
  void  Free_LigatureSubst( TTO_LigatureSubst*  ls );
  void  Free_ContextSubst( TTO_ContextSubst*  cs );
  void  Free_ChainContextSubst( TTO_ChainContextSubst*  ccs );


  /* functions from ftxgpos.c */

  TT_Error  Load_SinglePos( TTO_SinglePos*  sp,
                            PFace           input );
  TT_Error  Load_PairPos( TTO_PairPos*  pp,
                          PFace         input );
  TT_Error  Load_CursivePos( TTO_CursivePos*  cp,
                             PFace            input );
  TT_Error  Load_MarkBasePos( TTO_MarkBasePos*  mbp,
                              PFace             input );
  TT_Error  Load_MarkLigPos( TTO_MarkLigPos*  mlp,
                             PFace            input );
  TT_Error  Load_MarkMarkPos( TTO_MarkMarkPos*  mmp,
                              PFace             input );
  TT_Error  Load_ContextPos( TTO_ContextPos*  cp,
                             PFace            input );
  TT_Error  Load_ChainContextPos( TTO_ChainContextPos*  ccp,
                                  PFace                 input );

  void  Free_SinglePos( TTO_SinglePos*  sp );
  void  Free_PairPos( TTO_PairPos*  pp );
  void  Free_CursivePos( TTO_CursivePos*  cp );
  void  Free_MarkBasePos( TTO_MarkBasePos*  mbp );
  void  Free_MarkLigPos( TTO_MarkLigPos*  mlp );
  void  Free_MarkMarkPos( TTO_MarkMarkPos*  mmp );
  void  Free_ContextPos( TTO_ContextPos*  cp );
  void  Free_ChainContextPos( TTO_ChainContextPos*  ccp );


  /* query functions */

  TT_Error  Coverage_Index( TTO_Coverage*  c,
                            UShort         glyphID,
                            UShort*        index );
  TT_Error  Get_Class( TTO_ClassDefinition*  cd,
                       UShort                glyphID,
                       UShort*               class,
                       UShort*               index );
  TT_Error  Get_Device( TTO_Device*  d,
                        UShort       size,
                        Short*       value );


  /* functions from ftxgdef.c */

  TT_Error  Add_Glyph_Property( TTO_GDEFHeader*  gdef,
                                UShort           glyphID,
                                UShort           property );

  TT_Error  Check_Property( TTO_GDEFHeader*  gdef,
                            UShort           index,
                            UShort           flags,
                            UShort*          property );

#define CHECK_Property( gdef, index, flags, property )              \
          ( ( error = Check_Property( (gdef), (index), (flags),     \
                                      (property) ) ) != TT_Err_Ok )

#ifdef __cplusplus
}
#endif

#endif /* FTXOPENF_H */


/* END */
