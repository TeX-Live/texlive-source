dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2011 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define t1lib_version
AC_INIT([T1lib for TeX Live], t1lib_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([t1lib-]t1lib_version[/lib/t1lib/t1base.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign no-define])
AM_MAINTAINER_MODE

KPSE_LIB_VERSION([t1lib])[]dnl
AC_SUBST([T1LIB_VERSION], [t1lib_major])[]dnl
AC_SUBST([T1LIB_REVISION], [t1lib_minor])[]dnl
AC_SUBST([T1LIB_PATCHLEVEL], [t1lib_micro])[]dnl
AC_SUBST([T1LIB_VERSIONSTRING], '"t1lib_version"')

AC_CONFIG_HEADERS([lib/t1lib/sysconf.h])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

dnl **** Check for some programs and libraries ****

AC_PROG_CC
AC_PROG_RANLIB

KPSE_COMPILER_WARNINGS

AC_SEARCH_LIBS([sqrt], [m])

dnl **** Check for some compiler/system characteristics

AC_C_CHAR_UNSIGNED
AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([float])
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([long double])
AC_CHECK_SIZEOF([void *])

dnl **** Check for gcc strength-reduce bug ****

T1LIB_CFLAGS=
if test "x$GCC" = xyes; then
  AC_CACHE_CHECK([for gcc strength-reduce bug],
                 [ac_cv_c_gcc_strength_bug],
                 [AC_RUN_IFELSE([AC_LANG_PROGRAM([[]],
                                                 [[static int Array[3];
                                                   unsigned int B = 3;
                                                   int i;
                                                   for(i=0; i<B; i++) Array[i] = i - 3;
                                                   exit( Array[1] != -2 );]])],
                                [ac_cv_c_gcc_strength_bug=no],
                                [ac_cv_c_gcc_strength_bug=yes],
                                [ac_cv_c_gcc_strength_bug=yes])])
  if test "x$ac_cv_c_gcc_strength_bug" = xyes; then
    T1LIB_CFLAGS="-fno-strength-reduce"
  fi
fi

dnl **** We build t1lib without X support

T1LIB_DEFINES="-DT1LIB_NO_X11_SUPPORT"

dnl **** Check which ANSI integer type is 16 bit
if test "x$ac_cv_sizeof_short" = x2; then
  T1_INT16=short
elif test "x$ac_cv_sizeof_int" = x2; then
  T1_INT16=int
else
  AC_MSG_ERROR([Sorry, no usable 16 bit integer type])
fi
AC_SUBST([T1_INT16])
T1LIB_DEFINES="$T1LIB_DEFINES -DT1_AA_TYPE16=$T1_INT16"

dnl **** Check which ANSI integer type is 32 bit
if test "x$ac_cv_sizeof_int" = x4; then
  T1_INT32=int
elif test "x$ac_cv_sizeof_long" = x4; then
  T1_INT32=long
else
  AC_MSG_ERROR([Sorry, no usable 32 bit integer type])
fi
AC_SUBST([T1_INT32])
T1LIB_DEFINES="$T1LIB_DEFINES -DT1_AA_TYPE32=$T1_INT32"

dnl **** Check which ANSI integer type is 64 bit
if test "x$ac_cv_sizeof_long" = x8; then
  T1_INT64=long
elif test "x$ac_cv_sizeof_long_long" = x8; then
  T1_INT64="'long long'"
else
  T1_INT64=
fi
test -z "x$T1_INT64" || \
  T1LIB_DEFINES="$T1LIB_DEFINES -DT1_AA_TYPE64=$T1_INT64"

T1LIB_DATA_DIR="${datadir}/t1lib"
AC_SUBST([T1LIB_DATA_DIR])

dnl **** Check for functions and header files ****

AC_C_CONST()
AC_TYPE_SIZE_T()

AC_SUBST([T1LIB_DEFINES])
AC_SUBST([T1LIB_CFLAGS])

T1LIB_TREE=t1lib-[]t1lib_version
AC_SUBST([T1LIB_TREE])

if test -f $srcdir/$T1LIB_TREE/lib/t1lib/t1lib.h; then
  AC_MSG_ERROR([Sorry, you must remove the file $T1LIB_TREE/lib/t1lib/t1lib.h])
fi

dnl Use $T1LIB_TREE to exclude $T1LIB_TREE/lib/t1lib/t1lib.h.in from DIST_COMMON
AC_CONFIG_FILES([lib/t1lib/t1lib.h:$T1LIB_TREE/lib/t1lib/t1lib.h.in])
AC_CONFIG_LINKS([t1lib.h:lib/t1lib/t1lib.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
