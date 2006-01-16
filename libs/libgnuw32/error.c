/* 

Error messages for fpTeX.

Copyright (C) 1998-2000 Free Software Foundation, Inc.

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

#include <win32lib.h>

#ifdef MAKE_KPSE_DLL
#include <kpathsea/progname.h>

extern KPSEDLL char *kpathsea_version_string; /* from kpathsea/version.c */
#endif

/*
  Various messages to display error messages, either from Windows
  or on the console.
*/

/* Last error message in a MessageBox. */
void Win32Error(char *caller)
{
  LPVOID lpMsgBuf;
  DWORD errCode;
  if (errCode = GetLastError()) {
    if (FormatMessage(
		      FORMAT_MESSAGE_ALLOCATE_BUFFER 
		      | FORMAT_MESSAGE_FROM_SYSTEM
		      | FORMAT_MESSAGE_IGNORE_INSERTS,
		      NULL,
		      errCode,
		      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
		      (LPTSTR) &lpMsgBuf,
		      0,
		      NULL 
		      )) {
      /* Display the string. */
      MessageBox( NULL, lpMsgBuf, caller, MB_OK|MB_ICONINFORMATION );
      /* Free the buffer. */
      LocalFree( lpMsgBuf );
    }
    else {
      char szBuf[24];
      wsprintf(szBuf, "%d", errCode);
      MessageBox( NULL, szBuf, caller, MB_OK|MB_ICONINFORMATION );
    }    
  }
}

void Win32Msg(char *msg)
{
#ifdef MAKE_KPSE_DLL  
  char *program = "(unknown)";
  char *caller = NULL;

  if (kpse_program_name && *kpse_program_name) {
    program = kpse_program_name;
  }

  caller = concat3(program, "\r\n", kpathsea_version_string);
#else
  char *caller = xmalloc(4096);

  if (caller && GetModuleFileName(NULL, caller, sizeof(caller)) == 0) {
    strcpy(caller, "(unknown)");
  }
#endif

  MessageBox(NULL, msg, caller, MB_OK|MB_ICONINFORMATION);
  
  free(caller);
}

