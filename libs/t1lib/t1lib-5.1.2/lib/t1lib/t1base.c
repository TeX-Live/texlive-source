/*--------------------------------------------------------------------------
  ----- File:        t1base.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2005-05-17
  ----- Description: This file is part of the t1-library. It contains basic
                     routines to initialize the data structures used
		     by the t1-library.
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


#define T1BASE_C


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
#include <ctype.h>
#include <stdarg.h>

#include "../type1/ffilest.h" 
#include "../type1/types.h"
#include "parseAFM.h"
#include "../type1/objects.h"
#include "../type1/spaces.h"
#include "../type1/util.h"
#include "../type1/fontfcn.h"
#include "../type1/paths.h"
#include "../type1/regions.h"


#include "sysconf.h"
#include "t1base.h"
#include "t1types.h"
#include "t1global.h"
#include "t1env.h"
#include "t1delete.h"


static int test_for_t1_file( char *buffer );
static int T1_pad=0;

/* A fix for Encoding Vector problem. Initialization / Deinitialization
   is now done in T1_InitLib() / T1-CloseLib. */
extern boolean Init_BuiltInEncoding( void);
extern psobj *StdEncArrayP;

static const char* T1errmsg[] = {
  "",                                                          /*                              -10 */
  "",                                                          /*                              -9  */
  "",                                                          /*                              -8  */
  "",                                                          /*                              -7  */
  "",                                                          /*                              -6  */
  "Attempt to Load Multiple Master Font",                      /* T1ERR_SCAN_FONT_FORMAT       -5  */
  "Type 1 Font File Open Error",                               /* T1ERR_SCAN_FILE_OPEN_ERR     -4  */
  "Virtual Memory Exceeded",                                   /* T1ERR_SCAN_OUT_OF_MEMORY     -3  */
  "Syntactical Error Scanning Font File",                      /* T1ERR_SCAN_ERROR             -2  */
  "Premature End of Font File Encountered",                    /* T1ERR_SCAN_FILE_EOF          -1  */
  "",                                                          /*                               0  */
  "Path Construction Error",                                   /* T1ERR_PATH_ERROR              1  */
  "Font is Corrupt",                                           /* T1ERR_PARSE_ERROR             2  */
  "Rasterization Aborted",                                     /* T1ERR_TYPE1_ABORT             3  */
  "",                                                          /*                               4  */
  "",                                                          /*                               5  */
  "",                                                          /*                               6  */
  "",                                                          /*                               7  */
  "",                                                          /*                               8  */
  "",                                                          /*                               9  */
  "Font ID Invalid in this Context",                           /* T1ERR_INVALID_FONTID          10 */
  "Invalid Argument in Function Call",                         /* T1ERR_INVALID_PARAMETER       11 */
  "Operation not Permitted",                                   /* T1ERR_OP_NOT_PERMITTED        12 */
  "Memory Allocation Error",                                   /* T1ERR_ALLOC_MEM               13 */
  "Error Opening File",                                        /* T1ERR_FILE_OPEN_ERR           14 */
  "Unspecified T1Lib Error",                                   /* T1ERR_UNSPECIFIED             15 */
  "Missing AFM Data",                                          /* T1ERR_NO_AFM_DATA             16 */
  "X11 Interface Error",                                       /* T1ERR_X11                     17 */
  "Missing Component of Composite Character"                   /* T1ERR_COMPOSITE_CHAR          18 */
  "Error Scanning Encoding File",                              /* T1ERR_SCAN_ENCODING           19 */
  "",                                                          /*                               20 */
};


/* T1_StrError(): Return an error message corresponding to the value of
   t1err. */
const char *T1_StrError( int t1err) 
{
  int errind;

  errind = t1err + 10;

  if ( errind < 0 )
    errind = 0;
  if ( errind > 29 )
    errind = 0;
  
  return T1errmsg[errind];
}



/* This function is to be called by the user to initialize
   the font mechanism */
void *T1_InitLib( int log)
{
  int result;
  int i;
  
  char *usershome=NULL;
  char *logfilepath=NULL;
  char *envlogreq=NULL;
  int  usrforcelog=0;

  /* Check against multiple initialization. */
  if ( T1_Up != 0 ) {
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return NULL;
  }
  
  /* Reset T1_errno */
  T1_errno=0;
  
  /* Assign pointer to global struct and set entry padding: */
  pFontBase=&FontBase;
  if (T1_pad)
    pFontBase->bitmap_pad=T1_pad;
  else
    pFontBase->bitmap_pad=T1GLYPH_PAD;

  pFontBase->pFontArray = NULL;
  pFontBase->t1lib_flags=0;
  /* Check for AA-caching */
  if ((log & T1_AA_CACHING)){
    pFontBase->t1lib_flags |= T1_AA_CACHING;
  }
  /* Check for AFM disable */
  if ((log & T1_NO_AFM)) {
    pFontBase->t1lib_flags |= T1_NO_AFM;
  }

  /* Check environment variable ENV_LOG_STRING. By this means, a user may
     generate a log file even if at compile time log file creation has
     been suppressed. Of course, if the loglevel is reduced after
     initialization by the programmer, this environment variable takes
     no effect! */
  if ((envlogreq=getenv(ENV_LOG_STRING))!=NULL) {
    if (strcmp( envlogreq, "logDebug")==0)
      T1_SetLogLevel( T1LOG_DEBUG);
    else if (strcmp( envlogreq, "logStatistic")==0)
      T1_SetLogLevel( T1LOG_STATISTIC);
    else if (strcmp( envlogreq, "logWarning")==0)
      T1_SetLogLevel( T1LOG_WARNING);
    else if (strcmp( envlogreq, "logError")==0)
      T1_SetLogLevel( T1LOG_ERROR);
    usrforcelog=1;
  }
  
  /* Open log-file: */
  t1lib_log_file=NULL;
  if ((log & LOGFILE) || (usrforcelog!=0)) {
    pFontBase->t1lib_flags |= LOGFILE;
    /* Try first opening in current directory: */
    if ((t1lib_log_file=fopen( T1_LOG_FILE, "wb"))==NULL) {
      if ((usershome=getenv("HOME"))!=NULL) {
	logfilepath=(char *)malloc((strlen(usershome) +
				    strlen(T1_LOG_FILE) + 2
				    ) * sizeof(char));
	strcpy( logfilepath, usershome);
	strcat( logfilepath, DIRECTORY_SEP);
	strcat( logfilepath, T1_LOG_FILE);
	if ((t1lib_log_file=fopen( logfilepath, "wb"))==NULL){
	  t1lib_log_file=stderr;
	}
	free( logfilepath);
      }
      else {
	t1lib_log_file=stderr;
      }
    }
    if (t1lib_log_file==stderr) {
      T1_PrintLog( "T1_InitLib()", "Unable to open a logfile, using stderr",
		   T1LOG_ERROR);
    }
  }
  
  T1_PrintLog( "T1_InitLib()", "Initialization started",
	       T1LOG_STATISTIC);
  /* Check for representation of data in memory: */
  if ((pFontBase->endian=T1_CheckEndian())){
    T1_PrintLog( "T1_InitLib()", "Using Big Endian data presentation (MSBFirst)",
		 T1LOG_DEBUG);
    pFontBase->endian=1;
  }
  else{
    T1_PrintLog( "T1_InitLib()", "Using Little Endian data presentation (LSBFirst)",
		 T1LOG_DEBUG);
    pFontBase->endian=0;
  }

  /* Save version identifier */
  sprintf( err_warn_msg_buf, "Version Identifier: %s", T1LIB_IDENT);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf,
	       T1LOG_DEBUG);
  /* Save how t1lib is initialized */
  sprintf( err_warn_msg_buf, "Initialization flags: 0x%X", log);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf,
	       T1LOG_DEBUG);
  /* Save padding value in log file */
  sprintf( err_warn_msg_buf, "Glyphs are padded to %d bits",
	   pFontBase->bitmap_pad);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf,
	       T1LOG_DEBUG);
