/* stackenv.c: routines that help to deal with files and directories.

Copyright (C) 1997 Fabrice POPINEAU.

Adapted to DJGPP by Eli Zaretskii.

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

#include "fileutils.h"

/* A few utility functions shared by both mktex and makempx.  */

int mt_append_mask = 0;

/* Execute a command, much like system() but using popen() */
int 
execute_command(string cmd)
{
  int retcode = 0;
  FILE *fcmd = NULL;
  if ((fcmd = popen(cmd, "r")) == NULL)
    return -1;
  retcode = pclose(fcmd);
  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "executing %s => %d\n", cmd, retcode);
  }
  return retcode;
}

/* Copy a file, appending if required.  */

int catfile (const char *from, const char *to, int append)
{
  FILE *finp, *fout;
  unsigned char buf[16*1024];
  int rbytes;

  finp = fopen (from, "rb");
  if (!finp)
    return FALSE;
  fout = fopen (to, append ? "ab" : "wb");
  if (!fout) {
    fclose (finp);
    return FALSE;
  }

  while ((rbytes = fread (buf, 1, sizeof buf, finp)) > 0) {
    int wbytes = fwrite (buf, 1, rbytes, fout);

    if (wbytes < rbytes) {
      if (wbytes >= 0)
	fprintf (stderr, "%s: disk full\n", to);
      fclose (finp);
      fclose (fout);
      unlink (to);
      return FALSE;
    }
  }

  fclose (finp);
  fclose (fout);
  return TRUE;
}

/* Move a file using rename if possible, copy if necessary.  */

int mvfile (const char *from, const char *to)
{
  int e = errno;
  int retval = TRUE;
  char errstring[PATH_MAX*4];

  errno = 0;
  if (rename (from, to) == 0) {
    errno = e;
    return TRUE;
  }

#ifndef DOSISH
  /* I don't want to rely on this on DOSISH systems.  First, not all
     of them know about EXDEV.  And second, DOS will fail `rename'
     with EACCES when the target is more than 8 directory levels deep.  */
  if (errno != EXDEV)
    retval = FALSE;
  else
#endif

  /* `rename' failed on cross-filesystem move.  Copy, then delete.  */
  if (catfile (from, to, 0) == FALSE)
    retval = FALSE;
  else
    unlink (from);	/* ignore possible errors */

  /* Simulate error message from `mv'.  */
  if (retval == FALSE) {
    sprintf(errstring, "%s: can't move %s to %s", kpse_program_name, from, to);
    perror(errstring);
  }

  if (!errno)
    errno = e;
  return retval;
}

/* Like the test function from Unix */

boolean test_file(int c, string path)
{
  struct stat stat_buf;
  boolean file_exists;

  if (c == 'z')
    return (path == NULL || *path == '\0');

  if (c == 'n')
    return (path != NULL && *path != '\0');
  
#ifdef WIN32
  /* Bug under W95 */
  if (c == 'd')
    return dir_p(path);
#endif

  file_exists = (stat(path, &stat_buf) != -1);

#ifdef DOSISH
  /* When they say "test -x foo", make so we find "foo.exe" etc.  */
  if (c == 'x' && !file_exists)
    {
      static string try_ext[] = {
	".exe",
	".bat",
	".cmd",
	".btm",
	0
      };
      string *p = try_ext;

      while (p)
	{
	  string try = concat(path, *p++);
	  if (stat(try, &stat_buf) != -1)
	    {
	      file_exists = 1;
	      free(try);
	      break;
	    }
	  free(try);
	}
    }
#endif

  if (!file_exists) return FALSE;
  switch (c) {
  case 'd':
    return ((stat_buf.st_mode & S_IFMT) == S_IFDIR);
  case 'e':
  case 'r':
    return TRUE;
  case 'f':
    return ((stat_buf.st_mode & S_IFMT) == S_IFREG);
  case 's':
    return (stat_buf.st_size > 0);
  case 'x':
    return ((stat_buf.st_mode & S_IFMT) == S_IFREG
	    && (stat_buf.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)));
  case 'w':
    {
      return (
#ifdef DOSISH
	      /* DOSISH systems allow to create files in read-only
		 directories, and `is_writable' knows about that.  So we
		 artificially look at the write bit returned by `stat'
		 and refuse to write in  read-only directories.  */
	      (stat_buf.st_mode & S_IWUSR) != 0 &&
#endif
	      is_writable(path));
    }
  default:
    return FALSE;
  }
}

#ifdef _WIN32

