# Autoconf macros for xdvik.
# Copyright (C) 2001 - 2009 Paul Vojta <xdvi-core@lists.sourceforge.net>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_SYS_OLD_LINUX
# ------------------
# Check for certain broken versions of Linux.
AC_DEFUN([XDVI_SYS_OLD_LINUX],
[AC_CACHE_CHECK([for certain old versions of Linux],
                [xdvi_cv_sys_old_linux],
                [AS_CASE(["`(uname -sr) 2>/dev/null`"],
                         [["Linux 2."[01].* | "Linux 2.2."[0-8] | "Linux 2.2."[0-8]-*]],
                         [xdvi_cv_sys_old_linux=yes],
                         [xdvi_cv_sys_old_linux=no])])
if test "x$xdvi_cv_sys_old_linux" = xyes; then
  AC_DEFINE([FLAKY_SIGPOLL], 1,
            [Define if you are using Linux 2.1.xxx -- 2.2.8, or if you find it necessary.])
fi
]) # XDVI_SYS_OLD_LINUX
