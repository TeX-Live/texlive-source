/* xdvi-config.h: master configuration file, included first by all
   compilable source files (not headers).  */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Some xdvi options we want by default.  */
#define USE_PK
#define USE_GF
#define MAKEPK

/* all others are defined in this file */
#include "c-auto.h"

#ifndef HAVE_VPRINTF
#ifdef HAVE_DOPRNT
#define	vfprintf(stream, message, args)	_doprnt(message, args, stream)
/* If we have neither, should fall back to fprintf with fixed args.  */
#endif
#endif

/* XPM is used for the toolbar buttons and in xicon.c */
#if USE_XPM
# if defined(HAVE_X11_XPM_H) || defined(HAVE_XM_XPMP_H) || defined(HAVE_XPM_H)
#  define HAVE_XPM 1
# else
#  define HAVE_XPM 0
# endif
#endif

/* enable X server info output */
#define XSERVER_INFO 1

/* change this to 1 when #470325 is fixed */
#define FIXED_FLUSHING_PAGING 0

#endif /* not CONFIG_H_ */
