dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2021 Karl Berry <tex-live@tug.org>
dnl   Copyright 2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define gregorio_version
AC_INIT([Gregorio (TeX Live)], gregorio_version, [tex-k@tug.org], [gregorio--tex-live-], [http://gregorio-project.github.io/])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([gregorio-src/src/gregorio-utils.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([gregorio])


# [from gnulib] canonicalize.m4 serial 26

dnl Copyright (C) 2003-2007, 2009-2016 Free Software Foundation, Inc.

dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

# Check whether realpath works.  Assume that if a platform has both
# realpath and canonicalize_file_name, but the former is broken, then
# so is the latter.
AC_DEFUN([gl_FUNC_REALPATH_WORKS],
[
  AC_CHECK_FUNCS_ONCE([realpath])
  AC_REQUIRE([AC_CANONICAL_HOST]) dnl for cross-compiles
  AC_CACHE_CHECK([whether realpath works], [gl_cv_func_realpath_works], [
    touch conftest.a
    mkdir conftest.d
    AC_RUN_IFELSE([
      AC_LANG_PROGRAM([[
        ]GL_NOCRASH[
        #include <stdlib.h>
        #include <string.h>
      ]], [[
        int result = 0;
        {
          char *name = realpath ("conftest.a", NULL);
          if (!(name && *name == '/'))
            result |= 1;
        }
        {
          char *name = realpath ("conftest.b/../conftest.a", NULL);
          if (name != NULL)
            result |= 2;
        }
        {
          char *name = realpath ("conftest.a/", NULL);
          if (name != NULL)
            result |= 4;
        }
        {
          char *name1 = realpath (".", NULL);
          char *name2 = realpath ("conftest.d//./..", NULL);
          if (strcmp (name1, name2) != 0)
            result |= 8;
        }
        return result;
      ]])
     ],
     [gl_cv_func_realpath_works=yes],
     [gl_cv_func_realpath_works=no],
     [case "$host_os" in
                       # Guess yes on glibc systems.
        *-gnu* | gnu*) gl_cv_func_realpath_works="guessing yes" ;;
                       # If we don't know, assume the worst.
        *)             gl_cv_func_realpath_works="guessing no" ;;
      esac
     ])
    rm -rf conftest.a conftest.d
  ])
  case "$gl_cv_func_realpath_works" in
    *yes)
      AC_DEFINE([FUNC_REALPATH_WORKS], [1], [Define to 1 if realpath()
        can malloc memory, always gives an absolute path, and handles
        trailing slash correctly.])
      ;;
  esac
])
# end function from canonicalize.m4.  now call it:

gl_FUNC_REALPATH_WORKS

dnl Checks for header files.
AC_CHECK_HEADERS([stdint.h])
AC_CHECK_HEADERS([stdalign.h])

AC_CHECK_ALIGNOF(uint32_t)

AC_C_BIGENDIAN

AC_SUBST([GREGORIO_TREE], [gregorio-src])
AC_SUBST([PERL5], ['/usr/bin/env perl'])

KPSE_KPATHSEA_FLAGS

AC_CONFIG_HEADERS([config_.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
