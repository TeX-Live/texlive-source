/*--------------------------------------------------------------------------
  ----- File:        t1base.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2002-05-16
  ----- Description: This file is part of the t1-library. It contains
                     declarations and definitions for t1base.c
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
		     independ from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  
#ifdef T1BASE_C

void *T1_InitLib( int log);
int intT1_scanFontDBase( char *filename);
int intT1_scanFontDBaseXLFD( char *filename);
int T1_CloseLib( void);
int T1_AddFont( char *fontfilename);
void T1_PrintLog( char *func_ident, char *msg_txt, int level, ...);
void T1_SetLogLevel( int level);
int T1_CheckForInit(void);
int T1_CheckForFontID( int FontID);
char *T1_GetFontFileName( int FontID);
int  T1_GetNoFonts(void);
int T1_SetDeviceResolutions( float x_res, float y_res);
int T1_CopyFont( int FontID);
int T1_QueryX11Support( void);
int T1_CheckEndian(void);
int T1_SetBitmapPad( int pad);
int T1_GetBitmapPad( void);
char *T1_GetLibIdent( void);
void T1_SetRasterFlags( int flags);
char *T1_GetAfmFileName( int FontID);
int T1_SetAfmFileName( int FontId, char *afm_name);
char *T1_GetFontFilePath( int FontID);
char *T1_GetAfmFilePath( int FontID);
const char *T1_StrError( int t1err);

extern int T1_Type1OperatorFlags;

#else

extern void *T1_InitLib( int log);
extern int intT1_scanFontDBase( char *filename);
extern int intT1_scanFontDBaseXLFD( char *filename);
extern int T1_CloseLib( void);
extern int T1_AddFont( char *fontfilename);
extern void T1_PrintLog( char *func_ident, char *msg_txt, int level, ...);
extern void T1_SetLogLevel( int level);
extern int T1_CheckForInit(void);
extern int T1_CheckForFontID( int FontID);
extern char *T1_GetFontFileName( int FontID);
extern int  T1_GetNoFonts(void);
extern int T1_SetDeviceResolutions( float x_res, float y_res);
extern int T1_QueryX11Support( void);
extern int T1_CopyFont( int FontID);
extern int T1_CheckEndian(void);
extern int T1_SetBitmapPad( int pad);
extern int T1_GetBitmapPad( void);
extern char *T1_GetLibIdent( void);
extern void T1_SetRasterFlags( int flags);
extern char *T1_GetAfmFileName( int FontID);
extern int T1_SetAfmFileName( int FontId, char *afm_name);
extern char *T1_GetFontFilePath( int FontID);
extern char *T1_GetAfmFilePath( int FontID);
extern const char *T1_StrError( int t1err);

#endif
