# Autoconf macros for xdvik.
# Copyright (C) 2002 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_FUNC_MEMICMP
# -----------------
# Check for memicmp(), which some installations have in <string.h>.
AC_DEFUN([XDVI_FUNC_MEMICMP],
[AC_CACHE_CHECK([for memicmp],
                [xdvi_cv_memicmp],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <string.h>]],
                                                 [[(void)memicmp((char *)NULL, (char *)NULL, 0);]])],
                                [xdvi_cv_memicmp=yes],
                                [xdvi_cv_memicmp=no])])
if test "x$xdvi_cv_memicmp" = xyes; then
  AC_DEFINE([HAVE_MEMICMP], 1, [Define if the memicmp() function is in <string.h>])
fi
]) # XDVI_FUNC_MEMICMP
