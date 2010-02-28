#include <windows.h>

__declspec(dllimport) int dllwrunscript(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  char *argline,
  int winshow 
 );

int APIENTRY WinMain( 
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  char *argline,
  int winshow 
){
  return dllwrunscript( hInstance, hPrevInstance, argline, winshow );
}
