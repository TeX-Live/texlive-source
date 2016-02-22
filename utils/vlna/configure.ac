dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([vlna_version], [1.5])[]dnl using unmodified vlna source tree
AC_INIT([vlna (TeX Live)], vlna_version, [tex-live@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([vlna-src/vlna.w])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([vlna], [no-define])

# Checks for programs.
AC_PROG_CC

# With --enable-maintainer-mode we may want to regenerate
# vlna.c, vlna.tex, and vlna.dvi, this requires ctangle, cweave, and csplain.
AC_CHECK_PROGS([CTANGLE], [ctangle], [none])
AC_CHECK_PROGS([CWEAVE], [cweave], [none])
AC_CHECK_PROGS([CSPLAIN], [csplain], [none])

do_cweb=$enable_maintainer_mode
if test x"$do_cweb" = xyes; then
  case :$CTANGLE:$CWEAVE:$CSPLAIN: in
  *:none:*)
    AC_MSG_WARN([Can not regenerate vlna.c, vlna.tex, and vlna.dvi -- requires ctangle, cweave, and csplain.])
    do_cweb=no
  ;;
  esac
fi
AM_CONDITIONAL([do_cweb], [test "x$do_cweb" = xyes])

AC_SUBST([VLNA_TREE], [vlna-src])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
