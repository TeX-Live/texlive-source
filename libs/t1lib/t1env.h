/*--------------------------------------------------------------------------
  ----- File:        t1env.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2001-09-26
  ----- Description: This file is part of the t1-library. It contains
                     declarations and definitions for t1env.c
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


#ifdef T1ENV_C

void intT1_FreeSearchPaths( void);
void intT1_SetupDefaultSearchPaths( void);
int intT1_ScanConfigFile( void);
char *intT1_Env_GetCompletePath( char *FileName, char **env_ptr );
int T1_SetFileSearchPath( int type, char *pathname);
int T1_AddToFileSearchPath( int pathtype, int mode, char *pathname);
char *T1_GetFileSearchPath( int type);
int T1_SetFontDataBase( char *filename);
int T1_AddFontDataBase( int mode, char *filename);

#else

extern void intT1_FreeSearchPaths( void);
extern void intT1_SetupDefaultSearchPaths( void);
extern int intT1_ScanConfigFile( void);
extern char *intT1_Env_GetCompletePath( char *FileName, char **env_ptr );
extern int T1_SetFileSearchPath( int type, char *pathname);
extern int T1_AddToFileSearchPath( int pathtype, int mode, char *pathname);
extern char *T1_GetFileSearchPath( int type);
extern int T1_SetFontDataBase( char *filename);
extern int T1_AddFontDataBase( int mode, char *filename);

#endif

