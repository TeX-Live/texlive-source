/*--------------------------------------------------------------------------
  ----- File:        t1aaset.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
                     Subsampling based on code by Raph Levien (raph@acm.org)
  ----- Date:        2007-12-21
  ----- Description: This file is part of the t1-library. It contains
                     functions for antialiased setting of characters
		     and strings of characters.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2007.
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
  
#define T1AASET_C


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if defined(_MSC_VER)
# include <io.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <unistd.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../type1/ffilest.h"
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
#include "t1aaset.h"
#include "t1set.h"
#include "t1load.h"
#include "t1finfo.h"
#include "t1misc.h"
#include "t1base.h"
#include "t1outline.h"


#define DEFAULTBPP 8


/* As a fall back */
#ifndef T1_AA_TYPE16 
#define T1_AA_TYPE16    short
#endif
#ifndef T1_AA_TYPE32 
#define T1_AA_TYPE32    int
#endif


/* In the following arrays take the gray values. Entry 0 is associated
   with the white (background) value and the max entry is the
   black (foreground) value. */
static unsigned T1_AA_TYPE32 gv[5]={0,0,0,0,0};
static unsigned T1_AA_TYPE32 gv_h[17]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned T1_AA_TYPE32 gv_n[2]={0,0};

static int T1aa_level=T1_AA_LOW;  /* The default value */
static T1_AA_TYPE32 T1aa_lut[625];
static int T1aa_count[256];
static T1_AA_TYPE32 T1aa_h_lut[289];
static int T1aa_h_count[256];
static T1_AA_TYPE32 T1aa_n_lut[64];

/* This global is for querying the current bg from other parts
   of t1lib */
unsigned T1_AA_TYPE32 T1aa_bg=0;

/* The limit values for smart antialiasing */
float T1aa_smartlimit1=T1_AA_SMARTLIMIT1;
float T1aa_smartlimit2=T1_AA_SMARTLIMIT2;
int   T1aa_SmartOn=0;     /* We do not enable smart AA by default */

/* T1_AAInit: This function must be called whenever the T1aa_gray_val
   or T1aa_bpp variables change, or the level changes. */
static int T1_AAInit ( int level )
{
  int i;
  int i0, i1, i2, i3;
  int movelow=0, movehigh=0, indlow=0, indhigh=0;

  /* Note: movelow, movehigh, indlow and indhigh take care for proper
     byte swapping in dependence of endianess for level=4 */
  if (level==T1_AA_NONE){
    if (T1aa_bpp==8){
      if (pFontBase->endian){
	movelow=3;
	movehigh=2;
      }
      else{
	movelow=0;
	movehigh=1;
      }
    }
    else if (T1aa_bpp==16){
      if (pFontBase->endian){
	movelow=1;
	movehigh=0;
      }
      else{
	movelow=0;
	movehigh=1;
      }
    }
  }

  if (level==T1_AA_HIGH){
    
    if (T1aa_bpp==8){
      if (pFontBase->endian){
	indlow=17;
	indhigh=1;
	movelow=3;
	movehigh=2;
      }
      else{
	indlow=1;
	indhigh=17;
	movelow=0;
	movehigh=1;
      }
    }
    else if (T1aa_bpp==16){
      if (pFontBase->endian){
	indlow=17;
	indhigh=1;
	movelow=1;
	movehigh=0;
      }
      else{
	indlow=1;
	indhigh=17;
	movelow=0;
	movehigh=1;
      }
    }
    else if (T1aa_bpp==32){
      indlow=1;
      indhigh=17;
    }
    for (i = 0; i < 256; i++) {
      T1aa_h_count[i] = 0;
      if (i & 0x80) T1aa_h_count[i] += indhigh; 
      if (i & 0x40) T1aa_h_count[i] += indhigh;
      if (i & 0x20) T1aa_h_count[i] += indhigh;
      if (i & 0x10) T1aa_h_count[i] += indhigh;
      if (i & 0x08) T1aa_h_count[i] += indlow;
      if (i & 0x04) T1aa_h_count[i] += indlow;
      if (i & 0x02) T1aa_h_count[i] += indlow;
      if (i & 0x01) T1aa_h_count[i] += indlow;
    }
  }
  
  if (level == 2 && T1aa_bpp == 8) {
    for (i0 = 0; i0 < 5; i0++)
      for (i1 = 0; i1 < 5; i1++)
	for (i2 = 0; i2 < 5; i2++)
	  for (i3 = 0; i3 < 5; i3++) {
	    ((char *)T1aa_lut)[(((i0 * 5 + i1) * 5 + i2) * 5 + i3) * 4] = gv[i3];
	    ((char *)T1aa_lut)[(((i0 * 5 + i1) * 5 + i2) * 5 + i3) * 4 + 1] = gv[i2];
	    ((char *)T1aa_lut)[(((i0 * 5 + i1) * 5 + i2) * 5 + i3) * 4 + 2] = gv[i1];
	    ((char *)T1aa_lut)[(((i0 * 5 + i1) * 5 + i2) * 5 + i3) * 4 + 3] = gv[i0];
	  }
    for (i = 0; i < 256; i++) {
      T1aa_count[i] = 0;
      if (i & 0x80) T1aa_count[i] += 125;
      if (i & 0x40) T1aa_count[i] += 125;
      if (i & 0x20) T1aa_count[i] += 25;
      if (i & 0x10) T1aa_count[i] += 25;
      if (i & 0x08) T1aa_count[i] += 5;
      if (i & 0x04) T1aa_count[i] += 5;
      if (i & 0x02) T1aa_count[i] += 1;
      if (i & 0x01) T1aa_count[i] += 1;
    }
    return(0);
  } else if (level == 2 && T1aa_bpp == 16) {
    for (i0 = 0; i0 < 5; i0++)
      for (i1 = 0; i1 < 5; i1++) {
	((T1_AA_TYPE16 *)T1aa_lut)[(i0 * 5 + i1) * 2] = gv[i1];
	((T1_AA_TYPE16 *)T1aa_lut)[(i0 * 5 + i1) * 2 + 1] = gv[i0];
      }
    for (i = 0; i < 256; i++) {
      T1aa_count[i] = 0;
      if (i & 0x80) T1aa_count[i] += 160;
      if (i & 0x40) T1aa_count[i] += 160;
      if (i & 0x20) T1aa_count[i] += 32;
      if (i & 0x10) T1aa_count[i] += 32;
      if (i & 0x08) T1aa_count[i] += 5;
      if (i & 0x04) T1aa_count[i] += 5;
      if (i & 0x02) T1aa_count[i] += 1;
      if (i & 0x01) T1aa_count[i] += 1;
    }
    return(0);
  } else if (level == 2 && T1aa_bpp == 32) {
    for (i0 = 0; i0 < 5; i0++)
      ((T1_AA_TYPE32 *)T1aa_lut)[i0] = gv[i0];
    for (i = 0; i < 256; i++) {
      T1aa_count[i] = 0;
      if (i & 0x80) T1aa_count[i] += 512;
      if (i & 0x40) T1aa_count[i] += 512;
      if (i & 0x20) T1aa_count[i] += 64;
      if (i & 0x10) T1aa_count[i] += 64;
      if (i & 0x08) T1aa_count[i] += 8;
      if (i & 0x04) T1aa_count[i] += 8;
      if (i & 0x02) T1aa_count[i] += 1;
      if (i & 0x01) T1aa_count[i] += 1;
    }
    return(0);
  }
  else if (level == 4 && T1aa_bpp == 8) {
    for (i0 = 0; i0 < 17; i0++){ /* i0 indexes higher nibble */
      for (i1 = 0; i1 < 17; i1++){ /* i1 indixes lower nibble */
	((char *)T1aa_h_lut)[(i0 * 17 + i1) * 4 + movelow] = gv_h[i1];
	((char *)T1aa_h_lut)[(i0 * 17 + i1) * 4 + movehigh] = gv_h[i0];
      }
    }
    return(0);
  }
  else if (level == 4 && T1aa_bpp == 16) {
    for (i0 = 0; i0 < 17; i0++){ /* i0 indexes higher nibble */
      for (i1 = 0; i1 < 17; i1++){ /* i1 indixes lower nibble */
	((T1_AA_TYPE16 *)T1aa_h_lut)[(i0 * 17 + i1) * 2 + movelow] = gv_h[i1];
	((T1_AA_TYPE16 *)T1aa_h_lut)[(i0 * 17 + i1) * 2 + movehigh] = gv_h[i0];
      }
    }
    return(0);
  }
  else if (level == 4 && T1aa_bpp == 32) {
    for (i0 = 0; i0 < 17; i0++){ /* i0 indexes higher nibble */
      for (i1 = 0; i1 < 17; i1++){ /* i1 indixes lower nibble */
	((T1_AA_TYPE32 *)T1aa_h_lut)[(i0 * 17 + i1)] = gv_h[i1];
      }
    }
    return(0);
  }
  else if (level == 1 && T1aa_bpp == 8) {
    for (i0=0; i0<16; i0++) {
      ((char *)T1aa_n_lut)[i0*4+movelow]=gv_n[i0 & 0x01];
      ((char *)T1aa_n_lut)[i0*4+movelow+1]=gv_n[(i0>>1) & 0x01];
      ((char *)T1aa_n_lut)[i0*4+movelow+2]=gv_n[(i0>>2) & 0x01];
      ((char *)T1aa_n_lut)[i0*4+movelow+3]=gv_n[(i0>>3) & 0x01];
    }
    return(0);
  }
  else if (level == 1 && T1aa_bpp == 16) {
    for (i0=0; i0<4; i0++) {
      ((T1_AA_TYPE16 *)T1aa_n_lut)[i0*2]=gv_n[i0 & 0x01];
      ((T1_AA_TYPE16 *)T1aa_n_lut)[i0*2+1]=gv_n[(i0>>1) & 0x01];
    }
    return(0);
  }
  else if (level == 1 && T1aa_bpp == 32) {
    for ( i0=0; i0<2; i0++) {
      ((T1_AA_TYPE32 *)T1aa_n_lut)[i0]=gv_n[i0];
    }
    return(0);
  }
  else {
    /* unsupported combination of level and bpp -> we set T1_errno and
       put an entry into the logfile! */
    T1_errno=T1ERR_INVALID_PARAMETER;
    sprintf( err_warn_msg_buf,
	     "Unsupported AA specification: level=%d, bpp=%d",
	     level, T1aa_bpp);
    T1_PrintLog( "T1_AAInit()", err_warn_msg_buf, T1LOG_WARNING);
  }
  return(1);
}


