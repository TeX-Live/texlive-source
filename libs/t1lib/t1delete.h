/*--------------------------------------------------------------------------
  ----- File:        t1delete.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2001-06-03
  ----- Description: This file is part of the t1-library. It contains
                     functions for setting characters and strings of
		     characters.
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

#ifdef T1DELETE_C

int T1_DeleteSize( int FontID, float size);
int T1_DeleteAllSizes( int FontID);
int T1_FreeGlyph( GLYPH *glyph);
int T1_FreeCompCharData( T1_COMP_CHAR_INFO *cci);
int T1_DeleteFont( int FontID);
int FreeAFMData( FontInfo *pAFMData);

#else

extern int T1_DeleteSize( int FontID, float size);
extern int T1_DeleteAllSizes( int FontID);
extern int T1_FreeGlyph( GLYPH *glyph);
extern int T1_FreeCompCharData( T1_COMP_CHAR_INFO *cci);
extern int T1_DeleteFont( int FontID);
extern int FreeAFMData( FontInfo *pAFMData);

#endif


