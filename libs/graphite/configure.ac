dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define silgraphite_version
AC_INIT([silgraphite for TeX Live], silgraphite_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([engine-]silgraphite_version[/src/segment/GrEngine.cpp])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([graphite], [no-define])

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

# Checks for header files.
AC_HEADER_STDC

AC_C_BIGENDIAN

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF([wchar_t])
AC_SUBST([SIZEOF_WCHAR_T])

AC_SEARCH_LIBS([fabsf], [m])
AC_CHECK_FUNCS([fabsf])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AC_SUBST([GRAPHITE_TREE], [engine-]silgraphite_version)

AC_CONFIG_FILES([Makefile include/graphite/Makefile])

AC_OUTPUT
