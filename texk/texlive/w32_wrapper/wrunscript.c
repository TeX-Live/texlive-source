/************************************************************************

    Generic script wrapper

    Public Domain
    Originally written 2009 by T.M.Trzeciak

    This is a GUI subsystem version of the binary stub.
    For more general info on wrapper structure see runscript.tlu.

    Compilation with gcc (size optimized):
    gcc -Os -s -o wrunscript.exe wrunscript.c -L./ -lluatex

    Compilation with tcc (extra small size):
    tiny_impdef luatex.dll
    tcc -o wrunscript.exe wrunscript.c luatex.def

************************************************************************/

#include <stdio.h>
#include <windows.h>
#define MAX_MSG 1024
#define IS_WHITESPACE(c) ((c == ' ') || (c == '\t'))
#define DIE(...) {\
  _snprintf( msg_buf, MAX_MSG - 1, __VA_ARGS__ );\
  fprintf( stderr, msg_buf );\
  MessageBox( NULL, msg_buf, own_path, MB_ICONERROR | MB_SETFOREGROUND );\
  return 1;\
}

const char err_env_var[] = "RUNSCRIPT_ERROR_MESSAGE";
const char script_name[] = "runscript.tlu";
static char own_path[MAX_PATH] = "(NULL)";
static char msg_buf[MAX_MSG];

void finalize( void )
{
  // check for and display error message if any
  char *err_msg;
  if ( err_msg = (char *) getenv(err_env_var) )
    MessageBox( NULL, err_msg, own_path, MB_ICONERROR | MB_SETFOREGROUND );
}

__declspec(dllimport) int dllluatexmain( int argc, char *argv[] );

int APIENTRY WinMain( 
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  char *argline,
  int winshow 
){
  char argv0[MAX_PATH];
  char fpath[MAX_PATH];
  char *fname, *fext, **lua_argv;
  int k, quoted, lua_argc;
  
  // clear error var in case it exists already
  SetEnvironmentVariable(err_env_var, NULL);
  
  // file path of this executable
  k = (int) GetModuleFileName(NULL, own_path, MAX_PATH);
  if ( !k || (k == MAX_PATH) ) 
    DIE("could not get own path (may be too long): %s\n", own_path);
 
  // script path
  strcpy(fpath, own_path);
  fname = strrchr(fpath, '\\');
  if ( fname == NULL ) DIE("no directory part in module path: %s\n", fpath);
  fname++;
  if ( fname + strlen(script_name) >= fpath + MAX_PATH - 1 ) 
    DIE("path too long: %s\n", fpath);
  strcpy(fname, script_name);
  if ( GetFileAttributes(fpath) == INVALID_FILE_ATTRIBUTES ) 
    DIE("main lua script not found: %s\n", fpath);

  // set up argument list for texlua script
  lua_argc = 5;
  lua_argv = (char **) malloc( ( lua_argc + 1 ) * sizeof( char * ) );
  lua_argv[0] = "texlua"; // just a bare name, luatex strips the rest anyway
  lua_argv[1] = fpath; // script to execute
  lua_argv[lua_argc - 3] = "GUI_MODE\n"; // sentinel argument
  lua_argv[lua_argc - 2] = own_path; // our argv[0]
  lua_argv[lua_argc - 1] = argline; // unparsed arguments
  lua_argv[lua_argc] = NULL;

  // dllluatexmain  never returns, it exits instead
  // register atexit handler to recover control
  atexit(finalize);
  return dllluatexmain( lua_argc, lua_argv );
}
