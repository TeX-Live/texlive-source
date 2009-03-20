/*--------------------------------------------------------------------------
  ----- File:        t1finfo.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-01
  ----- Description: This file is part of the t1-library. It contains
                     functions for accessing afm-data and some other
		     fontinformation data.
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
  
#define T1FINFO_C


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
#include "t1finfo.h"
#include "t1base.h"
#include "t1misc.h"
#include "t1set.h"
#include "t1load.h"


/* The following variable controls the computation of the bbox internal
   to T1_GetMetricsInfo(). Its influence may be overridden by the
   global variable ForceAFMBBox: */
static int ForceAFMBBoxInternal=0;


extern int ForceAFMBBox;
extern char *t1_get_abort_message( int number);
  

/* int T1_GetKerning(): This function returns the amount of kerning that
   is specified in the afm-file for the supplied character-pair. If an
   an extension has been applied to the font in question, this is taken
   into account.
   If for whatever reason there's no afm information available (that's not
   deadly), simply 0 is returned, indicating that no kerning should be used.
   The value returned is meant to be in character space coordinates. Thus,
   it must be transformed to be applicable in device space.
   */
int T1_GetKerning( int FontID, char char1, char char2)
{
  METRICS_ENTRY entry;
  METRICS_ENTRY *target_pair=NULL;
  

  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0);
  }

  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( 0);
  }

  /* if there's no kerning info, return immediately */
  if (pFontBase->pFontArray[FontID].KernMapSize==0)
    return( 0);
  
  entry.chars=(char1<<8) | char2;
  if ((target_pair=(METRICS_ENTRY *)
       bsearch( &entry, pFontBase->pFontArray[FontID].pKernMap,
		(size_t) pFontBase->pFontArray[FontID].KernMapSize,
		sizeof(METRICS_ENTRY),
		&cmp_METRICS_ENTRY))==NULL)
    return(0);
  else
    return( target_pair->hkern * pFontBase->pFontArray[FontID].extend);
  
}



/* int T1_GetCharWidth(): This function returns the characterwidth
   specified in the .afm-file. If no .afm-file is loaded for that font,
   0 is returned. Note that if one tries to raster strings, afm data
   must always be available. The returned character width is corrected
   using  a possibly applied font extension!
   */
int T1_GetCharWidth( int FontID, char char1)
{
  unsigned char uchar1;

  uchar1=(unsigned char) char1;
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0);
  }
  
  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( 0);
  }
  
  /* return appriate value */
  if (pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]>0) { /* ordinary character */
    return((int) ((pFontBase->pFontArray[FontID].pAFMData->cmi[pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]-1].wx) * pFontBase->pFontArray[FontID].extend));
  }
  else if (pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]<0) { /* composite character */
    return((int) ((pFontBase->pFontArray[FontID].pAFMData->ccd[-(pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]+1)].wx) * pFontBase->pFontArray[FontID].extend));
  }
  else { /* undefined or .notdef */
    return(0);
  }
  
}



/* T1_GetCharBBox(): Get the BoundingBox of specified character. If an
   extension factor has been applied to the font in question, this
   is taken into account. However, a slant factor which has been applied
   to the font, also affects the bounding box of a character. The
   only way to determine its influence on the character bounding box
   is to compute the exact shape of that slanted character. There's no
   simple way to extract the new bounding box from the former bounding
   box. Thus, if a font has been slanted, the characters outline itself
   is examined. Since this must be done at 1000 bp it takes considerably
   longer than reading afm data. */
