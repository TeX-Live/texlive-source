/*--------------------------------------------------------------------------
  ----- File:        t1outline.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-01
  ----- Description: This file is part of the t1-library. It contains
                     functions for getting glyph outline descriptions of
		     strings and characters.
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
  
#define T1OUTLINE_C


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
#include "t1outline.h"


/* As a fall back */
#ifndef T1_AA_TYPE16 
#define T1_AA_TYPE16    short
#endif
#ifndef T1_AA_TYPE32 
#define T1_AA_TYPE32    int
#endif


extern char *t1_get_abort_message( int number);
extern struct segment *Type1Line(psfont *env, struct XYspace *S,
				 float line_position,
				 float line_thickness,
				 float line_length);
extern struct segment *t1_Join( struct segment *path1,
				struct segment *path2);
extern struct segment *CopyPath( struct segment *p);
extern void KillPath( struct segment *p);




/* T1_SetChar(...): Generate the bitmap for a character */
T1_OUTLINE *T1_GetCharOutline( int FontID, char charcode, float size,
			       T1_TMATRIX *transform)
{
  int i;
  int mode;
  T1_PATHSEGMENT *charpath;
  struct XYspace *Current_S;
  unsigned char ucharcode;
  
  
  FONTSIZEDEPS *font_ptr;
  FONTPRIVATE  *fontarrayP;
  
  /* We don't implement underlining for characters, but the rasterer
     implements it. Thus, we use a modflag of constant 0 */
  int modflag=0;

  
  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_GetCharOutline()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }

  ucharcode=(unsigned char)charcode;

  
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

  fontarrayP=&(pFontBase->pFontArray[FontID]);
  
  /* font is now loaded into memory =>
     Check for size: */
  if ((font_ptr=T1int_QueryFontSize( FontID, size, NO_ANTIALIAS))==NULL){
    font_ptr=T1int_CreateNewFontSize( FontID, size, NO_ANTIALIAS);
    if (font_ptr==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
  }
  
  /* Setup an appropriate charspace matrix. Note that the rasterizer
     assumes vertical values with inverted sign! Transformation should
     create a copy of the local charspace matrix which then still has
     to be made permanent. */
  if (transform!=NULL) {
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
  charpath=(T1_PATHSEGMENT *)fontfcnB( FontID, modflag, Current_S,
				       fontarrayP->pFontEnc,
				       ucharcode, &mode,
				       fontarrayP->pType1Data,
				       DO_NOT_RASTER,0.0f);
  KillSpace (Current_S);

  return((T1_OUTLINE *)charpath);
}



/* T1_GetStringOutline(...): Generate the outline for a string of
                             characters */
T1_OUTLINE *T1_GetStringOutline( int FontID, char *string, int len, 
				 long spaceoff, int modflag, float size,
				 T1_TMATRIX *transform)
{
  int i;
  int mode;
  /* initialize this to NULL just to be on the safe side */
  T1_PATHSEGMENT *charpath = NULL;
  struct XYspace *Current_S;
  int *kern_pairs;       /* use for accessing the kern pairs if kerning is
			    requested */
  int no_chars=0;        /* The number of characters in the string */
  static int lastno_chars=0;
  long spacewidth;       /* This is given to fontfcnb_string() */
  
  
  FONTSIZEDEPS *font_ptr;
  FONTPRIVATE  *fontarrayP;
  
  static int *pixel_h_anchor_corr=NULL;
  static int *flags=NULL;

  unsigned char *ustring;


  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_GetStringOutline()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }

  /* force string elements into unsigned */
  ustring=(unsigned char*)string;
  
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

  fontarrayP=&(pFontBase->pFontArray[FontID]);
  
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
  
  if (len==0) /* should be computed assuming "normal" 0-terminated string */
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
  if (transform!=NULL){
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
  
  mode=0;
  kern_pairs=(int *)calloc(no_chars, sizeof(int));
  if ((modflag & T1_KERNING))
    for (i=0; i<no_chars -1; i++)
      kern_pairs[i]=T1_GetKerning( FontID, ustring[i], ustring[i+1]);
  charpath=(T1_PATHSEGMENT *) fontfcnB_string( FontID, modflag, Current_S,
					       fontarrayP->pFontEnc,
					       (unsigned char *)string,
					       no_chars, &mode,
					       fontarrayP->pType1Data,
					       kern_pairs, spacewidth,
					       DO_NOT_RASTER,0.0f);
  KillSpace (Current_S);
  
  /* In all cases, free memory for kerning pairs */
  free(kern_pairs);
  
  /* fill the string_glyph-structure */
  if (mode != 0) {
    sprintf( err_warn_msg_buf, "fontfcnB_string() set mode=%d", mode);
    T1_PrintLog( "T1_GetStringOutline()", err_warn_msg_buf, T1LOG_WARNING);
    T1_errno=mode;
    /* make sure to get rid of path if it's there */
    if (charpath){
      KillRegion (charpath);
    }
    return(NULL);
  }
  if (charpath == NULL){
    T1_PrintLog( "T1_GetStringOutline()", "path=NULL returned by fontfcnB_string()", T1LOG_WARNING);
    T1_errno=mode;
    return(NULL);
  }
  
  return( (T1_OUTLINE *)charpath);
}


/* T1_GetMoveOutline(...): Generate the "outline" for a movement
                           */
T1_OUTLINE *T1_GetMoveOutline( int FontID, int deltax, int deltay, int modflag,
			       float size, T1_TMATRIX *transform)
{
  int i;
  FONTSIZEDEPS *font_ptr;
  struct segment *path, *tmppath;
  struct XYspace *Current_S;
  psfont *FontP;
  float length;
  

  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_GetMoveOutline()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }


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

  FontP=pFontBase->pFontArray[i].pType1Data;
  
  /* font is now loaded into memory =>
     Check for size: */
  if ((font_ptr=T1int_QueryFontSize( FontID, size, NO_ANTIALIAS))==NULL){
    font_ptr=T1int_CreateNewFontSize( FontID, size, NO_ANTIALIAS);
    if (font_ptr==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(NULL);
    }
  }

  /* Setup an appropriate charspace matrix. Note that the rasterizer
     assumes vertical values with inverted sign! Transformation should
     create a copy of the local charspace matrix which then still has
     to be made permanent. */
  if (transform!=NULL){
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
  
  
  path=(struct segment *)ILoc( Current_S, deltax, deltay); 

  /* Take care for underlining and such */
  length=(float) deltax;
  if (modflag & T1_UNDERLINE){
    tmppath=(struct segment *)Type1Line(FontP,Current_S,
					pFontBase->pFontArray[FontID].UndrLnPos,
					pFontBase->pFontArray[FontID].UndrLnThick,
					length);
    path=(struct segment *)Join(path,tmppath);
  }
  if (modflag & T1_OVERLINE){
    tmppath=(struct segment *)Type1Line(FontP,Current_S,
					pFontBase->pFontArray[FontID].OvrLnPos,
					pFontBase->pFontArray[FontID].OvrLnThick,
					length);
    path=(struct segment *)Join(path,tmppath);
  }
  if (modflag & T1_OVERSTRIKE){
    tmppath=(struct segment *)Type1Line(FontP,Current_S,
					pFontBase->pFontArray[FontID].OvrStrkPos,
					pFontBase->pFontArray[FontID].OvrStrkThick,
					length);
    path=(struct segment *)Join(path,tmppath);
  }
      
  KillSpace( Current_S);
  
  return( (T1_OUTLINE *)path);
  
}


/* T1_ConcatOutlines(): Concatenate two outline to one path */
T1_OUTLINE *T1_ConcatOutlines( T1_OUTLINE *path1, T1_OUTLINE *path2)
{

  return( (T1_OUTLINE *) t1_Join( (struct segment *) path1,
				  (struct segment *) path2));
  
}


/* T1_ScaleOutline(): Scale an outline */
T1_OUTLINE *T1_ScaleOutline( T1_OUTLINE *path, float scale)
{
  struct segment *ipath;

  /* Step through the path list */
  ipath=(struct segment *)path;

  do {
    if (ipath->type==LINETYPE){
      ipath->dest.x = (fractpel) (ipath->dest.x * scale);
      ipath->dest.y = (fractpel) (ipath->dest.y * scale);
    }
    if (ipath->type==CONICTYPE){
      ((struct conicsegment *)ipath)->dest.x =
	(fractpel) ((struct conicsegment *)ipath)->dest.x * scale;
      ((struct conicsegment *)ipath)->dest.y =
	(fractpel) ((struct conicsegment *)ipath)->dest.y * scale;
      ((struct conicsegment *)ipath)->M.x =
	(fractpel) ((struct conicsegment *)ipath)->M.x * scale;
      ((struct conicsegment *)ipath)->M.y =
	(fractpel) ((struct conicsegment *)ipath)->M.y * scale;
    }
    if (ipath->type==MOVETYPE){
      ipath->dest.x = (fractpel) (ipath->dest.x * scale);
      ipath->dest.y = (fractpel) (ipath->dest.y * scale);
    }
    if (ipath->type==BEZIERTYPE){
      ((struct beziersegment *)ipath)->B.x =
	(fractpel) (((struct beziersegment *)ipath)->B.x * scale);
      ((struct beziersegment *)ipath)->B.y =
	(fractpel) (((struct beziersegment *)ipath)->B.y * scale);
      ((struct beziersegment *)ipath)->C.x =
	(fractpel) (((struct beziersegment *)ipath)->C.x * scale);
      ((struct beziersegment *)ipath)->C.y =
	(fractpel) (((struct beziersegment *)ipath)->C.y * scale);
      ((struct beziersegment *)ipath)->dest.x =
	(fractpel) (((struct beziersegment *)ipath)->dest.x * scale);
      ((struct beziersegment *)ipath)->dest.y =
	(fractpel) (((struct beziersegment *)ipath)->dest.y * scale);
    }
    if (ipath->type==HINTTYPE){
      ((struct hintsegment *)ipath)->dest.x =
	(fractpel) (((struct hintsegment *)ipath)->dest.x * scale);
      ((struct hintsegment *)ipath)->dest.y =
	(fractpel) (((struct hintsegment *)ipath)->dest.y * scale);
      ((struct hintsegment *)ipath)->ref.x =
	(fractpel) (((struct hintsegment *)ipath)->ref.x * scale);
      ((struct hintsegment *)ipath)->ref.y =
	(fractpel) (((struct hintsegment *)ipath)->ref.y * scale);
      ((struct hintsegment *)ipath)->width.x =
	(fractpel) (((struct hintsegment *)ipath)->width.x * scale);
      ((struct hintsegment *)ipath)->width.y =
	(fractpel) (((struct hintsegment *)ipath)->width.y * scale);
    }
    if (ipath->type==TEXTTYPE)
      ;
    ipath=ipath->link;

  } while (ipath!=NULL);
  
  return( path);
  
}


/* dump a description of path elements to stdout */
void T1_DumpPath( T1_OUTLINE *path)
{

  struct segment *ipath;

  /* Step through the path list */
  ipath=(struct segment *)path;

  do {
    if (ipath->type==LINETYPE){
      printf("Line-Segment: -> (%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF));
    }
    if (ipath->type==MOVETYPE){
      printf("Move-Segment: -> (%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF));
    }
    if (ipath->type==BEZIERTYPE){
      printf("Bezier-Segment: ... (%f,%f) ... (%f,%f) -> (%f,%f)\n",
	     (((struct beziersegment *)ipath)->B.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->B.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->C.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->C.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->dest.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->dest.y)/(double)(0xFFFF));
    }
    ipath=ipath->link;
  } while (ipath!=NULL);
  
}


/* convert a relative path to an absolute path */
void T1_AbsolutePath( T1_OUTLINE *rpath)
{

  struct segment *ipath;
  fractpel xcurr=0, ycurr=0;
  
  /* Step through the path list */
  ipath=(struct segment *)rpath;

  do {
    if (ipath->type==LINETYPE){
      ipath->dest.x +=xcurr;
      ipath->dest.y +=ycurr;
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Line-Segment: -> (%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF));
#endif
      xcurr=ipath->dest.x;
      ycurr=ipath->dest.y;
    }
    if (ipath->type==MOVETYPE){
      ipath->dest.x +=xcurr;
      ipath->dest.y +=ycurr;
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Move-Segment: -> (%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF));
#endif
      xcurr=ipath->dest.x;
      ycurr=ipath->dest.y;
    }
    if (ipath->type==BEZIERTYPE){
      ((struct beziersegment *)ipath)->B.x +=xcurr;
      ((struct beziersegment *)ipath)->B.y +=ycurr;
      ((struct beziersegment *)ipath)->C.x +=xcurr;
      ((struct beziersegment *)ipath)->C.y +=ycurr;
      ipath->dest.x +=xcurr;
      ipath->dest.y +=ycurr;
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Bezier-Segment: ... (%f,%f) ... (%f,%f) -> (%f,%f)\n",
	     (((struct beziersegment *)ipath)->B.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->B.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->C.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->C.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->dest.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->dest.y)/(double)(0xFFFF));
#endif
      xcurr=ipath->dest.x;
      ycurr=ipath->dest.y;
    }
    ipath=ipath->link;
  } while (ipath!=NULL);
  
}



