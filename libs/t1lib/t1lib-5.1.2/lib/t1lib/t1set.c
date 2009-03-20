/*--------------------------------------------------------------------------
  ----- File:        t1set.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-01
  ----- Description: This file is part of the t1-library. It contains
                     functions for setting characters and strings of
		     characters.
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
  
#define T1SET_C


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
#include <setjmp.h>

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
#include "t1set.h"
#include "t1load.h"
#include "t1finfo.h"
#include "t1misc.h"
#include "t1base.h"


/* As a fall back */
#ifndef T1_AA_TYPE16 
#define T1_AA_TYPE16    short
#endif
#ifndef T1_AA_TYPE32 
#define T1_AA_TYPE32    int
#endif


extern char *t1_get_abort_message( int number);
extern struct region *Interior(struct segment *path, int fillrule);
extern unsigned T1_AA_TYPE32 T1aa_bg;          /* white value */


static void T1_ComputeLineParameters( int FontID, int width,
				      int mode, float size,
				      int *startx, int *endx,
				      int *starty, int *endy);
  
  

static int T1_bit=T1GLYPH_BIT;
static int T1_byte;
static int T1_pad;
static int T1_wordsize;

static int c_shift=8;
static int s_shift=16;
static int l_shift=32;



/* bin_dump(): Print a binary dump of a byte, short and
   long variable (used for debug purposes only): */
static void bin_dump_c(unsigned char value, char space_flag)
{
  int i,j;
  
  for (i=0;i<=7;i++){
    if ((j=((value)>>i)&0x01))
      printf("X");
    else
      printf(".");
  }
  if (space_flag)
    printf(" ");

}

