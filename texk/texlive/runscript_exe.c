// an .exe part of runscript program
// see runscript_dll.c for more details
#include <windows.h>
__declspec(dllimport) int dllrunscript( argc, argv );
int main( int argc, char *argv[] ) {
  return dllrunscript( argc, argv );
}
