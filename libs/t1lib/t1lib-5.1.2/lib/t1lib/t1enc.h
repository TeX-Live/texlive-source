/*--------------------------------------------------------------------------
  ----- File:        t1enc.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2001-06-16
  ----- Description: This file is part of the t1-library. It contains
                     declarations and definitions for t11enc.c.
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
		     independ from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/

#ifdef T1ENC_C

static char **ScanEncodingFile( char *FileName);
char **T1_LoadEncoding( char *FileName);
int T1_DeleteEncoding( char **encoding);
int T1_ReencodeFont( int FontID, char **Encoding);
int T1_SetDefaultEncoding( char **encoding);
char *T1_GetEncodingScheme( int FontID);
static int cmp_METRICS_ENTRY( const void *entry1, const void *entry2);

#else

extern char **T1_LoadEncoding( char *FileName);
extern int T1_DeleteEncoding( char **encoding);
extern int T1_ReencodeFont( int FontID, char **Encoding);
extern int T1_SetDefaultEncoding( char **encoding);
extern char *T1_GetEncodingScheme( int FontID);

#endif