static void bin_dump_s(unsigned short value, char space_flag)
{
  int i,j;

  if (T1_CheckEndian()){
    for (i=8;i<=15;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
    for (i=0;i<=7;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
  }
  else{
    for (i=0;i<=15;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
  }
  if (space_flag)
    printf(" ");
  
}

static void bin_dump_l(unsigned long value, char space_flag)
{
  int i,j;
  
  if (T1_CheckEndian()){
    for (i=24;i<=31;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
    for (i=16;i<=23;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
    for (i=8;i<=15;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
    for (i=0;i<=7;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
  }
  else{
    for (i=0;i<=31;i++){
      if ((j=((value)>>i)&0x01))
	printf("X");
      else
	printf(".");
    }
  }
  if (space_flag)
    printf(" ");

}



/* T1_SetChar(...): Generate the bitmap for a character */
GLYPH *T1_SetChar( int FontID, char charcode, float size,
		   T1_TMATRIX *transform)
{
  int i;
  int mode;
  struct region *area;
  struct XYspace *Current_S;
  int cache_flag    = 1;
  int rot_flag      = 0;
  unsigned char ucharcode;
  float strokewidth = 0.0f;
  volatile int strokeextraflag = 0;
  
  
  FONTSIZEDEPS *font_ptr;
  FONTPRIVATE  *fontarrayP;
  
  volatile int memsize=0;
  LONG h,w;
  LONG paddedW;

  /* We don't implement underlining for characters, but the rasterer
     implements it. Thus, we use a modflag of constant 0 */
  int modflag=0;

  static GLYPH glyph={NULL,{0,0,0,0,0,0},NULL,1};


  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_SetChar()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }

  ucharcode=(unsigned char)charcode;

  
  /* Reset character glyph, if necessary */
  if (glyph.bits!=NULL){
    free(glyph.bits);
    glyph.bits=NULL;
  }
  glyph.metrics.leftSideBearing=0;
  glyph.metrics.rightSideBearing=0;
  glyph.metrics.advanceX=0;
  glyph.metrics.advanceY=0;
  glyph.metrics.ascent=0;
  glyph.metrics.descent=0;
  glyph.pFontCacheInfo=NULL;
  glyph.bpp=1;  
  
  /* First, check for a correct ID */
  i=T1_CheckForFontID(FontID);
  if (i==-1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  /* if necessary load font into memory */
  if (i==0)
    if (T1_LoadFont(FontID))
      return(NULL);

  /* Check for valid size */
  if (size<=0.0){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  }

  /* Assign padding value */
  T1_pad=pFontBase->bitmap_pad;
  if (pFontBase->endian)
    T1_byte=1;
  else
    T1_byte=0;
  T1_wordsize=T1_pad;

  fontarrayP=&(pFontBase->pFontArray[FontID]);
  
  /* Check if rotation is requested */
  if (transform!=NULL){
    /* there's rotation requested => do not cache the resulting bitmap */
    rot_flag=1;
    cache_flag=0;
  }
  else {
    rot_flag=0;
    cache_flag=1;
  }
  
  /* handle stroking stuff */
  if ( pFontBase->pFontArray[FontID].info_flags & RASTER_STROKED) {
    /* Stroking requested. If caching is not desired, clear cache_flag.
       Otherwise, leave it unaffected. */
    if ( (pFontBase->pFontArray[FontID].info_flags & CACHE_STROKED) == 0 ) {
      /* filled glyphs are cached, indicate that character is to be rendered
	 on the fly and not to be cached */
      strokeextraflag = 1;
      cache_flag = 0;
    }
    strokewidth = pFontBase->pFontArray[FontID].StrokeWidth;
  }
  else {
    /* filling requested. */
    if ( (pFontBase->pFontArray[FontID].info_flags & CACHE_STROKED) != 0 ) {
      /* stroked glyphs are cached, indicate that character is to be rendered
	 on the fly and not to be cached */
      strokeextraflag = 1;
      cache_flag = 0;
    }
    strokewidth = 0.0f;
  }
  
  /* font is now loaded into memory =>
     Check for size: */
  if ((font_ptr=T1int_QueryFontSize( FontID, size, NO_ANTIALIAS))==NULL){
    font_ptr=T1int_CreateNewFontSize( FontID, size, NO_ANTIALIAS);
    if (font_ptr==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
  }
  else {/* size is already existent in cache */
    /* If no rotation and no noncached stroking , try to get character from cache */
    if ( (rot_flag==0) && (strokeextraflag==0) ) {
      /* we don't use the .bits entry to check because in newer releases
	 also white glyphs (bits=NULL) are allowed. Rather, we check
	 whether bpp > 0! */
      if (font_ptr->pFontCache[ucharcode].bpp > 0){
	/* Character is already in Chache -> create a copy of cache
	   and return a pointer to the result: */
	memcpy( &glyph, &(font_ptr->pFontCache[ucharcode]), sizeof(GLYPH));
	paddedW = PAD((glyph.metrics.rightSideBearing-glyph.metrics.leftSideBearing),
		      T1_pad);
	if (font_ptr->pFontCache[ucharcode].bits!=NULL) {
	  memsize = (glyph.metrics.ascent-glyph.metrics.descent)*paddedW / 8;
	  glyph.bits = (char *)malloc(memsize*sizeof( char));
	  if (glyph.bits == NULL){
	    T1_errno=T1ERR_ALLOC_MEM;
	    return(NULL);
	  }
	  memcpy( glyph.bits, font_ptr->pFontCache[ucharcode].bits, memsize);
	}
	return(&(glyph));
      }
    }
  }
  /* Setup an appropriate charspace matrix. Note that the rasterizer
     assumes vertical values with inverted sign! Transformation should
     create a copy of the local charspace matrix which then still has
     to be made permanent. */
  if (rot_flag){
    Current_S=(struct XYspace *) 
      Permanent(Scale(Transform (font_ptr->pCharSpaceLocal,
				 transform->cxx, - transform->cxy,
				 transform->cyx, - transform->cyy),
		      DeviceSpecifics.scale_x, DeviceSpecifics.scale_y));
  }
  else{
    Current_S=(struct XYspace *)
      Permanent(Scale(Transform(font_ptr->pCharSpaceLocal,
				1.0, 0.0, 0.0, -1.0),
		      DeviceSpecifics.scale_x, DeviceSpecifics.scale_y));
  }
  
  /* fnt_ptr now points to the correct FontSizeDeps-struct =>
     lets now raster the character */
  mode=0;
  area=fontfcnB( FontID, modflag, Current_S,
		 fontarrayP->pFontEnc,
		 ucharcode, &mode,
		 fontarrayP->pType1Data,
		 DO_RASTER,
		 strokewidth);
  KillSpace (Current_S);

  /* fill the glyph-structure */
  if (mode > 0) {
    sprintf( err_warn_msg_buf, "fontfcnB() set mode=%d", mode);
    T1_PrintLog( "T1_SetChar()", err_warn_msg_buf, T1LOG_WARNING);
    T1_errno=mode;
    return(NULL);
  }
  if (area == NULL){
    T1_PrintLog( "T1_SetChar()", "area=NULL returned by fontfcnB()", T1LOG_WARNING);
    T1_errno=mode;
    return(NULL);
  }
  h = area->ymax - area->ymin;
  w = area->xmax - area->xmin;

  if (mode == FF_NOTDEF_SUBST) {
    sprintf( err_warn_msg_buf,
	     "\".notdef\" substituted for character %d from font %d",
	     ucharcode, FontID);
    T1_PrintLog( "T1_SetChar()", err_warn_msg_buf, T1LOG_WARNING);
    mode=0;
  }
  
  paddedW = PAD(w, T1_pad);
  
  if (h > 0 && w > 0) {
    memsize = h * paddedW / 8 + 1;
    /* This is for the users copy of the character, for security-reasons
       the original pointer to the cache area is not used. The entry glyph.bits
       is free'ed every time this function is called: */
    glyph.bits = (char *)malloc(memsize*sizeof( char));
    if (glyph.bits == NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      /* make sure to get rid of 'area' before leaving! */
      KillRegion (area);
      return(NULL);
    }
  }
  else {
    h = w = 0;
    area->xmin = area->xmax = 0;
    area->ymin = area->ymax = 0;
  }

  /* Assign metrics */
  glyph.metrics.leftSideBearing  = area->xmin;
  glyph.metrics.advanceX   = NEARESTPEL(area->ending.x - area->origin.x);
  glyph.metrics.advanceY   = - NEARESTPEL(area->ending.y - area->origin.y);
  glyph.metrics.rightSideBearing = area->xmax;
  glyph.metrics.descent          = - area->ymax;
  glyph.metrics.ascent           = - area->ymin;

  
  if (h > 0 && w > 0) {
    (void) memset(glyph.bits, 0, memsize);
    fill(glyph.bits, h, paddedW, area, T1_byte, T1_bit, T1_wordsize );
  }
  
  /* make sure to get rid of 'area' before leaving! */
  KillRegion (area);
  
  /* Cache glyph if requested, also if it contains only white space */
  if (cache_flag){
    /* Copy glyph to cache but be careful not to modify the pixmap
       entry */
    font_ptr->pFontCache[ucharcode].metrics.leftSideBearing=glyph.metrics.leftSideBearing;
    font_ptr->pFontCache[ucharcode].metrics.rightSideBearing=glyph.metrics.rightSideBearing;
    font_ptr->pFontCache[ucharcode].metrics.advanceX=glyph.metrics.advanceX;
    font_ptr->pFontCache[ucharcode].metrics.advanceY=glyph.metrics.advanceY;
    font_ptr->pFontCache[ucharcode].metrics.ascent=glyph.metrics.ascent;
    font_ptr->pFontCache[ucharcode].metrics.descent=glyph.metrics.descent;
    font_ptr->pFontCache[ucharcode].pFontCacheInfo=glyph.pFontCacheInfo;
    font_ptr->pFontCache[ucharcode].bpp=glyph.bpp;
    if (glyph.bits==NULL) {
      font_ptr->pFontCache[ucharcode].bits=NULL;
    }
    else {
      font_ptr->pFontCache[ucharcode].bits = (char *)malloc(memsize*sizeof( char));
      if (font_ptr->pFontCache[ucharcode].bits == NULL){
	T1_errno=T1ERR_ALLOC_MEM;
	return(NULL);
      }
      /* ... and copy bitmap to cache area: */
      memcpy( font_ptr->pFontCache[ucharcode].bits, glyph.bits, memsize);
    }
    
  }


  return(&glyph);
}



/* T1_SetString(...): Generate the bitmap for a string of characters */
GLYPH *T1_SetString( int FontID, char *string, volatile int len, 
		     long spaceoff, int modflag, float size,
		     T1_TMATRIX *transform)
{
  volatile int i, j=0, k;
  int mode;
  /* initialize this to NULL just to be on the safe side */
  struct region *area = NULL;
  struct XYspace *Current_S, *S;
  int cache_flag=1;
  volatile int rot_flag=0;
  int *kern_pairs;       /* use for accessing the kern pairs if kerning is
			    requested */
  int no_chars=0;        /* The number of characters in the string */
  static int lastno_chars=0;
  float factor;
  long spacewidth;       /* This is given to fontfcnb_string() */
  float strokewidth = 0.0f;
  volatile int strokeextraflag = 0;
  
  
  FONTSIZEDEPS *font_ptr;
  FONTPRIVATE  *fontarrayP;
  

  volatile int memsize=0;

  long h,w;
  long paddedW, char_paddedW;
  int h_anchor, v_anchor;
  static int *pixel_h_anchor_corr=NULL;
  static int *flags=NULL;
  int lsb_min, rsb_max;
  long overallwidth=0;
  long overallascent=0;
  long overalldescent=0;

  int underline_startx, underline_starty, underline_endx, underline_endy;
  int overline_startx, overline_starty, overline_endx, overline_endy;
  int overstrike_startx, overstrike_starty, overstrike_endx, overstrike_endy;
  int start, middle;
  int afm_ind;
  
  char startmask, endmask;
  static unsigned char *r2lstring;
  static int r2l_len=0;
  
  static GLYPH string_glyph={NULL,{0,0,0,0,0,0},NULL,1};
  GLYPH *currchar;

  
  
  /* The following are for bitmap blitting */
  long BitShift;
  long ByteOffset;
  /* pad=8 */
  unsigned T1_AA_TYPE16 BitBuf_c;
  unsigned char *p_c;
  unsigned char *Target_c;
  /* pad=16 */
  unsigned T1_AA_TYPE32 BitBuf_s;
  unsigned T1_AA_TYPE16 *p_s;
  unsigned T1_AA_TYPE16 *Target_s;
#ifdef T1_AA_TYPE64 
  /* pad=32 */
  unsigned T1_AA_TYPE64 BitBuf_l;
  unsigned T1_AA_TYPE32 *p_l;
  unsigned T1_AA_TYPE32 *Target_l;
#endif
  unsigned char *ustring;
  

  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_SetString()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }

  /* force string elements into unsigned */
  ustring=(unsigned char*)string;


  /* Check for valid string */
  if (string==NULL){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  }

  /* Reorganize if required */
  if (modflag & T1_RIGHT_TO_LEFT){
    if (len)
      i=len;
    else
      i=j=strlen( string);
    if (i+1>r2l_len){
      if (r2lstring!=NULL)
	free( r2lstring);
      r2lstring=(unsigned char *)malloc( (i+1)*sizeof(char));
      r2l_len=i+1;
    }
    j--;
    while ( i--) {
      r2lstring[j-i]=ustring[i];
    }
    ustring=r2lstring;
    len=j+1;
  }
  
  
  /* Reset string glyph, if necessary */
  if (string_glyph.bits!=NULL){
    free(string_glyph.bits);
    string_glyph.bits=NULL;
  }
  string_glyph.metrics.leftSideBearing=0;
  string_glyph.metrics.rightSideBearing=0;
  string_glyph.metrics.advanceX=0;
  string_glyph.metrics.advanceY=0;
  string_glyph.metrics.ascent=0;
  string_glyph.metrics.descent=0;
  string_glyph.pFontCacheInfo=NULL;
  string_glyph.bpp=1;
  
  /* We don't want to cache the resulting bitmap: */
  cache_flag=0;
  
  /* First, check for a correct ID */
  i=T1_CheckForFontID(FontID);
  if (i==-1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  /* if necessary load font into memory */
  if (i==0)
    if (T1_LoadFont(FontID))
      return(NULL);

  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return(NULL);
  }

  /* Check for valid size */
  if (size<=0.0){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  }

  /* Assign padding value */
  T1_pad=pFontBase->bitmap_pad;
  if (pFontBase->endian)
    T1_byte=1;
  else
    T1_byte=0;
  T1_wordsize=T1_pad;

  fontarrayP=&(pFontBase->pFontArray[FontID]);
  
  /* Check if rotation is requested */
  if (transform!=NULL){
    /* there's rotation requested => do not cache the resulting bitmap */
    rot_flag=1;
    cache_flag=0;
  }
  
  /* handle stroking stuff */
  if ( pFontBase->pFontArray[FontID].info_flags & RASTER_STROKED) {
    /* Stroking requested. If caching is not desired, clear cache_flag.
       Otherwise, leave it unaffected. */
    if ( (pFontBase->pFontArray[FontID].info_flags & CACHE_STROKED) == 0 ) {
      /* filled glyphs are cached, indicate that character is to be rendered
	 on the fly and not to be cached */
      strokeextraflag = 1;
      cache_flag = 0;
    }
    strokewidth = pFontBase->pFontArray[FontID].StrokeWidth;
  }
  else {
    /* filling requested. */
    if ( (pFontBase->pFontArray[FontID].info_flags & CACHE_STROKED) != 0 ) {
      /* stroked glyphs are cached, indicate that character is to be rendered
	 on the fly and not to be cached */
      strokeextraflag = 1;
      cache_flag = 0;
    }
    strokewidth = 0.0f;
  }
  
  /* font is now loaded into memory =>
     Check for size: */
  if ((font_ptr=T1int_QueryFontSize( FontID, size, NO_ANTIALIAS))==NULL){
    font_ptr=T1int_CreateNewFontSize( FontID, size, NO_ANTIALIAS);
    if (font_ptr==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
  }
  
  /* Now comes string specific stuff: Get length of string and create an
     array of integers where to store the bitmap positioning dimens: */
  if (len<0){  /* invalid length */
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  }
  
  if (len==0) /* should be computed assuming "normal" 0-terminated string,
		 or R2L-part has already been computed! */
    no_chars=strlen(string);
  else        /* use value given on command line */
    no_chars=len;

  /* If necessary, allocate memory */
  if (no_chars>lastno_chars){
    if (pixel_h_anchor_corr!=NULL){
      free(pixel_h_anchor_corr);
    }
    if (flags!=NULL){
      free(flags);
    }
    pixel_h_anchor_corr=(int *)calloc(no_chars, sizeof(int));
    flags=(int *)calloc(no_chars, sizeof(int));
    lastno_chars=no_chars;
  }
  else{
    /* Reset flags  and position array */
    for (i=0; i<no_chars; i++){
      flags[i]=0;
      pixel_h_anchor_corr[i]=0;
    }
  }
  
  /* Setup an appropriate charspace matrix. Note that the rasterizer
     assumes vertical values with inverted sign! Transformation should
     create a copy of the local charspace matrix which then still has
     to be made permanent. */
  if (rot_flag){
    Current_S=(struct XYspace *) 
      Permanent(Scale(Transform (font_ptr->pCharSpaceLocal,
				 transform->cxx, - transform->cxy,
				 transform->cyx, - transform->cyy),
		      DeviceSpecifics.scale_x, DeviceSpecifics.scale_y));
  }
  else{
    Current_S=(struct XYspace *)
      Permanent(Scale(Transform(font_ptr->pCharSpaceLocal,
				1.0, 0.0, 0.0, -1.0),
		      DeviceSpecifics.scale_x, DeviceSpecifics.scale_y));
  }
  
  /* Compute the correct spacewidth value (in charspace units). The
     value supplied by the user is interpreted as an offset in
     char space units:
     */
  spacewidth=T1_GetCharWidth(FontID,fontarrayP->space_position)+spaceoff;
  
  
  /* The following if (rot_flag==0){ ... } contains the whole code
     to execute when the string is oriented horizontal. Then, there's
     the possibility of concatenating most probably existent bitmaps
     from the Cache, which should be much faster than every time raster
     all the characters.
     */
  /* First, ensure that all needed characters are in the Cache; if not,
     generate them */
  if ( (rot_flag==0) && (strokeextraflag==0) ){
    overallwidth=0;
    for (i=0; i<no_chars; i++) {
      currchar= &(font_ptr->pFontCache[ustring[i]]);
      if (currchar->bpp<1) {
	flags[i]=0;
	mode=0;
	/*  Don't try to generate space-character: */
	if (ustring[i]!=fontarrayP->space_position){
	  /* Note: Never underline etc. cached chars --> modflag=0 */ 
	  area=fontfcnB( FontID, 0, Current_S,
			 fontarrayP->pFontEnc,
			 ustring[i], &mode,
			 fontarrayP->pType1Data,
			 DO_RASTER,
			 strokewidth);

	  /* fill the glyph-structure */
	  if (mode > 0) {
	    sprintf( err_warn_msg_buf, "fontfcnB() set mode=%d", mode);
	    T1_PrintLog( "T1_SetString()", err_warn_msg_buf, T1LOG_WARNING);
	    T1_errno=mode;
	    /* Make sure to free the region if it's non-NULL */
	    if (area){
	      KillRegion (area);
	    }
	    return(NULL);
	  }
	  if (area == NULL){
	    T1_PrintLog( "T1_SetString()", "area=NULL returned by fontfcnB()", T1LOG_WARNING);
	    T1_errno=mode;
	    return(NULL);
	  }
	  
	  if (mode == FF_NOTDEF_SUBST) {
	    sprintf( err_warn_msg_buf,
		     "\".notdef\" substituted in string[%d] (=character %d) from font %d",
		     i, ustring[i], FontID);
	    T1_PrintLog( "T1_SetString()", err_warn_msg_buf, T1LOG_WARNING);
	    mode=0;
	    flags[i]=1; /* indicates that .notdef has been substituted */
	  }
	  
	  h = area->ymax - area->ymin;
	  w = area->xmax - area->xmin;
	  
	  if (h > 0 && w > 0) {  /* bitmap contains pixels */
	    paddedW = PAD(w, T1_pad);
	    memsize = h * paddedW / 8 + 1;
	    currchar->bits = (char *)malloc(memsize*sizeof( char));
	    if (currchar->bits == NULL) {
	      T1_errno=T1ERR_ALLOC_MEM;
	      /* Make sure to free up the region if it's non-NULL */
	      if (area){
		KillRegion (area);
	      }
	      return(NULL);
	    }
	    (void) memset(currchar->bits, 0, memsize);
	    fill(currchar->bits, h, paddedW, area,
		 T1_byte, T1_bit, T1_wordsize );
	  }
	  /* area valid -> set metrics in cache for that character */ 
	  currchar->metrics.leftSideBearing  = area->xmin;
	  currchar->metrics.advanceX   = NEARESTPEL(area->ending.x - area->origin.x);
	  currchar->metrics.advanceY   = - NEARESTPEL(area->ending.y - area->origin.y);
	  currchar->metrics.rightSideBearing = area->xmax;
	  currchar->metrics.descent          = - area->ymax;
	  currchar->metrics.ascent           = - area->ymin;
	  currchar->bpp=1;
	  
	  /* By this point, it's clear that the 'area' struct exists,
	     and that we no longer need it, so free it up. */
	  KillRegion (area);
	}
      } /* end of if ( ... ), needed characters are now in cache! */
    } /* end of for (i<no_chars) loop */
    /* we still have to delete the Space */
    KillSpace (Current_S);

    /* We've to compute the scale factor: */
    factor=size*DeviceSpecifics.scale_x/1000;

    /* h_anchor is the horizontal position in the resulting bitmap;
       it is incremented on the fly and of course initially 0! */
    h_anchor=0;
    /* These are the initial values for left and right sidebearing */
    rsb_max=-30000;
    lsb_min= 30000;
    overallascent=-30000;
    overalldescent=30000;
    for (i=0;i<no_chars;i++){
      /* first get index into AFM-tables */
      afm_ind=pFontBase->pFontArray[FontID].pEncMap[(int) ustring[i]];
      /* Advance to next character in high resolution */
      if (ustring[i]==fontarrayP->space_position)
	h_anchor +=(int)spacewidth;
      /* check for a substituted char or notdef and get missing escapement */
      else if (afm_ind==0) {
	/* Setup apropriate charspace matrix */
	S=(struct XYspace *)IDENTITY;
	/* Make this permanent so that scaling it in fontfcnB_ByName will
	   make a duplicate of it, and this duplicate can thus be safely
	   destroyed.  Fixes the somewhat smaller memory leak */
	S=(struct XYspace *)Permanent
	  (Transform(S, pFontBase->pFontArray[FontID].FontTransform[0],
		     pFontBase->pFontArray[FontID].FontTransform[1],
		     pFontBase->pFontArray[FontID].FontTransform[2],
		     pFontBase->pFontArray[FontID].FontTransform[3]));
	/* Get metrics values */
	area=fontfcnB_ByName( FontID, 0, S, ".notdef", &mode,
			      pFontBase->pFontArray[FontID].pType1Data, DO_RASTER);
	if (area==NULL || mode!=0){
	  sprintf( err_warn_msg_buf,
		   "Could not get charspace representation of \".notdef\", Font %d, mode=%d!",
		   FontID, mode);
	  T1_PrintLog( "T1_SetString()", err_warn_msg_buf,
		       T1LOG_ERROR);
	  T1_errno=mode;
	  return( NULL);
	}
	/* value to add to hanchor */ 
	h_anchor +=NEARESTPEL(area->ending.x);
	/* make sure to free S */
	if (S) {
	  KillSpace (S);
	}
	/* make sure to free area */
	if (area) {
	  KillRegion (area);
	}
      }
      else{
	currchar=&(font_ptr->pFontCache[ustring[i]]);
	pixel_h_anchor_corr[i]=((int)floor(h_anchor*factor+0.5));
	/* Store maximum rsb ...*/
	if ( pixel_h_anchor_corr[i]+currchar->metrics.rightSideBearing > rsb_max){
	  rsb_max=pixel_h_anchor_corr[i]+currchar->metrics.rightSideBearing;
	}
	/* ... and minimum lsb */
	if ( pixel_h_anchor_corr[i]+currchar->metrics.leftSideBearing < lsb_min){
	  lsb_min=pixel_h_anchor_corr[i]+currchar->metrics.leftSideBearing;
	}
	/* Advance by current chars width */
	/*	h_anchor +=T1_GetCharWidth( FontID, ustring[i]); */
	if (afm_ind>0) { /* we have a standard character */
	  h_anchor +=(fontarrayP->pAFMData->cmi[afm_ind-1].wx) * fontarrayP->extend;
	}
	else { /* afm_ind must be negative so that we have a composite char */
	  h_anchor +=(fontarrayP->pAFMData->ccd[-(afm_ind+1)].wx) * fontarrayP->extend;
	}
	/* If kerning, insert kerning amount */
	if ((i<no_chars -1) && (modflag & T1_KERNING))
	  h_anchor +=T1_GetKerning( FontID, ustring[i], ustring[i+1]);
	
	/* We still have to care for vertical metrics: */
	if (currchar->metrics.ascent>overallascent)
	  overallascent=currchar->metrics.ascent;
	if (currchar->metrics.descent<overalldescent)
	  overalldescent=currchar->metrics.descent;
      }
    }
    string_glyph.metrics.advanceX=(int)floor(h_anchor*factor+0.5);

    /* We have to check for underlining and such things. They may affect the
       bounding box of the glyph */
    if (modflag & T1_UNDERLINE){
      T1_ComputeLineParameters( FontID, T1_UNDERLINE,
				string_glyph.metrics.advanceX,
				size,
			        &underline_startx, &underline_endx,
			        &underline_starty, &underline_endy);
      /* Check whether rsb is affected ...*/
      if ( underline_endx > rsb_max){
	rsb_max=underline_endx;
      }
      /* ... and for lsb */
      if ( underline_startx < lsb_min){
	lsb_min=underline_startx;
      }

      /* Vertical metrics may also be affected. */
      if (underline_starty > overallascent)
	overallascent=underline_starty;
      if (underline_endy < overalldescent)
	overalldescent=underline_endy;
    }
    if (modflag & T1_OVERLINE){
      T1_ComputeLineParameters( FontID, T1_OVERLINE,
				string_glyph.metrics.advanceX,
				size,
			        &overline_startx, &overline_endx,
			        &overline_starty, &overline_endy);
      /* Check whether rsb is affected ...*/
      if ( overline_endx > rsb_max){
	rsb_max=overline_endx;
      }
      /* ... and for lsb */
      if ( overline_startx < lsb_min){
	lsb_min=overline_startx;
      }

      /* Vertical metrics may also be affected. */
      if (overline_starty > overallascent)
	overallascent=overline_starty;
      if (overline_endy < overalldescent)
	overalldescent=overline_endy;
    }
    if (modflag & T1_OVERSTRIKE){
      T1_ComputeLineParameters( FontID, T1_OVERSTRIKE,
				string_glyph.metrics.advanceX,
				size,
			        &overstrike_startx, &overstrike_endx,
			        &overstrike_starty, &overstrike_endy);
      /* Check whether rsb is affected ...*/
      if ( overstrike_endx > rsb_max){
	rsb_max=overstrike_endx;
      }
      /* ... and for lsb */
      if ( overstrike_startx < lsb_min){
	lsb_min=overstrike_startx;
      }

      /* Vertical metrics may also be affected. */
      if (overstrike_starty > overallascent)
	overallascent=overstrike_starty;
      if (overstrike_endy < overalldescent)
	overalldescent=overstrike_endy;
    }
    

    
    /* Set the string-glyphs' metrics: */
    /* Bearings: */
    string_glyph.metrics.rightSideBearing=rsb_max;      
    string_glyph.metrics.leftSideBearing=lsb_min;
    /* ascent and descent: */
    string_glyph.metrics.ascent=overallascent;
    string_glyph.metrics.descent=overalldescent;

    

    /* Now, allocate memory for the resulting bitmap and set it to 0: */
    h=overallascent - overalldescent;
    w=string_glyph.metrics.rightSideBearing-string_glyph.metrics.leftSideBearing;
    
    paddedW = PAD( w, T1_pad);
    if ( h > 0 && w > 0){
      memsize = h * paddedW / 8;
      /* We alloc some amount more in order to avoid alignment check in
	 bit-blitting loop */
      memsize += T1_pad/8;
      string_glyph.bits = (char *)malloc( memsize);
      if (string_glyph.bits == NULL) {
	T1_errno=T1ERR_ALLOC_MEM;
	return(NULL);
      }
      
    }
    else {
    /* We have only characters without pixels in the string ->
       we allow this and only set the advance width in the glyph.
       The bits pointer will be NULL. We reset rsb and lsb */
      string_glyph.metrics.rightSideBearing=0;      
      string_glyph.metrics.leftSideBearing=0;
      string_glyph.metrics.ascent=0;      
      string_glyph.metrics.descent=0;
    }
    
    if (string_glyph.bits != NULL) {
      
      memset(string_glyph.bits, 0, memsize);
      
      /* Now comes the loop for bitmap blitting: */
      for (i=0;i<no_chars;i++){
	/* Get pointer to character number i of string: */
	currchar=&(font_ptr->pFontCache[ustring[i]]);
	
	/* First, we have to correct the positioning values to refer to
	   the bitmap BBox */
	pixel_h_anchor_corr[i] -= lsb_min;
	pixel_h_anchor_corr[i] += currchar->metrics.leftSideBearing;	
	
	/* Compute vertical anchor for current char-bitmap: */
	v_anchor=overallascent - currchar->metrics.ascent;
	char_paddedW=PAD( currchar->metrics.rightSideBearing
			  - currchar->metrics.leftSideBearing , T1_pad);
	/* We have to check for Big Endian. In that case, we have to
	   act on byte-level */
	if (T1_byte){
	  BitShift =  pixel_h_anchor_corr[i] % 8;
	  ByteOffset = pixel_h_anchor_corr[i] / 8;
	}
	else { 
	  BitShift =  pixel_h_anchor_corr[i] % T1_pad;
	  ByteOffset = pixel_h_anchor_corr[i] / T1_pad;
	  if (T1_pad==32)
	    ByteOffset *=4;
	  else if (T1_pad==16)
	    ByteOffset *=2;
	}  
#ifdef T1_AA_TYPE64
	/* We compile this part only if long is 64 bits to be conform to ANSI C */
	if (T1_pad==32 && T1_byte==0){
	  /* The following loop steps through the lines of the character bitmap: */
	  for (j=0;j<currchar->metrics.ascent-currchar->metrics.descent;j++){
	    Target_l= (unsigned T1_AA_TYPE32 *)(string_glyph.bits +((v_anchor+j)*paddedW/8)
					+ByteOffset);
	    /* The following loop copies the scanline of a character bitmap: */
	    if (currchar->bits != NULL) {
	      p_l = (unsigned T1_AA_TYPE32 *)(currchar->bits+(char_paddedW/8*j));
	      if (BitShift == 0) {
		for (k=char_paddedW >> 5; k; k--)
		  *Target_l++ |= *p_l++;
	      } else {
		for (k=0; k < char_paddedW / 32 ; k++){
		  BitBuf_l= ((T1_AA_TYPE64)(*p_l++)) << BitShift;
		  *Target_l++ |= BitBuf_l;
		  *Target_l |= BitBuf_l>>l_shift;
		} /* End of for ( .. ) stepping through columns */
	      }
	    } /* End of if(currchar->bits != NULL) */
	  } /* End of for( .. ) steppin' through lines of char bitmap */
	}/* end if (T1_pad==32) */
	else
#endif 
	  if (T1_pad==16 && T1_byte==0){
	    /* The following loop steps through the lines of the character bitmap: */
	    for (j=0;j<currchar->metrics.ascent-currchar->metrics.descent;j++){
	      Target_s= (unsigned T1_AA_TYPE16 *)(string_glyph.bits +((v_anchor+j)*paddedW/8)
					   +ByteOffset);
	      /* The following loop copies the scanline of a character bitmap: */
	      if (currchar->bits != NULL) {
		p_s = (unsigned  T1_AA_TYPE16 *)(currchar->bits+(char_paddedW/8*j));
		if (BitShift == 0) {
		  for (k=char_paddedW >> 4; k; k--)
		    *Target_s++ |= *p_s++;
		}
		else{
		  for (k=char_paddedW >> 4; k; k--){
		    BitBuf_s= ((T1_AA_TYPE32)(*p_s++))<<BitShift;
		    *Target_s++ |= BitBuf_s;
		    *Target_s |= BitBuf_s>>s_shift;
		  } /* End of for ( .. ) stepping through columns */
		}
	      } /* End of if(currchar->bits != NULL) */
	    } /* End of for( .. ) steppin' through lines of char bitmap */
	  }/* end if (T1_pad==16 */
	  else{ /* T1_pad = 8 or Big Endian machine */
	    /* The following loop steps through the lines of the character bitmap: */
	    for (j=0;j<currchar->metrics.ascent-currchar->metrics.descent;j++){
	      Target_c= (unsigned char *)(string_glyph.bits +((v_anchor+j)*paddedW/8)
					  +ByteOffset);
	      /* The following loop copies the scanline of a character bitmap: */
	      if (currchar->bits != NULL) {
		p_c = (unsigned char *)(currchar->bits+(char_paddedW/8*j));
		if (BitShift == 0){
		  for (k=char_paddedW >> 3; k; k--)
		    *Target_c++ |= *p_c++;
		}
		else{
		  for (k=char_paddedW >> 3; k; k--){
		    BitBuf_c = ((T1_AA_TYPE16)(*p_c++)) << BitShift;
		    *Target_c++ |= BitBuf_c;
		    *Target_c |= BitBuf_c>>c_shift;
		  } /* End of for ( .. ) stepping through columns */
		}
	      } /* End of if(currchar->bits != NULL) */
	    } /* End of for( .. ) steppin' through lines of char bitmap */
	  } /* end if (T1_pad==8) and/or BigEndian  */
      }
    } /* if (string_glyph.bits != NULL) */
    
    
    /* We now put the underlining rule on the glyph */
    if (modflag & T1_UNDERLINE){
      start=-string_glyph.metrics.leftSideBearing;
      for (i=string_glyph.metrics.ascent-underline_starty;
	   i<string_glyph.metrics.ascent-underline_endy; i++){
	start =-string_glyph.metrics.leftSideBearing +
	  (int) floor((double)(string_glyph.metrics.ascent-i)
		      *fontarrayP->slant+0.5);
	middle = ((start+string_glyph.metrics.advanceX) / 8) - (start / 8);
	startmask = 0xFF << (start % 8);
	endmask = (char) ~(0xFF << ((start+string_glyph.metrics.advanceX) % 8));
	Target_c= (unsigned char *)(string_glyph.bits +(i*paddedW/8)
				    + (start / 8));
	if (!Target_c) {
	  continue;
	}
	j=middle;
	if (j == 0)
	  *Target_c++ |= startmask & endmask;
	else {
	  *Target_c++ |= startmask;
	  while (--j > 0)
	    *Target_c++ = (unsigned char) 0xFF;
	  if ((endmask))
	    *Target_c |= endmask;
	}
      }
    }
    /* Put an overstrike rule on the glyph */
    if (modflag & T1_OVERSTRIKE){
      start=-string_glyph.metrics.leftSideBearing;
      for (i=string_glyph.metrics.ascent-overstrike_starty;
	   i<string_glyph.metrics.ascent-overstrike_endy; i++){
	start =-string_glyph.metrics.leftSideBearing +
	  (int) floor((double)(string_glyph.metrics.ascent-i)
		      *fontarrayP->slant+0.5);
	middle = ((start+string_glyph.metrics.advanceX) / 8) - (start / 8);
	startmask = 0xFF << (start % 8);
	endmask = (char) ~(0xFF << ((start+string_glyph.metrics.advanceX) % 8));
	Target_c= (unsigned char *)(string_glyph.bits +(i*paddedW/8)
				    + (start / 8));
	if (!Target_c) {
	  continue;
	}
	j=middle;
	if (j == 0)
	  *Target_c++ |= startmask & endmask;
	else {
	  *Target_c++ |= startmask;
	  while (--j > 0)
	    *Target_c++ = (unsigned char) 0xFF;
	  if ((endmask))
	    *Target_c |= endmask;
	}
      }
    }
    /* Put an overline rule */
    if (modflag & T1_OVERLINE){
      start=-string_glyph.metrics.leftSideBearing;
      for (i=string_glyph.metrics.ascent-overline_starty;
	   i<string_glyph.metrics.ascent-overline_endy; i++){
	start =-string_glyph.metrics.leftSideBearing +
	  (int) floor((double)(string_glyph.metrics.ascent-i)
		      *fontarrayP->slant+0.5);
	middle = ((start+string_glyph.metrics.advanceX) / 8) - (start / 8);
	startmask = 0xFF << (start % 8);
	endmask = (char) ~(0xFF << ((start+string_glyph.metrics.advanceX) % 8));
	Target_c= (unsigned char *)(string_glyph.bits +(i*paddedW/8)
				    + (start / 8));
	if (!Target_c) {
	  continue;
	}
	j=middle;
	if (j == 0)
	  *Target_c++ |= startmask & endmask;
	else {
	  *Target_c++ |= startmask;
	  while (--j > 0)
	    *Target_c++ = (unsigned char) 0xFF;
	  if ((endmask))
	    *Target_c |= endmask;
	}
      }
    }

    /* Check for writing direction and re-compute dimensions appropriately: */
    if (modflag & T1_RIGHT_TO_LEFT){
      string_glyph.metrics.advanceX *= -1;
      string_glyph.metrics.leftSideBearing += string_glyph.metrics.advanceX;
      string_glyph.metrics.rightSideBearing += string_glyph.metrics.advanceX;
    } 
    
    return(&string_glyph);
  } /* end of "if (rot_flag==0.0)" */


  /* fnt_ptr now points to the correct FontSizeDeps-struct =>
     lets now raster the character */
  mode=0;
  kern_pairs=(int *)calloc(no_chars, sizeof(int));
  if ((modflag & T1_KERNING))
    for (i=0; i<no_chars -1; i++)
      kern_pairs[i]=T1_GetKerning( FontID, ustring[i], ustring[i+1]);
  area=fontfcnB_string( FontID, modflag, Current_S,
			fontarrayP->pFontEnc,
			ustring, no_chars, &mode,
			fontarrayP->pType1Data,
			kern_pairs, spacewidth,
			DO_RASTER,
			strokewidth);
  KillSpace (Current_S);
  
  /* In all cases, free memory for kerning pairs */
  free(kern_pairs);
  
  /* fill the string_glyph-structure */
  if (mode > 0) {
    sprintf( err_warn_msg_buf, "fontfcnB() set mode=%d", mode);
    T1_PrintLog( "T1_SetString()", err_warn_msg_buf, T1LOG_WARNING);
    T1_errno=mode;
    /* make sure to get rid of area if it's there */
    if (area){
      KillRegion (area);
    }
    return(NULL);
  }
  if (area == NULL){
    T1_PrintLog( "T1_SetString()", "area=NULL returned by fontfcnB()", T1LOG_WARNING);
    T1_errno=mode;
    return(NULL);
  }
  
  if (mode == FF_NOTDEF_SUBST) {
    sprintf( err_warn_msg_buf,
	     "\".notdef\" substituted somewhere in string from font %d",
	     FontID);
    T1_PrintLog( "T1_SetString()", err_warn_msg_buf, T1LOG_WARNING);
    mode=0;
  }
  
  h = area->ymax - area->ymin;
  w = area->xmax - area->xmin;
  paddedW = PAD(w, T1_pad);
  if (h > 0 && w > 0) {
    memsize = h * paddedW / 8 + 1;
    /* This is for the users copy of the character, for security-reasons
       the original pointer to the cache area is not used. The entry string_glyph.bits
       is free'ed every time this function is called: */
    string_glyph.bits = (char *)malloc(memsize*sizeof( char));
    if (string_glyph.bits == NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      /* make sure to get rid of area if it's there */
      if (area){
	KillRegion (area);
      }
      /* it's safe to free this, since this is a rotated glyph and
	 therefore it's a copy of the original one */
      return(NULL);
    }
  }
  else {
    h = w = 0;
    area->xmin = area->xmax = 0;
    area->ymin = area->ymax = 0;
  }
  
  string_glyph.metrics.leftSideBearing=area->xmin;
  string_glyph.metrics.advanceX=NEARESTPEL(area->ending.x - area->origin.x);
  string_glyph.metrics.advanceY=-NEARESTPEL(area->ending.y - area->origin.y);
  string_glyph.metrics.rightSideBearing=area->xmax; 
  string_glyph.metrics.descent=-area->ymax;  
  string_glyph.metrics.ascent=-area->ymin; 

  if (h > 0 && w > 0) {
    (void) memset(string_glyph.bits, 0, memsize);
    fill(string_glyph.bits, h, paddedW, area, T1_byte, T1_bit, T1_wordsize );
  }
  else { /* We have no black pixels */
    string_glyph.metrics.leftSideBearing=0;
    string_glyph.metrics.advanceX=NEARESTPEL(area->ending.x - area->origin.x);
    string_glyph.metrics.advanceY=-NEARESTPEL(area->ending.y - area->origin.y);
    string_glyph.metrics.rightSideBearing=0; 
    string_glyph.metrics.descent=0;  
    string_glyph.metrics.ascent=0; 
  }
  

  /* make sure to get rid of area if it's there */
  if (area){
    KillRegion (area);
  }
  /* Check for writing direction and re-compute dimensions appropriately: */
  if (modflag & T1_RIGHT_TO_LEFT){
    string_glyph.metrics.advanceX *= -1;
    string_glyph.metrics.advanceY *= -1;
    string_glyph.metrics.leftSideBearing += string_glyph.metrics.advanceX;
    string_glyph.metrics.rightSideBearing += string_glyph.metrics.advanceX;
    string_glyph.metrics.descent += string_glyph.metrics.advanceY;
    string_glyph.metrics.ascent += string_glyph.metrics.advanceY;
  } 
  return(&string_glyph);
}



void fill(dest, h, w, area, byte, bit, wordsize)
     register char *dest;  /* destination bitmap                           */
     int h,w;              /* dimensions of 'dest', w padded               */
     register struct region *area;  /* region to write to 'dest'           */
     int byte,bit;         /* flags; LSBFirst or MSBFirst                  */
     int wordsize;         /* number of bits per word for LSB/MSB purposes */
{
  register struct edgelist *edge;  /* for looping through edges         */
  register char *p;     /* current scan line in 'dest'                  */
  register int y;       /* for looping through scans                    */
  register int wbytes = w / 8;  /* number of bytes in width             */
  register pel *leftP,*rightP;  /* pointers to X values, left and right */
  int xmin = area->xmin;  /* upper left X                               */
  int ymin = area->ymin;  /* upper left Y                               */
  
  for (edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link) {
    
    p = dest + (edge->ymin - ymin) * wbytes;
    leftP = edge->xvalues;
    rightP = edge->link->xvalues;
    
    for (y = edge->ymin; y < edge->ymax; y++) {
      fillrun(p, *leftP++ - xmin , *rightP++ - xmin, bit);
      p += wbytes;
    }
  }
  /* 
    Now, as an afterthought, we'll go reorganize if odd byte order requires
    it:
    */
  /* We do not reorganize since t1lib internally always uses LSBFirst */
  /*
  if ( 0 && wordsize != 8) {
    register int i; 
    printf("Reorganizing data ..\n");
    
    switch (wordsize) { 
    case 16: 
      { 
	register unsigned short data,*p; 
	p = (unsigned short *) dest; 
	for (i = h * w /16; --i >= 0;) { 
	  data = *p; 
	  *p++ = (data << 8) + (data >> 8); 
	} 
	break; 
      } 
    case 64: 
    case 32: 
      { 
	register ULONG data,*p; 
	p = (ULONG *) dest; 
	for (i = h * w / 32; --i >= 0;) { 
	  data = *p; 
	  *p++ = (data << 24) + (data >> 24) 
	    + (0xFF00 & (data >> 8)) 
	    + (0xFF0000 & (data << 8)); 
	} 
	if (wordsize == 64) { 
	  p = (ULONG *) dest; 
	  for (i = h * w / 64; --i >= 0;) { 
	    data = *p++; 
	    p[-1] = p[0]; 
	    *p++ = data; 
	  } 
	}
	break; 
      } 
    default: 
      abort("xiFill: unknown format"); 
    } 
  }
  */
}

#define  ALLONES  0xFF

void fillrun(register char *p, pel x0, pel x1, int bit)
{
  register int startmask,endmask;  /* bits to set in first and last char*/
  register int middle;  /* number of chars between start and end + 1    */

  if (x1 <= x0)
    return;
  middle = x1/8 - x0/8;
  p += x0/8;
  x0 &= 7;  x1 &= 7;
  if (bit == LSBFirst) {
    startmask = ALLONES << x0;
    endmask = ~(ALLONES << x1);
  }
  else {
    startmask = ALLONES >> x0;
    endmask = ~(ALLONES >> x1);
  }
  if (middle == 0)
    *p++ |= startmask & endmask;
  else {
    *p++ |= startmask;
    while (--middle > 0){
      *p++ = (unsigned char)ALLONES;
    }
    if (endmask)
      *p |= endmask;
  }
}


/* outline */
#undef CHECK_OUTLINEFILL
#ifdef CHECK_OUTLINEFILL
void fill(dest, h, w, area, byte, bit, wordsize)
     register char *dest;  /* destination bitmap                           */
     int h,w;              /* dimensions of 'dest', w padded               */
     register struct region *area;  /* region to write to 'dest'           */
     int byte,bit;         /* flags; LSBFirst or MSBFirst                  */
     int wordsize;         /* number of bits per word for LSB/MSB purposes */
{
  register struct edgelist *edge;  /* for looping through edges         */
  register char *p;     /* current scan line in 'dest'                  */
  register int y;       /* for looping through scans                    */
  register int wbytes = w / 8;  /* number of bytes in width             */
  register pel *leftP,*rightP;  /* pointers to X values, left and right */
  int xmin = area->xmin;  /* upper left X                               */
  int ymin = area->ymin;  /* upper left Y                               */
  
  for (edge = area->anchor; VALIDEDGE(edge); edge = edge->link->link) {
    
    p = dest + (edge->ymin - ymin) * wbytes;
    leftP = edge->xvalues;
    rightP = edge->link->xvalues;

    printf("leftP=%d, RightP=%d,\n", *leftP, *rightP);
    
    
    for (y = edge->ymin; y < edge->ymax; y++) {
      printf("leftP=%d, RightP=%d,y=%d\n", *leftP, *rightP, y);
      
      fillrun(p, *leftP++ - xmin, *rightP++ - xmin, bit);
      p += wbytes;
    }
  }
}
#endif


/* T1_CopyGlyph(): Make a copy of an existent glyph-structure to save it
   for later usage by the user. */
GLYPH *T1_CopyGlyph( GLYPH *glyph)
{
  GLYPH *dest;
  long size;
  

  if (glyph==NULL){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  }
  
  /* Assign padding value */
  T1_pad=pFontBase->bitmap_pad;
  /* Allocate memory for struct: */
  if ((dest=(GLYPH *)malloc(sizeof(GLYPH)))==NULL){
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  /* Copy the structure members: */
  *dest=*glyph;
  /* Allocate memory for bitmap, initialize pointer to it and copy bitmap: */
  size=PAD((dest->metrics.rightSideBearing-dest->metrics.leftSideBearing)*
	   glyph->bpp, T1_pad) / 8;
  size=size*(dest->metrics.ascent-dest->metrics.descent);
   /* We must check whether there's actually a bits-pointer different from
      NULL. If not omit the following step: */
  if (glyph->bits!=NULL){
    if ((dest->bits=(char *)malloc(size*sizeof(char)))==NULL){
      free(dest);
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
    memcpy(dest->bits,glyph->bits,size);
  }
  return(dest);
}

/* T1_DumpGlyph(): Dump a glyph-representation to stdout: */
void T1_DumpGlyph( GLYPH *glyph)
{
  int i,j,h,w;   /* i=line-index, j=column-index */ 
  long paddedW;
  
  
  printf("Dataformat: T1_bit=%d, T1_byte=%d, T1_wordsize=%d, T1_pad=%d\n",
	 T1_bit, T1_byte, T1_pad, T1_pad);

  if (glyph==NULL){
    return;
  }
  
  h=glyph->metrics.ascent-glyph->metrics.descent;
  w=glyph->metrics.rightSideBearing-glyph->metrics.leftSideBearing;
  
  paddedW=PAD(w,T1_pad);
  
  printf("GlyphInfo: h=%d, w=%d, paddedW=%ld\n", h, w, paddedW);
  
  for ( i=0; i<h; i++){
    if (T1_pad==8)
      for (j=0; j<paddedW/T1_pad; j++)
	bin_dump_c(glyph->bits[i*paddedW/T1_pad+j], 1);
    else if (T1_pad==16)
      for (j=0; j<paddedW/T1_pad; j++){
	bin_dump_s(((unsigned short *)glyph->bits)[i*paddedW/T1_pad+j], 1);
      }
    else
      for (j=0; j<paddedW/T1_pad; j++){
	bin_dump_l(((unsigned long *)glyph->bits)[i*paddedW/T1_pad+j], 1);
      }
    printf("\n");
  } 
} 



/* This function will essentially return the bounding box of the
   line-rule */
static void T1_ComputeLineParameters( int FontID, int mode,
				      int width, float size,
				      int *startx, int *endx,
				      int *starty, int *endy)
{
  float position=0.0, thickness=0.0;
  int startx1, startx2, endx1, endx2;
  
  if (mode & T1_UNDERLINE){
    position=pFontBase->pFontArray[FontID].UndrLnPos
      * DeviceSpecifics.scale_y;
    thickness=pFontBase->pFontArray[FontID].UndrLnThick
      * DeviceSpecifics.scale_y;
  }
  else if (mode & T1_OVERLINE){
    position=pFontBase->pFontArray[FontID].OvrLnPos
      * DeviceSpecifics.scale_y;
    thickness=pFontBase->pFontArray[FontID].OvrLnThick
      * DeviceSpecifics.scale_y;
  }
  else if (mode & T1_OVERSTRIKE){
    position=pFontBase->pFontArray[FontID].OvrStrkPos
      * DeviceSpecifics.scale_y;
    thickness=pFontBase->pFontArray[FontID].OvrStrkThick
      * DeviceSpecifics.scale_y;
  }
  
  *starty=(int) floor((position+0.5*(thickness-DeviceSpecifics.y_resolution/1000.0))
		      *size/1000.0 +0.5 );
  startx1=(int) floor(*starty*pFontBase->pFontArray[FontID].slant+0.5);
  *endy  =*starty - (int) floor(thickness/1000.0*size+0.5);
  startx2=(int) floor(*endy*pFontBase->pFontArray[FontID].slant+0.5);
  *startx=startx1 < startx2 ? startx1 : startx2;
  endx1  = width +
    (int) floor(*starty*pFontBase->pFontArray[FontID].slant+0.5);
  endx2  = width +
    (int) floor(*endy*pFontBase->pFontArray[FontID].slant+0.5);
  *endx=endx1 < endx2 ? endx2 : endx1;

  /* We take care that at least one pixel is drawn */
  if (*starty==*endy)
    *endy -=1;
  
  return;

}



/* The following function concatenates two glyphs. Optional offsets
   x_off, y_off are respected. By the function.
   If either glyph is NULL or the glyphs have distinct depth, NULL is
   returned. */
GLYPH *T1_ConcatGlyphs( GLYPH *glyph_1, GLYPH *glyph_2,
			int x_off, int y_off, int modflag)
{

  int lsb1, lsb2, rsb1, rsb2;
  int advanceX1, advanceX2, advanceY1, advanceY2;
  int ascent1, ascent2, descent1, descent2;
  int rsb, lsb, ascent, descent, advanceX, advanceY;
  int vanchor, w, h, wscanline, wscanline1, wscanline2, bpp;
  int memsize, BitShift;
  GLYPH *glyph1, *glyph2;
  unsigned T1_AA_TYPE16 BitBuf_c;
  unsigned T1_AA_TYPE32 BitBuf_s;
  unsigned long BitBuf_l;   /* This is only actually used if sizeof long = 8 */
  int i, j, k;
  int ByteOffset;
  
  unsigned char *Target_c, *p_c;
  unsigned T1_AA_TYPE16 *Target_s, *p_s;
  unsigned T1_AA_TYPE32 *Target_l, *p_l;
  
  static GLYPH glyph={NULL,{0,0,0,0,0,0},NULL,1};
  

  /* We handle the Right-To-Left concatenation the simple way:
     1) Change the order of the two glyphs
     2) Invert the sign of the y-part of the offset
     3) Recalculate the dimensions of the resulating glyph.
  */
  /* Check for writing direction and reorganize appropriately: */
  if (modflag & T1_RIGHT_TO_LEFT){
    glyph1=glyph_2;
    glyph2=glyph_1;
    y_off=-y_off;
  }
  else {
    glyph1=glyph_1;
    glyph2=glyph_2;
  }
  
  if (glyph1==NULL){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return( NULL);
  }
  if (glyph2==NULL){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return( NULL);
  }
  if (glyph1->bpp != glyph2->bpp){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return( NULL);
  }
  
  /* We have two glyph different from NULL */
  /* Reset glyph, if necessary */
  if (glyph.bits!=NULL){
    free(glyph.bits);
    glyph.bits=NULL;
  }
  glyph.metrics.leftSideBearing=0;
  glyph.metrics.rightSideBearing=0;
  glyph.metrics.advanceX=0;
  glyph.metrics.advanceY=0;
  glyph.metrics.ascent=0;
  glyph.metrics.descent=0;
  glyph.pFontCacheInfo=NULL;
  glyph.bpp=1;

  /* Assign dimens */
  lsb1=glyph1->metrics.leftSideBearing;
  rsb1=glyph1->metrics.rightSideBearing;
  ascent1=glyph1->metrics.ascent;
  descent1=glyph1->metrics.descent;
  advanceX1=glyph1->metrics.advanceX + x_off;
  advanceY1=glyph1->metrics.advanceY + y_off;
  
  lsb2=glyph2->metrics.leftSideBearing;
  rsb2=glyph2->metrics.rightSideBearing;
  ascent2=glyph2->metrics.ascent;
  descent2=glyph2->metrics.descent;
  advanceX2=glyph2->metrics.advanceX;
  advanceY2=glyph2->metrics.advanceY;
  
  lsb=lsb1 < lsb2+advanceX1 ? lsb1 : lsb2+advanceX1;
  rsb=rsb1 > rsb2+advanceX1 ? rsb1 : rsb2+advanceX1;
  ascent=ascent1 > ascent2+advanceY1 ? ascent1 : ascent2+advanceY1;
  descent=descent1 < descent2+advanceY1 ? descent1 : descent2+advanceY1;
  advanceX=advanceX1+advanceX2;
  advanceY=advanceY1+advanceY2;

  bpp=glyph1->bpp;
  w=rsb-lsb;
  h=ascent-descent;
  wscanline=PAD( w*bpp, T1_pad) / 8;
  wscanline1=PAD( (rsb1-lsb1)*bpp, T1_pad) / 8;
  wscanline2=PAD( (rsb2-lsb2)*bpp, T1_pad) / 8;
  memsize=wscanline*h;
  if ((glyph.bits=(char *)calloc( memsize + 1, sizeof(unsigned char)))==NULL){
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  if (bpp==1){
    if (T1_pad==32 && T1_byte==0 && sizeof(long)==8){
      /* The following loop steps through the lines of the first glyph: */
      vanchor=ascent-ascent1;
      BitShift=(lsb1-lsb) % 32;
      ByteOffset=(lsb1-lsb)/32*4;
      for ( i=0; i<ascent1-descent1; i++){
	Target_l= (unsigned T1_AA_TYPE32 *)(glyph.bits +((vanchor+i)*wscanline)
					    +ByteOffset);
	p_l = (unsigned T1_AA_TYPE32 *)(glyph1->bits+(wscanline1*i));
	if (BitShift == 0){
	  for (k=wscanline1/4; k; k--)
	    *Target_l++ |= *p_l++;
	}
	else{
	  for (k=wscanline1/4; k; k--){
	    BitBuf_l = ((long)*p_l++) << BitShift;
	    *Target_l++ |= BitBuf_l;
	    *Target_l |= BitBuf_l>>l_shift;
	  }
	}
      }
      /* The following loop steps through the lines of the second glyph: */
      vanchor=ascent-(ascent2+advanceY1);
      BitShift=(lsb2+advanceX1-lsb) % 32;
      ByteOffset=(lsb2+advanceX1-lsb)/32*4;
      for ( i=0; i<ascent2-descent2; i++){
	Target_l= (unsigned T1_AA_TYPE32 *)(glyph.bits +((vanchor+i)*wscanline)
					    +ByteOffset);
	p_l = (unsigned T1_AA_TYPE32 *)(glyph2->bits+(wscanline2*i));
	if (BitShift == 0){
	  for (k=wscanline2/4; k; k--)
	    *Target_l++ |= *p_l++;
	}
	else{
	  for (k=wscanline2/4; k; k--){
	    BitBuf_l = ((long)*p_l++) << BitShift;
	    *Target_l++ |= BitBuf_l;
	    *Target_l |= BitBuf_l>>l_shift;
	  } 
	} 
      }
    }
    else if(T1_pad==16 && T1_byte==0){
      /* The following loop steps through the lines of the first glyph: */
      vanchor=ascent-ascent1;
      BitShift=(lsb1-lsb) % 16;
      ByteOffset=(lsb1-lsb)/16*2;
      for ( i=0; i<ascent1-descent1; i++){
	Target_s= (unsigned T1_AA_TYPE16 *)(glyph.bits +((vanchor+i)*wscanline)
					    +ByteOffset);
	p_s = (unsigned T1_AA_TYPE16 *)(glyph1->bits+(wscanline1*i));
	if (BitShift == 0){
	  for (k=wscanline1/2; k; k--)
	    *Target_s++ |= *p_s++;
	}
	else{
	  for (k=wscanline1/2; k; k--){
	    BitBuf_s = ((T1_AA_TYPE32)*p_s++) << BitShift;
	    *Target_s++ |= BitBuf_s;
	    *Target_s |= BitBuf_s>>s_shift;
	  }
	}
      }
      /* The following loop steps through the lines of the second glyph: */
      vanchor=ascent-(ascent2+advanceY1);
      BitShift=(lsb2+advanceX1-lsb) % 16;
      ByteOffset=(lsb2+advanceX1-lsb)/16*2;
      for ( i=0; i<ascent2-descent2; i++){
	Target_s= (unsigned T1_AA_TYPE16 *)(glyph.bits +((vanchor+i)*wscanline)
					    +ByteOffset);
	p_s = (unsigned T1_AA_TYPE16 *)(glyph2->bits+(wscanline2*i));
	if (BitShift == 0){
	  for (k=wscanline2/2; k; k--)
	    *Target_s++ |= *p_s++;
	}
	else{
	  for (k=wscanline2/2; k; k--){
	    BitBuf_s = ((T1_AA_TYPE32)*p_s++) << BitShift;
	    *Target_s++ |= BitBuf_s;
	    *Target_s |= BitBuf_s>>s_shift;
	  } 
	} 
      }
    }
    else{ /* T1_pad==8 or Big Endian machine */
      /* The following loop steps through the lines of the first glyph: */
      vanchor=ascent-ascent1;
      BitShift=(lsb1-lsb) % 8;
      ByteOffset=(lsb1-lsb) / 8;
      for ( i=0; i<ascent1-descent1; i++){
	Target_c= (unsigned char *)(glyph.bits +((vanchor+i)*wscanline)
				    +ByteOffset);
	p_c = (unsigned char *)(glyph1->bits+(wscanline1*i));
	if (BitShift == 0){
	  for (k=wscanline1; k; k--)
	    *Target_c++ |= *p_c++;
	}
	else{
	  for (k=wscanline1; k; k--){
	    BitBuf_c = ((T1_AA_TYPE16)*p_c++) << BitShift;
	    *Target_c++ |= BitBuf_c;
	    *Target_c |= BitBuf_c>>c_shift;
	  }
	}
      }
      /* The following loop steps through the lines of the second glyph: */
      vanchor=ascent-(ascent2+advanceY1);
      BitShift=(lsb2+advanceX1-lsb) % 8;
      ByteOffset=(lsb2+advanceX1-lsb) / 8;
      for ( i=0; i<ascent2-descent2; i++){
	Target_c= (unsigned char *)(glyph.bits +((vanchor+i)*wscanline)
				    +ByteOffset);
	p_c = (unsigned char *)(glyph2->bits+(wscanline2*i));
	if (BitShift == 0){
	  for (k=wscanline2; k; k--)
	    *Target_c++ |= *p_c++;
	}
	else{
	  for (k=wscanline2; k; k--){
	    BitBuf_c = ((T1_AA_TYPE16)*p_c++) << BitShift;
	    *Target_c++ |= BitBuf_c;
	    *Target_c |= BitBuf_c>>c_shift;
	  } 
	} 
      }
    }
  } /* end of if (bpp==1) ... */
  else if (bpp==8){
    /* Set background */
    for ( i=0; i<memsize; i++)
      ((char *)glyph.bits)[i]=(char) T1aa_bg;
    /* The following loop steps through the lines of the first glyph: */
    vanchor=ascent-ascent1;
    for ( i=0; i<ascent1-descent1; i++){
      Target_c= (unsigned char *)(glyph.bits +((vanchor+i)*wscanline)
				  +(lsb1-lsb));
      p_c = (unsigned char *)(glyph1->bits+(wscanline1*i));
      memcpy( Target_c, p_c, (rsb1-lsb1));
    }
    /* The following loop steps through the lines of the second glyph.
       Note that we only set the pixel if it is not background! */
    vanchor=ascent-(ascent2+advanceY1);
    for ( i=0; i<ascent2-descent2; i++){
      Target_c= (unsigned char *)(glyph.bits +((vanchor+i)*wscanline)
				  +(lsb2+advanceX1-lsb));
      p_c = (unsigned char *)(glyph2->bits+(wscanline2*i));
      for (j=0; j<(rsb2-lsb2); j++)
	if (p_c[j] != (unsigned char) T1aa_bg)
	  Target_c[j]=p_c[j];
    }
  } /* end of if (bpp==8) ... */
  else if (bpp==16){
    /* Set background */
    for ( i=0; i<memsize/2; i++)
      ((T1_AA_TYPE16 *)glyph.bits)[i]=(T1_AA_TYPE16) T1aa_bg;
    /* The following loop steps through the lines of the first glyph: */
    vanchor=ascent-ascent1;
    for ( i=0; i<ascent1-descent1; i++){
      Target_s= (unsigned T1_AA_TYPE16 *)(glyph.bits +((vanchor+i)*wscanline)
				  +(lsb1-lsb)*2);
      p_s = (unsigned T1_AA_TYPE16 *)(glyph1->bits+(wscanline1*i));
      memcpy( Target_s, p_s, (rsb1-lsb1)*2);
    }
    /* The following loop steps through the lines of the second glyph.
       Note that we only set the pixel if it is not background! */
    vanchor=ascent-(ascent2+advanceY1);
    for ( i=0; i<ascent2-descent2; i++){
      Target_s= (unsigned T1_AA_TYPE16 *)(glyph.bits +((vanchor+i)*wscanline)
				  +(lsb2+advanceX1-lsb)*2);
      p_s = (unsigned T1_AA_TYPE16 *)(glyph2->bits+(wscanline2*i));
      for (j=0; j<(rsb2-lsb2); j++)
	if (p_s[j] != (unsigned T1_AA_TYPE16) T1aa_bg)
	  Target_s[j]=p_s[j];
    }
  } /* end of if (bpp==16) ... */
  else if (bpp==32){
    /* Set background */
    for ( i=0; i<memsize/4; i++)
      ((T1_AA_TYPE32 *)glyph.bits)[i]=(T1_AA_TYPE32) T1aa_bg;
    /* The following loop steps through the lines of the first glyph: */
    vanchor=ascent-ascent1;
    for ( i=0; i<ascent1-descent1; i++){
      Target_l= (unsigned T1_AA_TYPE32 *)(glyph.bits +((vanchor+i)*wscanline)
				  +(lsb1-lsb)*4);
      p_l = (unsigned T1_AA_TYPE32 *)(glyph1->bits+(wscanline1*i));
      memcpy( Target_l, p_l, (rsb1-lsb1)*4);
    }
    /* The following loop steps through the lines of the second glyph.
       Note that we only set the pixel if it is not background! */
    vanchor=ascent-(ascent2+advanceY1);
    for ( i=0; i<ascent2-descent2; i++){
      Target_l= (unsigned T1_AA_TYPE32 *)(glyph.bits +((vanchor+i)*wscanline)
				  +(lsb2+advanceX1-lsb)*4);
      p_l = (unsigned T1_AA_TYPE32 *)(glyph2->bits+(wscanline2*i));
      for (j=0; j<(rsb2-lsb2); j++)
	if (p_l[j] != (unsigned T1_AA_TYPE32) T1aa_bg)
	  Target_l[j]=p_l[j];
    }
  } /* end of if (bpp==32) ... */
  
  
  /* Check for writing direction and re-compute dimens appropriately: */
  if (modflag & T1_RIGHT_TO_LEFT){
    advanceX=-advanceX1-advanceX2;
    advanceY=-advanceY1-advanceY2;
    lsb=lsb1 < lsb2+advanceX1 ? advanceX+lsb1 : advanceX+lsb2+advanceX1;
    rsb=rsb1 > rsb2+advanceX1 ? advanceX+rsb1 : advanceX+rsb2+advanceX1;
    ascent=ascent1 > ascent2+advanceY1 ? ascent1 : ascent2+advanceY1;
    descent=descent1 < descent2+advanceY1 ? descent1 : descent2+advanceY1;
    ascent += advanceY;
    descent += advanceY;
  } 
  
  glyph.metrics.leftSideBearing=lsb;
  glyph.metrics.rightSideBearing=rsb;
  glyph.metrics.advanceX=advanceX;
  glyph.metrics.advanceY=advanceY;
  glyph.metrics.ascent=ascent;
  glyph.metrics.descent=descent;
  glyph.bpp=bpp;
  
  /*
  printf("lsb=%d, rsb=%d, adX=%d, adY=%d asc=%d, desc=%d\n",
	 glyph.metrics.leftSideBearing,
	 glyph.metrics.rightSideBearing,
	 glyph.metrics.advanceX,
	 glyph.metrics.advanceY,
	 glyph.metrics.ascent,
	 glyph.metrics.descent );
	 */
  
  return( &glyph);
  
}



/* T1_FillOutline(): Create a filled glyph from an outline description */
GLYPH *T1_FillOutline( T1_OUTLINE *path, int modflag)
{
  struct region *area=NULL;

  static GLYPH glyph={NULL,{0,0,0,0,0,0},NULL,1};
  volatile int memsize=0;
  int i;
  LONG h,w;
  LONG paddedW;

  
  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_FillOutline()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }

  /* Reset character glyph, if necessary */
  if (glyph.bits!=NULL){
    free(glyph.bits);
    glyph.bits=NULL;
  }
  glyph.metrics.leftSideBearing=0;
  glyph.metrics.rightSideBearing=0;
  glyph.metrics.advanceX=0;
  glyph.metrics.advanceY=0;
  glyph.metrics.ascent=0;
  glyph.metrics.descent=0;
  glyph.pFontCacheInfo=NULL;
  glyph.bpp=1;  
  
  
  /* Assign padding value */
  T1_pad=pFontBase->bitmap_pad;
  if (pFontBase->endian)
    T1_byte=1;
  else
    T1_byte=0;
  T1_wordsize=T1_pad;

  /* Create a region from outline */
  area=(struct region *)Interior( (struct segment *)path,
				  WINDINGRULE+CONTINUITY);

  /* fill the glyph-structure */
  if (area == NULL){
    T1_PrintLog( "T1_FillOutline()", "area=NULL returned by Interior()", T1LOG_WARNING);
    T1_errno=1000;
    return(NULL);
  }
  h = area->ymax - area->ymin;
  w = area->xmax - area->xmin;

  
  paddedW = PAD(w, T1_pad);
  if ( (area->xmin > area->xmax) || (area->ymin > area->ymax) ){
    /* There was a character like .notdef or space, that didn't
       produce any black pixels on the bitmap! -> we return a glyph with
       correct metrics and bitmap pointer set to NULL */
    sprintf( err_warn_msg_buf,
	     "No black pixels in outline %p", path);
    T1_PrintLog( "T1_FillOutline()", err_warn_msg_buf, T1LOG_WARNING);
    
    glyph.metrics.leftSideBearing  = 0;
    glyph.metrics.advanceX   = NEARESTPEL(area->ending.x - area->origin.x);
    glyph.metrics.advanceY   = - NEARESTPEL(area->ending.y - area->origin.y);
    glyph.metrics.rightSideBearing = 0;
    glyph.metrics.descent          = 0;
    glyph.metrics.ascent           = 0;
    /* make sure to get rid of 'area' before leaving! */
    KillRegion (area);
    return( &glyph);
  }
  if (h > 0 && w > 0) {
    memsize = h * paddedW / 8 + 1;
    /* This is for the users copy of the character, for security-reasons
       the original pointer to the cache area is not used. The entry glyph.bits
       is free'ed every time this function is called: */
    glyph.bits = (char *)malloc(memsize*sizeof( char));
    if (glyph.bits == NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      /* make sure to get rid of area if it's there */
      if (area){
	KillRegion (area);
      }
      return(NULL);
    }
    
  }
  else {
    h = w = 0;
    area->xmin = area->xmax = 0;
    area->ymin = area->ymax = 0;
  }
  
  glyph.metrics.leftSideBearing  = area->xmin;
  glyph.metrics.advanceX   = NEARESTPEL(area->ending.x - area->origin.x);
  glyph.metrics.advanceY   = - NEARESTPEL(area->ending.y - area->origin.y);
  glyph.metrics.rightSideBearing = area->xmax;
  glyph.metrics.descent          = - area->ymax;
  glyph.metrics.ascent           = - area->ymin;
  
  
  if (h > 0 && w > 0) {
    (void) memset(glyph.bits, 0, memsize);
    fill(glyph.bits, h, paddedW, area, T1_byte, T1_bit, T1_wordsize );
  }

  /* Check for writing direction and re-compute dimensions appropriately: */
  if (modflag & T1_RIGHT_TO_LEFT){
    glyph.metrics.advanceX *= -1;
    glyph.metrics.advanceY *= -1;
    glyph.metrics.leftSideBearing += glyph.metrics.advanceX;
    glyph.metrics.rightSideBearing += glyph.metrics.advanceX;
    glyph.metrics.descent += glyph.metrics.advanceY;
    glyph.metrics.ascent += glyph.metrics.advanceY;
  } 
  
  /* make sure to get rid of area if it's there */
  if (area){
    KillRegion (area);
  }
  return( &glyph);
  
  
}


/* T1_SetRect(): Raster a rectangle, whose size is given in charspace units.
   The resulting glyph does not cause any escapement. */
GLYPH* T1_SetRect( int FontID, float size,
		   float width, float height, T1_TMATRIX *transform)
{
  int i;
  int mode;
  struct region *area;
  struct XYspace *Current_S;
  float strokewidth = 0.0f;
  
  FONTSIZEDEPS *font_ptr;
  FONTPRIVATE  *fontarrayP;
  
  volatile int memsize=0;
  LONG h,w;
  LONG paddedW;

  static GLYPH glyph={NULL,{0,0,0,0,0,0},NULL,1};


  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_SetRect()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }

  font_ptr = NULL;
  
  /* Reset character glyph, if necessary */
  if (glyph.bits!=NULL){
    free(glyph.bits);
    glyph.bits=NULL;
  }
  glyph.metrics.leftSideBearing=0;
  glyph.metrics.rightSideBearing=0;
  glyph.metrics.advanceX=0;
  glyph.metrics.advanceY=0;
  glyph.metrics.ascent=0;
  glyph.metrics.descent=0;
  glyph.pFontCacheInfo=NULL;
  glyph.bpp=1;  
  
  /* First, check for a correct ID. */
  i=T1_CheckForFontID(FontID);
  if ( i == -1 ) {
    return NULL;
  }
  /* if necessary load font into memory */
  if ( i == 0 )
    if ( T1_LoadFont( FontID) )
      return NULL;

  /* Check for valid size */
  if (size<=0.0){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  }

  /* Assign padding value */
  T1_pad=pFontBase->bitmap_pad;
  if (pFontBase->endian)
    T1_byte=1;
  else
    T1_byte=0;
  T1_wordsize=T1_pad;

  if ( i > 0 ) {
    /* FontID identifies a valid font */
    fontarrayP = &(pFontBase->pFontArray[FontID]);
    
    /* Check for size and create it if necessary */
    if ((font_ptr=T1int_QueryFontSize( FontID, size, NO_ANTIALIAS))==NULL){
      font_ptr=T1int_CreateNewFontSize( FontID, size, NO_ANTIALIAS);
      if (font_ptr==NULL){
	T1_errno=T1ERR_ALLOC_MEM;
	return(NULL);
      }
    }
    
    /* handle stroking stuff */
    if ( fontarrayP->info_flags & RASTER_STROKED) {
      strokewidth = pFontBase->pFontArray[FontID].StrokeWidth;
    }
    else {
      strokewidth = 0.0f;
    }
  }
  else {
    fontarrayP = NULL;
    strokewidth = 0.0f;
  }
  
  
  /* Setup an appropriate charspace matrix. Note that the rasterizer
     assumes vertical values with inverted sign! Transformation should
     create a copy of the local charspace matrix which then still has
     to be made permanent. */
  if ( transform != NULL ) {
    Current_S = (struct XYspace *) 
      Permanent(Scale(Transform (font_ptr->pCharSpaceLocal,
				 transform->cxx, - transform->cxy,
				 transform->cyx, - transform->cyy),
		      DeviceSpecifics.scale_x, DeviceSpecifics.scale_y));
  }
  else{
    Current_S = (struct XYspace *)
      Permanent(Scale(Transform(font_ptr->pCharSpaceLocal,
				1.0, 0.0, 0.0, -1.0),
		      DeviceSpecifics.scale_x, DeviceSpecifics.scale_y));
  }
  
  mode=0;
  area=fontfcnRect( width,
		    height,
		    Current_S,
		    &mode,
		    DO_RASTER,
		    strokewidth);
  KillSpace (Current_S);
  
  /* fill the glyph-structure */
  if ( mode > 0 ) {
    sprintf( err_warn_msg_buf, "fontfcnRect() set mode=%d", mode);
    T1_PrintLog( "T1_SetRect()", err_warn_msg_buf, T1LOG_WARNING);
    T1_errno=mode;
    return(NULL);
  }
  if ( area == NULL ) {
    T1_PrintLog( "T1_SetRect()", "area=NULL returned by fontfcnRect()", T1LOG_WARNING);
    T1_errno=mode;
    return(NULL);
  }
  h = area->ymax - area->ymin;
  w = area->xmax - area->xmin;

  paddedW = PAD(w, T1_pad);
  
  if (h > 0 && w > 0) {
    memsize = h * paddedW / 8 + 1;
    /* This is for the users copy of the character, for security-reasons
       the original pointer to the cache area is not used. The entry glyph.bits
       is free'ed every time this function is called: */
    glyph.bits = (char *)malloc(memsize*sizeof( char));
    if ( glyph.bits == NULL ) {
      T1_errno=T1ERR_ALLOC_MEM;
      /* make sure to get rid of 'area' before leaving! */
      KillRegion (area);
      return(NULL);
    }
  }
  else {
    h = w = 0;
    area->xmin = area->xmax = 0;
    area->ymin = area->ymax = 0;
  }

  /* Assign metrics */
  glyph.metrics.leftSideBearing  = area->xmin;
  glyph.metrics.advanceX   = NEARESTPEL(area->ending.x - area->origin.x);
  glyph.metrics.advanceY   = - NEARESTPEL(area->ending.y - area->origin.y);
  glyph.metrics.rightSideBearing = area->xmax;
  glyph.metrics.descent          = - area->ymax;
  glyph.metrics.ascent           = - area->ymin;

  
  if (h > 0 && w > 0) {
    (void) memset(glyph.bits, 0, memsize);
    fill(glyph.bits, h, paddedW, area, T1_byte, T1_bit, T1_wordsize );
  }
  
  /* make sure to get rid of 'area' before leaving! */
  KillRegion (area);
  
  return(&glyph);
}
