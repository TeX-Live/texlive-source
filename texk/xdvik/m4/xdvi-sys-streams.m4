# Autoconf macros for xdvik.
# Copyright (C) 1999 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_SYS_STREAMS
# ----------------
# Check for at-least-pretend Streams capability.
AC_DEFUN([XDVI_SYS_STREAMS],
[AC_CACHE_CHECK([for stropts.h and isastream()],
                [xdvi_cv_sys_streams],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stropts.h>]],
                                                 [[#ifndef I_SETSIG
choke me
#else
isastream(0);
#endif]])],
                                [xdvi_cv_sys_streams=yes],
                                [xdvi_cv_sys_streams=no])])
if test "x$xdvi_cv_sys_streams" = xyes; then
  AC_DEFINE([HAVE_STREAMS], 1, [Define if your system has STREAMS (and if X uses it).])
fi
]) # XDVI_SYS_STREAMS
