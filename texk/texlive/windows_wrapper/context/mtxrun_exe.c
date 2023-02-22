// This is the .exe part of the mtxrun program, see mtxrun_dll.c
// for more details.

#include <windows.h>

__declspec(dllimport) int dllrunscript( int argc, char *argv[] );

int main( int argc, char *argv[] ) { return dllrunscript( argc, argv ); }
