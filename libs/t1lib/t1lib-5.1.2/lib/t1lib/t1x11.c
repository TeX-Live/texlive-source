/*--------------------------------------------------------------------------
  ----- File:        t1x11.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-01
  ----- Description: This file is part of the t1-library. It contains
                     functions for generating glyphs with data in
		     X11-Pixmap format.
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
		     independent from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  
#define T1X11_C


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../type1/types.h"
#include "parseAFM.h" 
#include "../type1/objects.h" 
#include "../type1/spaces.h"  
#include "../type1/util.h" 
#include "../type1/fontfcn.h"
#include "../type1/paths.h"
#include "../type1/regions.h"

#include "t1types.h"
#include "t1extern.h"
#include "t1set.h"
#include "t1aaset.h"
#include "t1load.h"
#include "t1finfo.h"
#include "t1misc.h"
#include "t1x11.h"
#include "t1base.h"



#define T1GCMASK GCForeground | GCBackground 

/* As a fall back */
#ifndef T1_AA_TYPE16 
#define T1_AA_TYPE16    short
#endif
#ifndef T1_AA_TYPE32 
#define T1_AA_TYPE32    int
#endif



/* The three important X11 parameters t1lib has to deal with: */
static Display  *T1_display=NULL;  /* Must be accessible from t1delete.c */
static Visual   *T1_visual=NULL;
static Colormap T1_colormap;
static unsigned int T1_depth=0;
static int T1_byte_order;
static int lastlevel=0;
static unsigned long oldfg_n=0, oldbg_n=0, oldfg_l=0, oldbg_l=0;
static unsigned long oldfg_h=0, oldbg_h=0, oldfg=0, oldbg=0;


extern int T1aa_SmartOn;        /* from t1aaset.c */
extern float T1aa_smartlimit1;
extern float T1aa_smartlimit2;



static XColor aacolors[AAMAXPLANES];
static unsigned long aapixels[AAMAXPLANES];


/* The following parameter determines whether t1lib will use logical
   positioning of chars and string (place the origin at specified
   destination-point) or absolute positioning with respect to the
   origin (upper left corner) of the generated bitmap/pixmap. */
static int T1_lposition=1;



/* T1_SetX11Params(): Set X11-parameters which t1lib has to know in order
   to properly generate pixmaps from characters */
int T1_SetX11Params( Display *display,
		     Visual *visual,
		     unsigned int depth,
		     Colormap colormap)
{

  T1_display  =display;
  T1_visual   =visual;
  T1_depth    =depth;
  T1_colormap =colormap;

  if (T1_CheckEndian()==0)
    T1_byte_order=0;
  else
    T1_byte_order=1;

  /* For bug hunting: */
  if (ImageByteOrder( T1_display)==0)
    T1_PrintLog( "T1_SetX11Params()",
		 "X-Server uses Little Endian data representation",
		 T1LOG_DEBUG);
  else
    T1_PrintLog( "T1_SetX11Params()",
		 "X-Server uses Big Endian data representation",
		 T1LOG_DEBUG);
  
  return(0);
}



/* T1_SetCharX(): Generate an object of type GLYPH, i.e, a glyph with
   a pixmap ID instead of a pointer to a bitmap: */