#ifdef __CHAR_UNSIGNED__
  T1_PrintLog( "T1_InitLib()", "System-Info: char is unsigned",
	       T1LOG_DEBUG);
#else
  T1_PrintLog( "T1_InitLib()", "System-Info: char is signed",
	       T1LOG_DEBUG);
#endif
  sprintf( err_warn_msg_buf, "System-Info: sizeof(char):         %d",
	   SIZEOF_CHAR);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(short):        %d",
	   SIZEOF_SHORT);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(int):          %d",
	   SIZEOF_INT);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(long):         %d",
	   SIZEOF_LONG);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(long long):    %d",
	   SIZEOF_LONG_LONG);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(float):        %d",
	   SIZEOF_FLOAT);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(double):       %d",
	   SIZEOF_DOUBLE);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(long double):  %d",
	   SIZEOF_LONG_DOUBLE);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  sprintf( err_warn_msg_buf, "System-Info: sizeof(void *):       %d",
	   SIZEOF_VOID_P);
  T1_PrintLog( "T1_InitLib()", err_warn_msg_buf, T1LOG_DEBUG);
  
  intT1_SetupDefaultSearchPaths();
  if (log & IGNORE_CONFIGFILE) {
    pFontBase->t1lib_flags |= IGNORE_CONFIGFILE;
    T1_PrintLog( "T1_InitLib()", "Skipping configuration file search!",
		 T1LOG_STATISTIC);
  }
  else {
    if ((result=intT1_ScanConfigFile())==0)
      T1_PrintLog( "T1_InitLib()", "Warning t1lib configuration file not found!",
		   T1LOG_WARNING);
  }
  
  /* Initialize builtin Standard Encoding */
  if ( !(Init_BuiltInEncoding()) ) {
      T1_PrintLog( "T1_InitLib()", "Unable initialize internal StandardEncoding!",
		   T1LOG_ERROR);
  }
  
  /* Set the default encoding to the fonts' internal encoding */
  pFontBase->default_enc=NULL;
  
  /* Initialize the no_fonts... values */
  pFontBase->no_fonts=0;
  pFontBase->no_fonts_ini=pFontBase->no_fonts;
  pFontBase->no_fonts_limit=pFontBase->no_fonts;

  
  /* Check whether to read font database */
  if ((log & IGNORE_FONTDATABASE)){
    pFontBase->t1lib_flags |= IGNORE_FONTDATABASE;

    T1_Up=1;         /* System has been initialized ! */
    T1_PrintLog( "T1_InitLib()", "Initialization successfully finished (Database empty)",
		 T1LOG_STATISTIC);
    
    return((void *) pFontBase);
  }

  result=0;
  /* Read fontdatabase(s) */
  i=0;
  while (T1_FDB_ptr[i]!=NULL) {
    if ((result=intT1_scanFontDBase(T1_FDB_ptr[i]))==-1){
      T1_PrintLog( "T1_InitLib()", "Fatal error scanning Font Database File %s",
		   T1LOG_WARNING, T1_FDB_ptr[i]);
    }
    if (result>-1)
      pFontBase->no_fonts+=result;
    i++;
  }
  if ( (result == 0) && (i != 0)){
    T1_PrintLog( "T1_InitLib()", "No fonts from Font Database File(s) found (T1_errno=%d)",
		 T1LOG_ERROR, T1_errno);
    return(NULL);
  }

  result=0;
  /* Read XLFD fontdatabase(s) */
  i=0;
  while (T1_FDBXLFD_ptr[i]!=NULL) {
    if ((result=intT1_scanFontDBaseXLFD(T1_FDBXLFD_ptr[i]))==-1){
      T1_PrintLog( "T1_InitLib()", "Fatal error scanning XLFD Font Database File %s",
		   T1LOG_WARNING, T1_FDB_ptr[i]);
    }
    if (result>-1)
      pFontBase->no_fonts+=result;
    i++;
  }
  if ( (result == 0) && (i != 0)){
    T1_PrintLog( "T1_InitLib()", "No fonts from XLFD Font Database File(s) found (T1_errno=%d)",
		 T1LOG_ERROR, T1_errno);
    return(NULL);
  }

  /* Initialize the no_fonts... values */
  pFontBase->no_fonts_ini=pFontBase->no_fonts;
  pFontBase->no_fonts_limit=pFontBase->no_fonts;

  T1_Up=1;         /* System has been initialized ! */
  T1_PrintLog( "T1_InitLib()", "Initialization successfully finished",
	     T1LOG_STATISTIC);

  return((void *) pFontBase);
}