BBox T1_GetCharBBox( int FontID, char char1)
{

  struct region *area;
  struct XYspace *S;    
  int mode=0;
  int i;
  
  BBox NullBBox= { 0, 0, 0, 0}; /* A bounding box containing all 0's. */
  BBox ResultBox= { 0, 0, 0, 0}; /* The Box returned if char is found */
  
  unsigned char uchar1;


  /* We return to this if something goes wrong deep in the rasterizer */
  if ((i=setjmp( stck_state))!=0) {
    T1_errno=T1ERR_TYPE1_ABORT;
    sprintf( err_warn_msg_buf, "t1_abort: Reason: %s",
	     t1_get_abort_message( i));
    T1_PrintLog( "T1_GetCharBBox()", err_warn_msg_buf,
	       T1LOG_ERROR);
    return( NullBBox);
  }

  
  uchar1=(unsigned char) char1;
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NullBBox);
  }

  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( NullBBox);
  }
  
  
  /* Check for a font slant */
  if ((pFontBase->pFontArray[FontID].slant!=0.0)
      &&(ForceAFMBBox==0)
      &&(ForceAFMBBoxInternal==0)){
    /* We have a font slant -> character outline must be examined in order
       to determine bounding box */
    /* Set up an identity charspace matrix 
       and take a slant and an extension into account */
    /* And make it permanent, to plug a memory leak */
    S=(struct XYspace *)IDENTITY;
    S=(struct XYspace *)Permanent
      (Transform(S, pFontBase->pFontArray[FontID].FontTransform[0],
		 pFontBase->pFontArray[FontID].FontTransform[1],
		 pFontBase->pFontArray[FontID].FontTransform[2],
		 pFontBase->pFontArray[FontID].FontTransform[3]));
    /* Genrate an edgelist for the current character at size 1000bp
       using current transformation and encoding: */
    area=fontfcnB( FontID, 0, S,
		   pFontBase->pFontArray[FontID].pFontEnc,
		   (int) uchar1, &mode,
		   pFontBase->pFontArray[FontID].pType1Data,
		   DO_RASTER,0.0f);
    /* Read out bounding box */
    ResultBox.llx =area->xmin;
    ResultBox.urx =area->xmax;
    ResultBox.lly =area->ymin;
    ResultBox.ury =area->ymax;
    
    /* Reset AFM-switch and return BBox */
    ForceAFMBBoxInternal=0;
    /* make sure to destroy 'area' before leaving! */
    KillRegion (area);
    /* make sure to free S */
    if (S) {
      KillSpace (S);
    }
    return(ResultBox);
  }
  else{
    /* Assign bounding box for the different cases: */
    /* Check if character is  */
    if (pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]>0) { /* ordinary char */
      ResultBox=(pFontBase->pFontArray[FontID].pAFMData->cmi[pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]-1].charBBox);
    }
    else if (pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]<0) { /* composite char */
      ResultBox=(pFontBase->pFontArray[FontID].pAFMData->ccd[-(pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]+1)].charBBox);
    }
    else { /* undefined char */
      return(NullBBox);
    }
    
    /* .. and apply transformations: */
    ResultBox.llx *=pFontBase->pFontArray[FontID].extend;
    ResultBox.urx *=pFontBase->pFontArray[FontID].extend;
    
    return(ResultBox);
  }
}



/* int T1_GetUnderlinePosition(): Return underline position of specified
   font in charspace units. If 0 is returned, it indicated that the font
   is not yet loaded into memory. or an invalid ID has been specified. */
float T1_GetUnderlinePosition( int FontID)
{
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }
  
  return((float)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[UNDERLINEPOSITION].value.data.real));
}



/* int T1_GetUnderlineThickness(): Return underline thickness of specified
   font in charspace units. If 0 is returned, it indicated that the font
   is not yet loaded into memory. or an invalid ID has been specified. */
float T1_GetUnderlineThickness( int FontID)
{
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }
  
  return((float)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[UNDERLINETHICKNESS].value.data.real));
}


/* int T1_ItalicAngle(): Return underline position of specified
   font in charspace units. If 0.0 is returned, it indicated that the font
   is not yet loaded into memory. or an invalid ID has been specified. */
float T1_GetItalicAngle( int FontID)
{
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }
  
  return((float)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ITALICANGLE].value.data.real));
}



/* int T1_GetUnderlinePosition(): Return underline position of specified
   font in charspace units. If 0 is returned, it indicated that the font
   is not yet loaded into memory. or an invalid ID has been specified. */
int T1_GetIsFixedPitch( int FontID)
{
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0.0);
  }
  
  return((int)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ISFIXEDPITCH].value.data.boolean));
}



/* char *T1_GetFontName( FontID): Get the PostScript FontName of
   the  font dictionary associated with the specified font, or NULL if
   an error occurs. */
char *T1_GetFontName( int FontID)
{
  static char fontname[MAXPSNAMELEN];
  
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  strncpy(fontname,
	  (char *)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTNAME].value.data.nameP),
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTNAME].value.len);
  fontname[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTNAME].value.len]=0;
    
  return(fontname);
  
}


/* char *T1_GetFullName( FontID): Get the Full Name from
   the  font dictionary associated with the specified font, or NULL if
   an error occurs. */
char *T1_GetFullName( int FontID)
{
  static char fullname[MAXPSNAMELEN];
  
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  strncpy(fullname,
	  (char *)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FULLNAME].value.data.nameP),
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FULLNAME].value.len);
  fullname[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FULLNAME].value.len]=0;
    
  return(fullname);
  
}


