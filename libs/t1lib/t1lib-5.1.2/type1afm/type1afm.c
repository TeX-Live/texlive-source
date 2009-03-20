/*--------------------------------------------------------------------------
  ----- File:        type1afm.c 
  ----- Author:      Rainer Menzner (Rainer.Menzner@web.de)
  ----- Date:        2002-11-27
  ----- Description: This file is part of t1lib. The program type1afm 
                     extracts metrics information from Type 1 font files
		     and dumps it to  afm files.
  ----- Copyright:   t1lib is copyrighted (c) Rainer Menzner, 1996-2002.
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
/* Note: We include t1lib.h from lib/t1lib. That way the objectfile does only
   need to be rebuild when the header itself changes and not each time the
   library has been recompiled */
#include "../lib/t1lib/t1lib.h"


void printusage( void);


int main(int argc, char *argv[])
{

  int i, j, result;
  int rasterflags=0;
  
  
  if (argc==1){
    printusage();
    return(0);
  }

  /* If we want to log anything, then log all */
  T1_SetLogLevel(T1LOG_DEBUG);
  /* Check whether logfile is requested */
  if (strcmp( argv[1], "-l")==0){
    if (argc==2){
      printusage();
      return(0);
    }
    j=2;
    if ((T1_InitLib( LOGFILE |
		     IGNORE_CONFIGFILE |
		     IGNORE_FONTDATABASE)==NULL)){
      fprintf(stderr, "Initialization of t1lib failed\n");
      return(1);
    }
  }
  else{
    j=1;
    if ((T1_InitLib( NO_LOGFILE |
		     IGNORE_CONFIGFILE |
		     IGNORE_FONTDATABASE)==NULL)){
      fprintf(stderr, "Initialization of t1lib failed\n");
      return(1);
    }
  }
  
  /* First, build font data base */
  for (i=j; i<argc; i++){
    if ((result=T1_AddFont( argv[i]))<0)
      fprintf( stderr, "type1afm: Could not load font file %s (%d)\n",
	       argv[i], result);
  }
  if (T1_GetNoFonts()<1){
    fprintf( stderr, "type1afm: Nothing to do\n");
    T1_CloseLib();
    return(-1);
  }

  /* switch rasterizer to disable hinting */
  rasterflags |= T1_IGNORE_FORCEBOLD;
  rasterflags |= T1_IGNORE_FAMILYALIGNMENT;
  rasterflags |= T1_IGNORE_HINTING;
  T1_SetRasterFlags( rasterflags);

  
  /* Second, load fonts and generate afm files */
  for( i=0; i<T1_GetNoFonts(); i++){
    fprintf( stdout, "Loading %s ... ", T1_GetFontFileName(i));
    fflush(stdout);
    if ((T1_LoadFont(i))){
      fprintf(stdout, "failed\n");
      continue;
    }
    else{
      fprintf(stdout, "finished\n");
    }
    fprintf( stdout, "Processing ... ");
    fflush(stdout);
    if ((result=T1_WriteAFMFallbackFile( i))){
      fprintf(stdout, "failed (Error: %d)\n", result);
    }
    else{
      fprintf(stdout, "finished\n");
    }
    fprintf( stdout, "Removing font %d ... ", i);
    fflush(stdout);
    if ((T1_DeleteFont( i))){
      fprintf(stdout, "failed\n");
    }
    else{
      fprintf(stdout, "finished\n");
    }
  }
  
  T1_CloseLib();
  return( 0);
}


void printusage( void)
{
  fprintf(stdout, "Usage: type1afm [-l] <fontfile1> [<fontfile2> ...]\n");
  fprintf(stdout, "\n");
  fprintf(stdout, "Generate afm-file from Adobe Type 1 font file (T1lib-%s)!\n",
	  T1_GetLibIdent());
  fprintf(stdout, "Options: -l        Write a log-file t1lib.log\n");
}

