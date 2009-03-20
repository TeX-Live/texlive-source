/*--------------------------------------------------------------------------
  ----- File:        t1trans.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-01
  ----- Description: This file is part of the t1-library. It contains
                     functions for transforming fonts and setting
		     line-parameters.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2005.
                     As of version 0.5, t1lib is distributed under the
		     GNU General Public Library Lincense. The
		     conditions can be found in the files LICENSE and
		     LGPL, which should reside in the toplevel
		     directory of the distribution.  Please note that 
		     there are parts of t1lib that are subject to
		     other licenses:
		     The parseAFM-package is copyrighted by Adobe Systems
		     Inc.
		     The type1 rasterizer is copyrighted by IBM and the
		     X11-consortium.
  ----- Warranties:  Of course, there's NO WARRANTY OF ANY KIND :-)
  ----- Credits:     I want to thank IBM and the X11-consortium for making
                     their rasterizer freely available.
		     Also thanks to Piet Tutelaers for his ps2pk, from
		     which I took the rasterizer sources in a format
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  
#define T1TRANS_C


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined(_MSC_VER)
# include <io.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <unistd.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../type1/ffilest.h"
#include "../type1/types.h"
#include "parseAFM.h" 
#include "../type1/objects.h"
#include "../type1/spaces.h"
#include "../type1/util.h"
#include "../type1/fontfcn.h"
#include "../type1/paths.h"
#include "../type1/regions.h"


#include "t1types.h"
#include "t1extern.h"
#include "t1trans.h"
#include "t1base.h"


/* T1_ExtendFont(): Extend the font referenced by FontID by the factor
   extend. This is only allowed if no size dependent data exists.
   Of course, the font must already have been loaded.
   Returns 0 for success and -1 otherwise.
   */
int T1_ExtendFont( int FontID, double extend)
{
  
  /* First, check for font residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }
  
  /* Second, check whether size-dependent data exists: */
  if (pFontBase->pFontArray[FontID].pFontSizeDeps != NULL){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1); 
  }
  
  pFontBase->pFontArray[FontID].extend=extend;
  pFontBase->pFontArray[FontID].FontTransform[0] = extend;
  return(0);
}



/* T1_GetExtend(): Return the current extension factor of the
   font FontID
   Return: 0.0                    if font not loaded
           current extent         otherwise
*/
double T1_GetExtend( int FontID)
{
  /* First, check for font residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }

  return( pFontBase->pFontArray[FontID].extend);
}




/* T1_SlantFont(): Slant the font referenced by FontID by the factor
   extend. This is only allowed if no size dependent data exists.
   Of course, the font must already have been loaded.
   Returns 0 for success and -1 otherwise.
   */
int T1_SlantFont( int FontID, double slant)
{
  
  /* First, check for font residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }
  
  /* Second, check whether size-dependent data exists: */
  if (pFontBase->pFontArray[FontID].pFontSizeDeps != NULL){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1); 
  }
  
  pFontBase->pFontArray[FontID].slant=slant;
  pFontBase->pFontArray[FontID].FontTransform[2] = slant;
  return(0);
}



/* T1_GetSlant(): Return the current slanting factor of the
   font FontID
   Return: 0.0                    if font not loaded
           current slant          otherwise (may also be 0.0!)
*/
double T1_GetSlant( int FontID)
{
  /* First, check for font residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }

  return( pFontBase->pFontArray[FontID].slant);
}



/* T1_TransformFont(): Transform the font referenced by FontID according
   to the transform matrix.  This is only allowed if no size dependent
   data exists.  Of course, the font must already have been loaded.
   Returns 0 for success and -1 otherwise.
   */
int T1_TransformFont( int FontID, T1_TMATRIX *matrix)
{
  
  /* First, check for font residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }
  
  /* Second, check whether size-dependent data exists: */
  if (pFontBase->pFontArray[FontID].pFontSizeDeps != NULL){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1); 
  }
  
  pFontBase->pFontArray[FontID].FontTransform[0] = matrix->cxx;
  pFontBase->pFontArray[FontID].FontTransform[1] = matrix->cxy;
  pFontBase->pFontArray[FontID].FontTransform[2] = matrix->cyx;
  pFontBase->pFontArray[FontID].FontTransform[3] = matrix->cyy;
  return(0);
}



