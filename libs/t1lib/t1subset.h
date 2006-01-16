/*--------------------------------------------------------------------------
  ----- File:        t1subset.h
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2001-04-01
  ----- Description: This file is part of the t1-library. It contains
                     declarations and definitions for t1subset.c.
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
  

#ifdef T1SUBSET_C


#define T1_SUBSET_DEFAULT            0x00
#define T1_SUBSET_FORCE_REENCODE     0x01
#define T1_SUBSET_SKIP_REENCODE      0x02
#define T1_SUBSET_ENCRYPT_BINARY     0x04
/* This is only for debugging, it does not produce valid font-files! */
#define T1_SUBSET_ENCRYPT_NONE       0x08


/* Functions used from the t1io-module, partially written
   and added for font-subsetting. We don't care about the
   special F_FILE-type defined in that module because all
   accesses are done via pointers. */
extern unsigned long T1GetFileSize( FILE *f);
extern FILE *T1Open( char *fn, char *mode);
extern int T1Gets(char *string, int size, FILE *f);
extern int T1GetDecrypt( void);
extern int T1GetTrailer(char *string, int size, FILE *f);
extern int T1Close( FILE *f);
extern void T1io_reset( void);

char *T1_SubsetFont( int FontID,
		     char *mask,
		     unsigned int flags,
		     int linewidth,
		     unsigned long maxblocksize,
		     unsigned long *bufsize);
char *T1_GetCharString( int FontID, char *charname, int *len);
int T1_GetlenIV( int FontID);


#define ZEROS 512

#else

extern char *T1_SubsetFont( int FontID,
			    char *mask,
			    unsigned int flags,
			    int linewidth,
			    unsigned long maxblocksize,
			    unsigned long *bufsize);
extern char *T1_GetCharString( int FontID, char *charname, int *len);
extern int T1_GetlenIV( int FontID);


#endif
