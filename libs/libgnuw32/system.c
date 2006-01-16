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
    - win32_system() rewritten
  */

#include <win32lib.h>
#include <errno.h>

#undef _DEBUG

#ifdef _DEBUG
#define TRACE(x) x
#else
#define TRACE(x)
#endif

extern char *quote_args(char **argv);
extern void free_cmd_pipe(char ***cmd_pipe, int nCmds);

/*
  It has been proven that system() fails to retrieve exit codes
  under Win9x. This is a workaround for this bug.
*/

int __cdecl win32_system(const char *cmd, int async)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  DWORD ret = 0, dwCode;
  HANDLE *hIn, *hOut, *hProcess, hDummyIn, hDummyOut, hDummyErr, hCurrentProcess;
  int i, j, nCmds;

  char  *app_name = NULL, *new_cmd = NULL;
  char  *red_input, *red_output, ***cmd_pipe;

  /* Reset errno ??? */
  errno = 0;

  /* Admittedly, the command interpreter will allways be found. */
  if (! cmd) {
    TRACE(fprintf(stderr, "system: (null) command.\n"));
    errno = 0;
    return 1;
  }

  cmd_pipe = parse_cmdline(cmd, &red_input, &red_output, &nCmds);

  if (nCmds == 0) {
    /* Failed to find the command or malformed cmd */
    TRACE(fprintf(stderr, "system: failed to find command.\n"));
    errno = ENOEXEC;
    return -1;
  }

  /* First time, allow to call the command interpretor */
  if (look_for_cmd(cmd_pipe[0][0], &app_name) == FALSE) {
    /* releasing previous split */
    TRACE(fprintf(stderr, "look for command: `%s' not found, concatenating comspec\n", cmd_pipe[0][0]));
    free_cmd_pipe(cmd_pipe, nCmds);
    new_cmd = concatn(getenv("COMSPEC"), " /c ", cmd, NULL);
    TRACE(fprintf(stderr, "new command is: %s\n", new_cmd));
    cmd_pipe = parse_cmdline(new_cmd, &red_input, &red_output, &nCmds);
    if (nCmds == 0 || look_for_cmd(cmd_pipe[0][0], &app_name) == FALSE) {
      /* Failed to find the command or malformed cmd */
      TRACE(fprintf(stderr, "system: failed to find command.\n"));
      errno = ENOEXEC;
      return -1;
    }
  }

  /* Arrays for pipes handles and process handles */
  hIn = malloc(nCmds * sizeof(HANDLE));
  hOut = malloc(nCmds * sizeof(HANDLE));
  hProcess = malloc(nCmds * sizeof(HANDLE));

  for (i = 0; i < nCmds; i++) {
    hIn[i] = hOut[i] = hProcess[i] = INVALID_HANDLE_VALUE;
  }

  /* We need this handle to duplicate other handles */
  hCurrentProcess = GetCurrentProcess();

  TRACE(fprintf(stderr, "%d cmds found\n", nCmds));

  /* First time, use red_input if available */

  if (red_input) {
    hIn[0] = CreateFile(red_input,
			GENERIC_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,	/* non inheritable */
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
    if (hIn[0] == INVALID_HANDLE_VALUE) {
      TRACE(fprintf(stderr, "system: failed to open hIn (%s) with error %d.\n", red_input, GetLastError()));
      errno = EIO;
      return -1;
    }

    TRACE(fprintf(stderr, "First pipe input redirected to %s\n", red_input));

  }
  else {
    hDummyIn = GetStdHandle(STD_INPUT_HANDLE);
    DuplicateHandle(hCurrentProcess, hDummyIn,
		    hCurrentProcess, &hIn[0], 0,
		    FALSE,
		    DUPLICATE_SAME_ACCESS);
    TRACE(fprintf(stderr, "STD_INPUT_HANDLE %x -> %x\n", hDummyIn, hIn[0]));
  }
  
  /* Last time, use red_output if available */
  if (red_output) {
    hOut[nCmds - 1] = CreateFile(red_output,
				 GENERIC_WRITE,
				 FILE_SHARE_READ | FILE_SHARE_WRITE,
				 NULL, /* non inheritable */
				 CREATE_ALWAYS,
				 FILE_ATTRIBUTE_NORMAL,
				 NULL);
    if (hOut[nCmds - 1] == INVALID_HANDLE_VALUE) {

      TRACE(fprintf(stderr, "system: failed to open hOut (%s) with error %d.\n", 
		    red_output, GetLastError()));

      errno = EIO;
      return -1;
    }

    TRACE(fprintf(stderr, "Last pipe output redirected to %s\n", red_output));

  }
  else {
    hDummyOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DuplicateHandle(hCurrentProcess, hDummyOut,
		    hCurrentProcess, &hOut[nCmds - 1], 0,
		    FALSE,
		    DUPLICATE_SAME_ACCESS);
    TRACE(fprintf(stderr, "STD_OUTPUT_HANDLE %x -> %x\n", hDummyOut, hOut[nCmds - 1]));
  }

    hDummyErr = GetStdHandle(STD_ERROR_HANDLE);


  /* Loop on every command */
  for (i = 0; i < nCmds; i++) {
    
    /* free the cmd and build the current one */
    if (new_cmd) {
      free(new_cmd);
      new_cmd = NULL;
    }
    if (app_name) {
      free(app_name);
      app_name = NULL;
    }

    /* Get the real app_name */
    if (look_for_cmd(cmd_pipe[i][0], &app_name) == FALSE) {
      errno = ENOEXEC;
      ret = -1;
      goto cleanup;
    }

    new_cmd = quote_args(cmd_pipe[i]);

    TRACE(fprintf(stderr, "Cmd[%d] = %s\n", i, new_cmd));

    if (i < nCmds - 1) {
      /* Handles are created non-inheritable */
      if (CreatePipe(&hIn[i+1], &hOut[i], NULL, 4096) == FALSE) {
	fprintf(stderr, "win32_system(%s) call failed to create pipe (Error %d).\n", cmd, GetLastError());
      }

      TRACE(fprintf(stderr, "Creating pipe between hOut[%d] = %x and hIn[%d] = %x\n", 
		    i, hOut[i], i+1, hIn[i+1]));

    }

    /* Enable handle inheritance for hIn[i] */
    DuplicateHandle(hCurrentProcess, hIn[i],
		    hCurrentProcess, &hDummyIn, 0,
		    TRUE,
		    DUPLICATE_SAME_ACCESS);

    TRACE(fprintf(stderr, "Duplicating hIn %x -> %x\n", hIn[i], hDummyIn));

    /* Enable handle inheritance for hOut[i] */
    DuplicateHandle(hCurrentProcess, hOut[i],
		    hCurrentProcess, &hDummyOut, 0,
		    TRUE,
		    DUPLICATE_SAME_ACCESS);

    TRACE(fprintf(stderr, "Duplicating hOut %x -> %x\n", hOut[i], hDummyOut));

    /* We don't need them anymore */
    CloseHandle(hIn[i]);
    CloseHandle(hOut[i]);
    TRACE(fprintf(stderr, "Close handles hIn = %x and hOut = %x\n", hIn[i], hOut[i]));
    
    ZeroMemory( &si, sizeof(STARTUPINFO) );
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES /* | STARTF_USESHOWWINDOW */ ;
    si.wShowWindow = 0 /* SW_SHOWDEFAULT */ ;
    si.hStdInput = hDummyIn;
    si.hStdOutput = hDummyOut;
    si.hStdError = hDummyErr;
    
    TRACE(fprintf(stderr, "Executing: %s (app_name = %s)\n", new_cmd, app_name));

    dwCode = CreateProcess(app_name,
			   new_cmd,
			   NULL,
			   NULL,
			   TRUE,
			   0,
			   NULL,
			   NULL,
			   &si,
			   &pi);

    TRACE(fprintf(stderr, "New cmd(%s), process handle %x, Id %d, ret (%d), error (%d).\n", 
		  new_cmd, pi.hProcess, pi.dwProcessId, dwCode,
		  (dwCode ? 0 : GetLastError() )));

    CloseHandle(hDummyIn);
    CloseHandle(hDummyOut);

    if (dwCode != 0) {
      TRACE(fprintf(stderr, "StdIn = %x(%x), StdOut = %x(%x)\n", 
		    hIn[i], hDummyIn, hOut[i], hDummyOut));
    
      /* Only the process handle is needed */
      CloseHandle(pi.hThread);
      hProcess[i] = pi.hProcess;
    }
    else {
      fprintf(stderr, "win32_system(%s) call failed to create process (Error %d, hProcess %d).\n", new_cmd, GetLastError(), pi.hProcess);
      ret = -1;
      goto cleanup;
    }
  }

 cleanup:
  
  for (i = 0; i < nCmds; i++) {
    if (hProcess[i] != INVALID_HANDLE_VALUE) {
      if (ret == -1) {
	TerminateProcess(hProcess[i], -1);
      }
      else if (async == 0) {
	DWORD dwRet = WaitForSingleObject(hProcess[i], INFINITE);
	if (dwRet == WAIT_OBJECT_0) {
	  
	  TRACE(fprintf(stderr, "Process %d exited\n", i));
	  
	}
	else {
	  fprintf(stderr, "Failed to wait for process termination: %s (Ret %d, Error %d)\n", 
		  cmd, dwRet, GetLastError());
	  ret = -1;
	}
      }
    }    
  }
    
  if (async == 0 && hProcess[nCmds - 1] != INVALID_HANDLE_VALUE) {
    if (GetExitCodeProcess(hProcess[nCmds - 1], &ret) == 0) {
      fprintf(stderr, "Failed to retrieve exit code: %s (Error %d)\n", cmd, GetLastError());
      ret = -1;
    }

    TRACE(fprintf(stderr, "All processes terminated, exit code = %d\n", ret));
    
  }

  for(i = 0; i < nCmds; i++) {
    if (hProcess[i] != INVALID_HANDLE_VALUE) 
      CloseHandle(hProcess[i]);
  }
  free_cmd_pipe(cmd_pipe, nCmds);

  if (new_cmd) free(new_cmd);
  if (app_name) free(app_name);
    
  return ret;
}
