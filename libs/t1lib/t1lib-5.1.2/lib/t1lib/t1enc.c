/*--------------------------------------------------------------------------
  ----- File:        t1enc.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-01
  ----- Description: This file is part of the t1-library. It contains
                     functions encoding handling at runtime.
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
  
#define T1ENC_C


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER)
# include <io.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>


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
#include "t1enc.h"
#include "t1env.h"
#include "t1base.h"
#include "t1finfo.h"


static char defaultencodingname[]="Unspecified";

/* This struct is retunred by the tokenizer. It contains indices
   in linebuf for the first and the last character in a token */ 
typedef struct 
{
  int first;
  int last;
} CNTOKEN;



/* ScanForWord(): Tokenizer for ScanEncodingFile.

   - and return the first and last index in linebuf of the token
   
   - skips whitespace and comments

   - the vector marks [ and ] are considered token-delimiters
     and also treated as independent tokens

   - the literal escape char "/" is also considered as a token
     delimiter but is not returned as a token.

   This function leaves linebuf unmodified so that in case of a
   failure, TryT1LibEncoding() will receive a clean buffer!  */
static CNTOKEN *ScanForWord( char *lb, int size)
{
  static int i=-1;
  int j;
  int comment;
  
  static CNTOKEN currtoken={-1,-1};
  
  /* Reset tokenizer */
  if (lb==NULL) {
    i=-1;
    currtoken.first=-1;
    currtoken.last=-1;
    return NULL;
  }

  comment=0;
  j=-1;

  while (++i<size) {

    /* Inside word */
    if (j!=-1) {
      /* Whitespace, comment, mark or literal ends word */
      if ( (lb[i]=='%') ||
	   (lb[i]=='[') ||
	   (lb[i]==']') ||
	   (lb[i]=='/') ||
	   isspace((int)lb[i]) ) {
	currtoken.last=i-1;
	if ( (lb[i]=='[') ||
	     (lb[i]==']') ||
	     (lb[i]=='/') ) {
	  i--;
	}
	return &currtoken;
      }
      /* End not found, try next char */
      continue;
    }
    
    /* first, check for special tokens */
    if ( ( (lb[i]=='[') || (lb[i]==']')) ) {
      currtoken.first=i;
      currtoken.last=i;
      return &currtoken;
    }

    /* Inside coment */
    if (comment) {
      if (lb[i]=='\n')
	comment=0;
      continue;
    }
    if (lb[i]=='%') {
      comment=1;
      continue;
    }

    /* **Whitespace */
    if (isspace((int)lb[i]))
      continue;

    /* **Something else => word */
    if (j==-1) {
      j=i;
      currtoken.first=j;
      continue;
    }
  }

  /* We're at the end of the buffer. Do we have a word? */
  if (j!=-1) {
    currtoken.last=i-1;
    return &currtoken;
  }

  /* We have nothing */
  return NULL;
}


/* tokcpy(): Copy a token from linebuf and append \0 */
static char *tokcpy( char *dest, const char *src,
		    int first, int last) 
{
  /* we do not do any range or error checking in this function */
  memcpy( dest, &(src[first]), last-first+1);
  dest[last-first+1]='\0';
  return dest;
}


/* TryDVIPSEncoding(): Try to read an encoding file conforming to the
   dvips specification. The file's contents is expected in a buffer
   "linebuf" of size "filesize". Function returns the actual size of the
   charnames memory or -1. */
