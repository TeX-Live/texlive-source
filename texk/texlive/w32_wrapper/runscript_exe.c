#include <windows.h>

__declspec(dllimport) int dllrunscript( int argc, char *argv[] );

int main( int argc, char *argv[] ) { return dllrunscript( argc, argv ); }