/* intT1_scanFontDBase():
   - opens the file with the font definitions,
   - reads the number of fonts defined and saves this in FontBase,
   - allocates memory for all the filenames of the Type1 files
   - tests for .pfa und .pfb files and saves the name found
   - initializes an array that allows to acces these names by an
     index number, the font_ID
   - returns -1 on fatal error and the number of fonts located
     successfully
   */
int intT1_scanFontDBase( char *filename)
{
  int fd;
  int filesize, i, j, k, m;
  int found=0, located=0;
  char *filebuffer;
  int nofonts=0;
  FONTPRIVATE* fontarrayP=NULL;
  
#ifndef O_BINARY
#  define O_BINARY 0x0
#endif

  if ((fd=open( filename, O_RDONLY | O_BINARY))<3){
    T1_PrintLog( "intT1_scanFontDBase()", "Font Database File %s not found!",
		 T1LOG_WARNING, filename);
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(-1);
  }

  /* Get the file size */
  filesize=lseek( fd, 0, 2);
  /* Reset fileposition to start */
  lseek (fd, 0, 0); 

  if ((filebuffer=(char *)malloc(filesize*sizeof(char)
				 )) == NULL){
    T1_PrintLog(  "intT1_scanFontDBase()",
		  "Couldn't allocate memory for loading font database file %s",
		  T1LOG_ERROR, filename);
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }
  
  i=read( fd, filebuffer, filesize);
  close(fd);   /* Close font database file */
   
  i=j=m=0;
  
  while (i<filesize) {
    if (filebuffer[i]=='\n'){ /* We are at the end of line */
      if (j==0) {  /* Read the first line as the number of fonts */
	filebuffer[i]=0;
	sscanf( &filebuffer[0], "%d", &nofonts);
	filebuffer[i]='\n';  /* Because it gives a better feeling */
	/* (Re)Allocate memory for 'no_fonts' structures: */ 
	if ((FontBase.pFontArray=(FONTPRIVATE *)
	     realloc( FontBase.pFontArray, (FontBase.no_fonts+nofonts)*sizeof(FONTPRIVATE))) == NULL) {
	  T1_PrintLog( "inT1_scanFontDBase()",
		       "Failed to allocate memory for FONTPRIVATE-area while scanning %s",
		       T1LOG_ERROR, filename);
	  T1_errno=T1ERR_ALLOC_MEM;
	  return(-1);
	}
	/* setup pointer to newly allocated area and do a reset */
	fontarrayP=&(FontBase.pFontArray[FontBase.no_fonts]);
	memset(fontarrayP, 0, nofonts*sizeof(FONTPRIVATE));
	located=1; /* In  order to increment m */
      }
      else {       /* We are in the second or higher line */
	k=i;
	while (isspace((int)filebuffer[k])){
	  k--;
	}
	/* We are at the last printable character of a line; let's 
	   step back (over the [afm/sfm]) to the period and replace it
	   by an ASCII 0 */
	while ((filebuffer[k]!='.') && (!isspace((int)filebuffer[k])) ){
	  k--;
	}
	if (filebuffer[k]=='.'){ /* We have a name terminated with . */
	  filebuffer[k]=0; /* termination for string reading */
	  while (!isspace((int)filebuffer[k])){
	    k--;
	  }
	}
	else { /* The filename was without . and / or the first on the line */ 
	  ;
	}
	sscanf( &(filebuffer[k+1]), "%s", &(linebuf[0]));
	/* We print error string before testing because after the call
	   to test_for_t1_file() filename is substituted by an emty
	   string if the file was not found: */
	sprintf( err_warn_msg_buf, "Type 1 Font file %s.[pfa/pfb] not found (FontID=%d, SearchPath=%s)",
		 linebuf, m-1, T1_GetFileSearchPath(T1_PFAB_PATH));
	if ((test_for_t1_file( &linebuf[0]))){
	  T1_PrintLog( "intT1_scanFontDBase()", err_warn_msg_buf, T1LOG_WARNING);
	  located=0;
	}
	else{
	  /* linebuf contains now the valid Type1 filename; let's
	     now copy this string into the appropriate place in the
	     FONTPRIVATE-struct: */
	  found++;
	  located=1;
	  if ((fontarrayP[m-1].pFontFileName=(char *)
	       calloc( strlen( &linebuf[0])+1, sizeof(char))) == NULL){
	    T1_PrintLog( "intT1_scanFontDBase()",
			 "Failed to allocate memory for Filename %s (FontID=%d)",
			 T1LOG_ERROR, &linebuf[0], m-1);
	    T1_errno=T1ERR_ALLOC_MEM;
	    return(-1);
	  }
	  strcpy( fontarrayP[m-1].pFontFileName, &linebuf[0]);
	}
      }
      j++; /* Advance line counter */
      if ((located))
	m++;
    }
    if (j>nofonts) /* to ignore especially white space at end */
      break;
    i++;   /* Step further in file position */
  }
  /* Return the memory for file reading */
  free(filebuffer); 
  
  return( found);
}


/* intT1_scanFontDBaseXLFD():
   - opens the file with the font definitions,
   - reads the number of fonts defined and saves this in FontBase,
   - allocates memory for all the filenames of the Type1 files
   - tests for .pfa und .pfb files and saves the name found
   - initializes an array that allows to acces these names by an
     index number, the font_ID
   - returns -1 on fatal error and the number of fonts located
     successfully.

     This function is identical to intT1_scanFontDBase() with the
     difference that it expects the database file to be in XLFD format,
     that is, the font's name comes in the first place stead of in
     the last.
   */