static int TryDVIPSEncoding( char *linebuf, int filesize, char *charnames) 
{

  char token[256];
  char *encname;
  int charname_count=0;
  int k=0;
  CNTOKEN *currtokenP;
  
  
  /* Initialize tokenizer */
  currtokenP=ScanForWord( NULL, filesize);

  
  currtokenP=ScanForWord(linebuf,filesize);
  if ( (currtokenP!=NULL) &&
       (linebuf[currtokenP->first]=='/')) { 
    /* / indicates start of postscript string literal, so this
       could be a postscript .enc file */
    if ((encname=(char *)malloc( (currtokenP->last - currtokenP->first + 1 + 1) *
				 sizeof( char)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return -1;
    }
    else {
      /* store encoding name */
      if (currtokenP->first==currtokenP->last) {
	/* PostScript encoding requires an identifier
	   so this does not seem to be a valid encoding file */
	free( encname);
	return( -1);
      }
      tokcpy( encname, linebuf, currtokenP->first+1, currtokenP->last);
      
    }
    
    
    /* Next, the PostScript "mark" character is expected */
    if ( ((currtokenP=ScanForWord(linebuf,filesize))!=NULL) &&
	 (currtokenP->first==currtokenP->last) &&
	 (linebuf[currtokenP->first]!='[') ) {
      /* Since we got up to here, there is a certain probability that
	 we have a PostScript encoding definition, but with invalid
	 syntax. So put log message. */
      sprintf( err_warn_msg_buf,
	       "Expected encoding definition after %s, but did not find \"[\"-character",
	       encname);
      T1_PrintLog( "TryDVIPSEncoding()", err_warn_msg_buf, T1LOG_WARNING);
      if (encname!=NULL)
	free( encname);
      return( -1);
    }

    /* now, try to read 256 literal character names. We do not check
       for charname count because this would bypass error checking. */
    while((currtokenP=ScanForWord(linebuf,filesize))!=NULL) {

      /* End of vector operator? */
      if ( (currtokenP->first==currtokenP->last) && /* one character? */
	   (linebuf[currtokenP->first]==']')) {
	break;
      }
      /* We drop the escape character if it is present. However,
	 non-literal name specifications are not allowed in
	 encoding vectors. */
      if (linebuf[currtokenP->first]!='/') {
	sprintf( err_warn_msg_buf,
		 "Found non-literal name (c=%c (first=%d, last=%d)) at slot %d while scanning encoding vector %s.",
		 linebuf[currtokenP->first],
		 currtokenP->first, currtokenP->last,
		 charname_count, encname);
	T1_PrintLog( "TryDVIPSEncoding()", err_warn_msg_buf, T1LOG_WARNING);
	if (encname!=NULL)
	  free( encname);
	return( -1);
      }
      else {
	/* OK, first char in token is "/". Does there follow a name? */
	if ( currtokenP->first==currtokenP->last) {
	  sprintf( err_warn_msg_buf,
		   "Empty literal name at slot %d while scanning encoding vector %s.",
		   charname_count, encname);
	  T1_PrintLog( "TryDVIPSEncoding()", err_warn_msg_buf, T1LOG_WARNING);
	  if (encname!=NULL)
	    free( encname);
	  return( -1);
	}
      }
      
      /* We seem to have a valid name -> copy name to *charnames-array */
      tokcpy( &(charnames[k]), linebuf,
	      currtokenP->first+1, currtokenP->last);
      k+=currtokenP->last-currtokenP->first+1; /* note: we have omitted "/"! */
      /* Increment character counter */
      charname_count++;
      /* set index to start of next word/line */
    } /* end of loop scanning character names */
    
    if (currtokenP==NULL) {
      /* In this case loop has been stopped because buffer end has been
	 reached. Since we did not alreay read the ]-character, this is
	 an error condition. */
      sprintf( err_warn_msg_buf,
	       "Premature end of encoding definition encountered." );
      T1_PrintLog( "TryDVIPSEncoding()", err_warn_msg_buf, T1LOG_WARNING);
      free(encname);
      return( -1);
    }
    /* Since the above loop has not been finished due to a NULL-ptr,
       the token ] must have been encountered. Thus, read ahead and look
       for def: */
    if ((currtokenP=ScanForWord(linebuf,filesize))==NULL) {
      sprintf( err_warn_msg_buf,
	       "Premature end of encoding definition encountered.");
      T1_PrintLog( "TryDVIPSEncoding()", err_warn_msg_buf, T1LOG_WARNING);
      free(encname);
      return( -1);
    }
    
    if (strcmp(tokcpy( &(charnames[k]), linebuf,
		       currtokenP->first, currtokenP->last), "def")!=0) {
      /* we store the current token in charnames only temporarily, so we
	 do not increment k! */   
      sprintf( err_warn_msg_buf,
	       "Closing token \"def\" expected but found \"%s\".",
	       &(charnames[k]));
      T1_PrintLog( "TryDVIPSEncoding()", err_warn_msg_buf, T1LOG_WARNING);
      free(encname);
      return( -1);
    }
    /* Encoding definition is complete. we do not allow any further tokens
       except comments. */
    if ((currtokenP=ScanForWord(linebuf,filesize))!=NULL) {
      tokcpy( token, linebuf, currtokenP->first, currtokenP->last);
      sprintf( err_warn_msg_buf,
	       "Token \"%s\" after closing \"def\" in successfully scanned encoding file makes encoding definition file invalid", token);
      T1_PrintLog( "TryDVIPSEncoding()", err_warn_msg_buf, T1LOG_WARNING);
      free(encname);
      return( -1);
    }

    /* we allow less than 256 character names. The missing ones are filled
       now with .notdef */
    for ( ; charname_count<256; charname_count++) {
      tokcpy( &(charnames[k]), ".notdef", 0, 6);
      k+=8;
     }
    
    /* Append the string for the encoding's name */
    strcpy( &(charnames[k]), encname);
    k +=strlen(encname)+1;
    
    free(encname);
    
    return( k);
      
  } /* file does not start with / -> no dvips-encoding file */

  return( -1);
  
}



/* TryT1LibEncoding(): Try to read an encoding file conforming to the
   t1lib specification. The file's contents is expected in a buffer
   "linebuf" of size "filesize". Function returns the actual size of the
   charnames memory or -1. */
static int TryT1LibEncoding( char *linebuf, int filesize, char *charnames) 
{
  int i=0, j=0, k=0, l=0;
  
  char save_char;
  int charname_count=0;

  while(i<filesize){
    j=i;     /* Save index of beginning of line */
    while ( (i<filesize) &&
	    (isspace((int)linebuf[i])==0) )
      i++;
    if (i==filesize)
      continue; /* this will leave this loop */
    save_char=linebuf[i];
    linebuf[i]=0;  /* replace ' ' by ASCII-0 */
    if (strncmp( "Encoding=", &linebuf[j], strlen("Encoding="))==0) {
      /* We save the current file position to read the encoding name
	 later */
      l=j+strlen("Encoding=");
      /* set index to start of next line */
      if (save_char=='\n')
	i++;   
      else {
	while ( (i<filesize) &&
		(linebuf[i]!='\n') )
	  i++;
	if (i==filesize)
	  continue; /* this will leave this loop */
	i++;
      }
      /* keyword found => now, 256 lines should follow, each
	 specifying a character name and optionally some comment
	 to enhance readability: */
      break;
    }
    i++;
  }

  
  while((i<filesize) && (charname_count<256)){
    j=i;     /* Save index of beginning of line */
    while ( (i<filesize) &&
	    (isspace((int)linebuf[i])==0))
      i++;
    if (i==filesize)
      continue; /* this will leave this loop */
    save_char=linebuf[i];
    linebuf[i]=0;  /* replace whitespace by ASCII-0 */
    /* Copy Name to *char_names-array */
    while (linebuf[j])
      charnames[k++]=linebuf[j++];
    /* Append ASCII-0 */
    charnames[k++]=linebuf[j++];
    /* set index to start of next line */
    if (save_char=='\n')
      i++;   
    else {
      while ( (i<filesize) &&
	      (linebuf[i]!='\n'))
	i++;
      if (i==filesize)
	continue; /* this will leave this loop */
      i++;
    }
    /* Increment character counter */
    charname_count++;
  }

  /* Check if exactly 256 characters have been defined, if not,
     return NULL: */
  if (charname_count!=256){
    return( -1);
  }

  /* Append the string for the encoding's name */
  i=l;
  while (isspace((int)linebuf[i])==0 && linebuf[i]!='\0'){
    charnames[k++]=linebuf[i++];
  }
  
  if (i==l){
    strcpy(&(charnames[k]), defaultencodingname);
    k +=strlen(defaultencodingname+1);
    charnames[k++]='\0';
  }
  else{
    charnames[k++]='\0';
  }

  return( k);
  
}



/* ScanEncodingFile(): Read an encoding file of an appropriate format
   and prepare the read data for usage with the type1 rasterizer, i.e.
   generate an array char *enc[257]. Return the pointer to the data area
   or NULL in case of an error.
   */
static char **ScanEncodingFile( char *FileName) 
{
  
  char *linebuf;
  char **encoding;        /* charnames array */
  char *charnames=NULL;   /* charnames memory */
  
  int cnsize;             /* size of charnames memory, this will be
			     set from the Try...() function */
  
  FILE *enc_fp; 
  int filesize;
  int i=0, j=0;
  
  if ((enc_fp=fopen( FileName,"rb"))==NULL){
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(NULL);  /* file could not be opened 
		      => no encoding read */
  }
  
  
  /* enc_fp points now to a (hopefully) valid encoding file */
  /* Get the file size */
  fseek( enc_fp, 0, SEEK_END);
  filesize=ftell(enc_fp);
  /* Reset fileposition to start */
  fseek( enc_fp, 0, SEEK_SET);
  
  if ((linebuf=(char *)calloc( filesize,
			       sizeof(char)))==NULL){
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  /* Allocate space for character names, assume the worst case and realloc
     later. The DVIPS-parser requires one more char in order to work properly */
  if ((charnames=(char *)calloc( filesize + strlen(defaultencodingname+1),
				 sizeof(char)))==NULL){
    free( linebuf);
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  fread((char *)linebuf, sizeof(char), filesize, enc_fp);
  fclose(enc_fp);


  /* file is read. Operate now on the buffer. */

  
  /* try dvips encoding file */
  cnsize=TryDVIPSEncoding( linebuf, filesize, charnames);
  if ( cnsize>-1) {
    /* a debug message to log file */
    sprintf( err_warn_msg_buf,
	     "Scanned file %s (%d bytes) as dvips-encoding file.",
	     FileName, filesize);
    T1_PrintLog( "ScanEncodingFile()", err_warn_msg_buf, T1LOG_DEBUG);
  }
  else {
    /* try t1lib encoding file */
    cnsize=TryT1LibEncoding( linebuf, filesize, charnames);
    if ( cnsize>-1) {
      /* write a debug message to log file */
      sprintf( err_warn_msg_buf,
	       "Scanned file %s (%d bytes) as t1lib-encoding file.",
	       FileName, filesize);
      T1_PrintLog( "ScanEncodingFile()", err_warn_msg_buf, T1LOG_DEBUG);
    }
    else {
      /* write a warning message because loading encoding
	 entirely failed */ 
      sprintf( err_warn_msg_buf,
	       "Scanning file %s (%d bytes) as encoding file failed.",
	       FileName, filesize);
      T1_PrintLog( "ScanEncodingFile()", err_warn_msg_buf, T1LOG_WARNING);
    }
  }
  
  if ( cnsize<0) {
    T1_errno=T1ERR_SCAN_ENCODING;
    if ( charnames!=NULL) {
      free(charnames); 
    }
    free(linebuf);
    return( NULL);
  }
  
  
  /* cnsize contains the size of the charnames' memory, so let's
     now realloc charnames */
  charnames=(char *)realloc( charnames, cnsize*sizeof(char));
  /* Now initialize the array with the start-addresses of the character
     name strings */
  /* We alloc 257 to save the encoding's name at the 257th entry */
  if ((encoding=(char **)malloc(257*sizeof(char *)))==NULL) {
    if ( charnames!=NULL) {
      free(charnames);
    }
    free(linebuf);
    T1_errno=T1ERR_ALLOC_MEM;
    return(NULL);
  }
  
  while (i<257) {
    encoding[i]=&charnames[j];
    while (charnames[j])
      j++;
    j++;
    i++;
  }
  
  free( linebuf);
  return(encoding);

}



/* T1_LoadEncoding(): Load an encoding file to have a new encoding
   available. If successful, the pointer to the encoding array is
   returned. In case of an error, the return value is NULL.
   */
char **T1_LoadEncoding( char *FileName)
{
  char **Encoding;
  char *EncFileName;
  
  if( FileName==NULL){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(NULL);
  }
  
  if ((EncFileName=intT1_Env_GetCompletePath( FileName, T1_ENC_ptr))==NULL){
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(NULL);
  }
  Encoding=ScanEncodingFile(EncFileName);
  free(EncFileName);
  
  return(Encoding);
}

  

/* T1_DeleteEncoding() free a previously loaded encoding */
int T1_DeleteEncoding( char **encoding)
{
  if (encoding){
    /* First free character names memory */
    free( encoding[0]);
    /* then, free pointer array */
    free( encoding);
  }
  return(0);
  
}



/* T1_ReencodeFont(): Assign a new encoding to an existent font. This is
   only allowed if no size dependent data exists for the font in question.
   Moreover, the font must be loaded already since must get the position
   of the space-character. Function returns 0 if successful, and -1 otherwise.
   */
int T1_ReencodeFont( int FontID, char **Encoding)
{
  int i, j, k, l, m;
  char *charname;
  PairKernData *pkd;
  METRICS_ENTRY *kern_tbl;
  int char1, char2;
  
  
  /* First, check for valid font ID residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }
  
  /* Second, check whether size-dependent data exists: */
  if (pFontBase->pFontArray[FontID].pFontSizeDeps != NULL){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }
  
  pFontBase->pFontArray[FontID].pFontEnc=Encoding;


  /* We have to update the space_position-entry in the FONTPRIVATE. 
     If space is not found (not encoded), set it to -1: */
  pFontBase->pFontArray[FontID].space_position=-1;
  i=0;
  if (Encoding){ /* external encoding */
    while (i<256){
      if (strcmp( (char *)pFontBase->pFontArray[FontID].pFontEnc[i],
		  "space")==0){
	/* space found at position i: */
	pFontBase->pFontArray[FontID].space_position=i;
	break;
      }
      i++;
    }
  }
  else{ /* reencoding to internal encoding */
    while (i<256){
      if (strcmp( (char *)pFontBase->pFontArray[FontID].pType1Data->fontInfoP[ENCODING].value.data.arrayP[i].data.arrayP,
		  "space")==0){
	/* space found at position i: */
	pFontBase->pFontArray[FontID].space_position=i;
	break;
      }
      i++;
    }
  }

  /* Now update afm index mapping: */
  if (pFontBase->pFontArray[FontID].pAFMData != NULL){
    for (i=0; i<256; i++) {
      charname=T1_GetCharName( FontID, i);
      /* in a first loop check for ordinary characters */
      for ( j=0; j<pFontBase->pFontArray[FontID].pAFMData->numOfChars; j++) {
	if (strcmp( charname,
		    pFontBase->pFontArray[FontID].pAFMData->cmi[j].name)==0) {
	  pFontBase->pFontArray[FontID].pEncMap[i]=j+1; /* index 0 is reserved! */
	  continue;
	}
      }
      /* if nothing has been found, check for composite characters */ 
      for ( j=0; j<pFontBase->pFontArray[FontID].pAFMData->numOfComps; j++) {
	if (strcmp( charname,
		    pFontBase->pFontArray[FontID].pAFMData->ccd[j].ccName)==0) {
	  pFontBase->pFontArray[FontID].pEncMap[i]=-(j+1); /* index 0 is reserved! */
	  /* Note: Metrics of composite characters already exist so that there is
	     no need to recalculate them! */
	  continue;
	}
      }
    }
    /* Update kerning table */
    pFontBase->pFontArray[FontID].KernMapSize=0;
    k=pFontBase->pFontArray[FontID].pAFMData->numOfPairs;
    if (k>0){ /* i.e., there are any pairs */
      /* OK, it does not suffice to alloc numOfPairs METRICS_ENTRYs, because
	 a given character might be encoded at several locations and kerning
	 should still work. As a worst case estimation, we allocate 256^2
	 and realloc later. */ 
      if ((pFontBase->pFontArray[FontID].pKernMap=
	   (METRICS_ENTRY *)malloc( (256*256) *sizeof( METRICS_ENTRY)))==NULL){
	sprintf( err_warn_msg_buf, "Error allocating memory for metrics map (FontID=%d)",
		 FontID);
	T1_PrintLog( "T1_LoadFont()", err_warn_msg_buf,
		     T1LOG_WARNING);
	T1_errno=T1ERR_ALLOC_MEM;
	return(-1);
      }
      kern_tbl=pFontBase->pFontArray[FontID].pKernMap;
      pkd=pFontBase->pFontArray[FontID].pAFMData->pkd;
      j=0;
      for ( i=0; i<k; i++) {
	/* We do not check T1_GetEncodingIndices() against the return value
	   NULL because we just loading the font in question: */
	l=0;
	while ((char1=(T1_GetEncodingIndices( FontID, pkd[i].name1))[l++])!=-1) {
	  /* pair could be relevant in current encoding */
	  m=0;
	  while ((char2=(T1_GetEncodingIndices( FontID, pkd[i].name2))[m++])!=-1) {
	    /* Since we get here we have a relevant pair -->
	       Put char1 in higher byte and char2 in LSB: */
	    kern_tbl[j].chars=(char1 << 8) | char2;
	    /* We only make use of horizontal kerning */
	    kern_tbl[j].hkern=pkd[i].xamt;
	    j++;
	  } /* while (char2) */
	} /* while (char1) */
      } /* for */
      /* We are done, realloc memory: */
      kern_tbl=(METRICS_ENTRY*) realloc( kern_tbl, j*sizeof(METRICS_ENTRY));
      /* We now sort the kerning array with respect to char indices */
      qsort( kern_tbl, (size_t) j, sizeof(METRICS_ENTRY),
	     &cmp_METRICS_ENTRY );
      /* Finally write back pointer for the case that realloc changed the
	 pointer */
      pFontBase->pFontArray[FontID].pKernMap=kern_tbl;
      pFontBase->pFontArray[FontID].KernMapSize=j;
    }
    else {
      pFontBase->pFontArray[FontID].pKernMap=NULL;
    }
  }
  return(0);
}



/* T1_SetDefaultEncoding(): Set the default encoding vector that's
   used when fonts are loaded.
   */
int T1_SetDefaultEncoding( char **encoding)
{
  
  if (T1_CheckForInit()){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }
  
  pFontBase->default_enc=encoding;
  return(0);
}


/* T1_GetEncodingScheme(): Get the name associated with the current
   encoding vector of font FontID */
char *T1_GetEncodingScheme( int FontID)
{
  
  static char enc_scheme[256];
  
  /* First, check for valid font ID residing in memory: */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }

  if (pFontBase->pFontArray[FontID].pFontEnc==NULL){
    if (pFontBase->pFontArray[FontID].info_flags & USES_STANDARD_ENCODING){
      strcpy( enc_scheme, "StandardEncoding");
    }
    else {
      strcpy( enc_scheme, "FontSpecific");
    }
  }
  else
    strcpy( enc_scheme, pFontBase->pFontArray[FontID].pFontEnc[256]);
  
  return(enc_scheme);
  
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
    
