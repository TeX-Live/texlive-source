/*--------------------------------------------------------------------------
  ----- File:        t1trans.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2002-10-19
  ----- Description: This file is part of the t1-library. It contains
                     definitions and declarations fort t1trans.c
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2002. 
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
  
#ifdef T1TRANS_C

int T1_ExtendFont( int FontID, double extend);
int T1_SlantFont( int FontID, double slant);
int T1_TransformFont( int FontID, T1_TMATRIX *matrix);
double T1_GetExtend( int FontID);
double T1_GetSlant( int FontID);
T1_TMATRIX T1_GetTransform( int FontID);
int T1_SetLinePosition( int FontID, int linetype, float value);
int T1_SetLineThickness( int FontID, int linetype, float value);
float T1_GetLinePosition( int FontID, int linetype);
float T1_GetLineThickness( int FontID, int linetype);
T1_TMATRIX *T1_RotateMatrix( T1_TMATRIX *matrix, double angle);
T1_TMATRIX *T1_MirrorHMatrix( T1_TMATRIX *matrix);
T1_TMATRIX *T1_MirrorVMatrix( T1_TMATRIX *matrix);
T1_TMATRIX *T1_ShearHMatrix( T1_TMATRIX *matrix, double shear);
T1_TMATRIX *T1_ShearVMatrix( T1_TMATRIX *matrix, double shear);
T1_TMATRIX *T1_ExtendHMatrix( T1_TMATRIX *matrix, double extent);
T1_TMATRIX *T1_ExtendVMatrix( T1_TMATRIX *matrix, double extent);
T1_TMATRIX *T1_TransformMatrix( T1_TMATRIX *matrix,
				double cxx, double cyx,
				double cxy, double cyy);
int T1_StrokeFont( int FontID, int dostroke);
int T1_SetStrokeFlag( int FontID);
int T1_ClearStrokeFlag( int FontID);
int T1_GetStrokeMode( int FontID);
int T1_SetStrokeWidth( int FontID, float strokewidth);
float T1_GetStrokeWidth( int FontID);


#else


extern int T1_ExtendFont( int FontID, double extend);
extern int T1_SlantFont( int FontID, double slant);
extern int T1_TransformFont( int FontID, T1_TMATRIX *matrix);
extern double T1_GetExtend( int FontID);
extern double T1_GetSlant( int FontID);
extern T1_TMATRIX T1_GetTransform( int FontID);
extern int T1_SetLinePosition( int FontID, int linetype, float value);
extern int T1_SetLineThickness( int FontID, int linetype, float value);
extern float T1_GetLinePosition( int FontID, int linetype);
extern float T1_GetLineThickness( int FontID, int linetype);
extern T1_TMATRIX *T1_RotateMatrix( T1_TMATRIX *matrix, double angle);
extern T1_TMATRIX *T1_MirrorHMatrix( T1_TMATRIX *matrix);
extern T1_TMATRIX *T1_MirrorVMatrix( T1_TMATRIX *matrix);
extern T1_TMATRIX *T1_ShearHMatrix( T1_TMATRIX *matrix, double shear);
extern T1_TMATRIX *T1_ShearVMatrix( T1_TMATRIX *matrix, double shear);
extern T1_TMATRIX *T1_ExtendHMatrix( T1_TMATRIX *matrix, double extent);
extern T1_TMATRIX *T1_ExtendVMatrix( T1_TMATRIX *matrix, double extent);
extern T1_TMATRIX *T1_TransformMatrix( T1_TMATRIX *matrix,
				       double cxx, double cyx,
				       double cxy, double cyy);
extern int T1_StrokeFont( int FontID, int dostroke);
extern int T1_SetStrokeFlag( int FontID);
extern int T1_ClearStrokeFlag( int FontID);
extern int T1_GetStrokeMode( int FontID);
extern int T1_SetStrokeWidth( int FontID, float strokewidth);
extern float T1_GetStrokeWidth( int FontID);

#endif