int intT1_scanFontDBaseXLFD( char *filename)
{
  int fd;
  int filesize, i, j, l, m;
  int found=0, located=0;
  char *filebuffer;
  int nofonts=0;
  FONTPRIVATE* fontarrayP=NULL;
  
#ifndef O_BINARY
#  define O_BINARY 0x0
#endif

  if ((fd=open( filename, O_RDONLY | O_BINARY))<3){
    T1_PrintLog( "intT1_scanFontDBaseXLFD()", "XLFD Font Database File %s not found!",
		 T1LOG_WARNING, filename);
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(-1);
  }

  /* Get the file size */
  filesize=lseek( fd, 0, 2);
  /* Reset fileposition to start */
  lseek (fd, 0, 0); 

  if ((filebuffer=(char *)malloc(filesize*sizeof(char)
				 )) == NULL){
    T1_PrintLog(  "intT1_scanFontDBaseXLFD()",
		  "Couldn't allocate memory for loading XLFD font database file %s",
		  T1LOG_ERROR, filename);
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }
  
  i=read( fd, filebuffer, filesize);
  close(fd);   /* Close XLFD font database file */
   
  i=j=l=m=0;
  
  while (i<filesize) {
    if (filebuffer[i]=='\n'){ /* We are at the end of line */
      if (j==0) {  /* Read the first line as the number of fonts */
	filebuffer[i]=0;
	sscanf( &filebuffer[0], "%d", &nofonts);
	filebuffer[i]='\n';  /* Because it gives a better feeling */
	/* (Re)Allocate memory for 'no_fonts' structures: */ 
	if ((FontBase.pFontArray=(FONTPRIVATE *)
	     realloc( FontBase.pFontArray, (FontBase.no_fonts+nofonts)*sizeof(FONTPRIVATE))) == NULL) {
	  T1_PrintLog( "inT1_scanFontDBaseXLFD()",
		       "Failed to allocate memory for FONTPRIVATE-area while scanning %s",
		       T1LOG_ERROR, filename);
	  T1_errno=T1ERR_ALLOC_MEM;
	  return(-1);
	}
	/* setup pointer to newly allocated area and do a reset */
	fontarrayP=&(FontBase.pFontArray[FontBase.no_fonts]);
	memset(fontarrayP, 0, nofonts*sizeof(FONTPRIVATE));
	located=1; /* In  order to increment m */
      }
      else {       /* We are in the second or higher line */
	/* l indexes the beginning of the line */
	while (isspace((int)filebuffer[l])){
	  l++;
	}
	/* We are at the first printable character of a line; 
	   scan the first white space terminated string as font
	   file name. */
	sscanf( &(filebuffer[l]), "%s", &(linebuf[0]));
	/* We print error string before testing because after the call
	   to test_for_t1_file() filename is substituted by an emty
	   string if the file was not found: */
	sprintf( err_warn_msg_buf, "Type 1 Font file %s.[pfa/pfb] not found (FontID=%d, SearchPath=%s)",
		 linebuf, m-1, T1_GetFileSearchPath(T1_PFAB_PATH));
	if ((test_for_t1_file( &linebuf[0]))){
	  T1_PrintLog( "intT1_scanFontDBase()", err_warn_msg_buf, T1LOG_WARNING);
	  located=0;
	}
	else{
	  /* linebuf contains now the valid Type1 filename; let's
	     now copy this string into the appropriate place in the
	     FONTPRIVATE-struct: */
	  found++;
	  located=1;
	  if ((fontarrayP[m-1].pFontFileName=(char *)
	       calloc( strlen( &linebuf[0])+1, sizeof(char))) == NULL){
	    T1_PrintLog( "intT1_scanFontDBaseXLFD()",
			 "Failed to allocate memory for Filename %s (FontID=%d)",
			 T1LOG_ERROR, &linebuf[0], m-1);
	    T1_errno=T1ERR_ALLOC_MEM;
	    return(-1);
	  }
	  strcpy( fontarrayP[m-1].pFontFileName, &linebuf[0]);
	}
      }
      j++; /* Advance line counter */
      if ((located))
	m++;
      /* Store index to new line */
      l = i + 1;
    }
    if (j>nofonts) /* to ignore especially white space at end */
      break;
    i++;   /* Step further in file position */
  }
  /* Return the memory for file reading */
  free(filebuffer); 
  
  return( found);
}


/* T1_CloseLib(): Close the library and free all associated memory */
int T1_CloseLib( void)
{

  int i, j, error=0;

  if ( T1_Up != 0 ) {
    for (i=pFontBase->no_fonts; i; i--){
      /* Free filename only if not NULL and if the font is physical!
	 Do it before removing the font since the physical information
	 is no more available afterwards. If necessary, an explicitly
	 specified AFM filename is also freed.
	 */
      if ((pFontBase->pFontArray[i-1].pFontFileName!=NULL)
	  && (pFontBase->pFontArray[i-1].physical==1)){
	free( pFontBase->pFontArray[i-1].pFontFileName);
	pFontBase->pFontArray[i-1].pFontFileName=NULL;
	if (pFontBase->pFontArray[i-1].pAfmFileName!=NULL){
	  free( pFontBase->pFontArray[i-1].pAfmFileName);
	  pFontBase->pFontArray[i-1].pAfmFileName=NULL;
	}
      }
      
      /* Now, remove font: */
      if ((j=T1_DeleteFont( i-1))){
	error=1;
	sprintf( err_warn_msg_buf, "T1_DeleteFont() returned %d for Font %d",
		 j, i-1);
	T1_PrintLog( "T1_CloseLib()", err_warn_msg_buf, T1LOG_ERROR);
      }
    }
    /* Free the FONTPRIVATE area */
    if (pFontBase->pFontArray)
      free( pFontBase->pFontArray);
    else
      error=1;

    /* Get rid of internal StandardEncoding vector */
    if ( StdEncArrayP != NULL ) {
      free( StdEncArrayP);
      StdEncArrayP = NULL;
    }

    /* Free search paths */
    intT1_FreeSearchPaths();

    /* Reset the flags */
    pFontBase->t1lib_flags=0;
    
    /* Indicate Library is no longer initialized */
    pFontBase=NULL;
    T1_Up=0;
    T1_PrintLog( "T1_CloseLib()", "Library closed", T1LOG_STATISTIC);
    if ((t1lib_log_file!=NULL) && (t1lib_log_file!=stderr))
      fclose(t1lib_log_file);
    t1lib_log_file=NULL;
  }
  
  return( error);
}


