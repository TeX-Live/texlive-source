/* readable.c: check if a filename is a readable non-directory file.

   Copyright 1993, 1995, 1996, 2008, 2011, 2012, 2016, 2018 Karl Berry.
   Copyright 1998, 1999, 2000, 2001, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>
#include <kpathsea/c-namemx.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/readable.h>
#include <kpathsea/tex-hush.h>


/* If access can read FN, run stat (assigning to stat buffer ST) and
   check that fn is not a directory.  Don't check for just being a
   regular file, as it is potentially useful to read fifo's or some
   kinds of devices.  */

#ifdef __DJGPP__
/* `stat' is way too expensive for such a simple job.  */
#define READABLE(kpse, fn, st) \
  (access ((fn), R_OK) == 0 && access ((fn), D_OK) == -1)
#elif defined (WIN32)
/* st must be an unsigned int under Windows */
static boolean
READABLE(kpathsea kpse, const_string fn, unsigned int st)
{
  wchar_t *fnw;
  unsigned char *fnn;
  unsigned char *p;
  size_t len = strlen(fn);

  fnn = xmalloc(len + 10);
/*
  Support very long input path name, longer than _MAX_PATH for
  Windows, if it really exists and input name is given in
  full-absolute path in a command line.
  /../, /./, \..\, \.\ should be excluded (2020/06/06)
*/
  p = strstr(fn, ".\\");
  if (!p) {
    p = strstr(fn, "./");
  }
  if (!p && len > 2 && ((fn[0] == '/' && fn[1] == '/') ||
      (fn[0] == '\\' && fn[1] == '\\' && fn[2] != '?'))) {
    fn += 2;
    strcpy (fnn, "\\\\?\\UNC\\");
    strcat (fnn, fn);
  } else if (!p && len > 2 && fn[1] == ':') {
    strcpy (fnn, "\\\\?\\");
    strcat (fnn, fn);
  } else {
    strcpy (fnn, fn);
  }

  for (p = fnn; *p; p++) {
    if (*p == '/')
      *p = '\\';
  }

  fnw = get_wstring_from_mbstring(kpse->File_system_codepage, fnn, fnw=NULL);
  if ((st = GetFileAttributesW(fnw)) != 0xFFFFFFFF) {
    /* succeeded */
    errno = 0;
  } else {
    switch(GetLastError()) {
    case ERROR_BUFFER_OVERFLOW:
      errno = ENAMETOOLONG;
      break;
    case ERROR_ACCESS_DENIED:
      errno = EACCES;
      break;
    default :
      errno = EIO;          /* meaningless, will make ret=NULL later */
      break;
    }
  }
  free (fnn);
  if (fnw)
    free (fnw);
  return ((st != 0xFFFFFFFF) && !(st & FILE_ATTRIBUTE_DIRECTORY));
}
#else /* not __DJGPP__ and not WIN32 */
#define READABLE(kpse, fn, st) \
 (access((fn), R_OK) == 0 && stat((fn), &(st)) == 0 && !S_ISDIR ((st).st_mode))
#endif


/* POSIX invented the brain-damage of not necessarily truncating
   filename components; the system's behavior is defined by the value of
   the symbol _POSIX_NO_TRUNC, but it can't be changed.  */

string
kpathsea_readable_file (kpathsea kpse, string name)
{
#ifdef WIN32
  unsigned int st = 0;
#else /* ! WIN32 */
  struct stat st;
#endif

  kpathsea_normalize_path (kpse, name);
  if (READABLE (kpse, name, st)) {
    return name;

#ifdef ENAMETOOLONG
  } else if (errno == ENAMETOOLONG) {
    /* Truncate any too-long components in NAME.  */
    unsigned c_len = 0;        /* Length of current component.  */
    char *s = name;            /* Position in current component.  */
    char *t = name;            /* End of allowed length.  */

    for (; *s; s++) {
      if (c_len <= NAME_MAX) {
        t = s;
      }
#if defined(WIN32)
      if (kpathsea_IS_KANJI (kpse, s)) {
        s++;
        c_len += 2;
        continue;
      }
#endif /* WIN32 */
      if (IS_DIR_SEP (*s) || IS_DEVICE_SEP (*s)) {
        if (c_len > NAME_MAX) {
          /* Truncate if past the max for a component.  */
          memmove (t, s, strlen (s) + 1);
          s = t;
        }
        /* At a directory delimiter, reset component length.  */
        c_len = 0;
      } else {
        c_len++;
      }
    }
    if (c_len > NAME_MAX) {
      /* Truncate if past the max for last component.  */
      *t = 0;
    }

    /* Perhaps some other error will occur with the truncated name, so
       let's call access again.  */
    if (READABLE (kpse, name, st)) /* Success.  */
      return name;
#endif /* ENAMETOOLONG */

  } else { /* Some other error.  */
    if (errno == EACCES) { /* Maybe warn them if permissions are bad.  */
      if (!kpathsea_tex_hush (kpse, "readable")) {
        perror (name);
      }
    }
  }

  return NULL;
}


#if defined (KPSE_COMPAT_API)
string
kpse_readable_file (string name)
{
  return kpathsea_readable_file (kpse_def, name);
}
#endif