GLYPH *T1_SetCharX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
		    int FontID, char charcode,
		    float size, T1_TMATRIX *transform)
{
  GLYPH  *pglyph;
  static GLYPH xglyph={NULL,{0,0,0,0,0,0},NULL,0};
 
  int height, width;
  Pixmap clipmask=0;
  int opaque;
  

  xglyph.metrics.leftSideBearing=0;
  xglyph.metrics.rightSideBearing=0;
  xglyph.metrics.advanceX=0;
  xglyph.metrics.advanceY=0;
  xglyph.metrics.ascent=0;
  xglyph.metrics.descent=0;
  xglyph.pFontCacheInfo=NULL;
  

  opaque=mode;
  
  
  if ((pglyph=T1_SetChar( FontID, charcode, size, transform))==NULL){
    T1_PrintLog( "T1_SetCharX()",
		 "T1_SetChar() returned NULL-pointer!",
		 T1LOG_WARNING);
    return(NULL);
  }

  /* Check for empty bitmap */
  if (pglyph->bits==NULL) {
    xglyph=*pglyph;
    return( &xglyph);
  }
  
  width=pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing;
  height=pglyph->metrics.ascent-pglyph->metrics.descent;
  
  
  clipmask=XCreateBitmapFromData( T1_display,
				  d,
				  (char *)pglyph->bits,
				  PAD(width, pFontBase->bitmap_pad),  
				  height
				  );
				  
  /* Correct position */
  if (T1_lposition){
    x_dest += pglyph->metrics.leftSideBearing;
    y_dest -= pglyph->metrics.ascent;
  }

  if (opaque==0){
    XSetClipMask(T1_display, gc, clipmask);
    XSetClipOrigin(T1_display, gc, x_dest, y_dest);
  }
  
  XCopyPlane( T1_display, clipmask, d, gc, 0, 0,
	      width, height, x_dest, y_dest, 0x01);
  
  if (clipmask){
    XFreePixmap( T1_display, clipmask);
    clipmask=0;
    XSetClipMask(T1_display, gc, None);
    XSetClipOrigin(T1_display, gc, 0, 0);
  }
  
  pglyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
  xglyph.metrics.leftSideBearing=pglyph->metrics.leftSideBearing;
  xglyph.metrics.rightSideBearing=pglyph->metrics.rightSideBearing;
  xglyph.metrics.advanceX=pglyph->metrics.advanceX;
  xglyph.metrics.advanceY=pglyph->metrics.advanceY;
  xglyph.metrics.ascent=pglyph->metrics.ascent;
  xglyph.metrics.descent=pglyph->metrics.descent;
  xglyph.bpp=pglyph->bpp;
  
  return( &xglyph);
  
  
}



/* T1_SetStringX(...): Draw a string of characters into an X11 drawable */
GLYPH *T1_SetStringX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
		      int FontID, char *string, int len, 
		      long spaceoff, int modflag, float size,
		      T1_TMATRIX *transform)
{
  GLYPH  *pglyph;
  static GLYPH xglyph={NULL,{0,0,0,0,0,0},NULL,0};
 
  int height, width;
  Pixmap clipmask=0;
  int opaque;
  

  xglyph.metrics.leftSideBearing=0;
  xglyph.metrics.rightSideBearing=0;
  xglyph.metrics.advanceX=0;
  xglyph.metrics.advanceY=0;
  xglyph.metrics.ascent=0;
  xglyph.metrics.descent=0;
  xglyph.pFontCacheInfo=NULL;
  

  opaque=mode;
  

  if ((pglyph=T1_SetString( FontID, string, len, 
			    spaceoff, modflag, size,
			    transform))==NULL){
    T1_PrintLog( "T1_SetStringX()",
		 "T1_SetString() returned NULL-pointer!",
		 T1LOG_WARNING);
    return(NULL);
  }

  /* Check for empty bitmap */
  if (pglyph->bits==NULL) {
    xglyph=*pglyph;
    return( &xglyph);
  }

  width=pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing;
  height=pglyph->metrics.ascent-pglyph->metrics.descent;
  
  clipmask=XCreateBitmapFromData( T1_display,
				  d,
				  (char *)pglyph->bits,
				  PAD(width, pFontBase->bitmap_pad), /* width */
				  height
				  );
  
  /* Correct position */
  if (T1_lposition){
    x_dest += pglyph->metrics.leftSideBearing;
    y_dest -= pglyph->metrics.ascent;
  }

  if (opaque==0){
    XSetClipMask(T1_display, gc, clipmask);
    XSetClipOrigin(T1_display, gc, x_dest, y_dest);
  }
  
  XCopyPlane( T1_display, clipmask, d, gc, 0, 0,
	      width, height, x_dest, y_dest, 0x01);
  
  if (clipmask){
    XFreePixmap( T1_display, clipmask);
    clipmask=0;
    XSetClipMask(T1_display, gc, None);
    XSetClipOrigin(T1_display, gc, 0, 0);
  }
  
  pglyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
  xglyph.metrics.leftSideBearing=pglyph->metrics.leftSideBearing;
  xglyph.metrics.rightSideBearing=pglyph->metrics.rightSideBearing;
  xglyph.metrics.advanceX=pglyph->metrics.advanceX;
  xglyph.metrics.advanceY=pglyph->metrics.advanceY;
  xglyph.metrics.ascent=pglyph->metrics.ascent;
  xglyph.metrics.descent=pglyph->metrics.descent;
  xglyph.bpp=pglyph->bpp;
  
  return( &xglyph);
  
  
}


/* T1_SetRectX(): Draw a rectangle into an x11 drawable */

