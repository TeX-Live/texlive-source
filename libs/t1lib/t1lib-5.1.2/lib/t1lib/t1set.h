/*--------------------------------------------------------------------------
  ----- File:        t1set.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2003-01-02
  ----- Description: This file is part of the t1-library. It contains
                     definitions and declarations for t1set.c.
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
  
#ifdef T1SET_C

GLYPH *T1_SetChar( int FontID, char charcode, 
		   float size, T1_TMATRIX *transform);
GLYPH *T1_SetString( int FontID, char *string, volatile int len,
		     long spaceoff, int modflag,
		     float size, T1_TMATRIX *transform);
GLYPH* T1_SetRect( int FontID, float size,
		   float width, float height,
		   T1_TMATRIX *transform);
void fill(char *dest, int h, int w,
	  struct region *area, int byte, int bit,
	  int wordsize);
void fillrun(char *p, pel x0, pel x1, int bit);
GLYPH *T1_CopyGlyph(GLYPH *glyph);
void T1_DumpGlyph( GLYPH *glyph);
GLYPH *T1_ConcatGlyphs( GLYPH *glyph1, GLYPH *glyph2,
			int x_off, int y_off, int modflag);
void T1_DumpGlyph( GLYPH *glyph);
GLYPH *T1_FillOutline( T1_OUTLINE *path, int modflag);


#else

extern GLYPH *T1_SetChar( int FontID, char charcode, 
			  float size, T1_TMATRIX *transform);
extern GLYPH *T1_SetString( int FontID, char *string, volatile int len,
			    long spaceoff, int modflag,
			    float size, T1_TMATRIX *transform);
extern GLYPH* T1_SetRect( int FontID, float size,
			  float width, float height,
			  T1_TMATRIX *transform);
extern void fill(char *dest, int h, int w,
		 struct region *area, int byte, int bit,
		 int wordsize);
extern void fillrun(char *p, pel x0, pel x1, int bit);
extern GLYPH *T1_CopyGlyph(GLYPH *glyph);
extern void T1_DumpGlyph( GLYPH *glyph);
extern GLYPH *T1_ConcatGlyphs( GLYPH *glyph1, GLYPH *glyph2,
			       int x_off, int y_off, int modflag);
extern void T1_DumpGlyph( GLYPH *glyph);
extern GLYPH *T1_FillOutline( T1_OUTLINE *path, int modflag);

#endif


extern struct region *fontfcnB(int FontID, int modflag,
			       struct XYspace *S, char **ev,
			       unsigned char index, int *mode,
			       psfont *Font_Ptr,
			       int do_raster,
			       float strokewidth);
extern struct region *fontfcnB_string( int FontID, int modflag,
				       struct XYspace *S, char **ev,
				       unsigned char *string, int no_chars,
				       int *mode, psfont *Font_Ptr,
				       int *kern_pairs, long spacewidth,
				       int do_raster,
				       float strokewidth);
extern struct region *fontfcnB_ByName( int FontID, int modflag,
				       struct XYspace *S,
				       char *charname,
				       int *mode, psfont *Font_Ptr,
				       int do_raster);
extern struct region* fontfcnRect( float width,
				   float height,
				   struct XYspace* S,
				   int* mode,
				   int do_raster,
				   float strokewidth);