/* T1_AddFont(): Add a new fontfile to the fontdatabase.
   Return values: >0: Assigned FontID
                  -1: Fontfile not found
		  -2: Error allocating memory for FONTPRIVATE-area
		  -3: No memory for saving font filename
		  */
int T1_AddFont( char *fontfilename)
{
  
  char *FullName;
  FONTPRIVATE *save_ptr;
  int i;
  int new_ID;
  
  
  if (fontfilename==NULL){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }
  
  /* Check for existence of fontfile */
  if ((FullName=intT1_Env_GetCompletePath(fontfilename,T1_PFAB_ptr))==NULL) {
    T1_errno=T1ERR_FILE_OPEN_ERR;
    return(-1);
  }
  free(FullName);

  /* Check if free space for a new FONTPRIVATE is available; if not,
     realloc memory some amount larger */
  save_ptr=pFontBase->pFontArray;
  if (pFontBase->no_fonts==pFontBase->no_fonts_limit){
    if (pFontBase->pFontArray == NULL) {
      /* In case this is the first font */
      pFontBase->pFontArray=(FONTPRIVATE *)calloc(pFontBase->no_fonts_limit 
						  + ADVANCE_FONTPRIVATE,
						  sizeof(FONTPRIVATE));
    }
    else {
      /* We already have some fonts */
      pFontBase->pFontArray=(FONTPRIVATE *)realloc(pFontBase->pFontArray,
						   (pFontBase->no_fonts_limit
						    + ADVANCE_FONTPRIVATE)
						   * sizeof(FONTPRIVATE));
      if (pFontBase->pFontArray==NULL){
	/* Restore pointer */
	pFontBase->pFontArray=save_ptr;
	T1_errno=T1ERR_ALLOC_MEM;
	return(-2); /* No memory available */
      }
    }
    pFontBase->no_fonts_limit += ADVANCE_FONTPRIVATE;
    /* First, initialize newly allocated to be not used */
    for ( i=pFontBase->no_fonts;
	  i<pFontBase->no_fonts+ADVANCE_FONTPRIVATE; i++){
      pFontBase->pFontArray[i].pFontFileName=NULL;
      pFontBase->pFontArray[i].pAfmFileName=NULL;
      pFontBase->pFontArray[i].pAFMData=NULL;
      pFontBase->pFontArray[i].pType1Data=NULL;
      pFontBase->pFontArray[i].pEncMap=NULL;
      pFontBase->pFontArray[i].pKernMap=NULL;
      pFontBase->pFontArray[i].pFontEnc=NULL;
      pFontBase->pFontArray[i].pFontSizeDeps=NULL;
      pFontBase->pFontArray[i].vm_base=NULL;
      pFontBase->pFontArray[i].FontMatrix[0]=0.0;
      pFontBase->pFontArray[i].FontMatrix[1]=0.0;
      pFontBase->pFontArray[i].FontMatrix[2]=0.0;
      pFontBase->pFontArray[i].FontMatrix[3]=0.0;
      pFontBase->pFontArray[i].FontTransform[0]=0.0;
      pFontBase->pFontArray[i].FontTransform[1]=0.0;
      pFontBase->pFontArray[i].FontTransform[2]=0.0;
      pFontBase->pFontArray[i].FontTransform[3]=0.0;
      pFontBase->pFontArray[i].slant=0.0;
      pFontBase->pFontArray[i].extend=0.0;
      pFontBase->pFontArray[i].physical=0;
      pFontBase->pFontArray[i].refcount=0; 
      pFontBase->pFontArray[i].space_position=0; 
      pFontBase->pFontArray[i].info_flags=0; 
    }
  }
  /* no_fonts-1 was the largest allowed font ID */
  new_ID=pFontBase->no_fonts;
  pFontBase->no_fonts++;

  if ((FontBase.pFontArray[new_ID].pFontFileName=(char *)
       calloc( strlen( fontfilename)+1, sizeof(char))) == NULL){
    T1_PrintLog( "T1_AddFont()",
		 "Failed to allocate memory for Filename %s (FontID=%d)",
		 T1LOG_ERROR, fontfilename, new_ID);
    T1_errno=T1ERR_ALLOC_MEM;
    return(-3);
  }
  strcpy( FontBase.pFontArray[new_ID].pFontFileName, fontfilename);

  /* Generate logfile entry */
  sprintf( err_warn_msg_buf, "Assigned FontID %d to fontfile %s",
	   new_ID, FontBase.pFontArray[new_ID].pFontFileName);
  T1_PrintLog( "T1_AddFont()", err_warn_msg_buf,
	       T1LOG_STATISTIC);
  /* Return FontID of newly declared font */
  return( new_ID);
  
}


/* T1_PrintLog() generates entries in the log file. msg_txt is subject to scan
   conversion and ... signifies a accordingly lrge variable list. */
void T1_PrintLog( char *func_ident, char *msg_txt, int level, ...)
{
  va_list vararg;
  static char levelid[4]={ 'E', 'W', 'S', 'D'};
  time_t s_clock, *tp;
  
  if (t1lib_log_file==NULL)
    return;
  if ((level>t1lib_log_level) || (level<1)){
    return;
  }
  else{
    /* initialize argument list */
    va_start( vararg, level);
    
    tp=&s_clock;
    s_clock=time( tp);
    /*
    fprintf( t1lib_log_file, "(%c) (%.24s) %s: ",
	     levelid[level-1], ctime(&s_clock), func_ident);
	     */
    /* Don't print the time stamp */
    fprintf( t1lib_log_file, "(%c) %s: ", levelid[level-1], func_ident );
    vfprintf( t1lib_log_file, msg_txt, vararg );
    fprintf( t1lib_log_file, "\n");
    fflush( t1lib_log_file);

    /* cleanup variable list */
    va_end( vararg);
    
    return;
  }
}


