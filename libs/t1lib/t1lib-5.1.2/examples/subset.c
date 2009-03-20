/*--------------------------------------------------------------------------
  ----- File:        subset.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2007-12-22
  ----- Description: This file is part of t1lib. The program subset reads
                     a font file name and a string from the commandline
		     and creates a subset of the font which is sufficient
		     to display the string. It is an example of how to use
		     font subsetting.
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Note: We include t1lib.h from lib/t1lib. That way the objectfile does only
   need to be rebuild when the header itself changes and not each time the
   library has been recompiled */
#include "../lib/t1lib/t1lib.h"


void printusage( void);


int main(int argc, char *argv[])
{

  int i, j, result;
  int realargs=0;
  int logfile=0;
  char mask[256];
  int flags=T1_SUBSET_DEFAULT;
  char *subset=NULL;
  unsigned long ofsize=0;
  FILE *ofp=stdout;
  
  char** encoding     = 0;
  char*  encodingfile = 0;
  int    doreencode   = 0;
  int    reresult     = 0;

  
  if (argc==1){
    printusage();
    return(0);
  }

  /* If we want to log anything, then log all */
  T1_SetLogLevel(T1LOG_DEBUG);

  realargs=argc-1;

  for (j=1; j<argc; j++) {
    if (argv[j][0]=='-') {
      if (strcmp( argv[j], "-l")==0) {
	realargs -=1;
	logfile=1;
      }
      else if (strcmp( argv[j], "-a")==0) {
	/* default case */
	realargs -=1;
      }
      else if (strcmp( argv[j], "-b")==0) {
	realargs -=1;
	flags |=T1_SUBSET_ENCRYPT_BINARY;
      }
      else if (strcmp( argv[j], "-f")==0) {
	realargs -=1;
	flags |=T1_SUBSET_FORCE_REENCODE;
      }
      else if (strcmp( argv[j], "-s")==0) {
	realargs -=1;
	flags |=T1_SUBSET_SKIP_REENCODE;
      }
      else if (strcmp( argv[j], "-e")==0) {
	--realargs;
	if ( j == argc-1 ) {
	  fprintf(stderr, "subset: Option \"-e\": Missing filename argument.\n");
	  printusage();
	  return 1;
	}
	++j;
	encodingfile=argv[j];
	doreencode=1;
	--realargs;
      }
      else {
	fprintf(stderr, "subset: Unknown option: %s\n", argv[j]);
	printusage();
	return( 1);
      }
    }
    else {
      i=j;  /* this is assumed to be the first font file name */
      break;
    }
  }
  
  
  if (realargs<2) {
    fprintf( stderr, "subset: Need at least one font file and one string\n");
    printusage();
    return( 1);
  }
  
  
  if (logfile!=0) {
    if ((T1_InitLib( LOGFILE | 
		     IGNORE_CONFIGFILE |
		     IGNORE_FONTDATABASE)==NULL)) {
      fprintf(stderr, "subset: Initialization of t1lib failed (T1_errno=%d)\n",
	      T1_errno);
      return(1);
    }
  }
  else {
    if ((T1_InitLib( NO_LOGFILE | 
		     IGNORE_CONFIGFILE |
		     IGNORE_FONTDATABASE)==NULL)){
      fprintf(stderr, "subset: Initialization of t1lib failed (T1_errno=%d)\n", T1_errno);
      return(1);
    }
  }

  /* Load encoding file if specified */
  if ( doreencode != 0 ) {
    if ( (encoding = T1_LoadEncoding( encodingfile)) == NULL ) {
      fprintf( stderr, "subset: Could not load Encoding File %s (T1_errno=%d, %s).\n",
	       encodingfile, T1_errno, T1_StrError(T1_errno));
      exit( 0);
    }
  }
  

  /* First, build font data base */
  for (i=j; i<argc-1; i++){
    if ((result=T1_AddFont( argv[i]))<0)
      fprintf( stderr, "subset: Could not load font file %s (T1_errno=%d)\n",
	       argv[i], T1_errno);
  }
  if (T1_GetNoFonts()<1){
    fprintf( stderr, "subset: Nothing to do\n");
    T1_CloseLib();
    return( 2);
  }
  
  /* second, setup subsetting mask from commandline string */
  for ( i=0; i<256; i++) {
    mask[i]=0;
  }
  for (i=0; i<strlen(argv[argc-1]); i++) {
    mask[(unsigned char)argv[argc-1][i]]=1;
  }
  
  /* third, load fonts, generate subset and write it stdout. */
  for( i=0; i<T1_GetNoFonts(); i++){
    fprintf( stderr, "Loading %s ... ", T1_GetFontFileName(i));
    fflush(stdout);
    if ((T1_LoadFont(i))){
      fprintf(stderr, "failed\n");
      continue;
    }
    else {
      fprintf(stderr, "finished\n");
    }

    /* Reencode font */
    if ( doreencode != 0 ) {
      if ( (reresult = T1_ReencodeFont( i, encoding)) != 0 ) {
	fprintf( stderr, "Warning Reencoding font %d failed (%d)!\n", i, reresult);
      }
    }
  
    fprintf( stderr, "Processing ... ");
    fflush(stderr);
    if ((subset=T1_SubsetFont(i, mask, flags, 64, 16384, &ofsize))==NULL) {
      fprintf(stderr, "failed (T1_errno: %d)\n", T1_errno);
    }
    else {
      fwrite(subset, 1, ofsize, ofp);
      fprintf(stderr, "finished, wrote %lu bytes\n", ofsize);
    }
    fprintf( stderr, "Removing font %d ... ", i);
    fflush(stderr);
    if ((T1_DeleteFont( i))){
      fprintf(stderr, "failed\n");
    }
    else {
      fprintf(stderr, "finished\n");
    }
  }
  
  T1_CloseLib();
  return( 0);
}


void printusage( void)
{
  fprintf(stdout, "Usage: subset [-l|-a|-b|-f|-s|-e <encfile>] <fontfile1> [<fontfile2> ...] string\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Subset source fontfile(s) according to `string' and write result\nto stdout (T1Lib-%s)!\n\n",
	  T1_GetLibIdent());
  fprintf(stdout, "Options: -l                  Write a log-file t1lib.log.\n");
  fprintf(stdout, "         -a                  Create ASCII-encrypted file (default).\n");
  fprintf(stdout, "         -b                  Create Binary-enrypted file.\n");
  fprintf(stdout, "         -f                  Force reencoding of the font subset, even if\n");
  fprintf(stdout, "                             the source font uses internal StandardEncoding.\n");
  fprintf(stdout, "         -s                  Skip reencoding of the font subset, even if the\n");
  fprintf(stdout, "                             source font defines a font-specific encoding.\n");
  fprintf(stdout, "         -e <encodingfile>   Load an encoding from specified file and reencode\n");
  fprintf(stdout, "                             source font before starting to create the subset.\n");
  fprintf(stdout, "                             This option also implies [-f].\n");

  return;
}

