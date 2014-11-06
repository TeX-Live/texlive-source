#ifndef LCDF_TYPETOOLS_CONFIG_H
#define LCDF_TYPETOOLS_CONFIG_H 1

#include <autoconf.h>

/* Allow compilation on Windows (thanks, Fabrice Popineau). */
#ifdef WIN32
# ifdef __MINGW32__
#  include <winsock2.h>
#  include <windows.h>
# else
#  include <win32lib.h>
# endif
#else
# include <stddef.h>
# define CDECL /* nothing */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Prototype strerror if necessary. */
#if !HAVE_STRERROR
char *strerror(int errno);
#endif

/* Prototype strnlen if necessary. */
#if !HAVE_DECL_STRNLEN
size_t strnlen(const char *s, size_t maxlen);
#endif

/* Prototype good_strtod if necessary. */
#if HAVE_BROKEN_STRTOD
double good_strtod(const char *s, char **endptr);
#endif

#ifdef __cplusplus
}
/* Get rid of a possible inline macro under C++. */
# define inline inline
#endif

#endif /* LCDF_TYPETOOLS_CONFIG_H */