/* char *T1_GetFamilyName( FontID): Get the Family Name of
   the  font dictionary associated with the specified font, or NULL if
   an error occurs. */
char *T1_GetFamilyName( int FontID)
{
  static char familyname[MAXPSNAMELEN];
  
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  strncpy(familyname,
	  (char *)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FAMILYNAME].value.data.nameP),
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FAMILYNAME].value.len);
  familyname[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FAMILYNAME].value.len]=0;
    
  return(familyname);
  
}


/* char *T1_GetWeight( FontID): Get the Weight entry from
   the  font dictionary associated with the specified font, or NULL if
   an error occurs. */
char *T1_GetWeight( int FontID)
{
  static char weight[128];
  
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  strncpy(weight,
	  (char *)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[WEIGHT].value.data.nameP),
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[WEIGHT].value.len);
  weight[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[WEIGHT].value.len]=0;
    
  return(weight);
  
}


/* char *T1_GetFontName( FontID): Get the Version entry from 
   the  font dictionary associated with the specified font, or NULL if
   an error occurs. */
char *T1_GetVersion( int FontID)
{
  static char version[2048];
  
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  strncpy(version,
	  (char *)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[VERSION].value.data.nameP),
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[VERSION].value.len);
  version[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[VERSION].value.len]=0;
    
  return(version);
  
}


/* char *T1_GetNotice( FontID): Get the Notice entry from
   the  font dictionary associated with the specified font, or NULL if
   an error occurs. */
char *T1_GetNotice( int FontID)
{
  static char notice[2048];
  
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  strncpy(notice,
	  (char *)(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[NOTICE].value.data.nameP),
	  pFontBase->pFontArray[FontID].pType1Data->fontInfoP[NOTICE].value.len);
  notice[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[NOTICE].value.len]=0;
    
  return(notice);
  
}




/* char *T1_GetCharName(): Get the PostScript character name of
   the  character indexed by char1. */
char *T1_GetCharName( int FontID, char char1)
{
  static char cc_name1[256];
  char *c1;
  

  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }

  if (pFontBase->pFontArray[FontID].pFontEnc==NULL){
    /* We have to get the names from the fonts internal encoding */
    c1= (char *)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].data.arrayP;
    strncpy(cc_name1,
	    (char *)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].data.arrayP,
	    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].len);
    cc_name1[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].len]=0;
  }
  else{
    /* Take names from explicitly loaded and assigned encoding */
    c1=pFontBase->pFontArray[FontID].pFontEnc[(unsigned char)char1];
    strcpy(cc_name1,c1);
  }

  /* Return address of charname */
  return(cc_name1);
  
}



/* T1_QueryLigs(): Get the number of ligatures defined in the font FontID for
   the character which is located at position char1 in the current encoding
   vector!
   Function returns the number of defined ligs (including 0) or -1 if an
   error occured.
   */
int T1_QueryLigs( int FontID,  char char1, char **successors,
		  char **ligatures)
{

  FontInfo *afm_ptr;
  CharMetricInfo *m_ptr;
  char *c_name;
  char cc_name[128];
  static char succ[MAX_LIGS];
  int succ_index;
  static char lig[MAX_LIGS];
  int lig_index;
  
  Ligature *ligs;
  int i,j;
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }
  
  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( -1);
  }

  /* All OK, ... */
  afm_ptr=pFontBase->pFontArray[FontID].pAFMData;
  m_ptr=afm_ptr->cmi;

  /* Get the name of the character: */
  if (pFontBase->pFontArray[FontID].pFontEnc==NULL){
    /* We have to get the name from the fonts internal encoding */
    c_name=(char *)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].data.arrayP;
    strncpy(cc_name,
	    (char *)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].data.arrayP,
	    pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].len);
    cc_name[pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[(unsigned char)char1].len]=0;
  }
  else{
    /* Take name from explicitly loaded and assigned encoding */
    c_name=pFontBase->pFontArray[FontID].pFontEnc[(unsigned char)char1];
    strcpy(cc_name,c_name);
  }

  for (i=0; i<afm_ptr->numOfChars; i++){
    if (strcmp(m_ptr[i].name,cc_name)==0)
      break;
  }
  

  if (i==afm_ptr->numOfChars) /* we didn't find the characters name */
    return(-1);
  
  ligs=m_ptr[i].ligs;

  j=0;
  if (ligs==NULL)
    return(0);
  
  while (ligs!=NULL) {
    /* Get indices of the two characters: */
    if ((succ_index=T1_GetEncodingIndex( FontID, (char*) ligs->succ))==-1) {
      /* successor is not current encoding */
      ligs=ligs->next;
      continue;
    }
    if ((lig_index=T1_GetEncodingIndex( FontID, (char*) ligs->lig))==-1) {
      /* Specified ligature is not in current encoding */
      ligs=ligs->next;
      continue;
    }
    succ[j]=(char)succ_index;
    lig[j]=(char)lig_index;
    j++;
    ligs=ligs->next;
  }
    
  *successors=succ;
  *ligatures=lig;
  
  return(j);
}

      