/* T1_AADoLine: Create a single scanline of antialiased output. The
   (x, y) arguments refer to the number of pixels in the input image
   to convert down. The width argument is the number of bytes
   separating scanlines in the input. The quantity hcorr describes the
   number of subpixels. It is the shift of the oversampled bitmap to
   the right */
static void T1_AADoLine ( int level, int x, int y, int width,
			  char *c_in_ptr, char *target_ptr, int hcorr )
{
  int i=0;
  int size;
  int count=0;
  int mod;
  
  unsigned char bcarry1=0, bcarry2=0, bcarry3=0, bcarry4=0;
  
  static char *align_buf = NULL;
  static int align_buf_size = 0;
  unsigned char *in_ptr;
  
  int new_size=55;
  register char *optr;

  
  
  /* We convert the input pointer to unsigned since we use it as index! */
  in_ptr=(unsigned char*)c_in_ptr;
  
  
  if ((long)target_ptr & 3){
    /* calculate new_size (size in bytes of output buffer */
    if (level == T1_AA_LOW){
      new_size=((x + hcorr + 1) >> 1) * (T1aa_bpp >> 3);
    }
    else{ /* T1_AA_HIGH */
      new_size = ((x + hcorr + 3) >> 2) * (T1aa_bpp >> 3);
    }
    if (new_size > align_buf_size)
      {
	if (align_buf)
	  free (align_buf);
	/* Note: we allocate 12 more than necessary to have tolerance
	   at the end of line */
	align_buf = (char *)malloc(new_size+12);
	align_buf_size = new_size;
      }
    optr = align_buf;
  }
  else
    optr = target_ptr;


  /* size: The number of valid byte in the input string, i.e., the number of bytes
           partially filled with pixels before shifting with hcorr.
     mod:  Is 1 if after shifting with hcorr the last byte in the input line has an
           overflow.
  */
  
  if (level == T1_AA_LOW) {
    size=(x+7)>>3; 
    mod=(x+hcorr)>(size*8) ? 1 : 0;
    
    if (T1aa_bpp == 8) {
      if (y == 2){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_lut[(T1aa_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
		      T1aa_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	}
      }
      else if (y == 1){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_lut[(T1aa_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	}
      }
      if (mod) {
	if (y == 2)
	  ((T1_AA_TYPE32 *)optr)[i]=T1aa_lut[(T1aa_count[bcarry1] +
					      T1aa_count[bcarry2])];
	else if (y == 1)
	  ((T1_AA_TYPE32 *)optr)[i]=T1aa_lut[(T1aa_count[bcarry1])];
      }
    }
    else if (T1aa_bpp == 16) {
      if (y == 2){
	for (i = 0; i < size; i++) {
	  count = T1aa_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)]
	    + T1aa_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)];
	  ((T1_AA_TYPE32 *)optr)[i * 2] = T1aa_lut[count & 31];
	  ((T1_AA_TYPE32 *)optr)[i * 2 + 1] = T1aa_lut[count >> 5];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	}
      }
      else if (y == 1){
	for (i = 0; i < size; i++) {
	  count = T1aa_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)];
	  ((T1_AA_TYPE32 *)optr)[i * 2] = T1aa_lut[count & 31];
	  ((T1_AA_TYPE32 *)optr)[i * 2 + 1] = T1aa_lut[count >> 5];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	}
      }
      if (mod){
	if (y == 2)
	  count = T1aa_count[bcarry1] +
	    T1aa_count[bcarry2];
	else if (y == 1)
	  count = T1aa_count[bcarry1];
	((T1_AA_TYPE32 *)optr)[i * 2] = T1aa_lut[count & 31];
	((T1_AA_TYPE32 *)optr)[i * 2 + 1] = T1aa_lut[count >> 5];
      }
    }
    else if (T1aa_bpp == 32) {
      if (y == 2){
	for (i = 0; i < size; i++) {
	  count = T1aa_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
	    T1aa_count[(unsigned char)((in_ptr[i+width]<<hcorr)|bcarry2)];
	  ((T1_AA_TYPE32 *)optr)[i * 4] = T1aa_lut[count & 7];
	  ((T1_AA_TYPE32 *)optr)[i * 4 + 1] = T1aa_lut[(count >> 3) & 7];
	  ((T1_AA_TYPE32 *)optr)[i * 4 + 2] = T1aa_lut[(count >> 6) & 7];
	  ((T1_AA_TYPE32 *)optr)[i * 4 + 3] = T1aa_lut[(count >> 9) & 7];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	}
      }
      else if (y == 1) {
	for (i = 0; i < size; i++) {
	  count = T1aa_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)];
	  ((T1_AA_TYPE32 *)optr)[i * 4] = T1aa_lut[count & 7];
	  ((T1_AA_TYPE32 *)optr)[i * 4 + 1] = T1aa_lut[(count >> 3) & 7];
	  ((T1_AA_TYPE32 *)optr)[i * 4 + 2] = T1aa_lut[(count >> 6) & 7];
	  ((T1_AA_TYPE32 *)optr)[i * 4 + 3] = T1aa_lut[(count >> 9) & 7];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	}
      }
      if(mod) {
	if (y == 2){
	  count = T1aa_count[bcarry1] +
	    T1aa_count[bcarry2];
	}
	else if (y == 1){
	  count = T1aa_count[bcarry1];
	}
	((T1_AA_TYPE32 *)optr)[i * 4] = T1aa_lut[count & 7];
	((T1_AA_TYPE32 *)optr)[i * 4 + 1] = T1aa_lut[(count >> 3) & 7];
	((T1_AA_TYPE32 *)optr)[i * 4 + 2] = T1aa_lut[(count >> 6) & 7];
	((T1_AA_TYPE32 *)optr)[i * 4 + 3] = T1aa_lut[(count >> 9) & 7];
      }
    }
  }
  else if (level==T1_AA_HIGH){ 
    size=(x+7)>>3; 
    mod=(x+hcorr)>(size*8) ? 1 : 0;
    
    if (T1aa_bpp == 8) {
      if (y == 4){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] + 
			T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)] + 
			T1aa_h_count[(unsigned char)((in_ptr[i + 2*width]<<hcorr)|bcarry3)] + 
			T1aa_h_count[(unsigned char)((in_ptr[i + 3*width]<<hcorr)|bcarry4)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	  bcarry3=in_ptr[i+2*width]>>(8-hcorr);
	  bcarry4=in_ptr[i+3*width]>>(8-hcorr);
	}
      }
      else if (y == 3){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
			T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)] +
			T1aa_h_count[(unsigned char)((in_ptr[i + 2*width]<<hcorr)|bcarry3)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	  bcarry3=in_ptr[i+2*width]>>(8-hcorr);
	}
      }
      else if (y == 2){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
			T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	}
      }
      else if (y == 1){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	}
      }
      if (mod) {
	if (y == 4)
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[bcarry1] +
			T1aa_h_count[bcarry2] + 
			T1aa_h_count[bcarry3] +
			T1aa_h_count[bcarry4])];
	else if (y == 3)
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[bcarry1] +
			T1aa_h_count[bcarry2] + 
			T1aa_h_count[bcarry3])];
	else if (y == 2)
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[bcarry1] +
			T1aa_h_count[bcarry2])];
	else if (y == 1)
	  ((T1_AA_TYPE16 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[bcarry1])];
      }
    } else if (T1aa_bpp == 16) {
      if (y == 4){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] + 
			T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)] + 
			T1aa_h_count[(unsigned char)((in_ptr[i + 2*width]<<hcorr)|bcarry3)] + 
			T1aa_h_count[(unsigned char)((in_ptr[i + 3*width]<<hcorr)|bcarry4)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	  bcarry3=in_ptr[i+2*width]>>(8-hcorr);
	  bcarry4=in_ptr[i+3*width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	  bcarry3=in_ptr[2*width]<<hcorr;
	  bcarry4=in_ptr[3*width]<<hcorr;
	}
      }
      else if (y == 3){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
			T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)] +
			T1aa_h_count[(unsigned char)((in_ptr[i + 2*width]<<hcorr)|bcarry3)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+2*width]>>(8-hcorr);
	  bcarry3=in_ptr[i+3*width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	  bcarry3=in_ptr[2*width]<<hcorr;
	}
      }
      else if (y == 2){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
			T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	}
      }
      else if (y == 1){
	for (i = 0; i < size; i++) {
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)])];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	}
      }
      if (mod) {
	if (y == 4)
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[bcarry1] +
			T1aa_h_count[bcarry2] + 
			T1aa_h_count[bcarry3] +
			T1aa_h_count[bcarry4])];
	else if (y == 3)
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[bcarry1] +
			T1aa_h_count[bcarry2] + 
			T1aa_h_count[bcarry3])];
	else if (y == 2)
	  ((T1_AA_TYPE32 *)optr)[i] =
	    T1aa_h_lut[(T1aa_h_count[bcarry1] +
			T1aa_h_count[bcarry2])];
	else if (y == 1)
	  ((T1_AA_TYPE32 *)optr)[i] =
	  T1aa_h_lut[(T1aa_h_count[bcarry1])]; 
      }
    }
    else if (T1aa_bpp == 32) {
      if (y == 4){
	for (i = 0; i < size; i++) {
	  count=T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
	    T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)] +
	    T1aa_h_count[(unsigned char)((in_ptr[i + 2*width]<<hcorr)|bcarry3)] +
	    T1aa_h_count[(unsigned char)((in_ptr[i + 3*width]<<hcorr)|bcarry4)];
	  ((T1_AA_TYPE32 *)optr)[2*i] = T1aa_h_lut[count % 17];
	  ((T1_AA_TYPE32 *)optr)[2*i+1] = T1aa_h_lut[count / 17];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	  bcarry3=in_ptr[i+2*width]>>(8-hcorr);
	  bcarry4=in_ptr[i+3*width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	  bcarry3=in_ptr[2*width]<<hcorr;
	  bcarry4=in_ptr[3*width]<<hcorr;
	}
      }
      else if (y == 3){
	for (i = 0; i < size; i++) {
	  count=T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
	    T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)] +
	    T1aa_h_count[(unsigned char)((in_ptr[i + 2*width]<<hcorr)|bcarry3)];
	  ((T1_AA_TYPE32 *)optr)[2*i] = T1aa_h_lut[count % 17];
	  ((T1_AA_TYPE32 *)optr)[2*i+1] = T1aa_h_lut[count / 17];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	  bcarry3=in_ptr[i+2*width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	  bcarry3=in_ptr[2*width]<<hcorr;
	}
      }
      else if (y == 2){
	for (i = 0; i < size; i++) {
	  count=T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)] +
	    T1aa_h_count[(unsigned char)((in_ptr[i + width]<<hcorr)|bcarry2)];
	  ((T1_AA_TYPE32 *)optr)[2*i] = T1aa_h_lut[count % 17];
	  ((T1_AA_TYPE32 *)optr)[2*i+1] = T1aa_h_lut[count / 17];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	  bcarry2=in_ptr[i+width]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	  bcarry2=in_ptr[width]<<hcorr;
	}
      }
      else if (y == 1){
	for (i = 0; i < size; i++) {
	  count=T1aa_h_count[(unsigned char)((in_ptr[i]<<hcorr)|bcarry1)];
	  ((T1_AA_TYPE32 *)optr)[2*i] = T1aa_h_lut[count % 17];
	  ((T1_AA_TYPE32 *)optr)[2*i+1] = T1aa_h_lut[count / 17];
	  bcarry1=in_ptr[i]>>(8-hcorr);
	}
	if (size==0){
	  bcarry1=in_ptr[0]<<hcorr;
	}
      }
      if (mod) {
	if (y == 4){
	  count=T1aa_h_count[bcarry1] +
	    T1aa_h_count[bcarry2] +
	    T1aa_h_count[bcarry3] +
	    T1aa_h_count[bcarry4];
	}
	else if (y == 3)
	  count=T1aa_h_count[bcarry1] +
	    T1aa_h_count[bcarry2] +
	    T1aa_h_count[bcarry3];
	else if (y == 2)
	  count=T1aa_h_count[bcarry1] +
	    T1aa_h_count[bcarry2];
	else if (y == 1)
	  count=T1aa_h_count[bcarry1];
	((T1_AA_TYPE32 *)optr)[2*i] = T1aa_h_lut[count % 17];
	((T1_AA_TYPE32 *)optr)[2*i+1] = T1aa_h_lut[count / 17];
      }
    }
  }
  
  /* Copy to target if necessary */
  if ((long)target_ptr & 3){
    memcpy (target_ptr, align_buf, new_size);
  }
  
}



