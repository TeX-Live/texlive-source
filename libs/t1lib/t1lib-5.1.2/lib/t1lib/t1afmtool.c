/*--------------------------------------------------------------------------
  ----- File:        t1afmtool.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2007-12-23
  ----- Description: This file is part of the t1-library. It contains
                     functions for generating a fallback set of afm data
		     from type 1 font files.
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
  
#define T1AFMTOOL_C


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
#include <time.h>
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
#include "../type1/blues.h"


#include "t1types.h"
#include "t1extern.h"
#include "t1finfo.h"
#include "t1base.h"
#include "t1misc.h"
#include "t1set.h"
#include "t1load.h"
#include "t1afmtool.h"


#define DELTA_MAX    30
#define ENCODINGSIZE (int) 256
#ifndef T1LIB_IDENT
#define T1LIB_IDENT  "???.???"
#endif


extern char *t1_get_abort_message( int number);
  

/* T1_GenerateAFMFallbackInfo(): Generate fallback information from
   Type 1 font file by rasterizing every character at 1000 bp. Returns
   a pointer to a generated FontInfo struct or NULL in case of an error.
   */
FontInfo *T1_GenerateAFMFallbackInfo( int FontID)
{
  int i, j;
  struct region *area;
  struct XYspace *S;    
  int mode=0;
  char **charnames;
  int nochars=0;
  FontInfo *pAFMData;

  /* When generaing fallback info, we accumulate a font bounding box that
     could be useful when the font's definition is missing or trivial. */
  int acc_llx=0;
  int acc_lly=0;
  int acc_urx=0;
  int acc_ury=0;
  
  
  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NULL);
  }
  

  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    sprintf( err_warn_msg_buf,
	     "Can't generate AFM Info from Font %d (invalid ID)\n", FontID);
    T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
		 T1LOG_WARNING);
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }

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

  /* Alloc memory for FontInfo: */
  if ((pAFMData=(FontInfo *)malloc( sizeof(FontInfo)))==NULL){
    sprintf( err_warn_msg_buf,
	     "Failed to allocate memory for FontInfo in Font %d!", FontID);
    T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
		 T1LOG_WARNING);
    T1_errno=T1ERR_ALLOC_MEM;
    /* make sure to free S */
    if (S) {
      KillSpace (S);
    }
    return( NULL);
  }
  /* Initialize pointers */
  pAFMData->gfi=NULL;
  pAFMData->cwi=NULL;
  pAFMData->numOfChars=0;
  pAFMData->cmi=NULL;
  pAFMData->numOfTracks=0;
  pAFMData->tkd=NULL;
  pAFMData->numOfPairs=0;
  pAFMData->pkd=NULL;
  pAFMData->numOfComps=0;
  pAFMData->ccd=NULL;
  
  /* Get list of character name */
  charnames=T1_GetAllCharNames( FontID);
  /* and count number of characters */
  nochars=0;
  
  while (charnames[nochars]!=NULL) 
    nochars++;
  pAFMData->numOfChars=nochars;
  /* Allocate memory for CharMetricInfo area */
  if ((pAFMData->cmi=(CharMetricInfo *)
       malloc( nochars * sizeof( CharMetricInfo)))==NULL){
    sprintf( err_warn_msg_buf,
	     "Failed to allocate memory for CharMetricsInfo area in Font %d!",
	     FontID);
    T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
		 T1LOG_WARNING);
    free( pAFMData);
    T1_errno=T1ERR_ALLOC_MEM;
    /* make sure to free S */
    if (S) {
      KillSpace (S);
    }
    return( NULL);
  }
  /* Get metrics values */
  for (i=0; i<nochars; i++){
    area=fontfcnB_ByName( FontID, 0, S, charnames[i], &mode,
			  pFontBase->pFontArray[FontID].pType1Data, DO_RASTER);
    
    if (area==NULL){
      sprintf( err_warn_msg_buf,
	       "Could not get charspace representation of character %d (%s) Font %d!",
	       i, charnames[i], FontID);
      T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
		   T1LOG_WARNING);
      /* Return since we don't know how to fill the values */
      for (j=i-1; j>=0; j--)
	free( pAFMData->cmi[j].name);
      if (pAFMData->cmi!=NULL)
	free( pAFMData->cmi);
      if (pAFMData!=NULL)
	free( pAFMData);
      T1_errno=mode;
      /* make sure to free S */
      if (S) {
	KillSpace (S);
      }
      return( NULL);
    }
    else if ((pAFMData->cmi[i].name=(char *)
	      malloc( (size_t)(strlen( charnames[i])+1)))==NULL){
      sprintf( err_warn_msg_buf,
	       "Failed to allocate memory for CharName %d (%s) Font %d!",
	       i, charnames[i], FontID);
      T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
		   T1LOG_WARNING);
      /* NULL-ptr in charnames is prone to SIGSEGV-errors, thus,
	 we have to return: */
      for (j=i; j>=0; j--)
	free( pAFMData->cmi[j].name);
      free( pAFMData->cmi);
      free( pAFMData);
      T1_errno=T1ERR_ALLOC_MEM;
      /* make sure to free S */
      if (S) {
	KillSpace (S);
      }
      return( NULL);
    }
    else{
      strcpy( pAFMData->cmi[i].name, charnames[i]);
      pAFMData->cmi[i].code=T1_GetEncodingIndex( FontID, charnames[i]);
      pAFMData->cmi[i].wx=NEARESTPEL(area->ending.x);
      pAFMData->cmi[i].wy=NEARESTPEL(area->ending.y);
      /* We check for a valid BBox and set it to zero otherwise */
      if ((int)area->xmax > (int)area->xmin){
	pAFMData->cmi[i].charBBox.llx =(int)area->xmin;
	pAFMData->cmi[i].charBBox.urx =(int)area->xmax;
	pAFMData->cmi[i].charBBox.lly =(int)area->ymin;
	pAFMData->cmi[i].charBBox.ury =(int)area->ymax;
      }
      else{
	pAFMData->cmi[i].charBBox.llx =0;
	pAFMData->cmi[i].charBBox.urx =0;
	pAFMData->cmi[i].charBBox.lly =0;
	pAFMData->cmi[i].charBBox.ury =0;
      }
      pAFMData->cmi[i].ligs=NULL;

      /* Accumulate bounding box of font */
      if ( pAFMData->cmi[i].charBBox.llx < acc_llx ) {
	acc_llx=pAFMData->cmi[i].charBBox.llx;
      }
      if ( pAFMData->cmi[i].charBBox.lly < acc_lly ) {
	acc_lly=pAFMData->cmi[i].charBBox.lly;
      }
      if ( pAFMData->cmi[i].charBBox.urx > acc_urx ) {
	acc_urx=pAFMData->cmi[i].charBBox.urx;
      }
      if ( pAFMData->cmi[i].charBBox.ury > acc_ury ) {
	acc_ury=pAFMData->cmi[i].charBBox.ury;
      }
      
      /* We are done with area, so get rid of it. Solves the REALLY
	 HUGE memory leak */
      KillRegion (area);
    }
  }
  sprintf( err_warn_msg_buf,
	   "Generated metric information for %d characters of font %d!",
	   nochars, FontID);
  T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
	       T1LOG_STATISTIC);

  /* Check whether the bounding box we computed could be better than that
     specified in the font file itself. Id so, we overwrite it. */
  if ( pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[0].data.integer == 0   &&
       pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[1].data.integer == 0   &&
       pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[2].data.integer == 0   &&
       pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[3].data.integer == 0 ) {
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[0].data.integer = acc_llx;  
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[1].data.integer = acc_lly;  
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[2].data.integer = acc_urx;  
    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[3].data.integer = acc_ury;
    
    sprintf( err_warn_msg_buf,
	     "Substituted accumulated FontBBox [%d,%d,%d,%d] for trivial FontBBox of font %d!",
	     acc_llx, acc_lly, acc_urx, acc_ury, FontID);
    T1_PrintLog( "T1_GenerateAFMFallbackInfo()", err_warn_msg_buf,
		 T1LOG_WARNING);
  }
  
  /* make sure to free S */
  if (S) {
    KillSpace (S);
  }
  return( pAFMData);  
}