/* T1_GetEncodingIndex(): Return the Index of char1 in the current
   encoding vector of font FontID */
int T1_GetEncodingIndex( int FontID, char *char1)
{
  int i;
  int len1;
  int result_index;
  char **extern_enc;
  psobj *objptr;
  
  
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }

  extern_enc=pFontBase->pFontArray[FontID].pFontEnc;

  len1=strlen( char1);
  
  /* The default return-value if character is not found: */
  result_index=-1;

  if (extern_enc==NULL) {
    objptr=&(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[0]);
    /* We have to search the fonts internal encoding */
    for (i=0;i<256;i++){
      if (len1==objptr[i].len){
	if (strncmp((char *)objptr[i].data.arrayP,
		    char1, objptr[i].len)==0){ 
	  result_index=i; 
	  break; 
	}
      }
    }
    
  }
  else {
    /* Take name from explicitly loaded and assigned encoding */
    for (i=0;i<256;i++){
      if (strcmp(extern_enc[i], char1)==0){
	result_index=i;
	break;
      }
    }
  }

  return(result_index);
}


/* T1_GetEncodingIndices(): Return all indices of char1 in the current
   encoding vector of font FontID. */
int *T1_GetEncodingIndices( int FontID, char *char1)
{
  int i;
  int endmark=0;
  int len1;
  char **extern_enc;
  psobj *objptr;
  /* the following array suffices for the extreme unlikely case of a font
     where one single fillsthe whole encoding vector */
  static int indices[257];

  
  if (T1_CheckForFontID(FontID)!=1) {
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }

  extern_enc=pFontBase->pFontArray[FontID].pFontEnc;

  len1=strlen( char1);
  
  if (extern_enc==NULL) {
    objptr=&(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[0]);
    /* We have to search the fonts internal encoding */
    for (i=0;i<256;i++){
      if (len1==objptr[i].len){
	if (strncmp((char *)objptr[i].data.arrayP,
		    char1, objptr[i].len)==0){ 
	  indices[endmark++]=i; 
	}
      }
    }
  }
  else {
    /* Take name from explicitly loaded and assigned encoding */
    for (i=0;i<256;i++){
      if (strcmp(extern_enc[i], char1)==0){
	indices[endmark++]=i; 
      }
    }
  }

  indices[endmark]=-1;
  return((int *)indices);
}


/* int T1_GetStringWidth(): This function returns the width of string
   in .afm-file units. If no .afm-file is loaded for font FontID,
   0 is returned. Note that if one tries to raster strings, afm data
   should always be available. The returned character width is corrected
   using  a possibly applied font extension!
   */
