# Autoconf macros for xdvik.
# Copyright (C) 1999 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_CC_CONCAT
# --------------
# Check whether the C compiler does string concatenation.
AC_DEFUN([XDVI_CC_CONCAT],
[AC_CACHE_CHECK([whether C compiler supports string concatenation],
                [xdvi_cv_cc_concat],
                [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>]],
                                                    [[puts("Testing" " string" " concatenation");]])],
                                   [xdvi_cv_cc_concat=yes],
                                   [xdvi_cv_cc_concat=no])])
if test "x$xdvi_cv_cc_concat" = xyes; then
  AC_DEFINE([HAVE_CC_CONCAT], 1, [Define if your C compiler can do string concatenation])
fi
]) # XDVI_CC_CONCAT
