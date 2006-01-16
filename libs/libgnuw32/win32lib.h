/* System description file for Windows NT.
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.

This file is part of Web2C.

Web2C is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Web2C is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Web2C; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _GNU_W32_H_
#define _GNU_W32_H_

#pragma warning( disable : 4007 4096 4018 4244 )  

#if defined(WIN32)
# if defined(KPSEDLL)
#  define GNUW32DLL KPSEDLL
# else
#  if defined(GNUW32_DLL) || defined(KPSE_DLL)
#   if defined(MAKE_GNUW32_DLL) || defined(MAKE_KPSE_DLL)
#    define GNUW32DLL __declspec( dllexport)
#   else
#    define GNUW32DLL __declspec( dllimport)
#   endif
#  else
#   define GNUW32DLL
#  endif
# endif
#else /* ! WIN32 */
# define GNUW32DLL
#endif

/*
 *      Define symbols to identify the version of Unix this is.
 *      Define all the symbols that apply correctly.
 */

#ifndef DOSISH
#define DOSISH
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN      _MAX_PATH
#endif

#define HAVE_DUP2       	1
#define HAVE_RENAME     	1
#define HAVE_RMDIR      	1
#define HAVE_MKDIR      	1
#define HAVE_GETHOSTNAME	1
#define HAVE_RANDOM		1
#define USE_UTIME		1
#define HAVE_MOUSE		1
#define HAVE_TZNAME		1

typedef unsigned char boolean;
#define HAVE_BOOLEAN            1
#define false FALSE
#define true TRUE

#include <windows.h>

#undef CDECL
#define CDECL __cdecl

/* Defines size_t and alloca ().  */
#include <malloc.h>

/* For proper declaration of environ.  */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * pwd.h doesn't exist on NT, so we put together our own.
 */

struct passwd {
  char *pw_name;
  char *pw_passwd;
  int   pw_uid;
  int   pw_gid;
  int   pw_quota;
  char *pw_gecos;
  char *pw_dir;
  char *pw_shell;
};

#ifdef FPTEX
#define REPLACE_LIBC_FUNCTIONS 1
#endif

#if defined(__STDC__)
/* We want to compile with __STDC__ defined, but in this
   case, MSVC headers do not define the standard names for
   the C library functions (_open() instead of open()) */
#include <oldnames.h>
#else
# if REPLACE_LIBC_FUNCTIONS
#  define stat(p, s) win32_stat(p, s)
#  define system(p) win32_system(p, 0)
#  define popen(cmd, mode) win32_popen(cmd, mode)
#  define pclose(file) win32_pclose(file)
# endif
#endif

#define getwd(dir)                 GetCurrentDirectory(MAXPATHLEN, dir)
#ifndef index
#define index(s, c)                strchr(s, c)
#endif
#ifndef rindex
#define rindex(s, c)               strrchr(s, c)
#endif
#define pipe(fd, size, mode)       _pipe(fd, size, mode)
#define strcasecmp(s1, s2)         _stricmp(s1, s2)
#define strncasecmp(s1, s2, n)     _strnicmp(s1, s2, n)
#define sleep(n)                   Sleep((DWORD)n);
#define vsnprintf                  _vsnprintf
#define snprintf                   _snprintf

#ifdef _MSC_VER

/* Test for each symbol individually and define the ones necessary (some
   systems claiming Posix compatibility define some but not all). */

#if defined (S_IFBLK) && !defined (S_ISBLK)
#define        S_ISBLK(m)      (((m)&S_IFMT) == S_IFBLK)       /* block device */
#endif

#if defined (S_IFCHR) && !defined (S_ISCHR)
#define        S_ISCHR(m)      (((m)&S_IFMT) == S_IFCHR)       /* character device */
#endif

#if defined (S_IFDIR) && !defined (S_ISDIR)
#define        S_ISDIR(m)      (((m)&S_IFMT) == S_IFDIR)       /* directory */
#endif

#if defined (S_IFREG) && !defined (S_ISREG)
#define        S_ISREG(m)      (((m)&S_IFMT) == S_IFREG)       /* file */
#endif

#if defined (S_IFIFO) && !defined (S_ISFIFO)
#define        S_ISFIFO(m)     (((m)&S_IFMT) == S_IFIFO)       /* fifo - named pipe */
#endif

#if defined (S_IFLNK) && !defined (S_ISLNK)
#define        S_ISLNK(m)      (((m)&S_IFMT) == S_IFLNK)       /* symbolic link */
#endif

#if defined (S_IFSOCK) && !defined (S_ISSOCK)
#define        S_ISSOCK(m)     (((m)&S_IFMT) == S_IFSOCK)      /* socket */
#endif

