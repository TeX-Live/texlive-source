/************************************************************************

    Generic script wrapper

    Public Domain
    Originally written 2009 by T.M.Trzeciak

    For rationale and structure details see runscript.tlu script.

    Compilation with gcc (size optimized):
    gcc -Os -s -shared -o runscript.dll runscript_dll.c -L./ -lluatex
    gcc -Os -s -o runscript.exe runscript_exe.c -L./ -lrunscript

    Compilation with tcc (extra small size):
    tiny_impdef luatex.dll
    tcc -shared -o runscript.dll runscript_dll.c luatex.def
    tcc -o runscript.exe runscript_exe.c runscript.def

************************************************************************/

#include <windows.h>

__declspec(dllimport) int dllrunscript( int argc, char *argv[] );

int main( int argc, char *argv[] ) { return dllrunscript( argc, argv ); }
