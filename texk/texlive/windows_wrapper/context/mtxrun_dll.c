/************************************************************************

  Copyright:

  Public Domain
  Originally written in 2010 by Tomasz M. Trzeciak and Hans Hagen

  This program is derived from the 'runscript' program originally
  written in 2009 by T.M. Trzeciak. It has been adapted for use in
  ConTeXt MkIV.

  Comment:

  In ConTeXt MkIV we have two core scripts: luatools.lua and
  mtxrun.lua where the second one is used to launch other scripts.
  Normally a user will use a call like:

  mtxrun --script font --reload

  Here mtxrun is a lua script. In order to avoid the usage of a cmd
  file on windows this runner will start texlua directly. If the
  shared library luatex.dll is available, texlua will be started in
  the same process avoiding thus any additional overhead. Otherwise
  it will be spawned in a new proces.

  We also don't want to use other runners, like those that use kpse
  to locate the script as this is exactly what mtxrun itself is doing
  already. Therefore the runscript program is adapted to a more direct
  approach suitable for mtxrun.

  Compilation:

  with gcc (size optimized):

  gcc -Os -s -shared -o mtxrun.dll mtxrun_dll.c
  gcc -Os -s -o mtxrun.exe mtxrun_exe.c -L./ -lmtxrun

  with tcc (extra small size):

  tcc -shared -o mtxrun.dll mtxrun_dll.c
  tcc -o mtxrun.exe mtxrun_exe.c mtxrun.def

************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//#define STATIC
#define IS_WHITESPACE(c) ((c == ' ') || (c == '\t'))
#define MAX_CMD 32768
#define DIE(...) { \
  fprintf( stderr, "mtxrun: " ); \
  fprintf( stderr, __VA_ARGS__ ); \
  return 1; \
}

static char cmdline[MAX_CMD];
static char dirpath[MAX_PATH];
static char progname[MAX_PATH];
static char scriptpath[MAX_PATH];
static char luatexpath[MAX_PATH];
HMODULE dllluatex = NULL;
typedef int ( *mainlikeproc )( int, char ** );

#ifdef STATIC
int main( int argc, char *argv[] )
#else
__declspec(dllexport) int dllrunscript( int argc, char *argv[] )
#endif
{
  char *binary, *s, *luatexfname, *argstr, **lua_argv;
  int k, quoted, lua_argc;
  int passprogname = 0;
  unsigned char is_jit=0;

  // directory of this module/executable
  HMODULE module_handle = GetModuleHandle( "mtxrun.dll" );
  // if ( module_handle == NULL ) exe path will be used, which is OK too
  k = (int) GetModuleFileName( module_handle, dirpath, MAX_PATH );
  if ( !k || ( k == MAX_PATH ) )
    DIE( "unable to determine a valid module name\n" );
  s = strrchr(dirpath, '\\');
  if ( s == NULL ) DIE( "no directory part in module path: %s\n", dirpath );
  *(++s) = '\0'; //remove file name, leave trailing backslash

  // program name
  k = strlen(argv[0]);
  while ( k && (argv[0][k-1] != '/') && (argv[0][k-1] != '\\') ) k--;
  strcpy(progname, &argv[0][k]);
  s = progname;
  if ( s = strrchr(s, '.') ) *s = '\0'; // remove file extension part

   /* check "jit" : strlen("jit") = 3 */
  if (strncmp(progname + strlen(progname) - 3, "jit", 3) == 0) {
       is_jit = 1;
       progname[strlen(progname) - 3]='\0';
  }
  else
       is_jit = 0;
 
  // script path

  strcpy( scriptpath, dirpath );
  k = strlen(progname);
  if ( k < 6 ) k = 6; // in case the program name is shorter than "mtxrun"
  if ( strlen(dirpath) + k + 4 >=  MAX_PATH )
    DIE( "path too long: %s%s\n", dirpath, progname );

  if ( strcmpi(progname,"mtxrun") == 0 ) {
    strcat( scriptpath, progname );
    strcat( scriptpath, ".lua" );
  } else if ( strcmpi(progname,"luatools") == 0 ) {
    strcat( scriptpath, "mtxrun.lua" );
    strcpy( progname, "base" );
    passprogname = 1;
  } else if ( strcmpi(progname,"texmfstart") == 0 ) {
    strcat( scriptpath, "mtxrun.lua" );
  } else {
    strcat( scriptpath, "mtxrun.lua" );
    passprogname = 1;
  }
  if ( GetFileAttributes(scriptpath) == INVALID_FILE_ATTRIBUTES )
    DIE( "file not found: %s\n", scriptpath );

  // find luatex.exe /luajittex.exe
  if ( SearchPath(
		  dirpath, // was getenv( "PATH" ), // path to search (optional)
		  (is_jit ? "luajittex.exe":"luatex.exe"),     // file name to search
		  NULL,             // file extension to add (optional)
		  MAX_PATH,         // output buffer size
		  luatexpath,       // output buffer pointer
		  &luatexfname )    // pointer to a file part in the output buffer (optional)
       ) {
    binary = (is_jit ? "luajittex.exe":"luatex.exe");
  } else if ( SearchPath(
			 dirpath, // was getenv( "PATH" ), // path to search (optional)
			 (is_jit ? "texluajit.exe":"texlua.exe"),     // file name to search
			 NULL,             // file extension to add (optional)
			 MAX_PATH,         // output buffer size
			 luatexpath,       // output buffer pointer
			 &luatexfname )    // pointer to a file part in the output buffer (optional)
	      ) {
    binary = (is_jit ? "texluajit.exe":"texlua.exe");
  } else if ( SearchPath(
			 getenv("PATH"), // was dirpath,          // path to search (optional)
			 (is_jit ? "luajittex.exe":"luatex.exe"),     // file name to search
			 NULL,             // file extension to add (optional)
			 MAX_PATH,         // output buffer size
			 luatexpath,       // output buffer pointer
			 &luatexfname )    // pointer to a file part in the output buffer (optional)
	      ) {
    binary = (is_jit ? "luajittex.exe":"luatex.exe");
  } else if ( SearchPath(
			 getenv("PATH") , // was dirpath,          // path to search (optional)
			 (is_jit ? "texluajit.exe":"texlua.exe"),     // file name to search
			 NULL,             // file extension to add (optional)
			 MAX_PATH,         // output buffer size
			 luatexpath,       // output buffer pointer
			 &luatexfname )    // pointer to a file part in the output buffer (optional)
	      ) {
    binary = (is_jit ? "texluajit.exe":"texlua.exe");
  }else {
    DIE( "unable to locate texlua.exe on the search path" );
  }

  /* if ( SearchPath( */
  /* 	   dirpath, // was getenv( "PATH" ), // path to search (optional) */
  /* 	   (is_jit ? "luajittex.exe":"luatex.exe"),     // file name to search */
  /* 	   NULL,             // file extension to add (optional) */
  /* 	   MAX_PATH,         // output buffer size */
  /* 	   luatexpath,       // output buffer pointer */
  /* 	   &luatexfname )    // pointer to a file part in the output buffer (optional) */
  /*      ) { */
  /*   binary = (is_jit ? "luajittex.exe":"luatex.exe"); */
  /* }else if ( SearchPath( */
  /* 	       getenv("PATH"), // was dirpath,          // path to search (optional) */
  /* 	       (is_jit ? "luajittex.exe":"luatex.exe"),     // file name to search */
  /* 	       NULL,             // file extension to add (optional) */
  /* 	       MAX_PATH,         // output buffer size */
  /* 	       luatexpath,       // output buffer pointer */
  /* 	       &luatexfname )    // pointer to a file part in the output buffer (optional) */
  /* 	     ) { */
  /*   binary = (is_jit ? "luajittex.exe":"luatex.exe"); */
  /* }else if ( SearchPath( */
  /* 	   dirpath, // was getenv( "PATH" ), // path to search (optional) */
  /* 	   (is_jit ? "texluajit.exe":"texlua.exe"),     // file name to search */
  /* 	   NULL,             // file extension to add (optional) */
  /* 	   MAX_PATH,         // output buffer size */
  /* 	   luatexpath,       // output buffer pointer */
  /* 	   &luatexfname )    // pointer to a file part in the output buffer (optional) */
  /*      ) { */
  /*   binary = (is_jit ? "texluajit.exe":"texlua.exe"); */
  /* }else if ( SearchPath( */
  /* 	       getenv("PATH") , // was dirpath,          // path to search (optional) */
  /* 	       (is_jit ? "texluajit.exe":"texlua.exe"),     // file name to search */
  /* 	       NULL,             // file extension to add (optional) */
  /* 	       MAX_PATH,         // output buffer size */
  /* 	       luatexpath,       // output buffer pointer */
  /* 	       &luatexfname )    // pointer to a file part in the output buffer (optional) */
  /* 	     ) { */
  /*   binary = (is_jit ? "texluajit.exe":"texlua.exe"); */
  /* }else { */
  /*   DIE( "unable to locate texlua.exe on the search path" ); */
  /* } */




  // link directly with luatex.dll if available in texlua's dir
  strcpy( luatexfname, (is_jit ? "luajittex.dll":"luatex.dll") );
  if ( dllluatex = LoadLibrary(luatexpath) )
  {
    mainlikeproc dllluatexmain = (mainlikeproc) GetProcAddress( dllluatex, (is_jit ? "dllluajittexmain": "dllluatexmain" ));
    if ( dllluatexmain == NULL )
      if (is_jit)
       DIE( "unable to locate dllluatexmain procedure in luajittex.dll"  )
     else 
       DIE( "unable to locate dllluatexmain procedure in luatex.dll"  );

    // set up argument list for texlua script

    lua_argv = (char **)malloc( (argc + 5) * sizeof(char *) );
    if ( lua_argv == NULL ) DIE( "out of memory\n" );
    lua_argv[lua_argc=0] =  luatexfname;
    lua_argv[++lua_argc] = "--luaonly";
    lua_argv[++lua_argc] = scriptpath; // script to execute
    if (passprogname) {
      lua_argv[++lua_argc] = "--script";
      lua_argv[++lua_argc] = progname;
    }
    for ( k = 1; k < argc; k++ ) lua_argv[++lua_argc] = argv[k];
    lua_argv[++lua_argc] = NULL;

    // call texlua interpreter
    // dllluatexmain  never returns, but we pretend that it does
    
    k = dllluatexmain( lua_argc, lua_argv );
    if (lua_argv) free( lua_argv );
    return k;
  }
  // we are still here, so no luatex.dll; spawn texlua.exe instead

  strcpy( luatexfname,binary);
  strcpy( cmdline, " --luaonly " );
  strcpy( cmdline, "\"" );
  strcat( cmdline, luatexpath );
  strcat( cmdline, "\" \"" );
  strcat( cmdline, scriptpath );
  strcat( cmdline, "\"" );
  if (passprogname) {
    strcat( cmdline, " --script " );
    strcat( cmdline, progname );
  }
  argstr = GetCommandLine(); // get the command line of this process
  if ( argstr == NULL ) DIE( "unable to retrieve the command line string\n" );

  // skip over argv[0] in the argument string
  // (it can contain embedded double quotes if launched from cmd.exe!)

  for ( quoted = 0; (*argstr) && ( !IS_WHITESPACE(*argstr) || quoted ); argstr++ )
    if (*argstr == '"') quoted = !quoted;

  // pass through all the arguments

  if ( strlen(cmdline) + strlen(argstr) >= MAX_CMD )
    DIE( "command line string too long:\n%s%s\n", cmdline, argstr );
  strcat( cmdline, argstr );

  // create child process

  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;// | STARTF_USESHOWWINDOW;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_HIDE ; // can be used to hide console window (requires STARTF_USESHOWWINDOW flag)
	si.hStdInput  = GetStdHandle( STD_INPUT_HANDLE );
	si.hStdOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	si.hStdError  = GetStdHandle( STD_ERROR_HANDLE );
  ZeroMemory( &pi, sizeof(pi) );
  if( !CreateProcess(
    NULL,     // module name (uses command line if NULL)
    cmdline,  // command line
    NULL,     // process security atrributes
    NULL,     // thread security atrributes
    TRUE,     // handle inheritance
    0,        // creation flags, e.g. CREATE_NEW_CONSOLE, CREATE_NO_WINDOW, DETACHED_PROCESS
    NULL,     // pointer to environment block (uses parent if NULL)
    NULL,     // starting directory (uses parent if NULL)
    &si,      // STARTUPINFO structure
    &pi )     // PROCESS_INFORMATION structure
  ) DIE( "command execution failed: %s\n", cmdline );
  DWORD ret = 0;
  CloseHandle( pi.hThread ); // thread handle is not needed
  if ( WaitForSingleObject( pi.hProcess, INFINITE ) == WAIT_OBJECT_0 ) {
    if ( !GetExitCodeProcess( pi.hProcess, &ret) )
        DIE( "unable to retrieve process exit code: %s\n", cmdline );
  } else DIE( "failed to wait for process termination: %s\n", cmdline );
  CloseHandle( pi.hProcess );

  // propagate exit code from the child process
  return ret;

}