GLYPH *T1_SetRectX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
		    int FontID, float size,
		    float rwidth, float rheight,
		    T1_TMATRIX *transform)
{
  GLYPH  *pglyph;
  static GLYPH xglyph={NULL,{0,0,0,0,0,0},NULL,0};
 
  int height, width;
  Pixmap clipmask=0;
  int opaque;
  

  xglyph.metrics.leftSideBearing=0;
  xglyph.metrics.rightSideBearing=0;
  xglyph.metrics.advanceX=0;
  xglyph.metrics.advanceY=0;
  xglyph.metrics.ascent=0;
  xglyph.metrics.descent=0;
  xglyph.pFontCacheInfo=NULL;
  

  opaque=mode;
  
  
  if ((pglyph=T1_SetRect( FontID, size, rwidth, rheight, transform))==NULL){
    T1_PrintLog( "T1_SetRectrX()",
		 "T1_SetRect() returned NULL-pointer!",
		 T1LOG_WARNING);
    return(NULL);
  }

  /* Check for empty bitmap */
  if (pglyph->bits==NULL) {
    xglyph=*pglyph;
    return( &xglyph);
  }
  
  width=pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing;
  height=pglyph->metrics.ascent-pglyph->metrics.descent;
  
  
  clipmask=XCreateBitmapFromData( T1_display,
				  d,
				  (char *)pglyph->bits,
				  PAD(width, pFontBase->bitmap_pad),  
				  height
				  );
				  
  /* Correct position */
  if (T1_lposition){
    x_dest += pglyph->metrics.leftSideBearing;
    y_dest -= pglyph->metrics.ascent;
  }

  if (opaque==0){
    XSetClipMask(T1_display, gc, clipmask);
    XSetClipOrigin(T1_display, gc, x_dest, y_dest);
  }
  
  XCopyPlane( T1_display, clipmask, d, gc, 0, 0,
	      width, height, x_dest, y_dest, 0x01);
  
  if (clipmask){
    XFreePixmap( T1_display, clipmask);
    clipmask=0;
    XSetClipMask(T1_display, gc, None);
    XSetClipOrigin(T1_display, gc, 0, 0);
  }
  
  pglyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
  xglyph.metrics.leftSideBearing=pglyph->metrics.leftSideBearing;
  xglyph.metrics.rightSideBearing=pglyph->metrics.rightSideBearing;
  xglyph.metrics.advanceX=pglyph->metrics.advanceX;
  xglyph.metrics.advanceY=pglyph->metrics.advanceY;
  xglyph.metrics.ascent=pglyph->metrics.ascent;
  xglyph.metrics.descent=pglyph->metrics.descent;
  xglyph.bpp=pglyph->bpp;
  
  return( &xglyph);
  
  
}



/* T1_AASetCharX(): Generate an object of type GLYPH, i.e, a glyph with
   a pixmap ID instead of a pointer to a bitmap: */
