/*
 * snprintf.h -- interface for our fixup vsnprintf() etc. functions
 */

#ifndef SNPRINTF_H
#define SNPRINTF_H 1

#ifdef __GNUC__
#pragma interface
#endif

#include "config2.h"
#include <stdarg.h> /* ... */
#ifndef __cplusplus
#  define EXTERN_C extern
#else
#  define EXTERN_C extern "C"
#endif

EXTERN_C slen_t fixup_vsnprintf(char *str, slen_t count, PTS_const char *fmt, va_list args);
EXTERN_C slen_t fixup_snprintf(char *str,slen_t count,PTS_const char *fmt,...);
EXTERN_C slen_t fixup_vasprintf(char **ptr, PTS_const char *format, va_list ap);
EXTERN_C slen_t fixup_asprintf(char **ptr, PTS_const char *format, ...);
EXTERN_C slen_t fixup_sprintf(char *ptr, PTS_const char *format, ...);

#endif /* snprintf.h */
