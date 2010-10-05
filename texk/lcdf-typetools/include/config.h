#ifndef LCDF_TYPETOOLS_CONFIG_H
#define LCDF_TYPETOOLS_CONFIG_H 1

#include <autoconf.h>

/* Allow compilation on Windows (thanks, Fabrice Popineau). */
#ifdef WIN32
# ifdef __MINGW32__
#  include <windows.h>
# else
#  include <win32lib.h>
# endif
#else
# define CDECL /* nothing */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Prototype strerror if we don't have it. */
#if !HAVE_STRERROR
char *strerror(int errno);
#endif

/* Prototype good_strtod if we need it. */
#if BROKEN_STRTOD
double good_strtod(const char *nptr, char **endptr);
#endif

#ifdef __cplusplus
}
/* Get rid of a possible inline macro under C++. */
# define inline inline
#endif

#endif /* LCDF_TYPETOOLS_CONFIG_H */
