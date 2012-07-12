dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2011-2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define poppler_version
AC_INIT([poppler for TeX Live], poppler_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([poppler-]poppler_version[/poppler/Array.cc])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign no-define])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_COMPILER_WARNINGS

test "x$GXX" = xyes && NO_WARN_CXXFLAGS='-Wno-write-strings'
AC_SUBST([NO_WARN_CXXFLAGS])  

dnl ##### Optional features.
AC_ARG_ENABLE([opi],
              AS_HELP_STRING([--enable-opi],
                             [include support for OPI comments]),
              [if test "x$enableval" = xyes; then
  AC_DEFINE([OPI_SUPPORT], 1, [Generate OPI comments in PS output.])
fi
])

AC_ARG_ENABLE([multithreaded],
              AS_HELP_STRING([--enable-multithreaded],
                             [include support for multithreading]),
              [if test "x$enableval" = xyes; then
  AC_DEFINE([MULTITHREADED], 1, [Enable multithreading support.])
fi
])

AC_ARG_ENABLE([exceptions],
              AS_HELP_STRING([--enable-exceptions],
                             [use C++ exceptions]),
              [if test "x$enableval" = xyes; then
  AC_DEFINE([USE_EXCEPTIONS], 1, [Throw exceptions to deal with not enough memory and similar problems])
fi
])

AC_ARG_ENABLE([fixedpoint],
              AS_HELP_STRING([--enable-fixedpoint],
                             [use fixed point (instead of floating point) arithmetic]),
              [if test "x$enableval" = xyes; then
  AC_DEFINE([USE_FIXEDPOINT], 1, [Use fixed point arithmetic])
fi
])

dnl **** Checks for header files ****

AC_CHECK_HEADERS([strings.h])
AC_HEADER_DIRENT

dnl **** Checks for library functions ****

AC_CHECK_FUNCS([gettimeofday gmtime_r popen mkstemp])
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_HEADERS([poppler/poppler-config.h:poppler-]poppler_version[/poppler/poppler-config.h.in])

dnl ##### Export the version
AC_DEFINE_UNQUOTED([POPPLER_VERSION], ["poppler_version"], [Define to the poppler version.])

KPSE_ZLIB_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([inflate], , [AC_ERROR([zlib not found])])
AC_CHECK_HEADERS([zlib.h], , [AC_ERROR([zlib not found])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_DEFINE([ENABLE_ZLIB], 1, [Use zlib instead of builtin zlib decoder.])

AC_SUBST([POPPLER_TREE], [poppler-]poppler_version)

AC_CONFIG_FILES([Makefile poppler/Makefile goo/Makefile fofi/Makefile])

AC_OUTPUT