int T1_GetStringWidth( int FontID, char *string,
		       int len,  long spaceoff, int kerning)
{

  int no_chars;      /* Number of chars in string */
  int i;
  int *kern_pairs;
  int *charwidths;
  int spacewidth; 
  int stringwidth;

  unsigned char *ustring;

  ustring=(unsigned char *) string;
  
  /* First, check for a correct ID */
  i=T1_CheckForFontID(FontID);
  if (i!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(0);
  }
  
  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( 0);
  }

  /* Get length of string: */
  if (len<0 || ustring == NULL){  /* invalid length or NULL-pointer */
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(0);
  }
  if (len==0) /* should be computed assuming "normal" 0-terminated string */
    no_chars=strlen(string);
  else        /* use value given on command line */
    no_chars=len;

  switch (no_chars) {
    case 0:
      /* Empty string has width 0 */
      stringwidth=0;
      break;

    case 1:
      /* Width of string with 1 character is the width of that character.
         If the character is a space, adjust by the value of spaceoff.
         */
      stringwidth=T1_GetCharWidth(FontID,ustring[0]);
      if (ustring[0]==pFontBase->pFontArray[FontID].space_position)
        stringwidth+=spaceoff;
      break;

    default:
      /* Two or more characters.  Add widths of characters and adjust by
         the adjustment widths for any kerning pairs.  For spaces, use the
         width of the space character in the font adjusted by the value of
         spaceoff.
         */
    
      /* Allocate room for temporary arrays of kerning and width arrays: */
      kern_pairs=(int *)calloc(no_chars -1, sizeof(int));
      if (kern_pairs==NULL){
        T1_errno=T1ERR_ALLOC_MEM;
        return(0);
      }
      charwidths=(int *)calloc(no_chars, sizeof(int));
      if (charwidths==NULL){
        T1_errno=T1ERR_ALLOC_MEM;
        return(0);
      }
  
      /* If kerning is requested, get kerning amounts and fill the array: */
      if (kerning){
        for (i=0; i<no_chars -1; i++){
          kern_pairs[i]=T1_GetKerning( FontID, ustring[i], ustring[i+1]);
        }
      }
  
      /* Compute the correct spacewidth value (in charspace units): */
      spacewidth=T1_GetCharWidth(FontID,pFontBase->pFontArray[FontID].space_position)+spaceoff;
  
      /* Fill the width-array:  */
      for (i=0; i<no_chars; i++){
        if (ustring[i]==pFontBase->pFontArray[FontID].space_position)
          charwidths[i]=(int)spacewidth;
        else
          charwidths[i]=T1_GetCharWidth(FontID,ustring[i]);
      }
  
      /* Accumulate width: */
      stringwidth=0;
      for (i=0; i<no_chars-1; i++){
        stringwidth += kern_pairs[i];
      }
      for (i=0; i<no_chars; i++){
        stringwidth += charwidths[i];
      }
  
      /* free memory: */
      free( charwidths);
      free( kern_pairs);

      break;
  }

  /* .. and return result: */
  return( stringwidth);
}

    

/* int T1_GetStringBBox(): This function returns the bounding box of string
   in .afm-file units. If no .afm-file is loaded for font FontID,
   0 is returned. Note that if one tries to raster strings, afm data
   should always be available. The returned character width is corrected
   using  a possibly applied font extension!
   */
BBox T1_GetStringBBox( int FontID, char *string,
		       int len,  long spaceoff, int kerning)
{

  BBox NullBBox= { 0, 0, 0, 0}; /* A bounding box containing all 0's. */
  BBox tmp_BBox= { 0, 0, 0, 0}; 
  BBox ResultBBox= { 0, 0, 0, 0}; /* The resulting BBox */
  int i;
  int no_chars;
  int curr_width=0;
  int spacewidth=0;
  
  int rsb_max=-30000;
  int lsb_min= 30000;
  int overallascent=-30000;
  int overalldescent=30000;

  
  /* First, check for a correct ID */
  i=T1_CheckForFontID(FontID);
  if (i!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NullBBox);
  }
  
  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( NullBBox);
  }

  /* Get length of string: */
  if (len<0 || string==NULL) {  /* invalid length or NULL-pointer */
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NullBBox);
  }
  if (len==0) /* should be computed assuming "normal" 0-terminated string */
    no_chars=strlen(string);
  else        /* use value given on command line */
    no_chars=len;
  
  spacewidth=
    T1_GetCharWidth(FontID,pFontBase->pFontArray[FontID].space_position)+spaceoff;
  
  /* Accumulate metrics: */
  for (i=0; i<no_chars; i++){
    if (string[i]==pFontBase->pFontArray[FontID].space_position)
      curr_width +=spacewidth;
    else{
      tmp_BBox=T1_GetCharBBox( FontID, string[i]);
      if (curr_width+tmp_BBox.llx < lsb_min)
	lsb_min=curr_width+tmp_BBox.llx;
      if (curr_width+tmp_BBox.urx > rsb_max)
	rsb_max=curr_width+tmp_BBox.urx;
      if (tmp_BBox.lly < overalldescent)
	overalldescent=tmp_BBox.lly;
      if (tmp_BBox.ury > overallascent)
	overallascent=tmp_BBox.ury;
      curr_width +=T1_GetCharWidth( FontID, string[i]);
      if ((i<no_chars-1) && (kerning != 0))
	curr_width += T1_GetKerning( FontID, string[i], string[i+1]);
    }
  }

  ResultBBox.llx=lsb_min;
  ResultBBox.lly=overalldescent;
  ResultBBox.urx=rsb_max;
  ResultBBox.ury=overallascent;
  
  return( ResultBBox);
  
}


/* T1_GetMetricsInfo(): Return a structure containing metrics information
   about the string to the user. */