/* T1_GetTransform(): Return the current transformation matrix for the
   font FontID
   Return: [0.0, 0.0, 0.0, 0.0]    if font not loaded
           current tmatrix         otherwise 
*/
T1_TMATRIX T1_GetTransform( int FontID)
{
  T1_TMATRIX tmatrix={0.0, 0.0, 0.0, 0.0};
  
  /* First, check for font residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(tmatrix);
  }

  tmatrix.cxx=pFontBase->pFontArray[FontID].FontTransform[0];
  tmatrix.cxy=pFontBase->pFontArray[FontID].FontTransform[1];
  tmatrix.cyx=pFontBase->pFontArray[FontID].FontTransform[2];
  tmatrix.cyy=pFontBase->pFontArray[FontID].FontTransform[3];
  
  return( tmatrix);
}



/* Functions for setting line-parameters:
   linetypeis expected to be an OR'ed combination of
   T1_UNDERLINE, T1_OVERLINE and T1_OVERSTRIKE.
 */
int T1_SetLinePosition( int FontID, int linetype, float value)
{

  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }

  if (linetype & T1_UNDERLINE){
    pFontBase->pFontArray[FontID].UndrLnPos=value;
    return( 0);
  }
  if (linetype & T1_OVERLINE){
    pFontBase->pFontArray[FontID].OvrLnPos=value;
    return( 0);
  }
  if (linetype & T1_OVERSTRIKE){
    pFontBase->pFontArray[FontID].OvrStrkPos=value;
    return( 0);
  }
    
  /* The linetype was bad */
  T1_errno=T1ERR_INVALID_PARAMETER;
  return( -1);
  
}



int T1_SetLineThickness( int FontID, int linetype, float value)
{

  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }

  if (linetype & T1_UNDERLINE){
    pFontBase->pFontArray[FontID].UndrLnThick=value;
    return( 0);
  }
  if (linetype & T1_OVERLINE){
    pFontBase->pFontArray[FontID].OvrLnThick=value;
    return( 0);
  }
  if (linetype & T1_OVERSTRIKE){
    pFontBase->pFontArray[FontID].OvrStrkThick=value;
    return( 0);
  }
    
  /* The linetype was bad */
  T1_errno=T1ERR_INVALID_PARAMETER;
  return( -1);
  
}


float T1_GetLinePosition( int FontID, int linetype)
{

  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }

  if (linetype & T1_UNDERLINE)
    return( pFontBase->pFontArray[FontID].UndrLnPos);
  if (linetype & T1_OVERLINE)
    return( pFontBase->pFontArray[FontID].OvrLnPos);
  if (linetype & T1_OVERSTRIKE)
    return( pFontBase->pFontArray[FontID].OvrStrkPos);
  
  /* The linetype was bad */
  T1_errno=T1ERR_INVALID_PARAMETER;
  return( 0.0);
  
}



float T1_GetLineThickness( int FontID, int linetype)
{

  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }

  if (linetype & T1_UNDERLINE)
    return( pFontBase->pFontArray[FontID].UndrLnThick);
  if (linetype & T1_OVERLINE)
    return( pFontBase->pFontArray[FontID].OvrLnThick);
  if (linetype & T1_OVERSTRIKE)
    return( pFontBase->pFontArray[FontID].OvrStrkThick);
  
  /* The linetype was bad */
  T1_errno=T1ERR_INVALID_PARAMETER;
  return( 0.0);

}


/* Functions for intuitively transforming matrices. All function operate on their
   original objects. In case NULL is specified, a unity-matrix is allocated by
   the function and then tranformed appropriately. Note that in order to concatenate
   transformation in the sense of t1lib the current transformation matrix must be
   left-multiplied by the trnasformation to be applied! */
/* Rotation:
            ( x11'  x21' )   ( cos(a)    -sin(a) )   ( x11  x21 ) 
	    (            ) = (                   ) * (          ) 
            ( x12'  x22' )   ( sin(a)     cos(a) )   ( x12  x22 ) 
*/
T1_TMATRIX *T1_RotateMatrix( T1_TMATRIX *matrix, double angle)
{
  T1_TMATRIX tmat;
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  memcpy( &tmat, matrix, sizeof(T1_TMATRIX));
  /* Convert angle to radians: */
  angle=angle*PI/180.0;
  /* multiply matrices */
  matrix->cxx=cos((double)angle) * tmat.cxx - sin((double)angle) * tmat.cxy;
  matrix->cyx=cos((double)angle) * tmat.cyx - sin((double)angle) * tmat.cyy;
  matrix->cxy=sin((double)angle) * tmat.cxx + cos((double)angle) * tmat.cxy;
  matrix->cyy=sin((double)angle) * tmat.cyx + cos((double)angle) * tmat.cyy;
  return( matrix);
}


/* Horizontal mirroring */
/* H-Mirror:
            ( x11'  x21' )   ( -1    0 )   ( x11  x21 )   ( -x11   -x21 ) 
	    (            ) = (         ) * (          ) = (             ) 
            ( x12'  x22' )   (  0    1 )   ( x12  x22 )   (  x12    x22 ) 
*/
T1_TMATRIX *T1_MirrorHMatrix( T1_TMATRIX *matrix)
{
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  matrix->cxx *=-1.0;
  matrix->cyx *=-1.0;
  return( matrix);
}