GLYPH *T1_AASetCharX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
		      int FontID, char charcode,
		      float size, T1_TMATRIX *transform)
{
  int j, k;

  GLYPH  *pglyph;
  XImage *ximage;

  static GLYPH xglyph={NULL,{0,0,0,0,0,0},NULL,0};
 
  int height, width, width_pad;
  
  XGCValues xgcvalues;
  static unsigned long fg, bg;

  Pixmap clipmask=0;
  int clipmask_h, clipmask_v, line_off;
  char *clipmask_ptr;
  
  int opaque;
  

	       
  xglyph.metrics.leftSideBearing=0;
  xglyph.metrics.rightSideBearing=0;
  xglyph.metrics.advanceX=0;
  xglyph.metrics.advanceY=0;
  xglyph.metrics.ascent=0;
  xglyph.metrics.descent=0;
  xglyph.pFontCacheInfo=NULL;
  

  opaque=mode;
  
  xglyph.bpp=T1_depth;
  
  /* In order to be able to contruct the pixmap we need to know
     foreground and background color as well the copy function */
  XGetGCValues( T1_display, gc, T1GCMASK, &xgcvalues);
  fg=xgcvalues.foreground;
  bg=xgcvalues.background;
  

  /* At this point we must compute the colors that are needed to do
     antialiasing between fore- and background. The following function
     fills the static aacolors */
  if (T1aa_SmartOn==0)
    j=T1_AAGetLevel();
  else if (size>=T1aa_smartlimit2)
    j=1;
  else if (size>=T1aa_smartlimit1)
    j=2;
  else
    j=4;
  if ( j!=lastlevel || fg!=oldfg || bg!=oldbg ){
    switch ( j){
    case 1:
      if ( fg!=oldfg_n || bg!=oldbg_n){
	oldfg_n=fg;
	oldbg_n=bg;
	/* computing colors is not necessary here */
	T1_AANSetGrayValues( bg, fg);
      }
      break;
    case 2:
      if ( fg!=oldfg_l || bg!=oldbg_l){
	T1_ComputeAAColorsX( fg, bg, AAMAXPLANES);
	/*~derekn*/
	/* If fg=bg, the clipmask will be messed up; in this case  */
	/* we can arbitrarily change bg to get a correct clipmask. */
	if (opaque == 0 && fg == bg)
	  aapixels[0] = bg = (fg > 0) ? fg - 1 : fg + 1;
	oldfg_l=fg;
	oldbg_l=bg;
	T1_AASetGrayValues(aapixels[0],   /* white */
			   aapixels[4],
			   aapixels[8],
			   aapixels[12],
			   aapixels[16] ); /* black */
      }
      break;
    case 4:
      if ( fg!=oldfg_h || bg!=oldbg_h){
	T1_ComputeAAColorsX( fg, bg, AAMAXPLANES);
	/*~derekn*/
	/* If fg=bg, the clipmask will be messed up; in this case  */
	/* we can arbitrarily change bg to get a correct clipmask. */
	if (opaque == 0 && fg == bg)
	  aapixels[0] = bg = (fg > 0) ? fg - 1 : fg + 1;
	oldfg_h=fg;
	oldbg_h=bg;
	T1_AAHSetGrayValues( aapixels); 
      }
      break;
    }
    lastlevel=j;
    oldfg=fg;
    oldbg=bg;
  }

  if ((pglyph=T1_AASetChar( FontID, charcode, size,
			    transform))==NULL){
    T1_PrintLog( "T1_AASetCharX()",
		 "T1_AASetChar() returned NULL-pointer!",
		 T1LOG_WARNING);
    return(NULL);
  }

  /* Check for empty bitmap */
  if (pglyph->bits==NULL) {
    xglyph=*pglyph;
    return( &xglyph);
  }

  width=pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing;
  height=pglyph->metrics.ascent-pglyph->metrics.descent;
  
	       
  /* Correct position */
  if (T1_lposition){
    x_dest += pglyph->metrics.leftSideBearing;
    y_dest -= pglyph->metrics.ascent;
  }

  if (opaque==0){
    clipmask_v=height;
    clipmask_h=width;
    width_pad=PAD(width*T1aa_bpp, pFontBase->bitmap_pad)/T1aa_bpp;
    clipmask_ptr=(char *)calloc((PAD(clipmask_h, 8)>>3) * clipmask_v, sizeof( char)); 
    if (clipmask_ptr==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
    /* Note: We pad the clipmask always to byte boundary */
    if (pglyph->bpp==8)
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((char *)(pglyph->bits))[k*width_pad+j]!=bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    else if (pglyph->bpp==16)
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((T1_AA_TYPE16 *)(pglyph->bits))[k*width_pad+j]!=(T1_AA_TYPE16)bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    else 
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((T1_AA_TYPE32 *)(pglyph->bits))[k*width_pad+j]!=(T1_AA_TYPE32)bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    
    clipmask=XCreateBitmapFromData( T1_display,
				    d,
				    (char *)clipmask_ptr,
				    width,
				    height
				    );
    free( clipmask_ptr);
    XSetClipMask(T1_display, gc, clipmask);
    XSetClipOrigin(T1_display, gc, x_dest, y_dest);

  }
  ximage=XCreateImage( T1_display,
		       T1_visual,
		       T1_depth, 
		       ZPixmap, /* XYBitmap or XYPixmap */
		       0, /* No offset */
		       (char *)pglyph->bits,
		       width,
		       height,
		       pFontBase->bitmap_pad,  
		       0 /*PAD(width,8)/8*/  /* number of bytes per line */
		       );
  ximage->byte_order=T1_byte_order;
  XPutImage(T1_display,
	    d,
	    gc,
	    ximage,
	    0,  
	    0,  
	    x_dest,  
	    y_dest,  
	    width,
	    height
	    );
  XDestroyImage(ximage);
  if (clipmask){
    XFreePixmap( T1_display, clipmask);
    clipmask=0;
    XSetClipMask(T1_display, gc, None);
    XSetClipOrigin(T1_display, gc, 0, 0);
  }
  
  pglyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
  xglyph.metrics.leftSideBearing=pglyph->metrics.leftSideBearing;
  xglyph.metrics.rightSideBearing=pglyph->metrics.rightSideBearing;
  xglyph.metrics.advanceX=pglyph->metrics.advanceX;
  xglyph.metrics.advanceY=pglyph->metrics.advanceY;
  xglyph.metrics.ascent=pglyph->metrics.ascent;
  xglyph.metrics.descent=pglyph->metrics.descent;
  xglyph.bpp=pglyph->bpp;
  
  return( &xglyph);
  
  
}



/* T1_AASetStringX(...): Draw a string of characters into an X11 drawable */
GLYPH *T1_AASetStringX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
			int FontID, char *string, int len, 
			long spaceoff, int modflag, float size,
			T1_TMATRIX *transform)
{
  int  j, k;

  GLYPH  *pglyph;
  XImage *ximage;

  
  static GLYPH xglyph={NULL,{0,0,0,0,0,0},NULL,0};

  int height, width, width_pad;
  
  XGCValues xgcvalues;
  static unsigned long fg, bg;
  

  Pixmap clipmask=0;
  int clipmask_h, clipmask_v, line_off;
  char *clipmask_ptr;
  
  int opaque;
  

  xglyph.metrics.leftSideBearing=0;
  xglyph.metrics.rightSideBearing=0;
  xglyph.metrics.advanceX=0;
  xglyph.metrics.advanceY=0;
  xglyph.metrics.ascent=0;
  xglyph.metrics.descent=0;
  xglyph.pFontCacheInfo=NULL;
  

  opaque=mode;
  
  
  /* In order to be able to contruct the pixmap we need to know
     foreground and background color as well the copy function */
  XGetGCValues( T1_display, gc, T1GCMASK, &xgcvalues);
  fg=xgcvalues.foreground;
  bg=xgcvalues.background;
  
  xglyph.bpp=T1_depth;
  
  /* At this point we must compute the colors that are needed to do
     antialiasing between fore- and background. The following function
     fills the static aacolors */
  if (T1aa_SmartOn==0)
    j=T1_AAGetLevel();
  else if (size>=T1aa_smartlimit2)
    j=1;
  else if (size>=T1aa_smartlimit1)
    j=2;
  else
    j=4;
  if ( j!=lastlevel || fg!=oldfg || bg!=oldbg ){
    switch ( j){
    case 1:
      if ( fg!=oldfg_n || bg!=oldbg_n){
	oldfg_n=fg;
	oldbg_n=bg;
	/* computing colors is not necessary here */
	T1_AANSetGrayValues( bg, fg);
      }
      break;
    case 2:
      if ( fg!=oldfg_l || bg!=oldbg_l){
	T1_ComputeAAColorsX( fg, bg, AAMAXPLANES);
	/*~derekn*/
	/* If fg=bg, the clipmask will be messed up; in this case  */
	/* we can arbitrarily change bg to get a correct clipmask. */
	if (opaque == 0 && fg == bg)
	  aapixels[0] = bg = (fg > 0) ? fg - 1 : fg + 1;
	oldfg_l=fg;
	oldbg_l=bg;
	T1_AASetGrayValues(aapixels[0],   /* white */
			   aapixels[4],
			   aapixels[8],
			   aapixels[12],
			   aapixels[16] ); /* black */
      }
      break;
    case 4:
      if ( fg!=oldfg_h || bg!=oldbg_h){
	T1_ComputeAAColorsX( fg, bg, AAMAXPLANES);
	/*~derekn*/
	/* If fg=bg, the clipmask will be messed up; in this case  */
	/* we can arbitrarily change bg to get a correct clipmask. */
	if (opaque == 0 && fg == bg)
	  aapixels[0] = bg = (fg > 0) ? fg - 1 : fg + 1;
	oldfg_h=fg;
	oldbg_h=bg;
	T1_AAHSetGrayValues( aapixels); 
      }
      break;
    }
    lastlevel=j;
    oldfg=fg;
    oldbg=bg;
  }

  
  if ((pglyph=T1_AASetString( FontID, string, len, 
			      spaceoff, modflag, size,
			      transform))==NULL){
    T1_PrintLog( "T1_AASetStringX()",
		 "T1_AASetString() returned NULL-pointer!",
		 T1LOG_WARNING);
    return(NULL);
  }

  /* Check for empty bitmap */
  if (pglyph->bits==NULL) {
    xglyph=*pglyph;
    return( &xglyph);
  }

  width=pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing;
  height=pglyph->metrics.ascent-pglyph->metrics.descent;
  
  
  /* Correct position */
  if (T1_lposition){
    x_dest += pglyph->metrics.leftSideBearing;
    y_dest -= pglyph->metrics.ascent;
  }

  if (opaque==0){
    clipmask_v=height;
    clipmask_h=width;
    width_pad=PAD(width*T1aa_bpp, pFontBase->bitmap_pad)/T1aa_bpp;
    clipmask_ptr=(char *)calloc((PAD(clipmask_h, 8)>>3) * clipmask_v, sizeof( char));
    if (clipmask_ptr==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
    /* Note: We pad the clipmask always to byte boundary */
    if (pglyph->bpp==8)
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((char *)(pglyph->bits))[k*width_pad+j]!=bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    else if (pglyph->bpp==16)
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((T1_AA_TYPE16 *)(pglyph->bits))[k*width_pad+j]!=(T1_AA_TYPE16)bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    else 
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((T1_AA_TYPE32 *)(pglyph->bits))[k*width_pad+j]!=(T1_AA_TYPE32)bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    
    clipmask=XCreateBitmapFromData( T1_display,
				    d,
				    (char *)clipmask_ptr,
				    width,
				    height
				    );
    free( clipmask_ptr);
    XSetClipMask(T1_display, gc, clipmask);
    XSetClipOrigin(T1_display, gc, x_dest, y_dest);

  }
  ximage=XCreateImage( T1_display,
		       T1_visual,
		       T1_depth, 
		       ZPixmap, /* XYBitmap or XYPixmap */
		       0, /* No offset */
		       (char *)pglyph->bits,
		       width,
		       height,
		       pFontBase->bitmap_pad,  /* lines padded to bytes */
		       0 /*PAD(width,8)/8*/  /* number of bytes per line */
		       );
  ximage->byte_order=T1_byte_order;
  XPutImage(T1_display,
	    d,
	    gc,
	    ximage,
	    0,  
	    0,  
	    x_dest,  
	    y_dest,  
	    width,
	    height
	    );
  XDestroyImage(ximage);
  if (clipmask){
    XFreePixmap( T1_display, clipmask);
    clipmask=0;
    XSetClipMask(T1_display, gc, None);
    XSetClipOrigin(T1_display, gc, 0, 0);
  }
  
  pglyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
  xglyph.metrics.leftSideBearing=pglyph->metrics.leftSideBearing;
  xglyph.metrics.rightSideBearing=pglyph->metrics.rightSideBearing;
  xglyph.metrics.advanceX=pglyph->metrics.advanceX;
  xglyph.metrics.advanceY=pglyph->metrics.advanceY;
  xglyph.metrics.ascent=pglyph->metrics.ascent;
  xglyph.metrics.descent=pglyph->metrics.descent;
  xglyph.bpp=pglyph->bpp;
  
  return( &xglyph);
  
  
}



/* T1_AASetCharX(): Draw a rectangle into an x11 drawable */
GLYPH *T1_AASetRectX( Drawable d, GC gc, int mode, int x_dest, int y_dest,
		      int FontID, float size,
		      float rwidth, float rheight,
		      T1_TMATRIX *transform)
{
  int j, k;

  GLYPH  *pglyph;
  XImage *ximage;

  static GLYPH xglyph={NULL,{0,0,0,0,0,0},NULL,0};
 
  int height, width, width_pad;
  
  XGCValues xgcvalues;
  static unsigned long fg, bg;

  Pixmap clipmask=0;
  int clipmask_h, clipmask_v, line_off;
  char *clipmask_ptr;
  
  int opaque;
  

	       
  xglyph.metrics.leftSideBearing=0;
  xglyph.metrics.rightSideBearing=0;
  xglyph.metrics.advanceX=0;
  xglyph.metrics.advanceY=0;
  xglyph.metrics.ascent=0;
  xglyph.metrics.descent=0;
  xglyph.pFontCacheInfo=NULL;
  

  opaque=mode;
  
  xglyph.bpp=T1_depth;
  
  /* In order to be able to contruct the pixmap we need to know
     foreground and background color as well the copy function */
  XGetGCValues( T1_display, gc, T1GCMASK, &xgcvalues);
  fg=xgcvalues.foreground;
  bg=xgcvalues.background;
  

  /* At this point we must compute the colors that are needed to do
     antialiasing between fore- and background. The following function
     fills the static aacolors */
  if (T1aa_SmartOn==0)
    j=T1_AAGetLevel();
  else if (size>=T1aa_smartlimit2)
    j=1;
  else if (size>=T1aa_smartlimit1)
    j=2;
  else
    j=4;
  if ( j!=lastlevel || fg!=oldfg || bg!=oldbg ){
    switch ( j){
    case 1:
      if ( fg!=oldfg_n || bg!=oldbg_n){
	oldfg_n=fg;
	oldbg_n=bg;
	/* computing colors is not necessary here */
	T1_AANSetGrayValues( bg, fg);
      }
      break;
    case 2:
      if ( fg!=oldfg_l || bg!=oldbg_l){
	T1_ComputeAAColorsX( fg, bg, AAMAXPLANES);
	/*~derekn*/
	/* If fg=bg, the clipmask will be messed up; in this case  */
	/* we can arbitrarily change bg to get a correct clipmask. */
	if (opaque == 0 && fg == bg)
	  aapixels[0] = bg = (fg > 0) ? fg - 1 : fg + 1;
	oldfg_l=fg;
	oldbg_l=bg;
	T1_AASetGrayValues(aapixels[0],   /* white */
			   aapixels[4],
			   aapixels[8],
			   aapixels[12],
			   aapixels[16] ); /* black */
      }
      break;
    case 4:
      if ( fg!=oldfg_h || bg!=oldbg_h){
	T1_ComputeAAColorsX( fg, bg, AAMAXPLANES);
	/*~derekn*/
	/* If fg=bg, the clipmask will be messed up; in this case  */
	/* we can arbitrarily change bg to get a correct clipmask. */
	if (opaque == 0 && fg == bg)
	  aapixels[0] = bg = (fg > 0) ? fg - 1 : fg + 1;
	oldfg_h=fg;
	oldbg_h=bg;
	T1_AAHSetGrayValues( aapixels); 
      }
      break;
    }
    lastlevel=j;
    oldfg=fg;
    oldbg=bg;
  }

  if ((pglyph=T1_AASetRect( FontID, size, rwidth, rheight, transform))==NULL){
    T1_PrintLog( "T1_AASetRectX()",
		 "T1_AASetRect() returned NULL-pointer!",
		 T1LOG_WARNING);
    return(NULL);
  }

  /* Check for empty bitmap */
  if (pglyph->bits==NULL) {
    xglyph=*pglyph;
    return( &xglyph);
  }

  width=pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing;
  height=pglyph->metrics.ascent-pglyph->metrics.descent;
  
	       
  /* Correct position */
  if (T1_lposition){
    x_dest += pglyph->metrics.leftSideBearing;
    y_dest -= pglyph->metrics.ascent;
  }

  if (opaque==0){
    clipmask_v=height;
    clipmask_h=width;
    width_pad=PAD(width*T1aa_bpp, pFontBase->bitmap_pad)/T1aa_bpp;
    clipmask_ptr=(char *)calloc((PAD(clipmask_h, 8)>>3) * clipmask_v, sizeof( char)); 
    if (clipmask_ptr==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
    /* Note: We pad the clipmask always to byte boundary */
    if (pglyph->bpp==8)
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((char *)(pglyph->bits))[k*width_pad+j]!=bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    else if (pglyph->bpp==16)
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((T1_AA_TYPE16 *)(pglyph->bits))[k*width_pad+j]!=(T1_AA_TYPE16)bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    else 
      for ( k=0; k<clipmask_v; k++){
	line_off=k*(PAD(clipmask_h, 8)>>3);
	for (j=0; j<clipmask_h; j++){
	  if (((T1_AA_TYPE32 *)(pglyph->bits))[k*width_pad+j]!=(T1_AA_TYPE32)bg)
	    clipmask_ptr[line_off+(j>>3)] |= (0x01<<(j%8));
	}
      }
    
    clipmask=XCreateBitmapFromData( T1_display,
				    d,
				    (char *)clipmask_ptr,
				    width,
				    height
				    );
    free( clipmask_ptr);
    XSetClipMask(T1_display, gc, clipmask);
    XSetClipOrigin(T1_display, gc, x_dest, y_dest);

  }
  ximage=XCreateImage( T1_display,
		       T1_visual,
		       T1_depth, 
		       ZPixmap, /* XYBitmap or XYPixmap */
		       0, /* No offset */
		       (char *)pglyph->bits,
		       width,
		       height,
		       pFontBase->bitmap_pad,  
		       0 /*PAD(width,8)/8*/  /* number of bytes per line */
		       );
  ximage->byte_order=T1_byte_order;
  XPutImage(T1_display,
	    d,
	    gc,
	    ximage,
	    0,  
	    0,  
	    x_dest,  
	    y_dest,  
	    width,
	    height
	    );
  XDestroyImage(ximage);
  if (clipmask){
    XFreePixmap( T1_display, clipmask);
    clipmask=0;
    XSetClipMask(T1_display, gc, None);
    XSetClipOrigin(T1_display, gc, 0, 0);
  }
  
  pglyph->bits=NULL;    /* Since XDestroyImage() free's this also! */
  xglyph.metrics.leftSideBearing=pglyph->metrics.leftSideBearing;
  xglyph.metrics.rightSideBearing=pglyph->metrics.rightSideBearing;
  xglyph.metrics.advanceX=pglyph->metrics.advanceX;
  xglyph.metrics.advanceY=pglyph->metrics.advanceY;
  xglyph.metrics.ascent=pglyph->metrics.ascent;
  xglyph.metrics.descent=pglyph->metrics.descent;
  xglyph.bpp=pglyph->bpp;
  
  return( &xglyph);
  
  
}



/* T1_ComputeAAColorsX(): Compute the antialiasing colors in dependency
   of foreground and background */
int T1_ComputeAAColorsX( unsigned long fg, unsigned long bg, int nolevels)
{

  static unsigned long last_fg;
  static unsigned long last_bg;
  long delta_red, delta_green, delta_blue;
  int i;
  int nocolors=0;
  
  
  aacolors[0].pixel=bg;
  aacolors[nolevels-1].pixel=fg;

  if ((fg==last_fg)&&(bg==last_bg))
    return(nocolors);
  
  /* Get RGB values for fore- and background */
  XQueryColor( T1_display, T1_colormap, &aacolors[0]);
  XQueryColor( T1_display, T1_colormap, &aacolors[nolevels-1]);
  delta_red   = (aacolors[nolevels-1].red - aacolors[0].red)/(nolevels-1);
  delta_green = (aacolors[nolevels-1].green - aacolors[0].green)/(nolevels-1);
  delta_blue  = (aacolors[nolevels-1].blue - aacolors[0].blue)/(nolevels-1);
  aapixels[0]=aacolors[0].pixel;
  aapixels[nolevels-1]=aacolors[nolevels-1].pixel;

  for (i=1; i<nolevels-1; i++){
    aacolors[i].red   = aacolors[0].red + i*delta_red;
    aacolors[i].green = aacolors[0].green + i*delta_green;
    aacolors[i].blue  = aacolors[0].blue + i*delta_blue;
    aacolors[i].flags = DoRed | DoGreen | DoBlue;
    aacolors[i].pad   = aacolors[0].pad;
    /* Allocate color in current palette */
    if (XAllocColor( T1_display, T1_colormap, &aacolors[i])!=0){
      aapixels[i]=aacolors[i].pixel;
      nocolors++;
    }
  }
  
  return(nocolors);

}



/* Set the positioning switch */
void T1_LogicalPositionX( int pos_switch)
{
  if (pos_switch)
    T1_lposition=1;
  else
    T1_lposition=0;
  return;
}



XImage *T1_XImageFromGlyph( GLYPH *pglyph)
{
  XImage *ximage;
  
  /* Check for empty bitmap */
  if (pglyph->bits==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return( NULL);
  }

  if (pglyph->bpp==1) { /* we have a bitmap glyph */
    ximage=XCreateImage( T1_display,
			 T1_visual,
			 1, 
			 XYBitmap, /* XYBitmap or XYPixmap */
			 0, /* No offset */
			 (char *)pglyph->bits,
			 pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing,
			 pglyph->metrics.ascent-pglyph->metrics.descent,
			 pFontBase->bitmap_pad,  
			 0  /* number of bytes per line */
			 );
  }
  else { /* we have an anztialiased glyph */
    ximage=XCreateImage( T1_display,
			 T1_visual,
			 T1_depth, 
			 ZPixmap, /* XYBitmap or XYPixmap */
			 0, /* No offset */
			 (char *)pglyph->bits,
			 pglyph->metrics.rightSideBearing-pglyph->metrics.leftSideBearing,
			 pglyph->metrics.ascent-pglyph->metrics.descent,
			 pFontBase->bitmap_pad,  
			 0  /* number of bytes per line */
			 );
  }
  
  if (ximage==NULL) {
    T1_errno=T1ERR_X11;
    return( NULL);
  }
  ximage->byte_order=T1_byte_order; /* Set t1lib´s byteorder */

  return( ximage);

}