/* T1_SetLogLevel(): Set the level which a message must have so
   that it is printed into the logfile. This function may be called
   before T1_InitLib(). */
void T1_SetLogLevel( int level)
{
  if ((level>0) && (level<5))
    t1lib_log_level=level;
  return;
}



/* T1_CheckForInit(): If no initialization of font mechanism has been
   done, return -1, indicating an error. */
int T1_CheckForInit(void)
{
  if(T1_Up)
    return(0);
  else
    return(-1);
  
}



/* T1_CheckForFontID(): Checks the font mechanism concerning the specified
   ID. It returns:
                   0  if font belonging to FontID has not yet been loaded
		   1  if font belonging to FontID has already been loaded
		   -1 if FontID is an invalid specification or t1lib not
		      initialized
		   */
int T1_CheckForFontID( int FontID)
{

  /* FontID is invalid */
  if ((FontID<0)||(FontID>(pFontBase->no_fonts - 1))||(T1_Up==0))
    return(-1);
  
  if (pFontBase->pFontArray[FontID].pType1Data==NULL)
    return(0);     /* has not yet been loaded */
  else
    return(1);     /* has already been loaded */
}

 


/* test_for_t1_file returns 0 if a file "name.pfa" or "name.pfb"
   was found. Else, -1 is returned. If successful, buffer contains the
   found filename string */
static int test_for_t1_file( char *buffer )
{
  int i=0;
  char *FullName;
  
  /* First case: A PostScript Font ASCII File without extension
     (according to some UNIX-conventions) */
  if ((FullName=intT1_Env_GetCompletePath(buffer,T1_PFAB_ptr))!=NULL) {
    free(FullName);
    return(0);
  }

  while (buffer[i]!=0){
    i++;
  }
  buffer[i]='.';
  buffer[i+1]='p';
  buffer[i+2]='f';
  buffer[i+4]=0;

  
  /* Second case: A PostScript Font ASCII File */
  buffer[i+3]='a';
  if ((FullName=intT1_Env_GetCompletePath(buffer,T1_PFAB_ptr))!=NULL) {
    free(FullName);
    return(0);
  }
  /* Third case: A PostScript Font Binary File */
  buffer[i+3]='b';
  if ((FullName=intT1_Env_GetCompletePath(buffer,T1_PFAB_ptr))!=NULL) {
    free(FullName);
    return(0);
  }
  
  /* If we get here no file was found => Set buffer
     to an empty string and return -1 */
  
  buffer[0]=0;
  return(-1);
}


/* T1_GetFontFileName() returns a pointer to the filename of the font,
   associated with FontID. This filename does not contain a full path.
   */
