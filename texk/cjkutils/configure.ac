dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define cjk_version
AC_INIT([CJK utils (TeX Live)], cjk_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([cjkutils-src/hbf2gf/hbf.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([cjkutils], [no-define])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

dnl get Compiler flags right.
XX_CFLAGS=$WARNING_CFLAGS
AC_SUBST([XX_CFLAGS])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TM

dnl Checks for library functions.
AC_FUNC_VPRINTF

dnl we need this for hbf.c
if test "x$ac_cv_func_strdup" = xno; then
  XX_CPPFLAGS=-DNO_STRDUP
fi
AC_SUBST([XX_CPPFLAGS])

KPSE_KPATHSEA_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_HEADER([kpathsea/version.h], [],
                [AC_MSG_ERROR([Sorry, kpathsea too old])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

dnl Rebuilding hbf2gf.c, hbf2gf.tex, and hbf2gf.tex requires ctangle, cweave, and tex
do_cweb=$enable_maintainer_mode
if test x"$do_cweb" = xyes; then
  AC_CHECK_PROGS([CTANGLE], [ctangle], [none])
  AC_CHECK_PROGS([CWEAVE], [cweave], [none])
  AC_CHECK_PROGS([TEX], [tex], [none])
  case :$CTANGLE:$CWEAVE:$TEX: in
  *:none:*)
    AC_MSG_WARN([Will not run cweb -- requires ctangle, cweave, and tex.])
    do_cweb=no
  ;;
  esac
fi
AM_CONDITIONAL([do_cweb], [test "x$do_cweb" = xyes])

KPSE_COND_WIN32

AC_SUBST([CJKUTILS_TREE], [cjkutils-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
