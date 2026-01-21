/*  dirent.h

    Definitions for UNIX like directory operations.

*/

/*
      for VC++2.0 (  by A. K., 1995 )
      Corrected by Clerk Ma, to work in 64bit mode in Visual Studio 2015,(2017).
*/

#ifndef __DIRENTAK_H
#define __DIRENTAK_H

/* avoid redefinition error */
#define boolean ms_boolean_type
#include <windows.h>
#undef boolean

#include <kpathsea/c-proto.h>

#ifndef NULL
#define NULL 0
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* dirent structure returned by readdir().
 */
struct dirent
{
  int         d_isdir;
  int         d_namlen;
  char        d_name[260];
};

/* DIR type returned by opendir().  The members of this structure
   must not be accessed by application programs.
*/
typedef struct
{
    HANDLE         _d_hdir;      /* directory handle */
    struct dirent *_d_direntptr; /* directory entry pointer */
    unsigned       _d_magic;     /* file attribute */
    unsigned       _d_nfiles;    /* access count */
    char           _d_buf[318];  /* buffer (WIN32_FIND_DATA) */
} DIR;

KPSEDLL DIR                 *opendir(char *__dirname);
KPSEDLL struct dirent       *readdir(DIR *__dir);
KPSEDLL int                  closedir(DIR *__dir);

#ifdef __cplusplus
}
#endif
#endif  /* __DIRENTAK_H */
