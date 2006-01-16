/* libc replacement functions for win32.

Copyright (C) 1998, 99 Free Software Foundation, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
  This does make sense only under WIN32.
  Functions:
    - popen() rewritten
    - pclose() rewritten

  FIXME : 
    - exit code ?
    - check leaks.
*/

#include <win32lib.h>
#include <errno.h>

extern void free_cmd_pipe(char ***cmd_pipe, int nCmds);
extern char *quote_args(char **argv);

#undef _DEBUG

#ifdef _DEBUG
#define TRACE(x) x
#else
#define TRACE(x)
#endif

#define IS_DIRECTORY_SEP(c) IS_DIR_SEP(c)

struct _popen_elt {
  FILE *f;			/* File stream returned */
  HANDLE hp;			/* Handle of associated process */
  struct _popen_elt *next;	/* Next list element */
};

static struct _popen_elt _z = { NULL, 0, &_z };
static struct _popen_elt *_popen_list = &_z;

static BOOL bCriticalSectionInitialized = FALSE;
CRITICAL_SECTION thePopenCriticalSection;

void __cdecl
popen_cleanup(void)
{
  if (bCriticalSectionInitialized) {
    DeleteCriticalSection(&thePopenCriticalSection);
  }
}

FILE * __cdecl win32_popen (const char *cmd, const char *mode)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  FILE *f = NULL;
  int fno, i;
  HANDLE child_in, child_out;
  HANDLE father_in, father_out;
  HANDLE father_in_dup, father_out_dup;
  HANDLE current_pid;
  int binary_mode;
  char *new_cmd = NULL, *app_name = NULL;
  struct _popen_elt *new_process;
  BOOL read_mode, write_mode;
  char ***cmd_pipe;
  int nCmds;

  /* Admittedly, the command interpreter will allways be found. */
  if (cmd == NULL) {
    TRACE(fprintf(stderr, "popen: (null) command.\n"));
    return NULL;
  }


  cmd_pipe = parse_cmdline(cmd, NULL, NULL, &nCmds);

  /* Allow only one command in the pipe */
  if (nCmds != 1) {
    /* Failed to find the command or malformed cmd */
    TRACE(fprintf(stderr, "system: failed to find command.\n"));
    return NULL;
  }

  /* First time, allow to call the command interpretor */
  if (look_for_cmd(cmd_pipe[0][0], &app_name) == FALSE) {
    /* releasing previous split */
    free_cmd_pipe(cmd_pipe, nCmds);
    TRACE(fprintf(stderr, "look for command: %s not found, concatenating comspec\n", app_name));
    new_cmd = concatn(getenv("COMSPEC"), " /c ", cmd, NULL);
    TRACE(fprintf(stderr, "new command is: %s\n", new_cmd));
    cmd_pipe = parse_cmdline(new_cmd, NULL, NULL, &nCmds);
    /* We don't need it anymore */
    free(new_cmd);
    if (look_for_cmd(cmd_pipe[0][0], &app_name) == FALSE) {
      /* Failed to find the command or malformed cmd */
      TRACE(fprintf(stderr, "system: failed to find command.\n"));
      return NULL;
    }
  }

  new_cmd = quote_args(cmd_pipe[0]);

  TRACE(fprintf(stderr, "popen: app_name = %s\n", app_name));
  TRACE(fprintf(stderr, "popen: cmd_line = %s\n", new_cmd));

  current_pid = GetCurrentProcess();

  ZeroMemory( &si, sizeof(STARTUPINFO) );
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW ;
  si.wShowWindow = /* 0 */ SW_HIDE ;
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

  binary_mode = (strchr(mode, 'b') ? _O_BINARY : _O_TEXT);

  read_mode = (strchr(mode, 'r') != 0);
  write_mode = (strchr(mode, 'w') != 0);

  /* Opening the pipe for writing */
  if (write_mode) {
    binary_mode |= _O_WRONLY;
    if (CreatePipe(&child_in, &father_out, NULL, 0) == FALSE) {
      fprintf(stderr, "popen: error CreatePipe\n");
      return NULL;
    }

    if (DuplicateHandle(current_pid, child_in, 
			current_pid, &father_in_dup, 
			0, TRUE, DUPLICATE_SAME_ACCESS) == FALSE) {
      fprintf(stderr, "popen: error DuplicateHandle father_out\n");
      return NULL;
    }
    si.hStdInput = father_in_dup;

    CloseHandle(child_in);
    fno = _open_osfhandle((long)father_out, binary_mode);
    f = _fdopen(fno, mode);
    i = setvbuf( f, NULL, _IONBF, 0 );
  }
  /* Opening the pipe for reading */
  else if (read_mode) {
    binary_mode |= _O_RDONLY;
    if (CreatePipe(&father_in, &child_out, NULL, 0) == FALSE) {
      fprintf(stderr, "popen: error CreatePipe\n");
      return NULL;
    }
    if (DuplicateHandle(current_pid, child_out, 
			current_pid, &father_out_dup, 
			0, TRUE, DUPLICATE_SAME_ACCESS) == FALSE) {
      fprintf(stderr, "popen: error DuplicateHandle father_in\n");
      return NULL;
    }
    CloseHandle(child_out);
    si.hStdOutput = father_out_dup;
    fno = _open_osfhandle((long)father_in, binary_mode);
    f = _fdopen(fno, mode);
    i = setvbuf( f, NULL, _IONBF, 0 );
  }
  else {
    fprintf(stderr, "popen: invalid mode %s\n", mode);
    return NULL;
  }

  /* creating child process */
  if (CreateProcess(app_name,	/* pointer to name of executable module */
		    new_cmd,	/* pointer to command line string */
		    NULL,	/* pointer to process security attributes */
		    NULL,	/* pointer to thread security attributes */
		    TRUE,	/* handle inheritance flag */
		    0,		/* creation flags */
		    NULL,	/* pointer to environment */
		    NULL,	/* pointer to current directory */
		    &si,	/* pointer to STARTUPINFO */
		    &pi		/* pointer to PROCESS_INFORMATION */
		  ) == FALSE) {
    fprintf(stderr, "popen: CreateProcess %x\n", GetLastError());
    return NULL;
  }
  
  /* Only the process handle is needed, ignore errors */
  CloseHandle(pi.hThread);

  /* Closing the unnecessary part of the pipe */
  if (read_mode) {
    CloseHandle(father_out_dup);
  }
  else if (write_mode) {
    CloseHandle(father_in_dup);
  }

  /* Add the pair (f, pi.hProcess) to the list */
  if ((new_process = malloc(sizeof(struct _popen_elt))) == NULL) {
    fprintf (stderr, "popen: malloc(new_process) error\n");
    return NULL;
  }

  if (! bCriticalSectionInitialized) {
    InitializeCriticalSection(&thePopenCriticalSection);
    atexit(popen_cleanup);
    bCriticalSectionInitialized = TRUE;
  }

  EnterCriticalSection(&thePopenCriticalSection);

  /* Saving the FILE * pointer, access key for retrieving the process
     handle later on */
  new_process->f = f;
  /* Saving the process handle */
  new_process->hp = pi.hProcess;
  /* Linking it to the list of popen() processes */
  new_process->next = _popen_list;
  _popen_list = new_process;

  LeaveCriticalSection(&thePopenCriticalSection);

  free_cmd_pipe(cmd_pipe, nCmds);

  if (new_cmd) free(new_cmd);
  if (app_name) free(app_name);

  return f;
}

