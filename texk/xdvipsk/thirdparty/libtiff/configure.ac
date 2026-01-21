dnl Process this file with autoconf§ to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define libtiff_version
AC_INIT([libtiff], libtiff_version)
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([libtiff-src/tiff.h])
AC_CONFIG_AUX_DIR([../../../../build-aux])
AC_CONFIG_MACRO_DIR([../../../../m4])

KPSE_BASIC([libtiff])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

# Generate configuration headers.
# AC_CONFIG_FILES([libtiff-src/jconfig.h:libtiff-src/jconfig.cfg])
AC_CONFIG_FILES([libtiff-src/tif_config.h:libtiff-src/tiffconf.h])
AC_CONFIG_HEADERS([config.h])

KPSE_COMPILER_VISIBILITY

KPSE_LIBJPEG_FLAGS
KPSE_ZLIB_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

echo timestamp >config.force

fi

echo timestamp >config.force

if test "x$host" = x; then
  config_args=
else
  config_args="--host=$host"
  if test "x$build" != x; then
    config_args="$config_args --build=$build"
  fi
fi
AC_SUBST([config_args])

# Select memory manager depending on user input.
# If no "-enable-maxmem", use jmemnobs
MEMORYMGR='jmemnobs'
AC_SUBST([MEMORYMGR])

AC_SUBST([LIBTIFF_TREE], [libtiff-src])

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