METRICSINFO T1_GetMetricsInfo( int FontID, char *string,
			       int len, long spaceoff, int kerning)
{

  BBox NullBBox= { 0, 0, 0, 0}; /* A bounding box containing all 0's. */
  BBox tmp_BBox= { 0, 0, 0, 0}; 


  int i;
  int no_chars;
  
  int curr_width=0;
  int spacewidth=0;
  
  int rsb_max=-30000;
  int lsb_min= 30000;
  int overallascent=-30000;
  int overalldescent=30000;

  static METRICSINFO metrics={ 0, {0, 0, 0, 0}, 0, NULL};

  unsigned char *ustring;

  
  ustring=(unsigned char *) string;
  
  /* Reset struct: */
  metrics.width=0;
  metrics.bbox=NullBBox;
  metrics.numchars=0;
  if (metrics.charpos != NULL){
    free( metrics.charpos);
    metrics.charpos=NULL;
  }


  /* First, check for a correct ID */
  i=T1_CheckForFontID(FontID);
  if (i!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(metrics);
  }
  
  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( metrics);
  }

  /* Get length of string: */
  if (len<0 || ustring==NULL ) {  /* invalid length or NULL_pointer */
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(metrics);
  }
  
  if (len==0) /* should be computed assuming "normal" 0-terminated string */
    no_chars=strlen(string);
  else        /* use value given on command line */
    no_chars=len;

  /* Compute the correct spacewidth value (in charspace units): */
  spacewidth=T1_GetCharWidth(FontID,pFontBase->pFontArray[FontID].space_position)+spaceoff;

  /* Allocate memory for character positions array: */
  metrics.charpos=(int *)calloc(no_chars, sizeof(int));

  metrics.numchars=no_chars;
  
  /* Accumulate metrics: */
  for (i=0; i<no_chars; i++){
    /* Save current offst to array */
    metrics.charpos[i]=curr_width;
    if (string[i]==pFontBase->pFontArray[FontID].space_position)
      curr_width +=spacewidth;
    else{
      tmp_BBox=T1_GetCharBBox( FontID, string[i]);
      if (curr_width+tmp_BBox.llx < lsb_min)
	lsb_min=curr_width+tmp_BBox.llx;
      if (curr_width+tmp_BBox.urx > rsb_max)
	rsb_max=curr_width+tmp_BBox.urx;
      if (tmp_BBox.lly < overalldescent)
	overalldescent=tmp_BBox.lly;
      if (tmp_BBox.ury > overallascent)
	overallascent=tmp_BBox.ury;
      curr_width +=T1_GetCharWidth( FontID, string[i]);
      if ((i<no_chars-1) && (kerning != 0))
	curr_width += T1_GetKerning( FontID, string[i], string[i+1]);
    }
  }

  metrics.width   =curr_width;
  metrics.bbox.llx=lsb_min;
  metrics.bbox.lly=overalldescent;
  metrics.bbox.urx=rsb_max;
  metrics.bbox.ury=overallascent;

  return( metrics);
  
}



/* T1_GetFontBBox(): Return the font's bounding box. Note: The font
   BBox is taken is taken from the font file rather than from afm
   file since I have seen some afm with rather inaccurate BBoxes.: */
BBox T1_GetFontBBox( int FontID)
{
  
  BBox outbox= { 0, 0, 0, 0}; 
  struct ps_obj *obj;

  /* return Null-box if font not loaded */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(outbox);
  }

  /* As suggested by Derek B. Noonburg (xpdf-Author), we allow the
     FontBBox also to be specified by real numbers. */
  obj = &(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[0]);
  outbox.llx =
    objPIsInteger(obj) ? obj->data.integer : obj->data.real > 0 ?
    (int) ceil(obj->data.real) : (int) floor(obj->data.real);
  obj = &(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[1]);
  outbox.lly =
    objPIsInteger(obj) ? obj->data.integer : obj->data.real > 0 ?
    (int) ceil(obj->data.real) : (int) floor(obj->data.real);
  obj = &(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[2]);
  outbox.urx =
    objPIsInteger(obj) ? obj->data.integer : obj->data.real > 0 ?
    (int) ceil(obj->data.real) : (int) floor(obj->data.real);
  obj = &(pFontBase->pFontArray[FontID].pType1Data->fontInfoP[FONTBBOX].value.data.arrayP[3]);
  outbox.ury =
    objPIsInteger(obj) ? obj->data.integer : obj->data.real > 0 ?
    (int) ceil(obj->data.real) : (int) floor(obj->data.real);
  
  return( outbox);
}

       

