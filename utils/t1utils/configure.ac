dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([t1utils for TeX Live], [1.34], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([t1ascii.c])
AC_CONFIG_AUX_DIR([../../build-aux])

AM_INIT_AUTOMAKE([foreign dist-bzip2 check-news])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_C_CONST

AC_ARG_ENABLE([warnings],
              AS_HELP_STRING([--enable-warnings],
                             [compile with -Wall]))

if test "x$enable_warnings" = xyes ; then
  CFLAGS="$CFLAGS -W -Wall"
fi


dnl
dnl strerror()?
dnl

AC_REPLACE_FUNCS([strerror])


dnl
dnl integer types
dnl

AC_CHECK_HEADERS([inttypes.h],
                 [have_inttypes_h=yes],
                 [have_inttypes_h=no])
AC_CHECK_HEADERS([sys/types.h],
                 [have_sys_types_h=yes],
                 [have_sys_types_h=no])

if test "x$have_inttypes_h" = xno && test "x$have_sys_types_h" = xyes; then
  AC_CACHE_CHECK([for uintXX_t typedefs],
                 [ac_cv_uint_t],
                 [AC_EGREP_HEADER([(^|[^a-zA-Z_0-9])uint32_t[^a-zA-Z_0-9]],
                                  [sys/types.h],
                                  [ac_cv_uint_t=yes],
                                  [ac_cv_uint_t=no])])
  if test "x$ac_cv_uint_t" = xno; then
    AC_CACHE_CHECK([for u_intXX_t typedefs],
                   [ac_cv_u_int_t],
                   [AC_EGREP_HEADER([(^|[^a-zA-Z_0-9])u_int32_t[^a-zA-Z_0-9]],
                                    [sys/types.h],
                                    [ac_cv_u_int_t=yes],
                                    [ac_cv_u_int_t=no])])
  fi
fi
if test "x$have_inttypes_h" = xyes || test "x$ac_cv_uint_t" = xyes; then
    :
elif test "x$ac_cv_u_int_t" = xyes; then
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

#ifdef __cplusplus
}
/* Get rid of a possible inline macro under C++. */
# define inline inline
#endif

#endif /* T1UTILS_CONFIG_H */])


dnl
dnl Output
dnl

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
