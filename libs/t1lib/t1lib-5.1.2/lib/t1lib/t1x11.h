/*--------------------------------------------------------------------------
  ----- File:        t1x11.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2003-01-02
  ----- Description: This file is part of the t1-library. It contains
                     definitions and declarations for t1x11.c.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2003. 
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

#include <X11/Xlib.h>

#ifdef T1X11_C

int T1_SetX11Params( Display *display,
		     Visual *visual,
		     unsigned int depth,
		     Colormap colormap);
GLYPH *T1_SetCharX( Drawable d, GC gc, int mode, int x, int y,
		    int FontID, char charcode,
		    float size, T1_TMATRIX *transform);
GLYPH *T1_SetStringX( Drawable d, GC gc, int mode, int x, int y,
		      int FontID, char *string, int len,
		      long spaceoff, int modflag,
		      float size, T1_TMATRIX *transform);
GLYPH *T1_SetRectX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
		    int FontID, float size,
		    float width, float height,
		    T1_TMATRIX *transform);
GLYPH *T1_AASetCharX( Drawable d, GC gc, int mode, int x, int y,
		      int FontID, char charcode,
		      float size, T1_TMATRIX *transform);
GLYPH *T1_AASetStringX( Drawable d, GC gc, int mode, int x, int y,
			int FontID, char *string, int len,
			long spaceoff, int modflag,
			float size, T1_TMATRIX *transform);
GLYPH *T1_AASetRectX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
		      int FontID, float size,
		      float width, float height,
		      T1_TMATRIX *transform);
int T1_ComputeAAColorsX( unsigned long fg,
			 unsigned long bg,
			 int nolevels);
int T1_GetDepthOfDrawable( Drawable drawable);
void T1_LogicalPositionX( int pos_switch);


#else

extern Display *T1_display;

extern int T1_SetX11Params( Display *display,
			    Visual *visual,
			    unsigned int depth,
			    Colormap colormap);
extern GLYPH *T1_SetCharX( Drawable d, GC gc, int mode, int x, int y,
			   int FontID, char charcode,
			   float size, T1_TMATRIX *transform);
extern GLYPH *T1_SetStringX( Drawable d, GC gc, int mode, int x, int y,
			     int FontID, char *string, int len,
			     long spaceoff, int modflag,
			     float size, T1_TMATRIX *transform);
extern GLYPH *T1_SetRectX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
			   int FontID, float size,
			   float width, float height,
			   T1_TMATRIX *transform);
extern GLYPH *T1_AASetCharX( Drawable d, GC gc, int mode, int x, int y,
			     int FontID, char charcode,
			     float size, T1_TMATRIX *transform);
extern GLYPH *T1_AASetStringX( Drawable d, GC gc, int mode, int x, int y,
			       int FontID, char *string, int len,
			       long spaceoff, int modflag,
			       float size, T1_TMATRIX *transform);
extern GLYPH *T1_AASetRectX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
			     int FontID, float size,
			     float width, float height,
			     T1_TMATRIX *transform);
extern int T1_ComputeAAColorsX( unsigned long fg,
				unsigned long bg,
				int nolevels);
extern int T1_GetDepthOfDrawable( Drawable drawable);
extern void T1_LogicalPositionX( int pos_switch);


#endif