/* T1_GetAllCharNames(): Get a list of all defined character names in
   in the font FontID: */
char **T1_GetAllCharNames( int FontID)
{
  static char **bufmem=NULL;
  register char *namedest;
  psdict *pCharStrings;
  int len, i, j;
  long nameoffset;
  
  int bufmemsize=0;
  
  /* return NULL if font not loaded */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return( NULL);
  }
  
  pCharStrings=pFontBase->pFontArray[FontID].pType1Data->CharStringsP;

  /* First, get number of charstrings: */
  len=pCharStrings[0].key.len;

  /* We must be careful here: size of the charstrings dict might be larger
     than the actual number of charstrings. We correct for this by reducing
     the value of len appropriately */
  for ( i=1; i<=len; i++){
    /* calculate room for each characters name plus the prepending \0 */ 
    if ((j=pCharStrings[i].key.len)){
      bufmemsize += j + 1;
    }
    else{ /* we skip this (the remaining) entries */
      len--;
      i--;
    }
  }
  /* Now we reserve memory for the pointers (including final NULL) */
  nameoffset=(len+1)*sizeof( char *);
  bufmemsize += nameoffset;

  /* Now allocate memory, copy strings and initialize pointers */
  if (bufmem!=NULL)
    free(bufmem);
  if ((bufmem=(char **)malloc( bufmemsize))==NULL){
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  namedest=(char *)((long)bufmem + nameoffset);
  j=0;
  for ( i=0; i<len; i++){
    bufmem[i]=&(namedest[j]);
    strncpy( &(namedest[j]), pCharStrings[i+1].key.data.nameP,
	     pCharStrings[i+1].key.len);
    j += pCharStrings[i+1].key.len;
    namedest[j++]='\0';
  }
  bufmem[i++]=NULL;
  
  return( bufmem);
  
}



/* T1_GetNoKernPairs(): Return the number of kerning pairs defined
   for font FontID */
int T1_GetNoKernPairs( int FontID)
{
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return( -1);
  }

  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( -1);
  }

  return( pFontBase->pFontArray[FontID].pAFMData->numOfPairs);
  
}



/* A function for comparing METRICS_ENTRY structs */
static int cmp_METRICS_ENTRY( const void *entry1, const void *entry2)
{
  if (((METRICS_ENTRY *)entry1)->chars <
      ((METRICS_ENTRY *)entry2)->chars)
    return(-1);
  if (((METRICS_ENTRY *)entry1)->chars >
      ((METRICS_ENTRY *)entry2)->chars)
    return(1);
  return(0); /* This should not happen */
}



/* A few functions for accessing composite character data: */
/* T1_GetNoCompositeChars(): Return the number of characters for
   for which composite character information is available
   for font FontID */
int T1_GetNoCompositeChars( int FontID)
{
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return( -1);
  }

  /* If no AFM info is present, we return an error */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( -1);
  }

  return( pFontBase->pFontArray[FontID].pAFMData->numOfComps);
  
}



/* T1_QueryCompositeChar(): Query whether char1 from font FontID
   is a composite character. If so, the index of the composite
   character data within the afm array is returned. The index can
   be used to retrieve the retrieve the composite character data.

   retval>=0:    index into AFM-array where the corresponding
                 composite char data is located
   retval=-1:    No composite character, but result is valid,
   retval=-2:    No composite character, but result is invalid.
                 T1_errno indicated the reason.
*/
int T1_QueryCompositeChar( int FontID, char char1) 
{
  unsigned char uchar1;

  uchar1=char1;
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return( -2);
  }

  /* If no AFM info is present, we return -2 */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( -2);
  }

  if (pFontBase->pFontArray[FontID].pEncMap[ uchar1]<0) { /* composite char */
    return( -(pFontBase->pFontArray[FontID].pEncMap[(int) uchar1]+1));
  }

  return(-1);
  
}



/* T1_GetCompCharData(): Retrieve data to construct composite
   character char1 from font FontID. In case of an error NULL is returned
   and T1_errno is set appropriately. */
