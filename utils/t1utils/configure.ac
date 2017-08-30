dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from t1utils-src/configure.in
dnl
dnl *********************************************************************
dnl
m4_include([version.ac])[] dnl define t1utils_version
AC_INIT([t1utils (TeX Live)], t1utils_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([t1utils-src/t1ascii.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([t1utils])

AC_PROG_CC
AC_C_CONST

dnl
dnl strerror()?
dnl

AC_REPLACE_FUNCS([strerror])

AC_CHECK_FUNCS([strtoul])

dnl
dnl integer types
dnl

AC_CHECK_HEADERS([inttypes.h sys/types.h])

if test "x$ac_cv_header_inttypes_h:$ac_cv_header_sys_types_h" = xno:yes; then
  AC_CACHE_CHECK([for uintXX_t typedefs],
                 [lcdf_cv_uint_t],
                 [AC_EGREP_HEADER([(^|[^a-zA-Z_0-9])uint32_t[^a-zA-Z_0-9]],
                                  [sys/types.h],
                                  [lcdf_cv_uint_t=yes],
                                  [lcdf_cv_uint_t=no])])
  if test "x$lcdf_cv_uint_t" = xno; then
    AC_CACHE_CHECK([for u_intXX_t typedefs],
                   [lcdf_cv_u_int_t],
                   [AC_EGREP_HEADER([(^|[^a-zA-Z_0-9])u_int32_t[^a-zA-Z_0-9]],
                                    [sys/types.h],
                                    [lcdf_cv_u_int_t=yes],
                                    [lcdf_cv_u_int_t=no])])
  fi
fi
if test "x$ac_cv_header_inttypes_h:$lcdf_cv_uint_t" = xno:no; then
  if test "x$lcdf_cv_u_int_t" = xyes; then
    AC_DEFINE([HAVE_U_INT_TYPES], 1,
              [Define if you have u_intXX_t types but not uintXX_t types.])
  else
    AC_MSG_WARN([
=========================================

Neither uint32_t nor u_int32_t defined by <inttypes.h> or <sys/types.h>!
Assuming "short" has 16 bits and "int" has 32 bits.

=========================================])
    AC_DEFINE([HAVE_FAKE_INT_TYPES], 1,
              [Define if intXX_t types are not available.])
  fi
fi

AC_CHECK_TYPES([uintptr_t], [], [],
[#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
])

AC_CHECK_SIZEOF([void *])
AC_CHECK_SIZEOF([unsigned long])
AC_CHECK_SIZEOF([unsigned int])


AC_CONFIG_HEADERS([config.h])

dnl
dnl verbatim portions of the header
dnl

AH_TOP([#ifndef T1UTILS_CONFIG_H
#define T1UTILS_CONFIG_H])

AH_BOTTOM([#include <lcdf/inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Prototype strerror if we don't have it. */
#if !HAVE_STRERROR
char *strerror(int errno);
#endif

/* Prototype my_memmem */
void *my_memmem(const void* haystack, size_t haystack_len,
             const void* needle, size_t needle_len);

#ifdef __cplusplus
}
/* Get rid of a possible inline macro under C++. */
# define inline inline
#endif

#endif /* T1UTILS_CONFIG_H */])

AC_SUBST([T1UTILS_TREE], [t1utils-src])

dnl
dnl Output
dnl

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
