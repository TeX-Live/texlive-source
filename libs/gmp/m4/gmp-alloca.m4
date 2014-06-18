# Autoconf macros for the GNU MP Library.
# Copyright (C) 2000-2014 Free Software Foundation, Inc.
#
# Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
# Extracted from gmp-6.0.0/acinclude.m4 and adapted for TeX Live.
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# GMP_FUNC_ALLOCA
# ---------------
# Determine whether "alloca" is available.  This is AC_FUNC_ALLOCA from
# autoconf, but changed so it doesn't use alloca.c if alloca() isn't
# available, and also to use gmp-impl.h for the conditionals detecting
# compiler builtin alloca's.

AC_DEFUN([GMP_FUNC_ALLOCA], [dnl
AC_REQUIRE([KPSE_GMP_H])
_GMP_HEADER_ALLOCA
AC_CACHE_CHECK([for alloca (via gmp-impl.h)],
               [gmp_cv_func_alloca],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include "gmp-tmp.h"
#include "$srcdir/gmp-]gmp_version[/gmp-impl.h"]],
                                    [[char *p = (char *) alloca (1);]])],
  [gmp_cv_func_alloca=yes],
  [gmp_cv_func_alloca=no])])
if test "x$gmp_cv_func_alloca" = xyes; then
  AC_DEFINE([HAVE_ALLOCA], 1, [Define to 1 if alloca() works (via gmp-impl.h).])
  AC_DEFINE([WANT_TMP_ALLOCA])
else
  AC_DEFINE([WANT_TMP_REENTRANT])
fi
AH_VERBATIM([WANT_TMP],
[/* Define one of these to 1 for the temporary memory allocation method. */
#undef WANT_TMP_ALLOCA
#undef WANT_TMP_REENTRANT])
]) # GMP_FUNC_ALLOCA

# _GMP_HEADER_ALLOCA
# ------------------
# Internal subroutine
m4_define([_GMP_HEADER_ALLOCA],
[# The Ultrix 4.2 mips builtin alloca declared by alloca.h only works
# for constant arguments.  Useless!
AC_CACHE_CHECK([for working alloca.h],
               [gmp_cv_header_alloca],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <alloca.h>]],
                                    [[char *p = (char *) alloca (2 * sizeof (int));]])],
  [gmp_cv_header_alloca=yes],
  [gmp_cv_header_alloca=no])])
if test "x$gmp_cv_header_alloca" = xyes; then
  AC_DEFINE([HAVE_ALLOCA_H], 1,
  [Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).])
fi
]) # _GMP_HEADER_ALLOCA
