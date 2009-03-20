/*--------------------------------------------------------------------------
  ----- File:        t1env.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2007-12-22
  ----- Description: This file is part of the t1-library. It implements
                     the reading of a configuration file and path-searching
		     of type1-, afm- and encoding files.
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
		     independ from X11.
                     Thanks to all people who make free software living!
--------------------------------------------------------------------------*/
  

#define T1ENV_C


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


#include "../type1/types.h"
#include "parseAFM.h" 
#include "../type1/objects.h" 
#include "../type1/spaces.h"  
#include "../type1/util.h" 
#include "../type1/fontfcn.h"
#include "../type1/fontmisc.h"

#include "sysconf.h"
#include "t1types.h"
#include "t1extern.h"
#include "t1env.h"
#include "t1misc.h"
#include "t1base.h"


/* The following static variables are used to store information on the distinct
   file search paths:

   -1         t1lib has not yet been initialized!
    0         t1lib has been initialized and default paths have been setup
    n (>0)    there are n path elements for current search path type, either built
              from a FontDataBase file or from explicit fucntion calls.
*/
	    
static int pfab_no=-1;
static int afm_no=-1;
static int enc_no=-1;
static int fdb_no=-1;
static int fdbxlfd_no=-1;

static char path_sep_char='\0';
static char path_sep_string[2];

static char pathbuf[2048];

/* Define some default search paths */
#ifndef VMS
static char T1_pfab[]=".";
static char T1_afm[]=".";
static char T1_enc[]=".";
#else
static char T1_pfab[]="sys$disk:[]";
static char T1_afm[]="sys$disk:[]";
static char T1_enc[]="sys$disk:[]";
#endif
char T1_fdb[]="FontDataBase";
char T1_fdbxlfd[]="";  /* By default, we do not search XLFD databases. */


/* keywords recognized in config file */
static const char enc_key[]="ENCODING";
static const char pfab_key[]="TYPE1";
static const char afm_key[]="AFM";
static const char fdb_key[]="FONTDATABASE";
static const char fdbxlfd_key[]="FONTDATABASEXLFD";  


/* qstrncpy(): Copy bytes from srcP to to destP. srcP is count bytes long
   and destP is the number of quoted characters shorter. That is, count
   refers to the number of characters including the escapement chars in
   srcP! */
static void qstrncpy( char *destP, const char *srcP, long nochars)
{
  long i;
  long j;
  
  i=0;  /* dest-index */
  j=0;  /* src-index */
  
  while (j<nochars) {
    if (srcP[j]=='\\') {
      if (srcP[j+1]=='"') { 
	j++;                /* escaped quotation character --> omit escape char. */
      }
    }
    else {                  /* normal character */
      destP[i++]=srcP[j++];
    }
  }
}




/* Setup the default paths for searching the distinct file types. If
   paths have been setup explicitly, skip the step of setting up a default path. */
void intT1_SetupDefaultSearchPaths( void) 
{

  path_sep_char=PATH_SEP_CHAR;
  sprintf( path_sep_string, "%c", path_sep_char);
  
  /* We set the number of stored path elements 0 so that we can distiguish
     between explicitly setup paths and default paths in intT1_ScanConfigFile(). */
  if (pfab_no==-1) {
    T1_PFAB_ptr=(char**) calloc( 2, sizeof(char*));
    T1_PFAB_ptr[0]=(char*)malloc(strlen(T1_pfab)+1);
    strcpy(T1_PFAB_ptr[0],T1_pfab);
    pfab_no=0;
  }
  
  if (afm_no==-1) {
    T1_AFM_ptr=(char**) calloc( 2, sizeof(char*));
    T1_AFM_ptr[0]=(char*)malloc(strlen(T1_afm)+1);
    strcpy(T1_AFM_ptr[0],T1_afm);
    afm_no=0;
  }
  
  if (enc_no==-1) {
    T1_ENC_ptr=(char**) calloc( 2, sizeof(char*));
    T1_ENC_ptr[0]=(char*)malloc(strlen(T1_enc)+1);
    strcpy(T1_ENC_ptr[0],T1_enc);
    enc_no=0;
  }

  if (fdb_no==-1) {
    T1_FDB_ptr=(char**) calloc( 2, sizeof(char*));
    T1_FDB_ptr[0]=(char*)malloc(strlen(T1_fdb)+1);
    strcpy(T1_FDB_ptr[0],T1_fdb);
    fdb_no=0;
  }

  if (fdbxlfd_no==-1) {
    /* The XLFD font data base defaults to be empty */
    T1_FDBXLFD_ptr=(char**) calloc( 1, sizeof(char*));
    fdbxlfd_no=0;
  }
}


/* This function is called from T1_CloseLib(). We have to indicate the state
   of a non-initialzed t1lib! */