/* T1_DoLine(): Generate a scanline of bytes from a scanline of bits */
static void T1_DoLine ( long wd, long paddedW, char *ptr, register char *target_ptr )
{
  register int j;
  register unsigned char *in_ptr;
  T1_AA_TYPE16 *starget_ptr;
  T1_AA_TYPE32 *ltarget_ptr;
  
  in_ptr=(unsigned char *)ptr;
  
  if (T1aa_bpp==8) {
    for ( j=0; j<wd; j++ ){
      *target_ptr++=T1aa_n_lut[((in_ptr[j/8])>>j%8)&0x0F];
    }
  }
  else if (T1aa_bpp==16) {
    starget_ptr=(T1_AA_TYPE16 *)target_ptr;
    for ( j=0; j<wd; j++){
      *starget_ptr++=T1aa_n_lut[((in_ptr[j/8])>>j%8)&0x03];
    }
  }
  else if (T1aa_bpp==32) {
    ltarget_ptr=(T1_AA_TYPE32 *)target_ptr;
    for ( j=0; j<wd; j++)
      *ltarget_ptr++=T1aa_n_lut[((in_ptr[j/8])>>j%8)&0x01];
  }
  return;
}



/* T1_AASetChar(...): Generate the anti-aliased bitmap for a character */
GLYPH *T1_AASetChar( int FontID, char charcode, float size,
		     T1_TMATRIX *transform)
{
  
  GLYPH *glyph;   /* pointer to bitmap glyph */
  static GLYPH aaglyph={NULL,{0,0,0,0,0,0},NULL,DEFAULTBPP};/* The anti-aliased glyph */
  long asc, dsc, ht, wd;
  long i;
  long n_horz, n_horz_pad, n_vert, n_asc, n_dsc;
  long v_start, v_end;
  char *target_ptr;
  long offset;
  char *ptr;
  int y;
  long lsb, aalsb, aahstart;
  int memsize;
  LONG paddedW;
  int savelevel;
  FONTSIZEDEPS *font_ptr=NULL;
  unsigned char ucharcode;
  

  /* Reset character glyph, if necessary */
  if (aaglyph.bits!=NULL){
    free(aaglyph.bits);
    aaglyph.bits=NULL;
  }
  aaglyph.metrics.leftSideBearing=0;
  aaglyph.metrics.rightSideBearing=0;
  aaglyph.metrics.advanceX=0;
  aaglyph.metrics.advanceY=0;
  aaglyph.metrics.ascent=0;
  aaglyph.metrics.descent=0;
  aaglyph.pFontCacheInfo=NULL;
  aaglyph.bpp=T1aa_bpp;


  ucharcode=charcode;
  
  /* Check for smart antialiasing */
  savelevel=T1aa_level;
  if (T1aa_SmartOn){
    if (size>=T1aa_smartlimit2) {
      T1aa_level=T1_AA_NONE;
    }
    else if (size>=T1aa_smartlimit1) {
      T1aa_level=T1_AA_LOW;
    }
    else {
      T1aa_level=T1_AA_HIGH;
    }
  }


  /* The following code is only exectued if caching of antialiased
     chracters is enabled. */
  /* Check if char is in cache */
  if ((pFontBase->t1lib_flags & T1_AA_CACHING)) {
    if (transform==NULL){
      /* if size/aa is not existent we create it */
      if ((font_ptr=T1int_QueryFontSize( FontID, size, T1aa_level))==NULL){
	/* We create the required size struct and leave the rest
	   for T1_SetChar() */
	font_ptr=T1int_CreateNewFontSize( FontID, size, T1aa_level);
	if (font_ptr==NULL){
	  T1_errno=T1ERR_ALLOC_MEM;
	  T1aa_level=savelevel;
	  return(NULL);
	}
      }
      else {/* size is already existent in cache */
	if (font_ptr->pFontCache[ucharcode].bits != NULL){
	  /* Character is already in Chache -> create a copy of cache
	     and return a pointer to the result: */
	  memcpy( &aaglyph, &(font_ptr->pFontCache[ucharcode]), sizeof(GLYPH));
	  memsize = (aaglyph.metrics.ascent-aaglyph.metrics.descent) *
	    PAD((aaglyph.metrics.rightSideBearing-aaglyph.metrics.leftSideBearing) *
		T1aa_bpp,pFontBase->bitmap_pad)/8;
	  aaglyph.bits = (char *)malloc(memsize*sizeof( char));
	  if (aaglyph.bits == NULL){
	    T1_errno=T1ERR_ALLOC_MEM;
	    T1aa_level=savelevel;
	    return(NULL);
	  }
	  memcpy( aaglyph.bits, font_ptr->pFontCache[ucharcode].bits, memsize);
	  return(&(aaglyph));
	}
      }
    } /* (transform==NULL) */ 
  } /* T1_AA_CACHING */
  

  /* First, call routine to rasterize character, all error checking is
     done in this function: */ 
  if ((glyph=T1_SetChar( FontID, charcode, T1aa_level*size, transform))==NULL){
    /* restore level */
    T1aa_level=savelevel;
    return(NULL); /* An error occured */
  }
  
  /* In case there are no black pixels, we simply set the dimensions and
     then return */
  if ( glyph->bits == NULL) {
    aaglyph.bits=NULL;
    aaglyph.metrics.leftSideBearing=0;
    aaglyph.metrics.rightSideBearing=0;
    aaglyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
    aaglyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
    aaglyph.metrics.ascent=0;
    aaglyph.metrics.descent=0;
    aaglyph.pFontCacheInfo=NULL;
    /* restore level and return */
    T1aa_level=savelevel;
    return(&aaglyph);
  }

  /* Get dimensions of bitmap: */
  asc=glyph->metrics.ascent;
  dsc=glyph->metrics.descent;
  lsb=glyph->metrics.leftSideBearing;
  ht=asc-dsc;
  wd=glyph->metrics.rightSideBearing-lsb;

  
  
  if (T1aa_level==T1_AA_NONE){
    /* we only convert bitmap to bytemap */
    aaglyph=*glyph;
    aaglyph.bpp=T1aa_bpp;
    /* Compute scanline length and such */
    n_horz_pad=PAD( wd*T1aa_bpp, pFontBase->bitmap_pad )>>3;
    /* Allocate memory for glyph */
    memsize = n_horz_pad*ht*8;
    /*    aaglyph.bits = (char *)malloc(memsize*sizeof( char)); */
    aaglyph.bits = (char *)malloc(memsize*sizeof( char));
    if (aaglyph.bits == NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      /* restore level */
      T1aa_level=savelevel;
      return(NULL);
    }
    paddedW=PAD(wd,pFontBase->bitmap_pad)>>3;
    ptr=glyph->bits;
    target_ptr=aaglyph.bits;
    for (i = 0; i < ht; i++) {
      T1_DoLine ( wd, paddedW, ptr, target_ptr );
      ptr += paddedW;
      target_ptr += n_horz_pad;
    }
    /* restore level */
    T1aa_level=savelevel;
    return(&aaglyph);
  }
  

  /* Set some looping parameters for subsampling */
  if (lsb<0){
    aalsb=lsb/T1aa_level-1;
    aahstart=T1aa_level+(lsb%T1aa_level);
  }
  else{
    aalsb=lsb/T1aa_level;
    aahstart=lsb%T1aa_level;
  }
  
  /* The horizontal number of steps: */
  n_horz=(wd+aahstart+T1aa_level-1)/T1aa_level;
  /* And the padded value */
  n_horz_pad=PAD( n_horz*T1aa_bpp, pFontBase->bitmap_pad )>>3;

  /* vertical number of steps: */
  if (asc % T1aa_level){ /* not aligned */
    if ( asc > 0){
      n_asc=asc/T1aa_level+1;
      v_start=asc % T1aa_level;
    }
    else{
      n_asc=asc/T1aa_level;
      v_start=T1aa_level + (asc % T1aa_level); 
    }
  }
  else{
    n_asc=asc/T1aa_level;
    v_start=T1aa_level;
  }
  if (dsc % T1aa_level){ /* not aligned */
    if ( dsc < 0){
      n_dsc=dsc/T1aa_level-1;
      v_end=-(dsc % T1aa_level);
    }
    else{
      n_dsc=dsc/T1aa_level;
      v_end=T1aa_level - (dsc % T1aa_level);
    }
  }
  else{
    n_dsc=dsc/T1aa_level;
    v_end=T1aa_level;
  }
  /* the total number of lines: */
  n_vert=n_asc-n_dsc;
  
  /* Allocate memory for glyph */
  memsize = n_horz_pad*n_vert;

  /* Note: we allocate 12 bytes more than necessary */
  aaglyph.bits = (char *)malloc(memsize*sizeof( char) +12);
  if (aaglyph.bits == NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    /* restore level */
    T1aa_level=savelevel;
    return(NULL);
  }
  

  paddedW=PAD(wd,pFontBase->bitmap_pad)/8;
  offset=0;
  target_ptr=aaglyph.bits;
  
  /* We must check for n_vert==1 because then both v_start and v_end could / will
     affect the same AA scan line. Because I'm forgetful, a reminder:

          v_end |   000000000000000000000
	        |   111111111111111111111  ^ 
		Y   111111111111111111111  |
                    000000000000000000000  | v_start

     In order to count the v_end from bottom to top, we express it as (T1aa_level-v_end).
     The number of rows to take into account is then v_start-(T1aa_level-v_end).
  */
  if (n_vert==1) {
    v_start=v_start - (T1aa_level - v_end);
  }
  
  ptr = glyph->bits;
  for (i = 0; i < n_vert; i++) {
    if (i==0)
      y=v_start;
    else if (i==n_vert-1)
      y=v_end;
    else
      y=T1aa_level;
    T1_AADoLine ( T1aa_level, wd, y, paddedW, ptr, target_ptr, aahstart );
    ptr += y * paddedW;
    target_ptr += n_horz_pad;
  }
  
  /* .. and set them in aaglyph */
  aaglyph.metrics.leftSideBearing=aalsb;
  aaglyph.metrics.rightSideBearing=aalsb + n_horz;
  aaglyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
  aaglyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
  aaglyph.metrics.ascent=n_asc;
  aaglyph.metrics.descent=n_dsc;
  aaglyph.pFontCacheInfo=NULL;

  
  if ((pFontBase->t1lib_flags & T1_AA_CACHING) && (transform==NULL)) {
    /* Put char into cache area */
    memcpy( &(font_ptr->pFontCache[ucharcode]), &aaglyph, sizeof(GLYPH));
    font_ptr->pFontCache[ucharcode].bits = (char *)malloc(memsize*sizeof( char));
    if (font_ptr->pFontCache[ucharcode].bits == NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      T1aa_level=savelevel;
      return(NULL);
    }
    memcpy( font_ptr->pFontCache[ucharcode].bits, aaglyph.bits, memsize);
  }
  
  /* restore level */
  T1aa_level=savelevel;

  return(&aaglyph);
}



