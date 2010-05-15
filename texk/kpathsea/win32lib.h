/* Public domain.  */

#ifndef _WIN32LIB_H_
#define _WIN32LIB_H_

#include <io.h>
#include <fcntl.h>
#include <direct.h>

#if !__STDC__
#if _MSC_VER>=1200
   #pragma warning(push)
#endif
#pragma warning(disable: 4668)
#pragma warning(disable: 4255)

#define boolean donotuse_boolean
#include <windows.h>	/* Somewhere here WIN32 gets defined */
#undef boolean

#if _MSC_VER>=1200
   #pragma warning(pop)
#else
   #pragma warning(default: 4668)
   #pragma warning(default: 4255)
#endif

#else /*__STDC__*/
/* non-ansi definitions for an ANSI compliation with /Fa */
#define WIN32
#define off_t _off_t
#define putenv _putenv
#define getcwd _getcwd
#define getcw _getcw
#define putenv _putenv
#define strdup _strdup
#define vsnprintf _vsnprintf
#define fileno _fileno
#define strlwr _strlwr
#define isascii __isascii
#define mkdir _mkdir
#define stat _stat
#define stricmp _stricmp
#define MAX_PATH _MAX_PATH
#define S_IFREG _S_IFREG
#define S_IFMT _S_IFMT

#endif /*__STDC__*/

/*This is not defined even if !__STDC__*/
#define snprintf _snprintf

/*The gnu name is different*/
#define strcasecmp stricmp

#endif