/* Vertical mirroring */
/* V-Mirror:
            ( x11'  x21' )   ( 1    0 )   ( x11  x21 )   (  x11    x21 )
	    (            ) = (        ) * (          ) = (             )
            ( x12'  x22' )   ( 0   -1 )   ( x12  x22 )   ( -x12   -x22 )
*/
T1_TMATRIX *T1_MirrorVMatrix( T1_TMATRIX *matrix)
{
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  matrix->cxy *=-1.0;
  matrix->cyy *=-1.0;
  return( matrix);
  
}


/* Horizontal shearing */
/* H-Shearing:
            ( x11'  x21' )   ( 1    f )   ( x11  x21 )   ( x11+f*x12   x21+f*x22 ) 
	    (            ) = (        ) * (          ) = (                       ) 
            ( x12'  x22' )   ( 0    1 )   ( x12  x22 )   ( x12         x22       ) 
*/
T1_TMATRIX *T1_ShearHMatrix( T1_TMATRIX *matrix, double shear)
{
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  matrix->cxx +=shear*matrix->cxy;
  matrix->cyx +=shear*matrix->cyy;
  return( matrix);
  
}


/* Vertical shearing */
/* V-Shearing:
            ( x11'  x21' )   ( 1    0 )   ( x11  x21 )   ( x11         x21       ) 
	    (            ) = (        ) * (          ) = (                       ) 
            ( x12'  x22' )   ( f    1 )   ( x12  x22 )   ( x12+f*x11   x22+f*x21 ) 
*/
T1_TMATRIX *T1_ShearVMatrix( T1_TMATRIX *matrix, double shear)
{
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  matrix->cxy +=shear*matrix->cxx;
  matrix->cyy +=shear*matrix->cyx;
  return( matrix);
  
}


/* Horizontal extension */
/* Horizontal-Extension:
            ( x11'  x21' )   ( f    0 )   ( x11  x21 )   ( f*x11   f*x21 ) 
	    (            ) = (        ) * (          ) = (               ) 
            ( x12'  x22' )   ( 0    1 )   ( x12  x22 )   ( x12     x22   ) 
*/
T1_TMATRIX *T1_ExtendHMatrix( T1_TMATRIX *matrix, double extent)
{
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  matrix->cxx *=extent;
  matrix->cyx *=extent;
  return( matrix);
  
}


/* Vertical extension */
/* Vertical-Extension:
            ( x11'  x21' )   ( 1    0 )   ( x11  x21 )   ( x11    x21   ) 
	    (            ) = (        ) * (          ) = (              ) 
            ( x12'  x22' )   ( 0    f )   ( x12  x22 )   ( f*x12  f*x22 ) 
*/
T1_TMATRIX *T1_ExtendVMatrix( T1_TMATRIX *matrix, double extent)
{
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  matrix->cxy *=extent;
  matrix->cyy *=extent;
  return( matrix);
  
}


/* General transformation */
/*  
    ( x11   x21 )   ( y11   y21 )   ( x11*y11+x21*y12   x11*y21+x21*y22 )
    (           ) * (           ) = (                                   )
    ( x12   x22 )   ( y12   y22 )   ( x12*y11+x22*y12   x12*y21+x22*y22 )
*/
T1_TMATRIX *T1_TransformMatrix( T1_TMATRIX *matrix,
				double cxx, double cyx,
				double cxy, double cyy)
{
  T1_TMATRIX tmat;
  
  if (matrix==NULL){
    if ((matrix=(T1_TMATRIX *)malloc( sizeof(T1_TMATRIX)))==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return( NULL);
    }
    matrix->cxx=1.0;
    matrix->cyx=0.0;
    matrix->cxy=0.0;
    matrix->cyy=1.0;
  }
  memcpy( &tmat, matrix, sizeof(T1_TMATRIX));
  matrix->cxx=cxx * tmat.cxx + cyx * tmat.cxy;
  matrix->cyx=cxx * tmat.cyx + cyx * tmat.cyy;
  matrix->cxy=cxy * tmat.cxx + cyy * tmat.cxy;
  matrix->cyy=cxy * tmat.cyx + cyy * tmat.cyy;
  
  return( matrix);
}



/* T1_StrokeFont(): Switch the  font referenced by FontID to stroking
   or filling. The stroked character will be cached and 
   filled characters are no longer cached and vice versa.
   This is only allowed if no size dependent data exists.
   Of course, the font must already have been loaded.
   Returns 0 for success and -1 otherwise.
   */