boolean is_writable(string path)
{
  DWORD dw;
  char tmppath[PATH_MAX*2];
  int writable;

  /* Test 2 cases: directory or file */
  if ((dw = GetFileAttributes(path)) == 0xFFFFFFFF) {
    /* Invalid Path, not writable */
    writable = false;
    errno = ENOENT;	/* like `access' does */
  }
  else if (dw & FILE_ATTRIBUTE_READONLY) {
    writable = false;
  }
  else if (dw & FILE_ATTRIBUTE_DIRECTORY) {
    /* Trying to open a file */
    if (GetTempFileName(path, "foo", 0, tmppath) == 0) {
      writable = false;
    }
    else {
      writable = true;
    }
    DeleteFile(tmppath);
  }
  else {
    writable = true;
  }
  return writable;
}

#else  /* !_WIN32 */

boolean is_writable(string path)
{
  if (!path || !*path)	/* like `access' called with empty variable as arg */
    return false;
#ifdef MSDOS
  /* Directories always return as writable, even on read-only filesystems
     such as CD-ROMs or write-protected floppies.  Need to try harder...  */
  if (access(path, D_OK) == 0) {
    /* Use a unique file name.  */
    string template = concat3(path, "/", "wrXXXXXX");
    int fd;

    if (mktemp(template) && (fd = creat(template, S_IRUSR | S_IWUSR)) >= 0) {
      close(fd);
      unlink(template);
      free(template);
      return true;
    }

    free(template);

    /* Simulate errno from `access'.  */
#ifdef EROFS
    errno = EROFS;	/* POSIX platforms should have this */
#else
    errno = EACCES;
#endif
    return false;
  }
  else
#endif /* MSDOS */
  return access(path, W_OK) == 0 ? true : false;
}

#endif /* !_WIN32 */

/*
  Recursive walk through the directory tree. Depth-first order. 
*/
void recurse_dir(string path, execfn before, execfn after)
{
    /* In depth traversal of the subdir tree */
#if defined(_WIN32)
  WIN32_FIND_DATA find_file_data;
  HANDLE hnd;
#else
  DIR *dp;
  struct dirent *ep;
#endif
  struct stat stat_buf;	/* We have to have one local because
			   of after */

  int path_len = strlen(path);

  /* current node */
  if (stat(path, &stat_buf))
    perror(path);

  /* execute before for the current node */
  if (before)
    (*before)(path, &stat_buf);

  /* if it is a directory, recurse through all sons */
  if ((stat_buf.st_mode & S_IFMT) == S_IFDIR) {
#if defined(_WIN32)
    strcat(path, "/*");
    hnd = FindFirstFile(path, &find_file_data);
    while (hnd != INVALID_HANDLE_VALUE && 
	   FindNextFile(hnd, &find_file_data) != FALSE) { 
      if(!strcmp(find_file_data.cFileName, ".")
	 || !strcmp(find_file_data.cFileName, "..")) 
	continue;
      path[path_len+1] = '\0';
      strcat(path, find_file_data.cFileName);
      recurse_dir(path, before, after);
    }
    path[path_len] = '\0';
    FindClose(hnd);
#else
    if ((dp = opendir(path))) {
      while ((ep = readdir(dp)))
	if (strcmp(ep->d_name, ".") &&
	    strcmp(ep->d_name, "..")) {
	  path[path_len] = '/';
	  strcpy(path+path_len+1, ep->d_name);
	  recurse_dir(path, before, after);
	}
      path[path_len] = '\0';
      closedir(dp);
    }
    else
      perror(path);
#endif
  }
  /* execute after for the current node */
  if (after)
    (*after)(path, &stat_buf);
}

void do_rmdir(string path)
{
  static char name[PATH_MAX];
  strcpy(name,path);

  if (test_file('d', path)) {
    recurse_dir(name, NULL, remove_path);
  }
  else
    unlink(path);
}

/* Recursively destructs files & directories from path. */
void rec_rmdir(string path)
{
  string p;

  for(p=path+strlen(path)-1; IS_DIR_SEP(*p) && (p > path) ; p--) {
    *p = '\0';
  }
  do_rmdir(path);
}

/* Creates path, and all intermediate directories if necessary. */

