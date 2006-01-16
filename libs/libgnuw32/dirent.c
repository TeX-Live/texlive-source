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

#include <sys/dir.h>
#include <win32lib.h>

/* Emulate the Unix directory procedures opendir, closedir, 
   and readdir.  We can't use the procedures supplied in sysdep.c,
   so we provide them here.  */

struct direct dir_static;       /* simulated directory contents */
HANDLE dir_find_handle = INVALID_HANDLE_VALUE;
static int    dir_is_fat;
char   dir_pathname[MAXPATHLEN+1];
WIN32_FIND_DATA dir_find_data;

DIR *
opendir (const char *filename)
{
  DIR *dirp;
  
  /* Opening is done by FindFirstFile.  However, a read is inherent to
     this operation, so we defer the open until read time.  */
  
  if (!(dirp = (DIR *)xmalloc(sizeof(DIR))))
    return NULL;
  ZeroMemory(dirp, sizeof(DIR));
  if (dir_find_handle != INVALID_HANDLE_VALUE)
    return NULL;

  dirp->dd_fd = 0;
  dirp->dd_loc = 0;
  dirp->dd_size = 0;

  strncpy (dir_pathname, map_win32_filename (filename, NULL), MAXPATHLEN);
  dir_pathname[MAXPATHLEN] = '\0';
  dir_is_fat = is_fat_volume (filename, NULL);

  return dirp;
}

void
closedir (DIR *dirp)
{
  /* If we have a find-handle open, close it.  */
  if (dir_find_handle != INVALID_HANDLE_VALUE)
    {
      FindClose (dir_find_handle);
      dir_find_handle = INVALID_HANDLE_VALUE;
    }
  free (dirp);
}

struct direct *
readdir (DIR *dirp)
{
  /* If we aren't dir_finding, do a find-first, otherwise do a find-next. */
  if (dir_find_handle == INVALID_HANDLE_VALUE)
    {
      char filename[MAXNAMLEN + 3];
      int ln;

      strcpy (filename, dir_pathname);
      ln = strlen (filename) - 1;
      if (!IS_DIR_SEP (filename[ln]))
		strcat (filename, "\\");
      strcat (filename, "*");

      dir_find_handle = FindFirstFile (filename, &dir_find_data);

      if (dir_find_handle == INVALID_HANDLE_VALUE)
		return NULL;
    }
  else
    {
      if (!FindNextFile (dir_find_handle, &dir_find_data))
		return NULL;
    }
  
  /* Emacs never uses this value, so don't bother making it match
     value returned by stat().  */
  dir_static.d_ino = 1;
  
  dir_static.d_reclen = sizeof (struct direct) - MAXNAMLEN + 3 +
    dir_static.d_namlen - dir_static.d_namlen % 4;
  
  dir_static.d_namlen = strlen (dir_find_data.cFileName);
  strcpy (dir_static.d_name, dir_find_data.cFileName);
  if (dir_is_fat)
    _strlwr (dir_static.d_name);
  else 
#if 0
    if (!NILP (Vmswindows_downcase_file_names))
#endif
	  {
		char *p;
		for (p = dir_static.d_name; *p; p++)
		  if (*p >= 'a' && *p <= 'z')
			break;
		if (!*p)
		  _strlwr (dir_static.d_name);
	  }
  
  return &dir_static;
}
