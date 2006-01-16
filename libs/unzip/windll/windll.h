#ifndef __windll_h   /* prevent multiple inclusions */
#define __windll_h

#include <windows.h>
#include <assert.h>    /* required for all Windows applications */
#include <setjmp.h>
#include <commdlg.h>
#if (!defined(__RSXNT__) && !defined(__MINGW32__))
#  include <dlgs.h>
#endif
#define UNZIP_INTERNAL
#include "unzip.h"
#include "structs.h"
#include "decs.h"

#ifndef MSWIN
#  define MSWIN
#endif

/* Allow compilation under Borland C++ also */
#ifndef __based
#  define __based(A)
#endif

#ifndef PATH_MAX
#  define PATH_MAX 260            /* max total file or directory name path */
#endif

#define IDM_REPLACE_NO     100
#define IDM_REPLACE_TEXT   101
#define IDM_REPLACE_YES    102
#define IDM_REPLACE_ALL    103
#define IDM_REPLACE_NONE   104
#define IDM_REPLACE_RENAME 105
#define IDM_REPLACE_HELP   106

extern jmp_buf dll_error_return;

extern HANDLE hInst;        /* current instance */

#ifdef UNZIP_INTERNAL
void FreeDllMem(__GPRO);
int win_fprintf(zvoid *pG, FILE *file, unsigned int, char far *);
#endif

#endif /* __windll_h */
