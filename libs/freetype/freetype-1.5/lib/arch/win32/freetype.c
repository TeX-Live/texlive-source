/* This file is part of the FreeType project */

/* single object library component for Win32 */
#define TT_MAKE_OPTION_SINGLE_OBJECT

/* Note, you should define the FT_EXPORT_xxx macros here if you   */
/* want to build a Win32 DLL. If undefined, the macros default to */
/* "extern"/"" (nothing), which is suitable for static libraries. */
/* See `ttconfig.h' for details.                                  */

/* The macro FT_EXPORT_DEF is placed before each high-level API   */
/* function declaration, and FT_EXPORT_FUNC before each           */
/* definition (body). Similarly, FT_EXPORT_DECL is placed before  */
/* each variable declaration, and FT_EXPORT_VAR before its        */
/* definition. You can then use it to take any compiler-specific  */
/* pragma for DLL-exported symbols                                */

/* first include common core components */

#include "ttapi.c"
#include "ttcache.c"
#include "ttcalc.c"
#include "ttcmap.c"
#include "ttdebug.c"
#include "ttgload.c"
#include "ttinterp.c"
#include "ttload.c"
#include "ttobjs.c"
#include "ttraster.c"

/* then system-specific (or ANSI) components */

#include "ttfile.c"
#include "ttmemory.c"
#include "ttmutex.c"

/* finally, add some extensions */

#ifdef TT_CONFIG_OPTION_EXTEND_ENGINE
#include "ttextend.c"
#endif


/* END */