int do_makedir(string path)
{
  struct stat stat_buf;
  string p = path;
  mode_t dir_perms = 0777 & ~umask (0);	/* default permissions for `mkdir' */
  mode_t parent_perms = 0;

  /* We can't authorize multiple slashes here */
  normalize(path);

  if (KPSE_DEBUG_P(MKTEX_DEBUG)) {
    fprintf(stderr, "Creating directory: %s\n", path);
  }

  if (stat(path, &stat_buf) == 0 &&
      (stat_buf.st_mode & S_IFMT) == S_IFDIR)
    return 0;			/* The path already exists */

#ifdef WIN32
  /* Handling of UNC names */
  if (IS_UNC_NAME(path)) {
    /* UNC pathname : \\server\dir\...
       We must begin at the third component.
       Name is normalized, so only '/' as dir sep.
       */
    p = strchr(path+3, '/');
    if (!p) {
      /* invalid pathname */
      return ENOTDIR;
    }
    p = strchr(p+1, '/');
    if (!p) {
      /* invalid pathname */
      return ENOTDIR;
    }
    p++;
  }
  else {
    if (NAME_BEGINS_WITH_DEVICE(path)) {
      p = path+2;
    }
    if (IS_DIR_SEP(*p))
      p++;
  }
#else
  /* Finding the [drive letter][:] */
  if (IS_DEVICE_SEP(*(p+1)))
    p +=2;

  if (IS_DIR_SEP(*p))
    p++;
#endif
  for ( ; *p ; p++) {
    if (IS_DIR_SEP(*p)) {
      *p = '\0';
      errno = 0;
      if (!test_file('d', path)) {
          if ((mkdir(path, dir_perms) == -1)) {
              return errno;
          } else if (parent_perms == 0 && errno == 0) {
              /* The first time we actually create a directory, remember the
                 permission bits of its parent.  */
              string parent = concat(path, "/..");
              
              if (stat(parent, &stat_buf) == 0) {
                  parent_perms = stat_buf.st_mode;
                  /* Use the correct bits from now on.  */
                  dir_perms = parent_perms | mt_append_mask;
              }
              free(parent);
          }
	  /* chmod the directory we just created.  */
	  chmod(path, dir_perms);
      }
      *p = DIR_SEP;
    }
  }
  /* and for the whole path */
  if (mkdir(path, dir_perms) == -1) {
    return errno;
  }
  chmod(path, dir_perms);

  return 0;
}

/* remove something which already exists */
void remove_path(string path, struct stat* st)
{
  int e = errno;

  if ((st->st_mode & S_IFMT) == S_IFDIR) {
    errno = 0;
    if (rmdir(path) == -1 && errno == EBUSY) {
      /* `rmdir' will fail (at least on MS-DOS) if PATH is the current
	 working directory on another drive, or the parent of the
	 working directory.  To work around that, we chdir to the root
	 of that drive, then try to rmdir again.  */
      char drv_root[4], *p = drv_root;

      if (path[0] && path[1] == ':') {
	*p++ = path[0];
	*p++ = path[1];
      }
      *p++ = '/';
      *p = '\0';
      pushd(drv_root);
      if (rmdir(path) == 0) {
	errno = e;
	popd();
	return;
      }
    }
    else if (errno)
      perror(path);
    else
      errno = e;
  }
  else {
    if (unlink(path) == -1)
      perror(path);
  }
}

/* 
  Rewrite path with the same DIR_SEP and deleting multiple ones
  FIXME: does this eliminates the trailing ones ? W95 doesn't grok 
  stat() on pathes that end up in PATH_SEP. 
  */
char *normalize(string path)
{
  string p, q;
  int offset = 0;

  /* Be safe */
  if (path == NULL) return path;

  /* Delete any multiple slashes, except leading ones on WIN32 (UNC names) */
#ifdef WIN32
  if (IS_UNC_NAME(path)) {
    path[0] = path[1] = DIR_SEP;
    offset = 2;
  }
#endif
  for ( p = q = path+offset; *p != '\0'; p++, q++) {
    *q = (IS_DIR_SEP(*p) ? DIR_SEP : *p);
    if (IS_DIR_SEP(*p))
      while (*(p+1) && IS_DIR_SEP(*(p+1))) p++;
  }
  *q = '\0';

  return path;
}


/* Smart concatenation of 2 pathes: add the separator only 
 when needed. Allocates a new path. */
string concat_pathes(const_string p1, const_string p2)
{
  string res;
  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "Concat pathes %s and %s\n", p1, p2);
  }

  assert (p1 && p2);

  res = normalize(concat3(p1, DIR_SEP_STRING, p2));

  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "\t=> %s\n", res);
  }

  return res;
}

/*
  Kpathsea's basename does not strip suffixes
  */
string my_basename(string name, string suffix)
{
  string res = (string)xbasename(name);
  if (suffix) {
    int suffix_len = strlen(suffix);
    if (FILESTRCASEEQ(res+strlen(res)-suffix_len, suffix))
      *(res+strlen(res)-suffix_len) = '\0';
  }
  return xstrdup(res);
}

#if 0
/*
  Kpathsea's kpse_find_file() is not parametrized by '--progname'.
  This is an attempt to do that.
  */

/* The path spec we are defining, one element of the global array.  */
#define FMT_INFO kpse_format_info[format]
/* Call add_suffixes.  */
#define SUFFIXES(args) add_suffixes(&FMT_INFO.suffix, args, NULL)
#define ALT_SUFFIXES(args) add_suffixes (&FMT_INFO.alt_suffix, args, NULL)

/* Call `init_path', including appending the trailing NULL to the envvar
   list. Also initialize the fields not needed in setting the path.  */
#define INIT_FORMAT(text, default_path, envs) \
  FMT_INFO.type = text; \
  init_path (&FMT_INFO, default_path, envs, NULL)
#endif
