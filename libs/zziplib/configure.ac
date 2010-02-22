dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   based on zziplib-0.13.58:
dnl     Copyright (C) 2009 Guido Draheim <guido@gmx.de> for ZZipLib.SF.net
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([zziplib_version], [0.13.58])[]dnl using unmodified zziplib source tree
AC_INIT([zziplib for TeX Live], zziplib_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([zziplib-]zziplib_version[/zziplib.spec])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_COMPILER_WARNINGS

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_CHECK_HEADERS([stdint.h unistd.h windows.h sys/int_types.h])
AC_CHECK_HEADERS([winnt.h winbase.h], , , [#include <windows.h>])
AC_CHECK_HEADERS([sys/types.h sys/mman.h sys/stat.h sys/param.h]) dnl posix'ish
AC_CHECK_HEADERS([io.h direct.h byteswap.h])
AC_CHECK_HEADERS([fnmatch.h])

# ---------------------------------------------------------------
AC_MSG_CHECKING([whether can use mmap for file scanning])
enable_mmap='no (just a bit slower)'
test "x$ac_cv_header_winbase_h" = xyes && enable_mmap="yes, winbase.h"
test "x$ac_cv_header_sys_mman_h" = xyes && enable_mmap="yes, sys/mman.h"
case "$enable_mmap" in
   yes*) ZZIPLIB_DEFINES='-D_USE_MMAP';;
esac
AC_MSG_RESULT([$enable_mmap])
AC_SUBST([ZZIPLIB_DEFINES])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_CONST
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_CHECK_TYPE([ssize_t], [int])
AC_CHECK_TYPE([off64_t], [_zzip_off_t])
AC_CHECK_TYPE([__int64], [long long])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_C_BIGENDIAN
ZZIPLIB_CHECK_ALIGNED_ACCESS

# Checks for library functions.
AC_CHECK_FUNCS([strcasecmp strndup])

KPSE_ZLIB_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([zlib])
AC_CHECK_HEADERS([zlib.h])
AC_CHECK_FUNC([zlibVersion], , [AC_ERROR([zlib not found])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

ZZIPLIB_TREE=zziplib-[]zziplib_version
AC_SUBST([ZZIPLIB_TREE])

# -----------------------------------------------------------------------
AC_MSG_RESULT([creating include/zzip/_msvc.in via zzip/_msvc.sed of config.h.in])
test -d include || mkdir include
sed -f $srcdir/$ZZIPLIB_TREE/zzip/_msvc.sed $srcdir/config.h.in >include/_msvc.in
AC_CONFIG_HEADERS([config.h])
AX_PREFIX_CONFIG_H([include/zzip/_config.h],[zzip],[config.h])
AX_PREFIX_CONFIG_H([include/zzip/_msvc.h],[zzip],[include/_msvc.in])

AC_CONFIG_FILES([Makefile include/zzip/Makefile])

AC_OUTPUT
