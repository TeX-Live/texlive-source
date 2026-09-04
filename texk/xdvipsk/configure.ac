dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2026 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([xdvipsk (TeX Live)], [2026.1], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([xdvips.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

# Don't do anything if disabled.  The 32-bit Solaris for github doesn't
# have main in libstdc++ (last check here).  Have to figure out what's
# going on.
# https://productionresultssa15.blob.core.windows.net/actions-results/4ba3b577-6e5c-410c-a84a-aab41c3c0937/workflow-job-run-bfb09077-766b-50a7-85e6-c2be65408694/logs/job/job-logs.txt?rsct=text%2Fplain&se=2026-01-22T23%3A00%3A21Z&sig=f9XUBuDhUbo0QWKoo7xRXDBp98JkPYamHHCIyFSVm8g%3D&ske=2026-01-22T23%3A14%3A33Z&skoid=ca7593d4-ee42-46cd-af88-8b886a2f84eb&sks=b&skt=2026-01-22T22%3A14%3A33Z&sktid=398a6654-997b-47e9-b12b-9515b896b4de&skv=2025-11-05&sp=r&spr=https&sr=b&st=2026-01-22T22%3A50%3A16Z&sv=2025-11-05
if test "x$enable_xdvipsk" != xyes; then
  echo "$0: xdvipsk disabled, not configuring."
  exit 0
fi

# check for C++ compiler and the library compiler
AC_PROG_CXX
AC_PROG_RANLIB

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([xdvipsk])
KPSE_CANONICAL_HOST
AC_CANONICAL_HOST
AM_CONDITIONAL([cross], [test "x$cross_compiling" = xyes])
KPSE_COND_WIN32

KPSE_LUA53_DEFINES

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_CHECK_SIZEOF([int])

AC_SEARCH_LIBS([pow], [m])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

KPSE_KPATHSEA_FLAGS
KPSE_LUA53_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_FREETYPE2_FLAGS

AC_DEFUN([KPSE_LIBJPEG_OPTIONS], [_KPSE_LIB_OPTIONS([libjpeg], [$1], [pkg-config])])
AC_DEFUN([KPSE_LIBJPEG_SYSTEM_FLAGS], [dnl
AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])dnl
if $PKG_CONFIG libjpeg; then
  LIBJPEG_INCLUDES=`$PKG_CONFIG libjpeg --cflags`
  LIBJPEG_LIBS=`$PKG_CONFIG libjpeg --libs`
else
  LIBJPEG_INCLUDES=
  LIBJPEG_LIBS=-ljpeg
fi])

AC_DEFUN([KPSE_LIBTIFF_OPTIONS], [_KPSE_LIB_OPTIONS([libtiff], [$1], [pkg-config])])
AC_DEFUN([KPSE_LIBTIFF_SYSTEM_FLAGS], [dnl
AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])dnl
if $PKG_CONFIG libtiff-4; then
  LIBTIFF_INCLUDES=`$PKG_CONFIG libtiff-4 --cflags`
  LIBTIFF_LIBS=`$PKG_CONFIG libtiff-4 --libs`
else
  LIBTIFF_INCLUDES=
  LIBTIFF_LIBS=-ltiff
fi])

dnl libjpeg: use system library or build and link bundled copy (default).
KPSE_LIBJPEG_OPTIONS([with-system])
AS_IF([test "x$with_system_libjpeg" = xyes],
  [KPSE_LIBJPEG_SYSTEM_FLAGS],
  [LIBJPEG_LIBS=libjpeg.a
   LIBJPEG_INCLUDES='-I$(abs_top_srcdir)/thirdparty/libjpeg/libjpeg-src'])
AC_SUBST([LIBJPEG_LIBS])
AC_SUBST([LIBJPEG_INCLUDES])
AM_CONDITIONAL([BUNDLED_LIBJPEG], [test "x$with_system_libjpeg" != xyes])

dnl libtiff: use system library or build and link bundled copy (default).
KPSE_LIBTIFF_OPTIONS([with-system])
AS_IF([test "x$with_system_libtiff" = xyes],
  [KPSE_LIBTIFF_SYSTEM_FLAGS],
  [LIBTIFF_LIBS=libtiff.a
   LIBTIFF_INCLUDES='-I$(abs_top_srcdir)/thirdparty/libtiff/libtiff-src'])
AC_SUBST([LIBTIFF_LIBS])
AC_SUBST([LIBTIFF_INCLUDES])
AM_CONDITIONAL([BUNDLED_LIBTIFF], [test "x$with_system_libtiff" != xyes])

echo timestamp >config.force

# distribute additional compiler and linker flags
# --> set these variables instead of CXXFLAGS or LDFLAGS
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])
AC_SUBST([LIBS])

AC_CONFIG_FILES([Makefile  man/Makefile])

# The subdirectory squeeze must be configured for the build system.
KPSE_NATIVE_SUBDIRS([squeeze])

AC_HEADER_SYS_WAIT

AC_OUTPUT
