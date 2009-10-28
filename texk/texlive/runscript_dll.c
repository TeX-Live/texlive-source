/*

Public Domain
Originally written 2009 by T.M.Trzeciak

Batch script launcher:

Runs a batch file of the same name in the same location or sets 
TL_PROGNAME environment variable to its own name and calls 
the default batch script.

Rationale:

Batch scripts are not as universal as binary executables, there
are some odd cases where they are not interchangeable with them.

Usage:

Simply copy and rename the compiled program. The executable part 
is just a proxy the runscript function in runscript.dll. This arrangement
is for maintenance reasons - upgrades can be done by replacement of 
a single .dll rather than all .exe stubs

Compilation:

with gcc (size optimized):
gcc -Os -s -shared -o runscript.dll runscript_dll.c
gcc -Os -s -o runscript.exe runscript_exe.c -L./ -lrunscript

with tcc (ver. 0.9.24), extra small size
tcc -shared -o runscript.dll runscript_dll.c
tcc -o runscript.exe runscript_exe.c runscript.def

*/

#include <windows.h>
#include <stdio.h>
#define IS_WHITESPACE(c) ((c == ' ') || (c == '\t'))
#define DEFAULT_SCRIPT "tl-w32-wrapper.cmd"
#define MAX_CMD 32768
//#define DRYRUN

static char dirname[MAX_PATH];
static char basename[MAX_PATH];
static char progname[MAX_PATH];
static char cmdline[MAX_CMD];
char *envpath;

__declspec(dllexport) int dllrunscript( int argc, char *argv[] ) {
  int i;
  static char path[MAX_PATH];
  
  // get file name of this executable and split it into parts
  DWORD nchars = GetModuleFileNameA(NULL, path, MAX_PATH);
  if ( !nchars || (nchars == MAX_PATH) ) {
    fprintf(stderr, "runscript: cannot get own name or path too long\n");
    return -1;
  }
  // file extension part
  i = strlen(path);
  while ( i && (path[i] != '.') && (path[i] != '\\') ) i--;
  strcpy(basename, path);
  if ( basename[i] == '.' ) basename[i] = '\0'; //remove file extension
  // file name part
  while ( i && (path[i] != '\\') ) i--;
  if ( path[i] != '\\' ) {
    fprintf(stderr, "runcmd: no directory part in own name: %s\n", path);
    return -1;
  }
  strcpy(dirname, path);
  dirname[i+1] = '\0'; //remove file name, leave trailing backslash
  strcpy(progname, &basename[i+1]);

  // find program to execute
  if ( (strlen(basename)+4 >= MAX_PATH) || (strlen(dirname)+strlen(DEFAULT_SCRIPT) >= MAX_PATH) ) {
    fprintf(stderr, "runscript: path too long: %s\n", path);
    return -1;
  }
  // try .bat first
  strcpy(path, basename);
  strcat(path, ".bat");
  if ( GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES ) goto PROGRAM_FOUND;
  // try .cmd next
  strcpy(path, basename);
  strcat(path, ".cmd");
  if ( GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES ) goto PROGRAM_FOUND;
  // pass the program name through environment (generic launcher case)
  if ( !SetEnvironmentVariableA("TL_PROGNAME", progname) ) {
    fprintf(stderr, "runscript: cannot set evironment variable\n", path);
    return -1;
  }
  // check environment for default command
  /*if ( GetEnvironmentVariableA("TL_W32_WRAPPER", cmd, MAX_CMD) ) goto PROGRAM_FOUND;*/
  // use default script
  strcpy(path, dirname);
  strcat(path, DEFAULT_SCRIPT);
  if ( GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES ) {
    fprintf(stderr, "runscript: missing default script: %s\n", path);
    return -1;
  }
  
PROGRAM_FOUND:

  if ( !cmdline[0] ) {
    // batch file has to be executed through the call command in order to propagate its exit code
    // cmd.exe is searched for only on PATH to prevent attacks through writing ./cmd.exe
    envpath = (char *) getenv("PATH");
    if ( !envpath ) {
      fprintf(stderr, "runscript: failed to read PATH variable\n");
      return -1;
    }
    cmdline[0] = '"';
    if ( !SearchPathA( envpath,  /* Address of search path */
      "cmd.exe",  /* Address of filename */
      NULL,  /* Address of extension */
      MAX_PATH,  /* Size of destination buffer */
      &cmdline[1],  /* Address of destination buffer */
      NULL) /* File part of the full path */
    )	{
      fprintf(stderr, "runscript: cmd.exe not found on PATH\n");
      return -1;
    }
    strcat(cmdline, "\" /c call \"");
    strcat(cmdline, path);
    strcat(cmdline, "\" ");
  }
  
  // get the command line for this process
  char *argstr;
  argstr = GetCommandLineA();
  if ( argstr == NULL ) {
    fprintf(stderr, "runscript: cannot get command line string\n");
    return -1;
  }
  // skip over argv[0] (it can contain embedded double quotes if launched from cmd.exe!)
  int argstrlen = strlen(argstr);
  int quoted = 0;
  for ( i = 0; ( i < argstrlen) && ( !IS_WHITESPACE(argstr[i]) || quoted ); i++ )
    if (argstr[i] == '"') quoted = !quoted;
  // while ( IS_WHITESPACE(argstr[i]) ) i++; // arguments leading whitespace
  argstr = &argstr[i];
  if ( strlen(cmdline) + strlen(argstr) >= MAX_CMD ) {
    fprintf(stderr, "runscript: command line string too long:\n%s%s\n", cmdline, argstr);
    return -1;
  }
  // pass through all the arguments
  strcat(cmdline, argstr);
  
#ifdef DRYRUN
  printf("progname: %s\n", progname);
  printf("dirname: %s\n", dirname);
  printf("args: %s\n", &argstr[-i]);
  for (i = 0; i < argc; i++) printf("argv[%d]: %s\n", i, argv[i]);
  printf("cmdl: %s\n", cmdline);
  return;
#endif

  // create child process
  STARTUPINFOA si; // ANSI variant
  PROCESS_INFORMATION pi;
  ZeroMemory( &si, sizeof(si) );
  si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;// | STARTF_USESHOWWINDOW;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_HIDE ; // can be used to hide console window (requires STARTF_USESHOWWINDOW flag)
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
  ZeroMemory( &pi, sizeof(pi) );
  if( !CreateProcessA(
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
  ) {
    fprintf(stderr, "runscript: cannot create process: %s\n", cmdline);
    return -1;
  }
  CloseHandle( pi.hThread ); // thread handle is not needed
  DWORD ret = 0;
  if ( WaitForSingleObject( pi.hProcess, INFINITE ) == WAIT_OBJECT_0 ) {
    if ( !GetExitCodeProcess( pi.hProcess, &ret) ) {
        fprintf(stderr, "runscript: cannot get process exit code: %s\n", cmdline);
        return -1;
    }
  } else {
    fprintf(stderr, "runscript: failed to wait for process termination: %s\n", cmdline);
    return -1;
  }
  CloseHandle( pi.hProcess );
  return ret;
}
