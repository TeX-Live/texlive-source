dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([libpng_major], [1])[]dnl
m4_define([libpng_minor], [2])[]dnl
m4_define([libpng_micro], [38])[]dnl using unmodified libpng source tree
m4_define([libpng_version], libpng_major.libpng_minor.libpng_micro)[]dnl
AC_INIT([libpng for TeX Live], libpng_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([libpng-]libpng_version[/pngget.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

PNGLIB_MAJOR=libpng_major()
PNGLIB_MINOR=libpng_minor()
PNGLIB_RELEASE=libpng_micro()
AC_SUBST([PNGLIB_MAJOR])
AC_SUBST([PNGLIB_MINOR])
AC_SUBST([PNGLIB_RELEASE])

AC_PROG_CC
AC_PROG_RANLIB

KPSE_COMPILER_WARNINGS

AC_CONFIG_HEADERS([config.h])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset], , [AC_ERROR([memset not found in libc])])
AC_SEARCH_LIBS([pow], [m], , [AC_ERROR([cannot find pow])])

KPSE_ZLIB_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

# Configure must not fail, or 'make dist' will fail as well; thus
# don't run any tests for '--disable-build', there may be no zlib.
if test "x$enable_build" != xno; then

KPSE_ADD_FLAGS([zlib])

AC_CHECK_FUNC([zlibVersion], , [AC_ERROR([zlib not found])])

KPSE_RESTORE_FLAGS

fi

LIBPNG_TREE=libpng-[]libpng_version
AC_SUBST([LIBPNG_TREE])

LIBPNG_DEFINES='-DPNG_CONFIGURE_LIBPNG -DPNG_NO_MMX_CODE'
AC_SUBST([LIBPNG_DEFINES])

dnl Use $LIBPNG_TREE to exclude $LIBPNG_TREE/png.h from DIST_COMMON
AC_CONFIG_LINKS([png.h:$LIBPNG_TREE/png.h pngconf.h:$LIBPNG_TREE/pngconf.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
