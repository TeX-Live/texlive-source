#ifndef _MINGW32_H_
#define _MINGW32_H_

#include <stdlib.h>
/* The embedded rndnpc.h defines boolean as 'unsigned char',
   and we do not want that.
   This should be safe as long as we don't use npc ourselves. */
#define boolean saved_boolean
#include <windows.h>
#include <winerror.h>
#include <winnt.h>
#undef boolean
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

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

extern void init_user_info (void);
extern BOOL look_for_cmd (const char *, char **);
extern char *quote_args(char **);

#endif /* MAKE_KPSE_DLL */

extern KPSEDLL BOOL win32_get_long_filename (char *, char *, int);
extern KPSEDLL void dostounix_filename (char *p);

#endif
