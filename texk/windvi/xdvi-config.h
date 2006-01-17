/* xdvi-config.h: master configuration file, included first by all
   compilable source files (not headers).  */

#ifndef CONFIG_H
#define CONFIG_H

#define KPATHSEA 1

/* The stuff from the path searching library.  */
#include <kpathsea/c-auto.h>
#include <kpathsea/config.h>

#include <setjmp.h>

#ifndef HAVE_VPRINTF
#ifdef HAVE_DOPRNT
#define	vfprintf(stream, message, args)	_doprnt(message, args, stream)
/* If we have neither, should fall back to fprintf with fixed args.  */
#endif
#endif

/* Some xdvi options we want by default.  */
#ifndef USE_PK
#define USE_PK
#endif
#if 0
/* This is defined in ./c-auto.h */
#ifndef USE_GF
#define USE_GF
#endif
#endif /* 0 */
#ifndef MAKEPK
#define MAKEPK
#endif

#ifndef NOSELFILE
#define SELFILE
#endif
#ifndef NOGRID
#define GRID
#endif
#ifndef NOTEXXET
#define TEXXET
#endif
#if 0 && defined (HAVE_LIBWWW) && defined (HAVE_LIBWWW_H)
#define HTEX
#endif

/* On the other hand, NOTOOL means we lose practically everything.  */
#if defined(NOTOOL) && !defined(WIN32)
#undef SELFILE
#undef BUTTONS
#undef GRID
#undef HTEX
#endif /* NOTOOL */

#ifdef WIN32
#  include "windvi.h"
#define MYRGB(r,g,b) RGB(r,g,b)
#define MYADJUSTRGB(x) (x)
#  ifndef min
#    define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#  endif
#  ifndef max
#    define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#  endif
#else
/* xdvi's definitions.  */
#  include "xdvi.h"
#endif

#endif /* not CONFIG_H */
