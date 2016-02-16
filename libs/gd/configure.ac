dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define gd_version
AC_INIT([gd (TeX Live)], gd_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([libgd-src/src/gd.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([gd])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

AC_CONFIG_HEADERS([config.h])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h limits.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_SEARCH_LIBS([sin], [m], , [AC_ERROR([cannot find sin])])

KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_FREETYPE2_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([deflate], , [AC_ERROR([zlib not found])])
AC_CHECK_HEADERS([zlib.h], , [AC_ERROR([zlib not found])])

KPSE_ADD_FLAGS([libpng])
AC_CHECK_FUNC([png_create_read_struct], , [AC_ERROR([libpng not found])])
AC_CHECK_HEADERS([png.h], , [AC_ERROR([libpng not found])])

KPSE_ADD_FLAGS([freetype2])
AC_CHECK_FUNC([FT_Init_FreeType], , [AC_ERROR([libfreetype2 not found])])
AC_CHECK_HEADERS([ft2build.h], , [AC_ERROR([libfreetype2 not found])])

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_DEFINE([HAVE_LIBZ], 1, [Define if you have zlib.])
AC_DEFINE([HAVE_ZLIB_H], 1)
AC_DEFINE([HAVE_LIBPNG], 1, [Define if you have the png library.])
AC_DEFINE([HAVE_PNG_H], 1)
AC_DEFINE([HAVE_LIBFREETYPE], 1, [ Define if you have the freetype library.])
AC_DEFINE([HAVE_FT2BUILD_H], 1)

AC_SUBST([GD_TREE], [libgd-src])

KPSE_COND_WIN32

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