#endif /* _MSC_VER */

/* For access.  */
#ifndef F_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif /* not F_OK */

/*
  Path separators.
  Redundant with <kpathsea/c-pathch.h>, but useful
  outside of kpathsea.
*/

/* On DOS, it's good to allow both \ and / between directories.  */
#ifndef IS_DIR_SEP
#define IS_DIR_SEP(ch) ((ch) == '/' || (ch) == '\\')
#endif
#ifndef IS_DEVICE_SEP
#define IS_DEVICE_SEP(ch) ((ch) == ':')
#endif
#ifndef NAME_BEGINS_WITH_DEVICE
#define NAME_BEGINS_WITH_DEVICE(name) (*(name) && IS_DEVICE_SEP((name)[1]))
#endif
/* On win32, UNC names are authorized */
#ifndef IS_UNC_NAME
#define IS_UNC_NAME(name) (strlen(name)>=3 && IS_DIR_SEP(*name)  \
                            && IS_DIR_SEP(*(name+1)) && isalnum(*(name+2)))
#endif

#ifndef min
#define min(a,b) ((b) > (a) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifdef  __cplusplus
extern "C" {
#endif

/* Functions from popen.c */

#define MAX_PIPES 128

extern GNUW32DLL FILE * __cdecl win32_popen(const char *, const char *);
extern GNUW32DLL int __cdecl win32_pclose(FILE *);

/* Functions from stat.c */

extern GNUW32DLL int __cdecl win32_stat(const char * path, struct stat * buf);

/* Functions from system.c */

extern GNUW32DLL int __cdecl win32_system(const char * cmd, int async);

/* Functions from string.c */
/*
  Unfortunately, these functions are defined in kpathsea
  but useful elsewhere too. As they have a small footprint,
  they are duplicated for the moment and not exported.
*/
extern GNUW32DLL void *xmalloc(unsigned size);
extern GNUW32DLL void *xrealloc(void *, unsigned size);
extern GNUW32DLL char *xstrdup(const char *);
extern GNUW32DLL char *concat(const char *s1, const char *s2);
extern GNUW32DLL char *concat3(const char *s1, const char *s2, const char *s3);
extern GNUW32DLL char * __cdecl concatn(const char *s1, ...);

/* Functions from volume.c */

typedef struct volume_info_data {
  struct volume_info_data * next;

  /* time when info was obtained */
  DWORD     timestamp;

  /* actual volume info */
  char *    root_dir;
  DWORD     serialnum;
  DWORD     maxcomp;
  DWORD     flags;
  char *    name;
  char *    type;
} volume_info_data;

extern GNUW32DLL int is_fat_volume (const char * name, const char ** pPath);
extern GNUW32DLL int get_volume_info (const char * name, const char ** pPath);

/* Functions from filename.c */

extern GNUW32DLL void dostounix_filename (char *p);
extern GNUW32DLL void unixtodos_filename (char *p);
extern GNUW32DLL char *quote_filename (char *p);
extern GNUW32DLL char *unquote_filename (char *p);
extern GNUW32DLL int crlf_to_lf (int n, unsigned char *buf, unsigned *lf_count);
extern GNUW32DLL BOOL win32_get_long_filename (char * name, char * buf, int size);
extern GNUW32DLL const char *map_win32_filename (const char * name, const char ** pPath);
extern int _parse_root (char * name, char ** pPath);

/* Functions from user.c */
extern GNUW32DLL int getuid();
extern GNUW32DLL int geteuid();
extern GNUW32DLL int getgid();
extern GNUW32DLL int getegid();
extern GNUW32DLL struct passwd *getpwuid(int);
extern GNUW32DLL struct passwd *getpwnam(const char *);
extern GNUW32DLL void init_user_info();
extern GNUW32DLL void init_user_info();

/* Functions from home.c */
extern GNUW32DLL void set_home_warning(void);
extern GNUW32DLL char *get_home_directory (void);

/* Functions from time.h */
extern time_t convert_time (FILETIME ft);

/* Functions from lookcmd.c */

extern GNUW32DLL BOOL look_for_cmd(const char *command, char **app_name);

extern GNUW32DLL void *parse_cmdline(char *line, char **input, char **output, int *nb_cmds);
extern GNUW32DLL char *build_cmdline(char ***cmd, char *input, char *output);

/* Functions from error.c */

extern GNUW32DLL void Win32Error(char *caller);
extern GNUW32DLL void Win32Msg(char *msg);

/* ============================================================ */

#ifdef  __cplusplus
}
#endif

#endif /* not _GNU_W32_H_ */
