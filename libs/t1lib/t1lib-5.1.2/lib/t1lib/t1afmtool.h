/*--------------------------------------------------------------------------
  ----- File:        t1afmtool.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2001-04-01
  ----- Description: This file is part of the t1-library. It contains
                     declarations and definitions for t1afmtool.c.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2001. 
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
  
#ifdef T1AFMTOOL_C

FontInfo *T1_GenerateAFMFallbackInfo( int FontID);
int      T1_WriteAFMFallbackFile( int FontID);
extern struct region *fontfcnB_ByName( int FontID, int modflag,
				       struct XYspace *S,
				       char *charname,
				       int *mode, psfont *Font_Ptr,
				       int do_raster);

#else

extern FontInfo *T1_GenerateAFMFallbackInfo( int FontID);
extern int      T1_WriteAFMFallbackFile( int FontID);

#endif