/* T1_AASetString(...): Generate the antialiased bitmap for a
   string of characters */
GLYPH *T1_AASetString( int FontID, char *string, int len, 
		       long spaceoff, int modflag, float size,
		       T1_TMATRIX *transform)
{
  GLYPH *glyph;   /* pointer to bitmap glyph */
  static GLYPH aastring_glyph={NULL,{0,0,0,0,0,0},NULL,DEFAULTBPP};/* The anti-aliased glyph */
  long asc, dsc, ht, wd;
  long i;
  long n_horz, n_horz_pad, n_vert, n_asc, n_dsc;
  long v_start, v_end;
  char *target_ptr;
  long offset;
  char *ptr;
  int y;
  long lsb, aalsb, aahstart;
  int memsize;
  LONG paddedW;
  int savelevel;
  
  
  /* Reset character glyph, if necessary */
  if (aastring_glyph.bits!=NULL){
    free(aastring_glyph.bits);
    aastring_glyph.bits=NULL;
  }
  aastring_glyph.metrics.leftSideBearing=0;
  aastring_glyph.metrics.rightSideBearing=0;
  aastring_glyph.metrics.advanceX=0;
  aastring_glyph.metrics.advanceY=0;
  aastring_glyph.metrics.ascent=0;
  aastring_glyph.metrics.descent=0;
  aastring_glyph.pFontCacheInfo=NULL;
  aastring_glyph.bpp=T1aa_bpp;
  

  /* Check for smart antialiasing */
  savelevel=T1aa_level;
  if (T1aa_SmartOn){
    if (size>=T1aa_smartlimit2) {
      T1aa_level=T1_AA_NONE;
    }
    else if (size>=T1aa_smartlimit1) {
      T1aa_level=T1_AA_LOW;
    }
    else {
      T1aa_level=T1_AA_HIGH;
    }
  }
    
  /* First, call routine to rasterize character, all error checking is
     done in this function: */ 
  if ((glyph=T1_SetString( FontID, string, len, spaceoff,
			   modflag, T1aa_level*size, transform))==NULL){
    /* restore level */
    T1aa_level=savelevel;
    return(NULL); /* An error occured */
  }
  
  /* In case there are no black pixels, we simply set the dimensions and
     then return */
  if ( glyph->bits == NULL) {
    aastring_glyph.bits=NULL;
    aastring_glyph.metrics.leftSideBearing=0;
    aastring_glyph.metrics.rightSideBearing=0;
    aastring_glyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
    aastring_glyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
    aastring_glyph.metrics.ascent=0;
    aastring_glyph.metrics.descent=0;
    aastring_glyph.pFontCacheInfo=NULL;
    /* restore level and return */
    T1aa_level=savelevel;
    return(&aastring_glyph);
  }


  /* Get dimensions of bitmap: */
  asc=glyph->metrics.ascent;
  dsc=glyph->metrics.descent;
  lsb=glyph->metrics.leftSideBearing;
  ht=asc-dsc;
  wd=glyph->metrics.rightSideBearing-lsb;
  
  if (T1aa_level==T1_AA_NONE){
    /* we only convert bitmap to bytemap */
    aastring_glyph=*glyph;
    aastring_glyph.bpp=T1aa_bpp;
    /* Compute scanline length and such */
    n_horz_pad=PAD( wd*T1aa_bpp, pFontBase->bitmap_pad )>>3;
    /* Allocate memory for glyph */
    memsize = n_horz_pad*ht*8;
    aastring_glyph.bits = (char *)malloc(memsize*sizeof( char));
    if (aastring_glyph.bits == NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      /* restore level */
      T1aa_level=savelevel;
      return(NULL);
    }
    paddedW=PAD(wd,pFontBase->bitmap_pad)>>3;
    ptr=glyph->bits;
    target_ptr=aastring_glyph.bits;
    for (i = 0; i < ht; i++) {
      T1_DoLine ( wd, paddedW, ptr, target_ptr );
      ptr += paddedW;
      target_ptr += n_horz_pad;
    }
    /* restore level */
    T1aa_level=savelevel;
    return(&aastring_glyph);
  }
  

  /* Set some looping parameters for subsampling */
  if (lsb<0){
    aalsb=lsb/T1aa_level-1;
    aahstart=T1aa_level+(lsb%T1aa_level);
  }
  else{
    aalsb=lsb/T1aa_level;
    aahstart=lsb%T1aa_level;
  }
  
  /* The horizontal number of steps: */
  n_horz=(wd+aahstart+T1aa_level-1)/T1aa_level;
  /* And the padded value */
  n_horz_pad=PAD( n_horz*T1aa_bpp, pFontBase->bitmap_pad )>>3;

  /* vertical number of steps: */
  if (asc % T1aa_level){ /* not aligned */
    if ( asc > 0){
      n_asc=asc/T1aa_level+1;
      v_start=asc % T1aa_level;
    }
    else{
      n_asc=asc/T1aa_level;
      v_start=T1aa_level + (asc % T1aa_level); 
    }
  }
  else{
    n_asc=asc/T1aa_level;
    v_start=T1aa_level;
  }
  if (dsc % T1aa_level){ /* not aligned */
    if ( dsc < 0){
      n_dsc=dsc/T1aa_level-1;
      v_end=-(dsc % T1aa_level);
    }
    else{
      n_dsc=dsc/T1aa_level;
      v_end=T1aa_level - (dsc % T1aa_level);
    }
  }
  else{
    n_dsc=dsc/T1aa_level;
    v_end=T1aa_level;
  }
  /* the total number of lines: */
  n_vert=n_asc-n_dsc;
  
  /* Allocate memory for glyph */
  memsize = n_horz_pad*n_vert;

  /* Note: we allocate 12 bytes more than necessary */ 
  aastring_glyph.bits = (char *)malloc(memsize*sizeof( char) +12);
  if (aastring_glyph.bits == NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  paddedW=PAD(wd,pFontBase->bitmap_pad)/8;
  offset=0;
  target_ptr=aastring_glyph.bits;
  
  /* We must check for n_vert==1 because then both v_start and v_end could / will
     affect the same AA scan line. 
  */
  if (n_vert==1) {
    v_start=v_start - (T1aa_level - v_end);
  }
  
  ptr = glyph->bits;
  for (i = 0; i < n_vert; i++) {
    if (i==0)
      y=v_start;
    else if (i==n_vert-1)
      y=v_end;
    else
      y=T1aa_level;
    T1_AADoLine ( T1aa_level, wd, y, paddedW, ptr, target_ptr, aahstart );
    ptr += y * paddedW;
    target_ptr += n_horz_pad;
  }
  
  /* .. and set them in aastring_glyph */
  aastring_glyph.metrics.leftSideBearing=aalsb;
  aastring_glyph.metrics.rightSideBearing=aalsb + n_horz;
  aastring_glyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
  aastring_glyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
  aastring_glyph.metrics.ascent=n_asc;
  aastring_glyph.metrics.descent=n_dsc;
  aastring_glyph.pFontCacheInfo=NULL;

  /* restore level */
  T1aa_level=savelevel;

  return(&aastring_glyph);
}



/* T1_AASetRect(): Raster a rectangle, whose size is given in charspace units.
   The resulting glyph does not cause any escapement. */
GLYPH* T1_AASetRect( int FontID, float size,
		     float width, float height, T1_TMATRIX *transform)
{
  GLYPH *glyph;   /* pointer to bitmap glyph */
  static GLYPH aaglyph={NULL,{0,0,0,0,0,0},NULL,DEFAULTBPP};/* The anti-aliased glyph */
  long asc, dsc, ht, wd;
  long i;
  long n_horz, n_horz_pad, n_vert, n_asc, n_dsc;
  long v_start, v_end;
  char *target_ptr;
  long offset;
  char *ptr;
  int y;
  long lsb, aalsb, aahstart;
  int memsize;
  LONG paddedW;
  int savelevel;
  

  /* Reset character glyph, if necessary */
  if (aaglyph.bits!=NULL){
    free(aaglyph.bits);
    aaglyph.bits=NULL;
  }
  aaglyph.metrics.leftSideBearing=0;
  aaglyph.metrics.rightSideBearing=0;
  aaglyph.metrics.advanceX=0;
  aaglyph.metrics.advanceY=0;
  aaglyph.metrics.ascent=0;
  aaglyph.metrics.descent=0;
  aaglyph.pFontCacheInfo=NULL;
  aaglyph.bpp=T1aa_bpp;


  /* Check for smart antialiasing */
  savelevel=T1aa_level;
  if (T1aa_SmartOn){
    if (size>=T1aa_smartlimit2) {
      T1aa_level=T1_AA_NONE;
    }
    else if (size>=T1aa_smartlimit1) {
      T1aa_level=T1_AA_LOW;
    }
    else {
      T1aa_level=T1_AA_HIGH;
    }
  }


  /* First, call routine to rasterize character, all error checking is
     done in this function: */ 
  if ((glyph=T1_SetRect( FontID, T1aa_level*size, width, height, transform))==NULL){
    /* restore level */
    T1aa_level=savelevel;
    return(NULL); /* An error occured */
  }
  
  /* In case there are no black pixels, we simply set the dimensions and
     then return */
  if ( glyph->bits == NULL) {
    aaglyph.bits=NULL;
    aaglyph.metrics.leftSideBearing=0;
    aaglyph.metrics.rightSideBearing=0;
    aaglyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
    aaglyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
    aaglyph.metrics.ascent=0;
    aaglyph.metrics.descent=0;
    aaglyph.pFontCacheInfo=NULL;
    /* restore level and return */
    T1aa_level=savelevel;
    return(&aaglyph);
  }

  /* Get dimensions of bitmap: */
  asc=glyph->metrics.ascent;
  dsc=glyph->metrics.descent;
  lsb=glyph->metrics.leftSideBearing;
  ht=asc-dsc;
  wd=glyph->metrics.rightSideBearing-lsb;
  
  if (T1aa_level==T1_AA_NONE){
    /* we only convert bitmap to bytemap */
    aaglyph=*glyph;
    aaglyph.bpp=T1aa_bpp;
    /* Compute scanline length and such */
    n_horz_pad=PAD( wd*T1aa_bpp, pFontBase->bitmap_pad )>>3;
    /* Allocate memory for glyph */
    memsize = n_horz_pad*ht*8;
    /*    aaglyph.bits = (char *)malloc(memsize*sizeof( char)); */
    aaglyph.bits = (char *)malloc(memsize*sizeof( char));
    if (aaglyph.bits == NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      /* restore level */
      T1aa_level=savelevel;
      return(NULL);
    }
    paddedW=PAD(wd,pFontBase->bitmap_pad)>>3;
    ptr=glyph->bits;
    target_ptr=aaglyph.bits;
    for (i = 0; i < ht; i++) {
      T1_DoLine ( wd, paddedW, ptr, target_ptr );
      ptr += paddedW;
      target_ptr += n_horz_pad;
    }
    /* restore level */
    T1aa_level=savelevel;
    return(&aaglyph);
  }
  

  /* Set some looping parameters for subsampling */
  if (lsb<0){
    aalsb=lsb/T1aa_level-1;
    aahstart=T1aa_level+(lsb%T1aa_level);
  }
  else{
    aalsb=lsb/T1aa_level;
    aahstart=lsb%T1aa_level;
  }
  
  /* The horizontal number of steps: */
  n_horz=(wd+aahstart+T1aa_level-1)/T1aa_level;
  /* And the padded value */
  n_horz_pad=PAD( n_horz*T1aa_bpp, pFontBase->bitmap_pad )>>3;

  /* vertical number of steps: */
  if (asc % T1aa_level){ /* not aligned */
    if ( asc > 0){
      n_asc=asc/T1aa_level+1;
      v_start=asc % T1aa_level;
    }
    else{
      n_asc=asc/T1aa_level;
      v_start=T1aa_level + (asc % T1aa_level); 
    }
  }
  else{
    n_asc=asc/T1aa_level;
    v_start=T1aa_level;
  }
  if (dsc % T1aa_level){ /* not aligned */
    if ( dsc < 0){
      n_dsc=dsc/T1aa_level-1;
      v_end=-(dsc % T1aa_level);
    }
    else{
      n_dsc=dsc/T1aa_level;
      v_end=T1aa_level - (dsc % T1aa_level);
    }
  }
  else{
    n_dsc=dsc/T1aa_level;
    v_end=T1aa_level;
  }
  /* the total number of lines: */
  n_vert=n_asc-n_dsc;
  
  /* Allocate memory for glyph */
  memsize = n_horz_pad*n_vert;

  /* Note: we allocate 12 bytes more than necessary */
  aaglyph.bits = (char *)malloc(memsize*sizeof( char) +12);
  if (aaglyph.bits == NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    /* restore level */
    T1aa_level=savelevel;
    return(NULL);
  }
  

  paddedW=PAD(wd,pFontBase->bitmap_pad)/8;
  offset=0;
  target_ptr=aaglyph.bits;
  
  /* We must check for n_vert==1 because then both v_start and v_end could / will
     affect the same AA scan line. 
  */
  if (n_vert==1) {
    v_start=v_start - (T1aa_level - v_end);
  }

  ptr = glyph->bits;
  for (i = 0; i < n_vert; i++) {
    if (i==0)
      y=v_start;
    else if (i==n_vert-1)
      y=v_end;
    else
      y=T1aa_level;
    T1_AADoLine ( T1aa_level, wd, y, paddedW, ptr, target_ptr, aahstart );
    ptr += y * paddedW;
    target_ptr += n_horz_pad;
  }
  
  /* .. and set them in aaglyph */
  aaglyph.metrics.leftSideBearing=aalsb;
  aaglyph.metrics.rightSideBearing=aalsb + n_horz;
  aaglyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
  aaglyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
  aaglyph.metrics.ascent=n_asc;
  aaglyph.metrics.descent=n_dsc;
  aaglyph.pFontCacheInfo=NULL;

  /* restore level */
  T1aa_level=savelevel;

  return(&aaglyph);

}



/* T1_AASetGrayValues(): Sets the byte values that are put into the
   pixel position for the respective entries:
   Returns 0 if successfull.
   */
int T1_AASetGrayValues(unsigned long white,
		       unsigned long gray75,
		       unsigned long gray50,
		       unsigned long gray25,
		       unsigned long black)
{
  
  if (T1_CheckForInit()){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }
  
  gv[4]=(unsigned T1_AA_TYPE32)black;    /* black value */
  gv[3]=(unsigned T1_AA_TYPE32)gray25;   /* gray 25% value */
  gv[2]=(unsigned T1_AA_TYPE32)gray50;   /* gray 50% value */   
  gv[1]=(unsigned T1_AA_TYPE32)gray75;   /* gray 75% value */
  gv[0]=(unsigned T1_AA_TYPE32)white;    /* white value */
  
  T1aa_bg=white;
  
  if ((T1_AAInit( T1_AA_LOW)))
    return(-1);
  return(0);
  
}

		     

/* T1_AAHSetGrayValues(): Sets the byte values that are put into the
   pixel position for the respective entries (for 17 gray levels):
   Returns 0 if successfull.
   */
int T1_AAHSetGrayValues( unsigned long *grayvals)
{
  int i;
  
  if (T1_CheckForInit()){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }

  /* 0==white(background) ... 16==black(foreground) */
  for (i=0; i<17; i++){
    gv_h[i]=(unsigned T1_AA_TYPE32)grayvals[i];
  }
  

  T1aa_bg=grayvals[0];
  
  if ((T1_AAInit( T1_AA_HIGH)))
    return(-1);
  return(0);
  
}



/* T1_AANSetGrayValues(): Sets the byte values that are put into the
   pixel position for the respective entries (for 2 gray levels):
   Returns 0 if successfull. This is for the case the non-antialiased
   "bytemaps" should be generated.
   */
int T1_AANSetGrayValues( unsigned long bg, unsigned long fg)
{
  
  if (T1_CheckForInit()){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }

  gv_n[0]=bg;
  gv_n[1]=fg;
  
  T1aa_bg=bg;
  
  if ((T1_AAInit( T1_AA_NONE)))
    return(-1);
  return(0);
  
}



/* Get the current setting of graylevels for 2x antialiasing. The 5
   values are stored at address pgrayvals in order from background to
   foreground */
int T1_AAGetGrayValues( long *pgrayvals)  
{
  int i;
  
  if (T1_CheckForInit()) {
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }

  if (pgrayvals==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }
  
  for ( i=0; i<5; i++) { /* bg (i=0)  to fg (i=4) */
    pgrayvals[i]=gv[i];
  }
  return( 0);
  
}



/* Get the current setting of graylevels for 4x antialiasing. The 17
   values are stored at address pgrayvals in order from background to
   foreground */
int T1_AAHGetGrayValues( long *pgrayvals) 
{
  int i;

  if (T1_CheckForInit()) {
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }

  if (pgrayvals==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }

  for ( i=0; i<17; i++) { /* bg (i=0)  to fg (i=16) */
    pgrayvals[i]=gv_h[i];
  }
  return( 0);
}



/* Get the current setting of graylevels for no antialiasing. The 2
   values are stored at address pgrayvals in order from background to
   foreground */
int T1_AANGetGrayValues( long *pgrayvals) 
{

  if (T1_CheckForInit()) {
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }

  if (pgrayvals==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }
  pgrayvals[0]=gv_n[0]; /* background */
  pgrayvals[1]=gv_n[1]; /* foreground */
  return( 0);
}


/* T1_AASetBitsPerPixel(): Sets the depths of the antialiased glyph
   pixel. Returns 0 if bpp is valid and -1 otherwise. If 24 is
   specified, meaning to be the depth rather than the bpp-value,
   automatically 32 bpp is chosen. */
int  T1_AASetBitsPerPixel( int bpp)
{
  
  if (T1_CheckForInit()){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }
  

  /* T1aa_level = 0; */

  if (bpp==8){
    T1aa_bpp=8;
    return(0);
  }
  if (bpp==16){
    T1aa_bpp=16;
    return(0);
  }
  if ((bpp==32)|(bpp==24)){
    T1aa_bpp=32;
    return(0);
  }

  T1_errno=T1ERR_INVALID_PARAMETER;
  return(-1);
}


/* T1_AAGetBitsPerPixel(): Return the number of bits per pixel set in
   t1lib. 
*/
int T1_AAGetBitsPerPixel( void)
{
  return( T1aa_bpp);
  
}


/* Set the Subsampling level for subsequent operations: */
int T1_AASetLevel( int level)
{
  
   if (T1_CheckForInit()){
     T1_errno=T1ERR_OP_NOT_PERMITTED;
     return(-1);
   }

   if (level==T1_AA_LOW){
     T1aa_level=T1_AA_LOW;
     return(0);
   }
   else if (level==T1_AA_HIGH){
     T1aa_level=T1_AA_HIGH;
     return(0);
   }
   else if (level==T1_AA_NONE){
     T1aa_level=T1_AA_NONE;
     return(0);
   }
   
   T1_errno=T1ERR_INVALID_PARAMETER;
   return(-1);
   
}


/* Get the current subsampling level */
int T1_AAGetLevel( void)
{
  return( T1aa_level);
}


/* T1_AAFillOutline(): Create a filled glyph from an outline description */
GLYPH *T1_AAFillOutline( T1_OUTLINE *path, int modflag)
{
  
  GLYPH *glyph;   /* pointer to bitmap glyph */
  static GLYPH aaglyph={NULL,{0,0,0,0,0,0},NULL,DEFAULTBPP};/* The anti-aliased glyph */
  long asc, dsc, ht, wd;
  long i;
  long n_horz, n_horz_pad, n_vert, n_asc, n_dsc;
  long v_start, v_end;
  char *target_ptr;
  long offset;
  char *ptr;
  int y;
  long lsb, aalsb, aahstart;
  int memsize;
  LONG paddedW;
 

  /* Reset character glyph, if necessary */
  if (aaglyph.bits!=NULL){
    free(aaglyph.bits);
    aaglyph.bits=NULL;
  }
  aaglyph.metrics.leftSideBearing=0;
  aaglyph.metrics.rightSideBearing=0;
  aaglyph.metrics.advanceX=0;
  aaglyph.metrics.advanceY=0;
  aaglyph.metrics.ascent=0;
  aaglyph.metrics.descent=0;
  aaglyph.pFontCacheInfo=NULL;
  aaglyph.bpp=T1aa_bpp;


  /* First, scale outline appropriately: */
  path=T1_ScaleOutline( path, T1aa_level);
  
  /* Second, call routine to fill outline, all error checking is
     done in this function: */ 
  if ((glyph=T1_FillOutline( path, modflag))==NULL)
    return(NULL); /* An error occured */
  
  /* In case there are no black pixels, we simply set the dimensions and
     then return */
  if ( glyph->bits == NULL) {
    aaglyph.bits=NULL;
    aaglyph.metrics.leftSideBearing=0;
    aaglyph.metrics.rightSideBearing=0;
    aaglyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
    aaglyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
    aaglyph.metrics.ascent=0;
    aaglyph.metrics.descent=0;
    aaglyph.pFontCacheInfo=NULL;
    return(&aaglyph);
  }

  /* Get dimensions of bitmap: */
  asc=glyph->metrics.ascent;
  dsc=glyph->metrics.descent;
  lsb=glyph->metrics.leftSideBearing;
  ht=asc-dsc;
  wd=glyph->metrics.rightSideBearing-lsb;
  

  if (T1aa_level==T1_AA_NONE){
    /* we only convert bitmap to bytemap */
    aaglyph=*glyph;
    aaglyph.bpp=T1aa_bpp;
    /* Compute scanline length and such */
    n_horz_pad=PAD( wd*T1aa_bpp, pFontBase->bitmap_pad )>>3;
    /* Allocate memory for glyph, we alloc 12 bytes more to simplify
       subsampling! */
    memsize = n_horz_pad*ht*8;
    aaglyph.bits = (char *)malloc(memsize*sizeof( char) +12);
    if (aaglyph.bits == NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
    paddedW=PAD(wd,pFontBase->bitmap_pad)>>3;
    ptr=glyph->bits;
    target_ptr=aaglyph.bits;
    for (i = 0; i < ht; i++) {
      T1_DoLine ( wd, paddedW, ptr, target_ptr );
      ptr += paddedW;
      target_ptr += n_horz_pad;
    }
    return(&aaglyph);
  }
  

  /* Set some looping parameters for subsampling */
  if (lsb<0){
    aalsb=lsb/T1aa_level-1;
    aahstart=T1aa_level+(lsb%T1aa_level);
  }
  else{
    aalsb=lsb/T1aa_level;
    aahstart=lsb%T1aa_level;
  }
  
  /* The horizontal number of steps: */
  n_horz=(wd+aahstart+T1aa_level-1)/T1aa_level;
  /* And the padded value */
  n_horz_pad=PAD( n_horz*T1aa_bpp, pFontBase->bitmap_pad )>>3;
  
  /* vertical number of steps: */
  if (asc % T1aa_level){ /* not aligned */
    if ( asc > 0){
      n_asc=asc/T1aa_level+1;
      v_start=asc % T1aa_level;
    }
    else{
      n_asc=asc/T1aa_level;
      v_start=T1aa_level + (asc % T1aa_level); 
    }
  }
  else{
    n_asc=asc/T1aa_level;
    v_start=T1aa_level;
  }
  if (dsc % T1aa_level){ /* not aligned */
    if ( dsc < 0){
      n_dsc=dsc/T1aa_level-1;
      v_end=-(dsc % T1aa_level);
    }
    else{
      n_dsc=dsc/T1aa_level;
      v_end=T1aa_level - (dsc % T1aa_level);
    }
  }
  else{
    n_dsc=dsc/T1aa_level;
    v_end=T1aa_level;
  }
  /* the total number of lines: */
  n_vert=n_asc-n_dsc;
  
  /* Allocate memory for glyph */
  memsize = n_horz_pad*n_vert;
  
  aaglyph.bits = (char *)malloc(memsize*sizeof( char)+12);
  if (aaglyph.bits == NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  paddedW=PAD(wd,pFontBase->bitmap_pad)/8;
  offset=0;
  target_ptr=aaglyph.bits;
  
  /* We must check for n_vert==1 because then both v_start and v_end could / will
     affect the same AA scan line.
  */
  if (n_vert==1) {
    v_start=v_start - (T1aa_level - v_end);
  }

  ptr = glyph->bits;
  for (i = 0; i < n_vert; i++) {
    if (i==0)
      y=v_start;
    else if (i==n_vert-1)
      y=v_end;
    else
      y=T1aa_level;
    T1_AADoLine ( T1aa_level, wd, y, paddedW, ptr, target_ptr, aahstart );
    ptr += y * paddedW;
    target_ptr += n_horz_pad;
  }
  
  /* .. and set them in aaglyph */
  aaglyph.metrics.leftSideBearing=aalsb;
  aaglyph.metrics.rightSideBearing=aalsb + n_horz;
  aaglyph.metrics.advanceX=(int) floor(glyph->metrics.advanceX/(float)T1aa_level+0.5);
  aaglyph.metrics.advanceY=(int) floor(glyph->metrics.advanceY/(float)T1aa_level+0.5);
  aaglyph.metrics.ascent=n_asc;
  aaglyph.metrics.descent=n_dsc;
  aaglyph.pFontCacheInfo=NULL;

  return(&aaglyph);
}



/* T1_AASetSmartLimits(): Set the limit-values for smart
   antialiasing. Returns 0 if OK, and -1 else. */
int T1_AASetSmartLimits( float limit1, float limit2)
{

  if (limit1 > 0.0 && limit2 > 0.0 && limit2 >= limit2) {
    T1aa_smartlimit1=limit1;
    T1aa_smartlimit2=limit2;
    return( 0);
  }
  else{
    T1_errno=T1ERR_INVALID_PARAMETER;
    return( -1);
  }
}



/* T1_AASetSmartMode(): Enable or disable smart anialiasing */
int T1_AASetSmartMode( int smart)
{

  if (smart==T1_YES) {
    T1aa_SmartOn=1;
  }
  else if (smart==T1_NO) {
    T1aa_SmartOn=0;
  }
  else {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return( -1);
  }
  return( 0);
}


