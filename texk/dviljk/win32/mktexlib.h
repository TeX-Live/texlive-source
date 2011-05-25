#ifndef _MKTEXLIB_H_
#define _MKTEXLIB_H_

#if defined(WIN32)
#  ifdef MKTEX_DLL
#    ifdef MAKE_MKTEX_DLL
#      define MKTEXDLL __declspec( dllexport)
#    else
#      define MKTEXDLL __declspec( dllimport)
#    endif
#  else
#    define MKTEXDLL
#  endif
#else /* ! WIN32 */
#  define MKTEXDLL
#  define __cdecl
#endif

#include <assert.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <signal.h>
#endif
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>

#include <kpathsea/config.h>
#include <kpathsea/kpathsea.h>
#include <kpathsea/version.h>

#ifndef _WIN32
#include <sys/param.h>
#endif

#ifdef WIN32
# ifdef mkdir
#  undef mkdir
# endif
# define mkdir(p,m)  _mkdir(p)
#else
//# define FS_CASE_SENSITIVE	_FILESYS_CASE_SENSITIVE
typedef unsigned int DWORD;
#endif

/* gsftopk defines KPSE_LAST_DEBUG+2, so avoid clashes */
#define MKTEX_DEBUG (KPSE_LAST_DEBUG + 3)
#define MKTEX_FINE_DEBUG (KPSE_LAST_DEBUG + 4)


#define LAST_CHAR(s) (*((s)+strlen(s)-1))

extern MKTEXDLL const_string mktex_version_string;

#endif /* _MKTEXLIB_H_ */