/* convert path into a relative path as needed by the rasterizer */
void T1_RelativePath( T1_OUTLINE *apath)
{
  
  struct segment *ipath;
  fractpel xcurr=0, ycurr=0;
  
  
  /* Step through the path list */
  ipath=(struct segment *)apath;
  
  do {
    if (ipath->type==LINETYPE){
      ipath->dest.x -=xcurr;
      ipath->dest.y -=ycurr;
      xcurr +=ipath->dest.x;
      ycurr +=ipath->dest.y;
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Line-Segment: -> (%f,%f), curr=(%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF),
	     xcurr/(double)(0xFFFF), -ycurr/(double)(0xFFFF));
#endif
    }
    if (ipath->type==MOVETYPE){
      ipath->dest.x -=xcurr;
      ipath->dest.y -=ycurr;
      xcurr +=ipath->dest.x;
      ycurr +=ipath->dest.y;
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Move-Segment: -> (%f,%f), curr=(%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF),
	     xcurr/(double)(0xFFFF), -ycurr/(double)(0xFFFF));
#endif
    }
    if (ipath->type==BEZIERTYPE){
      ipath->dest.x -=xcurr;
      ipath->dest.y -=ycurr;
      ((struct beziersegment *)ipath)->C.x -=xcurr;
      ((struct beziersegment *)ipath)->C.y -=ycurr;
      ((struct beziersegment *)ipath)->B.x -=xcurr;
      ((struct beziersegment *)ipath)->B.y -=ycurr;
      xcurr +=ipath->dest.x;
      ycurr +=ipath->dest.y;
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Bezier-Segment: ... (%f,%f) ... (%f,%f) -> (%f,%f)\n",
	     (((struct beziersegment *)ipath)->B.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->B.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->C.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->C.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->dest.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->dest.y)/(double)(0xFFFF));
