/* mingw32.h: declarations for mingw32.

   Copyright 2009-2014 Taco Hoekwater <taco@luatex.org>.

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

#ifndef _MINGW32_H_
#define _MINGW32_H_

/* We need GetLongPathName and perhaps others.  */
#if !defined WINVER || WINVER < 0x0500
#undef WINVER
#define WINVER 0x0500
#endif

#include <stdlib.h>
/* The embedded rndnpc.h defines boolean as 'unsigned char',
   and we do not want that.
   This should be safe as long as we don't use npc ourselves. */
#define boolean saved_boolean
/* With WINVER >= 0x0403 winuser.h declares INPUT as `struct tagINPUT`,
   and we do not want that. */
#define INPUT saved_INPUT
#include <windows.h>
#include <winerror.h>
#include <winnt.h>
#undef boolean
#undef INPUT
#include <dirent.h>
#include <direct.h>
#include <fcntl.h>
#include <ctype.h>

/* sys/types.h defines off_t as `long' and we do not want that.
   We need to include unistd.h and sys/stat.h using off_t
   before defining off_t (no need to include wchar.h).  */
#include <unistd.h>
#include <sys/stat.h>
#define off_t off64_t
#define ftello ftello64
#define fseeko fseeko64

#ifndef MAXPATHLEN
#define MAXPATHLEN _MAX_PATH
#endif

#ifndef MAX_PIPES
#define MAX_PIPES 128
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

extern void init_user_info (void);
extern BOOL look_for_cmd (const char *, char **);
extern char *quote_args(char **);

#endif /* MAKE_KPSE_DLL */

extern KPSEDLL BOOL win32_get_long_filename (char *, char *, int);
extern KPSEDLL void texlive_gs_init (void);
extern KPSEDLL double win32_floor (double);

static inline FILE *
win32_popen (const char *command, const char *fmode)
{
  char mode[3];

  /* We always use binary mode */
  mode[0] = fmode[0];
  mode[1] = 'b';
  mode[2] = '\0';

  return _popen (command, mode);
}
#undef popen
#define popen(cmd, mode) win32_popen(cmd, mode)

#ifdef __cplusplus
}
#endif

#endif