void intT1_FreeSearchPaths( void) 
{
  int i;
  
  i=0;
  if (T1_PFAB_ptr!=NULL) {
    while (T1_PFAB_ptr[i]!=NULL) {
      free(T1_PFAB_ptr[i]);
      T1_PFAB_ptr[i++]=NULL;
    }
    free( T1_PFAB_ptr);
    T1_PFAB_ptr=NULL;
  }
  i=0;
  if (T1_AFM_ptr!=NULL) {
    while (T1_AFM_ptr[i]!=NULL) {
      free(T1_AFM_ptr[i]);
      T1_AFM_ptr[i++]=NULL;
    }
    free( T1_AFM_ptr);
    T1_AFM_ptr=NULL;
  }
  i=0;
  if (T1_ENC_ptr!=NULL) {
    while (T1_ENC_ptr[i]!=NULL) {
      free(T1_ENC_ptr[i]);
      T1_ENC_ptr[i++]=NULL;
    }
    free( T1_ENC_ptr);
    T1_ENC_ptr=NULL;
  }
  i=0;
  if (T1_FDB_ptr!=NULL) {
    while (T1_FDB_ptr[i]!=NULL) {
      free(T1_FDB_ptr[i]);
      T1_FDB_ptr[i++]=NULL;
    }
    free( T1_FDB_ptr);
    T1_FDB_ptr=NULL;
  }
  i=0;
  if (T1_FDBXLFD_ptr!=NULL) {
    while (T1_FDBXLFD_ptr[i]!=NULL) {
      free(T1_FDBXLFD_ptr[i]);
      T1_FDBXLFD_ptr[i++]=NULL;
    }
    free( T1_FDBXLFD_ptr);
    T1_FDBXLFD_ptr=NULL;
  }
  /* indicate t1lib non-initialized */
  pfab_no=-1;
  afm_no=-1;
  enc_no=-1;
  fdb_no=-1;
  fdbxlfd_no=-1;
  
  return;
}


/* ScanConfigFile(): Read a configuration file and scan and save the
   environment strings used for searching pfa/pfb-, afm- and encoding
   files as well as the name of the font database file. */
