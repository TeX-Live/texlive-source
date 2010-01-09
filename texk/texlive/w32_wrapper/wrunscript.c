/************************************************************************

    Generic script wrapper

    Public Domain
    Originally written 2009 by T.M.Trzeciak

    This is a GUI subsystem version of the binary stub.
    For more general info on wrapper structure see runscript.tlu.

    Compilation with gcc (size optimized):
    gcc -mwindows -Os -s -o wrunscript.exe wrunscript.c -L./ -lluatex

    Compilation with tcc (extra small size):
    tiny_impdef luatex.dll
    tcc -o wrunscript.exe wrunscript.c luatex.def

************************************************************************/

#include <stdio.h>
#include <windows.h>
#define MAX_MSG 512
#define IS_WHITESPACE(c) ((c == ' ') || (c == '\t'))
#define DIE(...) { _snprintf( msg_buf, MAX_MSG - 1, __VA_ARGS__ ); goto DIE; }

char err_env_var[] = "RUNSCRIPT_ERROR_MESSAGE";
char script_name[] = "runscript.tlu";
char texlua_name[] = "texlua";
char sentinel_arg[] = "GUI_MODE\n";
static char own_path[MAX_PATH] = "runscript";
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
  char fpath[MAX_PATH];
  char *fname, *fext, **lua_argv;
  int k, lua_argc;
  int argc = 0;
  char **argv, **env;
  int new_mode = 0;
  int expand_wildcards = 0;
  
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

#ifdef __MSVCRT__
  // WinMain doesn't provide argc & argv, use MSVCRT function directly
  __getmainargs(&argc, &argv, &env, expand_wildcards, &new_mode);
#endif

  // set up argument list for texlua script
  lua_argc = argc ? argc + 4 : 5;
  lua_argv = (char **) malloc( (lua_argc + 1) * sizeof(char *) );
  lua_argv[0] = texlua_name; // just a bare name, luatex strips the rest anyway
  lua_argv[1] = fpath; // script to execute
  for ( k = 1; k < argc; k++ ) lua_argv[k+1] = argv[k];
  lua_argv[lua_argc - 3] = sentinel_arg; // sentinel argument
  lua_argv[lua_argc - 2] = argc ? argv[0] : own_path; // original argv[0]
  lua_argv[lua_argc - 1] = argline; // unparsed arguments
  lua_argv[lua_argc] = NULL;

  // register atexit handler to recover control before terminating
  atexit(finalize);
  // dllluatexmain  never returns, but we pretend that it does
  k = dllluatexmain( lua_argc, lua_argv );
  if (lua_argv) free(lua_argv);
  return k;
  
DIE:
  fprintf(stderr, "%s: ", own_path);
  fprintf(stderr, msg_buf);
  MessageBox( NULL, msg_buf, own_path, MB_ICONERROR | MB_SETFOREGROUND );
  return 1;
}
