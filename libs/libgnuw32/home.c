/* Utility and Unix shadow routines for XEmacs on Windows NT.
   Copyright (C) 1994, 1995 Free Software Foundation, Inc.

This file is part of XEmacs.

XEmacs is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

XEmacs is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with XEmacs; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.


   Geoff Voelker (voelker@cs.washington.edu) 7-29-94 */

/* Adapted for XEmacs by David Hobley <david@spook-le0.cia.com.au> */
/* Sync'ed with Emacs 19.34.6 by Marc Paquette <marcpa@cam.org> */
/* Adapted to fpTeX 0.4 by Fabrice Popineau <Fabrice.Popineau@supelec.fr> */

#include <win32lib.h>
#include <shlobj.h>

char initial_directory[MAXPATHLEN];

static char *cached_home_directory;

void
uncache_home_directory (void)
{
  cached_home_directory = NULL;	/* in some cases, this may cause the leaking
								   of a few bytes */
}

int output_home_warning = 0;

void
set_home_warning() {
  output_home_warning = 1;
}

/* Returns the home directory, in external format */
char *
get_home_directory (void)
{

  if (cached_home_directory != NULL)
	goto done;

  if ((cached_home_directory = getenv("HOME")) != NULL) {
	char q[MAXPATHLEN];
	/* In case it is %HOMEDRIVE%%HOMEPATH% */
	if (ExpandEnvironmentStrings(cached_home_directory, q, sizeof(q)) == 0) {
	  /* Error */
	  cached_home_directory = NULL;
	}
	else {
	  cached_home_directory = xstrdup(q);
	  goto done;
	}
  }
  
  {
	char	*homedrive, *homepath;
	if ((homedrive = getenv("HOMEDRIVE")) != NULL &&
		(homepath = getenv("HOMEPATH")) != NULL) {
	  cached_home_directory = concat(homedrive, homepath);
	  goto done;
	}
  }
  
  /* This method is the prefered one because even if it requires a more recent shell32.dll,
     it does not need to call SHMalloc()->Free() */
  {
	/* This will probably give the wrong value */
	char q [MAXPATHLEN];
	HINSTANCE h;
	HRESULT (WINAPI * p1)(HWND, LPSTR, int, BOOL);
	HWND (WINAPI * p2)(void);
	HWND hwnd = NULL;

	if (h = LoadLibrary("user32.dll")) {
	  if (p2 = GetProcAddress(h, "GetDesktopWindow"))
	    hwnd = (*p2)();
	  FreeLibrary(h);
	}
	
	if (hwnd && (h = LoadLibrary("shell32.dll")) != NULL) {
	  if (p1 = GetProcAddress(h, "SHGetSpecialFolderPathA"))
	    if ((*p1)(hwnd, q, CSIDL_PERSONAL, TRUE)) {
	      cached_home_directory = xstrdup(q);
	    }
	  FreeLibrary(h);
	}
	if (cached_home_directory) goto done;
  }

  if (output_home_warning) {
	fprintf(stderr, "fpTeX has been unable to determine a good value for the user's $HOME\n"
			"	directory, and will be using the value:\n"
			"		%s\n"
			"	This is probably incorrect.\n",
			cached_home_directory
			);
  }
 done:
  return cached_home_directory;
}