int __cdecl win32_pclose (FILE *f)
{
  HANDLE h;
  struct _popen_elt *p, *q;
  int exit_code;

  EnterCriticalSection(&thePopenCriticalSection);

  /* Look for f as the access key in the linked list */
  for (q = NULL, p = _popen_list; 
       p != &_z && p->f != f; 
       q = p, p = p->next);

  /* remove the elt from the list if found */
  if (p != &_z) {
    if (q != NULL)
      q->next = p->next;
    else
      _popen_list = p->next;
    /* remember the handle */
    h = p->hp;
    /* free up the block */
    free(p);
  }

  LeaveCriticalSection(&thePopenCriticalSection);

  if (p == &_z) {
    fprintf(stderr, "pclose: error, file not found.");
    return -1;
  }

  /* Closing the FILE pointer */
  fclose(f);

  /* Waiting for the process to terminate */
  if (WaitForSingleObject(h, INFINITE) != WAIT_OBJECT_0) {
    fprintf(stderr, "pclose: error, process still active\n");
    return -1;
  }

  /* retrieving the exit code */
  if (GetExitCodeProcess(h, &exit_code) == 0) {
    fprintf(stderr, "pclose: can't get process exit code\n");
    return -1;
  }

  /* Closing the process handle, this will cause the system to
     remove the process from memory */
  if (CloseHandle(h) == FALSE) {
    fprintf(stderr, "pclose: error closing process handle\n");
    return -1;
  }

  return exit_code;
}
