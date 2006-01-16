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

/*
   MSVC stat function can't cope with UNC names and has other bugs, so
   replace it with our own.  This also allows us to calculate consistent
   inode values without hacks in the main Emacs code.
*/

#include <win32lib.h>
#include <errno.h>
#if 0
#include <sys/dir.h>
#include <sys/stat.h>
#include <pwd.h>
#endif

extern struct passwd the_passwd;

extern struct direct dir_static;       /* simulated directory contents */
extern HANDLE dir_find_handle;
extern char   dir_pathname[MAXPATHLEN+1];
extern WIN32_FIND_DATA dir_find_data;

extern FILETIME utc_base_ft;
extern volume_info_data volume_info;

int __cdecl
win32_stat (const char * path, struct stat * buf)
{
  char * name;
  WIN32_FIND_DATA wfd;
  HANDLE fh;
  DWORD fake_inode;
  int permission;
  int len;
  int rootdir = FALSE;

  if (path == NULL || buf == NULL)
    {
      errno = EFAULT;
      return -1;
    }

  name = (char *) map_win32_filename (path, &path);
  /* must be valid filename, no wild cards */
  if (strchr (name, '*') || strchr (name, '?'))
    {
      errno = ENOENT;
      return -1;
    }

  /* Remove trailing directory separator, unless name is the root
     directory of a drive or UNC volume in which case ensure there
     is a trailing separator. */
  len = strlen (name);
  rootdir = (path >= name + len - 1
	     && (IS_DIR_SEP (*path) || *path == 0));
  name = strcpy (alloca (len + 2), name);

  if (rootdir)
    {
      if (!IS_DIR_SEP (name[len-1]))
	strcat (name, "\\");
      if (GetDriveType (name) < 2)
	{
	  errno = ENOENT;
	  return -1;
	}
      memset (&wfd, 0, sizeof (wfd));
      wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
      wfd.ftCreationTime = utc_base_ft;
      wfd.ftLastAccessTime = utc_base_ft;
      wfd.ftLastWriteTime = utc_base_ft;
      strcpy (wfd.cFileName, name);
    }
  else
    {
      if (IS_DIR_SEP (name[len-1]))
	name[len - 1] = 0;

#if 0
      /* (This is hacky, but helps when doing file completions on
	 network drives.)  Optimize by using information available from
	 active readdir if possible.  */
      if (dir_find_handle != INVALID_HANDLE_VALUE &&
	  (len = strlen (dir_pathname)),
	  strnicmp (name, dir_pathname, len) == 0 &&
	  IS_DIR_SEP (name[len]) &&
	  stricmp (name + len + 1, dir_static.d_name) == 0)
	{
	  /* This was the last entry returned by readdir.  */
	  wfd = dir_find_data;
	}
      else
#endif
	{
	  fh = FindFirstFile (name, &wfd);
	  if (fh == INVALID_HANDLE_VALUE)
	    {
	      errno = ENOENT;
	      return -1;
	    }
	  FindClose (fh);
	}
    }

  if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
#ifdef TRACE_STAT
      fprintf(stderr, "stat found %s is a directory\n", name );
#endif
      buf->st_mode = _S_IFDIR;
      buf->st_nlink = 2;	/* doesn't really matter */
      fake_inode = 0;		/* this doesn't either I think */
    }
  else 
#if 0
    if (!NILP (Vmswindows_get_true_file_attributes))
    {
      /* This is more accurate in terms of getting the correct number
	 of links, but is quite slow (it is noticeable when Emacs is
	 making a list of file name completions). */
      BY_HANDLE_FILE_INFORMATION info;

      /* No access rights required to get info.  */
      fh = CreateFile (name, 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
		       OPEN_EXISTING, 0, NULL);

      if (GetFileInformationByHandle (fh, &info))
	{
	  switch (GetFileType (fh))
	    {
	    case FILE_TYPE_DISK:
	      buf->st_mode = _S_IFREG;
	      break;
	    case FILE_TYPE_PIPE:
	      buf->st_mode = _S_IFIFO;
	      break;
	    case FILE_TYPE_CHAR:
	    case FILE_TYPE_UNKNOWN:
	    default:
	      buf->st_mode = _S_IFCHR;
	    }
	  buf->st_nlink = (short) info.nNumberOfLinks;
	  /* Might as well use file index to fake inode values, but this
	     is not guaranteed to be unique unless we keep a handle open
	     all the time (even then there are situations where it is
	     not unique).  Reputedly, there are at most 48 bits of info
	     (on NTFS, presumably less on FAT). */
	  fake_inode = info.nFileIndexLow ^ info.nFileIndexHigh;
	  CloseHandle (fh);
	}
      else
	{
	  errno = EACCES;
	  return -1;
	}
    }
  else
#endif /* 0 */
    {
      /* Don't bother to make this information more accurate.  */
      buf->st_mode = _S_IFREG;
      buf->st_nlink = 1;
      fake_inode = 0;
    }

#if 0
  /* Not sure if there is any point in this.  */
  if (!NILP (Vwin32_generate_fake_inodes))
    fake_inode = generate_inode_val (name);
  else if (fake_inode == 0)
    {
      /* For want of something better, try to make everything unique.  */
      static DWORD gen_num = 0;
      fake_inode = ++gen_num;
    }
#endif

  /* #### MSVC defines _ino_t to be short; other libc's might not.  */
  buf->st_ino = (unsigned short) (fake_inode ^ (fake_inode >> 16));

  /* consider files to belong to current user */
  buf->st_uid = the_passwd.pw_uid;
  buf->st_gid = the_passwd.pw_gid;

  /* volume_info is set indirectly by map_win32_filename */
  buf->st_dev = volume_info.serialnum;
  buf->st_rdev = volume_info.serialnum;


  buf->st_size = wfd.nFileSizeLow;

  /* Convert timestamps to Unix format. */
  buf->st_mtime = convert_time (wfd.ftLastWriteTime);
  buf->st_atime = convert_time (wfd.ftLastAccessTime);
  if (buf->st_atime == 0) buf->st_atime = buf->st_mtime;
  buf->st_ctime = convert_time (wfd.ftCreationTime);
  if (buf->st_ctime == 0) buf->st_ctime = buf->st_mtime;

  /* determine rwx permissions */
  if (wfd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    permission = _S_IREAD;
  else
    permission = _S_IREAD | _S_IWRITE;
  
  if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    permission |= _S_IEXEC;
  else
    {
      char * p = strrchr (name, '.');
      if (p != NULL &&
	  (stricmp (p, ".exe") == 0 ||
	   stricmp (p, ".com") == 0 ||
	   stricmp (p, ".bat") == 0 ||
	   stricmp (p, ".cmd") == 0))
	permission |= _S_IEXEC;
    }

  buf->st_mode |= permission | (permission >> 3) | (permission >> 6);

  return 0;
}
