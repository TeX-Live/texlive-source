/*--------------------------------------------------------------------------
  ----- File:        t1load.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2002-12-02
  ----- Description: This file is part of the t1-library. It contains
                     declarations and definitions for t1load.c.
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
		     independ from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  

#ifdef T1LOAD_C

int T1_LoadFont( int FontID);
static int openFontMetricsFile( int FontID, int open_sloppy);
FONTSIZEDEPS *T1int_CreateNewFontSize( int FontID, float size, int aa);
FONTSIZEDEPS *T1int_GetLastFontSize( int FontID);
FONTSIZEDEPS *T1int_QueryFontSize( int FontID, float size, int aa);
int fontfcnA( char *env, int *mode, psfont *Font_Ptr);
static int cmp_METRICS_ENTRY( const void *entry1, const void *entry2);
extern char *vm_base; /* from fontfcn.c in initfont()! */
extern char *vm_used; /* from fontfcn.c in fontfcnA()! */

#else

extern int T1_LoadFont( int FontID);
extern FONTSIZEDEPS *T1int_CreateNewFontSize( int FontID, float size, int aa);
extern FONTSIZEDEPS *T1int_GetLastFontSize( int FontID);
extern FONTSIZEDEPS *T1int_QueryFontSize( int FontID, float size, int aa);

#endif
