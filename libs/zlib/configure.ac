dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2016 Karl Berry <tex-live@tug.org>
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define zlib_version
AC_INIT([zlib (TeX Live)], zlib_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([zlib-src/zlib.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([zlib])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_COMPILER_VISIBILITY

AC_CONFIG_HEADERS([config.h])[]dnl unused, to avoid zillions of -D's on command line

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$GCC" = xyes; then
  CFLAGS=${CFLAGS-"-O3"}
fi

AC_HEADER_STDC
if test "x$ac_cv_header_stdc" != xyes; then
  AC_MSG_ERROR([Sorry, can't do anything for you without header_stdc])
fi

AC_CHECK_HEADER([errno.h],
                [ZLIB_DEFINES=],
                [ZLIB_DEFINES="-DNO_ERRNO_H"])

AC_CHECK_HEADERS([unistd.h])
ZCONF_SRC=[zlib-src/zconf.h.in]
if test x"$ac_cv_header_unistd_h" = xyes; then
  zconf_sh='sed <$srcdir/'"$ZCONF_SRC"' "/^#ifdef HAVE_UNISTD_H.* may be/s/def HAVE_UNISTD_H\\(.*\\) may be/ 1\\1 was/" >'
else
  zconf_sh='cp $srcdir/'"$ZCONF_SRC"
fi

KPSE_LARGEFILE([ZLIB_DEFINES], [_LARGEFILE64_SOURCE])

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

KPSE_CHECK_WIN32
if test "x$kpse_cv_have_win32" != xno; then
  ZLIB_DEFINES="$ZLIB_DEFINES -DNO_VIZ"
fi

AC_SUBST([ZLIB_DEFINES])

AC_SUBST([ZLIB_TREE], [zlib-src])

if test -f $srcdir/$ZLIB_TREE/zconf.h; then
  AC_MSG_ERROR([Sorry, you must remove the file $ZLIB_TREE/zconf.h])
fi

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