/* T1_WriteAFMFallbackFile(): Write a fallback AFM-file from AFM data
   genarated for font FontID. returns
    0       if successful
   -1       if afm-data was loaded from existent AFM-file
   -2       if font is not loaded (invalid FontID)
   -3       if for some reason the fonts' AFM-data has not been generated
   -4       if the file could not be openend
   -5       if an error occurred during write
   -6       other error.
   */
int T1_WriteAFMFallbackFile( int FontID)
{

  int i, j, k, l;

  int nochars;
  int capheight, ascender, xheight, descender;
  int min=0, delta;
  time_t s_clock, *tp;
  
  char *afmfilename;
  FILE *afmfile;
  CharMetricInfo **cmi;
  

  /* Check for valid font */
  if (T1_CheckForFontID(FontID)!=1){
    sprintf( err_warn_msg_buf,
	     "Warning: Invalid FontID, font %d not loaded!",
	     FontID);
    T1_PrintLog( "T1_WriteAFMFallbackFile()", err_warn_msg_buf,
	       T1LOG_WARNING);
    T1_errno=T1ERR_INVALID_FONTID;
    return(-2);
  }

  /* Check for AFM-data to be existent */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL){
    sprintf( err_warn_msg_buf,
	     "Warning: No AFM-Data available for font %d",
	     FontID);
    T1_PrintLog( "T1_WriteAFMFallbackFile()", err_warn_msg_buf,
	       T1LOG_WARNING);
    return(-3);
  }

  /* Check if AFM data has been generated from existent afm file -->
     no need to generate a new one. */
  if ((pFontBase->pFontArray[FontID].info_flags & AFM_SUCCESS) 
      || (pFontBase->pFontArray[FontID].info_flags & AFM_SLOPPY_SUCCESS)){
    sprintf( err_warn_msg_buf,
	     "Alert:  Available AFM-Data for font %d is generated from existent AFM-file!",
	     FontID);
    T1_PrintLog( "T1_WriteAFMFallbackFile()", err_warn_msg_buf,
		 T1LOG_STATISTIC);
    return(-1);
  }
  
  /* Construct AFM-filename */
  i=strlen(pFontBase->pFontArray[FontID].pFontFileName);
  if ((afmfilename=(char *)malloc((size_t)(i + 1) ))==NULL){
    sprintf( err_warn_msg_buf,
	     "Memory allocation error (fontID = %d)",
	     FontID);
    T1_PrintLog( "T1_WriteAFMFallbackFile()", err_warn_msg_buf,
		 T1LOG_WARNING);
    T1_errno=T1ERR_ALLOC_MEM;
    return(-6);
  }
  strcpy( afmfilename, pFontBase->pFontArray[FontID].pFontFileName);
  while (afmfilename[i]!='.')
    i--;
  afmfilename[i+1]='a';
  afmfilename[i+2]='f';
  afmfilename[i+3]='m';
  
  if ((afmfile=fopen( afmfilename, "wb"))==NULL){
    sprintf( err_warn_msg_buf,
	     "Could not open %s, (FontID = %d)",
	     afmfilename, FontID);
    T1_PrintLog( "T1_WriteAFMFallbackFile()", err_warn_msg_buf,
		 T1LOG_WARNING);
    free( afmfilename);
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(-4);
  }

  /* Set the creation time of afm file */
  tp=&s_clock;
  s_clock=time( tp);

  /* Allocate a pointer-array to the metrics information, 
     of the size 256 + number of characters. */
  nochars=pFontBase->pFontArray[FontID].pAFMData->numOfChars;
  if ((cmi=(CharMetricInfo **)calloc( nochars+ENCODINGSIZE,
				     sizeof(CharMetricInfo *)))==NULL){
    sprintf( err_warn_msg_buf,
	     "Memory allocation error (fontID = %d)",
	     FontID);
    T1_PrintLog( "T1_WriteAFMFallbackFile()", err_warn_msg_buf,
		 T1LOG_WARNING);
    T1_errno=T1ERR_ALLOC_MEM;
    return(-6);
  }
  /* Reset area */
  for ( i=0; i<(nochars+ENCODINGSIZE); i++)
    cmi[i]=NULL;
  /* First half of array is dedicated to encoded characters, the
     second half for unencoded characters */
  k=ENCODINGSIZE;
  for ( i=0; i<nochars; i++){
    j=T1_GetEncodingIndex( FontID,
			   pFontBase->pFontArray[FontID].pAFMData->cmi[i].name);
    if (j>=0)
      cmi[j]=&(pFontBase->pFontArray[FontID].pAFMData->cmi[i]);
    else
      cmi[k++]=&(pFontBase->pFontArray[FontID].pAFMData->cmi[i]);
  }
  
  fprintf(afmfile, "StartFontMetrics 4.0\n");
  fprintf(afmfile, "Comment This is %s created from %s by t1lib V. %s.\n",
	  afmfilename, pFontBase->pFontArray[FontID].pFontFileName,
	  T1LIB_IDENT);
  fprintf(afmfile, "Comment File creation date: %s", ctime(&s_clock));
  fprintf(afmfile, "Comment t1lib is copyright (c) Rainer Menzner, 1996-2001.\n");
  fprintf(afmfile, "Comment t1lib is distributed under the GNU General Public Library License (LGPL)\n");
  fprintf(afmfile, "FontName %s\n", T1_GetFontName( FontID));
  fprintf(afmfile, "FullName %s\n", T1_GetFullName( FontID));
  fprintf(afmfile, "FamilyName %s\n", T1_GetFamilyName( FontID));
  fprintf(afmfile, "Weight %s\n", T1_GetWeight( FontID));
  fprintf(afmfile, "ItalicAngle %d\n", (int)T1_GetItalicAngle( FontID));
  fprintf(afmfile, "IsFixedPitch %s\n", T1_GetIsFixedPitch( FontID) ? "true" : "false" );
  fprintf(afmfile, "FontBBox %d %d %d %d\n",
	  T1_GetFontBBox( FontID).llx,
	  T1_GetFontBBox( FontID).lly,
	  T1_GetFontBBox( FontID).urx,
	  T1_GetFontBBox( FontID).ury);
  fprintf(afmfile, "UnderlinePosition %d\n",
	  (int)T1_GetUnderlinePosition( FontID));
  fprintf(afmfile, "UnderlineThickness %d\n",
	  (int)T1_GetUnderlineThickness( FontID));

  fprintf(afmfile, "Version %s\n", T1_GetVersion( FontID));
  fprintf(afmfile, "Notice %s\n", T1_GetNotice( FontID));
  /* Encoding */
  if (pFontBase->pFontArray[FontID].pFontEnc==NULL){
    if (pFontBase->pFontArray[FontID].info_flags & USES_STANDARD_ENCODING){
      fprintf( afmfile, "EncodingScheme AdobeStandardEncoding\n");
    }
    else{
      fprintf( afmfile, "EncodingScheme FontSpecific\n");
    }
  }
  else
    fprintf( afmfile, "EncodingScheme FontSpecific\n");
  /* Values like capheight ascender xheight and descender */
  /* We start with the overshoot positions */
  i=pFontBase->pFontArray[FontID].pType1Data->BluesP->numBlueValues;
  if (i>0){
    /* Capheight */
    if ((k=T1_GetEncodingIndex( FontID, "H"))!=-1){
      l=T1_GetCharBBox( FontID, (char) k).ury;
      delta=10000;
      for (j=0; j<i; j++){
	if (delta > abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[j] - l)){
	  min=j;
	  delta=abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[j] - l);
	}
      }
      if (min % 2) /* index is odd */ 
	min--;
      /* Check for plausibility */
      if (abs( pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[min] - l)
	  > DELTA_MAX) /* We probably did not found the right position */
	capheight=0;
      else
	capheight=pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[min];
    }
    else
      capheight=0;
    /* XHeight */
    if ((k=T1_GetEncodingIndex( FontID, "x"))!=-1){
      l=T1_GetCharBBox( FontID, (char) k).ury;
      delta=10000;
      for (j=0; j<i; j++){
	if (delta > abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[j] - l)){
	  min=j;
	  delta=abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[j] - l);
	}
      }
      if (min % 2) /* index is odd */ 
	min--;
      /* Check for plausibility */
      if (abs( pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[min] - l)
	  > DELTA_MAX) /* We probably did not found the right position */
	xheight=0;
      else
	xheight=pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[min];
    }
    else{
      xheight=0;
    }
    /* Ascender */
    if ((k=T1_GetEncodingIndex( FontID, "d"))!=-1){
      l=T1_GetCharBBox( FontID, (char) k).ury;
      delta=10000;
      for (j=0; j<i; j++){
	if (delta > abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[j] - l)){
	  min=j;
	  delta=abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[j] - l);
	}
      }
      if (min % 2) /* index is odd */ 
	min--;
      /* Check for plausibility */
      if (abs( pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[min] - l)
	  > DELTA_MAX) /* We probably did not found the right position */
	ascender=0;
      else
	ascender=pFontBase->pFontArray[FontID].pType1Data->BluesP->BlueValues[min];
    }
    else{
      ascender=0;
    }
  }
  else{
    capheight=0;
    xheight=0;
    ascender=0;
  }
  /* And now comes under shoot alignment position */
  i=pFontBase->pFontArray[FontID].pType1Data->BluesP->numOtherBlues;
  if (i>0){
    /* Descender */
    if ((k=T1_GetEncodingIndex( FontID, "p"))!=-1){
      l=T1_GetCharBBox( FontID, (char) k).lly;
      delta=10000;
      for (j=0; j<i; j++){
	if (delta > abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->OtherBlues[j] - l)){
	  min=j;
	  delta=abs(pFontBase->pFontArray[FontID].pType1Data->BluesP->OtherBlues[j] - l);
	}
      }
      if ((min % 2)==0) /* index is even */ 
	min++;
      /* Check for plausibility */
      if (abs( pFontBase->pFontArray[FontID].pType1Data->BluesP->OtherBlues[min] - l)
	  > DELTA_MAX) /* We probably did not found the right position */
	descender=0;
      else
	descender=pFontBase->pFontArray[FontID].pType1Data->BluesP->OtherBlues[min];
    }
    else{
      descender=0;
    }
  }
  else{
    descender=0;
  }
  if (capheight != 0)
    fprintf( afmfile, "CapHeight %d\n", capheight);
  if (xheight != 0)
    fprintf( afmfile, "XHeight %d\n", xheight);
  if (ascender != 0)
    fprintf( afmfile, "Ascender %d\n", ascender);
  if (descender != 0)
    fprintf( afmfile, "Descender %d\n", descender);
  if (pFontBase->pFontArray[FontID].pType1Data->BluesP->StdHW != 0.0)
    fprintf( afmfile, "StdHW %d\n",
	     (int) pFontBase->pFontArray[FontID].pType1Data->BluesP->StdHW);
  if (pFontBase->pFontArray[FontID].pType1Data->BluesP->StdVW != 0.0)
    fprintf( afmfile, "StdVW %d\n",
	     (int) pFontBase->pFontArray[FontID].pType1Data->BluesP->StdVW);

  /* Now fill in the values (we omit the .notdef) */
  fprintf(afmfile, "StartCharMetrics %d\n", nochars-1);
  for ( i=0; i<(nochars+ENCODINGSIZE); i++){
    if (cmi[i]!=NULL && strcmp(cmi[i]->name,".notdef"))
      fprintf( afmfile, "C %3d ; WX %4d ; N %-20s ; B %5d %5d %5d %5d ;\n",
	       cmi[i]->code, cmi[i]->wx, cmi[i]->name,
	       cmi[i]->charBBox.llx, cmi[i]->charBBox.lly,
	       cmi[i]->charBBox.urx, cmi[i]->charBBox.ury);
  }
  fprintf(afmfile, "EndCharMetrics\nEndFontMetrics\n"); 
  
  free(cmi);
  fclose( afmfile);
  return(0);
}


