dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([zlib_major], [1])[]dnl
m4_define([zlib_minor], [2])[]dnl
m4_define([zlib_micro], [3])[]dnl using unmodified zlib-1.2.3 source tree
m4_define([zlib_version], zlib_major.zlib_minor.zlib_micro)[]dnl
AC_INIT([zlib for TeX Live], zlib_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([zlib-]zlib_version[/zconf.in.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

AC_CONFIG_HEADERS([config.h])[]dnl unused, to avoid zillions of -D' on command line

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$GCC" = xyes; then
  CFLAGS=${CFLAGS-"-O3"}
fi

AC_HEADER_STDC
if test "x$ac_cv_header_stdc" != xyes; then
  AC_MSG_ERROR([Sorry, can't do anything for you])
fi

AC_CHECK_HEADER([unistd.h], [WITH_UNISTD=1], [WITH_UNISTD=0])
AC_SUBST([WITH_UNISTD])

AC_CHECK_HEADER([errno.h],
                [ZLIB_DEFINES=],
                [ZLIB_DEFINES="-DNO_ERRNO_H"])

AC_MSG_CHECKING([for mmap support])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
caddr_t hello() {
  return mmap((caddr_t)0, (off_t)0, PROT_READ, MAP_SHARED, 0, (off_t)0);
}]])],
                  [AC_MSG_RESULT([yes])
                  ZLIB_DEFINES="$ZLIB_DEFINES -DUSE_MMAP"],
                  [AC_MSG_RESULT([no])])
AC_SUBST([ZLIB_DEFINES])

ZLIB_TREE=zlib-[]zlib_version
AC_SUBST([ZLIB_TREE])

if test -f $srcdir/$ZLIB_TREE/zconf.h; then
  AC_MSG_ERROR([Sorry, you must remove the file $ZLIB_TREE/zconf.h])
fi

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