int intT1_ScanConfigFile( void)
{
  
  char *env_str;
  char *linebuf;
  char *usershome;
  char *cnffilepath;
  char *globalcnffilepath;
  static int linecnt;
  char local_path_sep_char;
  int quoted=0;
  int quotecnt=0;
  FILE *cfg_fp;
  int filesize, i, j, k;
  int ignoreline=0;

  char*** destP=NULL;
  int *idestP=NULL;
  char* curr_key=NULL;
  
  /* First, get the string stored in the environment variable: */
  env_str=getenv(ENV_CONF_STRING);
  linecnt=1;

  if (!env_str) {
    /* environment variable not set, try to open default file
       in user's home directory and afterwards global config file */
    if ((usershome=getenv("HOME"))!=NULL) {
      cnffilepath=(char *)malloc((strlen(usershome) +
				  strlen(T1_CONFIGFILENAME) + 2
				  ) * sizeof(char));
      if (cnffilepath==NULL){
	T1_errno=T1ERR_ALLOC_MEM;
	return(-1);
      }
      strcpy( cnffilepath, usershome);
    }
    else {
      cnffilepath=(char *)malloc((strlen(T1_CONFIGFILENAME) + 2
				  ) * sizeof(char));
    }
    strcat( cnffilepath, DIRECTORY_SEP);
    strcat( cnffilepath, T1_CONFIGFILENAME);

    globalcnffilepath=(char*)malloc((strlen(GLOBAL_CONFIG_DIR) +
				     strlen(GLOBAL_CONFIG_FILE) + 2
				     ) * sizeof(char));
    if (globalcnffilepath==NULL){
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    strcpy( globalcnffilepath, GLOBAL_CONFIG_DIR);
    strcat( globalcnffilepath, DIRECTORY_SEP);
    strcat( globalcnffilepath, GLOBAL_CONFIG_FILE);
    
    if ((cfg_fp=fopen( cnffilepath, "rb"))==NULL){
      sprintf( err_warn_msg_buf, "Could not open configfile %s",
	       cnffilepath);
      T1_PrintLog( "ScanConfigFile()", err_warn_msg_buf, T1LOG_STATISTIC);
      /* Try global config file */
      if ((cfg_fp=fopen( globalcnffilepath, "rb"))==NULL){
	sprintf( err_warn_msg_buf, "Could not open global configfile %s",
		 globalcnffilepath);
	T1_PrintLog( "ScanConfigFile()", err_warn_msg_buf, T1LOG_WARNING);
      }
      else{
	sprintf( err_warn_msg_buf, "Using %s as Configfile (global)",
		 globalcnffilepath);
	T1_PrintLog( "ScanConfigFile()", err_warn_msg_buf, T1LOG_STATISTIC);
      }
    }
    else{
      sprintf( err_warn_msg_buf, "Using %s as Configfile (user's)",
	       cnffilepath);
      T1_PrintLog( "ScanConfigFile()", err_warn_msg_buf, T1LOG_STATISTIC);
    }
    free( cnffilepath);
    free( globalcnffilepath);
    if (cfg_fp==NULL){
      T1_PrintLog( "ScanConfigFile()",
		   "Neither user's nor global Configfile has been found",
		   T1LOG_WARNING);
      return(0);
    }
  }
  else {
    /* open specified file for reading the configuration */
    if ((cfg_fp=fopen(env_str,"rb"))==NULL){
      T1_PrintLog( "ScanConfigFile()",
		   "Configfile as specified by Environment has not been found",
		   T1LOG_WARNING);
      return(0);  /* specified file could not be openend
		     => no config paths read */
    }
    else {
      sprintf( err_warn_msg_buf, "Using %s as Configfile (environment)",
	       env_str);
      T1_PrintLog( "ScanConfigFile()", err_warn_msg_buf, T1LOG_STATISTIC);
    }
  }
  

  /* cfg_fp points now to a valid config file */
  /* Get the file size */
  fseek( cfg_fp, 0, SEEK_END);
  filesize=ftell(cfg_fp);
  /* Reset fileposition to start */
  fseek( cfg_fp, 0, SEEK_SET);
  
  if ((linebuf=(char *)calloc( filesize+1,
			       sizeof(char)))==NULL){
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }
  
  fread((char *)linebuf, sizeof(char), filesize, cfg_fp);
  fclose(cfg_fp);
  
  i=0;

  /* this might be overwritten on a per file basis */
  local_path_sep_char=path_sep_char;

  while(i<filesize) {
    ignoreline=0;
    j=i;     /* Save index of beginning of line */
    while ((linebuf[i]!='=') && (linebuf[i]!='\n') && (i<filesize)) {
      i++;
    } 
    if (i==filesize) {
      free( linebuf);
      return(i);
    }
    
    if (strncmp( enc_key, &linebuf[j], 8)==0) {
      /* setup target */
      destP=&T1_ENC_ptr;
      idestP=&enc_no;
      curr_key=(char*)enc_key;
    }
    else if (strncmp( pfab_key, &linebuf[j], 5)==0) {
      /* setup target */
      destP=&T1_PFAB_ptr;
      idestP=&pfab_no;
      curr_key=(char*)pfab_key;
    }
    else if (strncmp( afm_key, &linebuf[j], 3)==0) {
      /* setup target */
      destP=&T1_AFM_ptr;
      idestP=&afm_no;
      curr_key=(char*)afm_key;
    }
    else if (strncmp( fdbxlfd_key, &linebuf[j], 16)==0) {
      /* The handling here is somewhat specific. XLFD font database
	 specifications may coexist with standard font database
	 specification. However, if the standard font database is
	 the default value, an existing XLFD specification clears
	 this default value. Let this precede the standard fdb because
	 otherwise, this code would never be reached. */
      if (fdb_no==0) { /* default paths are currently setup, get rid of them */
	free(T1_FDB_ptr[0]);
	T1_FDB_ptr[0]=NULL;
      }
      
      /* setup target */
      destP=&T1_FDBXLFD_ptr;
      idestP=&fdbxlfd_no;
      curr_key=(char*)fdbxlfd_key;
    }
    else if (strncmp( fdb_key, &linebuf[j], 12)==0) {
      /* setup target */
      destP=&T1_FDB_ptr;
      idestP=&fdb_no;
      curr_key=(char*)fdb_key;
    }
    else {
      ignoreline=1;
      T1_PrintLog( "ScanConfigFile()", "Ignoring line %d",
		   T1LOG_DEBUG, linecnt);
    }

    /* If appropriate, scan this line. */
    if (ignoreline==0) { 
      /* Check for an explicitly assigned value */
      if (*idestP==0) { /* default paths are currently setup, get rid of them */
	if ((*destP)[0]!=NULL) {
	  free((*destP)[0]);
	  (*destP)[0]=NULL;
	}
      }
      else { /* append to existing paths */
	T1_PrintLog( "ScanConfigFile()",
		     "Appending to existing %s search path",
		     T1LOG_DEBUG, curr_key);
      }
      while ( (!isspace((int)linebuf[i])) && (i<filesize) ) {
	k=++i;      /* index to current path element */
	(*idestP)++;
	quotecnt=0;
	if (linebuf[i]=='"') { /* We have a quoted string */
	  quoted=1;
	  k=++i;
	  while ( 1) {
	    if ( linebuf[i]=='"' ) {    /* we find a quote-char */ 
	      if ( linebuf[i-1]!='\\' ) 
		break;                     /* not escaped --> end of path specification */
	      else
		quotecnt++;
	    }                           /* some other char */
	    if (linebuf[i]=='\n') { /* a newline in a quoted string? Perhabs, quotes do not match! */
	      T1_PrintLog( "ScanConfigFile()",
			   "Newline in quoted %s-string in line %d, column %d, of config file! Closing quote missing?", 
			   T1LOG_WARNING, curr_key, linecnt, i-j+1);
	      j=i+1;                /* resynchronize linecount */
	      linecnt++;           
	    }
	    if (i<filesize) {            /* filesize not exceeded? */
	      i++;
	    }
	    else {                       /* issue error msg because end of quotation is missing */
	      T1_PrintLog( "ScanConfigFile()", "Unterminated quoted string in config file",
			   T1LOG_ERROR);
	      return -1;
	    }
	  }
	}
	else {
	  quoted=0;
	  while ( (linebuf[i]!=local_path_sep_char) && (!isspace((int)linebuf[i])) && (i<filesize) )
	    i++;
	}
	if (((*destP)=(char**)realloc( (*destP), ((*idestP)+1)*sizeof(char*)))==NULL) {
	  T1_errno=T1ERR_ALLOC_MEM;
	  return(-1);
	}
	if (((*destP)[(*idestP)-1]=(char*)malloc((i-k-quotecnt+1)*sizeof(char)))==NULL) {
	  T1_errno=T1ERR_ALLOC_MEM;
	  return(-1);
	}
	if (quoted==0) {
	  strncpy( (*destP)[*idestP-1], &(linebuf[k]), i-k);
	  (*destP)[(*idestP)-1][i-k]='\0';
	}
	else {
	  qstrncpy( (*destP)[(*idestP)-1], &(linebuf[k]), i-k);
	  (*destP)[(*idestP)-1][i-k-quotecnt]='\0';
	  i++;         /* step over closing quote */
	}
	(*destP)[(*idestP)]=NULL;     /* indicate end of string list */
      }
    }

    /* skip remaining of line or file */
    while ((linebuf[i]!='\n')&&(i<filesize))
      i++;
    i++;
    linecnt++;
  }
  /* file should now be read in */
  free( linebuf);
  
  return(i);
  
}



/* intT1_Env_GetCompletePath( ): Get a full path name from the file specified by
   argument 1 in the environment specified by argument 2. Return the pointer
   to the path string or NULL if no file was found.*/
char *intT1_Env_GetCompletePath( char *FileName,
				 char **env_ptr )
{
  struct stat filestats;    /* A structure where fileinfo is stored */
  int fnamelen, i, j;
  char *FullPathName;
  char *StrippedName;
  

  if (FileName==NULL)
    return(NULL);
  fnamelen=strlen(FileName);

  /* We check whether absolute or relative pathname is given. If so,
     stat() it and if appropriate, return that string immediately. */
  if ( (FileName[0]==DIRECTORY_SEP_CHAR)
       ||
       ((fnamelen>1) && (FileName[0]=='.') &&
	(FileName[1]==DIRECTORY_SEP_CHAR))
       ||
       ((fnamelen>2) && (FileName[0]=='.') &&
	(FileName[1]=='.') && (FileName[2]==DIRECTORY_SEP_CHAR))
#if defined(MSDOS) | defined(_WIN32) | defined (__EMX__)
       ||
       ((isalpha(FileName[0])) && (FileName[1]==':'))
#endif
#ifdef VMS
       || (strchr(FileName,':') != NULL)
#endif
       )
    {
    /* Check for existence of the path: */
    if (!stat( FileName, &filestats)) {
      if (t1lib_log_file!=NULL) {
	sprintf( err_warn_msg_buf, "stat()'ing complete path %s successful",
		 FileName);
	T1_PrintLog( "intT1_Env_GetCompletePath()", err_warn_msg_buf,
		     T1LOG_DEBUG);
      }
      /* Return a copy of the string */
      if ((FullPathName=(char *)malloc( fnamelen + 1))==NULL) {
	T1_errno=T1ERR_ALLOC_MEM;
	return(NULL);
      }
      strcpy( FullPathName, FileName);
      return(FullPathName);
    }
    if (t1lib_log_file!=NULL){
      sprintf( err_warn_msg_buf, "stat()'ing complete path %s failed",
	       FileName);
      T1_PrintLog( "intT1_Env_GetCompletePath()", err_warn_msg_buf,
		   T1LOG_DEBUG);
    }
    /* Trying to locate absolute path spec. failed. We try to recover
       by removing the path component and searching in the remaining search
       path entries. This depends on the OS. */
    i=fnamelen-1;
    StrippedName=&(FileName[i]);
    while ( FileName[i]!=DIRECTORY_SEP_CHAR
#if defined(VMS)
	    /* What exactly to do for VMS? */
#elif defined(MSDOS) | defined(_WIN32) | defined (__EMX__) | defined(_MSC_VER)
	    /* We take a drive specification into account. This means we
	       step back until the directory separator or a drive specifier
	       appears! */
	    && FileName[i]!=':'
#endif
	    ) {
      i--;
    }
    i++;
    StrippedName=&FileName[i];
    if (t1lib_log_file!=NULL){
      sprintf( err_warn_msg_buf, "path %s stripped to %s",
	       FileName, StrippedName);
      T1_PrintLog( "intT1_Env_GetCompletePath()", err_warn_msg_buf,
		   T1LOG_DEBUG);
    }
  }
  else{ /* We have a relative path name */
    StrippedName=&FileName[0];
  }

  i=0;
  while (env_ptr[i]!=NULL) {
    /* Copy current path element: */
    strcpy( pathbuf, env_ptr[i]);
    /* cut a trailing directory separator */
    j=strlen(pathbuf);
    if (pathbuf[j-1]==DIRECTORY_SEP_CHAR)
      pathbuf[--j]='\0';
    /* Add the directory separator: */
#ifdef VMS
    { char *p= strrchr(pathbuf, DIRECTORY_SEP_CHAR);
      if (p && *(p+1) ==  '\0')
       *p = '\0';
    } 
#endif 
    strcat( pathbuf, DIRECTORY_SEP);
    /* And finally the filename.
       The following is fix against a vulnerability given by passing in
       large filenames, cf.:

           http://www.securityfocus.com/bid/25079

       or

           http://packetstormsecurity.nl/0707-advisories/t1lib.txt

       If current pathbuf + StrippedName + 1 byte for NULL is bigger than
       pathbuf log a warning and try next pathbuf */
    if ( strlen(pathbuf) + strlen(StrippedName) + 1 > sizeof(pathbuf) ) {
      T1_PrintLog( "intT1_Env_GetCompletePath()", "Omitting suspicious long candidate path in order to prevent buffer overflow.",
		   T1LOG_WARNING);
      i++;
      continue;
    }
    strcat( pathbuf, StrippedName);
    
    /* Check for existence of the path: */
    if (!stat( pathbuf, &filestats)) {
      if ((FullPathName=(char*)malloc( (j+fnamelen+2)*sizeof(char)))==NULL) {
	T1_errno=T1ERR_ALLOC_MEM;
	return(NULL);
      }
      strcpy( FullPathName, pathbuf);
      if (t1lib_log_file!=NULL){
	sprintf( err_warn_msg_buf, "stat()'ing %s successful",
		 FullPathName);
	T1_PrintLog( "intT1_Env_GetCompletePath()", err_warn_msg_buf,
		     T1LOG_DEBUG);
      }
      return(FullPathName);
    }
    if (t1lib_log_file!=NULL){
      sprintf( err_warn_msg_buf, "stat()'ing %s failed",
	       pathbuf);
      T1_PrintLog( "intT1_Env_GetCompletePath()", err_warn_msg_buf,
		   T1LOG_DEBUG);
    }
    /* We didn't find the file --> try next path entry */
    i++;
  }
  /* If we get here, no file was found at all, so return a NULL-pointer */
  return(NULL);
}



/* T1_SetFileSearchPath(): Set the search path to find files of the
   specified type and return 0 if successful and -1 otherwise. An existing
   path is overwritten rigorously, unless the database already contains fonts.
   In the latter case the function returns with an error status.
   Multiple path types may be specified as a bitmask!
*/
int T1_SetFileSearchPath( int type, char *pathname)
{

  int i;
  int pathlen;
  
  
  if (pathname==NULL){
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }

  /* We do not allow to change the searchpath if the database already
     contains one or more entries. */
  if (T1_GetNoFonts()>0){
    sprintf( err_warn_msg_buf, "Path %s not set, database is not empty",
	     pathname);
    T1_PrintLog( "T1_SetFileSearchPath()", err_warn_msg_buf,
		 T1LOG_STATISTIC);
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return(-1);
  }

  pathlen=strlen(pathname)+1;
  /* Throw away a possibly existing path */
  if (type & T1_PFAB_PATH){
    if (pfab_no==-1) {
      T1_PFAB_ptr=NULL; /* realloc() will do a malloc() */
    }
    else {
      /* throw away current paths */
      i=0;
      while (T1_PFAB_ptr[i]!=NULL) {
	free (T1_PFAB_ptr[i++]);
      }
    }
    if ((T1_PFAB_ptr=(char**)realloc( T1_PFAB_ptr, 2*sizeof(char*)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    if ((T1_PFAB_ptr[0]=(char*)malloc(pathlen*sizeof(char)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    strcpy( T1_PFAB_ptr[0], pathname);
    T1_PFAB_ptr[1]=NULL;
    pfab_no=1;
  }
  if (type & T1_AFM_PATH){
    if (afm_no==-1) {
      T1_AFM_ptr=NULL; /* realloc() will do a malloc() */
    }
    else {
      /* throw away current paths */
      i=0;
      while (T1_AFM_ptr[i]!=NULL) {
	free (T1_AFM_ptr[i++]);
      }
    }
    if ((T1_AFM_ptr=(char**)realloc( T1_AFM_ptr, 2*sizeof(char*)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    if ((T1_AFM_ptr[0]=(char*)malloc(pathlen*sizeof(char)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    strcpy( T1_AFM_ptr[0], pathname);
    T1_AFM_ptr[1]=NULL;
    afm_no=1;
  }
  if (type & T1_ENC_PATH){
    if (enc_no==-1) {
      T1_ENC_ptr=NULL; /* realloc() will do a malloc() */
    }
    else {
      /* throw away current paths */
      i=0;
      while (T1_ENC_ptr[i]!=NULL) {
	free (T1_ENC_ptr[i++]);
      }
    }
    if ((T1_ENC_ptr=(char**)realloc( T1_ENC_ptr, 2*sizeof(char*)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    if ((T1_ENC_ptr[0]=(char*)malloc(pathlen*sizeof(char)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    strcpy( T1_ENC_ptr[0], pathname);
    T1_ENC_ptr[1]=NULL;
    enc_no=1;
  }
  
  return(0);
  
}



/* T1_GetFileSearchPath(): Return the specified file search path
   or NULL if an error occurred. Note: We do only one path at a
   time, so that if a bitmask is specified, the first match wins.
   The returned path is formatted using the actual PATH_SEP_CHAR. */
char *T1_GetFileSearchPath( int type)
{
  static char *out_ptr;
  int i;
  int pathlen;
  char **src_ptr=NULL;
  

  if (out_ptr!=NULL)
    free( out_ptr);
  out_ptr=NULL;
  
  if (type & T1_PFAB_PATH) {
    src_ptr=T1_PFAB_ptr;
  }
  else  if (type & T1_AFM_PATH) {
    src_ptr=T1_AFM_ptr;
  }
  else if (type & T1_ENC_PATH) {
    src_ptr=T1_ENC_ptr;
  }
  else if (type & T1_FDB_PATH) {
    src_ptr=T1_FDB_ptr;
  }
  
  
  i=0;
  pathlen=0;
  while (src_ptr[i]!=NULL) {
    pathlen +=strlen( src_ptr[i++]);
    pathlen+=1; /* path separator */
  }
  if ((out_ptr=(char *)malloc(pathlen+1))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return( NULL);
  }
  strcpy( out_ptr, src_ptr[0]);
  i=1;
  while (src_ptr[i]!=NULL) {
    strcat( out_ptr, path_sep_string);
    strcat( out_ptr, src_ptr[i++]);
  }
  
  return( out_ptr);

}


/* T1_AddToFileSearchPath(): Add the specified path element to
   the specified search path. If the existing path is the default path,
   it will not be replaced by the new path element. Since this function might
   be called before initialization, we have to be aware that even the default
   path could be missing. Multiple path types may be specified as a bitmask!
   Return value is 0 if successful and -1 otherwise */
int T1_AddToFileSearchPath( int pathtype, int mode, char *pathname)
{
  int i;
  int pathlen;
  char* newpath = NULL;
  int nofonts;
  
  
  if (pathname==NULL)
    return(-1);

  nofonts=T1_GetNoFonts();
  
  pathlen=strlen(pathname);
  
  if (pathtype & T1_PFAB_PATH){
    /* Allocate meory for string */
    if ((newpath=(char*)malloc( (pathlen+1)*sizeof(char)))==NULL)  {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    /* Check for and handle the existing path configuration */
    if (pfab_no==0) {   /* do not free the default path but establish it
			   as a regularly setup path, if database not empty! */
      if (nofonts>0) {
	pfab_no++;
      }
      else {
	free( T1_AFM_ptr[0]);
      }
    }
    if (pfab_no==-1) {  /* not initialized! */
      pfab_no=0;
      T1_PFAB_ptr=NULL; /* realloc() will do the malloc()! */
    }
    if ((T1_PFAB_ptr=(char**)realloc( T1_PFAB_ptr, (++pfab_no+1)*sizeof(char*)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    /* Insert the new path element: */
    if (mode & T1_PREPEND_PATH){ /* prepend */
      i=pfab_no-2;
      while (i>=0) {
	T1_PFAB_ptr[i+1]=T1_PFAB_ptr[i];
	i--;
      }
      T1_PFAB_ptr[0]=newpath;
    }
    else{ /* append */
      T1_PFAB_ptr[pfab_no-1]=newpath;
    }
    T1_PFAB_ptr[pfab_no]=NULL;
  }
  if (pathtype & T1_AFM_PATH){
    /* Allocate meory for string */
    if ((newpath=(char*)malloc( (pathlen+1)*sizeof(char)))==NULL)  {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    /* Check for and handle the existing path configuration */
    if (afm_no==0) {   /* do not free the default path but establish it
			  as a regularly setup path, if database not empty! */
      if (nofonts>0) {
	afm_no++;
      }
      else {
	free( T1_AFM_ptr[0]);
      }
    }
    if (afm_no==-1) {  /* not initialized! */
      afm_no=0;
      T1_AFM_ptr=NULL; /* realloc() will do the malloc()! */
    }
    if ((T1_AFM_ptr=(char**)realloc( T1_AFM_ptr, (++afm_no+1)*sizeof(char*)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    /* Insert the new path element */
    if (mode & T1_PREPEND_PATH){ /* prepend */
      i=afm_no-2;
      while (i>=0) {
	T1_AFM_ptr[i+1]=T1_AFM_ptr[i];
	i--;
      }
      T1_AFM_ptr[0]=newpath;
    }
    else{ /* append */
      T1_AFM_ptr[afm_no-1]=newpath;
    }
    T1_AFM_ptr[afm_no]=NULL;
  }
  if (pathtype & T1_ENC_PATH){
    /* Allocate meory for string */
    if ((newpath=(char*)malloc( (pathlen+1)*sizeof(char)))==NULL)  {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    /* Check for and handle the existing path configuration */
    if (enc_no==0) {    /* do not free the default path but establish it
			   as a regularly setup path, if database not empty! */
      if (nofonts>0) {
	enc_no++;
      }
      else {
	free( T1_ENC_ptr[0]);
      }
    }
    if (enc_no==-1) {  /* not initialized! */
      enc_no=0;
      T1_ENC_ptr=NULL; /* realloc() will do the malloc()! */
    }
    if ((T1_ENC_ptr=(char**)realloc( T1_ENC_ptr, (++enc_no+1)*sizeof(char*)))==NULL) {
      T1_errno=T1ERR_ALLOC_MEM;
      return(-1);
    }
    /* Insert the new path element: */
    if (mode & T1_PREPEND_PATH){ /* prepend */
      i=enc_no-2;
      while (i>=0) {
	T1_ENC_ptr[i+1]=T1_ENC_ptr[i];
	i--;
      }
      T1_ENC_ptr[0]=newpath;
    }
    else{ /* append */
      T1_ENC_ptr[enc_no-1]=newpath;
    }
    T1_ENC_ptr[enc_no]=NULL;
  }
  
  /* Copy new path to where it belongs ... */
  if (newpath)
    strcpy(newpath, pathname);
  
  return(0);
  
}



/* T1_SetFontDataBase(): Set a new name for the font database. It replaces the default
   name and any names specified previously with this function.
   Return value: 0 if OK, and -1 if filename not valid or an allocation
   error occurred */
int T1_SetFontDataBase( char *filename)
{
  int pathlen;
  int i;
  int result=0;
  
  
  /* chekc filename */
  if (filename==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return -1;
  }

  /* this function must be called before any font is in the database, that is, usually,
     before initialization! */
  if ( pFontBase!=NULL && pFontBase->no_fonts>0) {
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return -1;
  }

  
  pathlen=strlen(filename)+1;
  /* Throw away a possibly existing font database-statement */
  if (fdb_no==-1) {  
    T1_FDB_ptr=NULL; /* realloc() will do a malloc() */
  }
  else { 
    /* throw away current paths */
    i=0;
    while (T1_FDB_ptr[i]!=NULL) {
      free (T1_FDB_ptr[i++]);
    }
  }

  if ((T1_FDB_ptr=(char**)realloc( T1_FDB_ptr, 2*sizeof(char*)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return -1;
  }
  
  if ((T1_FDB_ptr[0]=(char*)malloc(pathlen*sizeof(char)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return -1;
  }
  strcpy( T1_FDB_ptr[0], filename);
  T1_FDB_ptr[1]=NULL;
  fdb_no=1;

  /* Load database immediately if t1lib already is initailzed */
  if (T1_CheckForInit()==0) {
    if ((result=intT1_scanFontDBase(T1_FDB_ptr[0]))==-1) {
      T1_PrintLog( "T1_AddFontDataBase()", "Fatal error scanning Font Database File %s (T1_errno=%d)",
		   T1LOG_WARNING, T1_FDB_ptr[0], T1_errno);
    }
    if (result>-1)
      pFontBase->no_fonts+=result;
    result=pFontBase->no_fonts;
  }
  return result;
  
}


/* T1_AddFontDataBase(): Add a new font database file to the list. If the
   lib is already initialzed, then the new database is immediately loaded.
   Otherwise it is simply appended to the list and loaded at the time of
   initialization.
   Returns: -1    an error occured
             0    successfully inserted but not loaded because lib not initilized
	     n>0  the highest defined FontID
*/
int T1_AddFontDataBase( int mode, char *filename) 
{
  int i;
  int pathlen;
  int result=0;
  char* newpath;
  
  
  if (filename==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }
  
  pathlen=strlen(filename);
  
  /* Allocate memory for string */
  if ((newpath=(char*)malloc( (pathlen+1)*sizeof(char)))==NULL)  {
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }
  strcpy( newpath, filename);
  /* Check for and handle the existing path configuration */
  if (fdb_no==0) {   /* defauls setup, free the path */
    free( T1_FDB_ptr[0]);
  }
  if (fdb_no==-1) {  /* not initialized! */
    fdb_no=0;
    T1_FDB_ptr=NULL; /* realloc() will do the malloc()! */
  }
  
  if ((T1_FDB_ptr=(char**)realloc( T1_FDB_ptr, (++fdb_no+1)*sizeof(char*)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }
  /* Insert the new database. If t1lib is already initialzed, the database can only
     be appended. Otherwise. prepending is also possible.*/
  if ((mode & T1_PREPEND_PATH) && (T1_CheckForInit()!=0) ) { /* prepend */
    i=fdb_no-2;
    while (i>=0) {
      T1_FDB_ptr[i+1]=T1_FDB_ptr[i];
      i--;
    }
    T1_FDB_ptr[0]=newpath;
    result=0;
  }
  else { /* append */
    T1_FDB_ptr[fdb_no-1]=newpath;
    if (T1_CheckForInit()==0) {
      if ((result=intT1_scanFontDBase(T1_FDB_ptr[fdb_no-1]))==-1) {
	T1_PrintLog( "T1_AddFontDataBase()", "Fatal error scanning Font Database File %s (T1_errno=%d)",
		     T1LOG_WARNING, T1_FDB_ptr[fdb_no-1], T1_errno);
      }
      if (result>-1)
	pFontBase->no_fonts+=result;
      result=pFontBase->no_fonts;
    }
  }
  T1_FDB_ptr[fdb_no]=NULL;
  return result;
  
}



/* T1_SetFontDataBaseXLFD(): Set a new name for the XLFD font database. It
   replaces the default name (which is empty and any names specified
   previously with this function.
   Return value: 0 if OK, and -1 if filename not valid or an allocation
   error occurred */
int T1_SetFontDataBaseXLFD( char *filename)
{
  int pathlen;
  int i;
  int result=0;
  
  
  /* check filename */
  if (filename==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return -1;
  }

  /* this function must be called before any font is in the database, that is, usually,
     before initialization! */
  if ( pFontBase!=NULL && pFontBase->no_fonts>0) {
    T1_errno=T1ERR_OP_NOT_PERMITTED;
    return -1;
  }

  
  pathlen=strlen(filename)+1;
  /* Throw away a possibly existing font database-statement */
  if (fdbxlfd_no==-1) {  
    T1_FDBXLFD_ptr=NULL; /* realloc() will do a malloc() */
  }
  else { 
    /* throw away current paths */
    i=0;
    while (T1_FDBXLFD_ptr[i]!=NULL) {
      free (T1_FDBXLFD_ptr[i++]);
    }
  }

  if ((T1_FDBXLFD_ptr=(char**)realloc( T1_FDBXLFD_ptr, 2*sizeof(char*)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return -1;
  }
  
  if ((T1_FDBXLFD_ptr[0]=(char*)malloc(pathlen*sizeof(char)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return -1;
  }
  strcpy( T1_FDBXLFD_ptr[0], filename);
  T1_FDBXLFD_ptr[1]=NULL;
  fdb_no=1;

  /* Load XLFD database immediately if t1lib already is initailzed */
  if (T1_CheckForInit()==0) {
    if ((result=intT1_scanFontDBaseXLFD(T1_FDBXLFD_ptr[0]))==-1) {
      T1_PrintLog( "T1_AddFontDataBaseXLFD()", "Fatal error scanning XLFD Font Database File %s (T1_errno=%d)",
		   T1LOG_WARNING, T1_FDBXLFD_ptr[0], T1_errno);
    }
    if (result>-1)
      pFontBase->no_fonts+=result;
    result=pFontBase->no_fonts;
  }
  return result;
  
}


/* T1_AddFontDataBaseXLFD(): Add a new XLFD font database file to the list. If
   the lib is already initialzed, then the new database is immediately loaded.
   Otherwise it is simply appended to the list and loaded at the time of
   initialization.
   Returns: -1    an error occured
             0    successfully inserted but not loaded because lib not initilized
	     n>0  the highest defined FontID
*/
int T1_AddFontDataBaseXLFD( int mode, char *filename) 
{
  int i;
  int pathlen;
  int result=0;
  char* newpath;
  
  
  if (filename==NULL) {
    T1_errno=T1ERR_INVALID_PARAMETER;
    return(-1);
  }
  
  pathlen=strlen(filename);
  
  /* Allocate memory for string */
  if ((newpath=(char*)malloc( (pathlen+1)*sizeof(char)))==NULL)  {
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }
  strcpy( newpath, filename);
  /* Check for and handle the existing path configuration */
  if (fdb_no==0) {   /* defauls setup, free the path */
    free( T1_FDB_ptr[0]);
  }
  if (fdbxlfd_no==-1) {  /* not initialized! */
    fdbxlfd_no=0;
    T1_FDBXLFD_ptr=NULL; /* realloc() will do the malloc()! */
  }
  
  if ((T1_FDBXLFD_ptr=(char**)realloc( T1_FDBXLFD_ptr, (++fdbxlfd_no+1)*sizeof(char*)))==NULL) {
    T1_errno=T1ERR_ALLOC_MEM;
    return(-1);
  }
  /* Insert the new database. If t1lib is already initialzed, the database can only
     be appended. Otherwise. prepending is also possible.*/
  if ((mode & T1_PREPEND_PATH) && (T1_CheckForInit()!=0) ) { /* prepend */
    i=fdbxlfd_no-2;
    while (i>=0) {
      T1_FDBXLFD_ptr[i+1]=T1_FDBXLFD_ptr[i];
      i--;
    }
    T1_FDBXLFD_ptr[0]=newpath;
    result=0;
  }
  else { /* append */
    T1_FDBXLFD_ptr[fdbxlfd_no-1]=newpath;
    if (T1_CheckForInit()==0) {
      if ((result=intT1_scanFontDBaseXLFD(T1_FDBXLFD_ptr[fdbxlfd_no-1]))==-1) {
	T1_PrintLog( "T1_AddFontDataBase()", "Fatal error scanning Font Database File %s (T1_errno=%d)",
		     T1LOG_WARNING, T1_FDBXLFD_ptr[fdbxlfd_no-1], T1_errno);
      }
      if (result>-1)
	pFontBase->no_fonts+=result;
      result=pFontBase->no_fonts;
    }
  }
  T1_FDBXLFD_ptr[fdbxlfd_no]=NULL;
  return result;
  
}


