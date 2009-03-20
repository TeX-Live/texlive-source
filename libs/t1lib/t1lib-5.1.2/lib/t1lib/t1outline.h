/*--------------------------------------------------------------------------
  ----- File:        t1outline.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2002-10-18
  ----- Description: This file is part of the t1-library. It contains
                     definitions and declarations for t1outline.c.
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
  
#ifdef T1OUTLINE_C

T1_OUTLINE *T1_GetCharOutline( int FontID, char charcode, float size,
			       T1_TMATRIX *transform);
T1_OUTLINE *T1_GetStringOutline( int FontID, char *string, int len, 
				 long spaceoff, int modflag, float size,
				 T1_TMATRIX *transform);
T1_OUTLINE *T1_ConcatOutlines( T1_OUTLINE *path1, T1_OUTLINE *path2);
T1_OUTLINE *T1_ScaleOutline( T1_OUTLINE *path, float scale);
T1_OUTLINE *T1_GetMoveOutline( int FontID, int deltax, int deltay, int modflag,
			       float size, T1_TMATRIX *transform);
void T1_DumpPath( T1_OUTLINE *path);
void T1_AbsolutePath( T1_OUTLINE *rpath);
void T1_RelativePath( T1_OUTLINE *apath);
void T1_ManipulatePath( T1_OUTLINE *path,
			void (*manipulate)(fractpel *x,fractpel *y,int type));
T1_OUTLINE *T1_CopyOutline( T1_OUTLINE *path);
void T1_FreeOutline( T1_OUTLINE *path);


#else

extern T1_OUTLINE *T1_GetCharOutline( int FontID, char charcode, float size,
				      T1_TMATRIX *transform);
extern T1_OUTLINE *T1_GetStringOutline( int FontID, char *string, int len, 
					long spaceoff, int modflag, float size,
					T1_TMATRIX *transform);
extern T1_OUTLINE *T1_ConcatOutlines( T1_OUTLINE *path1,
				      T1_OUTLINE *path2);
extern T1_OUTLINE *T1_ScaleOutline( T1_OUTLINE *path, float scale);
extern T1_OUTLINE *T1_GetMoveOutline( int FontID, int deltax, int deltay, int modflag,
				      float size, T1_TMATRIX *transform);
extern void T1_DumpPath( T1_OUTLINE *path);
extern void T1_AbsolutePath( T1_OUTLINE *rpath);
extern void T1_RelativePath( T1_OUTLINE *apath);
extern void T1_ManipulatePath( T1_OUTLINE *path,
			       void (*manipulate)(fractpel *x,fractpel *y,int type));
extern T1_OUTLINE *T1_CopyOutline( T1_OUTLINE *path);
extern void T1_FreeOutline( T1_OUTLINE *path);

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


