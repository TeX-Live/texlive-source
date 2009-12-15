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

#include <stdio.h>
#include <windows.h>
#define IS_WHITESPACE(c) ((c == ' ') || (c == '\t'))
#define MAX_MSG 512
#define DIE(...) { _snprintf( msg_buf, MAX_MSG - 1, __VA_ARGS__ ); goto DIE; }

const char module_name[] = "runscript.dll";
const char script_name[] = "runscript.tlu";
static char msg_buf[MAX_MSG];

__declspec(dllimport) int dllluatexmain( int argc, char *argv[] );

__declspec(dllexport) int dllrunscript( int argc, char *argv[] ) 
{
  static char fpath[MAX_PATH];
  char *fname, *argline, **lua_argv;
  int k, quoted, lua_argc;

  // file path of this executable
  k = (int) GetModuleFileName(NULL, fpath, MAX_PATH);
  if ( !k || (k == MAX_PATH) ) 
    DIE("cannot get own path (may be too long): %s\n", fpath);

  // script path
  fname = strrchr(fpath, '\\');
  if ( fname == NULL ) DIE("no directory part in module path: %s\n", fpath);
  fname++;
  if ( fname + strlen(script_name) >=  fpath + MAX_PATH - 1 ) 
    DIE("path too long: %s\n", fpath);
  strcpy(fname, script_name);
  if ( GetFileAttributes(fpath) == INVALID_FILE_ATTRIBUTES ) 
    DIE("lua script not found: %s\n", fpath);

  // get command line of this process
  argline = GetCommandLine();
  // skip over argv[0] (it can contain embedded double quotes if launched from cmd.exe!)
  for ( quoted = 0; (*argline) && ( !IS_WHITESPACE(*argline) || quoted ); argline++ )
    if ( *argline == '"' ) quoted = !quoted;
  while ( IS_WHITESPACE(*argline) ) argline++; // remove leading whitespace if any

  // set up argument list for texlua script
  lua_argc = argc + 4;
  lua_argv = (char **) malloc( (lua_argc + 1) * sizeof(char *) );
  lua_argv[0] = strdup("texlua"); // just a bare name, luatex strips the rest anyway
  lua_argv[1] = fpath; // script to execute
  for ( k = 1; k < argc; k++ ) lua_argv[k+1] = argv[k];
  lua_argv[lua_argc - 3] = strdup("CLI_MODE\n"); // sentinel argument
  lua_argv[lua_argc - 2] = argv[0]; // original argv[0]
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
  return 1;
}
