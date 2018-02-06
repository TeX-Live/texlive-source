#ifndef __ZZIP_INTERNAL_FNMATCH_H
#define __ZZIP_INTERNAL_FNMATCH_H
/** included by fseeko.c, mmapped.c, memdisk.c */

#include <zzip/conf.h>
#include <stdio.h>
#include <string.h>

#ifdef ZZIP_HAVE_FNMATCH_H
#include <fnmatch.h>
#endif

#ifdef ZZIP_HAVE_FNMATCH_H
#define _zzip_fnmatch fnmatch
# ifdef FNM_CASEFOLD
# define _zzip_FNM_CASEFOLD FNM_CASEFOLD
# else
# define _zzip_FNM_CASEFOLD 0
# endif
#else
# define _zzip_FNM_CASEFOLD 0
/* if your system does not have fnmatch, we fall back to strcmp: */
#define _zzip_fnmatch(x,y,z) strcmp ((x),(y))
#endif

#endif
