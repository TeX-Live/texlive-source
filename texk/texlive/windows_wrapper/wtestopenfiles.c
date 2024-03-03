/***********************************************************

  This program tries to prevent doomed update attempts on windows by testing
  whether all files concerned can be opened with exclusive read/write access.

  This file is written in 2022 by Siep Kroonenberg and is placed
  in the Public Domain.

***********************************************************/

#include <windows.h>
#include <stdio.h>

int main( int argc, char *argv[] ) {
  HANDLE hf;
  char buf[MAX_PATH];
  int ok = 1;
  int chatty = 0;
  int i;

  if( argc > 1 ) {
    for( i=1;i<argc;i++ ) {
      if( argv[i][1] == 'v' ) {
        chatty = 1;
      } else {
        puts( "This program reads a list of absolute filepaths from stdin" );
        puts( "and errors out if any of them cannot be opened with" );
        puts( "exclusive read/write access." );
        puts( "A -v option produces status output on stdout." );
        exit( 1 );
      }
    }
  }

  while ( fgets( buf, MAX_PATH, stdin )) {
    buf[strlen(buf)-1] = '\0';
    if (buf[0] != '\0') {
      // buf represents a filepath
      hf = CreateFile(
                  buf,
                  GENERIC_READ | GENERIC_WRITE,
                  0, // not shared
                  NULL,
                  OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL,
                  NULL
                  );
      // OPEN_ALWAYS: creates missing file
      // OPEN_EXISTING: can fail with ERROR_FILE_NOT_FOUND
      if( hf == INVALID_HANDLE_VALUE ) {
        if( GetLastError() == ERROR_FILE_NOT_FOUND ) {
          if( chatty ) printf( "%s not present, is ok\n", buf );
          continue;
        } else {
          ok = 0;
          if( chatty ) printf( "%s present, but cannot be opened\n", buf );
          break;
        }
      } else {
        if( chatty ) printf( "%s can be exclusively opened, is ok\n", buf );
        CloseHandle( hf );
      }
    }
  }
  if( ok ) {
    if( chatty ) puts( "ok" );
    exit(0);
  } else {
    if( chatty ) puts( "not ok" );
    exit( 1 );
  }
}
