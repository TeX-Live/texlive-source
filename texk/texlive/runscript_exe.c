// an .exe part of runscript program
// see runscript_dll.c for more details
#include <windows.h>
__declspec(dllimport) int dllrunscript( int ac, char *av[] );
int main( int argc, char *argv[] ) {
  return dllrunscript( argc, argv );
}
