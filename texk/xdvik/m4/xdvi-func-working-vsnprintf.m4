# Autoconf macros for xdvik.
# Copyright (C) 2002 Paul Vojta <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2002 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_FUNC_WORKING_VSNPRINTF
# ---------------------------
# Check for a working implementation of (v)snprintf()
# which should either return a negative result or the size actually needed
# (latter is C99 standard).
AC_DEFUN([XDVI_FUNC_WORKING_VSNPRINTF],
[AC_CACHE_CHECK([for a working implementation of (v)snprintf()],
                [xdvi_cv_func_good_vsnprintf],
                [AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#ifdef IRIX
#define _XOPEN_SOURCE 500
#endif
#include <stdio.h>]],
                                                [[char s[2];
  int retval = snprintf(s, 2, "test");
  if (retval > -1 && retval != 4) /* neither negative nor desired size, not OK */
    return 1;]])],
                               [xdvi_cv_func_good_vsnprintf=yes],
                               [xdvi_cv_func_good_vsnprintf=no],
                               [xdvi_cv_func_good_vsnprintf=no # safe value for cross-compiling])])
if test "x$xdvi_cv_func_good_vsnprintf" = xyes; then
  AC_DEFINE([HAVE_GOOD_VSNPRINTF], 1, [Define if the vsnprintf function works.])
fi
]) # XDVI_FUNC_WORKING_VSNPRINTF