#endif
    }
    ipath=ipath->link;
  } while (ipath!=NULL);
  
}


/* maniuplate the coordinata points of a path */
void T1_ManipulatePath( T1_OUTLINE *path,
			void (*manipulate)(fractpel *x,fractpel *y,int type))
{

  struct segment *ipath;
  
  
  /* Step through the path list */
  ipath=(struct segment *)path;

  do {
    if (ipath->type==LINETYPE){
      manipulate( &(ipath->dest.x), &ipath->dest.y, LINETYPE);
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Line-Segment: -> (%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF));
#endif
    }
    if (ipath->type==MOVETYPE){
      manipulate( &(ipath->dest.x), &ipath->dest.y, MOVETYPE);
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Move-Segment: -> (%f,%f)\n",
	     (ipath->dest.x)/(double)(0xFFFF),
	     -(ipath->dest.y)/(double)(0xFFFF));
#endif
    }
    if (ipath->type==BEZIERTYPE){
      manipulate( &((struct beziersegment *)ipath)->B.x,
		  &((struct beziersegment *)ipath)->B.y,
		  BEZIERTYPE);
      manipulate( &((struct beziersegment *)ipath)->C.x,
		  &((struct beziersegment *)ipath)->C.y,
		  BEZIERTYPE);
      manipulate( &((struct beziersegment *)ipath)->dest.x,
		  &((struct beziersegment *)ipath)->dest.y,
		  BEZIERTYPE);
#ifdef PRINT_PATHS_TO_STDOUT      
      printf("Bezier-Segment: ... (%f,%f) ... (%f,%f) -> (%f,%f)\n",
	     (((struct beziersegment *)ipath)->B.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->B.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->C.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->C.y)/(double)(0xFFFF),
	     (((struct beziersegment *)ipath)->dest.x)/(double)(0xFFFF),
	     -(((struct beziersegment *)ipath)->dest.y)/(double)(0xFFFF));
#endif
    }
    ipath=ipath->link;
  } while (ipath!=NULL);
  
}



/* T1_CopyOutline(): Copy an outline physically.
                     Returns a pointer to the path or NULL */
T1_OUTLINE *T1_CopyOutline( T1_OUTLINE *path)
{

  return( (T1_OUTLINE *) CopyPath( (struct segment *)path));
	  
}



/* T1_FreeOutline(): Free an outline. */
void T1_FreeOutline( T1_OUTLINE *path)
{
  
  KillPath( (struct segment *)path);
  return;
  
}

