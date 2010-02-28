/************************************************************************

    Generic script wrapper

    Public Domain
    Originally written 2009 by T.M.Trzeciak

    For rationale and structure details see runscript.tlu script.

    Compilation with gcc (size optimized):
    gcc -Os -s -shared -o runscript.dll runscript_dll.c -L./ -lluatex
    gcc -Os -s -o runscript.exe runscript_exe.c -L./ -lrunscript
    gcc -mwindows -Os -s -o wrunscript.exe wrunscript_exe.c -L./ -lrunscript

    Compilation with tcc (extra small size):
    tiny_impdef luatex.dll
    tcc -shared -o runscript.dll runscript_dll.c luatex.def
    tcc -o runscript.exe runscript_exe.c runscript.def
    tcc -o wrunscript.exe wrunscript_exe.c runscript.def

************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define IS_WHITESPACE(c) ((c == ' ') || (c == '\t'))
#define MAX_MSG 2*MAX_PATH
#define DIE(...) { _snprintf( msg_buf, MAX_MSG - 1, __VA_ARGS__ ); goto DIE; }

char module_name[] = "runscript.dll";
char script_name[] = "runscript.tlu";
char texlua_name[] = "texlua"; // just a bare name, luatex strips the rest anyway
char subsys_mode[] = "CUI_MODE\n";
char err_env_var[] = "RUNSCRIPT_ERROR_MESSAGE";
char msg_buf[MAX_MSG];

__declspec(dllimport) int dllluatexmain( int argc, char *argv[] );

__declspec(dllexport) int dllrunscript( int argc, char *argv[] ) 
{
  static char own_path[MAX_PATH];
  static char fpath[MAX_PATH];
  char *fname, *argline, **lua_argv;
  int k, quoted, lua_argc;

  // file path of this executable
  k = (int) GetModuleFileName(NULL, own_path, MAX_PATH);
  if ( !k || (k == MAX_PATH) ) 
    DIE("cannot get own path (may be too long): %s\n", own_path);

  // script path
  strcpy(fpath, own_path);
  fname = strrchr(fpath, '\\');
  if ( fname == NULL ) DIE("no directory part in module path: %s\n", fpath);
  fname++;
  if ( fname + strlen(script_name) >=  fpath + MAX_PATH - 1 ) 
    DIE("path too long: %s\n", fpath);
  strcpy(fname, script_name);
  if ( GetFileAttributes(fpath) == INVALID_FILE_ATTRIBUTES ) 
    DIE("main lua script not found: %s\n", fpath);

  // get command line of this process
  argline = GetCommandLine();
  if ( argline == NULL ) DIE("failed to retrieve command line string\n");
  // skip over argv[0] (it can contain embedded double quotes if launched from cmd.exe!)
  for ( quoted = 0; (*argline) && ( !IS_WHITESPACE(*argline) || quoted ); argline++ )
    if ( *argline == '"' ) quoted = !quoted;
  while ( IS_WHITESPACE(*argline) ) argline++; // remove leading whitespace if any

  // set up argument list for texlua script
  lua_argc = argc ? argc + 4 : 5;
  lua_argv = (char **)malloc( (lua_argc + 1) * sizeof(char *) );
  lua_argv[0] = texlua_name;
  lua_argv[1] = fpath; // script to execute
  for ( k = 1; k < argc; k++ ) lua_argv[k+1] = argv[k]; // copy argument list
  lua_argv[lua_argc - 3] = subsys_mode; // sentinel argument
  lua_argv[lua_argc - 2] = argc ? argv[0] : own_path; // original argv[0]
  lua_argv[lua_argc - 1] = argline; // unparsed arguments
  lua_argv[lua_argc] = NULL;

  // call texlua interpreter
  // dllluatexmain  never returns, but we pretend that it does
  k = dllluatexmain( lua_argc, lua_argv );
  if (lua_argv) free(lua_argv);
  return k;

DIE:
  fprintf(stderr, "%s: ", module_name);
  fprintf(stderr, msg_buf);
  if (*subsys_mode == 'G')
    MessageBox( NULL, msg_buf, module_name, MB_ICONERROR | MB_SETFOREGROUND );
  return 1;
}

void finalize( void )
{
  // check for and display error message if any
  char *err_msg;
  if ( err_msg = (char *) getenv(err_env_var) )
    MessageBox( NULL, err_msg, module_name, MB_ICONERROR | MB_SETFOREGROUND );
}

__declspec(dllexport) int dllwrunscript( 
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  char *argline,
  int winshow 
) {
  // set sentinel argument
  *subsys_mode = 'G';
  // clear error var in case it exists already
  SetEnvironmentVariable(err_env_var, NULL);
  // register atexit handler to recover control before terminating
  atexit( finalize );
  // call the console entry point routine
#ifdef __MSVCRT__
  // WinMain doesn't provide argc & argv, call MSVCRT proc to get them
	int argc = 0; 
  char **argv, **env; 
  int expand_wildcards = 0;
  int new_mode;
  __getmainargs(&argc, &argv, &env, expand_wildcards, &new_mode);
  return dllrunscript( argc, argv );
#else
  return dllrunscript( 0, NULL );
#endif
}


