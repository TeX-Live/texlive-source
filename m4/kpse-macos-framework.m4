# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2005 - 2008 Jonathan Kew <...@...>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_CHECK_FRAMEWORK(FRAMEWORK, BODY)
# -------------------------------------
# Check for mthe Mac OS X framework FRAMEWORK (using BODY) and if found,
# set kpse_cv_have_FRAMEWORK to yes and define HAVE_FRAMEWORK.
AC_DEFUN([KPSE_CHECK_FRAMEWORK],
[AC_CACHE_CHECK([for Mac OS X $1 framework],
                [kpse_cv_have_$1],
                [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <$1/$1.h>]],
                                                    [[$2]])],
                                   [kpse_cv_have_$1=yes],
                                   [kpse_cv_have_$1=no])])
AS_IF([test "x$kpse_cv_have_$1" = xyes],
      [AC_DEFINE([HAVE_]AS_TR_CPP([$1]), 1,
                 [Define to 1 if you have the Mac OS X $1 framework.])])
]) # KPSE_CHECK_FRAMEWORK
