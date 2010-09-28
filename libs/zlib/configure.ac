dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([zlib_version], [1.2.5])[]dnl using unmodified zlib source tree
AC_INIT([zlib for TeX Live], zlib_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([zlib-]zlib_version[/zlib.h])
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

AC_CHECK_HEADER([errno.h],
                [ZLIB_DEFINES=],
                [ZLIB_DEFINES="-DNO_ERRNO_H"])

AC_CHECK_HEADERS([unistd.h])
dnl zlib-1.2.3 has zconf.in.h, whereas zlib-1.2.5 has zconf.h.in
m4_if(zlib_version, [1.2.3],
      [
ZCONF_SRC=zlib-[]zlib_version/zconf.in.h
if test x"$ac_cv_header_unistd_h" = xyes; then
  zconf_sh='sed <$srcdir/'"$ZCONF_SRC"' "/HAVE_UNISTD_H/s%0%1%" >'
else
  zconf_sh='cp $srcdir/'"$ZCONF_SRC"
fi
AC_SUBST([OTHER_TREE], [zlib-1.2.5])],
      [
ZCONF_SRC=zlib-[]zlib_version/zconf.h.in
if test x"$ac_cv_header_unistd_h" = xyes; then
  zconf_sh='sed <$srcdir/'"$ZCONF_SRC"' "/^#ifdef HAVE_UNISTD_H.* may be/s/def HAVE_UNISTD_H\\(.*\\) may be/ 1\\1 was/" >'
else
  zconf_sh='cp $srcdir/'"$ZCONF_SRC"
fi

AC_SYS_LARGEFILE
AC_FUNC_FSEEKO
dnl add to ZLIB_DEFINES
if test "$enable_largefile" != no; then
  AS_CASE([$ac_cv_sys_file_offset_bits],
          [no], [],
          [unknown], [AS_CASE([$ac_cv_sys_large_files],
                              [no | unknown], [],
                              [ZLIB_DEFINES="$ZLIB_DEFINES -D_LARGE_FILES=1"])],
          [ZLIB_DEFINES="$ZLIB_DEFINES -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE=1"])
fi
AS_CASE([$ac_cv_sys_largefile_source],
        [no | unknown], [],
        [ZLIB_DEFINES="$ZLIB_DEFINES -D_LARGEFILE_SOURCE=1"])

AC_SUBST([OTHER_TREE], [zlib-1.2.3])])
AM_CONDITIONAL([ZLIB123], [test "x]zlib_version[" = x1.2.3])
AC_CONFIG_COMMANDS([zconf.h],
                   [eval "$zconf_sh" zconf.h],
                   [zconf_sh='$zconf_sh'])

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
