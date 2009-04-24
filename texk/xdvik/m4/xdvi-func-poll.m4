# Autoconf macros for xdvik.
# Copyright (C) 1999 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_FUNC_POLL
# --------------
# Check for poll().
AC_DEFUN([XDVI_FUNC_POLL],
[AC_CACHE_CHECK([for poll.h and poll()],
                [xdvi_cv_func_poll],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <poll.h>]],
                                                 [[poll((struct pollfd *) 0, 0, 0);]])],
                                [xdvi_cv_func_poll=yes],
                                [xdvi_cv_func_poll=no])])
if test "x$xdvi_cv_func_poll" = xyes; then
  AC_DEFINE([HAVE_POLL], 1, [Define if your system has <poll.h> and poll().])
else
  AC_CHECK_HEADERS([sys/select.h select.h])
fi
]) # XDVI_FUNC_POLL