T1_COMP_CHAR_INFO *T1_GetCompCharData( int FontID, char char1)
{
  T1_COMP_CHAR_INFO *cci=NULL;
  CompCharData *ccd=NULL;
  int afmind=-1;
  int i;
  unsigned char uchar1;
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return( cci);
  }

  /* If no AFM info is present, we return -2 */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( cci);
  }

  if ((cci=(T1_COMP_CHAR_INFO*)malloc( sizeof(T1_COMP_CHAR_INFO)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return( cci);
  }
  
  uchar1=(unsigned char)char1;
  
  /* set default values */
  cci->compchar=uchar1;
  cci->numPieces=1;
  cci->pieces=NULL;

  /* check char1 */
  if ((afmind=pFontBase->pFontArray[FontID].pEncMap[uchar1]) >= 0) {
    /* char is no composite char */
    return(cci);
  }
  
  /* character is a composite char-> retrieve index and pointer into
     AFM data */
  afmind=-(afmind+1);
  ccd=&(pFontBase->pFontArray[FontID].pAFMData->ccd[afmind]);

  /* cci->compchar is already setup correctly because char1 is a
     composite character */
  cci->numPieces=ccd->numOfPieces;
  /* we expect numPieces to be >1 */
  if ((cci->pieces=(T1_COMP_PIECE *)malloc( sizeof(T1_COMP_PIECE)*
					    cci->numPieces))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    free( cci);
    return( NULL);
  }
  /* Copy information */
  for (i=0; i<cci->numPieces; i++) {
    cci->pieces[i].piece=T1_GetEncodingIndex( FontID, ccd->pieces[i].pccName);
    cci->pieces[i].deltax=ccd->pieces[i].deltax;
    cci->pieces[i].deltay=ccd->pieces[i].deltay;
  }
  return( cci);
  
}



/* T1_GetCompCharDataByIndex(): Retrieve data to construct composite
   characters form font FontID. The data is addressed by index which
   may, for example, have been obtained by a call to
   T1_QueryCompositeChar().
   In case of error NULL is returned and T1_errno is set appropriately.
*/
T1_COMP_CHAR_INFO *T1_GetCompCharDataByIndex( int FontID, int index)
{
  T1_COMP_CHAR_INFO *cci=NULL;
  CompCharData *ccd=NULL;
  int i;
  
  /* Check whether font is loaded: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return( cci);
  }

  /* If no AFM info is present, we return -2 */
  if (pFontBase->pFontArray[FontID].pAFMData==NULL) {
    T1_errno=T1ERR_NO_AFM_DATA;
    return( cci);
  }

  /* range check for index */
  if ((index < 0) ||
      (index >= pFontBase->pFontArray[FontID].pAFMData->numOfComps)) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return( cci);
  }

  /* Alloc mem */
  if ((cci=(T1_COMP_CHAR_INFO*)malloc( sizeof(T1_COMP_CHAR_INFO)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return( cci);
  }
  
  /* set source pointer */
  ccd=&(pFontBase->pFontArray[FontID].pAFMData->ccd[index]);
  /* and copy information */
  cci->compchar=T1_GetEncodingIndex( FontID, ccd->ccName);
  cci->numPieces=ccd->numOfPieces;
  /* we expect numPieces to be >1 */
  if ((cci->pieces=(T1_COMP_PIECE *)malloc( sizeof(T1_COMP_PIECE)*
					    cci->numPieces))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    free( cci);
    return( NULL);
  }
  /* Copy information */
  for (i=0; i<cci->numPieces; i++) {
    cci->pieces[i].piece=T1_GetEncodingIndex( FontID, ccd->pieces[i].pccName);
    cci->pieces[i].deltax=ccd->pieces[i].deltax;
    cci->pieces[i].deltay=ccd->pieces[i].deltay;
  }
  return( cci);
  
}



/* T1_IsInternalChar(): Query whether the character in encoding slot
   char1 of font FontID has an internal definition (CharString) or
   whether it is constructed by t1lib from elementary units */
int T1_IsInternalChar( int FontID, char char1)
{
  unsigned char uchar1;
  char *charname;
  psdict *pCharStrings;
  int len, i, j;
  
  /* return NULL if font not loaded */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return( -1);
  }
  
  pCharStrings=pFontBase->pFontArray[FontID].pType1Data->CharStringsP;
  uchar1=(unsigned char)char1;

  charname=T1_GetCharName( FontID, uchar1);
  
  /* First, get the maximum number of charstrings: */
  len=pCharStrings[0].key.len;

  /* Check all CharString definitions */
  for ( i=1; i<=len; i++) {
    /* if len=0, then the CharStrings dict is larger that required which
       is valid and allowed by the spec.*/ 
    if ((j=pCharStrings[i].key.len)!=0) {
      if ( (j==strlen(charname)) &&
	   (strncmp( charname, pCharStrings[i].key.data.nameP, j)==0) ) {
	/* we have found an internal definition */
	return( 1);
      }
    }
  }
  return( 0);
}
