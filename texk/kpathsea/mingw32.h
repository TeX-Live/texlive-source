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

#define ftello ftello64
#define fseeko fseeko64

#ifndef MAXPATHLEN
#define MAXPATHLEN _MAX_PATH
#endif

#ifndef MAX_PIPES
#define MAX_PIPES 128
#endif

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

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

extern void init_user_info (void);
extern BOOL look_for_cmd (const char *, char **);
extern char *quote_args(char **);

#endif /* MAKE_KPSE_DLL */

extern KPSEDLL BOOL win32_get_long_filename (char *, char *, int);
extern KPSEDLL int win32_system(const char *, int);
extern KPSEDLL void dostounix_filename (char *p);

#endif