char *T1_GetFontFileName( int FontID)
{

  static char filename[MAXPATHLEN+1];
  
  if (T1_CheckForInit())return(NULL);

  /* Check first for valid FontID */
  if ((FontID<0) || (FontID>FontBase.no_fonts)){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  strcpy( filename, pFontBase->pFontArray[FontID].pFontFileName);
  
  return( filename);
  
}



/* As suggested by Nicolai Langfeldt, we make it possible to specify
   a completely independent path for the afm filename. This should
   make t1lib usable in context with using the kpathsearch-library.
   We allow setting those path´s after initialization, but before a
   font is loaded.
   returns  0:   OK
           -1:   Operation could not be performed
*/
int T1_SetAfmFileName( int FontID, char *afm_name)
{

  if (T1_CheckForFontID(FontID)!=0){
    /* Operation may not be applied because FontID is invalid
       or font is loaded */
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }
  if (afm_name==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }
  if (pFontBase->pFontArray[FontID].pAfmFileName!=NULL){
    /* we first free the current name */
    free( pFontBase->pFontArray[FontID].pAfmFileName);
    pFontBase->pFontArray[FontID].pAfmFileName=NULL;
  }
  
  if ((pFontBase->pFontArray[FontID].pAfmFileName=
       (char *)malloc( (strlen(afm_name)+1)*sizeof( char)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return( -1);
  }
  strcpy( pFontBase->pFontArray[FontID].pAfmFileName, afm_name);
	  
  return(0);
  
}

		  
 
/* We have a function for querying the name. Returns a pointer
   to the string or NULL if name was not explicitly set .*/
char *T1_GetAfmFileName( int FontID)
{

  static char filename[MAXPATHLEN+1];
  
  if (T1_CheckForInit())return(NULL);

  /* Check first for valid FontID */
  if ((FontID<0) || (FontID>FontBase.no_fonts)){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }

  if (pFontBase->pFontArray[FontID].pAfmFileName==NULL) {
    return( NULL);
  }
  
  strcpy( filename, pFontBase->pFontArray[FontID].pAfmFileName);
  return( filename);
  
}


  
/* T1_GetNoFonts(): Return the number of declared fonts */
int  T1_GetNoFonts(void)
{
  if (T1_CheckForInit())
    return(-1);
  return(FontBase.no_fonts);
}

	

/* T1_SetDeviceResolutions( x_res, y_res): Set the device's physical
   resolution in horizontal and vertical direction, mesured in DPI
   (Dots Per Inch). This should be done before the first font is
   loaded! */
int T1_SetDeviceResolutions( float x_res, float y_res)
{

  int i;
  
  if (T1_CheckForInit())
    ;   /* Not initialized -> no size dependent data -> OK */
  else
    /* Check if size-dependent data is existent */
    for ( i=T1_GetNoFonts(); i; i--)
      if (pFontBase->pFontArray[i-1].pFontSizeDeps!=NULL){
	T1_errno=T1ERR_OP_NOT_PERMITTED;
	return(-1); /* There's is size dependent data for a font */
      }
  
  /* Save resolutions and calculate horizontal and vertical
     scale factors to map desired bp to device pixel */
  DeviceSpecifics.x_resolution=(float) x_res;
  DeviceSpecifics.y_resolution=(float) y_res;
  DeviceSpecifics.scale_x=(float)(((float)x_res)/BIGPOINTSPERINCH);
  DeviceSpecifics.scale_y=(float)(((float)y_res)/BIGPOINTSPERINCH);
  return(0);
}


/* T1_QueryX11Support(): Check at runtime to see if t1lib was compiled
   with X11 interface: */
int T1_QueryX11Support( void)
{
#ifndef T1LIB_NO_X11_SUPPORT
  return(1);
#else
  return(0);
#endif
}




/* int T1_CopyFont(): Copies the font associated with FontID to another
   location. The pointers to type1- , afm- and encoding data as well as
   the matrices remain completely untouched. However, size dependent data
   is not copied. The produced font is marked as a "logical" font.
   If no memory is available in the FONTPRIVATE-array, there's realloc'ed
   some more memory. The FontID which is assigned to the newly generated
   font is given as the return value, or < 0 if an error occurs. Also,
   the refcount entry of the source font is incremented by one.

   Return value -1: invalid FontID specified
                -2: source font is not a "physical" font
		-3: no memory for reallocation of FONTPRIVATEs
		-4: no memory for one of the mapping tables
		*/
int T1_CopyFont( int FontID)
{
  FONTPRIVATE *save_ptr;
  int k;
  int new_ID;
  
  
  /* Check for a valid source font */
  if (T1_CheckForFontID(FontID)!=1){
    T1_errno=T1ERR_INVALID_FONTID;
    return(-1);
  }


  /* Check if the font in question is a "physical" font, otherwise it may
     not be copied */
  if (pFontBase->pFontArray[FontID].physical==0){
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-2);
  }
  
  
  /* Check if free space for a new FONTPRIVATE is available; if not,
     realloc memory some amount larger */
  save_ptr=pFontBase->pFontArray;
  if (pFontBase->no_fonts==pFontBase->no_fonts_limit){
    pFontBase->pFontArray=(FONTPRIVATE *)realloc(pFontBase->pFontArray,
						 (pFontBase->no_fonts_limit
						  + ADVANCE_FONTPRIVATE)
						 * sizeof(FONTPRIVATE));
    if (pFontBase->pFontArray==NULL){
      /* Restore pointer */
      pFontBase->pFontArray=save_ptr;
      T1_errno=T1ERR_ALLOC_MEM;
      return(-3);
    }
    /* We zero the newly allocated memory */
    if (pFontBase->pFontArray != NULL) {
      memset( pFontBase->pFontArray + pFontBase->no_fonts_limit, 0,
	      ADVANCE_FONTPRIVATE * sizeof(FONTPRIVATE));
    }
    pFontBase->no_fonts_limit += ADVANCE_FONTPRIVATE;
  }
  /* no_fonts-1 was the largest allowed font ID */
  new_ID=pFontBase->no_fonts;
  /* Copy FONTPRIVATE-structure: */
  pFontBase->pFontArray[new_ID]=pFontBase->pFontArray[FontID];
  /* (Re)Set some values explicitly, others remain untouched: */  
  pFontBase->pFontArray[new_ID].pFontSizeDeps=NULL;
  pFontBase->pFontArray[new_ID].physical=0;
  /* AFM-mapping tables are to be setup for logical fonts separately
   (if AFM data is there) */
  /* first, kerning map */
  if (pFontBase->pFontArray[new_ID].pAFMData) {
    k=pFontBase->pFontArray[new_ID].pAFMData->numOfPairs;
    if (k>0){ /* kern map exists only if kerning pairs exist! */
      if ((pFontBase->pFontArray[new_ID].pKernMap=
	   (METRICS_ENTRY *)malloc( k*sizeof( METRICS_ENTRY)))==NULL){
	sprintf( err_warn_msg_buf, "Error allocating memory for kerning map (new_ID=%d)",
		 new_ID);
	T1_PrintLog( "T1_CopyFont()", err_warn_msg_buf,
		     T1LOG_WARNING);
	T1_errno=T1ERR_ALLOC_MEM;
	return(-4);
      }
      memcpy( pFontBase->pFontArray[new_ID].pKernMap,
	      pFontBase->pFontArray[FontID].pKernMap,
	      k*sizeof( METRICS_ENTRY));
    } 
    else { /* no kerning pairs, bu AFM data present */
      pFontBase->pFontArray[new_ID].pKernMap=NULL;
    }
  } 
  else { /* AFM data not present at all */
    pFontBase->pFontArray[new_ID].pKernMap=NULL;
  }

  /* second, encoding map */
  if (pFontBase->pFontArray[FontID].pEncMap!=NULL) {
    if ((pFontBase->pFontArray[new_ID].pEncMap=
	 (int *)calloc(256,sizeof(int)))==NULL){
      sprintf( err_warn_msg_buf,
	       "Error allocating memory for encoding map (new_ID=%d)",
	       new_ID);
      T1_PrintLog( "T1_CopyFont()", err_warn_msg_buf,
		   T1LOG_WARNING);
      T1_errno=T1ERR_ALLOC_MEM;
      return(-4);
    }
    memcpy( pFontBase->pFontArray[new_ID].pEncMap,
	    pFontBase->pFontArray[FontID].pEncMap,
	    256*sizeof( int));
  }
  
  /* New font is logical --> indicate to which physical font it
     refers by setting refcount: */
  pFontBase->pFontArray[new_ID].refcount=FontID;
  
  /* Now the struct is setup; increment no_fonts by 1 because
     new_ID is a valid font specification from now on. */
  pFontBase->no_fonts++;
  /* Increment refcount in source font */
  pFontBase->pFontArray[FontID].refcount++;

  /* Generate logfile entry */
  sprintf( err_warn_msg_buf, "Assigned FontID %d to fontfile %s",
	   new_ID, FontBase.pFontArray[new_ID].pFontFileName);
  T1_PrintLog( "T1_CopyFont()", err_warn_msg_buf,
	       T1LOG_STATISTIC);
  return(new_ID);
}



/* T1_SetBitmapPad(): Set the value to which bitmap-scanlines are padded.
   This has to be done before initialization because it is a very
   rudimentary operation.
   */
int T1_SetBitmapPad( int pad)
{
  if (T1_Up){
    /* Library is initialized --> return error */
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }
  else{
    switch (pad){
    case 8: 
      T1_pad=8;
      return(0);
    case 16:
      T1_pad=16;
      return(0);
#ifdef T1_AA_TYPE64
    case 32:
      T1_pad=32;
      return(0);
#endif
    default:
      T1_errno=T1ERR_INVALID_PARAMETER;
      return(-1);
    }
  }
}



/* T1_GetBitmapPad(): Read the value to which scanlines of bitmap are
   padded. This can be done before or after initialization. */
int T1_GetBitmapPad( void)
{
  if (pFontBase) /* T1lib initialized --> return value from struct */
    return( pFontBase->bitmap_pad);
  else{
    if (T1_pad) 
      return(T1_pad);  /* pad is explicitly set --> return that value */
    else
      return( T1GLYPH_PAD); /* not expl. set --> return compilation default */
  }
}



/* CheckEndian(): Checks whether the current machine is of little or big
   endian architecture. This is important for concatenating bitmaps.
   Function returns 0 if LittleEndian and 1 if BigEndian representation
   is used  on the current hardware.
   */
int T1_CheckEndian()
{
  unsigned char *charptr;

  /* Generate test value */
  unsigned short test=0x0001;

  /* Read out memory as unsigned char */
  charptr=(unsigned char *)(&test)+1;

  /* Return value will be 1 if Big- and 0 if Little Endian */
  return((int) *charptr);
  
}



/* T1_GetLibIdent(): Return the identifier string for the current version
   of t1lib */
char *T1_GetLibIdent( void)
{
  static char buf[15];

  sprintf( buf, "%s", T1LIB_IDENT);
  
  return( (char *)buf);
}



/* T1_SetRasterFlags(): Enable/Disable certain features in
   the rasterizer */
extern void T1_SetRasterFlags( int flags)
{ 
  
  T1_Type1OperatorFlags=flags;
  return;
  
}



/* T1_GetFontFileName(): returns a pointer to the complete path filename
   of the font, associated with FontID as it is in use by t1lib.
   */
char *T1_GetFontFilePath( int FontID)
{

  static char filepath[MAXPATHLEN+1];
  char *FileNamePath=NULL;
  
  /* is initialzed? */
  if (T1_CheckForInit()) {
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  /* Check first for valid FontID */
  if ((FontID<0) || (FontID>FontBase.no_fonts)){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }

  /* lib is initialized and FontID is valid ->
     we can really expect a name */
  if ((FileNamePath=intT1_Env_GetCompletePath( pFontBase->pFontArray[FontID].pFontFileName, 
					       T1_PFAB_ptr))==NULL) { 
    T1_PrintLog( "T1_GetFontFilePath()", "Couldn't locate font file for font %d in %s", 
		 T1LOG_WARNING, FontID, T1_GetFileSearchPath(T1_PFAB_PATH)); 
    T1_errno=T1ERR_FILE_OPEN_ERR; 
    return(NULL); 
  } 
  
  strcpy( filepath, FileNamePath);
  free( FileNamePath);
  
  return( filepath);
  
}



/* We have a function for querying the name. Returns a pointer
   to the string or NULL if name was not explicitly set .*/
char *T1_GetAfmFilePath( int FontID)
{
  
  static char filepath[MAXPATHLEN+1];
  char *FontFileName;
  char *AFMFilePath;
  int i, j;
  
  /* is initialized? */
  if ((T1_CheckForInit())) {
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }
  
  /* Check first for valid FontID */
  if ((FontID<0) || (FontID>FontBase.no_fonts)){
    T1_errno=T1ERR_INVALID_FONTID;
    return(NULL);
  }

  /* Check wether AFM-file loading was suppressed on user's request */
  if ((pFontBase->t1lib_flags & T1_NO_AFM)!=0) {
    /* this is no error condition, we simply return (NULL) */
    return( NULL);
  }
  
  /* Check for explicitly associated metrics filename (via
     "T1_SetAfmFileName()"). If it exists, we return it! */
  if (pFontBase->pFontArray[FontID].pAfmFileName!=NULL) {
    strcpy( filepath, pFontBase->pFontArray[FontID].pAfmFileName);
    sprintf( err_warn_msg_buf, "Returning explicitly specified path %s for Font %d",
	     filepath, FontID);
    T1_PrintLog( "T1_GetAfmFilePath()", err_warn_msg_buf, T1LOG_DEBUG);
    return( filepath);
  }
  
  /* we have the usual case that the name of the metrics file has to be
     deduced from the font file name */
  FontFileName=T1_GetFontFileName( FontID);
  i=strlen(FontFileName);
  j=i;
  strcpy( filepath, FontFileName);
  while ( filepath[i] != '.'){
    if (i==0) break;
    else i--;
  }
  if (i==0){
    /* We have a filename without extension -> append extension */
    filepath[j]='.';
    filepath[j+1]='a'; 
    filepath[j+2]='f'; 
    filepath[j+3]='m'; 
    filepath[j+4]='\0'; 
  }
  else{
    /* we found a '.' -> replace extension */
    filepath[i+1]='a';
    filepath[i+2]='f';
    filepath[i+3]='m';
    filepath[i+4]='\0';
  }
  /* Get full path of the afm file (The case of a full path name
     name specification is valid) */
  if ((AFMFilePath=intT1_Env_GetCompletePath( filepath, T1_AFM_ptr)) == NULL) {
    return NULL;
  }
  
  strcpy( filepath, AFMFilePath);
  free( AFMFilePath);
  
  return( filepath);
  
}


