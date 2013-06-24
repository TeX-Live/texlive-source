/* debug.h: runtime tracing.

   Copyright 1993, 1994, 1995, 1996, 2008 Karl Berry.
   Copyright 1999, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef KPATHSEA_DEBUG_H
#define KPATHSEA_DEBUG_H

/* If NO_DEBUG is defined (not recommended), skip all this.  */
#ifndef NO_DEBUG

#include <kpathsea/c-proto.h>
#include <kpathsea/c-std.h>
#include <kpathsea/types.h>

#if defined(WIN32)
#if defined(_DEBUG)
/* This was needed at some time for catching errors in pdftex. */
#include <crtdbg.h>
#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  SETUP_CRTDBG \
   { _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );    \
     _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );  \
     _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );   \
     _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT ); \
     _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );  \
     _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );\
   }
#else /* ! _DEBUG */
#define SET_CRT_DEBUG_FIELD(a)
#define CLEAR_CRT_DEBUG_FIELD(a)
#define SETUP_CRTDBG
#endif /* _DEBUG */
#endif /* WIN32 */

/* OK, we'll have tracing support.  */
#define KPSE_DEBUG

/* Test if a bit is on.  */
#define KPATHSEA_DEBUG_P(bit) (kpse->debug & (1 << (bit)))

#if defined (KPSE_COMPAT_API)

/* Set a bit.  */
#define KPSE_DEBUG_SET(bit) kpathsea_debug |= 1 << (bit)

/* Test if a bit is on.  */
#define KPSE_DEBUG_P(bit) (kpathsea_debug & (1 << (bit)))

#endif /* KPSE_COMPAT_API */

#define KPSE_DEBUG_STAT 0               /* stat calls */
#define KPSE_DEBUG_HASH 1               /* hash lookups */
#define KPSE_DEBUG_FOPEN 2              /* fopen/fclose calls */
#define KPSE_DEBUG_PATHS 3              /* search path initializations */
#define KPSE_DEBUG_EXPAND 4             /* path element expansion */
#define KPSE_DEBUG_SEARCH 5             /* searches */
#define KPSE_DEBUG_VARS 6               /* variable values */
#define KPSE_LAST_DEBUG KPSE_DEBUG_VARS

/* A printf for the debugging.  */
#define DEBUGF_START() do { fputs ("kdebug:", stderr)
#define DEBUGF_END()        fflush (stderr); } while (0)

#define DEBUGF(str)                                                     \
  DEBUGF_START (); fputs (str, stderr); DEBUGF_END ()
#define DEBUGF1(str, e1)                                                \
  DEBUGF_START (); fprintf (stderr, str, e1); DEBUGF_END ()
#define DEBUGF2(str, e1, e2)                                            \
  DEBUGF_START (); fprintf (stderr, str, e1, e2); DEBUGF_END ()
#define DEBUGF3(str, e1, e2, e3)                                        \
  DEBUGF_START (); fprintf (stderr, str, e1, e2, e3); DEBUGF_END ()
#define DEBUGF4(str, e1, e2, e3, e4)                                    \
  DEBUGF_START (); fprintf (stderr, str, e1, e2, e3, e4); DEBUGF_END ()

#ifdef __cplusplus
extern "C" {
#endif

#undef fopen
#define fopen kpse_fopen_trace
extern KPSEDLL FILE *fopen (const char *filename, const char *mode);
#undef fclose
#define fclose kpse_fclose_trace
extern KPSEDLL int fclose (FILE *);

#ifdef __cplusplus
}
#endif

#endif /* not NO_DEBUG */

#endif /* not KPATHSEA_DEBUG_H */
