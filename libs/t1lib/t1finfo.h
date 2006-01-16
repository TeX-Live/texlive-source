/*--------------------------------------------------------------------------
  ----- File:        t1finfo.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2001-06-03
  ----- Description: This file is part of the t1-library. It contains
                     declarations and definitions for t1finfo.c.
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
  

#ifdef T1FINFO_C


int T1_GetKerning( int FontID, char char1, char char2);
int T1_GetCharWidth( int FontID, char char1);
BBox T1_GetCharBBox( int FontID, char char1);
float T1_GetUnderlinePosition( int FontID);
float T1_GetUnderlineThickness( int FontID);
float T1_GetItalicAngle( int FontID);
int T1_GetIsFixedPitch( int FontID);
char *T1_GetFontName( int FontID);
char *T1_GetFullName( int FontID);
char *T1_GetFamilyName( int FontID);
char *T1_GetWeight( int FontID);
char *T1_GetVersion( int FontID);
char *T1_GetNotice( int FontID);
char *T1_GetCharName( int FontID, char char1);
int T1_QueryLigs( int FontID, char char1, char **successors, char **ligatures);
int T1_GetEncodingIndex( int FontID, char *char1);
int *T1_GetEncodingIndices( int FontID, char *char1);
int T1_GetStringWidth( int FontID, char *string,
		       int len,  long spaceoff,int kerning);
BBox T1_GetStringBBox( int FontID, char *string,
		       int len,  long spaceoff, int kerning);
METRICSINFO T1_GetMetricsInfo( int FontID, char *string,
			       int len,  long spaceoff, int kerning);
BBox T1_GetFontBBox( int FontID);
char **T1_GetAllCharNames( int FontID);
int T1_GetNoKernPairs( int FontID);
static int cmp_METRICS_ENTRY( const void *entry1, const void *entry2);
int T1_GetNoCompositeChars( int FontID);
int T1_QueryCompositeChar( int FontID, char char1);
T1_COMP_CHAR_INFO *T1_GetCompCharData( int FontID, char char1);
T1_COMP_CHAR_INFO *T1_GetCompCharDataByIndex( int FontID, int index);
int T1_IsInternalChar( int FontID, char char1);


#else


extern int T1_GetKerning( int FontID, char char1,
			  char char2);
extern int T1_GetCharWidth( int FontID, char char1);
extern BBox T1_GetCharBBox( int FontID, char char1);
extern float T1_GetUnderlinePosition( int FontID);
extern float T1_GetUnderlineThickness( int FontID);
extern float T1_GetItalicAngle( int FontID);
extern int T1_GetIsFixedPitch( int FontID);
extern char *T1_GetFontName( int FontID);
extern char *T1_GetFullName( int FontID);
extern char *T1_GetFamilyName( int FontID);
extern char *T1_GetWeight( int FontID);
extern char *T1_GetVersion( int FontID);
extern char *T1_GetNotice( int FontID);
extern char *T1_GetCharName( int FontID, char char1);
extern int T1_QueryLigs( int FontID, char char1, char **successors, char **ligatures);
extern int T1_GetEncodingIndex( int FontID, char *char1);
extern int *T1_GetEncodingIndices( int FontID, char *char1);
extern int T1_GetStringWidth( int FontID, char *string,
			      int len,  long spaceoff,int kerning);
extern BBox T1_GetStringBBox( int FontID, char *string,
			      int len,  long spaceoff, int kerning);
extern METRICSINFO T1_GetMetricsInfo( int FontID, char *string,
				      int len,  long spaceoff, int kerning);
extern BBox T1_GetFontBBox( int FontID);
extern int T1_GetNoKernPairs( int FontID);
extern char **T1_GetAllCharNames( int FontID);
extern int T1_GetNoCompositeChars( int FontID);
extern int T1_QueryCompositeChar( int FontID, char char1);
extern T1_COMP_CHAR_INFO *T1_GetCompCharData( int FontID, char char1);
extern T1_COMP_CHAR_INFO *T1_GetCompCharDataByIndex( int FontID, int index);
extern int T1_IsInternalChar( int FontID, char char1);


#endif

