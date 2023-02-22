/************************************************************************

  This file is a part of the wrapper program for launching scripts
  and programs in TeX Live on Windows. See readme.txt for more details.
  
  This file was originally written in 2009 by Tomasz M. Trzeciak and
  was placed in the Public Domain.
 
************************************************************************/

#include <windows.h>

__declspec(dllimport) int dllrunscript( int argc, char *argv[] );

int main( int argc, char *argv[] ) { return dllrunscript( argc, argv ); }