int T1_StrokeFont( int FontID, int dostroke)
{
  
  /* First, check for font residing in memory: */
  if ( T1_CheckForFontID( FontID) != 1 ) {
    T1_errno = T1ERR_INVALID_FONTID;
    return -1;
  }
  
  /* Second, check whether size-dependent data exists: */
  if ( pFontBase->pFontArray[FontID].pFontSizeDeps != NULL ) {
    T1_errno = T1ERR_OP_NOT_PERMITTED;
    return -1; 
  }

  if ( dostroke != 0 ) {
    pFontBase->pFontArray[FontID].info_flags |= RASTER_STROKED;
    pFontBase->pFontArray[FontID].info_flags |= CACHE_STROKED;
  }
  else {
    pFontBase->pFontArray[FontID].info_flags &= ~RASTER_STROKED;
    pFontBase->pFontArray[FontID].info_flags &= ~CACHE_STROKED;
  }
  

  return 0;
}



/* T1_SetStrokeFlag(): Return the stroke flag for font FontID.
   Return:  0                      flag has been set
           -1                      flag could not be set 
*/
int T1_SetStrokeFlag( int FontID)
{
  /* First, check for font residing in memory: */
  if ( T1_CheckForFontID(FontID) != 1 ) {
    T1_errno = T1ERR_INVALID_FONTID;
    return -1;
  }
  
  /* Set stroke flag to true */
  pFontBase->pFontArray[FontID].info_flags |= RASTER_STROKED;
  
  return 0;
  
}



/* T1_ClearStrokeFlag(): Reset the stroke flag for font FontID.
   Return:  0                      flag has been reset
           -1                      flag could not be reset 
*/
int T1_ClearStrokeFlag( int FontID)
{
  /* First, check for font residing in memory: */
  if ( T1_CheckForFontID(FontID) != 1 ) {
    T1_errno = T1ERR_INVALID_FONTID;
    return -1;
  }

  /* Reset stroke flag */
  pFontBase->pFontArray[FontID].info_flags &= ~RASTER_STROKED;
  
  return 0;
  
}



/* T1_GetStrokeMode(): Return the stroke flag for font FontID.
   Return: -1                      if font is not loaded.
	    0                      if flag is reset,
	    1                      if stroking is enabled for this font,
	    2                      if stroked characters are cached,
	    3                      if stroking is enabled and stroked
	                           characters are cached.
*/
int T1_GetStrokeMode( int FontID)
{
  int outval = 0;
  
  /* First, check for font residing in memory: */
  if ( T1_CheckForFontID( FontID) != 1 ) {
    T1_errno = T1ERR_INVALID_FONTID;
    return -1;
  }

  if ( (pFontBase->pFontArray[FontID].info_flags & CACHE_STROKED) != 0 ) 
    outval |= 0x02;
  
  if ( (pFontBase->pFontArray[FontID].info_flags & RASTER_STROKED) != 0 ) 
    outval |= 0x01;
  
  return outval;
    
}



/* T1_SetStrokeWidth(): Set the penwidth used when stroking font FontID.
   Return  -1           If width could not be set.
            0           if width has been set.
 */
int T1_SetStrokeWidth( int FontID, float strokewidth)
{
  /* First, check for font residing in memory: */
  if ( T1_CheckForFontID( FontID) != 1 ) {
    T1_errno = T1ERR_INVALID_FONTID;
    return -1;
  }

  /* Second, check whether caching stroked characters is enabled
     for this font and glyph data is already existing. In this case
     the operation is forbidden, unless the previous non-zero value
     is just restored! */
  if ( ((pFontBase->pFontArray[FontID].info_flags & CACHE_STROKED) != 0) &&
       (pFontBase->pFontArray[FontID].pFontSizeDeps != NULL) &&
       (pFontBase->pFontArray[FontID].SavedStrokeWidth != strokewidth)
       ) {
    T1_errno = T1ERR_OP_NOT_PERMITTED;
    return -1; 
  }

  /* OK, accept stroke width after ensuring a numerically meaningful
     value */
  if ( strokewidth < 0.0f ) {
    T1_errno = T1ERR_INVALID_PARAMETER;
    return -1;
  }

  pFontBase->pFontArray[FontID].StrokeWidth = strokewidth;
  
  if ( strokewidth != 0.0f )
    pFontBase->pFontArray[FontID].SavedStrokeWidth = strokewidth;
  
  return 0;
    
}



/* T1_GetStrokeWidth(): Get the penwidth used when stroking font FontID.
   If 0.0 is returned, it might also indicate that the font is not loaded.
*/
float T1_GetStrokeWidth( int FontID)
{
  /* First, check for font residing in memory: */
  if ( T1_CheckForFontID( FontID) != 1 ) {
    T1_errno = T1ERR_INVALID_FONTID;
    return 0.0f;
  }

  return pFontBase->pFontArray[FontID].StrokeWidth;  
}

