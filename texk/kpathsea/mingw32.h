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

void init_user_info (void);
BOOL win32_get_long_filename (char *, char *, int);
void set_home_warning (void);
BOOL look_for_cmd(const char *, char **);

#endif
